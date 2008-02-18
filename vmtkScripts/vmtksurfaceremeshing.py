#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceremeshing.py,v $
## Language:  Python
## Date:      $Date: 2006/02/23 09:27:52 $
## Version:   $Revision: 1.7 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.


import vtk
import vtkvmtk
import sys

import pypes

vmtksurfaceremeshing = 'vmtkSurfaceRemeshing'

class vmtkSurfaceRemeshing(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Surface = None

        self.TargetArea = 1.0
        self.TargetAreaFactor = 1.0
        self.MinimumAreaFactor = 0.5
        self.NumberOfIterations = 10
        self.NumberOfConnectivityOptimizationIterations = 20
        self.CellEntityIdsArrayName = None
        self.TargetAreaArrayName = 'TargetArea'
        self.ElementSizeMode = 'fixedarea'
        self.AspectRatioThreshold = 1.2
        self.InternalAngleTolerance = 0.0
        self.NormalAngleTolerance = 0.2
        self.CollapseAngleThreshold = 0.5
        self.Relaxation = 0.5
        self.PreserveBoundaryEdges = 0

        self.SetScriptName('vmtksurfaceremeshing')
        self.SetScriptDoc('remesh a surface using quality triangles')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['TargetArea','targetarea','float',1,'(0.0,)'],
            ['TargetAreaFactor','targetareafactor','float',1,'(0.0,)'],
            ['MinimumAreaFactor','minareafactor','float',1,'(0.0,)'],
            ['NumberOfIterations','iterations','int',1,'(0,)'],
            ['NumberOfConnectivityOptimizationIterations','connectivityiterations','int',1,'(0,)'],
            ['CellEntityIdsArrayName','entityidsarray','str',1],
            ['TargetAreaArrayName','targetareaarray','str',1],
            ['ElementSizeMode','elementsizemode','str',1,'["fixedarea","areaarray"]'],
            ['AspectRatioThreshold','aspectratio','float',1,'(0.0,)'],
            ['InternalAngleTolerance','internalangletolerance','float',1,'(0.0,)'],
            ['NormalAngleTolerance','normalangletolerance','float',1,'(0.0,)'],
            ['CollapseAngleThreshold','collapseangle','float',1,'(0.0,)'],
            ['Relaxation','relaxation','float',1,'(0.5,)'],
            ['PreserveBoundaryEdges','preserveboundary','bool',1]
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        cleaner = vtk.vtkCleanPolyData()
        cleaner.SetInput(self.Surface)
        cleaner.Update()

        triangleFilter = vtk.vtkTriangleFilter()
        triangleFilter.SetInput(cleaner.GetOutput())
        triangleFilter.Update()

        surfaceRemeshing = vtkvmtk.vtkvmtkPolyDataSurfaceRemeshing()
        surfaceRemeshing.SetInput(triangleFilter.GetOutput())
        if (self.CellEntityIdsArrayName):
            surfaceRemeshing.SetCellEntityIdsArrayName(self.CellEntityIdsArrayName)
        surfaceRemeshing.SetTargetAreaArrayName(self.TargetAreaArrayName)
        if self.ElementSizeMode == 'fixedarea':
            surfaceRemeshing.SetElementSizeModeToTargetArea()
        elif self.ElementSizeMode == 'areaarray':
            surfaceRemeshing.SetElementSizeModeToTargetAreaArray()
        else:
            self.PrintError('Error: unsupported ElementSizeMode.')
        surfaceRemeshing.SetTargetArea(self.TargetArea)
        surfaceRemeshing.SetTargetAreaFactor(self.TargetAreaFactor)
        surfaceRemeshing.SetMinimumAreaFactor(self.MinimumAreaFactor)
        surfaceRemeshing.SetNumberOfIterations(self.NumberOfIterations)
        surfaceRemeshing.SetNumberOfConnectivityOptimizationIterations(self.NumberOfConnectivityOptimizationIterations)
        surfaceRemeshing.SetRelaxation(self.Relaxation)
        surfaceRemeshing.SetAspectRatioThreshold(self.AspectRatioThreshold)
        surfaceRemeshing.SetInternalAngleTolerance(self.InternalAngleTolerance)
        surfaceRemeshing.SetNormalAngleTolerance(self.NormalAngleTolerance)
        surfaceRemeshing.SetCollapseAngleThreshold(self.CollapseAngleThreshold)
        surfaceRemeshing.SetPreserveBoundaryEdges(self.PreserveBoundaryEdges)
        surfaceRemeshing.Update()

        self.Surface = surfaceRemeshing.GetOutput()

        if self.Surface.GetSource():
            self.Surface.GetSource().UnRegisterAllOutputs()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
