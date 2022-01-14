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
        self.ArrayName = 'CellEntityIds'
        self.CellData = 1
        self.LowThreshold = 0
        self.HighThreshold = 1

        # Member info: name, cmdlinename, typename, num, default, desc[, defaultpipetoscript]
        self.SetInputMembers([
                ['Surface', 'isurface', 'vtkPolyData', 1, '',
                 'the input surface', 'vmtksurfacereader'],
                ['Mesh', 'imesh', 'vtkUnstructuredGrid', 1, '',
                 'the input mesh', 'vmtkmeshreader'],
                ['ArrayName', 'array', 'str', 1, '',
                 'array to process'],
                ['CellData','celldata','bool',1,'','toggle processing a cell-data or a point-data array'],
                ['LowThreshold', 'lowthreshold', 'float', 1, '',
                 'lower threshold for surface filtering', ''],
                ['HighThreshold', 'highthreshold', 'float', 1, '',
                 'higher threshold for surface filtering', ''],
                ])
        self.SetOutputMembers([
                ['Surface', 'osurface', 'vtkPolyData', 1, '',
                 'the output surface', 'vmtksurfacewriter'],
                ['Mesh', 'omesh', 'vtkUnstructuredGrid', 1, '',
                 'the output mesh', 'vmtkmeshwriter'],
                ['ArrayName', 'array', 'str', 1, '',
                 'processed array name'],
                ])

    def Execute(self):
        if self.Surface == None and self.Mesh == None:
            self.PrintError('Error: No Surface or Mesh.')
        if self.Surface != None and self.Mesh != None:
            self.PrintError('Error: Both Surface and Mesh, expecting only one.')

        input = self.Surface or self.Mesh

        th = vtk.vtkThreshold()
        th.SetInputData(input)
        if self.CellData:
            th.SetInputArrayToProcess(0, 0, 0, 1, self.ArrayName)
        else:
            th.SetInputArrayToProcess(0, 0, 0, 0, self.ArrayName)
        th.SetLowerThreshold(self.LowThreshold)
        th.SetUpperThreshold(self.HighThreshold)
        th.SetThresholdFunction(vtk.vtkThreshold.THRESHOLD_BETWEEN)
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
