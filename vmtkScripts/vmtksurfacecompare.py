#!/usr/bin/env python

import sys
import vtk
import vtkvmtk
import pypes
import vmtkscripts


vmtksurfacecompare = 'vmtkSurfaceCompare'

class vmtkSurfaceCompare(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.ReferenceSurface = None
        self.Method = ''
        self.ArrayName = ''
	self.Tolerance = 1E-8
        self.Result = ''
        self.ResultLog = ''
        self.ResultData = None

        self.SetScriptName('vmtksurfacecompare')
        self.SetScriptDoc('compares a  surface against a baseline')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['ReferenceSurface','r','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['Method','method','str',1,'["addpointarray","addcellarray","projection","distance"]','method of the test'],
            ['ArrayName','array','str',1,'','name of the array'],
            ['Tolerance','tolerance','float',1,'','tolerance for numerical comparisons'],
            ])
        self.SetOutputMembers([
            ['Result','result','bool',1,'','Output boolean stating if surfaces are equal or not'],
            ['ResultData','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ['ResultLog','log','str',1,'','Result Log']
            ])

    def arrayCompare(self):

        if not self.ArrayName:
            self.PrintError('Error: No ArrayName.') 

        attributeData = None
        referenceAttributeData = None
        calculator = vtk.vtkArrayCalculator() 

        if self.Method in ['addpointarray','projection']:
            attributeData = self.Surface.GetPointData()
            referenceAttributeData = self.ReferenceSurface.GetPointData()
            calculator.SetAttributeModeToUsePointData()
        elif self.Method in ['addcellarray']:
            attributeData = self.Surface.GetCellData()
            referenceAttributeData = self.ReferenceSurface.GetCellData()
            calculator.SetAttributeModeToUseCellData()

        if not attributeData.GetArray(self.ArrayName):
            self.PrintError('Error: Invalid ArrayName.')
        if not referenceAttributeData.GetArray(self.ArrayName):
            self.PrintError('Error: Invalid ArrayName.')

        referenceArrayName = 'Ref' + self.ArrayName
        surfacePoints = self.Surface.GetNumberOfPoints()
        referencePoints = self.ReferenceSurface.GetNumberOfPoints() 
        pointsDifference = surfacePoints - referencePoints

        if self.Method in ['addpointarray','addcellarray']:
            if abs(pointsDifference) > 0:
                 self.ResultLog = 'Uneven NumberOfPoints'
                 return False
            refArray = referenceAttributeData.GetArray(self.ArrayName) 
            refArray.SetName(referenceArrayName) 
            attributeData.AddArray(refArray)
            calculator.SetInput(self.Surface)
        elif self.Method in ['projection']:
            referenceAttributeData.GetArray(self.ArrayName).SetName(referenceArrayName)
            projection = vmtkscripts.vmtkSurfaceProjection()
            projection.Surface = self.Surface
            projection.ReferenceSurface = self.ReferenceSurface
            projection.Execute()
            calculator.SetInput(projection.Surface)

        calculator.AddScalarVariable('a',self.ArrayName,0)
        calculator.AddScalarVariable('b',referenceArrayName,0)
        calculator.SetFunction("a - b") 
        calculator.SetResultArrayName('ResultArray')
        calculator.Update()

        self.ResultData = calculator.GetOutput()

        if self.Method in ['addpointarray','projection']:
            resultRange = self.ResultData.GetPointData().GetArray('ResultArray').GetRange()
        elif self.Method in ['addcellarray']:
            resultRange = self.ResultData.GetCellData().GetArray('ResultArray').GetRange() 

        self.PrintLog('Result Range: ' + str(resultRange))

        if max([abs(r) for r in resultRange]) < self.Tolerance:
            return True

        return False

    def distanceCompare(self):

        distance = vmtkscripts.vmtkSurfaceDistance()
        distance.Surface = self.Surface
        distance.ReferenceSurface = self.ReferenceSurface
        distance.DistanceArrayName = 'Distance'
        distance.Execute()
        
        distanceRange = distance.Surface.GetPointData().GetArray('Distance').GetRange()

        self.PrintLog('Distance Range: ' + str(distanceRange))
        
        if max(distanceRange) < self.Tolerance:
            return True        
 
        return False

    def Execute(self):

        if not self.Surface:
            self.PrintError('Error: No Surface.')
        if not self.ReferenceSurface:
            self.PrintError('Error: No Reference Surface.')
        if not self.Method:
            self.PrintError('Error: No method.')                 

        if self.Method in ['addpointarray','addcellarray','projection']:
            self.Result = self.arrayCompare()
        elif self.Method == 'distance':
            self.Result = self.distanceCompare()
        
if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
