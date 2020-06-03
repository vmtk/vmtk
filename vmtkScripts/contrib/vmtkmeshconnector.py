#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshconnector.py,v $
## Language:  Python
## Date:      $Date: 2020/06/02 12:27:13 $
## Version:   $Revision: 1.10 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by 
##       Marco Fedele (marco.fedele@polimi.it)
##       Politecnico di Milano

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys


from vmtk import pypes


vmtkmeshconnector = 'vmtkMeshConnector'

class vmtkMeshConnector(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.Mesh2 = None
        self.ConnectionMesh = None

        self.IdsToConnect1 = []
        self.IdsToConnect2 = []

        self.ConnectionEdgeLength = 1.0

        self.ConnectionVolumeId = 1
        self.ConnectionWallIds = []

        self.VolumeId1 = None
        self.VolumeId2 = None

        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.CellEntityIdsArray = None


        self.SetScriptName('vmtkmeshconnector')
        self.SetScriptDoc('connect two meshes generating a volumetric mesh between two selected regions of the input meshes')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the first input mesh','vmtkmeshreader'],
            ['Mesh2','i2','vtkUnstructuredGrid',1,'','the second input mesh; if None, the regions to be connected are supposed to be both on the first input mesh','vmtkmeshreader'],
            ['IdsToConnect1','ids1','int',-1,'','entity ids identifying the first region to be connected on the first input mesh'],
            ['IdsToConnect2','ids2','int',-1,'','entity ids identifying the second region to be connected on the second input mesh (or on the first one when the second one is None)'],
            ['ConnectionEdgeLength','edgelength','float',1,'(0.0,)','the edgelength of the connection mesh'],
            ['ConnectionVolumeId','volumeid','int',1,'','the id to be assigned to the generated volume between the input meshes'],
            ['ConnectionWallIds','wallids','int',-1,'','list of ids to be assigned to the walls of the generated volume'],
            ['VolumeId1','volumeid1','int',1,'','the id to be assigned to the first input mesh; if None, the input id is mantained'],
            ['VolumeId2','volumeid2','int',1,'','the id to be assigned to the second input mesh; if None, the input id is mantained'],
            ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, '','name of the array where entity ids have been stored'],
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter'],
            ['ConnectionMesh','oconnection','vtkUnstructuredGrid',1,'','the output connection mesh between the two input meshes','vmtkmeshwriter']
            ])


    def MeshBoundaryThreshold(self,mesh,low,high):
        from vmtk import vmtkscripts
        from vmtk import vmtkcontribscripts

        th = vmtkcontribscripts.vmtkThreshold()
        th.Mesh = mesh
        th.ArrayName = self.CellEntityIdsArrayName
        th.CellData = 1
        th.LowThreshold = low
        th.HighThreshold = high
        th.Execute()

        ms = vmtkscripts.vmtkMeshToSurface()
        ms.Mesh = th.Mesh
        ms.CleanOutput = 1
        ms.Execute()

        return ms.Surface


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

        if self.Mesh == None:
            self.PrintError('Error: no first input mesh.')

        if self.Mesh2 == None:
            self.Mesh2 = self.Mesh
            doubleMesh = False
        else:
            doubleMesh = True

        if self.IdsToConnect1==[] or self.IdsToConnect2==[]:
            self.PrintError('Error: empty list of ids to be connected')


        # 1. extract surface to be connected and join them
        self.IdsToConnect1 = set(self.IdsToConnect1)
        self.IdsToConnect2 = set(self.IdsToConnect2)

        surface1 = vtk.vtkPolyData()
        for item in self.IdsToConnect1:
            tag = self.MeshBoundaryThreshold(self.Mesh,item,item)
            surface1 = self.SurfaceAppend(surface1,tag)

        surface2 = vtk.vtkPolyData()
        for item in self.IdsToConnect2:
            tag = self.MeshBoundaryThreshold(self.Mesh2,item,item)
            surface2 = self.SurfaceAppend(surface2,tag)

        connectionSurface = self.SurfaceAppend(surface1, surface2)


        # 2. connect the two surfaces
        if not set(self.ConnectionWallIds).isdisjoint(self.IdsToConnect1 | self.IdsToConnect2):
            self.PrintError('ConnectionWallIds cannot be the same ids to be connected')

        while not self.CheckClosedSurface(connectionSurface):
            if not self.ConnectionWallIds:
                self.PrintError('ConnectionWallIds are less than the need walls')
            surfConn = vmtkcontribscripts.vmtkSurfaceConnector()
            surfConn.Surface = connectionSurface
            surfConn.CellEntityIdsArrayName = self.CellEntityIdsArrayName
            surfConn.IdValue = self.ConnectionWallIds.pop(0)
            surfConn.Execute()
            connectionSurface = surfConn.OutputSurface


        # 3. generate the connection volumetric mesh
        sr = vmtkscripts.vmtkSurfaceRemeshing()
        sr.Surface = connectionSurface
        sr.ElementSizeMode ='edgelength'
        sr.TargetEdgeLength = self.ConnectionEdgeLength
        sr.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        sr.ExcludeEntityIds = list(self.IdsToConnect1 | self.IdsToConnect2)
        sr.CleanOutput = 1
        sr.Execute()
        connectionSurface = sr.Surface

        mg = vmtkscripts.vmtkMeshGenerator()
        mg.Surface = connectionSurface
        mg.VolumeElementScaleFactor = 1.0
        mg.SkipRemeshing = 1
        mg.Execute()
        self.ConnectionMesh = mg.Mesh


        # 4. assign to each volume elements the correct VolumeId
        self.ChangeVolumeId(self.ConnectionMesh,self.ConnectionVolumeId)

        if self.VolumeId1!=None:
            self.ChangeVolumeId(self.Mesh,self.VolumeId1)

        if self.VolumeId2!=None and doubleMesh:
            self.ChangeVolumeId(self.Mesh2,self.VolumeId2)


        # 5. append the geometries into a unique mesh
        append = vtk.vtkAppendFilter()
        append.MergePointsOn()
        append.AddInputData(self.ConnectionMesh)
        append.AddInputData(self.Mesh)
        if doubleMesh:
            append.AddInputData(self.Mesh2)
        append.Update()
        self.Mesh = append.GetOutput()




if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
