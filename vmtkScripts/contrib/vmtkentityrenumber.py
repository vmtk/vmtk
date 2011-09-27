#!/usr/bin/env python

import sys
from vmtk import pypes

vmtkentityrenumber = 'VmtkEntityRenumber'

class VmtkEntityRenumber(pypes.pypeScript):
    def __init__(self):
        pypes.pypeScript.__init__(self)

        self.SetScriptName(vmtkentityrenumber)
        self.SetScriptDoc('Renumber cell entity id array.')

        self.Mesh = None
        self.CellEntityIdsArrayName = "CellEntityIds"
        self.CellEntityIdOffset = 0
        self.CellEntityIdRenumbering = []

        # Member info: name, cmdlinename, typename, num, default, desc[, defaultpipetoscript]
        self.SetInputMembers([
                ['Mesh', 'i', 'vtkUnstructuredGrid', 1, '',
                 'the input mesh', 'vmtkmeshreader'],
                ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, 'CellEntityIds',
                 'name of the array where entity ids have been stored'],
                ['CellEntityIdOffset', 'offset', 'int', 1, '',
                 'offset added to cell entity ids from mesh', ''],
                ['CellEntityIdRenumbering', 'renumbering', 'int', -1, '',
                 '[from1 to1] [from2 to2] ...', ''],
                ])
        self.SetOutputMembers([
                ['Mesh', 'o', 'vtkUnstructuredGrid', 1, '',
                 'the output mesh', 'vmtkmeshwriter'],
                ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, 'CellEntityIds',
                 'name of the array where entity ids have been stored'],
                ])

    def Execute(self):
        if self.Mesh == None:
            self.PrintError('Error: No Mesh.')

        if len(self.CellEntityIdRenumbering) % 2 != 0:
            self.PrintError('Renumbering must have even length.')

        renumbering = {}
        for i in range(len(self.CellEntityIdRenumbering)/2):
            a = self.CellEntityIdRenumbering[2*i]
            b = self.CellEntityIdRenumbering[2*i+1]
            renumbering[a] = b

        cellids = self.Mesh.GetCellData().GetScalars(self.CellEntityIdsArrayName)
        for i in range(cellids.GetNumberOfTuples()):
            v = cellids.GetValue(i)
            v = renumbering.get(v, v + self.CellEntityIdOffset)
            cellids.SetValue(i, v)

if __name__ == '__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
