#!/usr/bin/env python

import sys

from vmtk import pypes 
from vmtk import vmtkscripts 

pypetest = 'pypeTest'

class pypeTest(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Image = None
        self.Surface = None
        self.Mesh = None
        self.TestName = None
        self.BaselineFileName = None
        self.Method = None
	self.Tolerance = 1E-8
        self.PypeTestLog = ''

        self.SetScriptName('pypetest')
        self.SetScriptDoc('tests a pype output against a baseline')
        self.SetInputMembers([
            ['Image','image','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['Surface','surface','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['Mesh','mesh','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['TestName','name','str',1,'','name of the test'],
            ['Method','method','str',1,'','method of the test'],
            ['Tolerance','tolerance','float',1,'','tolerance for numerical comparisons'],
            ['BaselineFileName','baselinefile','str',1,'','baseline filename'],
            ])
        self.SetOutputMembers([
            ['PypeTestLog','log','str',1,'','log']
            ])

    def CompareData(self):

        self.PrintLog('Comparing Data')
        log = 'FAILED'
        return log

    def Execute(self):

        self.PrintLog('Testing')

        if self.BaselineFileName == None:
            self.PrintError('Error: No baseline.')
        if self.TestName == None:
            self.PrintError('Error: No test name.')

        self.PypeTestLog = "%s.%s" % (self.TestName, self.CompareData())


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
