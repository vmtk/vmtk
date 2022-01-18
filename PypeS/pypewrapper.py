#!/usr/bin/env python

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


from __future__ import print_function, absolute_import # NEED TO STAY AS TOP IMPORT
import sys
import os.path
from vmtk import pypes


class PypeWrapper(object):

    def __init__(self):

        self.Mode = 'slicer3'
        self.XMLDescription = ''
        self.PypeTitle = ''
        self.PypeDescription = ''
        self.Contributor = ''
        self.ModuleFileName = ''

        self.Arguments = None
        self.ScriptList = []
        self.ModulePipeArguments = []
        self.AllExposedMembers = []

        self.Indentation = '  '

    def ParseArguments(self):
        if '--help' in self.Arguments:
            print('hey!')
            return
        if '--pype' not in self.Arguments:
            print('complain!')
            return
        if '--mode' in self.Arguments:
            self.Mode = self.Arguments[self.Arguments.index('--mode')+1]
        if '--title' in self.Arguments:
            self.PypeTitle = self.Arguments[self.Arguments.index('--title')+1]
        if '--description' in self.Arguments:
            self.PypeDescription = self.Arguments[self.Arguments.index('--description')+1]
        if '--contributor' in self.Arguments:
            self.Contributor = self.Arguments[self.Arguments.index('--contributor')+1]
        if '--modulefile' in self.Arguments:
            self.ModuleFileName = self.Arguments[self.Arguments.index('--modulefile')+1]
        arguments = self.Arguments[self.Arguments.index('--pype')+1:]
        self.ModulePipeArguments = arguments[:]
        while '--pipe' in arguments:
            scriptSlice = arguments[:arguments.index('--pipe')]
            self.ScriptList.append([os.path.splitext(os.path.split(scriptSlice[0])[1])[0],scriptSlice[1:]])
            arguments = arguments[arguments.index('--pipe')+1:]
        scriptSlice = arguments[:]
        if not arguments:
            return
        self.ScriptList.append([os.path.splitext(os.path.split(scriptSlice[0])[1])[0],scriptSlice[1:]])

    def Execute(self):
        ind = self.Indentation
        self.XMLDescription = '<?xml version="1.0" encoding="utf-8"?>\n'
        self.XMLDescription += '<executable>\n'
        self.XMLDescription += ind + '<category>vmtk</category>\n'
        self.XMLDescription += ind + '<title>%s</title>\n' % (self.PypeTitle)
        self.XMLDescription += ind + '<description>%s</description>\n' % (self.PypeDescription)
        self.XMLDescription += ind + '<contributor>%s</contributor>\n' % (self.Contributor)

        self.AllExposedMembers = []
        for scriptNameAndArguments in self.ScriptList:
            self.XMLDescription += ind + '<parameters>\n'
            scriptName = scriptNameAndArguments[0]
            moduleName = scriptName
            scriptArguments = scriptNameAndArguments[1]
            try:
                exec('from vmtk import '+ moduleName)
            except ImportError:
                print('No module named ' + moduleName)
                break
            scriptObjectClassName = ''
            exec ('scriptObjectClassName =  '+moduleName+'.'+moduleName)
            self.XMLDescription += 2*ind + '<label>%s Parameters</label>\n' % (scriptObjectClassName)
            moduleScriptObjectClassName = moduleName+'.'+scriptObjectClassName
            scriptObject = 0
            exec ('scriptObject = '+moduleScriptObjectClassName+'()')
            scriptArguments = scriptNameAndArguments[1]
            exposedArgumentNames = [argument.split('@')[0] for argument in scriptArguments if '@' in argument[1:]]
            exposedArgumentChannels = [argument.split('@')[1] for argument in scriptArguments if '@' in argument[1:]]
            exposedArgumentOptions = [scriptArguments[scriptArguments.index(argument)-1][1:] for argument in scriptArguments if '@' in argument[1:]]
            exposedOptionsToNamesAndChannels = {}
            for i in range(len(exposedArgumentOptions)):
                exposedOptionsToNamesAndChannels[exposedArgumentOptions[i]] = [exposedArgumentNames[i], exposedArgumentChannels[i]]
            exposedMembers = []
            for member in scriptObject.InputMembers + scriptObject.OutputMembers:
                exec('member.MemberValue = scriptObject.'+member.MemberName)
                if member.OptionName in exposedOptionsToNamesAndChannels:
                    member.ExposedName = exposedOptionsToNamesAndChannels[member.OptionName][0]
                    member.ExposedChannel = exposedOptionsToNamesAndChannels[member.OptionName][1]
                    exposedMembers.append(member)
                    self.AllExposedMembers.append(member)
            for exposedMember in exposedMembers:
                memberXMLTag = ''
                memberXMLOptions = ''
                enumeration = exposedMember.GetRangeEnumeration()
                if exposedMember.MemberType == 'int':
                    memberXMLTag = 'integer'
                elif exposedMember.MemberType == 'float':
                    memberXMLTag = 'float'
                elif exposedMember.MemberType == 'str':
                    memberXMLTag = 'string'
                    if enumeration:
                        memberXMLTag += '-enumeration'
                elif exposedMember.MemberType == 'bool':
                    memberXMLTag = 'boolean'
                if exposedMember.MemberLength != 1:
                    memberXMLTag += '-vector'
                if exposedMember.MemberType == 'vtkImageData':
                    memberXMLTag = 'image'
                elif exposedMember.MemberType == 'vtkPolyData':
                    memberXMLTag = 'geometry'
                if exposedMember.ExposedChannel == 'point':
                    memberXMLTag = 'point'
                    if exposedMember.MemberLength == -1:
                        memberXMLOptions += 'multiple="true"'
                self.XMLDescription += 2*ind + '<%s>\n' % (memberXMLTag+' '+memberXMLOptions)
                self.XMLDescription += 3*ind + '<name>%s</name>\n' % (exposedMember.ExposedName)
                self.XMLDescription += 3*ind + '<longflag>%s</longflag>\n' % (exposedMember.ExposedName)
                self.XMLDescription += 3*ind + '<label>%s</label>\n' % (exposedMember.ExposedName)
                if exposedMember.MemberDoc:
                    self.XMLDescription += 3*ind + '<description>%s</description>\n' % (exposedMember.MemberDoc)
                if exposedMember.MemberValue not in [None, [], '']:
                    self.XMLDescription += 3*ind + '<default>%s</default>\n' % (str(exposedMember.MemberValue))
                if enumeration:
                    for element in enumeration:
                        self.XMLDescription += 3*ind + '<element>%s</element>\n' % (str(element))
                values = exposedMember.GetRangeValues()
                if values:
                    self.XMLDescription += 3*ind + '<constraints>\n'
                    if values[0] != None:
                        self.XMLDescription += 4*ind + '<minimum>%s</minimum>\n' % (str(values[0]))
                    if values[1] != None:
                        self.XMLDescription += 4*ind + '<maximum>%s</maximum>\n' % (str(values[1]))
                    if values[2] != None:
                        self.XMLDescription += 4*ind + '<step>%s</step>\n' % (str(values[2]))
                    self.XMLDescription += 3*ind + '</constraints>\n'
                if exposedMember.ExposedChannel in ['input','output']:
                    self.XMLDescription += 3*ind + '<channel>%s</channel>\n' % (exposedMember.ExposedChannel)
                self.XMLDescription += 2*ind + '</%s>\n' % (memberXMLTag)
            self.XMLDescription += ind + '</parameters>\n'
        self.XMLDescription += '</executable>\n'

        moduleFile = open(self.ModuleFileName,'w')
        moduleFile.write('#!/usr/bin/env python\n\n')
        moduleFile.write('xmlDescription = """')
        moduleFile.write(self.XMLDescription)
        moduleFile.write('"""\n')
        moduleFile.write('\n')
        moduleFile.write('pypeWrapperCommand = "%s"\n' % ' '.join(sys.argv))
        moduleFile.write('\n')
        moduleFile.write('import sys\n')
        moduleFile.write('if "--xml" in sys.argv:\n')
        moduleFile.write(self.Indentation+'print(xmlDescription\n)')
        moduleFile.write(self.Indentation+'sys.exit(0)\n')
        moduleFile.write('\n')
        moduleFile.write('if "--logo" in sys.argv:\n')
        moduleFile.write(self.Indentation+'sys.exit(0)\n')
        moduleFile.write('\n')
        moduleFile.write('import sys\n')
        moduleFile.write('if "--pypewrapper" in sys.argv:\n')
        moduleFile.write(self.Indentation+'print(pypeWrapperCommand\n)')
        moduleFile.write(self.Indentation+'sys.exit(0)\n')
        moduleFile.write('\n')

        substModulePipeArguments = []
        exposedMembersOrder = []
        for argument in self.ModulePipeArguments:
            if '@' in argument[1:]:
                substModulePipeArguments.append(argument.split('@')[0])
            else:
                substModulePipeArguments.append(argument)
        for exposedMember in self.AllExposedMembers:
            exposedMembersOrder.append(substModulePipeArguments.index(exposedMember.ExposedName))
            if exposedMember.ExposedChannel in ['input','output']:
                substModulePipeArguments[substModulePipeArguments.index(exposedMember.ExposedName)-1] += 'file'
            substModulePipeArguments[substModulePipeArguments.index(exposedMember.ExposedName)] = '%s'
        sortedExposedMembersOrder = exposedMembersOrder[:]
        sortedExposedMembersOrder.sort()
        allOrderedExposedMemberNames = []
        for position in sortedExposedMembersOrder:
            allOrderedExposedMemberNames.append(self.AllExposedMembers[exposedMembersOrder.index(position)].ExposedName)

        moduleFile.write('arguments = sys.argv[:]\n')
        moduleFile.write('\n')

        for exposedMember in self.AllExposedMembers:
            if exposedMember.MemberType == 'bool':
                moduleFile.write('%s = "0"\n' % exposedMember.ExposedName)
                moduleFile.write('if "--%s" in arguments:\n' % (exposedMember.ExposedName))
                moduleFile.write(self.Indentation+'%s = "1"\n' % (exposedMember.ExposedName))
                moduleFile.write(self.Indentation+'arguments.remove("--%s")\n' % exposedMember.ExposedName)
                moduleFile.write('%s = " ".join(%s.split(","))\n' % (exposedMember.ExposedName, exposedMember.ExposedName))
                moduleFile.write('\n')
            else:
                moduleFile.write('%s = ""\n' % exposedMember.ExposedName)
                moduleFile.write('while "--%s" in arguments:\n' % (exposedMember.ExposedName))
                moduleFile.write(self.Indentation+'index = arguments.index("--%s")\n' % (exposedMember.ExposedName))
                moduleFile.write(self.Indentation+'if index != len(arguments)-1 and "--" not in arguments[index+1]:\n')
                moduleFile.write(2*self.Indentation+'if %s:\n' % exposedMember.ExposedName)
                moduleFile.write(3*self.Indentation+'%s += ","\n' % exposedMember.ExposedName)
                moduleFile.write(2*self.Indentation+'%s += arguments[index+1]\n' % exposedMember.ExposedName)
                moduleFile.write(2*self.Indentation+'arguments.remove(arguments[index+1])\n')
                moduleFile.write(self.Indentation+'arguments.remove("--%s")\n' % exposedMember.ExposedName)
                moduleFile.write('%s = " ".join(%s.split(","))\n' % (exposedMember.ExposedName, exposedMember.ExposedName))
                moduleFile.write('\n')

        moduleFile.write('pipe = "%s" %% (%s)\n' % (' '.join(substModulePipeArguments),','.join(allOrderedExposedMemberNames)))

        moduleFile.write('\n')
        moduleFile.write('from vmtk import pypes\n')
        moduleFile.write('pypes.PypeRun(pipe)\n')
        moduleFile.write('\n')

        moduleFile.close()


if __name__=='__main__':
    pipeLumper = PypeWrapper()
    pipeLumper.Arguments = sys.argv
    pipeLumper.ParseArguments()
    pipeLumper.Execute()
