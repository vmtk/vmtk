#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceremeshing.py,v $
## Language:  Python
## Date:      $Date: 2006/02/23 09:27:52 $
## Version:   $Revision: 1.7 $

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


class vmtkSurfaceRemeshing(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None

        self.TargetArea = 1.0
        self.TargetEdgeLength = 1.0
        self.TargetAreaFactor = 1.0
        self.TargetEdgeLengthFactor = 1.0
        self.TriangleSplitFactor = 5.0
        self.MaxArea = 1E16
        self.MinArea = 0.0
        self.MaxEdgeLength = 1E16
        self.MinEdgeLength = 0.0
        self.NumberOfIterations = 10
        self.NumberOfConnectivityOptimizationIterations = 20
        self.CellEntityIdsArrayName = None
        self.TargetAreaArrayName = 'TargetArea'
        self.TargetEdgeLengthArrayName = ''
        self.ElementSizeMode = 'area'
        self.MinAreaFactor = 0.5
        self.AspectRatioThreshold = 1.2
        self.InternalAngleTolerance = 0.0
        self.NormalAngleTolerance = 0.2
        self.CollapseAngleThreshold = 0.2
        self.Relaxation = 0.5
        self.PreserveBoundaryEdges = 0
        self.ExcludeEntityIds = []

        self.SetScriptName('vmtksurfaceremeshing')
        self.SetScriptDoc('remesh a surface using quality triangles')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['ElementSizeMode','elementsizemode','str',1,'["area","edgelength","areaarray","edgelengtharray"]'],
            ['TargetArea','area','float',1,'(0.0,)'],
            ['TargetAreaArrayName','areaarray','str',1],
            ['TargetEdgeLength','edgelength','float',1,'(0.0,)'],
            ['TargetEdgeLengthArrayName','edgelengtharray','str',1],
            ['TargetAreaFactor','areafactor','float',1,'(0.0,)'],
            ['TargetEdgeLengthFactor','edgelengthfactor','float',1,'(0.0,)'],
            ['TriangleSplitFactor','trianglesplitfactor','float',1,'(0.0,)'],
            ['MaxArea','maxarea','float',1,'(0.0,)'],
            ['MinArea','minarea','float',1,'(0.0,)'],
            ['NumberOfIterations','iterations','int',1,'(0,)'],
            ['NumberOfConnectivityOptimizationIterations','connectivityiterations','int',1,'(0,)'],
            ['CellEntityIdsArrayName','entityidsarray','str',1],
            ['AspectRatioThreshold','aspectratio','float',1,'(0.0,)'],
            ['MinAreaFactor','minareafactor','float',1,'(0.0,)'],
            ['InternalAngleTolerance','internalangletolerance','float',1,'(0.0,)'],
            ['NormalAngleTolerance','normalangletolerance','float',1,'(0.0,)'],
            ['CollapseAngleThreshold','collapseangle','float',1,'(0.0,)'],
            ['Relaxation','relaxation','float',1,'(0.5,)'],
            ['ExcludeEntityIds','exclude','int',-1,''],
            ['PreserveBoundaryEdges','preserveboundary','bool',1]
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        cleaner = vtk.vtkCleanPolyData()
        cleaner.SetInputData(self.Surface)
        cleaner.Update()

        triangleFilter = vtk.vtkTriangleFilter()
        triangleFilter.SetInputConnection(cleaner.GetOutputPort())
        triangleFilter.Update()

        self.Surface = triangleFilter.GetOutput()

        if self.ElementSizeMode == 'edgelength':
            self.TargetArea = 0.25 * 3.0**0.5 * self.TargetEdgeLength**2
        elif self.ElementSizeMode == 'edgelengtharray':
            calculator = vtk.vtkArrayCalculator()
            calculator.SetInputData(self.Surface)
            calculator.AddScalarArrayName(self.TargetEdgeLengthArrayName,0)
            calculator.SetFunction("%f^2 * 0.25 * sqrt(3) * %s^2" % (self.TargetEdgeLengthFactor,self.TargetEdgeLengthArrayName))
            calculator.SetResultArrayName(self.TargetAreaArrayName)
            calculator.Update()
            self.MaxArea = 0.25 * 3.0**0.5 * self.MaxEdgeLength**2
            self.MinArea = 0.25 * 3.0**0.5 * self.MinEdgeLength**2
            self.Surface = calculator.GetOutput()

        excludedIds = vtk.vtkIdList()
        if self.ExcludeEntityIds:
            for excludedId in self.ExcludeEntityIds:
                excludedIds.InsertNextId(excludedId)

        surfaceRemeshing = vtkvmtk.vtkvmtkPolyDataSurfaceRemeshing()
        surfaceRemeshing.SetInputData(self.Surface)
        if self.CellEntityIdsArrayName:
            surfaceRemeshing.SetCellEntityIdsArrayName(self.CellEntityIdsArrayName)
        if self.ElementSizeMode in ['area','edgelength']:
            surfaceRemeshing.SetElementSizeModeToTargetArea()
        elif self.ElementSizeMode in ['areaarray','edgelengtharray']:
            surfaceRemeshing.SetElementSizeModeToTargetAreaArray()
            surfaceRemeshing.SetTargetAreaArrayName(self.TargetAreaArrayName)
        else:
            self.PrintError('Error: unsupported ElementSizeMode.')
        surfaceRemeshing.SetTargetArea(self.TargetArea)
        surfaceRemeshing.SetTargetAreaFactor(self.TargetAreaFactor)
        surfaceRemeshing.SetTriangleSplitFactor(self.TriangleSplitFactor)
        surfaceRemeshing.SetMaxArea(self.MaxArea)
        surfaceRemeshing.SetMinArea(self.MinArea)
        surfaceRemeshing.SetNumberOfIterations(self.NumberOfIterations)
        surfaceRemeshing.SetNumberOfConnectivityOptimizationIterations(self.NumberOfConnectivityOptimizationIterations)
        surfaceRemeshing.SetRelaxation(self.Relaxation)
        surfaceRemeshing.SetMinAreaFactor(self.MinAreaFactor)
        surfaceRemeshing.SetAspectRatioThreshold(self.AspectRatioThreshold)
        surfaceRemeshing.SetInternalAngleTolerance(self.InternalAngleTolerance)
        surfaceRemeshing.SetNormalAngleTolerance(self.NormalAngleTolerance)
        surfaceRemeshing.SetCollapseAngleThreshold(self.CollapseAngleThreshold)
        surfaceRemeshing.SetPreserveBoundaryEdges(self.PreserveBoundaryEdges)
        surfaceRemeshing.SetExcludedEntityIds(excludedIds)
        surfaceRemeshing.Update()

        self.Surface = surfaceRemeshing.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
