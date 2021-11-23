## Program: VMTK
## Language:  Python
## Date:      January 10, 2018
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga, All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this code was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo

import pytest
import sys
from vmtk import pype


def test_init():
    pipe = pype.Pype()
    assert pipe.ScriptName == 'pype'


def test_get_usage_string():
    pipe = pype.Pype()
    assert pipe.GetUsageString() ==  'Usage: pype --nolog --noauto --query firstScriptName -scriptOptionName scriptOptionValue --pipe secondScriptName -scriptOptionName scriptOptionValue -scriptOptionName @firstScriptName.scriptOptionName -id 2 --pipe thirdScriptName -scriptOptionName @secondScriptName-2.scriptOptionName'


def test_set_argument_string_two_args():
    pipe = pype.Pype()
    arg_string = 'vmtkimagereader --help'
    pipe.SetArgumentsString(arg_string)
    assert pipe.Arguments == ['vmtkimagereader', '--help']


def test_set_argument_string_five_args():
    pipe = pype.Pype()
    arg_string = 'vmtkimagereader --help 0123 vmtkimageviewer fixit'
    pipe.SetArgumentsString(arg_string)
    assert pipe.Arguments == ['vmtkimagereader', '--help', '0123', 'vmtkimageviewer', 'fixit']


@pytest.mark.xfail(raises=RuntimeError)
def test_fail_set_argument_string_nonmatching_quote():
    pipe = pype.Pype()
    arg_string = 'vmtkimagereader --help"'
    pipe.SetArgumentsString(arg_string)


def test_parse_arguments_one_function():
    pipe = pype.Pype()
    pipe.Arguments = ['vmtkimagereader', '-ifile', 'test.vtp']
    pipe.ParseArguments()
    assert pipe.ScriptList == [['vmtkimagereader', ['-ifile', 'test.vtp']]]


def test_parse_arguments_two_functions():
    pipe = pype.Pype()
    pipe.Arguments = ['vmtkimagereader', '-ifile', 'test.vtp', '--pipe', 'vmtkimageviewer']
    pipe.ParseArguments()
    assert pipe.ScriptList == [['vmtkimagereader', ['-ifile', 'test.vtp']], ['vmtkimageviewer', []]]


def test_parse_arguments_two_functions_with_text():
    pipe = pype.Pype()
    pipe.Arguments = ['vmtkimagereader', '-ifile', 'test.vtp', '--pipe', 'vmtkimageviewer', '-ofile', 'test.vti']
    pipe.ParseArguments()
    assert pipe.ScriptList == [['vmtkimagereader', ['-ifile', 'test.vtp']], ['vmtkimageviewer', ['-ofile', 'test.vti']]]
