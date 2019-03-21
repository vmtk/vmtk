#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceclipper.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.9 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by 
##       Marco Fedele (marco.fedele@polimi.it)
##       Politecnico di Milano

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import vmtkrenderer
from vmtk import pypes

class vmtkSurfaceThickness(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.CellEntityIdsArray = None
        self.ThicknessArrayName = 'Thickness'
        self.ThicknessArray = None
        self.InternalWallEntityIds = []
        self.ExternalWallEntityIds = []

        self.SetScriptName('vmtksurfacethickness')
        self.SetScriptDoc('compute local thickness of a structure as distance between the internal and the external walls (exploiting their entity ids)')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'','name of the array where the tags are stored'],
            ['ThicknessArrayName','thicknessarray','str',1,'','name of the array with which to define the boundary between tags'],
            ['InternalWallEntityIds','internalwallids','int',-1,'','entity ids on the internal wall of the structure'],
            ['ExternalWallEntityIds','externalwallids','int',-1,'','entity ids on the external wall of the structure']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ['CellEntityIdsArray','oentityidsarray','vtkIntArray',1,'','the output entity ids array']
            ])


    def Execute(self):
        from vmtk import vmtkscripts

        if self.Surface == None:
            self.PrintError('Error: No Surface.')

        if self.InternalWallEntityIds == [] or self.ExternalWallEntityIds == []:
            self.PrintError('Error: Empty Internal/External Wall EntityIds.')

        th = vtk.vtkThreshold()
        th.SetInputData(self.Surface)
        th.SetInputArrayToProcess(0,0,0,1,self.CellEntityIdsArrayName)

        def extractWall(ids):
            appendFilter = vtk.vtkAppendPolyData()
            wallParts = []
            for i,item in enumerate(ids):
                th.ThresholdBetween(item-0.5,item+0.5)
                th.Update()
                gf = vtk.vtkGeometryFilter()
                gf.SetInputConnection(th.GetOutputPort())
                gf.Update()
                wallParts.append(gf.GetOutput())
                appendFilter.AddInputData(wallParts[i])
            appendFilter.Update()
            return appendFilter.GetOutput()

        internalWall = extractWall(self.InternalWallEntityIds)
        externalWall = extractWall(self.ExternalWallEntityIds)
        bothWalls = extractWall(self.InternalWallEntityIds+self.ExternalWallEntityIds)

        def ComputeDistance(surface,referenceSurface):
            distance = vmtkscripts.vmtkSurfaceImplicitDistance()
            distance.Surface = surface
            distance.ReferenceSurface = referenceSurface
            distance.CellData = 0
            distance.ComputeSignedDistance = 0
            distance.Execute()
            return distance.Array

        internalWallDistanceArray = ComputeDistance(bothWalls,internalWall)
        externalWallDistanceArray = ComputeDistance(bothWalls,externalWall)

        numberOfTuple = bothWalls.GetNumberOfPoints()
        thicknessArray = vtk.vtkDoubleArray()
        thicknessArray.SetName(self.ThicknessArrayName)
        thicknessArray.SetNumberOfComponents(1)
        thicknessArray.SetNumberOfTuples(numberOfTuple)
        for i in range(numberOfTuple):
            d1 = internalWallDistanceArray.GetComponent(i,0)
            d2 = externalWallDistanceArray.GetComponent(i,0)
            thicknessArray.SetComponent(i,0,max(d1,d2))
        bothWalls.GetPointData().AddArray(thicknessArray)

        # project the thickness also in regions outside the two walls (e.g. caps)
        # WARNING: it works only with PointData
        surfaceCopy = vtk.vtkPolyData()
        surfaceCopy.DeepCopy(self.Surface)

        surfaceProjection = vmtkscripts.vmtkSurfaceProjection()
        surfaceProjection.Surface = surfaceCopy
        surfaceProjection.ReferenceSurface = bothWalls
        surfaceProjection.Execute()
        surfaceCopy = surfaceProjection.Surface

        self.Surface.GetPointData().AddArray(surfaceCopy.GetPointData().GetArray(self.ThicknessArrayName))



if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()