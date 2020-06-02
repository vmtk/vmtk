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
        self.MeshConnection = None

        self.IdsToConnect1 = []
        self.IdsToConnect2 = []

        self.ConnectionEdgeLength = 1.0

        self.ConnectionVolumeId = 1
        self.ConnectionWallId = 10
        self.ConnectionWallId2 = 20

        self.VolumeId1 = None
        self.VolumeId2 = None

        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.CellEntityIdsArray = None


        self.SetScriptName('vmtkmeshconnector')
        self.SetScriptDoc('connect two meshes generating a volumetric mesh between two selected regions of the input meshes')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the first input mesh','vmtkmeshreader'],
            ['Mesh2','i2','vtkUnstructuredGrid',1,'','the second input mesh','vmtkmeshreader'],
            ['IdsToConnect1','ids1','int',-1,'','entity ids of the first input mesh to be connected to the second input mesh'],
            ['IdsToConnect2','ids2','int',-1,'','entity ids of the second input mesh to be connected to the first input mesh'],
            ['ConnectionEdgeLength','edgelength','float',1,'(0.0,)','the edgelenth of the connection mesh'],
            ['ConnectionVolumeId','volumeid','int',1,'','the id to be assigned to the generated volume between the input meshes'],
            ['ConnectionWallId','wallid','int',1,'','the id to be assigned to the wall of the generated volume'],
            ['ConnectionWallId2','wallid2','int',1,'','the id to be assigned to the second wall of the generated volume (if it exists)'],
            ['VolumeId1','volumeid1','int',1,'','the id to be assigned to the first input mesh; if None, the input id is mantained'],
            ['VolumeId2','volumeid2','int',1,'','the id to be assigned to the second input mesh; if None, the input id is mantained'],
            ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, '','name of the array where entity ids have been stored'],
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter'],
            ['MeshConnection','oconnection','vtkUnstructuredGrid',1,'','the output connection mesh between the two input meshes','vmtkmeshwriter']
            ])



    def Execute(self):
        from vmtk import vtkvmtk
        from vmtk import vmtkscripts
        from vmtk import vmtkcontribscripts




if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
