#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkcenterlinesnetwork.py,v $
## Language:  Python
## Date:      $Date: 2018/04/24 09:52:56 $
## Version:   $Revision: 1.20 $

##   Copyright (c) Richard Izzo, Luca Antiga. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import vtkvmtk
from vmtk import pypes
from vmtk import vmtkcenterlines, vmtkcenterlinestonumpy, vmtknetworkextraction, vmtkdelaunayvoronoi, vmtknumpytocenterlines, vmtksurfacecapper
from joblib import Parallel, delayed
import random
import numpy as np

def _compute_centerlines(surfaceAddress, delaunayAddress, voronoiAddress, poleIdsAddress, cell, points):
    '''a method to compute centerlines which can be called in parallel
    
    Arguments:
        surfaceAddress (str): the input memory address of the surface to calculate centerlines of
        delaunayAddress (str): the memory address of a previously computed delaunay triangulation of the 
            surface vtkUnstructuredGrid 
        voronoiAddress (str): the memory address of the previously computed voronoi diagram vtkPolyData
        poleIdsAddress (str): the memory address of the previously computed poleIds vtkIdList
        cell (np.array): the cellID connectivity list
        points (np.array): the x,y,z coordinates of points identified in the cell argument
    '''
    cellStartIdx = cell[0]
    cellEndIdx = cell[-1]
    cellStartPoint = points[cellStartIdx].tolist()
    cellEndPoint = points[cellEndIdx].tolist()
    
    surface = vtk.vtkPolyData(surfaceAddress)
    delaunay = vtk.vtkUnstructuredGrid(delaunayAddress)
    voronoi = vtk.vtkPolyData(voronoiAddress)
    poleIds = vtk.vtkIdList(poleIdsAddress)
    
    cl = vmtkcenterlines.vmtkCenterlines()
    cl.Surface = surface
    cl.DelaunayTessellation = delaunay
    cl.VoronoiDiagram = voronoi
    cl.PoleIds = poleIds
    cl.SeedSelectorName = 'pointlist'
    cl.SourcePoints = cellStartPoint
    cl.TargetPoints = cellEndPoint
    cl.LogOn = 0
    cl.Execute()
    
    clConvert = vmtkcenterlinestonumpy.vmtkCenterlinesToNumpy()
    clConvert.Centerlines = cl.Centerlines
    clConvert.LogOn = 0
    clConvert.Execute()
    return clConvert.ArrayDict

