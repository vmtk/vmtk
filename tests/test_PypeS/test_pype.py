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
import vtk
from vmtk import pypes

def test_init():
    pipe = pypes.Pype()
    assert pipe.ScriptName == 'pype'

#---------------------------------------------------
# Test PrintLog Method
#---------------------------------------------------
def test_print_log_no_indent(capsys):
    pipe = pypes.Pype()
    pipe.PrintLog('testing no indent')
    captured = capsys.readouterr()
    assert captured.out == 'testing no indent\n'


def test_print_log_one_indent(capsys):
    pipe = pypes.Pype()
    pipe.PrintLog('testing one indent', 1)
    captured = capsys.readouterr()
    assert captured.out == '    testing one indent\n'


def test_print_log_set_logon_off(capsys):
    pipe = pypes.Pype()
    pipe.LogOn = 0
    pipe.PrintLog('this should not be printed')
    captured = capsys.readouterr()
    assert captured.out == ''

#---------------------------------------------------
# Test SetArgumentString Method
#---------------------------------------------------

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


def test_set_argument_string_matching_double_quote_no_space_in_file_name():
    pipe = pypes.Pype()
    arg_string = 'vmtkimagereader -ifile "/foo/anotherdirectory/bar.vti" --pipe vmtkimageviewer'
    pipe.SetArgumentsString(arg_string)
    assert pipe.Arguments == ['vmtkimagereader', '-ifile', '/foo/anotherdirectory/bar.vti', '--pipe', 'vmtkimageviewer']


def test_set_argument_string_matching_double_quote_space_in_file_name():
    pipe = pypes.Pype()
    arg_string = 'vmtkimagereader -ifile "/foo/another directory/bar.vti" --pipe vmtkimageviewer'
    pipe.SetArgumentsString(arg_string)
    assert pipe.Arguments == ['vmtkimagereader', '-ifile', '/foo/another directory/bar.vti', '--pipe', 'vmtkimageviewer']


def test_fail_set_argument_string_nonmatching_double_quote(capsys):
    with pytest.raises(SystemExit) as pytest_wrapped_e:
        pipe = pypes.Pype()
        arg_string = 'vmtkimagereader --help"'
        pipe.SetArgumentsString(arg_string)
    assert pytest_wrapped_e.type == SystemExit
    captured = capsys.readouterr()
    # need newline on comparison since pype.PrintError automatically includes it. 
    assert captured.out == 'Error: non-matching quote found\n'


def test_set_argument_string_no_quote_space_in_file_name():
    # this is not the behavior we want long term, since it split up the file name, but the set argument string
    # method is not responsible for catching that
    pipe = pypes.Pype()
    arg_string = 'vmtkimagereader -ifile /foo/another directory/bar.vti --pipe vmtkimageviewer'
    pipe.SetArgumentsString(arg_string)
    assert pipe.Arguments == ['vmtkimagereader', '-ifile', '/foo/another', 'directory/bar.vti', '--pipe', 'vmtkimageviewer']


def test_set_argument_string_replace_single_brace_variable_with_list():
    pipe = pypes.Pype()
    kwargs = {'flip': [0, 0, 1]}
    arg_string = 'vmtkimagereader -ifile /foo/anotherdirectory/bar.vti -flip {flip} --pipe vmtkimageviewer'
    pipe.SetArgumentsString(arg_string, **kwargs)
    assert pipe.Arguments == ['vmtkimagereader', '-ifile', '/foo/anotherdirectory/bar.vti', '-flip', [0, 0, 1], '--pipe', 'vmtkimageviewer']


def test_set_argument_string_replace_single_brace_variable_with_number():
    pipe = pypes.Pype()
    kwargs = {'filedimensionality': 2}
    arg_string = 'vmtkimagereader -ifile /foo/anotherdirectory/bar.vti -filedimensionality {filedimensionality} --pipe vmtkimageviewer'
    pipe.SetArgumentsString(arg_string, **kwargs)
    assert pipe.Arguments == ['vmtkimagereader', '-ifile', '/foo/anotherdirectory/bar.vti', '-filedimensionality', 2, '--pipe', 'vmtkimageviewer']


