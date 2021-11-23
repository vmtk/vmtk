#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtklocalgeometry.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:48:31 $
## Version:   $Revision: 1.5 $

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


class vmtkLocalGeometry(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Centerlines = None
        self.VoronoiDiagram = None
        self.PoleIds = None

        self.EdgeArrayName = 'EdgeArray'

        self.VoronoiGeodesicDistanceArrayName = 'VoronoiGeodesicDistance'

        self.VoronoiPoleCenterlineVectorsArrayName = 'VoronoiPoleCenterlineVectors'
        self.VoronoiCellIdsArrayName = 'VoronoiCellIds'
        self.VoronoiPCoordsArrayName = 'VoronoiPCoords'

        self.ComputePoleVectors = 0
        self.ComputeGeodesicDistance = 0
        self.ComputeNormalizedTangencyDeviation = 0
        self.ComputeEuclideanDistance = 0
        self.ComputeCenterlineVectors = 0
        self.ComputeCellIds = 0
        self.ComputePCoords = 0

        self.AdjustBoundaryValues = 0

        self.PoleVectorsArrayName = 'PoleVectors'
        self.GeodesicDistanceArrayName = 'GeodesicDistance'
        self.NormalizedTangencyDeviationArrayName = 'NTD'
        self.EuclideanDistanceArrayName = 'EuclideanDistance'
        self.CenterlineVectorsArrayName = 'CenterlineVectors'
        self.CellIdsArrayName = 'CellIdsArray'
        self.PCoordsArrayName = 'PCoordsArray'

        self.SetScriptName('vmtklocalgeometry')
        self.SetScriptDoc('compute local geometric properties of a surface')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','','vmtksurfacereader'],
            ['Centerlines','centerlines','vtkPolyData',1,'','','vmtksurfacereader'],
            ['VoronoiDiagram','voronoidiagram','vtkPolyData',1,'','','vmtksurfacereader'],
            ['PoleIds','poleids','vtkIdList',1],
            ['EdgeArrayName','edgearray','str',1],
            ['VoronoiGeodesicDistanceArrayName','voronoigeodesicdistancearray','str',1],
            ['VoronoiPoleCenterlineVectorsArrayName','voronoipolecenterlinevectorsarray','str',1],
            ['VoronoiCellIdsArrayName','voronoicellidsarray','str',1],
            ['VoronoiPCoordsArrayName','voronoipcoordsarray','str',1],
            ['ComputePoleVectors','computepolevectors','bool',1],
            ['ComputeGeodesicDistance','computegeodesicdistance','bool',1],
            ['ComputeNormalizedTangencyDeviation','computentd','bool',1],
            ['ComputeEuclideanDistance','computeeuclideandistance','bool',1],
            ['ComputeCenterlineVectors','computecenterlinevectors','bool',1],
            ['ComputeCellIds','computecellids','bool',1],
            ['ComputePCoords','computepcoords','bool',1],
            ['AdjustBoundaryValues','adjustboundaryvalues','bool',1],
            ['PoleVectorsArrayName','polevectorsarray','str',1],
            ['GeodesicDistanceArrayName','geodesicdistancearray','str',1],
            ['NormalizedTangencyDeviationArrayName','ntdarray','str',1],
            ['EuclideanDistanceArrayName','euclideandistancearray','str',1],
            ['CenterlineVectorsArrayName','centerlinevectorsarray','str',1],
            ['CellIdsArrayName','cellidsarray','str',1],
            ['PCoordsArrayName','pcoordsarray','str',1]
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','','vmtksurfacewriter'],
            ['PoleVectorsArrayName','polevectorsarray','str',1],
            ['GeodesicDistanceArrayName','geodesicdistancearray','str',1],
            ['NormalizedTangencyDeviationArrayName','ntdarray','str',1],
            ['EuclideanDistanceArrayName','euclideandistancearray','str',1],
            ['CenterlineVectorsArrayName','centerlinevectorsarray','str',1],
            ['CellIdsArrayName','cellidsarray','str',1],
            ['PCoordsArrayName','pcoordsarray','str',1]
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        if self.VoronoiDiagram == None:
            self.PrintError('Error: No input Voronoi diagram.')

        if self.PoleIds == None:
            self.PrintError('Error: No input pole ids.')

        voronoi = self.VoronoiDiagram

        if self.ComputeGeodesicDistance | self.ComputeNormalizedTangencyDeviation | self.ComputeEuclideanDistance | self.ComputeCenterlineVectors | self.ComputeCellIds | self.ComputePCoords:

            if self.EdgeArrayName == '':
                self.PrintError('Error: No input edge array name.')

            voronoiRadialFastMarching = vtkvmtk.vtkvmtkNonManifoldFastMarching()
            voronoiRadialFastMarching.SetInputData(voronoi)
            voronoiRadialFastMarching.UnitSpeedOn()
            voronoiRadialFastMarching.SetSolutionArrayName(self.VoronoiGeodesicDistanceArrayName)
            voronoiRadialFastMarching.PolyDataBoundaryConditionsOn()
            voronoiRadialFastMarching.SetBoundaryPolyData(self.Centerlines)
            voronoiRadialFastMarching.SetIntersectedEdgesArrayName(self.EdgeArrayName)
            voronoiRadialFastMarching.Update()
            voronoi = voronoiRadialFastMarching.GetOutput()

        if self.ComputeEuclideanDistance | self.ComputeCenterlineVectors | self.ComputeCellIds | self.ComputePCoords:

            voronoiShooter = vtkvmtk.vtkvmtkSteepestDescentShooter()
            voronoiShooter.SetInputData(voronoi)
            voronoiShooter.SetTarget(self.Centerlines)
            voronoiShooter.SetDescentArrayName(self.VoronoiGeodesicDistanceArrayName)
            voronoiShooter.SetEdgeArrayName(self.EdgeArrayName)
            voronoiShooter.SetSeeds(self.PoleIds)
            voronoiShooter.SetTargetVectorsArrayName(self.VoronoiPoleCenterlineVectorsArrayName)
            voronoiShooter.SetTargetCellIdsArrayName(self.VoronoiCellIdsArrayName)
            voronoiShooter.Update()
            voronoi = voronoiShooter.GetOutput()

        surfaceLocalGeometry = vtkvmtk.vtkvmtkPolyDataLocalGeometry()

        surfaceLocalGeometry.SetInputData(self.Surface)
        surfaceLocalGeometry.SetVoronoiDiagram(voronoi)
        surfaceLocalGeometry.SetVoronoiGeodesicDistanceArrayName(self.VoronoiGeodesicDistanceArrayName)
        surfaceLocalGeometry.SetPoleIds(self.PoleIds)

        surfaceLocalGeometry.SetVoronoiPoleCenterlineVectorsArrayName(self.VoronoiPoleCenterlineVectorsArrayName)
        surfaceLocalGeometry.SetVoronoiCellIdsArrayName(self.VoronoiCellIdsArrayName)
        surfaceLocalGeometry.SetVoronoiPCoordsArrayName(self.VoronoiPCoordsArrayName)

        surfaceLocalGeometry.SetComputePoleVectors(self.ComputePoleVectors)
        surfaceLocalGeometry.SetComputeGeodesicDistance(self.ComputeGeodesicDistance)
        surfaceLocalGeometry.SetComputeNormalizedTangencyDeviation(self.ComputeNormalizedTangencyDeviation)
        surfaceLocalGeometry.SetComputeEuclideanDistance(self.ComputeEuclideanDistance)
        surfaceLocalGeometry.SetComputeCenterlineVectors(self.ComputeCenterlineVectors)
        surfaceLocalGeometry.SetComputeCellIds(self.ComputeCellIds)
        surfaceLocalGeometry.SetComputePCoords(self.ComputePCoords)

        surfaceLocalGeometry.SetAdjustBoundaryValues(self.AdjustBoundaryValues)

        surfaceLocalGeometry.SetPoleVectorsArrayName(self.PoleVectorsArrayName)
        surfaceLocalGeometry.SetGeodesicDistanceArrayName(self.GeodesicDistanceArrayName)
        surfaceLocalGeometry.SetNormalizedTangencyDeviationArrayName(self.NormalizedTangencyDeviationArrayName)
        surfaceLocalGeometry.SetEuclideanDistanceArrayName(self.EuclideanDistanceArrayName)
        surfaceLocalGeometry.SetCenterlineVectorsArrayName(self.CenterlineVectorsArrayName)
        surfaceLocalGeometry.SetCellIdsArrayName(self.CellIdsArrayName)
        surfaceLocalGeometry.SetPCoordsArrayName(self.PCoordsArrayName)

        surfaceLocalGeometry.Update()

        self.Surface = surfaceLocalGeometry.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