class vmtkCenterlinesNetwork(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Surface = None
        self.Centerlines = None
        self.RadiusArrayName = 'MaximumInscribedSphereRadius'
        self.CostFunction = '1/R'

        self.EikonalSolutionArrayName = 'EikonalSolution'
        self.EdgeArrayName = 'EdgeArray'
        self.EdgePCoordArrayName = 'EdgePCoordArray'
        self.CostFunctionArrayName = 'CostFunctionArray'

        self.DelaunayTessellation = None
        self.VoronoiDiagram = None
        self.PoleIds = None

        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.SetScriptName('vmtkcenterlinesnetwork')
        self.SetScriptDoc('compute centerlines from a branching tubular surface automatically.')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['RadiusArrayName','radiusarray','str',1,'','name of the array where radius values of maximal inscribed spheres have to be stored'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']])
        self.SetOutputMembers([
            ['Centerlines','o','vtkPolyData',1,'','the output centerlines','vmtksurfacewriter'],
            ['RadiusArrayName','radiusarray','str',1,'','name of the array where radius values of maximal inscribed spheres are stored'],
            ['EikonalSolutionArrayName','eikonalsolutionarray','str',1],
            ['EdgeArrayName','edgearray','str',1],
            ['EdgePCoordArrayName','edgepcoordarray','str',1],
            ['CostFunctionArrayName','costfunctionarray','str',1],
            ['DelaunayTessellation','delaunaytessellation','vtkUnstructuredGrid',1,'','','vmtkmeshwriter'],
            ['VoronoiDiagram','voronoidiagram','vtkPolyData',1,'','','vmtksurfacewriter'],
            ['PoleIds','poleids','vtkIdList',1]])

    def Execute(self):
        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        # feature edges are used to find any holes in the surface. 
        fedges = vtk.vtkFeatureEdges()
        fedges.BoundaryEdgesOn()
        fedges.FeatureEdgesOff()
        fedges.ManifoldEdgesOff()
        fedges.SetInputData(self.Surface)
        fedges.Update()
        ofedges = fedges.GetOutput()

        # if numEdges is not 0, then there are holes which need to be capped
        numEdges = ofedges.GetNumberOfPoints()
        if numEdges != 0:
            tempcapper = vmtksurfacecapper.vmtkSurfaceCapper()
            tempcapper.Surface = self.Surface
            tempcapper.Interactive = 0
            tempcapper.Execute()
            
            networkSurface = tempcapper.Surface
        else:
            networkSurface = self.Surface

        # randomly select one cell to delete so that there is an opening for
        # vmtkNetworkExtraction to use.
        numCells = self.Surface.GetNumberOfCells()
        cellToDelete = random.randrange(0, numCells-1)
        networkSurface.BuildLinks()
        networkSurface.DeleteCell(cellToDelete)
        networkSurface.RemoveDeletedCells()

        # extract the network of approximated centerlines
        net = vmtknetworkextraction.vmtkNetworkExtraction()
        net.Surface = networkSurface
        net.AdvancementRatio = 1.001
        net.Execute()
        network = net.Network

        convert = vmtkcenterlinestonumpy.vmtkCenterlinesToNumpy()
        convert.Centerlines = network
        convert.Execute()
        ad = convert.ArrayDict
        cellDataTopology = ad['CellData']['Topology']

        # the network topology identifies an the input segment with the "0" id.
        # since we artificially created this segment, we don't want to use the
        # ends of the segment as source/target points of the centerline calculation
        nodeIndexToIgnore = np.where(cellDataTopology[:,0] == 0)[0][0]
        keepCellConnectivityList = []
        pointIdxToKeep = np.array([])
        # we remove the cell, points, and point data which are associated with the
        # segment we want to ignore
        for loopIdx, cellConnectivityList in enumerate(ad['CellData']['CellPointIds']):
            if loopIdx == nodeIndexToIgnore:
                removeCellStartIdx = cellConnectivityList[0]
                removeCellEndIdx = cellConnectivityList[-1]
                removeCellLength = cellConnectivityList.size
                if (removeCellEndIdx + 1) - removeCellStartIdx != removeCellLength:
                    raise(ValueError)
                continue
            else:
                rescaledCellConnectivity = np.subtract(cellConnectivityList, removeCellLength, where=cellConnectivityList >= removeCellLength)
                keepCellConnectivityList.append(rescaledCellConnectivity)
                pointIdxToKeep = np.concatenate((pointIdxToKeep, cellConnectivityList)).astype(np.int)
        newPoints = ad['Points'][pointIdxToKeep]
        newRadius = ad['PointData']['Radius'][pointIdxToKeep]

        # precompute the delaunay tessellation for the whole surface. 
        tessalation = vmtkdelaunayvoronoi.vmtkDelaunayVoronoi()
        tessalation.Surface = networkSurface
        tessalation.Execute()
        self.DelaunayTessellation = tessalation.DelaunayTessellation
        self.VoronoiDiagram = tessalation.VoronoiDiagram
        self.PoleIds = tessalation.PoleIds

        # vtk objects cannot be serialized in python. Instead of converting the inputs to numpy arrays and having
        # to reconstruct the vtk object each time the loop executes (a slow process), we can just pass in the
        # memory address of the data objects as a string, and use the vtk python bindings to create a python name 
        # referring to the data residing at that memory address. This works because joblib executes each loop
        # iteration in a fork of the original process, providing access to the original memory space. 
        # However, the process does not work for return arguments, since the original process will not have access to
        # the memory space of the fork. To return results we use the vmtkCenterlinesToNumpy converter.
        networkSurfaceMemoryAddress = networkSurface.__this__
        delaunayMemoryAddress = tessalation.DelaunayTessellation.__this__
        voronoiMemoryAddress = tessalation.VoronoiDiagram.__this__
        poleIdsMemoryAddress = tessalation.PoleIds.__this__

        outlist = Parallel(n_jobs=-1, backend='multiprocessing', verbose=20)(
            delayed(_compute_centerlines)(networkSurfaceMemoryAddress,
                                          delaunayMemoryAddress,
                                          voronoiMemoryAddress,
                                          poleIdsMemoryAddress,
                                          cell,
                                          newPoints) for cell in keepCellConnectivityList)

        out = []
        for item in outlist:
            npConvert = vmtknumpytocenterlines.vmtkNumpyToCenterlines()
            npConvert.ArrayDict = item
            npConvert.LogOn = 0
            npConvert.Execute()
            out.append(npConvert.Centerlines)

        # append each segment's polydata into a single polydata object
        new = vtk.vtkAppendPolyData()
        for data in out:  
            new.AddInputData(data)
        new.Update()

        self.Centerlines = new.GetOutput()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()