#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacecapper.py,v $
## Language:  Python
## Date:      $Date: 2006/07/17 09:53:14 $
## Version:   $Revision: 1.8 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by 
##       Marco Fedele (marco.fedele@polimi.it)
##       Politecnico di Milano

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import sys
import math
import vtk

from vmtk import vtkvmtk
from vmtk import pypes


class vmtkMeshRingGenerator(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.RingId = None
        self.InternalWallId = None
        self.ExternalWallId = None
        self.VolumeId = 1
        self.Thickness = 1.0
        self.NegateWarpVectors = 0

        self.SetScriptName('vmtkmeshringgenerator')
        self.SetScriptDoc('generate a volumetric ring by extruding along the boundary normal a surface ring defined on the input mesh')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['RingId','ringid','int',1,'(0,)','the id of the ring to extrude'],
            ['InternalWallId','internalid','int',1,'(0,)','the id of the internal wall generated by the extrusion'],
            ['ExternalWallId','externalid','int',1,'(0,)','the id of the external wall generated by the extrusion'],
            ['VolumeId','volumeid','int',1,'(0,)','the id of the volumetric ring generated by the extrusion'],
            ['Thickness','thickness','float',1,'(0.0,)','the thickness of the extrusion'],
            ['NegateWarpVectors','negatewarpvectors','bool',1,'','flip the orientation of warp vectors for the extrusion'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'','name of the array where the id of the caps have to be stored']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'','name of the array where the id of the caps are stored']
            ])


    def ExtractIds(self,mesh,ids,invert=False):
        from vmtk import vmtkcontribscripts
        extract = vmtkcontribscripts.vmtkEntityExtractor()
        extract.Mesh = mesh
        extract.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        extract.EntityIds = ids
        extract.Invert = invert
        extract.ConvertToInt = 1
        extract.Execute()
        return extract.Mesh


    def Execute(self):
        from vmtk import vmtkscripts
        from vmtk import vmtkcontribscripts

        if self.RingId == None or self.InternalWallId == None or self.ExternalWallId == None:
            self.PrintError('Error: Missing some input entity ids.')

        # 1. Extract the surface ring from the input mesh.
        ring = self.ExtractIds(self.Mesh, [self.RingId])

        # 2. Compute local or boundary normal.
        m2s = vtk.vtkGeometryFilter()
        m2s.SetInputData(ring)
        m2s.Update()
        ring = m2s.GetOutput()
 
        sn = vmtkscripts.vmtkSurfaceNormals()
        sn.Surface = ring
        sn.Execute()
        ring = sn.Surface

        s2m = vmtkscripts.vmtkSurfaceToMesh()
        s2m.Surface = ring
        s2m.Execute()
        ring = s2m.Mesh

        # 3. Extrude it.
        bl = vmtkscripts.vmtkBoundaryLayer()
        bl.Mesh = ring
        bl.ConstantThickness = 1
        bl.Thickness = self.Thickness
        bl.ThicknessRatio = 1.0
        bl.InnerSurfaceCellEntityId = self.RingId
        bl.OuterSurfaceCellEntityId = self.RingId
        bl.SidewallCellEntityId = self.InternalWallId
        bl.VolumeCellEntityId = self.VolumeId
        bl.NegateWarpVectors = self.NegateWarpVectors
        bl.Execute()

        tetra = vmtkscripts.vmtkMeshTetrahedralize()
        tetra.Mesh = bl.Mesh
        tetra.Execute()
        ring = tetra.Mesh

        ringOK = self.ExtractIds(ring, [self.InternalWallId], True)

        ringKO = self.ExtractIds(ring, [self.InternalWallId])

        m2s.SetInputData(ringKO)
        m2s.Update()
        ringKO = m2s.GetOutput()

        tagger = vmtkcontribscripts.vmtkSurfaceTagger()
        tagger.Surface = ringKO
        tagger.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        tagger.Method = 'connectivity'
        tagger.PrintTags = 0
        tagger.ConnectivityOffset = self.ExternalWallId - self.InternalWallId
        tagger.Execute()
        ringKO = tagger.Surface

        s2m.Surface = ringKO
        s2m.Execute()
        ringKO = s2m.Mesh


        # Check if InternalWallId has been assigned to the internal wall.
        # If not, invert internal and external wall ids.
        def bnorm(data):
            bounds = data.GetBounds()
            return math.sqrt(sum((bounds[2*i+1]-bounds[2*i])**2 for i in range(3)))

        if (bnorm(self.ExtractIds(ringKO, [self.InternalWallId])) > bnorm(self.ExtractIds(ringKO, [self.ExternalWallId]))):
            renumbering = vmtkcontribscripts.VmtkEntityRenumber()
            renumbering.Mesh = ringKO
            renumbering.CellEntityIdsArrayName = self.CellEntityIdsArrayName
            renumbering.CellEntityIdRenumbering = [self.InternalWallId, self.ExternalWallId, self.ExternalWallId, self.InternalWallId]
            renumbering.Execute()
            ringKO = renumbering.Mesh


        # 4. Append to the original mesh.
        append = vtkvmtk.vtkvmtkAppendFilter()
        append.AddInputData(self.Mesh)
        append.AddInputData(ringKO)
        append.AddInputData(ringOK)
        append.SetMergeDuplicatePoints(1)
        append.Update()

        listTags = vmtkcontribscripts.vmtkEntityList()
        listTags.Mesh = append.GetOutput()
        listTags.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        listTags.ConvertToInt = 1
        listTags.Execute()

        self.Mesh = listTags.Mesh




if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()