#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceprojection.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.6 $

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


class vmtkSurfaceProjection(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.ReferenceSurface = None
        self.Surface = None
        self.Method = 'linear'
        self.ActiveArrays = []

        self.SetScriptName('vmtksurfaceprojection')
        self.SetScriptDoc('project the data of a reference surface onto the input surface based on minimum distance criterion')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['ReferenceSurface','r','vtkPolyData',1,'','the reference surface','vmtksurfacereader'],
            ['Method','method','str',1,'["linear","closestpoint"]',"'linear': a linear interpolation/projection is performed on the points of the input surface (only works with point data arrays); 'closestpoint': in each cell/point of the input surface the value of the closest cell/point is projected (usefull to project discrete arrays like CellEntityIdsArray)"],
            ['ActiveArrays','activearrays','str',-1,'','list of the arrays to project (if empty, all the arrays are projected)']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])


    def ClosestPointProjection(self,idata,rdata):
        refPointData = rdata.GetPointData()
        refCellData = rdata.GetCellData()
        nPointArrays = refPointData.GetNumberOfArrays()
        nCellArrays = refCellData.GetNumberOfArrays()
        nPoints = idata.GetNumberOfPoints()
        nCells = idata.GetNumberOfCells()

        if nPointArrays > 0:
            # initialize point data arrays on the Surface
            pointDataArrays = []
            pointArrayNames = ""
            for i in range(nPointArrays):
                refArray = refPointData.GetArray(i)
                pointDataArrays.append(vtk.vtkDataArray.CreateDataArray(refArray.GetDataType()))
                pointDataArrays[i].SetNumberOfComponents(refArray.GetNumberOfComponents())
                pointDataArrays[i].SetNumberOfTuples(nPoints)
                pointDataArrays[i].SetName(refArray.GetName())
                pointArrayNames += refArray.GetName() + ","
                idata.GetPointData().AddArray(pointDataArrays[i])

            self.PrintLog("\tProcessing PointData arrays: ["+pointArrayNames[:-1]+"] ...")

            # initialize pointlocator on the ReferenceSurface
            pointLocator = vtk.vtkPointLocator()
            pointLocator.SetDataSet(rdata)
            pointLocator.BuildLocator()

            for j in range(nPoints):
                # find closest point on the reference
                pointId = pointLocator.FindClosestPoint(idata.GetPoint(j))

                # assign the value of the closest point arrays to current point
                for i in range(nPointArrays):
                    pointDataArrays[i].InsertTuple(j,pointId,refPointData.GetArray(i))

        if nCellArrays > 0:
            # initialize cell data arrays on the Surface
            cellDataArrays = []
            cellArrayNames = ""

            for i in range(nCellArrays):
                refArray = refCellData.GetArray(i)
                cellDataArrays.append(vtk.vtkDataArray.CreateDataArray(refArray.GetDataType()))
                cellDataArrays[i].SetNumberOfComponents(refArray.GetNumberOfComponents())
                cellDataArrays[i].SetNumberOfTuples(nCells)
                cellDataArrays[i].SetName(refArray.GetName())
                cellArrayNames += refArray.GetName() + ","
                idata.GetCellData().AddArray(cellDataArrays[i])

            self.PrintLog("\tProcessing CellData arrays: ["+cellArrayNames[:-1]+"] ...")

            # initialize celllocator on the ReferenceSurface
            cellLocator = vtk.vtkCellLocator()
            cellLocator.SetDataSet(rdata)
            cellLocator.BuildLocator()

            for j in range(nCells):
                # set the test point as the barycenter of the current cell
                cell = vtk.vtkGenericCell()
                idata.GetCell(j,cell)
                pcoords = [0.0,0.0,0.0]
                weights = [0.0,0.0,0.0]
                center = [0.0,0.0,0.0]
                cell.GetParametricCenter(pcoords)
                cell.EvaluateLocation(vtk.reference(0),pcoords,center,weights)

                # find closest cell on the reference
                closest = [0.0,0.0,0.0]
                cellId = vtk.reference(0)
                cellLocator.FindClosestPoint(center,closest,cellId,vtk.reference(0),vtk.reference(0))

                # assign the value of the closest point arrays to current point
                for i in range(nCellArrays):
                    cellDataArrays[i].InsertTuple(j,cellId,refCellData.GetArray(i))

        return idata


    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No Surface.')

        if self.ReferenceSurface == None:
            self.PrintError('Error: No ReferenceSurface.')

        if len(self.ActiveArrays) != 0:
            # Warning: vtkPassArray deprecated, it should be replaced by
            #          vtkDataArraySelection and vtkPassSelectedArrays
            passArray = vtk.vtkPassArrays()
            passArray.SetInputData(self.ReferenceSurface)

            for name in self.ActiveArrays:
                passArray.AddPointDataArray(name)
                passArray.AddCellDataArray(name)

            passArray.Update()
            self.ReferenceSurface = passArray.GetOutput()

        if self.Method == 'linear':
            self.PrintLog('Computing linear projection ...')
            surfaceProjection = vtkvmtk.vtkvmtkSurfaceProjection()
            surfaceProjection.SetInputData(self.Surface)
            surfaceProjection.SetReferenceSurface(self.ReferenceSurface)
            surfaceProjection.Update()
            self.Surface = surfaceProjection.GetOutput()
        elif self.Method == 'closestpoint':
            self.PrintLog('Computing closest point projection ...')
            self.Surface = self.ClosestPointProjection(self.Surface,self.ReferenceSurface)



if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
