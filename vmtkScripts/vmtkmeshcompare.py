#!/usr/bin/env python

import sys
import vtk
from vmtk import vtkvmtk
from vmtk import pypes

vmtkmeshcompare = 'vmtkMeshCompare'

class vmtkMeshCompare(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.ReferenceMesh = None
        self.Method = ''
	self.Tolerance = 1E-8
        self.Result = ''

        self.SetScriptName('vmtkmeshcompare')
        self.SetScriptDoc('compares a  mesh against a reference')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['ReferenceMesh','r','vtkUnstructuredGrid',1,'','the reference mesh to compare against','vmtkmeshreader'],
            ['Method','method','str',1,'["quality","other"]','method of the test'],
            ['Tolerance','tolerance','float',1,'','tolerance for numerical comparisons'],
            ])
        self.SetOutputMembers([
            ['Result','result','bool',1,'','Output boolean stating if meshes are equal or not']
            ])

    def qualityCompare(self):
        
        meshQuality = vtk.vtkMeshQuality()
        meshQuality.SetInput(self.Mesh)
        meshQuality.RatioOn()
        meshQuality.Update()
        meshQualityOutput = meshQuality.GetOutput()

        referenceQuality = vtk.vtkMeshQuality()
        referenceQuality.SetInput(self.ReferenceMesh)
        referenceQuality.RatioOn()
        referenceQuality.Update()
        referenceQualityOutput = referenceQuality.GetOutput()

        self.PrintLog("Mesh points: "+ str(meshQualityOutput.GetNumberOfPoints()) +"\nReference Points: " +str(referenceQualityOutput.GetNumberOfPoints()))
        meshQualityValue = 0.0
        referenceQualityValue = 0.0
        for i in range(meshQualityOutput.GetNumberOfPoints()):
            meshQualityValue += meshQualityOutput.GetCellData().GetArray("Quality").GetTuple1(i)
        meshQualityValue /= meshQualityOutput.GetNumberOfPoints()
        for i in range(referenceQualityOutput.GetNumberOfPoints()):
            referenceQualityValue += referenceQualityOutput.GetCellData().GetArray("Quality").GetTuple1(i)
        referenceQualityValue /= referenceQualityOutput.GetNumberOfPoints()
        diff = abs(meshQualityValue - referenceQualityValue)
        if diff > self.Tolerance:
            return False
            log = "failed"
  	self.PrintLog("mesh: "+ str(meshQualityValue) +" reference: "+ str(referenceQualityValue) + " diff: " + str(diff))

        return True

    def Execute(self):

        if not self.Mesh:
            self.PrintError('Error: No mesh.')
        if not self.ReferenceMesh:
            self.PrintError('Error: No reference.')
        if not self.Method:
            self.PrintError('Error: No method.')                 

        if (self.Method == 'quality'):
            self.Result = self.qualityCompare()
        elif (self.Method == 'other'):
            pass

        
if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
