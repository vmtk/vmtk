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
        self.Method = 'cliparray'
        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.CellEntityIdsArray = None
        self.ArrayName = None
        self.Array = None
        self.Value = 0.0
        self.InsideTag = 2
        self.OutsideTag = 1
        self.OverwriteOutsideTag = 1
        self.InsideOut = 0
        self.ConnectivityOffset = 1
        self.CleanOutput = 1
        self.PrintTags = 1

        self.SetScriptName('vmtksurfacetagger')
        self.SetScriptDoc('tag a surface exploiting an array defined on it')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['Method','method','str',1,'["cliparray","array","connectivity","constant"]','tagging method (cliparray: exploit an array to clip the surface at a certain value tagging the two parts; array: the same of cliparray, but without clipping the original triangles; connectivity: given an already tagged surface, tag disconnected part of each input tag); constant: assign a constant tag to the input surface'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'','name of the array where the tags are stored'],
            ['ArrayName','array','str',1,'','name of the array with which to define the boundary between tags'],
            ['Value','value','float',1,'','scalar value of the array identifying the boundary between tags'],
            ['InsideTag','inside','int',1,'','tag of the inside region (i.e. where the Array is lower than Value; used also in case of "constant" method)'],
            ['OutsideTag','outside','int',1,'','tag of the outside region (i.e. where the Array is greater than Value)'],
            ['OverwriteOutsideTag','overwriteoutside','bool',1,'','overwrite outside value also when the CellEntityIdsArray already exists in the input surface'],
            ['InsideOut','insideout','bool',1,'','toggle switching inside and outside tags'],
            ['ConnectivityOffset','offset','int',1,'','offset added to the entityidsarray of each disconnected parts of each input tag (connectivity only)'],
            ['CleanOutput','cleanoutput','bool',1,'','toggle cleaning the unused points'],
            ['PrintTags','printtags','bool',1,'','toggle printing the set of tags']
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



    def ClipArrayTagger(self):

        from vmtk import vmtkscripts

        # clip the surface according to the Array
        clipper = vmtkscripts.vmtkSurfaceClipper()
        clipper.Surface = self.Surface
        clipper.Interactive = False
        clipper.InsideOut = 1-self.InsideOut # inside means regions where the Array is lower than Value
        clipper.CleanOutput = self.CleanOutput
        clipper.ClipArrayName = self.ArrayName
        clipper.ClipValue = self.Value

        clipper.Execute()

        insideSurface = clipper.Surface
        outsideSurface = clipper.ClippedSurface

        # change values of the inside tags
        insideCellEntityIdsArray = insideSurface.GetCellData().GetArray( self.CellEntityIdsArrayName )
        outsideCellEntityIdsArray = outsideSurface.GetCellData().GetArray( self.CellEntityIdsArrayName )

        insideCellEntityIdsArray.FillComponent(0,self.InsideTag)

        # merge the inside and the outside surfaces
        mergeSurface = vtk.vtkAppendPolyData()
        mergeSurface.AddInputData(insideSurface)
        mergeSurface.AddInputData(outsideSurface)
        mergeSurface.Update()

        self.Surface = mergeSurface.GetOutput()
        self.CellEntityIdsArray = self.Surface.GetCellData().GetArray(self.CellEntityIdsArrayName)



    def ArrayTagger(self):
        pointsToCells = vtk.vtkPointDataToCellData()
        pointsToCells.SetInputData(self.Surface)
        pointsToCells.PassPointDataOn()
        pointsToCells.Update()
        self.Surface = pointsToCells.GetPolyDataOutput()
        self.CellEntityIdsArray = self.Surface.GetCellData().GetArray(self.CellEntityIdsArrayName)
        cellArray = self.Surface.GetCellData().GetArray(self.ArrayName)
        for i in range(self.Surface.GetNumberOfCells()):
            if cellArray.GetValue(i) < self.Value:
                self.CellEntityIdsArray.SetValue(i,self.InsideTag)



    def ConnectivityTagger(self):

        self.CleanSurface()

        tags = set()
        for i in range(self.Surface.GetNumberOfCells()):
            tags.add(self.CellEntityIdsArray.GetComponent(i,0))
        tags = sorted(tags)
        if self.PrintTags:
            self.PrintLog('Initial tags: '+str(tags))

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

        if self.Surface == None:
            self.PrintError('Error: no Surface.')

        self.CellEntityIdsArray = self.Surface.GetCellData().GetArray(self.CellEntityIdsArrayName)

        # initialize the CellEntityIdsArray with OutsideTag in some cases
        if self.CellEntityIdsArray == None or (self.OverwriteOutsideTag and self.Method != "connectivity"):
            self.CellEntityIdsArray = vtk.vtkIntArray()
            self.CellEntityIdsArray.SetName(self.CellEntityIdsArrayName)
            self.CellEntityIdsArray.SetNumberOfComponents(1)
            self.CellEntityIdsArray.SetNumberOfTuples(self.Surface.GetNumberOfCells())
            self.Surface.GetCellData().AddArray(self.CellEntityIdsArray)
            self.CellEntityIdsArray.FillComponent(0,self.OutsideTag)

        if self.Method == 'cliparray':
            self.ClipArrayTagger()
        elif self.Method == 'array':
            self.ArrayTagger()
        elif self.Method == 'connectivity':
            self.ConnectivityTagger()
        elif self.Method == 'constant':
            self.CellEntityIdsArray.FillComponent(0,self.InsideTag)
        else:
            self.PrintError("Method unknown (available: cliparray, array, connectivity)")

        if self.CleanOutput:
            self.CleanSurface()

        if self.PrintTags:
            tags = set()
            for i in range(self.Surface.GetNumberOfCells()):
                tags.add(self.CellEntityIdsArray.GetComponent(i,0))
            tags = sorted(tags)
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
