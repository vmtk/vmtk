#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkendpointsections.py,v $
## Language:  Python
## Date:      $Date: 2021/01/05 $
## Version:   $Revision: 1.5 $

##   Copyright (c) Ulf Schiller, Luca Antiga. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Ulf Schiller
##       Clemson University

## Extract endpoint sections of a split and grouped centerline

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import vtkvmtk
from vmtk import vmtkscripts
from vmtk import pypes

import numpy as np


class vmtkEndpointSections(pypes.pypeScript):

    def __init__(self):
        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Centerlines = None
        self.IoletSections = None

        self.CenterlineIdsArrayName = 'CenterlineIds'
        self.GroupIdsArrayName = 'GroupIds'
        self.NormalsArrayName = 'IoletNormals'

        self.SetScriptName('vmtkendpointsections')
        self.SetScriptDoc('extracts the inlets and outlets of all branches')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['Centerlines','centerlines','vtkPolyData',1,'','the input centerline','vmtksurfacereader'],
            ['CenterlineIdsArrayName','centerlineidsarray','str',1],
            ['GroupIdsArrayName','groupidsarray','str',1],
            ])
        self.SetOutputMembers([
            ['IoletSections','o','vtkPolyData',1,'','the iolet sections','vmtksurfacewriter'],
            ['CenterlineIdsArrayName','centerlineidsarray','str',1],
            ['GroupIdsArrayName','groupidsarray','str',1],
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        endpoints = vtk.vtkPolyData()
        cellArray = vtk.vtkCellArray()
        pointArray = vtk.vtkPoints()

        centerlineIdsArray = vtk.vtkIntArray()
        centerlineIdsArray.SetNumberOfComponents(1)
        centerlineIdsArray.SetName(self.CenterlineIdsArrayName)

        ioletIdsArray = vtk.vtkIntArray()
        ioletIdsArray.SetNumberOfComponents(1)
        ioletIdsArray.SetName(self.GroupIdsArrayName)

        normalsArray = vtk.vtkDoubleArray()
        normalsArray.SetNumberOfComponents(3)
        normalsArray.SetName(self.NormalsArrayName)

        # loop over centerlines
        numberOfCells = self.Centerlines.GetNumberOfCells()
        for cellId in range(numberOfCells):

            cell = self.Centerlines.GetCell(cellId)

            numberOfPoints = cell.GetNumberOfPoints()

            # create inlet section
            firstPoint = cell.GetPoints().GetPoint(0)
            nextPoint = cell.GetPoints().GetPoint(1)
            distance = np.sqrt(vtk.vtkMath.Distance2BetweenPoints(firstPoint,nextPoint))
            tangent = np.subtract(nextPoint, firstPoint)/distance
            vtk.vtkMath.Normalize(tangent)

            pointId = pointArray.InsertNextPoint(firstPoint)
            cellArray.InsertNextCell(1)
            cellArray.InsertCellPoint(pointId)
            centerlineIdsArray.InsertNextValue(cellId)
            ioletIdsArray.InsertNextValue(pointId)
            normalsArray.InsertNextTuple3(tangent[0],tangent[1],tangent[2])

            # create outlet section
            lastPoint = cell.GetPoints().GetPoint(numberOfPoints-1)
            nextPoint = cell.GetPoints().GetPoint(numberOfPoints-2)
            distance = np.sqrt(vtk.vtkMath.Distance2BetweenPoints(firstPoint,nextPoint))
            tangent = np.subtract(lastPoint, nextPoint)/distance
            vtk.vtkMath.Normalize(tangent)

            pointId = pointArray.InsertNextPoint(lastPoint)
            cellArray.InsertNextCell(1)
            cellArray.InsertCellPoint(pointId)
            centerlineIdsArray.InsertNextValue(cellId)
            ioletIdsArray.InsertNextValue(pointId)
            normalsArray.InsertNextTuple3(tangent[0],tangent[1],tangent[2])

        endpoints.SetPoints(pointArray)
        endpoints.SetVerts(cellArray)
        endpoints.GetCellData().AddArray(centerlineIdsArray)
        endpoints.GetCellData().AddArray(ioletIdsArray)
        endpoints.GetCellData().AddArray(normalsArray)

        self.GroupEndpointSections(endpoints)
        self.MakeGroupIdsAdjacent(endpoints)

        self.IoletSections = endpoints

    def GroupEndpointSections(self, endpoints):
        ioletIdsArray = endpoints.GetCellData().GetArray(self.GroupIdsArrayName)
        for ioletId in range(endpoints.GetNumberOfCells()):
            groupId = ioletIdsArray.GetValue(ioletId)
            currentPoint = endpoints.GetPoints().GetPoint(ioletId)
            for j in reversed(range(ioletId)):
                point = endpoints.GetPoints().GetPoint(j)
                distance = np.sqrt(vtk.vtkMath.Distance2BetweenPoints(point,currentPoint))
                print (groupId,j,"%.6e"%distance)
                if (distance < 1.e-12):
                    ioletIdsArray.SetValue(groupId,j)

    def MakeGroupIdsAdjacent(self, endpoints):
        currentGroupId = 0
        groupIdsArray = endpoints.GetCellData().GetArray(self.GroupIdsArrayName)
        for i in range(groupIdsArray.GetNumberOfTuples()):
            minGroupId = sys.maxsize # int is unbounded in Python 3
            for j in range(groupIdsArray.GetNumberOfTuples()):
                groupId = groupIdsArray.GetValue(j)
                if groupId < minGroupId and groupId >= currentGroupId:
                    minGroupId = groupId
            for j in range(groupIdsArray.GetNumberOfTuples()):
                if groupIdsArray.GetValue(j) == minGroupId:
                    groupIdsArray.SetValue(j,currentGroupId)
            currentGroupId += 1


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
