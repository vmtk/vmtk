#!/usr/bin/env python
# -*- coding: utf-8 -*-

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
        '''pype member object created from specification in pypeScript.SetInputMembers of SetOutputMembers

        note: default arguments of memberRange, memberDoc, and memberIO are = '' (aka. an empty string.)
        when treated as a "truthiness" value, empty strings return false (aka. eval(not '') >> True )

        Attributes:
            MemberName (str): Formal parameter name of the member within the script
            OptionName (str): Parameter name used to specify the value on the command line
            MemberType (str): Type of the object stored by the member.
            MemberLength (int): Home many values the parameter will store (in case of a list), or -1 if
                no limit on length is specified
            MemberRange (str, optional): a string which either specifies a list of values that must be selected
                [identified by brackets], or a string which specifies the upper and lower bounds of the member
                (identified by parentheses).
            MemberDoc (str, optional): a short summary of the parameter to print as help text
            MemberIO (str, optional): the name of the script which should be used as the default reader/writer.
        '''

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


    def _GetRangeEnumeration(self):
        '''Returns the valid range specified in script input members if valid range is part of a list

        returns:
            obj:'list': returns empty list if MemberRange is numeric value type. Otherwise return list
                of strings containing valid members if MemberRange is enumeration Type
        '''
        if not self.MemberRange:
            return []

        if self.MemberRange[0] == '[' and self.MemberRange[-1] == ']':
            parsedMemberRange = [eval(entry) for entry in self.MemberRange[1:-1].split(',')]
            return parsedMemberRange
        else:
            return []

    def _GetRangeValues(self):
        '''returns the valid range specified in the script input members if valid range in a numeric value.

        returns:
            obj:`list`: returns empty list if MemberRange is enumeration type. Otherwise returns list of
                size 2 where index 0 is the lower range bound and index 1 is upper range bound. If only
                one bound is specified, the element will be None.
        '''
        if not self.MemberRange:
            return []

        if self.MemberRange[0] == '(' and self.MemberRange[-1] == ')':
            parsedMemberRange = []
            for entry in self.MemberRange[1:-1].split(','):
                if entry != '':
                    parsedMemberRange.append(float(entry))
                else:
                    parsedMemberRange.append(None)
            if len(parsedMemberRange) != 2:
                # this essentially is a validity check for the input.
                # if the entry is mis-formatted, then just return an empty list
                return []
            return parsedMemberRange
        else:
            return []

    def GetRangeRepresentation(self):
        '''returns the valid range specified in the script input members

        This function is only called when trying to print the '--help' text
        of a script.

        In this context 'range enumeration' refers to a parameter which
        needs to match some value in a list. This is typically a string (ex: for
        vmtkimagemorphology, the Operation parameter contains a range enumeration
        of ['"dilate"', '"erode"', '"open"', '"close"'])

        Conversely, 'range value' refers to an integer or float value which much be
        contained within some bounding range (ex: for vmtkimagemorphology, the
        BallRadius parameter must be >= 0.0)

        returns:
            str: text containing the correct formatting for either a range enumeration
                or range value.
        '''
        if not self.MemberRange:
            return []
        enumeration = self._GetRangeEnumeration()
        values = self._GetRangeValues()
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

    def IsInRange(self,value):
        '''validates weather the option value is valid for the script member specified range.

        This method first checks for case where specific option is selected from a list of potentials.
        This is denoted by [brackets] surrounding the member range argument. Then it checks for case
        where option is a numeric type which needs to be in a particular range. This is denoted by
        (parentheses) surrounding the member range argument. Note that the numeric range can specify
        both an upper & lower bound, or just one bound. The truthiness will be returned correctly
        in either case.

        arguments:
            value (str, int, float): the parameter value which was set

        returns:
            boolean: true if valid, false if not.
        '''
        if not self.MemberRange:
            return

        parsedMemberRangeEnumeration = self._GetRangeEnumeration()
        parsedMemberRangeValues = self._GetRangeValues()
        if (not parsedMemberRangeEnumeration) and (not parsedMemberRangeValues):
            return False

        if value in parsedMemberRangeEnumeration:
            return True
        if parsedMemberRangeValues:
            if (parsedMemberRangeValues[0] is not None) and (value < parsedMemberRangeValues[0]):
                return False
            elif (parsedMemberRangeValues[1] is not None) and (value > parsedMemberRangeValues[1]):
                return False
            else:
                return True

        return False


