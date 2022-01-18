#!/usr/bin/env python

## Program:   PypeS
## Module:    $RCSfile: pypescript.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:36:30 $
## Version:   $Revision: 1.18 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import sys
import string
import os.path
import importlib
from inspect import isclass


class pypeMember(object):

    def __init__(self,memberName,optionName,memberType,memberLength,memberRange='',memberDoc='',memberIO=''):

        self.MemberName = memberName
        self.OptionName = optionName
        self.MemberType = memberType
        self.MemberLength = memberLength
        self.MemberRange = memberRange
        self.MemberDoc = memberDoc
        self.MemberIO = memberIO

        self.MemberPipe = ''
        self.MemberValue = None
        self.ExplicitPipe = ''
        self.AutoPipe = 1
        self.Pushed = 0

    def IsInRange(self,value):
        if not self.MemberRange:
            return
        if self.MemberRange[0] == '[' and self.MemberRange[-1] == ']':
            parsedMemberRange = [eval(entry) for entry in self.MemberRange[1:-1].split(',')]
            if value in parsedMemberRange:
                return True
        if self.MemberRange[0] == '(' and self.MemberRange[-1] == ')':
            parsedMemberRange = [entry for entry in self.MemberRange[1:-1].split(',')]
            if len(parsedMemberRange) < 2:
                return False
            inRange = True
            if parsedMemberRange[0] and value < eval(parsedMemberRange[0]):
                inRange = False
            if parsedMemberRange[1] and value > eval(parsedMemberRange[1]):
                inRange = False
            return inRange
        return False

    def GetRangeEnumeration(self):
        if not self.MemberRange:
            return []
        if self.MemberRange[0] == '[' and self.MemberRange[-1] == ']':
            parsedMemberRange = [eval(entry) for entry in self.MemberRange[1:-1].split(',')]
            return parsedMemberRange
        return []

    def GetRangeValues(self):
        if not self.MemberRange:
            return []
        if self.MemberRange[0] == '(' and self.MemberRange[-1] == ')':
            parsedMemberRange = []
            for entry in self.MemberRange[1:-1].split(','):
                if entry != '':
                    entry = eval(entry)
                else:
                    entry = None
                parsedMemberRange.append(entry)
            if len(parsedMemberRange) == 2:
                parsedMemberRange.append(None)
            if len(parsedMemberRange) != 3:
                return []
            return parsedMemberRange
        return []

    def GetRangeRepresentation(self):
        if not self.MemberRange:
            return []
        enumeration = self.GetRangeEnumeration()
        values = self.GetRangeValues()
        if enumeration:
            return 'in ' + str(enumeration)
        if values:
            representation = ''
            if values[0] != None:
                representation += '>= ' + str(values[0])
            if values[1] != None:
                if representation:
                    representation += ' and '
                representation += '<= ' + str(values[1])
            return representation


