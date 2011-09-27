#!/usr/bin/env python

import sys
import math
import numpy
import vtk
from vmtk import pypes
from vmtk import vmtkscripts
from vmtk import vtkvmtk

vmtkmeshmerge = 'VmtkMeshMerge'

class VmtkMeshMerge(pypes.pypeScript):
    def __init__(self):
        pypes.pypeScript.__init__(self)

        self.SetScriptName(vmtkmeshmerge)
        self.SetScriptDoc('Merge two or three meshes into one.')

        self.Mesh1 = None
        self.Mesh2 = None
        self.Mesh3 = None
        self.Mesh = None
        self.CellEntityIdsArrayName = "CellEntityIds"
        self.CellEntityIdOffset1 = 0
        self.CellEntityIdOffset2 = 0
        self.CellEntityIdOffset3 = 0

        # Member info: name, cmdlinename, typename, num, default, desc[, defaultpipetoscript]
        self.SetInputMembers([
                ['Mesh1', 'mesh1', 'vtkUnstructuredGrid', 1, '',
                 'the first of meshes to merge', 'vmtkmeshreader'],
                ['Mesh2', 'mesh2', 'vtkUnstructuredGrid', 1, '',
                 'the second of meshes to merge', 'vmtkmeshreader'],
                ['Mesh3', 'mesh3', 'vtkUnstructuredGrid', 1, '',
                 '(optional) the third of meshes to merge', 'vmtkmeshreader'],
                ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, 'CellEntityIds',
                 'name of the array where entity ids have been stored'],
                ['CellEntityIdOffset1', 'cellentityidoffset1', 'int', 1, '',
                 'offset added to cell entity ids from mesh1', ''],
                ['CellEntityIdOffset2', 'cellentityidoffset2', 'int', 1, '',
                 'offset added to cell entity ids from mesh2', ''],
                ['CellEntityIdOffset3', 'cellentityidoffset3', 'int', 1, '',
                 'offset added to cell entity ids from mesh3', ''],
                ])
        self.SetOutputMembers([
                ['Mesh', 'o', 'vtkUnstructuredGrid', 1, '',
                 'the output mesh', 'vmtkmeshwriter'],
                ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, 'CellEntityIds',
                 'name of the array where entity ids have been stored'],
                ])

    def Execute(self):
        if self.Mesh1 == None:
            self.PrintError('Error: No Mesh1.')
        if self.Mesh2 == None:
            self.PrintError('Error: No Mesh2.')

        def addIds(mesh, offset):
            if offset != 0:
                cellids = mesh.GetCellData().GetScalars(self.CellEntityIdsArrayName)
                for i in range(cellids.GetNumberOfTuples()):
                    cellids.SetValue(i, cellids.GetValue(i) + offset)

        addIds(self.Mesh1, self.CellEntityIdOffset1)
        addIds(self.Mesh2, self.CellEntityIdOffset2)
        if self.Mesh3 != None:
            addIds(self.Mesh3, self.CellEntityIdOffset3)

        merger = vtkvmtk.vtkvmtkAppendFilter()
        merger.AddInput(self.Mesh1)
        merger.AddInput(self.Mesh2)
        if self.Mesh3 != None:
            merger.AddInput(self.Mesh3)
        merger.SetMergeDuplicatePoints(1)
        merger.Update()

        self.Mesh = merger.GetOutput()

if __name__ == '__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
