#!/usr/bin/env python

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import sys
import math
import vtk
from vmtk import pypes
from vmtk import vmtkscripts


class vmtkThreshold(pypes.pypeScript):
    def __init__(self):
        pypes.pypeScript.__init__(self)

        self.SetScriptName('vmtkthreshold')
        self.SetScriptDoc('Extract part of surface or mesh with cell entitiy ids between given thresholds.')

        self.Surface = None
        self.Mesh = None
        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.LowThreshold = 0
        self.HighThreshold = 1

        # Member info: name, cmdlinename, typename, num, default, desc[, defaultpipetoscript]
        self.SetInputMembers([
                ['Surface', 'isurface', 'vtkPolyData', 1, '',
                 'the input surface', 'vmtksurfacereader'],
                ['Mesh', 'imesh', 'vtkUnstructuredGrid', 1, '',
                 'the input mesh', 'vmtkmeshreader'],
                ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, '',
                 'name of the array where entity ids have been stored'],
                ['LowThreshold', 'lowthreshold', 'int', 1, '',
                 'lower threshold for surface filtering', ''],
                ['HighThreshold', 'highthreshold', 'int', 1, '',
                 'higher threshold for surface filtering', ''],
                ])
        self.SetOutputMembers([
                ['Surface', 'osurface', 'vtkPolyData', 1, '',
                 'the output surface', 'vmtksurfacewriter'],
                ['Mesh', 'omesh', 'vtkUnstructuredGrid', 1, '',
                 'the output mesh', 'vmtkmeshwriter'],
                ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, '',
                 'name of the array where entity ids have been stored'],
                ])

    def Execute(self):
        if self.Surface == None and self.Mesh == None:
            self.PrintError('Error: No Surface or Mesh.')
        if self.Surface != None and self.Mesh != None:
            self.PrintError('Error: Both Surface and Mesh, expecting only one.')

        input = self.Surface or self.Mesh

        th = vtk.vtkThreshold()
        th.SetInputData(input)
        th.SetInputArrayToProcess(0, 0, 0, 1, self.CellEntityIdsArrayName)
        th.ThresholdBetween(self.LowThreshold, self.HighThreshold)
        th.Update()

        if self.Mesh != None:
            self.Mesh = th.GetOutput()
        else:
            assert self.Surface != None
            gf = vtk.vtkGeometryFilter()
            gf.SetInputConnection(th.GetOutputPort())
            gf.Update()
            self.Surface = gf.GetOutput()


if __name__ == '__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
