#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkdistancetocenterlines.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:48:31 $
## Version:   $Revision: 1.5 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
from vmtk import vtkvmtk
import sys

from vmtk import pypes


class vmtkDistanceToCenterlines(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Centerlines = None
        self.UseRadiusInformation = 0
        self.EvaluateTubeFunction = 0
        self.EvaluateCenterlineRadius = 0
        self.UseCombinedDistance = 0
        self.ProjectPointArrays = 0
        self.DistanceToCenterlinesArrayName = 'DistanceToCenterlines'
        self.RadiusArrayName = 'MaximumInscribedSphereRadius'

        self.SetScriptName('vmtkdistancetocenterlines')
        self.SetScriptDoc('calculate the minimum euclidian from surface points to a centerline')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','','vmtksurfacereader'],
            ['Centerlines','centerlines','vtkPolyData',1,'','','vmtksurfacereader'],
            ['UseRadiusInformation','useradius','bool',1],
            ['EvaluateTubeFunction','tubefunction','bool',1],
            ['EvaluateCenterlineRadius','centerlineradius','bool',1],
            ['UseCombinedDistance','combined','bool',1,'','combines local radius with maximum inscribed sphere radius'],
            ['ProjectPointArrays','projectarrays','bool',1],
            ['DistanceToCenterlinesArrayName','distancetocenterlinesarray','str',1],
            ['RadiusArrayName','radiusarray','str',1],
            ['UseRadiusThreshold','useradiusthreshold','bool',0],
            ['RadiusThreshold','radiusthreshold','float',1,'(0.0,)','set radius threshold']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        if self.UseCombinedDistance == 1:
            if self.RadiusArrayName == '':
                self.PrintError('Error: CenterlineRadiusArrayName not set.')
            distanceToCenterlinesFilter = vtkvmtk.vtkvmtkPolyDataDistanceToCenterlines()
            distanceToCenterlinesFilter.SetInputData(self.Surface)
            distanceToCenterlinesFilter.SetCenterlines(self.Centerlines)
            distanceToCenterlinesFilter.SetUseRadiusInformation(1)
            distanceToCenterlinesFilter.SetEvaluateCenterlineRadius(1)
            distanceToCenterlinesFilter.SetProjectPointArrays(self.ProjectPointArrays)
            distanceToCenterlinesFilter.SetDistanceToCenterlinesArrayName(self.DistanceToCenterlinesArrayName)
            distanceToCenterlinesFilter.SetCenterlineRadiusArrayName(self.RadiusArrayName)
            distanceToCenterlinesFilter.Update()

            surface = distanceToCenterlinesFilter.GetOutput()
            centerlineArray = surface.GetPointData().GetArray(self.DistanceToCenterlinesArrayName)
            radiusArray = surface.GetPointData().GetArray(self.RadiusArrayName)

            for i in range (surface.GetNumberOfPoints()):
                centerlineval = centerlineArray.GetComponent(i,0)
                radius = radiusArray.GetComponent(i,0)
                if centerlineval > 1.4 * radius:
                    centerlineArray.SetTuple1(i,1.4 * radius)
                elif centerlineval < 0.9 * radius:
                    centerlineArray.SetTuple1(i,radius)

            self.Surface = surface

        else:
            distanceToCenterlinesFilter = vtkvmtk.vtkvmtkPolyDataDistanceToCenterlines()
            distanceToCenterlinesFilter.SetInputData(self.Surface)
            distanceToCenterlinesFilter.SetCenterlines(self.Centerlines)
            distanceToCenterlinesFilter.SetUseRadiusInformation(self.UseRadiusInformation)
            distanceToCenterlinesFilter.SetEvaluateTubeFunction(self.EvaluateTubeFunction)
            distanceToCenterlinesFilter.SetEvaluateCenterlineRadius(self.EvaluateCenterlineRadius)
            distanceToCenterlinesFilter.SetProjectPointArrays(self.ProjectPointArrays)
            distanceToCenterlinesFilter.SetDistanceToCenterlinesArrayName(self.DistanceToCenterlinesArrayName)
            distanceToCenterlinesFilter.SetCenterlineRadiusArrayName(self.RadiusArrayName)
            distanceToCenterlinesFilter.Update()

            surface = distanceToCenterlinesFilter.GetOutput()

            if (self.UseRadiusThreshold):

                centerlineArray = surface.GetPointData().GetArray(self.DistanceToCenterlinesArrayName)
                radiusArray = surface.GetPointData().GetArray(self.RadiusArrayName)

                for i in range (surface.GetNumberOfPoints()):
                    centerlineval = centerlineArray.GetComponent(i,0)
                    if (centerlineval > self.RadiusThreshold):
                        centerlineArray.SetTuple1(i, self.RadiusThreshold)

            self.Surface = surface



if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
