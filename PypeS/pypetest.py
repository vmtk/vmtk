#!/usr/bin/env python

import vtk
import sys

from vmtk import pypes 

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

    def meshHandler(self):

        log = 'passed'
        reader = vtk.vtkXMLUnstructuredGridReader()
        reader.SetFileName(self.BaselineFileName)
        reader.Update()
        baseline = reader.GetOutput()

        meshQuality = vtk.vtkMeshQuality()
        meshQuality.SetInput(self.Mesh)
        meshQuality.RatioOn()
        meshQuality.Update()
        meshQualityOutput = meshQuality.GetOutput()

        baselineQuality = vtk.vtkMeshQuality()
        baselineQuality.SetInput(baseline)
        baselineQuality.RatioOn()
        baselineQuality.Update()
        baselineQualityOutput = baselineQuality.GetOutput()

        self.PrintLog("Mesh points: "+ str(meshQualityOutput.GetNumberOfPoints()) +"\nBaseline Points: " +str(baselineQualityOutput.GetNumberOfPoints()))
        meshQualityValue = 0.0
        baselineQualityValue = 0.0
        for i in range(meshQualityOutput.GetNumberOfPoints()):
            meshQualityValue += meshQualityOutput.GetCellData().GetArray("Quality").GetTuple1(i)
        meshQualityValue /= meshQualityOutput.GetNumberOfPoints()
        for i in range(baselineQualityOutput.GetNumberOfPoints()):
            baselineQualityValue += baselineQualityOutput.GetCellData().GetArray("Quality").GetTuple1(i)
        baselineQualityValue /= baselineQualityOutput.GetNumberOfPoints()
        diff = abs(meshQualityValue - baselineQualityValue)
        if diff > self.Tolerance:
            log = "failed"
  	self.PrintLog("mesh: "+ str(meshQualityValue) +" baseline: "+ str(baselineQualityValue) + " diff: " + str(diff))

        return log

    def imageHandler(self):

        log = 'failed'
        return log

    def surfaceHandler(self):

        log = 'failed'
        return log

    def CompareData(self):

        self.PrintLog('Comparing Data')
        log = 'failed'
        if self.Mesh:
            log = self.meshHandler()
        elif self.Image:
            log = self.imageHandler()
        elif self.Surface:
            log = self.surfaceHandler()
        else:
            self.PrintError('Error: No data.')
        return log

    def Execute(self):

        self.PrintLog('Testing')

        if not self.BaselineFileName:
            self.PrintError('Error: No baseline.')
        if not self.TestName:
            self.PrintError('Error: No test name.')

        self.PypeTestLog = "%s.%s" % (self.TestName, self.CompareData())

if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
