#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshconnectorfsi.py,v $
## Language:  Python
## Date:      $Date: 2021/04/26 18:00:00 $
## Version:   $Revision: 1.0 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Michele Bucelli (michele.bucelli@polimi.it)
##       Politecnico di Milano

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys


from vmtk import pypes


vmtkmeshconnector = 'vmtkMeshConnectorFSI'

class vmtkMeshConnectorFSI(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.FluidMesh = None
        self.FluidMesh2 = None
        self.FluidConnectionMesh = None
        self.FluidConnectionSurface = None
        self.FluidIdsToConnect1 = []
        self.FluidIdsToConnect2 = []

        self.StructureMesh = None
        self.StructureMesh2 = None
        self.StructureConnectionMesh = None
        self.StructureConnectionSurface = None
        self.StructureIdsToConnect1 = []
        self.StructureIdsToConnect2 = []

        self.ConnectionEdgeLength = 1.0

        self.ConnectionVolumeId = 1
        self.ConnectionWallIds = []

        self.VolumeId1 = None
        self.VolumeId2 = None

        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.CellEntityIdsArray = None

        self.ExportConnectionRegions = 1
        self.SkipRemeshing = 0
        self.Display = 0

        self.SetScriptName('vmtkmeshconnectorfsi')
        self.SetScriptDoc('given two meshes for a fluid domain, and two meshes for a structure domain enclosing the fluid ones with a conforming interface, connects them with a conforming ring, and attributes different volume tags to each portion')
        self.SetInputMembers([
            ['FluidMesh','fi','vtkUnstructuredGrid',1,'','the first fluid input mesh','vmtkmeshreader'],
            ['FluidMesh2','fi2','vtkUnstructuredGrid',1,'','the second fluid input mesh; if None, the regions to be connected are supposed to be both on the first fluid input mesh','vmtkmeshreader'],
            ['StructureMesh','si','vtkUnstructuredGrid',1,'','the first structure input mesh','vmtkmeshreader'],
            ['StructureMesh2','si2','vtkUnstructuredGrid',1,'','the second structure input mesh; if None, the regions to be connected are supposed to be both on the first structure input mesh','vmtkmeshreader'],
            ['FluidIdsToConnect1','fids1','int',-1,'','entity ids identifying the first region to be connected on the first fluid input mesh'],
            ['FluidIdsToConnect2','fids2','int',-1,'','entity ids identifying the second region to be connected on the second fluid input mesh (or on the first one when the second one is None)'],
            ['StructureIdsToConnect1','sids1','int',-1,'','entity ids identifying the first region to be connected on the first structure input mesh'],
            ['StructureIdsToConnect2','sids2','int',-1,'','entity ids identifying the second region to be connected on the second structure input mesh (or on the first one when the second one is None)'],
            ['ConnectionEdgeLength','edgelength','float',1,'(0.0,)','the edgelength of the connection mesh'],
            ['ConnectionVolumeId','volumeid','int',1,'','the id to be assigned to the generated volume between the input meshes'],
            ['ConnectionWallIds','wallids','int',-1,'','list of ids to be assigned to the walls of the generated volume'],
            ['VolumeId1','volumeid1','int',1,'','the id to be assigned to the first input mesh; if None, the input id is mantained'],
            ['VolumeId2','volumeid2','int',1,'','the id to be assigned to the second input mesh; if None, the input id is mantained'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'','name of the array where entity ids have been stored'],
            ['ExportConnectionRegions','exportregions','bool',1,'','toggle exporting connection regions of the input meshes on the output ConnectionMesh'],
            ['SkipRemeshing','skipremeshing','bool',1,'','toggle skipping the remeshing of the connection region'],
            ['Display','display','bool',1,'','toggle displaying connection algorithm progress']
            ])
        self.SetOutputMembers([
            ['FluidMesh','fo','vtkUnstructuredGrid',1,'','the output fluid mesh','vmtkmeshwriter'],
            ['FluidConnectionMesh','foconnection','vtkUnstructuredGrid',1,'','the output connection mesh between the two fluid input meshes','vmtkmeshwriter'],
            ['FluidConnectionSurface','foconnectionsurf','vtkPolyData',1,'','the output connection surface between the two fluid input meshes','vmtksurfacewriter'],
            ['StructureMesh','so','vtkUnstructuredGrid',1,'','the output structure mesh','vmtkmeshwriter'],
            ['StructureConnectionMesh','soconnection','vtkUnstructuredGrid',1,'','the output connection mesh between the two structure input meshes','vmtkmeshwriter'],
            ['StructureConnectionSurface','soconnectionsurf','vtkPolyData',1,'','the output connection surface between the two structure input meshes','vmtksurfacewriter']
            ])


    def MeshBoundaryThreshold(self,mesh,value):
        from vmtk import vmtkscripts
        from vmtk import vmtkcontribscripts

        th = vmtkcontribscripts.vmtkThreshold()
        th.Mesh = mesh
        th.ArrayName = self.CellEntityIdsArrayName
        th.CellData = 1
        th.LowThreshold = value
        th.HighThreshold = value
        th.Execute()

        ms = vmtkscripts.vmtkMeshToSurface()
        ms.Mesh = th.Mesh
        ms.CleanOutput = 1
        ms.Execute()

        return ms.Surface


    def MeshThreshold(self,mesh,value):
        from vmtk import vmtkcontribscripts

        th = vmtkcontribscripts.vmtkThreshold()
        th.Mesh = mesh
        th.ArrayName = self.CellEntityIdsArrayName
        th.CellData = 1
        th.LowThreshold = value
        th.HighThreshold = value
        th.Execute()

        return th.Mesh


    def SurfaceAppend(self,surface1,surface2):
        from vmtk import vmtkscripts
        if surface1 == None:
            surf = surface2
        elif surface2 == None:
            surf = surface1
        else:
            a = vmtkscripts.vmtkSurfaceAppend()
            a.Surface = surface1
            a.Surface2 = surface2
            a.Execute()
            surf = a.Surface
            tr = vmtkscripts.vmtkSurfaceTriangle()
            tr.Surface = surf
            tr.Execute()
            surf = tr.Surface
        return surf


    def CheckClosedSurface(self,surface):
        fe = vtk.vtkFeatureEdges()
        fe.FeatureEdgesOff()
        fe.BoundaryEdgesOn()
        fe.NonManifoldEdgesOn()
        fe.SetInputData(surface)
        fe.Update()
        return True if fe.GetOutput().GetNumberOfCells()==0 else False


    def ChangeVolumeId(self,mesh,newId):
        idsArray = mesh.GetCellData().GetArray(self.CellEntityIdsArrayName)
        for i in range(idsArray.GetNumberOfTuples()):
            if mesh.GetCellType(i) == 10: # 10 -> tetraedra
                idsArray.SetValue(i,newId)


    def Execute(self):
        from vmtk import vtkvmtk
        from vmtk import vmtkscripts
        from vmtk import vmtkcontribscripts

        if self.FluidMesh == None:
            self.PrintError('Error: no first fluid input mesh.')

        if self.FluidMesh2 == None:
            self.FluidMesh2 = self.FluidMesh
            doubleFluidMesh = False
        else:
            doubleFluidMesh = True

        if self.StructureMesh == None:
            self.PrintError('Error: no first structure input mesh.')

        if self.StructureMesh2 == None:
            self.StructureMesh2 = self.StructureMesh
            doubleStructureMesh = False
        else:
            doubleStructureMesh = True

        if not doubleStructureMesh == doubleFluidMesh:
            self.PrintError('Error: Either two meshes for both fluid and structure, or a single mesh for both.');

        if self.FluidIdsToConnect1==[] or self.FluidIdsToConnect2==[] or self.StructureIdsToConnect1==[] or self.StructureIdsToConnect2==[]:
            self.PrintError('Error: empty list of ids to be connected')


        # 1. extract surface to be connected and join them
        self.FluidIdsToConnect1 = set(self.FluidIdsToConnect1)
        self.FluidIdsToConnect2 = set(self.FluidIdsToConnect2)
        self.StructureIdsToConnect1 = set(self.StructureIdsToConnect1)
        self.StructureIdsToConnect2 = set(self.StructureIdsToConnect2)

        # Extract the fluid connection regions.
        fluidSurface1 = vtk.vtkPolyData()
        for item in self.FluidIdsToConnect1:
            tag = self.MeshBoundaryThreshold(self.FluidMesh, item)
            fluidSurface1 = self.SurfaceAppend(fluidSurface1, tag)

        fluidSurface2 = vtk.vtkPolyData()
        for item in self.FluidIdsToConnect2:
            tag = self.MeshBoundaryThreshold(self.FluidMesh2, item)
            fluidSurface2 = self.SurfaceAppend(fluidSurface2, tag)

        self.FluidConnectionSurface = self.SurfaceAppend(fluidSurface1, fluidSurface2)

        # Connect the fluid surfaces.
        sc = vmtkcontribscripts.vmtkSurfaceConnector()
        sc.Surface = self.FluidConnectionSurface
        sc.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        sc.IdValue = self.ConnectionWallIds[0]
        sc.Display = self.Display
        sc.Execute()
        self.FluidConnectionSurface = sc.OutputSurface

        # Remesh the fluid connection.
        sr = vmtkscripts.vmtkSurfaceRemeshing()
        sr.Surface = self.FluidConnectionSurface
        sr.ElementSizeMode ='edgelength'
        sr.TargetEdgeLength = self.ConnectionEdgeLength
        sr.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        sr.ExcludeEntityIds = list(self.FluidIdsToConnect1 | self.FluidIdsToConnect2)
        sr.CleanOutput = 1

        if not self.SkipRemeshing:
            sr.Execute()

        self.FluidConnectionSurface = sr.Surface

        # Extract the endocardial wall.
        ee = vmtkcontribscripts.vmtkEntityExtractor()
        ee.Surface = self.FluidConnectionSurface
        ee.EntityIds = [self.ConnectionWallIds[0]]
        ee.Execute()
        endoWall = ee.Surface

        # Extract the structure connection regions.
        structureSurface1 = vtk.vtkPolyData()
        for item in self.StructureIdsToConnect1:
            tag = self.MeshBoundaryThreshold(self.StructureMesh, item)
            structureSurface1 = self.SurfaceAppend(structureSurface1, tag)

        structureSurface2 = vtk.vtkPolyData()
        for item in self.StructureIdsToConnect2:
            tag = self.MeshBoundaryThreshold(self.StructureMesh2, item)
            structureSurface2 = self.SurfaceAppend(structureSurface2, tag)

        self.StructureConnectionSurface = self.SurfaceAppend(structureSurface1, structureSurface2)

        # Append the endocardial wall.
        self.StructureConnectionSurface = self.SurfaceAppend(self.StructureConnectionSurface, endoWall)

        # Connect the structure surfaces.
        sc = vmtkcontribscripts.vmtkSurfaceConnector()
        sc.Surface = self.StructureConnectionSurface
        sc.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        sc.IdValue = self.ConnectionWallIds[1]
        sc.Display = self.Display
        sc.Execute()
        self.StructureConnectionSurface = sc.OutputSurface

        # Remesh the epicardial wall.
        sr = vmtkscripts.vmtkSurfaceRemeshing()
        sr.Surface = self.StructureConnectionSurface
        sr.ElementSizeMode ='edgelength'
        sr.TargetEdgeLength = self.ConnectionEdgeLength
        sr.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        sr.ExcludeEntityIds = list(self.StructureIdsToConnect1 | self.StructureIdsToConnect2)
        sr.ExcludeEntityIds.append(self.ConnectionWallIds[0])
        sr.CleanOutput = 1

        if not self.SkipRemeshing:
            sr.Execute()

        self.StructureConnectionSurface = sr.Surface

        # Generate the connection meshes.
        mg = vmtkscripts.vmtkMeshGenerator()
        mg.Surface = self.FluidConnectionSurface
        mg.VolumeElementScaleFactor = 1.0
        mg.SkipRemeshing = 1
        mg.Execute()
        self.FluidConnectionMesh = mg.Mesh

        mg.Surface = self.StructureConnectionSurface
        mg.Execute()
        self.StructureConnectionMesh = mg.Mesh

        # Assign volume IDs.
        self.ChangeVolumeId(self.FluidConnectionMesh, self.ConnectionVolumeId)
        self.ChangeVolumeId(self.StructureConnectionMesh, self.ConnectionVolumeId)

        if self.VolumeId1 != None:
            self.ChangeVolumeId(self.FluidMesh, self.VolumeId1);
            self.ChangeVolumeId(self.StructureMesh, self.VolumeId1);

        if self.VolumeId2 != None:
            self.ChangeVolumeId(self.FluidMesh2, self.VolumeId2);
            self.ChangeVolumeId(self.StructureMesh2, self.VolumeId2);

        # Append into two separate meshes for fluid and structure.
        fluidConnectionVolume = self.MeshThreshold(self.FluidConnectionMesh, self.ConnectionVolumeId)
        fluidConnectionWall = self.MeshThreshold(self.FluidConnectionMesh, self.ConnectionWallIds[0])

        append = vtk.vtkAppendFilter()
        append.MergePointsOn()
        append.AddInputData(fluidConnectionVolume)
        append.AddInputData(fluidConnectionWall)

        if not self.ExportConnectionRegions:
            append.Update()
            self.FluidConnectionMesh.DeepCopy(append.GetOutput())

        append.AddInputData(self.FluidMesh)
        if doubleFluidMesh:
            append.AddInputData(self.FluidMesh2)

        append.Update()
        self.FluidMesh = append.GetOutput()

        structureConnectionVolume = self.MeshThreshold(self.StructureConnectionMesh, self.ConnectionVolumeId)
        structureConnectionWall1 = self.MeshThreshold(self.StructureConnectionMesh, self.ConnectionWallIds[0])
        structureConnectionWall2 = self.MeshThreshold(self.StructureConnectionMesh, self.ConnectionWallIds[1])

        append = vtk.vtkAppendFilter()
        append.MergePointsOn()
        append.AddInputData(structureConnectionVolume)
        append.AddInputData(structureConnectionWall1)
        append.AddInputData(structureConnectionWall2)

        if not self.ExportConnectionRegions:
            append.Update()
            self.StructureConnectionMesh.DeepCopy(append.GetOutput())

        append.AddInputData(self.StructureMesh)
        if doubleStructureMesh:
            append.AddInputData(self.StructureMesh2)

        append.Update()
        self.StructureMesh = append.GetOutput()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
