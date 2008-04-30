#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshgenerator.py,v $
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
import vmtkscripts

import pypes

vmtkmeshgenerator = 'vmtkMeshGenerator'

class vmtkMeshGenerator(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Surface = None

        self.TargetArea = 1.0
        self.TargetAreaFactor = 1.0
        self.TargetAreaArrayName = 'TargetArea'
        self.MaxArea = 1E16
        self.MinArea = 0.0
        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.SurfaceElementSizeMode = 'fixedarea'
        self.VolumeElementScaleFactor = 0.8
#        self.Order = 1

        self.SizingFunctionArrayName = 'VolumeSizingFunction'

        self.SetScriptName('vmtkmeshgenerator')
        self.SetScriptDoc('generate a mesh suitable for CFD from a surface')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['TargetArea','targetarea','float',1,'(0.0,)'],
            ['TargetAreaFactor','targetareafactor','float',1,'(0.0,)'],
            ['MaxArea','maxarea','float',1,'(0.0,)'],
            ['MinArea','minarea','float',1,'(0.0,)'],
            ['CellEntityIdsArrayName','entityidsarray','str',1],
            ['TargetAreaArrayName','targetareaarray','str',1],
            ['SizingFunctionArrayName','sizingfunctionarray','str',1],
            ['SurfaceElementSizeMode','sizemode','str',1,'["fixedarea","areaarray"]'],
            ['VolumeElementScaleFactor','volumeelementfactor','float',1,'(0.0,)'],
#            ['Order','order','int',1,'(1,2)','mesh order']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        self.PrintLog("Capping surface")
        capper = vmtkscripts.vmtkSurfaceCapper()
        capper.Surface = self.Surface
        capper.Interactive = 0
        capper.Method = 'simple'
        capper.TriangleOutput = 0
        capper.CellEntityIdOffset = 1
        capper.Execute()

        self.PrintLog("Remeshing surface")
        remeshing = vmtkscripts.vmtkSurfaceRemeshing()
        remeshing.Surface = capper.Surface
        remeshing.CellEntityIdsArrayName = capper.CellEntityIdsArrayName
        remeshing.TargetArea = self.TargetArea
        remeshing.MaxArea = self.MaxArea
        remeshing.MinArea = self.MinArea
        remeshing.TargetAreaFactor = self.TargetAreaFactor
        remeshing.TargetAreaArrayName = self.TargetAreaArrayName
        remeshing.ElementSizeMode = self.SurfaceElementSizeMode
        remeshing.Execute()

        self.PrintLog("Computing sizing function")
        sizingFunction = vtkvmtk.vtkvmtkPolyDataSizingFunction()
        sizingFunction.SetInput(remeshing.Surface)
        sizingFunction.SetSizingFunctionArrayName(self.SizingFunctionArrayName)
        sizingFunction.SetScaleFactor(self.VolumeElementScaleFactor)
        sizingFunction.Update()

        self.PrintLog("Converting surface to mesh")
        surfaceToMesh = vmtkscripts.vmtkSurfaceToMesh()
        surfaceToMesh.Surface = sizingFunction.GetOutput()
        surfaceToMesh.Execute()

        self.PrintLog("Generating volume mesh")
        tetgen = vmtkscripts.vmtkTetGen()
        tetgen.Mesh = surfaceToMesh.Mesh
        tetgen.GenerateCaps = 0
        tetgen.UseSizingFunction = 1
        tetgen.SizingFunctionArrayName = self.SizingFunctionArrayName
        tetgen.CellEntityIdsArrayName = self.CellEntityIdsArrayName
#        tetgen.Order = self.Order
        tetgen.Quality = 1
        tetgen.PLC = 1
        tetgen.NoBoundarySplit = 1
        tetgen.RemoveSliver = 1
        tetgen.OutputSurfaceElements = 1
        tetgen.OutputVolumeElements = 1
        tetgen.Execute()

        self.Mesh = tetgen.Mesh

        #TODO: smooth mesh?



if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