class pypeScript(object):
    '''the base class for every high-level script.

    It manages parsing, instantiates proper input/output methods for the
    script and keeps the script structure consistent. Each PypeScript is at
    the same time:
       - a script which can be called from the command line and piped to other scripts
       - a class which can be called from Python code (e.g. inside a tkinter GUI)

    Attributes:
        BuiltinOptionTypes (list):
        InputStream (function):
        OutputStream (function):
        ScriptName (string):
        ScriptDoc (string):
        Arguments (list):
        InputMembers (list):
        OutputMembers (list):
        Id (string):
        Disabled (bool):
        ExitOnError (bool):
        LogOn (bool):
        Progress (int):

    '''

    lastVisitedPath = '.'

    def __init__(self):

        self.BuiltinOptionTypes = ['int','str','float']

        self.InputStream = sys.stdin
        self.OutputStream = sys.stdout

        self.ScriptName = ''
        self.ScriptDoc = ''
        self.Arguments = []
        self.InputMembers = []
        self.OutputMembers = []
        self.Id = '0'
        self.Self = self
        self.Disabled = 0

        idMember = pypeMember('Id','id','str',1,'','script id')
        idMember.AutoPipe = 0
        self.InputMembers.append(idMember)
        self.OutputMembers.append(idMember)
        selfMember = pypeMember('Self','handle','self',1,'','handle to self')
        selfMember.AutoPipe = 0
        self.InputMembers.append(selfMember)
        self.OutputMembers.append(selfMember)
        selfMember = pypeMember('Disabled','disabled','bool',1,'','disable execution and piping')
        selfMember.AutoPipe = 0
        self.InputMembers.append(selfMember)

        self.ExitOnError = 1
        self.LogOn = 1

        self.Progress = 0

    def PrintLog(self,logMessage,indent=0):
        if not self.LogOn:
            return
        indentUnit = '    '
        indentation = ''
        for i in range(indent):
            indentation = indentation + indentUnit
        self.OutputStream.write(indentation + logMessage + '\n')

    def PrintError(self,errorMessage):
        self.OutputStream.write(errorMessage + '\n')
        if self.ExitOnError:
          self.Exit()
        else:
          raise RuntimeError(errorMessage)

    def Exit(self):
        sys.exit()

    def OutputText(self,text):
        self.OutputStream.write(text)

    def OutputProgress(self,progress,percentStep):
        if int(100 * progress)/percentStep == int(100 * self.Progress)/percentStep:
            return
        self.Progress = progress
        self.OutputStream.write('\r')
        self.OutputStream.write('Progress: '+str(int(100 * self.Progress))+'%')
        self.OutputStream.flush()
        self.InputInfo('Progress: '+str(int(100 * self.Progress))+'%')

    def EndProgress(self):
        self.OutputStream.write('\n')

    def InputInfo(self,prompt=''):
        self.OutputText(prompt)
        try:
            self.InputStream.prompt(prompt,info=True)
        except:
            pass

    def InputText(self,prompt='',validator=None):
        self.OutputText(prompt)
        try:
            self.InputStream.prompt(prompt)
        except:
            pass
        text = self.InputStream.readline()
        if text:
            text = text.rstrip('\n')
        if validator:
            while not validator(text):
                self.OutputText(prompt)
                try:
                    self.InputStream.prompt(prompt,info=False)
                except:
                    pass
                text = self.InputStream.readline()
                if text:
                    text = text.rstrip('\n')
        return text

    def PrintMembers(self,members):
        for memberEntry in members:
            memberName  = memberEntry.MemberName
            memberType = memberEntry.MemberType
            memberPipe = memberEntry.MemberPipe
            memberValue = self.__getattribute__(memberName)
##            if memberPipe:
##                self.__getattribute__("PrintLog")(memberName+' = '+memberPipe,1)
            if memberName == 'Self':
                pass
            elif (memberType in self.BuiltinOptionTypes + ["bool"]) or (memberType == 'list'):
                self.__getattribute__("PrintLog")(memberName+' = '+str(memberValue),1)
            elif memberValue:
                self.__getattribute__("PrintLog")(memberName+' = '+memberType,1)
            else:
                self.__getattribute__("PrintLog")(memberName+' = '+str(memberValue),1)

    def PrintInputMembers(self):
        self.PrintLog("Input " + self.ScriptName + " members:")
        self.PrintMembers(self.InputMembers)

    def PrintOutputMembers(self):
        self.PrintLog("Output " + self.ScriptName + " members:")
        self.PrintMembers(self.OutputMembers)

    def ConvertToPypeMembers(self,members):
        pypeMembers = []
        for member in members:
            if type(member) == pypeMember:
                pypeMembers.append(member)
            elif type(member) == list:
                pypeMembers.append(pypeMember(*member))
        return pypeMembers

    def SetInputMembers(self,membersToAppend):
        pypeMembersToAppend = self.ConvertToPypeMembers(membersToAppend)
        for member in pypeMembersToAppend:
            self.InputMembers.append(member)
            if member.MemberIO:
                filenameMember = pypeMember(self.GetIOInputFileNameMember(member.MemberName),self.GetIOFileNameOption(member.OptionName),'str',1,'','filename for the default ' + member.MemberName + ' reader')
##                filenameMember.AutoPipe = 0
                exec('self.'+filenameMember.MemberName+' = \'\'')
                self.InputMembers.append(filenameMember)

    def SetOutputMembers(self,membersToAppend):
        pypeMembersToAppend = self.ConvertToPypeMembers(membersToAppend)
        for member in pypeMembersToAppend:
            self.OutputMembers.append(member)
            if member.MemberIO:
                filenameMember = pypeMember(self.GetIOOutputFileNameMember(member.MemberName),self.GetIOFileNameOption(member.OptionName),'str',1,'','filename for the default ' + member.MemberName + ' writer')
