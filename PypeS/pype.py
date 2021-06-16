#!/usr/bin/env python
# -*- coding: utf-8 -*-

## Program:   PypeS
## Module:    $RCSfile: pype.py,v $
## Language:  Python
## Date:      $Date: 2006/07/07 10:45:42 $
## Version:   $Revision: 1.18 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import # NEEDS TO STAY AS TOP LEVEL IMPORT
import sys
import os
import importlib
from inspect import isclass
import re
import inspect
from .pypesutils import all_indices

pype = 'Pype'

class Pype(object):
    '''
    Orchestrates the interaction among PypeScripts.

    It enables one to pipe one PypeScript after another, and it takes care
    of connecting the right arguments from one script to the other. It can
    be called from the command line by issuing pype pype-arguments or
    automatically instantiated from a pypescript.
    '''

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

    def PrintLog(self,logMessage,indent=0):
        '''Prints log messages from pype controller members to the console.

        All vmtkscripts subclassing from pypes.Pype use the PrintLog method in order
        to write their output to the console.

        Args:
            logMessage (str): the message to be logged
            indent (int): number of spaces to intent the message. Defaults to 0.
        '''
        if not self.LogOn:
            return
        indentUnit = '    '
        indentation = indent * indentUnit
        self.OutputStream.write(indentation + logMessage + '\n')

    def PrintError(self,errorMessage):
        ''' Prints error messages from pype controller to the console then raises a runtime error.

        Args:
            errorMessage (string): the error message to print to the console
        '''
        if self.ExitOnError:
            self.OutputStream.write(errorMessage + '\n')
            sys.exit()
        else:
            raise RuntimeError(errorMessage)

    def SetArgumentsString(self,argumentsString,**kwargs):
        ''' Splits an input string into a list containing the class name and arguments.

        sets the class attribute "Arguments" by splitting the input string along spaces (or where
        appropriate if there are quotes for windows paths) into individual elements in a list.
        If the argument is enclosed in {curly braces}, a key-value pair with the same name and
        desired value should have been feed in from the calling scope in the kwargs dict.

        If the argument exists with curly braces, we don't need to check if it exists in the kwargs dict
        (at this point). By the time the kwargs reach this function (through vmtkinteractive), we have
        already done the checks to make sure that the variable contained within is a valid key-value pair.

        Args:
            argumentsString (string): the input argument string
            **kwargs (`obj`:dict): stores the values or object instances associated with the key names
                which were called from the local instance.

        ex: `vmtkimageviewer -ifile foo -flip {flipvar}' (where flipvar = [0, 1, 0])
            -> ["vmtkimageviewer", "-ifile", "foo", "-flip", [0, 1, 0]]
        '''
        if '"' not in argumentsString:
            arguments = argumentsString.split()

        else:
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

        itemToReplace = []
        for index, item in enumerate(arguments):
            if item.startswith('{') and item.endswith('}'):
                itemName = item[1:-1]
                itemValue = kwargs[itemName]
                itemToReplace.append({'index': index,
                                      'value': itemValue})
        for replace in itemToReplace:
            arguments[replace['index']] = replace['value']

        self.Arguments = arguments

    def _ParsePyperunControlingArgumentFlags(self, arguments):
        '''execute special case argument flag behavior if the pyperun executable is being prepended to a pype

        This was originally used to make parsing more robust. To control the pype behavior or to access global
        pype functionality like --nolog, the pyperun executable is prepended to a pype.

        checks if --noauto --nolog flags exist in the arguments list. if they do then execute the
        special behavior indicated by those flags. The --help flag prints the pyperun usage string.

        arguments:
            arguments: (obj:`list`): the initial arguments acquired from sys.argv with each logical unit as a string element.
        returns:
            obj:`list`: a modified copy of the input arguments with any of the special case flag elements removed
        '''
        if os.path.basename(arguments[0]).strip() in ['pyperun']:
            arguments.remove(arguments[0])
            pypeArguments = []
            for arg in arguments:
                if arg[:2] == '--':
                    pypeArguments.append(arg)
                else:
                    break
            if '--help' in pypeArguments:
                self.PrintLog('Usage: pype --nolog --noauto firstScriptName -scriptOptionName '\
                              'scriptOptionValue --pipe secondScriptName -scriptOptionName scriptOptionValue '\
                              '-scriptOptionName @firstScriptName.scriptOptionName -id 2 --pipe thirdScriptName '\
                              '-scriptOptionName @secondScriptName-2.scriptOptionName')
            if '--noauto' in pypeArguments:
                self.AutoPipe = 0
            if '--nolog' in pypeArguments:
                self.LogOn = 0
            for arg in pypeArguments:
                arguments.remove(arg)
        return arguments

    def _ParseArgumentsFileBrowser(self, arguments):
        '''open a file browser to specify input file if one of the elements in the arguments list == "FILE"

        arguments:
            arguments (obj:`list`): the initial arguments acquired from sys.argv with each logical unit as a string element.

        returns:
            obj:`list`: a modified copy of the input arguments with elements == FILE replaced with a path on the computer
        '''
        if 'FILE' in arguments:
            text = ''
            self.OutputStream.write('\nThe current pype contains filename placeholders identified by the string FILE.')
            self.OutputStream.write('\nEvery occurrence of FILE is meant to be replaced with an actual file path.\n')
            self.OutputStream.write('\nPress \'c\' to continue and be automatically prompted for the required filenames.')
            self.OutputStream.write('\nPress \'e\' to exit. You can still edit your pype manually and run it back.\n\n')
            while text not in ['c','e']:
                self.OutputStream.write('> ')
                text = self.InputStream.readline().rstrip('\n')
            if text == 'e':
                return
            elif text =='c':
                while 'FILE' in arguments:
                    arguments[arguments.index('FILE')] = 'BROWSER'
        return arguments

    def ParseArguments(self):
        '''split a flat list of scripts and arguments defining the pype into a nested structure linking arguments to each script.

        Given a flat pipe command, the input list is split at each "--pipe" element. For each slice (before the first,
        between each, and after the last "--pipe" index) a new list is created. The first element contains the name of the
        script to execute. The second element contains another list with all the following arguments placed inside it. Each of
        these "slice" lists are then appended to the class ScriptList attribute.

        This method sets the object ScriptList attribute as it's side effect rather than returning a value.

        example:
            self.Arguments = ['/Users/rick/projects/vmtk/vmtk-build/Install/bin/vmtkimagereader', '-ifile', './aorta.mha', '-flip',
                              '0', '1', '0', '--pipe', 'vmtkimageviewer', '-display', '0', '--pipe', 'vmtkimageotsuthresholds']
            will be converted to ->
            self.ScriptList = [['vmtkimagereader', ['-ifile', './aorta.mha', '-flip', '0', '1', '0']],
                               ['vmtkimageviewer', ['-display', '0']],
                               ['vmtkimageotsuthresholds', []]]
        '''
        self.ScriptList = []
        pyperunParsedArguments = self._ParsePyperunControlingArgumentFlags(self.Arguments[:])
        arguments = self._ParseArgumentsFileBrowser(pyperunParsedArguments)

        pipeIndices = all_indices('--pipe', arguments)
        scriptNameIndices = [0] + [int(x + 1) for x in pipeIndices] # there is always a script name at index 0
        for scriptNumber, scriptNameIndex in enumerate(scriptNameIndices):
            scriptName = os.path.split(arguments[scriptNameIndex])[1]
            try:
                scriptArgs = arguments[scriptNameIndex+1:pipeIndices[scriptNumber]]
            except IndexError:
                scriptArgs = arguments[scriptNameIndex+1:]
            self.ScriptList.append([scriptName, scriptArgs])

    def _CompatibleMember(self,memberEntry,script):
        '''given a memberEntry object, and script object, find compatible input members for a script object.

        we search the output members of the script object (along with any explicitly pushed input members)
        for  member entries which match the name and type of the input memberEntry object. When a comatible
        script member is found, we return it; otherwise return None.

        arguments:
            memberEntry (obj): a memberEntry object
            script (obj): a script object

        returns:
            (obj): a script member compatible with memberEntry, or None if not compatible
        '''
        pushedInputMembers = [scriptMember for scriptMember in script.InputMembers if scriptMember.Pushed]
        for scriptMember in pushedInputMembers + script.OutputMembers:
            if not scriptMember.AutoPipe: continue # only true when --no-auto flag is provided
            if ((scriptMember.MemberName == memberEntry.MemberName) and
                (scriptMember.MemberType == memberEntry.MemberType)):
                # only if scriptMember name/types both match memberEntry names/types
                return scriptMember
        return None

    def AutoPipeScriptObject(self,scriptObject):
        '''automatically connect inputs members of a scriptObject with outputMembers which have come before.

        When a script completes execution, the next script to execute is sent here. This attempts to automatically
        connect compatible output members of prior script objects with the input members of this (the next) script
        object. If a compatible member is found, this script object's input memberEntry.MemberPipe attribute
        is set. Note: Automatic piping is override by explicit member piping.

        example:
            for -> vmtkimagereader -ifile foo --pipe vmtkimageviewer
                the vmtkimageviewer memberEntry.MemberPipe for memberEntry MemberName == Image
                is set to: vmtkimagereader-0.Image
        '''
        self.PrintLog('Automatic piping ' + scriptObject.ScriptName)
        for memberEntry in scriptObject.InputMembers:
            if ((not memberEntry.AutoPipe) or
                (memberEntry.MemberType == 'id') or
                (memberEntry.MemberType == 'handle')): continue # these member types we never want to match
            # if a compatible member is found between InputMember entries and the prior script objects output
            # members, the compatible member is assigned to the pipedScriptObject name. We use the None value
            # to check wheather a compatible memver was found or not.
            pipedScriptObject = None
            for candidateScript in self.ScriptObjectList:
                if self._CompatibleMember(memberEntry,candidateScript):
                    pipedScriptObject = candidateScript
                    pipedMember = self._CompatibleMember(memberEntry, candidateScript)
            if not pipedScriptObject:
                continue
            # MemberPipe attribute syntax ex: 'vmtkimagereader-0.Image' or 'vmtklevelsetsegmentation-0.LevelSets'
            # this is critical attribute which specifies which script object member (script name, id, and member name)
            # should actually be transfered to the input member of the currently executing script.
            memberEntry.MemberPipe = pipedScriptObject.ScriptName + '-' + str(pipedScriptObject.Id) + '.' + pipedMember.MemberName
            self.PrintLog(memberEntry.MemberName + ' = ' + memberEntry.MemberPipe,1)

    def ExplicitPipeScriptObject(self,scriptObject):
        '''manually connect input members and  outputMembers of scriptObjects

        manual connection is specified by using the @ symbol, followed by the name of
        the script we want to pipe from, dot the piped option. This module determines which
        of the three types of explicit piping methods was used:
            1) option only (@.o): shorthand for specifying previous scriptname with option "o"
            2) scriptname and option only (@foo.o): specify scriptname "foo" with option "o"
            3) scriptname, id, and option (@foo-1.o): specify which scriptname to use if multiple are
               included in the same pipe, with option "o"

        Followed by the appropriate check for which scriptobjects are candidates for the given name and option.
        Nothing is returned, instead the scriptObject memberEntry.MemberPipe is set with the appropriate
        specifier for it's data source.

        arguments:
            scriptObject (obj): the scriptObject which needs to have it's input members set.

        TODO: Refactor into separate methods
        '''
        self.PrintLog('Explicit piping ' + scriptObject.ScriptName)
        for memberEntry in scriptObject.InputMembers:
            memberName  = memberEntry.MemberName
            if memberEntry.ExplicitPipe == 'None':
                memberEntry.MemberPipe = None
                setattr(scriptObject, memberEntry.MemberName, None)
                continue
            if not memberEntry.ExplicitPipe:
                continue

            pipedArgument = memberEntry.ExplicitPipe
            splitPipedArgument = pipedArgument.split('.')
            if len(splitPipedArgument) != 2:
                self.PrintError('Error: invalid option piping: '+pipedArgument)

            # check for 3 option cases described in docstring. upstreamPipedOption is common for every case
            # upstreamPipedId only overwritten in case 2
            upstreamPipedId = ''
            upstreamPipedOption = splitPipedArgument[1]
            if pipedArgument.startswith('.'):
                upstreamPipedModuleName = self.ScriptObjectList[-1].ScriptName
            elif ('-' in pipedArgument) and ('.' in pipedArgument):
                upstreamPipedModuleName, upstreamPipedId = splitPipedArgument[0].split('-')
            else:
                upstreamPipedModuleName = splitPipedArgument[0]

            candidateScriptObjectList = []
            for candidateScriptObject in self.ScriptObjectList:
                if candidateScriptObject.ScriptName == upstreamPipedModuleName:
                    if not upstreamPipedId:
                        candidateScriptObjectList.append(candidateScriptObject)
                    # if there is an upstream piped id, then candidateScriptObjectList should
                    # only contain one element (as long as a valid id was selected)
                    elif upstreamPipedId == candidateScriptObject.Id:
                        candidateScriptObjectList.append(candidateScriptObject)
            try:
                pipedScriptObject = candidateScriptObjectList[-1]
            except IndexError:
                self.PrintError('Error: invalid option piping: ' + pipedArgument)

            for member in pipedScriptObject.OutputMembers + pipedScriptObject.InputMembers:
                if upstreamPipedOption == member.OptionName:
                    pipedMember = member
                    break
            try:
                memberEntry.MemberPipe = pipedScriptObject.ScriptName + '-' + str(pipedScriptObject.Id) + '.' + pipedMember.MemberName
                self.PrintLog(memberName+' = '+memberEntry.MemberPipe,1)
            except NameError:
                self.PrintError('Error: invalid option piping: '+pipedArgument)

    def PipeScriptObject(self,scriptObject):
        '''perform the actual data transfer from one script object into the input member of another.

        arguments:
            scriptObject (obj): an instance of scriptObject which has had it's memberEntry.MemberPipe
                attributes set through automatic and explicit pyping
        '''
        scriptMemberList = []
        for member in scriptObject.InputMembers:
            if (member.MemberPipe) and (not member.MemberValue):
                scriptMemberList.append(member)

        for memberEntry in scriptMemberList:
            # pipedScriptName = memberEntry.MemberPipe.split('.')[0].split('-')[0]
            # pipedScriptId = memberEntry.MemberPipe.split('.')[0].split('-')[1]
            # pipedMemberName = memberEntry.MemberPipe.split('.')[1]

            pipedScriptInfo, pipedMemberName = memberEntry.MemberPipe.split('.')
            pipedScriptName, pipedScriptId = pipedScriptInfo.split('-')

            previousScriptObjects = self.ScriptObjectList[:]
            if scriptObject in previousScriptObjects:
                previousScriptObjects = previousScriptObjects[:previousScriptObjects.index(scriptObject)]

            candidatePipedScriptObjects = []
            for candidateScriptObject in previousScriptObjects:
                if (candidateScriptObject.ScriptName == pipedScriptName) and (candidateScriptObject.Id == pipedScriptId):
                    candidatePipedScriptObjects.append(candidateScriptObject)

            pipedScriptObject = candidatePipedScriptObjects[-1]
            pipedScriptObjectMemberName = getattr(pipedScriptObject, pipedMemberName)
            setattr(scriptObject, memberEntry.MemberName, pipedScriptObjectMemberName)

    def Execute(self):
        '''main method to run the pype.

        This method handles execution of the pype which is composed by the ParseArguments() method.
        For every script specification (name and arguments) defined within the ScriptList attribute
        this imports the main script object, sets logging and execution behavior of the pypescript base
        class, computes the compatible input output members, transfers data between script objects, and
        finally transfers control to the script object so it can perform it's intended operation.
        '''
        try:
            from vmtk import pypes
        except ImportError:
            return None

        self.ScriptObjectList = []
        for scriptNameAndArguments in self.ScriptList:
            self.PrintLog('')
            scriptName = scriptNameAndArguments[0]
            try:
                module = importlib.import_module('vmtk.vmtkscripts.'+scriptName)
                # Find the principle class to instantiate the requested action defined inside the requested writerModule script.
                # Returns a single member list (containing the principle class name) which satisfies the following criteria:
                #   1) is a class defined within the script
                #   2) the class is a subclass of pypes.pypescript
                scriptObjectClasses = [x for x in dir(module) if isclass(getattr(module, x)) and issubclass(getattr(module, x), pypes.pypeScript)]
                scriptObjectClassName = scriptObjectClasses[0]
            except ImportError as e:
                self.PrintError(str(e))

            scriptObject = getattr(module, scriptObjectClassName)
            scriptObject = scriptObject()
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
            proceedToExecute = scriptObject.ParseArguments() # calls to scriptObject subclass of pypes.pypeScript ParseArguments method
            if proceedToExecute == False:
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


def PypeRun(arguments, **kwargs):
    pipe = Pype()
    pipe.ExitOnError = 0
    pipe.SetArgumentsString(arguments, **kwargs)
    pipe.ParseArguments()
    pipe.Execute()
    return pipe


if __name__=='__main__':
    from vmtk import pypes
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()