#!/usr/bin/env python

## Program:   PypeServer
## Language:  Python

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.


from vmtk import pypes
import time

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
    pipe.Arguments = arguments
    pipe.ParseArguments()
    try: 
        pipe.Execute() 
    except BaseException, e:
        print e
    del pipe

def PypeServer(queue, output, returnIfEmptyQueue=False):
    outputStream = None
    if output != None:
        outputStream = OutputStream(output)
    while True:
        try:
            if queue:
                arguments = queue.pop(0)
                RunPypeProcess(arguments,outputStream=outputStream)
            elif returnIfEmptyQueue:
                return
            else:
                time.sleep(0.5)
        except IOError, e:
            print "Connection closed"
            break
        except KeyboardInterrupt, e:
            print "Connection closed"
            break

