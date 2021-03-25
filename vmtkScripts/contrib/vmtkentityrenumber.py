#!/usr/bin/env python

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import sys
from vmtk import pypes
import vtk


class VmtkEntityRenumber(pypes.pypeScript):
    def __init__(self):
        pypes.pypeScript.__init__(self)

        self.SetScriptName('vmtkentityrenumber')
        self.SetScriptDoc('Renumber cell entity id array.')

        self.Mesh = None
        self.CellEntityIdsArrayName = "CellEntityIds"
        self.CellEntityIdOffset = 0
        self.CellEntityIdRenumbering = []
        self.InteriorFacetsOffset = 0

        # Member info: name, cmdlinename, typename, num, default, desc[, defaultpipetoscript]
        self.SetInputMembers([
                ['Mesh', 'i', 'vtkUnstructuredGrid', 1, '',
                 'the input mesh', 'vmtkmeshreader'],
                ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, '',
                 'name of the array where entity ids have been stored'],
                ['CellEntityIdOffset', 'offset', 'int', 1, '',
                 'offset added to cell entity ids that are not mapped explicitly', ''],
                ['CellEntityIdRenumbering', 'renumbering', 'int', -1, '',
                 '[from1 to1] [from2 to2] ...', ''],
                ['InteriorFacetsOffset', 'interiorfacetsoffset', 'int', 1, '',
                 'offset added to ids of interior facets after renumbering mapping, to separate them from interior facets'],
                ])
        self.SetOutputMembers([
                ['Mesh', 'o', 'vtkUnstructuredGrid', 1, '',
                 'the output mesh', 'vmtkmeshwriter'],
                ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, '',
                 'name of the array where entity ids have been stored'],
                ])

    def Execute(self):
        if self.Mesh is None:
            self.PrintError('Error: No Mesh.')

        if len(self.CellEntityIdRenumbering) % 2 != 0:
            self.PrintError('Renumbering must have even length.')

        renumbering = {}
        for i in range(len(self.CellEntityIdRenumbering)//2):
            a = self.CellEntityIdRenumbering[2*i]
            b = self.CellEntityIdRenumbering[2*i+1]
            renumbering[a] = b

        cellids = self.Mesh.GetCellData().GetScalars(self.CellEntityIdsArrayName)
        for i in range(cellids.GetNumberOfTuples()):
            v = cellids.GetValue(i)

            # Renumber or add offset
            v = renumbering.get(v, v + self.CellEntityIdOffset)

            # TODO: This is triangles/tets only
            volumeTypes = (vtk.VTK_TETRA,)
            faceTypes = (vtk.VTK_TRIANGLE,)

            # Add offset if cell is an interior facet
            if self.InteriorFacetsOffset:
                if self.Mesh.GetCell(i).GetCellType() in faceTypes:
                    pIds = vtk.vtkIdList()
                    cIds = vtk.vtkIdList()
                    self.Mesh.GetCellPoints(i, pIds)
                    self.Mesh.GetCellNeighbors(i, pIds, cIds)

                    nIds = cIds.GetNumberOfIds()
                    if nIds == 2:
                        def getCellType(j):
                            return self.Mesh.GetCell(cIds.GetId(j)).GetCellType()
                        if all(getCellType(j) in volumeTypes for j in range(nIds)):
                            v += self.InteriorFacetsOffset

            cellids.SetValue(i, v)


if __name__ == '__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
