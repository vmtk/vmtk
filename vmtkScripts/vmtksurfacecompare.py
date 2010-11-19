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
        self.SetScriptDoc('compares a  mesh against a baseline')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input mesh','vmtksurfacereader'],
            ['ReferenceSurface','r','vtkPolyData',1,'','the input mesh','vmtksurfacereader'],
            ['Method','method','str',1,'["addarray","projection","distance"]','method of the test'],
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
        if not self.ReferenceSurface.GetPointData().GetArray(self.ArrayName):
            self.PrintError('Error: Invalid ArrayName.')
        if not self.Surface.GetPointData().GetArray(self.ArrayName):
            self.PrintError('Error: Invalid ArrayName.')

        referenceArrayName = 'Ref' + self.ArrayName
        surfacePoints = self.Surface.GetNumberOfPoints()
        referencePoints = self.ReferenceSurface.GetNumberOfPoints() 
        pointsDifference = surfacePoints - referencePoints
        calculator = vtk.vtkArrayCalculator() 

        if self.Method == 'addarray':
            if abs(pointsDifference) > 0:
                 self.ResultLog = 'Uneven NumberOfPoints'
                 return False
            refArray = self.ReferenceSurface.GetPointData().GetArray(self.ArrayName) 
            refArray.SetName(referenceArrayName) 
            self.Surface.GetPointData().AddArray(refArray)
            calculator.SetInput(self.Surface)
        elif self.Method == 'projection':
            self.ReferenceSurface.GetPointData().GetArray(self.ArrayName).SetName(referenceArrayName)
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
        resultRange = self.ResultData.GetPointData().GetArray('ResultArray').GetRange()

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

        if (self.Method == 'addarray'):
            self.Result = self.arrayCompare()
        if (self.Method == 'projection'):
            self.Result = self.arrayCompare()
        elif (self.Method == 'distance'):
            self.Result = self.distanceCompare()
        
if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
