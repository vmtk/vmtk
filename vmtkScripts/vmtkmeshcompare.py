#!/usr/bin/env python

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import sys
import vtk
from vmtk import vtkvmtk
from vmtk import pypes


class vmtkMeshCompare(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.ReferenceMesh = None
        self.Method = ''
        self.ArrayName = ''
        self.Tolerance = 1E-8
        self.Result = ''
        self.ResultLog = ''
        self.ResultData = None

        self.SetScriptName('vmtkmeshcompare')
        self.SetScriptDoc('compares a  mesh against a reference')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['ReferenceMesh','r','vtkUnstructuredGrid',1,'','the reference mesh to compare against','vmtkmeshreader'],
            ['Method','method','str',1,'["quality","pointarray","cellarray"]','method of the test'],
            ['ArrayName','array','str',1,'','name of the array'],
            ['Tolerance','tolerance','float',1,'','tolerance for numerical comparisons'],
            ])
        self.SetOutputMembers([
            ['Result','result','bool',1,'','Output boolean stating if meshes are equal or not'],
            ['ResultData','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter'],
            ['ResultLog','log','str',1,'','Result Log']
            ])

    def arrayCompare(self):

        calculator = vtk.vtkArrayCalculator()

        attributeData = None
        referenceAttributeData = None
        if self.Method == 'pointarray':
            attributeData = self.Mesh.GetPointData()
            referenceAttributeData = self.ReferenceMesh.GetPointData()
            if vtk.vtkVersion.GetVTKMajorVersion()>=9 or (vtk.vtkVersion.GetVTKMajorVersion()>=8 and vtk.vtkVersion.GetVTKMinorVersion()>=1):
                calculator.SetAttributeTypeToPointData()
            else:
                calculator.SetAttributeModeToUsePointData()
        elif self.Method == 'cellarray':
            attributeData = self.Mesh.GetCellData()
            referenceAttributeData = self.ReferenceMesh.GetCellData()
            if vtk.vtkVersion.GetVTKMajorVersion()>=9 or (vtk.vtkVersion.GetVTKMajorVersion()>=8 and vtk.vtkVersion.GetVTKMinorVersion()>=1):
                calculator.SetAttributeTypeToCellData()
            else:
                calculator.SetAttributeModeToUseCellData()

        if not self.ArrayName:
            self.PrintError('Error: No ArrayName.')
        if not referenceAttributeData.GetArray(self.ArrayName):
            self.PrintError('Error: Invalid ArrayName.')
        if not attributeData.GetArray(self.ArrayName):
            self.PrintError('Error: Invalid ArrayName.')

        referenceArrayName = 'Ref' + self.ArrayName
        meshPoints = self.Mesh.GetNumberOfPoints()
        referencePoints = self.ReferenceMesh.GetNumberOfPoints()
        pointsDifference = meshPoints - referencePoints

        self.PrintLog("Mesh points: "+ str(meshPoints))
        self.PrintLog("Reference Points: " +str(referencePoints))

        if abs(pointsDifference) > 0:
            self.ResultLog = 'Uneven NumberOfPoints'
            return False

        refArray = referenceAttributeData.GetArray(self.ArrayName)
        refArray.SetName(referenceArrayName)
        attributeData.AddArray(refArray)

        calculator.SetInputData(self.Mesh)
        calculator.AddScalarVariable('a',self.ArrayName,0)
        calculator.AddScalarVariable('b',referenceArrayName,0)
        calculator.SetFunction("a - b")
        calculator.SetResultArrayName('ResultArray')
        calculator.Update()

        self.ResultData = calculator.GetOutput()
        if self.Method == 'pointarray':
            resultRange = self.ResultData.GetPointData().GetArray('ResultArray').GetRange()
        elif self.Method == 'cellarray':
            resultRange = self.ResultData.GetCellData().GetArray('ResultArray').GetRange()

        self.PrintLog('Result Range: ' + str(resultRange))

        if max([abs(r) for r in resultRange]) < self.Tolerance:
            return True

        return False

    def qualityCompare(self):

        meshQuality = vtk.vtkMeshQuality()
        meshQuality.SetInputData(self.Mesh)
        meshQuality.RatioOn()
        meshQuality.Update()
        meshQualityOutput = meshQuality.GetOutput()

        referenceQuality = vtk.vtkMeshQuality()
        referenceQuality.SetInputData(self.ReferenceMesh)
        referenceQuality.RatioOn()
        referenceQuality.Update()
        referenceQualityOutput = referenceQuality.GetOutput()

        self.PrintLog("Mesh points: "+ str(meshQualityOutput.GetNumberOfPoints()))
        self.PrintLog("Reference Points: " +str(referenceQualityOutput.GetNumberOfPoints()))

        meshQualityRange = meshQualityOutput.GetCellData().GetArray("Quality").GetRange()
        referenceQualityRange = referenceQualityOutput.GetCellData().GetArray("Quality").GetRange()
        qualityRangeDifference = (meshQualityRange[0] - referenceQualityRange[0],meshQualityRange[1] - referenceQualityRange[1])

        self.PrintLog("Mesh Quality Range: "+ str(meshQualityRange))
        self.PrintLog("Reference Quality Range: "+ str(referenceQualityRange))
        self.PrintLog("Quality Range Difference: "+ str(qualityRangeDifference))

        if max(abs(d) for d in qualityRangeDifference) < self.Tolerance:
            return True

        return False

    def Execute(self):

        if not self.Mesh:
            self.PrintError('Error: No mesh.')
        if not self.ReferenceMesh:
            self.PrintError('Error: No reference.')
        if not self.Method:
            self.PrintError('Error: No method.')

        if self.Method == 'quality':
            self.Result = self.qualityCompare()
        elif self.Method in ['pointarray','cellarray']:
            self.Result = self.arrayCompare()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