##                filenameMember.AutoPipe = 0
                exec('self.'+filenameMember.MemberName+' = \'\'')
                self.InputMembers.append(filenameMember)

    def SetScriptName(self,scriptName):
        self.ScriptName = scriptName

    def SetScriptDoc(self,scriptDoc):
        self.ScriptDoc = scriptDoc

    def GetScriptDocString(self):
        return self.ScriptDoc

    def GetUsageString(self):
        usageString = ''
        scriptUsageString = os.path.splitext(self.ScriptName)[0]
        if self.ScriptDoc:
            scriptUsageString += ' : ' + self.ScriptDoc
        useTextWrap = 1
        try:
            import textwrap
        except ImportError:
            useTextWrap = 0
        else:
            textwrapper = textwrap.TextWrapper()
            textwrapper.initial_indent = ''
            textwrapper.subsequent_indent = ' '
        if useTextWrap:
            usageString += textwrapper.fill(scriptUsageString)
        else:
            usageString += scriptUsageString
        for memberList in [self.InputMembers, self.OutputMembers]:
            if memberList == self.InputMembers :
                 usageString += '\n' + '  ' + 'Input arguments:'
            elif memberList == self.OutputMembers :
                 usageString += '\n' + '  ' + 'Output arguments:'
            for memberEntry in memberList:
                memberUsageString = ''
                indentation = '   '
                if useTextWrap:
                    textwrapper.initial_indent = indentation
                    textwrapper.subsequent_indent = indentation + '  '
                memberName  = memberEntry.MemberName
                option = memberEntry.OptionName
                memberType = memberEntry.MemberType
                memberLength = memberEntry.MemberLength
                memberRange = memberEntry.MemberRange
                memberDoc = memberEntry.MemberDoc
                if option!='':
                    default = 0
                    if memberLength == 0:
                        memberUsageString += '-' + option + ' ' + memberName
                    elif memberType in self.BuiltinOptionTypes + ["bool"]:
                        default = self.__getattribute__(memberName)
                        memberUsageString += '-' + option + ' ' + memberName + ' (' + memberType + ',' + str(memberLength) + ')'
                        if memberRange:
                            memberUsageString += " " + memberEntry.GetRangeRepresentation()
                        if default != '':
                            memberUsageString += '; default=' + str(default)
                    else:
                        memberUsageString += '-' + option + ' ' + memberName + ' (' + memberType + ',' + str(memberLength) + ')'
                    if memberDoc != '':
                        memberUsageString += ': ' + memberDoc
                if useTextWrap:
                    usageString += '\n' + textwrapper.fill(memberUsageString)
                else:
                    usageString += '\n' + memberUsageString
        usageString += '\n'
        return usageString

    def GetHTMLUsageString(self):
        usageString = ''
        #usageString += '---' + '\n'
        #usageString += 'layout: default' + '\n'
        #usageString += '---' + '\n'
        usageString += '<h1>'
        usageString += self.ScriptName
        usageString += '</h1>'
        usageString += '\n'
        if self.ScriptDoc != '':
            usageString += '<h2>Description</h2>' + '\n'
            usageString += self.ScriptDoc + '\n'
        for memberList in [self.InputMembers, self.OutputMembers]:
            if memberList == self.InputMembers :
                 usageString += '<h3>Input arguments</h3>' + '\n'
            elif memberList == self.OutputMembers :
                 usageString += '<h3>Output arguments</h3>' + '\n'
            usageString += '<table class="vmtkscripts">' + '\n'
            usageString += '<tr>' + '\n'
            usageString += '<th>Argument</th><th>Variable</th><th>Type</th><th>Length</th><th>Range</th><th>Default</th><th>Description</th>\n'
            usageString += '</tr>' + '\n'
            for memberEntry in memberList:
                memberUsageString = ''
                memberName  = memberEntry.MemberName
                option = memberEntry.OptionName
                memberType = memberEntry.MemberType
                memberLength = memberEntry.MemberLength
                memberRange = memberEntry.MemberRange
                memberDoc = memberEntry.MemberDoc
                if option!='':
                    default = 0
                    if memberLength == 0:
                        memberUsageString += '<td>' + option + '</td><td>' + memberName + '</td><td></td><td></td><td></td><td></td>'
                    elif memberType in self.BuiltinOptionTypes + ["bool"]:
                        default = self.__getattribute__(memberName)
                        memberUsageString += '<td>' + option + '</td><td>' + memberName + '</td><td>' + memberType + '</td><td>' + str(memberLength) + '</td><td>' + str(memberRange) + '</td><td>' + str(default) + '</td>'
                    else:
                        memberUsageString += '<td>' + option + '</td><td>' + memberName + '</td><td>' + memberType + '</td><td>' + str(memberLength) + '</td><td></td><td></td>'
                    if not memberDoc:
                        memberDoc = ''
                    memberUsageString += '<td>' + memberDoc + '</td>'
                    #memberUsageString += ' | '
                memberUsageString += '\n'
                usageString += '<tr>' + memberUsageString + '</tr>' + '\n'
            usageString += '</table>\n'
        usageString += '\n'
        return usageString

    def GetDokuWikiUsageString(self):
        usageString = ''
        usageString = '======'
        usageString += self.ScriptName
        usageString += '======'
        usageString += '\n'
        if self.ScriptDoc != '':
            usageString += '=====Description=====' + '\n'
            usageString += self.ScriptDoc + '\n'
        for memberList in [self.InputMembers, self.OutputMembers]:
            if memberList == self.InputMembers :
                 usageString += '=====Input arguments=====' + '\n'
            elif memberList == self.OutputMembers :
                 usageString += '=====Output arguments=====' + '\n'
            usageString += '^ Argument ^ Variable ^ Type ^ Length ^ Range ^ Default ^ Description ^\n'
            for memberEntry in memberList:
                memberUsageString = ''
                memberName  = memberEntry.MemberName
                option = memberEntry.OptionName
                memberType = memberEntry.MemberType
                memberLength = memberEntry.MemberLength
                memberRange = memberEntry.MemberRange
                memberDoc = memberEntry.MemberDoc
                if option!='':
                    default = 0
                    if memberLength == 0:
                        memberUsageString += '| ' + option + ' | ' + memberName + ' | | | | | '
                    elif memberType in self.BuiltinOptionTypes + ["bool"]:
                        default = self.__getattribute__(memberName)
                        memberUsageString += '| ' + option + ' | ' + memberName + ' | ' + memberType + ' | ' + str(memberLength) + ' | ' + str(memberRange) + ' | ' + str(default) + ' | '
                    else:
                        memberUsageString += '| ' + option + ' | ' + memberName + ' | ' + memberType + ' | ' + str(memberLength) + ' | | | '
                    if memberDoc != '':
                        memberUsageString += memberDoc
                    memberUsageString += ' | '
                memberUsageString += '\n'
                usageString += memberUsageString
        usageString += '\n'
        return usageString

    def GetPmWikiUsageString(self):
        usageString = 'version=pmwiki-2.2.0-beta65 ordered=1 urlencoded=1\n'
        usageString += 'name=VmtkScripts.' + self.ScriptName.capitalize() + '\n'
        usageString += 'text='
        usageString += '!' + self.ScriptName
        usageString += '%0a'
        if self.ScriptDoc != '':
            usageString += '!!Description' + '%0a'
            usageString += self.ScriptDoc + '%0a'
            usageString += '%0a'
        for memberList in [self.InputMembers, self.OutputMembers]:
            if memberList == self.InputMembers :
                 usageString += '!!Input arguments' + '%0a'
            elif memberList == self.OutputMembers :
                 usageString += '!!Output arguments' + '%0a'
            usageString += '||border=1 cellpadding=3 cellspacing=0' + '%0a'
            usageString += '||!Argument ||!Variable ||!Type ||!Length ||!Range ||!Default ||!Description ||%0a'
            for memberEntry in memberList:
                memberUsageString = ''
                memberName  = memberEntry.MemberName
                option = memberEntry.OptionName
                memberType = memberEntry.MemberType
                memberLength = memberEntry.MemberLength
                memberRange = memberEntry.MemberRange
                memberDoc = memberEntry.MemberDoc
                if option!='':
                    default = 0
                    if memberLength == 0:
                        memberUsageString += '||' + option + ' ||' + memberName + ' || || || || ||'
                    elif memberType in self.BuiltinOptionTypes + ["bool"]:
                        default = self.__getattribute__(memberName)
                        memberUsageString += '||' + option + ' ||' + memberName + ' ||' + memberType + ' ||' + str(memberLength) + ' ||' + str(memberRange) + ' ||' + str(default) + ' ||'
                    else:
                        memberUsageString += '||' + option + ' ||' + memberName + ' ||' + memberType + ' ||' + str(memberLength) + ' || || ||'
                    if memberDoc != '':
                        memberUsageString += memberDoc
                    memberUsageString += ' ||'
                memberUsageString += '%0a'
                usageString += memberUsageString
            usageString += '%0a'
        usageString += '%0a'
        return usageString

    def ParseArguments(self):
        for arg in self.Arguments:
            if arg == '--help':
                self.PrintLog('')
                self.OutputText(self.GetUsageString())
                self.PrintLog('')
                return 0
            if arg == '--doc':
                self.PrintLog('')
                self.OutputText(self.GetScriptDocString())
                self.PrintLog('')
                return 0
            if arg == '--html':
                self.PrintLog('')
                self.OutputText(self.GetHTMLUsageString())
                self.PrintLog('')
                return 0
            if arg == '--dokuwiki':
                self.PrintLog('')
                self.OutputText(self.GetDokuWikiUsageString())
                self.PrintLog('')
                return 0
            if arg == '--pmwiki':
                self.PrintLog('')
                self.OutputText(self.GetPmWikiUsageString())
                self.PrintLog('')
                return 0
            if (arg[0] == '-') & (len(arg)==1):
                self.PrintError(self.GetUsageString() + '\n' + self.ScriptName + ' error: unknown option ' + arg + '\n')
                return 0
            if (arg[0] == '-'):
                if (arg[1] in string.ascii_letters):
                    matchingMembers = [member for member in self.InputMembers if member.OptionName in [arg.lstrip('-'), arg.lstrip('-').rstrip('@')]]
                    if not matchingMembers:
                        self.PrintError(self.GetUsageString() + '\n' + self.ScriptName + ' error: unknown option ' + arg + '\n')
                        return 0

        for memberEntry in self.InputMembers:

            if not memberEntry.OptionName:
                continue

            memberName  = memberEntry.MemberName
            option = '-' + memberEntry.OptionName
            memberType = memberEntry.MemberType
            memberLength = memberEntry.MemberLength
            memberRange = memberEntry.MemberRange
            memberValues = []
            activated = 0
            explicitPipe = 0

            specifiedOptions = [arg for arg in self.Arguments if (arg[0] == '-') and (arg[1] in string.ascii_letters + '-')]

            pushedOption = option + '@'
            if pushedOption in specifiedOptions:
                memberEntry.Pushed = 1
                specifiedOptions[specifiedOptions.index(pushedOption)] = option
                self.Arguments[self.Arguments.index(pushedOption)] = option

            if option in specifiedOptions:
                if memberLength == 0:
                    activated = 1
                optionValues = []
                optionIndex = self.Arguments.index(option)
                if option != specifiedOptions[-1]:
                    nextOptionIndex = self.Arguments.index(specifiedOptions[specifiedOptions.index(option)+1])
                    optionValues = self.Arguments[optionIndex+1:nextOptionIndex]
                else:
                    optionValues = self.Arguments[optionIndex+1:]
                for value in optionValues:
                    if value[0] == '@':
                        memberEntry.ExplicitPipe = value[1:]
                        if value[1:] == '':
                            memberEntry.ExplicitPipe = 'None'
                    else:
                        if memberType in self.BuiltinOptionTypes:
                            exec('castValue = '+memberType+'(\''+value+'\')', globals())
                            memberValues.append(castValue)
                        elif memberType == 'bool':
                            exec('castValue = int(\''+value+'\')', globals())
                            memberValues.append(castValue)
                        else:
                            memberValues.append(value)
            else:
                continue

            if memberLength != -1:
                if (len(memberValues) != memberLength) and not memberEntry.ExplicitPipe:
                    self.PrintError(self.GetUsageString() + '\n' + 'Error for option '+option+': '+str(len(memberValues))+' entries given, '+str(memberLength)+' expected.' + '\n')
                    return 0

            if len(memberValues) > 0:
                if (memberLength==0):
                    if (activated == 1):
                        setattr(self,memberName,1)
                        memberEntry.MemberValue = 1
                elif (memberLength==1):
                    setattr(self,memberName,memberValues[0])
                    memberEntry.MemberValue = memberValues[0]
                else:
                    setattr(self,memberName,memberValues)
                    memberEntry.MemberValue = memberValues

            if memberType == 'bool':
                if [value for value in memberValues if value not in [0,1]]:
                    self.PrintError(self.GetUsageString() + '\n' + 'Error for option '+option+': should be either 0 or 1' + '\n')
                    return 0

            if memberRange != '':
                if [value for value in memberValues if not memberEntry.IsInRange(value)]:
                    self.PrintError(self.GetUsageString() + '\n' + 'Error for option '+option+': should be ' + memberEntry.GetRangeRepresentation() + '\n')
                    return 0

            if (memberType != ''):
                if (memberLength==0):
                    if activated:
                        self.PrintLog(memberName + ' = ' + 'on',1)
                    else:
                        self.PrintLog(memberName + ' = ' + 'off',1)
                else:
                    if (memberLength==-1):
                        memberLength = len(memberValues)
                    if memberEntry.ExplicitPipe:
                        self.PrintLog(memberName+ ' = @' + memberEntry.ExplicitPipe,1)
                    else:
                        self.PrintLog(memberName+ ' = ' + str(self.__getattribute__(memberName)),1)
        return 1

    def IORead(self):
        try:
            from vmtk import pypes
        except ImportError:
            return None
        for member in self.InputMembers:
            if member.MemberIO:
                filename = eval('self.' + self.GetIOInputFileNameMember(member.MemberName))
                if filename:
                    try:
                        readerModule = importlib.import_module('vmtk.'+member.MemberIO)
                        # Find the principle class to instantiate the requested action defined inside the requested writerModule script.
                        # Returns a single member list (containing the principle class name) which satisfies the following criteria:
                        #   1) is a class defined within the script
                        #   2) the class is a subclass of pypes.pypescript
                        readerModuleClasses = [x for x in dir(readerModule) if isclass(getattr(readerModule, x)) and issubclass(getattr(readerModule, x), pypes.pypeScript)]
                        readerModuleClassName = readerModuleClasses[0]
                    except ImportError:
                        self.PrintError('Cannot import module ' + member.MemberIO + ' required for reading ' + member.MemberName)
                    reader = getattr(readerModule, readerModuleClassName)
                    reader = reader()
                    reader.InputFileName = filename
                    reader.LogOn = self.LogOn
                    reader.InputStream = self.InputStream
                    reader.OutputStream = self.OutputStream
                    reader.Execute()
                    exec('self.' + member.MemberName + ' = reader.Output')

    def IOWrite(self):
        try:
            from vmtk import pypes
        except ImportError:
            return None
        for member in self.OutputMembers:
            if member.MemberIO:
                filename = eval('self.' + self.GetIOOutputFileNameMember(member.MemberName))
                writerInput = eval('self.' + member.MemberName)
                if filename:
                    # Create code object representing local import of requested IO script
                    try:
                        writerModule = importlib.import_module('vmtk.'+member.MemberIO)
                        # Find the principle class to instantiate the requested action defined inside the requested writerModule script.
                        # Returns a single member list (containing the principle class name) which satisfies the following criteria:
                        #   1) is a class defined within the script
                        #   2) the class is a subclass of pypes.pypescript
                        writerModuleClasses = [x for x in dir(writerModule) if isclass(getattr(writerModule, x)) and issubclass(getattr(writerModule, x), pypes.pypeScript)]
                        writerModuleClassName = writerModuleClasses[0]
                    except ImportError:
                        self.PrintError('Cannot import module ' + member.MemberIO + ' required for writing ' + member.MemberIO)
                    writer = getattr(writerModule, writerModuleClassName)
                    writer = writer()
                    writer.Input = writerInput
                    writer.OutputFileName = filename
                    writer.LogOn = self.LogOn
                    writer.InputStream = self.InputStream
                    writer.OutputStream = self.OutputStream
                    writer.Execute()

    def GetIOInputFileNameMember(self,memberName):
        return memberName + 'InputFileName'

    def GetIOOutputFileNameMember(self,memberName):
        return memberName + 'OutputFileName'

    def GetIOFileNameOption(self,optionName):
        return optionName + 'file'

    def Execute(self):
        pass

    def Deallocate(self):
        pass


class pypeMain(object):

    def __init__(self):
        self.Arguments = None

    def Execute(self):
        from vmtk import pype
        pipe = pype.Pype()
        pipe.Arguments = self.Arguments
        pipe.ParseArguments()
        pipe.Execute()
