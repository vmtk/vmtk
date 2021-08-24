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
import vtk

from vmtk import vtkvmtk
from vmtk import pypes


class vmtkMeshCapper(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.IdsToCap = []
        self.CapsIds = [100, 200]
        self.VolumeId = [100, 200]
        self.TargetEdgeLength = 1.0

        self.Method = 'centerpoint'
        self.ConstraintFactor = 1.0
        self.NumberOfRings = 8

        self.VolumeElementScaleFactor = 1.0

        self.SetScriptName('vmtkmeshcapper')
        self.SetScriptDoc('add volumetric caps to the holes of a structure mesh (e.g. the vessel wall or the cardiac muscle) starting from a cylindrical entity id on the internal surface.')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['IdsToCap','idstocap','int',-1,'(0,)','the list of ids on the internal surface which form the cylindrical shape surface to be capped to generate the volumetric cap domain'],
            ['CapsIds','capsids','int',2,'(0,)','the two ids of the two surface generated to cap the cylindrical shape surface'],
            ['VolumeId','volumeid','int',1,'(0,)','the id of the volumetric cap'],
            ['TargetEdgeLength','edgelength','float',1,'(0.0,)','the target edge-length for the remeshing of the two surface caps'],
            ['Method','method','str',1,'["simple","centerpoint","smooth","annular","concaveannular"]','capping method used to cap the selected ids on the internal surfaces'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'','name of the array where the id of the caps have to be stored'],
            ['ConstraintFactor','constraint','float',1,'','amount of influence of the shape of the surface near the boundary on the shape of the cap ("smooth" method only)'],
            ['NumberOfRings','rings','int',1,'(0,)','number of rings composing the cap ("smooth" method only)'],
            ['VolumeElementScaleFactor','volumeelementfactor','float',1,'(0.0,)','volume-element factor for the volumetric mesh generation of the cap']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'','name of the array where the id of the caps are stored']
            ])


    def Execute(self):
        from vmtk import vmtkscripts
        from vmtk import vmtkcontribscripts

        # 1. Extract the cylindrical shaped surface from the input mesh.
        extractCylinder = vmtkcontribscripts.vmtkEntityExtractor()
        extractCylinder.Mesh = self.Mesh
        extractCylinder.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        extractCylinder.EntityIds = self.IdsToCap
        extractCylinder.Execute()

        m2s = vtk.vtkGeometryFilter()
        m2s.SetInputData(extractCylinder.Mesh)
        m2s.Update()
        cylinder = m2s.GetOutput()

        # 2. Cap the cylinder.
        sc = vmtkscripts.vmtkSurfaceCapper()
        sc.Surface = cylinder
        sc.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        sc.CellEntityIdOffset = self.CapsIds[0]-1
        sc.Interactive = 0
        sc.Method = self.Method
        sc.ConstraintFactor = self.ConstraintFactor
        sc.NumberOfRings = self.NumberOfRings
        sc.Execute()
        cylinder = sc.Surface

        renumbering = vmtkcontribscripts.VmtkEntityRenumber()
        renumbering.Surface = cylinder
        renumbering.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        renumbering.CellEntityIdRenumbering = [self.CapsIds[0]+1, self.CapsIds[1]]
        renumbering.Execute()
        cylinder = renumbering.Surface


        # 3. Remesh the surface of the cylinder only on the caps.
        sr = vmtkscripts.vmtkSurfaceRemeshing()
        sr.Surface = cylinder
        sr.ElementSizeMode = 'edgelength'
        sr.TargetEdgeLength = self.TargetEdgeLength
        sr.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        sr.ExcludeEntityIds = self.IdsToCap
        # sr.CollapseAngleThreshold = 0.05
        sr.CleanOutput = 1
        sr.Execute()
        cylinder = sr.Surface

        # 4. Create volumetric cap.
        mg = vmtkscripts.vmtkMeshGenerator()
        mg.Surface = cylinder
        mg.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        mg.SkipCapping = 1
        mg.SkipRemeshing = 1
        mg.VolumeElementScaleFactor = self.VolumeElementScaleFactor
        mg.Execute()
        cylinder = mg.Mesh

        # 5. Append cylinder mesh to the input mesh.
        extractCylinderVolume = vmtkcontribscripts.vmtkEntityExtractor()
        extractCylinder.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        extractCylinderVolume.Mesh = cylinder
        extractCylinderVolume.Invert = 1
        extractCylinderVolume.EntityIds = self.IdsToCap
        extractCylinderVolume.Execute()
        cylinder = extractCylinderVolume.Mesh

        renumbering.Surface = None
        renumbering.Mesh = cylinder
        renumbering.CellEntityIdRenumbering = [0, self.VolumeId]
        renumbering.Execute()
        cylinder = renumbering.Mesh

        append = vtkvmtk.vtkvmtkAppendFilter()
        append.AddInputData(self.Mesh)
        append.AddInputData(cylinder)
        append.SetMergeDuplicatePoints(1)
        append.Update()
        self.Mesh = append.GetOutput()



if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()