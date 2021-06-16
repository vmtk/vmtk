#!/usr/bin/env python
# -*- coding: utf-8 -*-

## Program:   vmtk
## Module:    $RCSfile: vmtkinteractive.py,v $
## Language:  Python
## Date:      $Date: 2018/04/02 10:45:42 $
## Version:   $Revision: 1.4 $

##   Copyright (c) Richard Izzo, Luca Antiga. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

'''
This module allows for the use of vmtkscripts in an interactive python environment
(ie. Jupyter Notebook). When importing vmtk, only the 'run' method symbol will be exported.
Other functions within this module provide for parsing of input arguments, access to the
local variables of the calling scope, and connection of the vmtk.run method to the
broader pypes library.
'''

import sys
import os
import inspect
from .pypes.pype import PypeRun
from .pypes.pypesutils import all_indices
from types import SimpleNamespace
from functools import wraps

__all__ = ['run']

def locals_to_kwargs(original_function):
    '''decorator to capture f-string variables of calling scope as kwargs for the PypeRun operation

    This is designed to enable the execution of traditional pype commands within a jupyter notebook.
    It identifies the characters enclosed within {curly braces}, and searches the local variables of
    the calling scope for their values. These values are then inserted into a dictionary with the same
    key name, and passed as kwargs to the decorated function.

    This is used in order to simplify the user experience. Instead of having to explicitly pass python
    objects into the pype, they can just name them within the pype specification string.

    This decorator also handles checking for valid class objects and variables identified by the names
    in {curly braces}. It will raise an error if a python object with the specified name is not alive
    in the calling scope.
    '''
    @wraps(original_function)
    def wrapper_func(arguments, **kwargs):
        '''create the kwarg dictionary of the input argument string mapped to locals of the calling frame'''
        frame = inspect.currentframe()
        callingLocals = frame.f_back.f_locals
        startIndices = all_indices('{', arguments)
        endIndices = all_indices('}', arguments)
        try:
            assert len(startIndices) == len(endIndices)
        except AssertionError as e:
            print('ERROR: Unbalanced curly braces if argument string')
            raise e
        zippedIndices = zip(startIndices, endIndices)

        keywordDict = {}
        for startIdx, endIdx in zippedIndices:
            # select the name which lies between the curly braces in the arguments string
            variableName = arguments[startIdx+1:endIdx]
            # if passing in a pyperun object (ex: foo.vmtkimagereader.OutputMembers.Image)
            if '.' in variableName:
                # split the variableName string into a list defining the hierarchy of
                # attributes to access.
                # ex: foo.vmtkimagereader.OutputMembers.Image ->
                #   ['foo', 'vmtkimagereader', 'OutputMembers', 'Image']
                objectAttributeList = variableName.split('.')
                kwargObject = callingLocals[objectAttributeList[0]]
                # iterate through list of nested attribute names to get the desired
                # value associated with the object attribute
                for idx, varNamePath in enumerate(objectAttributeList[1:]):
                    try:
                        # replace the current instance of kwargObject with an instance which is
                        # one attribute further along in the chain.
                        kwargObject = getattr(kwargObject, varNamePath)
                    except AttributeError as error:
                        print("specified python Object: ", '.'.join(objectAttributeList[:idx+1]),
                              " contains no attribute: ", varNamePath)
                        raise error
                keywordDict[variableName] = kwargObject
            # if passing in normal variable name (not a nested class hierarchy)
            # (ex: variableName -> flip = [0, 1, 0])
            else:
                try:
                    keywordDict[variableName] = callingLocals[variableName]
                except KeyError as error:
                    print("specified variable name: ", variableName,
                          " not an 'alive' python object in the calling scope.")
                    raise error
        return original_function(arguments, **keywordDict)
    return wrapper_func


def _vmtk_get(vmtkRunInstance):
    '''extract all script objects/members from a pype instance into a nested python namespace

    For every script object within the pype instance, create a namespace. Assign input and output
    members of each script object to the namespace object. This is returned to the user.
    '''
    # top level dictionary holds a name referring to each vmtkscript instance which was run
    # ex: a pype of vmtk.run('vmtkimagereader -ifile foo --pipe vmtkimageviewer')
    #     will have a top level scriptDict with keys: 'vmtkimagereader' and 'vmtkimageviewer'
    scriptDict = {}
    for scriptObject in vmtkRunInstance.ScriptObjectList:
        scriptName = scriptObject.ScriptName
        # create a nested dictionary structure under each top level vmtkscript with keys 'InputMember'
        # and 'OutputMembers'. Do NOT change these names. If the output of one vmtk.run instance is
        # sent into the input of another, the locals_to_kwargs decorator needs to be able to access
        # the pypeScript InputMember and OutputMembers class attribute.
        scriptDict[scriptName] = {}
        scriptDict[scriptName]['InputMembers'] = {}
        scriptDict[scriptName]['OutputMembers'] = {}
        # assign key/value pari asthird level nested dictionary for each script input and output member.
        for inputMember in scriptObject.InputMembers:
            inputMemberName = inputMember.MemberName
            if inputMemberName == 'Self':
                continue
            inputMemberValue = getattr(scriptObject, inputMemberName)
            scriptDict[scriptName]['InputMembers'][inputMemberName] = inputMemberValue
        for outputMember in scriptObject.OutputMembers:
            outputMemberName = outputMember.MemberName
            if outputMemberName == 'Self':
                continue
            outputMemberValue = getattr(scriptObject, outputMemberName)
            scriptDict[scriptName]['OutputMembers'][outputMemberName] = outputMemberValue
        # we mutate the scriptDict in place, creating a nested structure of simple namespace classes
        # by unpacking the scriptDict from the bottom up.
        scriptDict[scriptName]['InputMembers'] = SimpleNamespace(**scriptDict[scriptName]['InputMembers'])
        scriptDict[scriptName]['OutputMembers'] = SimpleNamespace(**scriptDict[scriptName]['OutputMembers'])
        scriptDict[scriptName] = SimpleNamespace(**scriptDict[scriptName])
    # one more level of unpacking is needed for every top level vmtkscript in the scriptDict
    return SimpleNamespace(**scriptDict)


@locals_to_kwargs
def run(arguments, **kwargs):
    '''run a pype script within an interactive python session (Jupyter Notebook)

    The function is designed to be called with a similar syntax to a typical pype script.
    vtkData, variables, and other parameters can be passed within the pype script by enclosing
    them with {curly braces}. If these variables are defined prior to calling this function, they
    will be transparently placed into the correct location in the pype.

    This returns an object which has attributes named for each member in the input script. Input and
    Output members can be accessed as nested attributes

    Example:
    >> filename = '~/foo.stl'
    >> level = 700.0
    >>
    >> bar = vmtk.run('vmtkimagereader -ifile {filename} --pipe vmtkmarchingcubes -l {level} --pipe vmtksurfaceviewer -i @.o')
    >> surface = bar.vmtkmarchingcubes.OutputMembers.Surface
    >>
    >> bat = vmtk.run('vmtkcenterlines -i {surface} --pipe vmtkcenterlineviewer')
    >> centerlines = bat.vmtkcenterlines.OutputMembers.Centerlines
    >> voronoiDiagram = bat.vmtkCenterlines.OutputMembesr.VoronoiDiagram
    '''
    pypeRunInstance = PypeRun(arguments, **kwargs)
    return _vmtk_get(pypeRunInstance)
