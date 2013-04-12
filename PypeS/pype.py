#!/usr/bin/env python

## Program:   PypeS
## Module:    $RCSfile: pype.py,v $
## Language:  Python
## Date:      $Date: 2006/07/07 10:45:42 $
## Version:   $Revision: 1.18 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.


import sys
import os.path 

pype = 'Pype'

class NullOutputStream(object):
  
    def __init__(self):
        pass

    def write(self,text):
        pass

    def flush(self):
        pass


class Pype(object):

    def __init__(self):
        self.ScriptObjectList = []
        self.ScriptList = []
        self.AutoPipe = 1
        self.LogOn = 1
        self.ScriptName = 'pype'
        self.ExitOnError = 1
        self.InputStream = sys.stdin
        self.OutputStream = sys.stdout
        self.Arguments = None

    def GetUsageString(self):
        usageString = 'Usage: pype --nolog --noauto --query firstScriptName -scriptOptionName scriptOptionValue --pipe secondScriptName -scriptOptionName scriptOptionValue -scriptOptionName @firstScriptName.scriptOptionName -id 2 --pipe thirdScriptName -scriptOptionName @secondScriptName-2.scriptOptionName'
        return usageString

    def SetOutputStreamToNull(self):
        self.OutputStream = NullOutputStream()
        
    def PrintLog(self,logMessage,indent=0):
        if not self.LogOn:
            return
        indentUnit = '    '
        indentation = ''
        for i in range(indent):
            indentation = indentation + indentUnit
        self.OutputStream.write(indentation + logMessage + '\n')
        
    def PrintError(self,logMessage):
        self.OutputStream.write(logMessage + '\n')

    def SetArgumentsString(self,argumentsString):
        if '"' not in argumentsString:
            self.Arguments = argumentsString.split()
        import re
        quoteRe = re.compile('(\".*?\")')
        splitArguments = quoteRe.split(argumentsString)
        arguments = []
        for splitArgument in splitArguments:
            arg = splitArgument
            if splitArgument.startswith('"') and splitArgument.endswith('"'):
                arg = splitArgument[1:-1]
                arguments.append(arg)
            else:
                arguments.extend(arg.strip().split())
            if '"' in arg:
                self.PrintError('Error: non-matching quote found')
        self.Arguments = arguments

    def ParseArguments(self):
        self.ScriptList = []
        arguments = self.Arguments[:]
        if os.path.basename(arguments[0]).strip() in ['pyperun']:
            arguments.remove(arguments[0])
            pypeArguments = []
            for arg in arguments:
                if arg[:2] == '--':
                    pypeArguments.append(arg)
                else:
                    break
            if '--help' in pypeArguments:
                self.PrintLog(self.GetUsageString())
                return
            if '--noauto' in pypeArguments:
                self.AutoPipe = 0
            if '--nolog' in pypeArguments:
                self.LogOn = 0
            if '--query' in pypeArguments:
                queryScripts = arguments[arguments.index('--query')+1:]
                for queryScript in queryScripts:
                    exec('import '+queryScript)
                    exec('allScripts = '+queryScript+'.__all__')
                    self.PrintLog('\n'.join(allScripts))
                return
            for arg in pypeArguments:
                arguments.remove(arg)
        if 'FILE' in arguments:
            text = ''
            self.OutputStream.write('\nThe current pype contains filename placeholders identified by the string FILE.')
            self.OutputStream.write('\nEvery occcurence of FILE is meant to be replaced with an actual file path.\n')
            self.OutputStream.write('\nPress \'c\' to continue and be authomatically prompted for the required filenames.')
            self.OutputStream.write('\nPress \'e\' to exit. You can still edit your pype manually and run it back.\n\n')
            while text not in ['c','e']:
                self.OutputStream.write('> ')
                text = self.InputStream.readline().rstrip('\n')
            if text == 'e':
                return
            elif text =='c':
                while 'FILE' in arguments:
                    arguments[arguments.index('FILE')] = 'BROWSER'
        while '--pipe' in arguments:
            scriptSlice = arguments[:arguments.index('--pipe')]
            self.ScriptList.append([os.path.splitext(os.path.split(scriptSlice[0])[1])[0],scriptSlice[1:]])
            arguments = arguments[arguments.index('--pipe')+1:]
        scriptSlice = arguments[:]
        if not arguments:
            return
        self.ScriptList.append([os.path.splitext(os.path.split(scriptSlice[0])[1])[0],scriptSlice[1:]])
           
    def GetCompatibleMember(self,member,script):
        pushedInputMembers = [scriptMember for scriptMember in script.InputMembers if scriptMember.Pushed]
        compatibleOutputMembers = [scriptMember for scriptMember in pushedInputMembers + script.OutputMembers if scriptMember.AutoPipe and (scriptMember.MemberName == member.MemberName) and (scriptMember.MemberType == member.MemberType)]
        if not compatibleOutputMembers:
            return None
        return compatibleOutputMembers[0]
    
    def AutoPipeScriptObject(self,scriptObject):
        self.PrintLog('Automatic piping ' + scriptObject.ScriptName)
        for memberEntry in scriptObject.InputMembers:
            if not memberEntry.AutoPipe:
                continue
            if memberEntry.MemberType == 'id':
                continue
            if memberEntry.MemberType == 'handle':
                continue
           
            candidateScriptObjectList = [candidateScriptObject for candidateScriptObject in self.ScriptObjectList if self.GetCompatibleMember(memberEntry,candidateScriptObject)]
            if not candidateScriptObjectList:
                continue
            pipedScriptObject = candidateScriptObjectList[-1]
            pipedMember = self.GetCompatibleMember(memberEntry,pipedScriptObject)
            memberEntry.MemberPipe = pipedScriptObject.ScriptName + '-' + str(pipedScriptObject.Id) + '.' + pipedMember.MemberName
            self.PrintLog(memberEntry.MemberName + ' = ' + memberEntry.MemberPipe,1)

    def GetScriptObject(self,scriptName,scriptId):
        for scriptObject in self.ScriptObjectList:
            if (scriptObject.ScriptName == scriptName) & (scriptObject.Id == scriptId):
                return scriptObject

    def ExplicitPipeScriptObject(self,scriptObject):
        self.PrintLog('Explicit piping ' + scriptObject.ScriptName)
        for memberEntry in scriptObject.InputMembers:
            memberName  = memberEntry.MemberName
            option = memberEntry.OptionName
            memberType  = memberEntry.MemberType
            if memberEntry.ExplicitPipe == 'None':
                memberEntry.MemberPipe = None
                exec ('scriptObject.'+memberEntry.MemberName+'= None')
                continue
            if memberEntry.ExplicitPipe:
                pipedArgument = memberEntry.ExplicitPipe
                splitPipedArgument = pipedArgument.split('.')
                if (len(splitPipedArgument)<2):
                    self.PrintError('Error: invalid option piping: '+pipedArgument)
                upstreamPipedModuleName = splitPipedArgument[0]
                if not upstreamPipedModuleName:
                    upstreamPipedModuleName = self.ScriptObjectList[-1].ScriptName
                upstreamPipedOption = ''
                if (len(splitPipedArgument) == 2):
                    upstreamPipedOption = splitPipedArgument[1]
                else:
                    self.PrintError('Error: invalid option piping: '+pipedArgument)

                upstreamPipedId = ''
                splitUpstreamPipedModuleName = upstreamPipedModuleName.split('-')
                if  (len(splitUpstreamPipedModuleName) > 1):
                    upstreamPipedModuleName = splitUpstreamPipedModuleName[-2]
                    upstreamPipedId = splitUpstreamPipedModuleName[-1]
                    
                if not upstreamPipedOption:
                    self.PrintError('Error: invalid option piping: '+pipedArgument)

                candidateScriptObjectList = []

                if upstreamPipedModuleName:
                    candidateScriptObjectList = [candidateScriptObject for candidateScriptObject in self.ScriptObjectList if candidateScriptObject.ScriptName == upstreamPipedModuleName]
                    if upstreamPipedId:
                        candidateScriptObjectList = [candidateScriptObject for candidateScriptObject in candidateScriptObjectList if upstreamPipedId == candidateScriptObject.Id]
                     
                if not candidateScriptObjectList:
                    self.PrintError('Error: invalid option piping: '+pipedArgument)
                    continue
                    
                pipedScriptObject = candidateScriptObjectList[-1]
                 
                candidatePipedMembers = [member for member in pipedScriptObject.OutputMembers + pipedScriptObject.InputMembers if upstreamPipedOption == member.OptionName]

                if not candidatePipedMembers:
                    self.PrintError('Error: invalid option piping: '+pipedArgument)
                    continue

                pipedMember = candidatePipedMembers[0]
               
                memberEntry.MemberPipe = pipedScriptObject.ScriptName + '-' + str(pipedScriptObject.Id) + '.' + pipedMember.MemberName
                self.PrintLog(memberName+' = '+memberEntry.MemberPipe,1)
               
    def PipeScriptObject(self,scriptObject):
        for memberEntry in [member for member in scriptObject.InputMembers if member.MemberPipe and not member.MemberValue]:
            pipedScriptName = memberEntry.MemberPipe.split('.')[0].split('-')[0]
            pipedScriptId = memberEntry.MemberPipe.split('.')[0].split('-')[1]
            pipedMemberName = memberEntry.MemberPipe.split('.')[1]
            previousScriptObjects = self.ScriptObjectList[:]
            if scriptObject in previousScriptObjects:
                previousScriptObjects = previousScriptObjects[:previousScriptObjects.index(scriptObject)]
            candidatePipedScriptObjects = [candidateScriptObject for candidateScriptObject in previousScriptObjects if (candidateScriptObject.ScriptName == pipedScriptName) and (candidateScriptObject.Id == pipedScriptId)]
            pipedScriptObject = candidatePipedScriptObjects[-1]
            exec ('scriptObject.'+memberEntry.MemberName+'='+'pipedScriptObject.'+pipedMemberName)
               
    def Execute(self):
        self.ScriptObjectList = []
        for scriptNameAndArguments in self.ScriptList:
            self.PrintLog('')
            scriptName = scriptNameAndArguments[0]
            moduleName = scriptName
            scriptArguments = scriptNameAndArguments[1]
            imported = True
            try:
                exec('import '+ moduleName)
            except ImportError, e:
                self.PrintError(e.message)
                break
            scriptObjectClassName = ''
            exec ('scriptObjectClassName =  '+moduleName+'.'+moduleName)
            moduleScriptObjectClassName = moduleName+'.'+scriptObjectClassName
            self.PrintLog('Creating ' + scriptObjectClassName + ' instance.')
            scriptObject = 0
            exec ('scriptObject = '+moduleScriptObjectClassName+'()')
            scriptArguments = scriptNameAndArguments[1]
            scriptObject.Arguments = scriptArguments
            scriptObject.LogOn = self.LogOn
            if self.InputStream:
                scriptObject.InputStream = self.InputStream
            if self.OutputStream:
                scriptObject.OutputStream = self.OutputStream
            scriptObject.ExitOnError = self.ExitOnError
            if self.AutoPipe:
                self.AutoPipeScriptObject(scriptObject)
            self.PrintLog('Parsing options ' + scriptObject.ScriptName)
            execute = scriptObject.ParseArguments()
            if not execute:
                return
            if scriptObject.Disabled:
                self.PrintLog('\n' + scriptObject.ScriptName + ' is disabled. Bypassing it.')
                continue
            self.ExplicitPipeScriptObject(scriptObject)
            self.PipeScriptObject(scriptObject)
            scriptObject.PrintInputMembers()
            scriptObject.IORead()
            self.PrintLog('Executing ' + scriptObject.ScriptName + ' ...')
            scriptObject.Execute()
            self.PrintLog('Done executing ' + scriptObject.ScriptName + '.')
            scriptObject.IOWrite()
            scriptObject.PrintOutputMembers()
            self.ScriptObjectList.append(scriptObject)

        for scriptObject in self.ScriptObjectList:
            scriptObject.Deallocate()

def PypeRun(arguments):
  
    pipe = Pype()
    pipe.ExitOnError = 0
    pipe.SetArgumentsString(arguments)
    pipe.ParseArguments()
    pipe.Execute()
    return pipe
  

if __name__=='__main__':
    from vmtk import pypes
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()

#    pipe = Pype()
#    pipe.Arguments = sys.argv
#    pipe.ParseArguments()
#    pipe.Execute()

