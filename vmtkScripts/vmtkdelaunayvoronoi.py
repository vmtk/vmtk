#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkdelaunayvoronoi.py,v $
## Language:  Python
## Date:      $Date: 2006/07/17 09:52:56 $
## Version:   $Revision: 1.20 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import vtkvmtk
from vmtk import vmtkrenderer
from vmtk import pypes


class vmtkNonManifoldSurfaceChecker(object):

    def __init__(self):

        self.Surface = 0

        self.NumberOfNonManifoldEdges = 0
        self.Report = 0
        self.NonManifoldEdgePointIds = vtk.vtkIdList()

        self.PrintError = None

    def Execute(self):

        if (self.Surface == 0):
            self.PrintError('NonManifoldSurfaceChecker error: Surface not set')
            return

        self.NonManifoldEdgesFound = 0
        self.Report = ''
        self.NonManifoldEdgePointIds.Initialize()

        neighborhoods = vtkvmtk.vtkvmtkNeighborhoods()
        neighborhoods.SetNeighborhoodTypeToPolyDataManifoldNeighborhood()
        neighborhoods.SetDataSet(self.Surface)
        neighborhoods.Build()

        neighborCellIds = vtk.vtkIdList()
        cellPointIds = vtk.vtkIdList()

        self.Surface.BuildCells()
        self.Surface.BuildLinks(0)

        numberOfNonManifoldEdges = 0

        for i in range(neighborhoods.GetNumberOfNeighborhoods()):

            neighborhood = neighborhoods.GetNeighborhood(i)

            for j in range(neighborhood.GetNumberOfPoints()):

                neighborId = neighborhood.GetPointId(j)

                if (i<neighborId):

                    neighborCellIds.Initialize()
                    self.Surface.GetCellEdgeNeighbors(-1,i,neighborId,neighborCellIds)

                    if (neighborCellIds.GetNumberOfIds()>2):

                        numberOfNonManifoldEdges = numberOfNonManifoldEdges + 1

                        self.Report = self.Report +  "Non-manifold edge found" + str(i) + ' ' + str(neighborId) + '.\n'

                        self.NonManifoldEdgePointIds.InsertNextId(i)
                        self.NonManifoldEdgePointIds.InsertNextId(neighborId)


