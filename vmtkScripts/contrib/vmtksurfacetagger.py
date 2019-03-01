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

class vmtkSurfaceTagger(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.ClippedSurface = None
        self.CutLines = None

        self.CleanOutput = 1

        self.InsideOut = 0

        self.ClipArrayName = None
        self.ClipArray = None
        self.ClipValue = 0.0
        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.CellEntityIdsArray = None
        self.InsideTag = 1
        self.OutsideTag = 2
        self.OverwriteOutsideTag = 1
        self.Connectivity = 0
        self.ConnectivityOffset = 1

        self.SetScriptName('vmtksurfacetagger')
        self.SetScriptDoc('tag a surface exploiting an array defined on it')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['CleanOutput','cleanoutput','bool',1,'','toggle cleaning the unused points'],
            ['InsideOut','insideout','bool',1,'','toggle switching inside and outside tags'],
            ['ClipArrayName','array','str',1,'','name of the array with which to define the boundary between tags'],
            ['ClipValue','value','float',1,'','scalar value at which to perform clipping between tags'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'','name of the array where the tags are stored'],
            ['InsideTag','inside','int',1,'','tag inside the clip (i.e. where the ClipArray is lower than ClipValue)'],
            ['OutsideTag','outside','int',1,'','tag outside the clip (i.e. where the ClipArray is greater than ClipValue)'],
            ['OverwriteOutsideTag','overwriteoutside','bool',1,'','overwrite outside value also when the CellEntityIdsArray already exists in the input surface'],
            ['Connectivity','connectivity','bool',1,'','toggle giving different tags to disconnected components of each tag'],
            ['ConnectivityOffset','offset','int',1,'','offset defining the inside/outside tags of the not connected regions']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ['CellEntityIdsArray','oentityidsarray','vtkIntArray',1,'','the output entity ids array']
            ])



    def CleanSurface(self):
        cleaner = vtk.vtkCleanPolyData()
        cleaner.SetInputData(self.Surface)
        cleaner.Update()
        self.Surface = cleaner.GetOutput()
        self.CellEntityIdsArray = self.Surface.GetCellData().GetArray(self.CellEntityIdsArrayName)



    def TagDisconnectedPartsOfEachTag(self):

        self.CleanSurface()

        tags = set()
        for i in range(self.Surface.GetNumberOfCells()):
            tags.add(self.CellEntityIdsArray.GetComponent(i,0))
        self.PrintLog('Tags of the surface before connectivity filter: '+str(tags))

        surface = []

        mergeTags = vtk.vtkAppendPolyData()

        for k, item in enumerate(tags):

            th = vtk.vtkThreshold()
            th.SetInputData(self.Surface)
            th.SetInputArrayToProcess(0, 0, 0, 1, self.CellEntityIdsArrayName)
            th.ThresholdBetween(item-0.001,item+0.001)
            th.Update()
            gf = vtk.vtkGeometryFilter()
            gf.SetInputConnection(th.GetOutputPort())
            gf.Update()
            surface.append(gf.GetOutput())

            connectivityFilter = vtk.vtkConnectivityFilter()
            connectivityFilter.SetInputData(surface[k])
            connectivityFilter.SetExtractionModeToAllRegions()
            connectivityFilter.ColorRegionsOn()
            connectivityFilter.Update()
            surface[k] = connectivityFilter.GetOutput()
            cellEntityIdsArray = surface[k].GetCellData().GetArray(self.CellEntityIdsArrayName)

            regionIdArray = surface[k].GetCellData().GetArray('RegionId')

            for i in range(surface[k].GetNumberOfCells()):
                tag = cellEntityIdsArray.GetComponent(i,0) +regionIdArray.GetComponent(i,0)*self.ConnectivityOffset
                cellEntityIdsArray.SetComponent(i,0,tag)

            mergeTags.AddInputData(surface[k])

        mergeTags.Update()
        self.Surface = mergeTags.GetOutput()
        self.CellEntityIdsArray = self.Surface.GetCellData().GetArray(self.CellEntityIdsArrayName)



    def Execute(self):

        from vmtk import vmtkscripts

        if self.Surface == None:
            self.PrintError('Error: no Surface.')

        self.CellEntityIdsArray = self.Surface.GetCellData().GetArray(self.CellEntityIdsArrayName)

        if self.CellEntityIdsArray == None or self.OverwriteOutsideTag:
            self.CellEntityIdsArray = vtk.vtkIntArray()
            self.CellEntityIdsArray.SetName(self.CellEntityIdsArrayName)
            self.CellEntityIdsArray.SetNumberOfComponents(1)
            self.CellEntityIdsArray.SetNumberOfTuples(self.Surface.GetNumberOfCells())
            self.Surface.GetCellData().AddArray(self.CellEntityIdsArray)
            for i in range(self.Surface.GetNumberOfCells()):
                self.CellEntityIdsArray.SetComponent(i,0,self.OutsideTag)

        # clip the surface according to the ClipArray
        clipper = vmtkscripts.vmtkSurfaceClipper()
        clipper.Surface = self.Surface
        clipper.Interactive = False
        clipper.InsideOut = self.InsideOut
        clipper.CleanOutput = self.CleanOutput
        clipper.ClipArrayName = self.ClipArrayName
        clipper.ClipValue = self.ClipValue

        clipper.Execute()

        insideSurface = clipper.Surface
        outsideSurface = clipper.ClippedSurface

        # change values of the inside tags
        insideCellEntityIdsArray = insideSurface.GetCellData().GetArray( self.CellEntityIdsArrayName )
        outsideCellEntityIdsArray = outsideSurface.GetCellData().GetArray( self.CellEntityIdsArrayName )

        for i in range(insideSurface.GetNumberOfCells()):
            insideCellEntityIdsArray.SetComponent(i,0,self.InsideTag)
        
        # merge the inside and the outside surfaces
        mergeSurface = vtk.vtkAppendPolyData()
        mergeSurface.AddInputData(insideSurface)
        mergeSurface.AddInputData(outsideSurface)
        mergeSurface.Update()

        self.Surface = mergeSurface.GetOutput()
        self.CellEntityIdsArray = self.Surface.GetCellData().GetArray(self.CellEntityIdsArrayName)

        # check the connectivity of the two parts of the surface
        if self.Connectivity:
            self.TagDisconnectedPartsOfEachTag()

        if self.CleanOutput:
            self.CleanSurface()

        tags = set()
        for i in range(self.Surface.GetNumberOfCells()):
            tags.add(self.CellEntityIdsArray.GetComponent(i,0))
        self.PrintLog('Tags of the output surface: '+str(tags))

        # useless, already triangulated
        # if self.Triangulate:
        #     triangleFilter = vtk.vtkTriangleFilter()
        #     triangleFilter.SetInputData(self.Surface)
        #     triangleFilter.Update()
        #     self.Surface = triangleFilter.GetOutput()



if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
