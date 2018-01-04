import pytest
from vmtk import pype

def test_init():
    pipe = pype.Pype()
    assert pipe.ScriptName == 'pype'

def test__get_usage_string():
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

