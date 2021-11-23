#!/usr/bin/env python

## Program:   PypeServer
## Language:  Python

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.


from __future__ import print_function, absolute_import # NEED TO STAY AS TOP IMPORT
from vmtk import pypes
import vtk
import time
import traceback


class OutputStream(object):

    def __init__(self,textList):
        self.textList = textList

    def write(self, text):
        self.textList.append(text)

    def flush(self):
        pass


def RunPypeProcess(arguments, inputStream=None, outputStream=None, logOn=True):
    pipe = pypes.Pype()
    pipe.ExitOnError = 0
    if inputStream:
        pipe.InputStream = inputStream
    if outputStream:
        pipe.OutputStream = outputStream
    pipe.LogOn = logOn
    pipe.LogOn = True
    if type(arguments) == str:
        pipe.SetArgumentsString(arguments)
    else:
        pipe.Arguments = arguments
    try:
        pipe.ParseArguments()
        pipe.Execute()
    except BaseException as e:
        print(traceback.format_exc())
    del pipe


def PypeServer(queue, output, error, returnIfEmptyQueue=False):

    def MessageCallback(o, e, m):
        if not error:
            return
        error.append(m)

    MessageCallback.CallDataType = 'string0'
    vtk.vtkOutputWindow.GetInstance().AddObserver('ErrorEvent',MessageCallback)
    vtk.vtkOutputWindow.GetInstance().AddObserver('WarningEvent',MessageCallback)

    outputStream = None
    if output != None:
        outputStream = OutputStream(output)
    if error != None:
        errorStream = OutputStream(error)
    ranOnce = False
    while True:
        try:
            if queue:
                arguments = queue.pop(0)
                RunPypeProcess(arguments,outputStream=outputStream)
                ranOnce = True
            elif returnIfEmptyQueue:
                if ranOnce:
                    return
            else:
                time.sleep(0.5)
        except IOError as e:
            print("Connection closed")
            break
        except KeyboardInterrupt as e:
            print("Connection closed")
            break
