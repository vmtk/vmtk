## Program: VMTK
## Language:  Python
## Date:      January 10, 2018
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga, All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this code was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo

import pytest
import sys
from vmtk import pypes

def test_init():
    pipe = pypes.Pype()
    assert pipe.ScriptName == 'pype'

def test_set_argument_string_two_args():
    pipe = pypes.Pype()
    arg_string = 'vmtkimagereader --help'
    pipe.SetArgumentsString(arg_string)
    assert pipe.Arguments == ['vmtkimagereader', '--help']


def test_set_argument_string_five_args():
    pipe = pypes.Pype()
    arg_string = 'vmtkimagereader --help 0123 vmtkimageviewer fixit'
    pipe.SetArgumentsString(arg_string)
    assert pipe.Arguments == ['vmtkimagereader', '--help', '0123', 'vmtkimageviewer', 'fixit']


def test_fail_set_argument_string_nonmatching_quote():
    with pytest.raises(SystemExit) as pytest_wrapped_e:
        pipe = pypes.Pype()
        arg_string = 'vmtkimagereader --help"'
        pipe.SetArgumentsString(arg_string)
    assert pytest_wrapped_e.type == SystemExit


def test_parse_arguments_one_function():
    pipe = pypes.Pype()
    pipe.Arguments = ['vmtkimagereader', '-ifile', 'test.vtp']
    pipe.ParseArguments()
    assert pipe.ScriptList == [['vmtkimagereader', ['-ifile', 'test.vtp']]]


def test_parse_arguments_two_functions():
    pipe = pypes.Pype()
    pipe.Arguments = ['vmtkimagereader', '-ifile', 'test.vtp', '--pipe', 'vmtkimageviewer']
    pipe.ParseArguments()
    assert pipe.ScriptList == [['vmtkimagereader', ['-ifile', 'test.vtp']], ['vmtkimageviewer', []]]


def test_parse_arguments_two_functions_with_text():
    pipe = pypes.Pype()
    pipe.Arguments = ['vmtkimagereader', '-ifile', 'test.vtp', '--pipe', 'vmtkimageviewer', '-ofile', 'test.vti']
    pipe.ParseArguments()
    assert pipe.ScriptList == [['vmtkimagereader', ['-ifile', 'test.vtp']], ['vmtkimageviewer', ['-ofile', 'test.vti']]]

