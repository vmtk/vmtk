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

        self.TargetEdgeLength = 1.0
        self.TargetEdgeLengthFactor = 1.0
        self.TargetEdgeLengthArrayName = ''
        self.MaxEdgeLength = 1E16
        self.MinEdgeLength = 0.0
        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.ElementSizeMode = 'edgelength'
        self.VolumeElementScaleFactor = 0.8
        self.CappingMethod = 'simple'
        self.SkipCapping = 0
        self.RemeshCapsOnly = 0

        self.BoundaryLayer = 0
        self.NumberOfSubLayers = 2
        self.BoundaryLayerThicknessFactor = 0.25

        self.Tetrahedralize = 0

        self.SizingFunctionArrayName = 'VolumeSizingFunction'

        # Output objects
        self.Mesh = None
        self.RemeshedSurface = None

        self.SetScriptName('vmtkmeshgenerator')
        self.SetScriptDoc('generate a mesh suitable for CFD from a surface')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['TargetEdgeLength','edgelength','float',1,'(0.0,)'],
            ['TargetEdgeLengthArrayName','edgelengtharray','str',1],
            ['TargetEdgeLengthFactor','edgelengthfactor','float',1,'(0.0,)'],
            ['MaxEdgeLength','maxedgelength','float',1,'(0.0,)'],
            ['MinEdgeLength','minedgelength','float',1,'(0.0,)'],
            ['CellEntityIdsArrayName','entityidsarray','str',1],
            ['ElementSizeMode','elementsizemode','str',1,'["edgelength","edgelengtharray"]'],
            ['CappingMethod','cappingmethod','str',1,'["simple","annular"]'],
            ['SkipCapping','skipcapping','bool',1,''],
            ['VolumeElementScaleFactor','volumeelementfactor','float',1,'(0.0,)'],
            ['BoundaryLayer','boundarylayer','bool',1,''],
            ['NumberOfSubLayers','sublayers','int',1,'(0,)'],
            ['BoundaryLayerThicknessFactor','thicknessfactor','float',1,'(0.0,1.0)'],
            ['RemeshCapsOnly','remeshcapsonly','bool',1,''],
            ['Tetrahedralize','tetrahedralize','bool',1,'']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter'],
            ['CellEntityIdsArrayName','entityidsarray','str',1],
            ['RemeshedSurface','remeshedsurface','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        wallEntityOffset = 1

        if self.SkipCapping:
            self.PrintLog("Not capping surface")
            surface = self.Surface
        else:
            self.PrintLog("Capping surface")
            capper = vmtkscripts.vmtkSurfaceCapper()
            capper.Surface = self.Surface
            capper.Interactive = 0
            capper.Method = self.CappingMethod
            capper.TriangleOutput = 0
            capper.CellEntityIdOffset = 1
            capper.CellEntityIdOffset = wallEntityOffset
            capper.Execute()
            surface = capper.Surface

        self.PrintLog("Remeshing surface")
        remeshing = vmtkscripts.vmtkSurfaceRemeshing()
        remeshing.Surface = surface
        remeshing.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        remeshing.TargetEdgeLength = self.TargetEdgeLength
        remeshing.MaxEdgeLength = self.MaxEdgeLength
        remeshing.MinEdgeLength = self.MinEdgeLength
        remeshing.TargetEdgeLengthFactor = self.TargetEdgeLengthFactor
        remeshing.TargetEdgeLengthArrayName = self.TargetEdgeLengthArrayName
        remeshing.ElementSizeMode = self.ElementSizeMode
        if self.RemeshCapsOnly:
            remeshing.ExcludeEntityIds = [wallEntityOffset]
        remeshing.Execute()

        if self.BoundaryLayer:

            projection = vmtkscripts.vmtkSurfaceProjection()
            projection.Surface = remeshing.Surface
            projection.ReferenceSurface = surface
            projection.Execute()

            normals = vmtkscripts.vmtkSurfaceNormals()
            normals.Surface = projection.Surface
            normals.NormalsArrayName = 'Normals'
            normals.Execute()
    
            surfaceToMesh = vmtkscripts.vmtkSurfaceToMesh()
            surfaceToMesh.Surface = normals.Surface
            surfaceToMesh.Execute()

            self.PrintLog("Generating boundary layer")
            boundaryLayer = vmtkscripts.vmtkBoundaryLayer()
            boundaryLayer.Mesh = surfaceToMesh.Mesh
            boundaryLayer.WarpVectorsArrayName = 'Normals'
            boundaryLayer.NegateWarpVectors = True
            boundaryLayer.ThicknessArrayName = self.TargetEdgeLengthArrayName
            if self.ElementSizeMode == 'edgelength':
                boundaryLayer.ConstantThickness = True
            else: 
                boundaryLayer.ConstantThickness = False
            boundaryLayer.IncludeSurfaceCells = 0
            boundaryLayer.NumberOfSubLayers = self.NumberOfSubLayers
            boundaryLayer.SubLayerRatio = 0.5
            boundaryLayer.Thickness = self.BoundaryLayerThicknessFactor * self.TargetEdgeLength
            boundaryLayer.ThicknessRatio = self.BoundaryLayerThicknessFactor * self.TargetEdgeLengthFactor
            boundaryLayer.MaximumThickness = self.BoundaryLayerThicknessFactor * self.MaxEdgeLength
            boundaryLayer.Execute()

            cellEntityIdsArray = vtk.vtkIntArray()
            cellEntityIdsArray.SetName(self.CellEntityIdsArrayName)
            cellEntityIdsArray.SetNumberOfTuples(boundaryLayer.Mesh.GetNumberOfCells())
            cellEntityIdsArray.FillComponent(0,0.0)
            boundaryLayer.Mesh.GetCellData().AddArray(cellEntityIdsArray)

            innerCellEntityIdsArray = vtk.vtkIntArray()
            innerCellEntityIdsArray.SetName(self.CellEntityIdsArrayName)
            innerCellEntityIdsArray.SetNumberOfTuples(boundaryLayer.InnerSurfaceMesh.GetNumberOfCells())
            innerCellEntityIdsArray.FillComponent(0,0.0)
            boundaryLayer.InnerSurfaceMesh.GetCellData().AddArray(cellEntityIdsArray)

            meshToSurface = vmtkscripts.vmtkMeshToSurface()
            meshToSurface.Mesh = boundaryLayer.InnerSurfaceMesh
            meshToSurface.Execute()

            self.PrintLog("Computing sizing function")
            sizingFunction = vtkvmtk.vtkvmtkPolyDataSizingFunction()
            sizingFunction.SetInput(meshToSurface.Surface)
            sizingFunction.SetSizingFunctionArrayName(self.SizingFunctionArrayName)
            sizingFunction.SetScaleFactor(self.VolumeElementScaleFactor)
            sizingFunction.Update()

            surfaceToMesh2 = vmtkscripts.vmtkSurfaceToMesh()
            surfaceToMesh2.Surface = sizingFunction.GetOutput()
            surfaceToMesh2.Execute()

            self.PrintLog("Generating volume mesh")
            tetgen = vmtkscripts.vmtkTetGen()
            tetgen.Mesh = surfaceToMesh2.Mesh
            tetgen.GenerateCaps = 0
            tetgen.UseSizingFunction = 1
            tetgen.SizingFunctionArrayName = self.SizingFunctionArrayName
            tetgen.CellEntityIdsArrayName = self.CellEntityIdsArrayName
            tetgen.Order = 1
            tetgen.Quality = 1
            tetgen.PLC = 1
            tetgen.NoBoundarySplit = 1
            tetgen.RemoveSliver = 1
            tetgen.OutputSurfaceElements = 0
            tetgen.OutputVolumeElements = 1
            tetgen.Execute()

            if tetgen.Mesh.GetNumberOfCells() == 0 and surfaceToMesh.Mesh.GetNumberOfCells() > 0:
                self.PrintLog('An error occurred during tetrahedralization. Will only output surface mesh and boundary layer.')

            appendFilter = vtkvmtk.vtkvmtkAppendFilter()
            appendFilter.AddInput(surfaceToMesh.Mesh)
            appendFilter.AddInput(boundaryLayer.Mesh)
            appendFilter.AddInput(tetgen.Mesh)
            appendFilter.Update()

            self.Mesh = appendFilter.GetOutput()

        else:

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
            tetgen.Order = 1
            tetgen.Quality = 1
            tetgen.PLC = 1
            tetgen.NoBoundarySplit = 1
            tetgen.RemoveSliver = 1
            tetgen.OutputSurfaceElements = 1
            tetgen.OutputVolumeElements = 1
            tetgen.Execute()

            self.Mesh = tetgen.Mesh

            if self.Mesh.GetNumberOfCells() == 0 and surfaceToMesh.Mesh.GetNumberOfCells() > 0:
                self.PrintLog('An error occurred during tetrahedralization. Will only output surface mesh.')
                self.Mesh = surfaceToMesh.Mesh

        if self.Tetrahedralize:

            tetrahedralize = vtk.vtkDataSetTriangleFilter()
            tetrahedralize.SetInput(self.Mesh)
            tetrahedralize.Update()

            self.Mesh = tetrahedralize.GetOutput()

        self.RemeshedSurface = remeshing.Surface

if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