class pypeScript(object):
    '''the base class for every high-level script.

    It manages parsing, instantiates proper input/output methods for the
    script and keeps the script structure consistent. Each PypeScript is at
    the same time:
       - a script which can be called from the command line and piped to other scripts
       - a class which can be called from Python code (e.g. inside a tkinter GUI)

    Methods:
        Print
    '''

    lastVisitedPath = '.'

    def __init__(self):

        self.BuiltinOptionTypes = ['int','str','float','bool'] # do not change?

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
        '''send logs to the selected output stream

        arguments:
            logMessage (str): text, or printable object, to send to output stream
            indent (int): indent level for the output text
        '''
        if not self.LogOn:
            return
        indentUnit = '    '
        indentation = ''
        for i in range(indent):
            indentation = indentation + indentUnit
        self.OutputStream.write(indentation + logMessage + '\n')

    def PrintError(self,errorMessage):
        '''raise runtime error and print log message to selected output stream.

        arguments:
            errorMessage (str): text, or printable object, to send to input stream.
        '''
        self.OutputStream.write(errorMessage + '\n')
        if self.ExitOnError:
          self.Exit()
        else:
          raise RuntimeError(errorMessage)

    def Exit(self):
        '''gracefully exit python and clean up held memory'''
        sys.exit()

    def OutputText(self,text):
        '''print message to output stream'''
        self.OutputStream.write(text)

    def OutputProgress(self,progress,percentStep):
        '''print progress of script operations which support progress polling

        arguments:
            progress (float): a value between 0 and 1 indicating
                the percentage complete to print.
            percentStep (int): a value between 1 and 99 specifying the interval
                between successive progress reports.
        '''
        if int(100 * progress)/percentStep == int(100 * self.Progress)/percentStep:
            return
        self.Progress = progress
        self.OutputStream.write('\r')
        self.OutputStream.write('Progress: '+str(int(100 * self.Progress))+'%')
        self.OutputStream.flush()
        self.InputInfo('Progress: '+str(int(100 * self.Progress))+'%')

    def EndProgress(self):
        '''method called when finished reporting algorithm progress to output stream.'''
        self.OutputStream.write('\n')

    def InputInfo(self,prompt=''):
        '''print an informational message to the input stream.

        This is typically used to alerts a user to a required action.

        arguments:
            prompt (str): message to print to the selected input stream.
        '''
        self.OutputText(prompt)
        try:
            self.InputStream.prompt(prompt,info=True)
        except:
            pass

    def InputText(self,prompt='',validator=None):
        '''prompts a user to input something on the keyboard and returns the input.

        arguments:
            prompt (str): the text to show to the user.
            validator (obj:`bool`, optional): defaults to None. A method attached to
                the script object instance which accepts a string and returns true
                or false if the input is valid or not

        returns:
            str: the user input casted to a string type.
        '''
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

    def _PrintMembers(self,members):
        '''given a list of pypemember objects, print the name and value to the output stream

        this method is called by PrintInputMembers and PrintOutputMember methods.

        arguments:
            members (list:'obj'): a list containing instances of pypeMember objects which have
                already been instantiated
        '''
        for memberEntry in members:
            memberName  = memberEntry.MemberName
            memberType = memberEntry.MemberType
            memberValue = self.__getattribute__(memberName)

            if memberName == 'Self':
                pass
            elif (memberType in self.BuiltinOptionTypes) or (memberType == 'list'):
                self.__getattribute__("PrintLog")(memberName+' = '+str(memberValue),1)
            elif memberValue:
                self.__getattribute__("PrintLog")(memberName+' = '+memberType,1)
            else:
                self.__getattribute__("PrintLog")(memberName+' = '+str(memberValue),1)

    def PrintInputMembers(self):
        '''prints script object input member names and values to output stream'''
        self.PrintLog("Input " + self.ScriptName + " members:")
        self._PrintMembers(self.InputMembers)

    def PrintOutputMembers(self):
        '''prints script object output member names and values to output stream'''
        self.PrintLog("Output " + self.ScriptName + " members:")
        self._PrintMembers(self.OutputMembers)

    def _ConvertToPypeMembers(self,member):
        '''creates a pypeMember instance of the input member list

        arguments:
            members (obj:`list`): a list containing pype member specification.

        returns:
            (obj): a pypeMember object
        '''
        if type(member) == pypeMember:
            return member
        elif type(member) == list:
            return pypeMember(*member)
        else:
            self.PrintError("Pype Member Specified By: "+ str(member) + " not valid expected type (obj or list)")

    def SetInputMembers(self,membersToAppend):
        '''creates a pypemember object instance for each input member specification

        Each pypemember object instance is stored as an element in the self.InputMembers list.

        arguments:
            membersToAppend (obj:'list':obj:'list'): a list of lists. Each list represents
                and describes an Input Member and will have in general 6 elements with a
                last 7th optional one. This first element is the name of the variable we
                are gonna use as a member. The second element This is the command line text
                that we use to refer to this member. The third element is the variable type
                of the element. The fourth element is This is how many elements our member will
                have. There are cases where we consider as a member an array or a list of elements.
                In case we don’t want to limit the number of elements for a member we write -1.
                The fifth is the validity check of our member. In case the type is a string,
                one can specify a list of valid strings (['stringA','stringB']), if it’s a number,
                one can specify the valid range ( (0.0,) or (1.0,100.0) ). The sixth is a short
                description of this particular member. This text will be shown when the –help
                option is invoked. The optional seventh argument denotes the script that should be
                used to read (for InputMembers) the associated data. This will automatically lead
                to the creation of an additional option at the command line with the same option
                name suffixed by ‘file’ (in this case, -ifile).

        example:
            membersToAppend = [['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
                               ['Levels','levels','float',-1,'','graylevels to generate the isosurface at']]
        '''
        for pypeMemberObject in membersToAppend:
            pypeMemberObject = self._ConvertToPypeMembers(pypeMemberObject)
            self.InputMembers.append(pypeMemberObject)
            if pypeMemberObject.MemberIO:
                filenameMember = pypeMember(self.GetIOInputFileNameMember(pypeMemberObject.MemberName),
                                            self.GetIOFileNameOption(pypeMemberObject.OptionName),
                                            'str', 1, '',
                                            'filename for the default ' + pypeMemberObject.MemberName + ' reader')
                setattr(self, str(filenameMember.MemberName), '')
                self.InputMembers.append(filenameMember)

    def SetOutputMembers(self,membersToAppend):
        '''creates a pypemember object instance for each output member specification

        Each pypemember object instance is stored as an element in the self.OutputMembers list.

        arguments:
            membersToAppend (obj:'list':obj:'list'): a list of lists. Each list represents
                and describes an Output Member and will have in general 6 elements with a
                last 7th optional one. This first element is the name of the variable we
                are gonna use as a member. The second element This is the command line text
                that we use to refer to this member. The third element is the variable type
                of the element. The fourth element is This is how many elements our member will
                have. There are cases where we consider as a member an array or a list of elements.
                In case we don’t want to limit the number of elements for a member we write -1.
                The fourth is This is the validity check of our member. In case the type is a string,
                one can specify a list of valid strings (['stringA','stringB']), if it’s a number,
                one can specify the valid range ( (0.0,) or (1.0,100.0) ). The fifth is a short
                description of this particular member. This text will be shown when the –help
                option is invoked. The optional seventh argument denotes the script that should be
                used to write (for OutputMembers) the associated data. This will automatically
                lead to the creation of an additional option at the command line with the same
                option name suffixed by ‘file’ (in this case, -ofile).

        example:
            membersToAppend = [['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']]
        '''
        for pypeMemberObject in membersToAppend:
            pypeMemberObject = self._ConvertToPypeMembers(pypeMemberObject)
            self.OutputMembers.append(pypeMemberObject)
            if pypeMemberObject.MemberIO:
                filenameMember = pypeMember(self.GetIOOutputFileNameMember(pypeMemberObject.MemberName),
                                            self.GetIOFileNameOption(pypeMemberObject.OptionName),
                                            'str', 1, '',
                                            'filename for the default ' + pypeMemberObject.MemberName + ' writer')
                setattr(self, str(filenameMember.MemberName), '')
                self.InputMembers.append(filenameMember)

    def SetScriptName(self,scriptName):
        '''sets the script object ScriptName attribute'''
        self.ScriptName = scriptName

    def SetScriptDoc(self,scriptDoc):
        '''sets the script object ScriptDoc attribute'''
        self.ScriptDoc = scriptDoc

    def GetScriptDocString(self):
        '''returns the scriptobject ScriptDoc attribute value'''
        return self.ScriptDoc

    def GetUsageString(self):
        '''get a plain text string describing useful information about the current script object

        returns:
            a formatted string which contains the current ScriptName, ScriptDoc, and Input/Output member
                fields (name, option, type, length, range, doc, default, and help text)
        '''
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
                    elif memberType in self.BuiltinOptionTypes:
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
        '''get a html formatted tagged string describing useful information about the current script object

        returns:
            A formatted string which contains the current ScriptName, ScriptDoc, and Input/Output member fields
                (name, option, type, length, range, doc, default, and help text)
        '''
        usageString = ''
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
                    elif memberType in self.BuiltinOptionTypes:
                        default = self.__getattribute__(memberName)
                        memberUsageString += '<td>' + option + '</td><td>' + memberName + '</td><td>' + memberType + '</td><td>' + str(memberLength) + '</td><td>' + str(memberRange) + '</td><td>' + str(default) + '</td>'
                    else:
                        memberUsageString += '<td>' + option + '</td><td>' + memberName + '</td><td>' + memberType + '</td><td>' + str(memberLength) + '</td><td></td><td></td>'
                    if not memberDoc:
                        memberDoc = ''
                    memberUsageString += '<td>' + memberDoc + '</td>'
                memberUsageString += '\n'
                usageString += '<tr>' + memberUsageString + '</tr>' + '\n'
            usageString += '</table>\n'
        usageString += '\n'
        return usageString


    def _ParseArgumentsFlags(self):
        '''execute special case argument flag behavior

        check if --help --doc --html flags exist in the arguments list. if they do then execute the
        special behavior indicated by those flags.

        Also check to ensure that the options specified by -foo options exist with an appropriate member
        with matching format to what is expected.
        '''
        for arg in self.Arguments:
            if arg == '--help':
                self.PrintLog('')
                self.OutputText(self.GetUsageString())
                self.PrintLog('')
                return False
            if arg == '--doc':
                self.PrintLog('')
                self.OutputText(self.GetScriptDocString())
                self.PrintLog('')
                return False
            if arg == '--html':
                self.PrintLog('')
                self.OutputText(self.GetHTMLUsageString())
                self.PrintLog('')
                return False
            if (arg[0] == '-') & (len(arg)==1):
                self.PrintError(self.GetUsageString() + '\n' + self.ScriptName + ' error: unknown option ' + arg + '\n')
                return False
            if (arg[0] == '-'):
                if (arg[1] in string.ascii_letters):
                    matchingMembers = [member for member in self.InputMembers if member.OptionName in [arg.lstrip('-'), arg.lstrip('-').rstrip('@')]]
                    if not matchingMembers:
                        self.PrintError(self.GetUsageString() + '\n' + self.ScriptName + ' error: unknown option ' + arg + '\n')
                        return False
        return True

    def _CheckMemberValuesLength(self, memberValues, memberLength, memberEntry, option):
        '''ensure that member values have the expected length
        '''
        if memberLength != -1:
            if (len(memberValues) != memberLength) and not memberEntry.ExplicitPipe:
                self.PrintError(self.GetUsageString() + '\n' + 'Error for option '+option+': '+str(len(memberValues))+' entries given, '+str(memberLength)+' expected.' + '\n')

    def _CheckMemberValuesBool(self, memberValues, option, memberType):
        '''ensure that bool types have the correct value'''
        if memberType is 'bool':
            if [value for value in memberValues if value not in [0,1]]:
                self.PrintError(self.GetUsageString() + '\n' + 'Error for option '+option+': should be either 0 or 1' + '\n')

    def _CheckMemberValuesInRange(self, memberValues, memberEntry, option, memberRange):
        '''ensure numeric values have the correct range'''
        if memberRange != '':
            if [value for value in memberValues if not memberEntry.IsInRange(value)]:
                self.PrintError(self.GetUsageString() + '\n' + 'Error for option '+option+': should be ' + memberEntry.GetRangeRepresentation() + '\n')

    def _PrintMemberTypeInformation(self, memberType, memberLength, activated, memberName, memberValues, memberEntry):
        '''print useful type information about the member object'''
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

    def ParseArguments(self):
        '''Set script object values from specified arguments

        TODO:
            finish refactor
            finish docstring
            self.Arguments: list of option names and args.
                ie: self.Arguments = ['-ifile', './aorta.mha', '-flip', '1', '0', '1']
        '''
        ParseArgumentStopper = self._ParseArgumentsFlags()
        if not ParseArgumentStopper:
            return False
        for memberEntry in self.InputMembers:
            memberName  = memberEntry.MemberName
            option = '-' + memberEntry.OptionName
            pushedOption = option + '@'
            memberType = memberEntry.MemberType
            memberLength = memberEntry.MemberLength
            memberRange = memberEntry.MemberRange
            memberValues = []
            activated = 0
            
            specifiedOptions = []
            for arg in self.Arguments:
                 if (arg[0] == '-') and (arg[1] in string.ascii_letters + '-'):
                     specifiedOptions.append(arg)
            if pushedOption in specifiedOptions:
                # replace pushed option input text to regular option name that compatible input members can be found.
                # example: ['-id', '-radiusfactor@'] -> ['-id', '-radiusfactor']
                memberEntry.Pushed = 1
                specifiedOptions[specifiedOptions.index(pushedOption)] = option
                self.Arguments[self.Arguments.index(pushedOption)] = option
            if option not in specifiedOptions:
                continue

            optionIndex = self.Arguments.index(option)
            if option != specifiedOptions[-1]:
                nextOptionIndex = self.Arguments.index(specifiedOptions[specifiedOptions.index(option)+1])
                optionValues = self.Arguments[optionIndex+1:nextOptionIndex]
            else:
                optionValues = self.Arguments[optionIndex+1:]

            for value in optionValues:
                if value.startswith('@'):
                    memberEntry.ExplicitPipe = value[1:]
                    if value[1:] == '':
                        memberEntry.ExplicitPipe = 'None'
                    continue
                if memberType.lower() in self.BuiltinOptionTypes:
                    if memberType.lower() == 'str':
                        castValue = str(value)
                    elif (memberType.lower() == 'int') or (memberType.lower() == 'bool'):
                        castValue = int(value)
                    elif memberType.lower() == 'float':
                        castValue = float(value)
                    memberValues.append(castValue)
                else: #TODO: Does this ever run? 
                    memberValues.append(value)

            self._CheckMemberValuesLength(memberValues, memberLength, memberEntry, option)

            if len(memberValues) > 0:
                if memberLength == 0:
                    activated = 1
                    setattr(self, memberName, 1)
                    memberEntry.MemberValue = 1
                elif memberLength == 1:
                    setattr(self, memberName, memberValues[0])
                    memberEntry.MemberValue = memberValues[0]
                else:
                    setattr(self,memberName,memberValues)
                    memberEntry.MemberValue = memberValues

            self._CheckMemberValuesBool(memberValues, option, memberType)
            self._CheckMemberValuesInRange(memberValues, memberEntry, option, memberRange)
            self._PrintMemberTypeInformation(memberType, memberLength, activated, memberName, memberValues, memberEntry)
        return True

    def IORead(self):
        '''read a file from disk if the members default reader script is specified'''
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
                    setattr(self, str(member.MemberName), reader.Output)

    def IOWrite(self):
        '''write a object to disk if the members default writer script is specified'''
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

