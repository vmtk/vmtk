#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkboundarylayer2.py,v $
## Language:  Python
## Date:      $$
## Version:   $$

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Tangui Morvan
##       Kalkulo AS
##       Simula Research Laboratory

## Extended version of vmtkboundarylayer2
##
## This version allows finer control on the surfaces to be included.
## If the original surface has open profiles, a surface can be extracted from them as well.
## The points on the open profiles may be specified with an array (OpenProfilesIdsArrayName).
## A value of -1 in this array specifies a point on the interior of the surface.
## Cell entity ids can also be output for the volume and surface elements.
## Surface elements are numbered in increasing numbers, using the openProfilesIds if specified.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
from vmtk import vtkvmtk
import sys

from vmtk import pypes


class vmtkBoundaryLayer2(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.InnerSurfaceMesh = None

        self.WarpVectorsArrayName = 'Normals'
        self.ThicknessArrayName = ''

        self.Thickness = 1.0
        self.ThicknessRatio = 0.1
        self.MaximumThickness = 1E10

        self.NumberOfSubLayers = 1
        self.SubLayerRatio = 1.0

        self.UseWarpVectorMagnitudeAsThickness = 0;
        self.ConstantThickness = 0
        self.IncludeSurfaceCells = 1
        self.NegateWarpVectors = 0

        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.OpenProfilesIdsArrayName = ''

        self.IncludeExtrudedOpenProfilesCells = 1
        self.IncludeExtrudedSurfaceCells = 1
        self.IncludeOriginalSurfaceCells = 1

        self.LayerEntityId = 0
        self.SurfaceEntityId = 1
        self.OpenProfilesEntityId = 2

        self.SetScriptName('vmtkboundarylayer2')
        self.SetScriptDoc('create a prismatic boundary layer from a surface mesh and a set of vectors defined on the nodes')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['WarpVectorsArrayName','warpvectorsarray','str',1,'','name of the array where warp vectors are stored'],
            ['ThicknessArrayName','thicknessarray','str',1,'','name of the array where scalars defining boundary layer thickness are stored'],
            ['Thickness','thickness','float',1,'','value of constant boundary layer thickness'],
            ['ThicknessRatio','thicknessratio','float',1,'(0.0,)','multiplying factor for boundary layer thickness'],
            ['MaximumThickness','maximumthickness','float',1,'','maximum allowed value for boundary layer thickness'],
            ['NumberOfSubLayers','numberofsublayers','int',1,'(0,)','number of sublayers which the boundary layer has to be made of'],
            ['SubLayerRatio','sublayerratio','float',1,'(0.0,)','ratio between the thickness of two successive boundary layers'],
            ['UseWarpVectorMagnitudeAsThickness','warpvectormagnitudeasthickness','bool',1,'','compute boundary layer thickness as the norm of warp vectors'],
            ['ConstantThickness','constantthickness','bool',1,'','toggle constant boundary layer thickness'],
            ['IncludeSurfaceCells','includesurfacecells','bool',1,'','include all surface cells in output mesh'],
            ['NegateWarpVectors','negatewarpvectors','bool',1,'','flip the orientation of warp vectors'],
            ['CellEntityIdsArrayName','cellentityidsarray','str',1,'','name of the array where the cell entity ids will be stored'],
            ['OpenProfilesIdsArrayName','openprofilesidsarray','str',1,'','name of the array indicating which points are on open profiles'],
            ['IncludeExtrudedOpenProfilesCells','includeextrudedopenprofilescells','bool',1,'','include the cells from the open profiles extruded surface in output mesh'],
            ['IncludeExtrudedSurfaceCells','includeextrudedsurfacecells','bool',1,'','include the cells from the extruded surface in output mesh'],
            ['IncludeOriginalSurfaceCells','includeoriginalsurfacecells','bool',1,'','include the cells from the original surface in output mesh'],
            ['LayerEntityId','layerentityid','int',1,'','id assigned to the volumetric layer'],
            ['SurfaceEntityId','surfaceentityid','int',1,'','id assigned to the first surface entity (ids go increasingly)'],
            ['OpenProfilesEntityId','openprofilesentityid','int',1,'','id assigned to the first extruded open profile (ids go increasingly)']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter'],
            ['InnerSurfaceMesh','oinner','vtkUnstructuredGrid',1,'','the output inner surface mesh','vmtkmeshwriter']
            ])

    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: No input mesh.')

        boundaryLayerGenerator = vtkvmtk.vtkvmtkBoundaryLayerGenerator2()
        boundaryLayerGenerator.SetInputData(self.Mesh)
        boundaryLayerGenerator.SetWarpVectorsArrayName(self.WarpVectorsArrayName)
        boundaryLayerGenerator.SetLayerThickness(self.Thickness)
        boundaryLayerGenerator.SetLayerThicknessArrayName(self.ThicknessArrayName)
        boundaryLayerGenerator.SetLayerThicknessRatio(self.ThicknessRatio)
        boundaryLayerGenerator.SetMaximumLayerThickness(self.MaximumThickness)
        boundaryLayerGenerator.SetNumberOfSubLayers(self.NumberOfSubLayers)
        boundaryLayerGenerator.SetSubLayerRatio(self.SubLayerRatio)
        boundaryLayerGenerator.SetConstantThickness(self.ConstantThickness)
        boundaryLayerGenerator.SetUseWarpVectorMagnitudeAsThickness(self.UseWarpVectorMagnitudeAsThickness)
        boundaryLayerGenerator.SetIncludeSurfaceCells(self.IncludeSurfaceCells)
        boundaryLayerGenerator.SetNegateWarpVectors(self.NegateWarpVectors)
        boundaryLayerGenerator.SetCellEntityIdsArrayName(self.CellEntityIdsArrayName)
        boundaryLayerGenerator.SetOpenProfilesIdsArrayName(self.OpenProfilesIdsArrayName)
        boundaryLayerGenerator.SetIncludeExtrudedOpenProfilesCells(self.IncludeExtrudedOpenProfilesCells)
        boundaryLayerGenerator.SetIncludeExtrudedSurfaceCells(self.IncludeExtrudedSurfaceCells)
        boundaryLayerGenerator.SetIncludeOriginalSurfaceCells(self.IncludeOriginalSurfaceCells)
        boundaryLayerGenerator.SetLayerEntityId(self.LayerEntityId)
        boundaryLayerGenerator.SetSurfaceEntityId(int(self.SurfaceEntityId))
        boundaryLayerGenerator.SetOpenProfilesEntityId(int(self.OpenProfilesEntityId))
        boundaryLayerGenerator.Update()
        self.Mesh = boundaryLayerGenerator.GetOutput()
        self.InnerSurfaceMesh = boundaryLayerGenerator.GetInnerSurface()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