def test_set_argument_string_replace_single_brace_variable_with_string():
    pipe = pypes.Pype()
    kwargs = {'filename': '/foo/anotherdirectory/bar.vti'}
    arg_string = 'vmtkimagereader -ifile {filename} --pipe vmtkimageviewer'
    pipe.SetArgumentsString(arg_string, **kwargs)
    assert pipe.Arguments == ['vmtkimagereader', '-ifile', '/foo/anotherdirectory/bar.vti', '--pipe', 'vmtkimageviewer']


def test_set_argument_string_replace_single_brace_variable_with_vtkImageData():
    pipe = pypes.Pype()
    imageData = vtk.vtkImageData()
    kwargs = {'image': imageData}
    arg_string = 'vmtkimageviewer -i {image}'
    pipe.SetArgumentsString(arg_string, **kwargs)
    assert pipe.Arguments == ['vmtkimageviewer', '-i', imageData]


def test_set_argument_string_replace_multiple_brace_variables():
    pipe = pypes.Pype()
    imageData = vtk.vtkImageData()
    kwargs = {'flip': [0, 0, 1],
              'filename': '/foo/anotherdirectory/bar.vti',
              'filedimensionality': 3,
              'image': imageData}
    arg_string = 'vmtkimagereader -ifile {filename} -flip {flip} -filedimensionality {filedimensionality} --pipe vmtkimageviewer -i {image}'
    pipe.SetArgumentsString(arg_string, **kwargs)
    assert pipe.Arguments == ['vmtkimagereader', '-ifile', '/foo/anotherdirectory/bar.vti', '-flip', [0, 0, 1], '-filedimensionality', 3, '--pipe', 'vmtkimageviewer', '-i', imageData]

#---------------------------------------------------
# Test ParseArguments Method
#---------------------------------------------------

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



def test_parse_pyperun_arguments_no_pyperun_method():
    pipe = pypes.Pype()
    arguments =  ['vmtkimagereader', '-ifile', 'test.vtp', '--pipe', 'vmtkimageviewer', '-ofile', 'test.vti']
    parsedArguments = pipe._ParsePyperunControlingArgumentFlags(arguments)
    assert parsedArguments == arguments

#---------------------------------------------------
# Test _ParsePyperunControlingArgumentFlags Method
#---------------------------------------------------

def test_parse_pyperun_arguments_help_flag(capsys):
    pipe = pypes.Pype()
    arguments =  ['pyperun', '--help', 'vmtkimagereader', '-ifile', 'test.vtp', '--pipe', 'vmtkimageviewer', '-ofile', 'test.vti']
    parsedArguments = pipe._ParsePyperunControlingArgumentFlags(arguments)
    assert parsedArguments == ['vmtkimagereader', '-ifile', 'test.vtp', '--pipe', 'vmtkimageviewer', '-ofile', 'test.vti']
    captured = capsys.readouterr()
    assert captured.out == 'Usage: pype --nolog --noauto firstScriptName -scriptOptionName '\
                            'scriptOptionValue --pipe secondScriptName -scriptOptionName scriptOptionValue '\
                            '-scriptOptionName @firstScriptName.scriptOptionName -id 2 --pipe thirdScriptName '\
                            '-scriptOptionName @secondScriptName-2.scriptOptionName\n'


def test_parse_pyperun_arguments_nolog_flag(capsys):
    pipe = pypes.Pype()
    arguments =  ['pyperun', '--nolog', 'vmtkimagereader', '-ifile', 'test.vtp', '--pipe', 'vmtkimageviewer', '-ofile', 'test.vti']
    parsedArguments = pipe._ParsePyperunControlingArgumentFlags(arguments)
    assert parsedArguments == ['vmtkimagereader', '-ifile', 'test.vtp', '--pipe', 'vmtkimageviewer', '-ofile', 'test.vti']
    assert pipe.LogOn == 0


def test_parse_pyperun_arguments_noauto_flag(capsys):
    pipe = pypes.Pype()
    arguments =  ['pyperun', '--noauto', 'vmtkimagereader', '-ifile', 'test.vtp', '--pipe', 'vmtkimageviewer', '-ofile', 'test.vti']
    parsedArguments = pipe._ParsePyperunControlingArgumentFlags(arguments)
    assert parsedArguments == ['vmtkimagereader', '-ifile', 'test.vtp', '--pipe', 'vmtkimageviewer', '-ofile', 'test.vti']
    assert pipe.AutoPipe == 0