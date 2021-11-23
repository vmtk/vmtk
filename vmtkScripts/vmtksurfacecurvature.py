#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacecurvature.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.8 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys
from vmtk import vtkvmtk

from vmtk import pypes


class vmtkSurfaceCurvature(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.ReferenceSurface = None

        self.CurvatureType = 'mean'

        self.AbsoluteCurvature = 0
        self.CurvatureOnBoundaries = 0
        self.MedianFiltering = 0
        self.BoundedReciprocal = 0
        self.Epsilon = 1.0
        self.Offset = 0.0

        self.SetScriptName('vmtksurfacecurvature')
        self.SetScriptDoc('compute curvature of an input surface, optionally reporting results back to a reference surface (by point id)')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['ReferenceSurface','r','vtkPolyData',1,'','the reference surface','vmtksurfacereader'],
            ['CurvatureType','type','str',1,'["mean","gaussian","maximum","minimum"]','the type of surface curvature to compute'],
            ['AbsoluteCurvature','absolute','bool',1,'','output the absolute value of curvature'],
            ['MedianFiltering','median','bool',1,'','output curvature after median filtering to suppress numerical noise speckles'],
            ['CurvatureOnBoundaries','boundaries','bool',1,'','compute curvature on boundaries'],
            ['BoundedReciprocal','reciprocal','bool',1,'','output bounded reciprocal of curvature'],
            ['Epsilon','epsilon','float',1,'','bounded reciprocal epsilon at the denominator'],
            ['Offset','offset','float',1,'','offset curvature by the specified value']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        curvatureFilter = vtk.vtkCurvatures()
        curvatureFilter.SetInputData(self.Surface)
        if self.CurvatureType == 'mean':
            curvatureFilter.SetCurvatureTypeToMean()
        elif self.CurvatureType == 'gaussian':
            curvatureFilter.SetCurvatureTypeToGaussian()
        elif self.CurvatureType == 'maximum':
            curvatureFilter.SetCurvatureTypeToMaximum()
        elif self.CurvatureType == 'minimum':
            curvatureFilter.SetCurvatureTypeToMinimum()
        curvatureFilter.Update()

        activeScalars = curvatureFilter.GetOutput().GetPointData().GetScalars()
        activeScalars.SetName('Curvature')

        if self.AbsoluteCurvature:
            for i in range(activeScalars.GetNumberOfTuples()):
                value = activeScalars.GetTuple1(i)
                activeScalars.SetTuple1(i,abs(value))

        neighborhoods = None
        if not self.CurvatureOnBoundaries or self.MedianFiltering:
            neighborhoods = vtkvmtk.vtkvmtkNeighborhoods()
            neighborhoods.SetNeighborhoodTypeToPolyDataManifoldNeighborhood()
            neighborhoods.SetDataSet(self.Surface)
            neighborhoods.Build()

        if not self.CurvatureOnBoundaries:
            boundaryExtractor = vtkvmtk.vtkvmtkPolyDataBoundaryExtractor()
            boundaryExtractor.SetInputData(self.Surface)
            boundaryExtractor.Update()
            boundaryIdsArray = vtk.vtkIdTypeArray.SafeDownCast(boundaryExtractor.GetOutput().GetPointData().GetScalars())
            boundaryIds = vtk.vtkIdList()
            boundaryIds.SetNumberOfIds(boundaryIdsArray.GetNumberOfTuples())
            for i in range(boundaryIdsArray.GetNumberOfTuples()):
                boundaryIds.SetId(i,boundaryIdsArray.GetValue(i))
            self.Surface.BuildLinks()
            for i in range(boundaryIds.GetNumberOfIds()):
                pointId = boundaryIds.GetId(i)
                neighborhood = neighborhoods.GetNeighborhood(pointId)
                values = []
                for j in range(neighborhood.GetNumberOfPoints()):
                    neighborId = neighborhood.GetPointId(j)
                    if boundaryIds.IsId(neighborId) != -1:
                        continue
                    value = activeScalars.GetTuple1(neighborId)
                    values.append(value)
                values.sort()
                if not values:
                    continue
                medianValue = values[(len(values) - 1)//2]
                activeScalars.SetTuple1(pointId,medianValue)

        if self.MedianFiltering:
            for i in range(neighborhoods.GetNumberOfNeighborhoods()):
                neighborhood = neighborhoods.GetNeighborhood(i)
                values = []
                for j in range(neighborhood.GetNumberOfPoints()):
                    neighborId = neighborhood.GetPointId(j)
                    value = activeScalars.GetTuple1(neighborId)
                    values.append(value)
                values.sort()
                if not values:
                    continue
                medianValue = values[(len(values) - 1)//2]
                activeScalars.SetTuple1(i,medianValue)

        if self.BoundedReciprocal:
            for i in range(activeScalars.GetNumberOfTuples()):
                value = activeScalars.GetTuple1(i)
                reciprocalValue = 1.0 / (self.Epsilon + value)
                activeScalars.SetTuple1(i,reciprocalValue)

        if self.Offset:
            for i in range(activeScalars.GetNumberOfTuples()):
                value = activeScalars.GetTuple1(i)
                activeScalars.SetTuple1(i,value + self.Offset)

        if self.ReferenceSurface == None:
            self.Surface.GetPointData().AddArray(activeScalars)
        else:
            self.ReferenceSurface.GetPointData().AddArray(activeScalars)
            self.Surface = self.ReferenceSurface


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