class vmtkDelaunayVoronoi(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.FlipNormals = 0
        self.CapDisplacement = 0.0
        self.RadiusArrayName = 'MaximumInscribedSphereRadius'
        self.CheckNonManifold = 0

        self.RemoveSubresolutionTetrahedra = 0
        self.SubresolutionFactor = 1.0

        self.SimplifyVoronoi = 0

        self.UseTetGen = 0
        self.TetGenDetectInter = 1

        self.DelaunayTolerance = 0.001

        self.DelaunayTessellation = None
        self.VoronoiDiagram = None
        self.PoleIds = None

        self.Mesh = None

        self.SetScriptName('vmtkdelaunayvoronoi')
        self.SetScriptDoc('calculate the delaunay tesellation, voronoi diagram, and voronoi poleIds of an input surface')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['CheckNonManifold','nonmanifoldcheck','bool',1,'','toggle checking the surface for non-manifold edges'],
            ['FlipNormals','flipnormals','bool',1,'','flip normals after outward normal computation; outward oriented normals must be computed for the removal of outer tetrahedra; the algorithm might fail so for weird geometries, so changing this might solve the problem'],
            ['CapDisplacement','capdisplacement','float',1,'','displacement of the center points of caps at open profiles along their normals (avoids the creation of degenerate tetrahedra)'],
            ['DelaunayTolerance','delaunaytolerance','float',1,'','tolerance for evaluating coincident points during Delaunay tessellation, evaluated as a fraction of the bounding box'],
            ['RadiusArrayName','radiusarray','str',1,'','name of the array where radius values of maximal inscribed spheres have to be stored'],
            ['DelaunayTessellation','delaunaytessellation','vtkUnstructuredGrid',1,'','optional input Delaunay tessellation'],
            ['RemoveSubresolutionTetrahedra','removesubresolution','bool',1,'','toggle removal of subresolution tetrahedra from Delaunay tessellation'],
            ['SubresolutionFactor','subresolutionfactor','float',1,'(0.0,)','factor for removal of subresolution tetrahedra, expressing the size of the circumsphere relative to the local edge length size of surface triangles'],
            ['SimplifyVoronoi','simplifyvoronoi','bool',1,'','toggle simplification of Voronoi diagram'],
            ['UseTetGen','usetetgen','bool',1,'','toggle use TetGen to compute Delaunay tessellation'],
            ['TetGenDetectInter','tetgendetectinter','bool',1,'','TetGen option']])
        self.SetOutputMembers([
            ['RadiusArrayName','radiusarray','str',1,'','name of the array where radius values of maximal inscribed spheres are stored'],
            ['DelaunayTessellation','delaunaytessellation','vtkUnstructuredGrid',1,'','','vmtkmeshwriter'],
            ['VoronoiDiagram','voronoidiagram','vtkPolyData',1,'','','vmtksurfacewriter'],
            ['Mesh','omesh','vtkUnstructuredGrid',1,'','conveniently named DelaunayTessellation output','vmtkmeshwriter'],
            ['Surface','osurface','vtkPolyData',1,'','conveniently named VoronoiDiagram output','vmtksurfacewriter'],
            ['PoleIds','poleids','vtkIdList',1]])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        if self.CheckNonManifold:
            self.PrintLog('NonManifold check.')
            nonManifoldChecker = vmtkNonManifoldSurfaceChecker()
            nonManifoldChecker.Surface = self.Surface
            nonManifoldChecker.PrintError = self.PrintError
            nonManifoldChecker.Execute()

            if nonManifoldChecker.NumberOfNonManifoldEdges > 0:
                self.PrintLog(nonManifoldChecker.Report)
                return

        self.PrintLog('Cleaning surface.')
        surfaceCleaner = vtk.vtkCleanPolyData()
        surfaceCleaner.SetInputData(self.Surface)
        surfaceCleaner.Update()

        self.PrintLog('Triangulating surface.')
        surfaceTriangulator = vtk.vtkTriangleFilter()
        surfaceTriangulator.SetInputConnection(surfaceCleaner.GetOutputPort())
        surfaceTriangulator.PassLinesOff()
        surfaceTriangulator.PassVertsOff()
        surfaceTriangulator.Update()

        surfaceCapper = vtkvmtk.vtkvmtkCapPolyData()
        surfaceCapper.SetInputConnection(surfaceTriangulator.GetOutputPort())
        surfaceCapper.SetDisplacement(self.CapDisplacement)
        surfaceCapper.SetInPlaneDisplacement(self.CapDisplacement)
        surfaceCapper.Update()

        capCenterIds = surfaceCapper.GetCapCenterIds()

        surfaceNormals = vtk.vtkPolyDataNormals()
        surfaceNormals.SetInputConnection(surfaceCapper.GetOutputPort())
        surfaceNormals.SplittingOff()
        surfaceNormals.AutoOrientNormalsOn()
        surfaceNormals.SetFlipNormals(self.FlipNormals)
        surfaceNormals.ComputePointNormalsOn()
        surfaceNormals.ConsistencyOn()
        surfaceNormals.Update()

        inputSurface = surfaceNormals.GetOutput()

        if self.UseTetGen:
            self.PrintLog('Running TetGen.')
            from vmtk import vmtkscripts
            surfaceToMesh = vmtkscripts.vmtkSurfaceToMesh()
            surfaceToMesh.Surface = inputSurface
            surfaceToMesh.Execute()
            tetgen = vmtkscripts.vmtkTetGen()
            tetgen.Mesh = surfaceToMesh.Mesh
            tetgen.PLC = 1
            tetgen.NoMerge = 1
            tetgen.Quality = 0
            if self.TetGenDetectInter:
                tetgen.DetectInter = 1
                tetgen.NoMerge = 0
            tetgen.OutputSurfaceElements = 0
            tetgen.Execute()
            self.DelaunayTessellation = tetgen.Mesh
        else:
            delaunayTessellator = vtk.vtkDelaunay3D()
            delaunayTessellator.CreateDefaultLocator()
            delaunayTessellator.SetInputConnection(surfaceNormals.GetOutputPort())
            delaunayTessellator.SetTolerance(self.DelaunayTolerance)
            delaunayTessellator.Update()
            self.DelaunayTessellation = delaunayTessellator.GetOutput()

        normalsArray = surfaceNormals.GetOutput().GetPointData().GetNormals()
        self.DelaunayTessellation.GetPointData().AddArray(normalsArray)

        internalTetrahedraExtractor = vtkvmtk.vtkvmtkInternalTetrahedraExtractor()
        internalTetrahedraExtractor.SetInputData(self.DelaunayTessellation)
        internalTetrahedraExtractor.SetOutwardNormalsArrayName(normalsArray.GetName())
        if self.RemoveSubresolutionTetrahedra:
            internalTetrahedraExtractor.RemoveSubresolutionTetrahedraOn()
            internalTetrahedraExtractor.SetSubresolutionFactor(self.SubresolutionFactor)
            internalTetrahedraExtractor.SetSurface(inputSurface)
        if capCenterIds.GetNumberOfIds() > 0:
          internalTetrahedraExtractor.UseCapsOn()
          internalTetrahedraExtractor.SetCapCenterIds(capCenterIds)
        internalTetrahedraExtractor.Update()

        self.DelaunayTessellation = internalTetrahedraExtractor.GetOutput()

        voronoiDiagramFilter = vtkvmtk.vtkvmtkVoronoiDiagram3D()
        voronoiDiagramFilter.SetInputData(self.DelaunayTessellation)
        voronoiDiagramFilter.SetRadiusArrayName(self.RadiusArrayName)
        voronoiDiagramFilter.Update()

        self.PoleIds = voronoiDiagramFilter.GetPoleIds()

        self.VoronoiDiagram = voronoiDiagramFilter.GetOutput()

        if self.SimplifyVoronoi:
            voronoiDiagramSimplifier = vtkvmtk.vtkvmtkSimplifyVoronoiDiagram()
            voronoiDiagramSimplifier.SetInputConnection(voronoiDiagramFilter.GetOutputPort())
            voronoiDiagramSimplifier.SetUnremovablePointIds(voronoiDiagramFilter.GetPoleIds())
            voronoiDiagramSimplifier.Update()
            self.VoronoiDiagram = voronoiDiagramSimplifier.GetOutput()

        self.Mesh = self.DelaunayTessellation
        self.Surface = self.VoronoiDiagram


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
