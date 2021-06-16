#!${PYTHON_SHEBANG}
# -*- coding: utf-8 -*-

## Program:   PypeS
## Module:    $RCSfile: pypebatch.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:47:30 $
## Version:   $Revision: 1.4 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.


from __future__ import print_function, absolute_import # NEED TO STAY AS TOP IMPORT
import sys
import os
import os.path
from vmtk import pypes
import importlib
from inspect import isclass

class pypeBatch(object):

    def __init__(self):
        self.Arguments = []

        self.Directory = '.'
        self.ScriptName = ''
        self.ScriptArguments = []

    def GetUsageString(self):
        usageString = 'Usage: pypebatch -d directory scriptName scriptArguments ["pythonStringMethods"]'
        return usageString

    def PrintLog(self,logMessage,indent=0):
        indentUnit = '    '
        indentation = ''
        for i in range(indent):
            indentation = indentation + indentUnit
        print(indentation + logMessage)

    def PrintError(self,logMessage):
        print(logMessage)

    def ParseArguments(self):
        state = 0
        for arg in self.Arguments:
            if (arg == '--help'):
                self.PrintLog(self.GetUsageString())
                sys.exit()
            elif (arg == '-d'):
                state = 1
            elif (state == 1):
                self.Directory = arg
                state = 2
            elif (state == 0):
                state == 2
            elif (state == 2):
                if (self.ScriptName == ''):
                    self.ScriptName = arg
                self.ScriptArguments.append(arg)

    def ReplaceFileNamesInScriptArguments(self,fileName):
        actualScriptArguments = []
        for arg in self.ScriptArguments:
            if not ((arg[0]=='[') & (arg[-1]==']')):
                actualScriptArguments.append(arg)
                continue
            pattern = arg[1:-1]
            actualArgument = self.Directory + '/'
            if (pattern != ''):
                actualArgument += fileName.__getattribute__(pattern)
            else:
                actualArgument += fileName
            actualScriptArguments.append(actualArgument)
        return actualScriptArguments

    def Execute(self):
        self.PrintLog('')
        moduleName = self.ScriptName
        self.PrintLog('module name: '+ moduleName)
        try:
            module = importlib.import_module('vmtk.vmtkscripts.'+moduleName)
            # Find the principle class to instantiate the requested action defined inside the requested writerModule script.
            # Returns a single member list (containing the principle class name) which satisfies the following criteria:
            #   1) is a class defined within the script
            #   2) the class is a subclass of pypes.pypescript
            scriptObjectClasses = [x for x in dir(module) if isclass(getattr(module, x)) and issubclass(getattr(module, x), pypes.pypeScript)]
            try:
                assert len(scriptObjectClasses) == 1
                scriptObjectClassName = scriptObjectClasses[0]
            except AssertionError:
                scriptObjectClassName = ''
        except ImportError as e:
            self.PrintError(str(e))

        fileNames = os.listdir(self.Directory)

        for fileName in fileNames:
            scriptObject = getattr(module, scriptObjectClassName)
            scriptObject = scriptObject()
            completeFileName = os.path.normpath(self.Directory + '/' + fileName)
            self.PrintLog('Replacing FileNames in ' + scriptObject.ScriptName + ' arguments')
            actualScriptArguments = self.ReplaceFileNamesInScriptArguments(fileName)
            scriptObject.Arguments = actualScriptArguments
            scriptObject.ParseArguments()
            scriptString = ''
            for arg in actualScriptArguments:
                scriptString += arg + ' '
            self.PrintLog('Executing ' + scriptString)
            scriptObject.Execute()

if __name__=='__main__':
    batch = pypeBatch()
    batch.Arguments = sys.argv
    batch.ParseArguments()
    batch.Execute()
