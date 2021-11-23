#!${PYTHON_SHEBANG}

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes

pypetest = 'pypeTest'


class pypeTest(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.TestName = None
        self.PypeTestLog = ''
        self.TestInput = None
        self.Condition = None
        self.ConditionValue = None
        self.ConditionType = None

        self.SetScriptName('pypetest')
        self.SetScriptDoc('tests a script property against a condition')
        self.SetInputMembers([
            ['TestName','name','str',1,'','name of the test'],
            ['TestInput','i','test',1,'','log'],
            ['Condition','condition','str',1,'["equalto","differentfrom","greaterthan","lessthan","nonnone"]','condition type'],
            ['ConditionValue','value','str',1,'','condition value'],
            ['ConditionType','type','str',1,'["str","int","float","bool"]','condition type']
            ])
        self.SetOutputMembers([
            ['PypeTestLog','log','str',1,'','log']
            ])

    def castValue(self,val):
        if not self.ConditionType:
            self.PrintError('Error: No condition type.')
        if self.ConditionType == 'str':
            return str(val)
        elif self.ConditionType == 'int':
            return int(val)
        elif self.ConditionType == 'float':
            return float(val)
        elif self.ConditionType == 'bool':
            if val == True or val in ['1','True','true']:
                return True
            else:
                return False

    def Execute(self):

        self.PrintLog('Testing')

        if not self.TestName:
            self.PrintError('Error: No test name.')
        if self.TestInput == None:
            self.PrintError('Error: No test input.')
        if not self.Condition:
            self.PrintError('Error: No condition.')

        passed = False
        self.CompareLog = 'failed'

        if self.Condition == 'equalto':
            if self.ConditionValue and self.castValue(self.TestInput) == self.castValue(self.ConditionValue):
                passed = True
        elif self.Condition == 'differentfrom':
            if self.ConditionValue and self.castValue(self.TestInput) != self.castValue(self.ConditionValue):
                passed = True
        elif self.Condition == 'greaterthan':
            if self.ConditionValue and self.castValue(self.TestInput) > self.castValue(self.ConditionValue):
                passed = True
        elif self.Condition == 'lessthan':
            if self.ConditionValue and self.castValue(self.TestInput) < self.castValue(self.ConditionValue):
                passed = True
        elif self.Condition == 'nonnone':
            if self.TestInput != None :
                passed = True

        if passed:
            self.CompareLog = 'passed'

        self.PypeTestLog = "%s.%s" % (self.TestName, self.CompareLog)


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
