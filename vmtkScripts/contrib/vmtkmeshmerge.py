#!/usr/bin/env python

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import sys
import math
import numpy
import vtk
from vmtk import pypes
from vmtk import vmtkscripts
from vmtk import vtkvmtk


class vmtkMeshMerge(pypes.pypeScript):
    def __init__(self):
        pypes.pypeScript.__init__(self)

        self.SetScriptName("vmtkmeshmerge")
        self.SetScriptDoc('Merge two or three meshes into one.')

        self.Mesh = None
        self.CellEntityIdsArrayName = "CellEntityIds"

        self.max_meshes = 7
        members = []
        for i in range(1, self.max_meshes+1):
            setattr(self, 'Mesh%d'%i, None)
            members.append(['Mesh%d'%i, 'mesh%d'%i, 'vtkUnstructuredGrid', 1, '',
                 'mesh number %d to merge'%i, 'vmtkmeshreader'])
        for i in range(1, self.max_meshes+1):
            setattr(self, 'CellEntityIdOffset%d'%i, None)
            members.append(['CellEntityIdOffset%d'%i, 'cellentityidoffset%d'%i, 'int', 1, '',
                 'offset added to cell entity ids from mesh%d'%i, ''])

        # Member info: name, cmdlinename, typename, num, default, desc[, defaultpipetoscript]
        self.SetInputMembers([
                ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, '',
                 'name of the array where entity ids have been stored'],
                ] + members)
        self.SetOutputMembers([
                ['Mesh', 'o', 'vtkUnstructuredGrid', 1, '',
                 'the output mesh', 'vmtkmeshwriter'],
                ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, '',
                 'name of the array where entity ids have been stored'],
                ])

    def Execute(self):
        data = [(getattr(self, 'Mesh%d'%i), getattr(self, 'CellEntityIdOffset%d'%i))
                for i in range(1,self.max_meshes+1)]
        n = sum(0 if d[0] is None else 1 for d in data)
        if n < 2:
            self.PrintError('Error: Need at least 2 meshes to merge.')

        def addIds(mesh, offset):
            if mesh is not None and offset != 0:
                cellids = mesh.GetCellData().GetScalars(self.CellEntityIdsArrayName)
                for i in range(cellids.GetNumberOfTuples()):
                    cellids.SetValue(i, cellids.GetValue(i) + offset)

        merger = vtkvmtk.vtkvmtkAppendFilter()
        for mesh, offsets in data:
            addIds(mesh, offsets)
            if mesh != None:
                merger.AddInput(mesh)
        merger.SetMergeDuplicatePoints(1)
        merger.Update()

        self.Mesh = merger.GetOutput()


if __name__ == '__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
