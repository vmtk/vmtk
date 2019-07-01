#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshgenerator.py,v $
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


class vmtkMeshGenerator(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None

        self.TargetEdgeLength = 1.0
        self.TargetEdgeLengthFactor = 1.0
        self.TargetEdgeLengthArrayName = ''
        self.MaxEdgeLength = 1E16
        self.MinEdgeLength = 0.0
        self.TriangleSplitFactor = 5.0
        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.ElementSizeMode = 'edgelength'
        self.VolumeElementScaleFactor = 0.8
        self.CappingMethod = 'simple'
        self.SkipCapping = 0
        self.RemeshCapsOnly = 0
        self.SkipRemeshing = 0
        self.EndcapsEdgeLengthFactor = 1.0

        self.BoundaryLayer = 0
        self.NumberOfSubLayers = 2
        self.SubLayerRatio = 0.5
        self.BoundaryLayerThicknessFactor = 0.25

        self.NumberOfSubsteps = 2000
        self.Relaxation = 0.01
        self.LocalCorrectionFactor = 0.45

        self.Tetrahedralize = 0

        self.BoundaryLayerOnCaps = 1

        self.SizingFunctionArrayName = 'VolumeSizingFunction'

        self.Mesh = None
        self.RemeshedSurface = None

        self.SetScriptName('vmtkmeshgenerator')
        self.SetScriptDoc('generate a mesh suitable for CFD from a surface')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['TargetEdgeLength','edgelength','float',1,'(0.0,)'],
            ['TargetEdgeLengthArrayName','edgelengtharray','str',1],
            ['TargetEdgeLengthFactor','edgelengthfactor','float',1,'(0.0,)'],
            ['TriangleSplitFactor','trianglesplitfactor','float',1,'(0.0,)'],
            ['EndcapsEdgeLengthFactor','endcapsedgelengthfactor','float',1,'(0.0,)'],
            ['MaxEdgeLength','maxedgelength','float',1,'(0.0,)'],
            ['MinEdgeLength','minedgelength','float',1,'(0.0,)'],
            ['CellEntityIdsArrayName','entityidsarray','str',1],
            ['ElementSizeMode','elementsizemode','str',1,'["edgelength","edgelengtharray"]'],
            ['CappingMethod','cappingmethod','str',1,'["simple","annular","concaveannular"]'],
            ['SkipCapping','skipcapping','bool',1,''],
            ['SkipRemeshing','skipremeshing','bool',1,''],
            ['VolumeElementScaleFactor','volumeelementfactor','float',1,'(0.0,)'],
            ['BoundaryLayer','boundarylayer','bool',1,''],
            ['NumberOfSubLayers','sublayers','int',1,'(0,)'],
            ['NumberOfSubsteps','substeps','int',1,'(0,)'],
            ['Relaxation','relaxation','float',1,'(0.0,)'],
            ['LocalCorrectionFactor','localcorrection','float',1,'(0.0,)'],
            ['SubLayerRatio','sublayerratio','float',1,'(0.0,)'],
            ['BoundaryLayerThicknessFactor','thicknessfactor','float',1,'(0.0,)'],
            ['RemeshCapsOnly','remeshcapsonly','bool',1,''],
            ['BoundaryLayerOnCaps','boundarylayeroncaps','bool',1,''],
            ['Tetrahedralize','tetrahedralize','bool',1,'']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter'],
            ['CellEntityIdsArrayName','entityidsarray','str',1],
            ['RemeshedSurface','remeshedsurface','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ])

    def Execute(self):

        from vmtk import vmtkscripts
        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        wallEntityOffset = 1

        if self.SkipCapping or not self.BoundaryLayerOnCaps:
            self.PrintLog("Not capping surface")
            surface = self.Surface
            cellEntityIdsArray = vtk.vtkIntArray()
            cellEntityIdsArray.SetName(self.CellEntityIdsArrayName)
            cellEntityIdsArray.SetNumberOfTuples(surface.GetNumberOfCells())
            cellEntityIdsArray.FillComponent(0,0.0)
            surface.GetCellData().AddArray(cellEntityIdsArray)
        else:
            self.PrintLog("Capping surface")
            capper = vmtkscripts.vmtkSurfaceCapper()
            capper.Surface = self.Surface
            capper.Interactive = 0
            capper.Method = self.CappingMethod
            capper.TriangleOutput = 0
            capper.CellEntityIdOffset = wallEntityOffset
            capper.CellEntityIdsArrayName = self.CellEntityIdsArrayName
            capper.Execute()
            surface = capper.Surface

        if self.SkipRemeshing:
            remeshedSurface = surface
        else:
            self.PrintLog("Remeshing surface")
            remeshing = vmtkscripts.vmtkSurfaceRemeshing()
            remeshing.Surface = surface
            remeshing.CellEntityIdsArrayName = self.CellEntityIdsArrayName
            remeshing.TargetEdgeLength = self.TargetEdgeLength
            remeshing.MaxEdgeLength = self.MaxEdgeLength
            remeshing.MinEdgeLength = self.MinEdgeLength
            remeshing.TargetEdgeLengthFactor = self.TargetEdgeLengthFactor
            remeshing.TargetEdgeLengthArrayName = self.TargetEdgeLengthArrayName
            remeshing.TriangleSplitFactor = self.TriangleSplitFactor
            remeshing.ElementSizeMode = self.ElementSizeMode
            if self.RemeshCapsOnly:
                remeshing.ExcludeEntityIds = [wallEntityOffset]
            remeshing.Execute()
            remeshedSurface = remeshing.Surface

        if self.BoundaryLayer:

            projection = vmtkscripts.vmtkSurfaceProjection()
            projection.Surface = remeshedSurface
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
            placeholderCellEntityId = 9999
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
            boundaryLayer.NumberOfSubsteps = self.NumberOfSubsteps
            boundaryLayer.Relaxation = self.Relaxation
            boundaryLayer.LocalCorrectionFactor = self.LocalCorrectionFactor
            boundaryLayer.SubLayerRatio = self.SubLayerRatio
            boundaryLayer.Thickness = self.BoundaryLayerThicknessFactor * self.TargetEdgeLength
            boundaryLayer.ThicknessRatio = self.BoundaryLayerThicknessFactor * self.TargetEdgeLengthFactor
            boundaryLayer.MaximumThickness = self.BoundaryLayerThicknessFactor * self.MaxEdgeLength
            if not self.BoundaryLayerOnCaps:
                boundaryLayer.SidewallCellEntityId = placeholderCellEntityId
                boundaryLayer.InnerSurfaceCellEntityId = wallEntityOffset
            boundaryLayer.Execute()

            meshToSurface = vmtkscripts.vmtkMeshToSurface()
            meshToSurface.Mesh = boundaryLayer.InnerSurfaceMesh
            meshToSurface.Execute()

            innerSurface = meshToSurface.Surface

            if not self.BoundaryLayerOnCaps:

                self.PrintLog("Capping inner surface")
                capper = vmtkscripts.vmtkSurfaceCapper()
                capper.Surface = innerSurface
                capper.Interactive = 0
                capper.Method = self.CappingMethod
                capper.TriangleOutput = 1
                capper.CellEntityIdOffset = wallEntityOffset
                capper.Execute()

                self.PrintLog("Remeshing endcaps")
                remeshing = vmtkscripts.vmtkSurfaceRemeshing()
                remeshing.Surface = capper.Surface
                remeshing.CellEntityIdsArrayName = self.CellEntityIdsArrayName
                remeshing.TargetEdgeLength = self.TargetEdgeLength * self.EndcapsEdgeLengthFactor
                remeshing.MaxEdgeLength = self.MaxEdgeLength
                remeshing.MinEdgeLength = self.MinEdgeLength
                remeshing.TargetEdgeLengthFactor = self.TargetEdgeLengthFactor * self.EndcapsEdgeLengthFactor
                remeshing.TargetEdgeLengthArrayName = self.TargetEdgeLengthArrayName
                remeshing.TriangleSplitFactor = self.TriangleSplitFactor
                remeshing.ElementSizeMode = self.ElementSizeMode
                remeshing.ExcludeEntityIds = [wallEntityOffset]
                remeshing.Execute()

                innerSurface = remeshing.Surface

            self.PrintLog("Computing sizing function")
            sizingFunction = vtkvmtk.vtkvmtkPolyDataSizingFunction()
            sizingFunction.SetInputData(innerSurface)
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

            #w = vtk.vtkXMLUnstructuredGridWriter()
            #w.SetInput(tetgen.Mesh)
            #w.SetFileName('tet.vtu')
            #w.Write()

            if tetgen.Mesh.GetNumberOfCells() == 0 and surfaceToMesh.Mesh.GetNumberOfCells() > 0:
                self.PrintLog('An error occurred during tetrahedralization. Will only output surface mesh and boundary layer.')

            if not self.BoundaryLayerOnCaps:
                surfaceToMesh.Mesh.GetCellData().GetArray(self.CellEntityIdsArrayName).FillComponent(0,wallEntityOffset)

            self.PrintLog("Assembling final mesh")
            appendFilter = vtkvmtk.vtkvmtkAppendFilter()
            appendFilter.AddInputData(surfaceToMesh.Mesh)
            appendFilter.AddInputData(boundaryLayer.Mesh)
            appendFilter.AddInputData(tetgen.Mesh)

            #appendFilter.AddInput(boundaryLayer.InnerSurfaceMesh)

            if not self.BoundaryLayerOnCaps:
                threshold = vtk.vtkThreshold()
                threshold.SetInputData(surfaceToMesh2.Mesh)
                threshold.ThresholdByUpper(1.5)
                threshold.SetInputArrayToProcess(0,0,0,1,self.CellEntityIdsArrayName)
                threshold.Update()
                endcaps = threshold.GetOutput()
                appendFilter.AddInputData(endcaps)

            appendFilter.Update()

            self.Mesh = appendFilter.GetOutput()

            if not self.BoundaryLayerOnCaps:
                cellEntityIdsArray = self.Mesh.GetCellData().GetArray(self.CellEntityIdsArrayName)

                def VisitNeighbors(i, cellEntityId):
                    cellPointIds = vtk.vtkIdList()
                    self.Mesh.GetCellPoints(i,cellPointIds)
                    neighborPointIds = vtk.vtkIdList()
                    neighborPointIds.SetNumberOfIds(1)
                    pointNeighborCellIds = vtk.vtkIdList()
                    neighborCellIds = vtk.vtkIdList()

                    for j in range(cellPointIds.GetNumberOfIds()):
                        neighborPointIds.SetId(0,cellPointIds.GetId(j))
                        self.Mesh.GetCellNeighbors(i,neighborPointIds,pointNeighborCellIds)
                        for k in range(pointNeighborCellIds.GetNumberOfIds()):
                            neighborCellIds.InsertNextId(pointNeighborCellIds.GetId(k))

                    for j in range(neighborCellIds.GetNumberOfIds()):
                        cellId = neighborCellIds.GetId(j)
                        neighborCellEntityId = cellEntityIdsArray.GetTuple1(cellId)
                        neighborCellType = self.Mesh.GetCellType(cellId)
                        if neighborCellType not in [vtk.VTK_TRIANGLE, vtk.VTK_QUADRATIC_TRIANGLE, vtk.VTK_QUAD]:
                            continue
                        if neighborCellEntityId != placeholderCellEntityId:
                            continue
                        cellEntityIdsArray.SetTuple1(cellId,cellEntityId)
                        VisitNeighbors(cellId,cellEntityId)

                for i in range(self.Mesh.GetNumberOfCells()):
                    cellEntityId = cellEntityIdsArray.GetTuple1(i)
                    cellType = self.Mesh.GetCellType(i)
                    if cellType not in [vtk.VTK_TRIANGLE, vtk.VTK_QUADRATIC_TRIANGLE, vtk.VTK_QUAD]:
                        continue
                    if cellEntityId in [0, 1, placeholderCellEntityId]:
                        continue
                    VisitNeighbors(i,cellEntityId)

        else:

            self.PrintLog("Computing sizing function")
            sizingFunction = vtkvmtk.vtkvmtkPolyDataSizingFunction()
            sizingFunction.SetInputData(remeshedSurface)
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

            tetrahedralize = vtkvmtk.vtkvmtkUnstructuredGridTetraFilter()
            tetrahedralize.SetInputData(self.Mesh)
            tetrahedralize.Update()

            self.Mesh = tetrahedralize.GetOutput()

        self.RemeshedSurface = remeshedSurface


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
