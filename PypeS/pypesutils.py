import os
import sys
import textwrap
import string
import os.path

def all_indices(value, qlist):
    '''convenience method to a list of all indexes where an element is within a collection

    arguments:
        value (str, int, float): the item to find within the collection
        qlist (list, tuple): the collection of elements which you want to find the the indexes
            of value in.

    returns:
        indices (list:`int`): a list containing the index of every location where qlist == value.
    '''
    indices = []
    idx = -1
    while True:
        try:
            idx = qlist.index(value, idx+1)
            indices.append(idx)
        except ValueError:
            break
    return indices

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
    textwrapper = textwrap.TextWrapper()
    textwrapper.initial_indent = ''
    textwrapper.subsequent_indent = ' '
    usageString += textwrapper.fill(scriptUsageString)
    for memberList in [self.InputMembers, self.OutputMembers]:
        if memberList == self.InputMembers:
                usageString += '\n' + '  ' + 'Input arguments:'
        elif memberList == self.OutputMembers:
                usageString += '\n' + '  ' + 'Output arguments:'
        for memberEntry in memberList:
            memberUsageString = ''
            indentation = '   '
            textwrapper.initial_indent = indentation
            textwrapper.subsequent_indent = indentation + '  '    
            memberName  = memberEntry.MemberName
            option = memberEntry.OptionName
            memberType = memberEntry.MemberType
            memberLength = memberEntry.MemberLength
            memberRange = memberEntry.MemberRange
            memberDoc = memberEntry.MemberDoc
            if option == '':
                usageString += '\n' + textwrapper.fill(memberUsageString)
                continue
            if memberLength == 0:
                memberUsageString += '-' + option + ' ' + memberName
            else:
                memberUsageString += '-' + option + ' ' + memberName + ' (' + memberType + ',' + str(memberLength) + ')'
                if (memberRange) and (memberType in self.BuiltinOptionTypes):
                    memberUsageString += " " + memberEntry.GetRangeRepresentation()
                if (getattr(self, memberName) != '') and (memberType in self.BuiltinOptionTypes):
                    memberUsageString += '; default=' + str(getattr(self, memberName))
            if memberDoc != '':
                memberUsageString += ': ' + memberDoc
            usageString += '\n' + textwrapper.fill(memberUsageString)
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