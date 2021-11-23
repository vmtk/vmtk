#!/usr/bin/env python

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import sys
import math
import numpy
import vtk
from vmtk import pypes
from vmtk import vmtkscripts


class VmtkSurfaceExtractInnerCylinder(pypes.pypeScript):
    def __init__(self):
        pypes.pypeScript.__init__(self)

        self.SetScriptName('vmtksurfaceextractinnercylinder')
        self.SetScriptDoc('Extract inner surface from an annular-cylindric volume.')

        # Define members
        self.Surface = None

        self.DoubleSurface = None
        self.ColoredSurface = None
        self.InnerSurface = None

        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.EndcapsThresholdLow = 0
        self.EndcapsThresholdHigh = 1

        # Potential output values
        self.InnerRegionId = None

        # Member info: name, cmdlinename, typename, num, default, desc[, defaultpipetoscript]
        self.SetInputMembers([
                ['Surface', 'i', 'vtkPolyData', 1, '',
                 'the input surface', 'vmtksurfacereader'],
                ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, '',
                 'name of the array where entity ids have been stored'],
                ['EndcapsThresholdLow', 'lowthreshold', 'int', 1, '',
                 'lower threshold for encaps filtering', ''],
                ['EndcapsThresholdHigh', 'highthreshold', 'int', 1, '',
                 'higher threshold for encaps filtering', ''],
                ])
        self.SetOutputMembers([
                ['DoubleSurface', 'doublesurface', 'vtkPolyData', 1, '',
                 'the double surface without caps', 'vmtksurfacewriter'],
                ['ColoredSurface', 'coloredsurface', 'vtkPolyData', 1, '',
                 'the colored surface', 'vmtksurfacewriter'],
                ['InnerSurface', 'o', 'vtkPolyData', 1, '',
                 'the innermost surface', 'vmtksurfacewriter'],
                ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, '',
                 'name of the array where entity ids have been stored'],
                ])

    def Execute(self):
        if self.Surface == None:
            self.PrintError('Error: No Surface.')

        self.removeEndCaps()
        self.colorSurfaceRegions()
        self.extractInnerSurface()

    def removeEndCaps(self):
        self.PrintLog("Using thresholding to remove endcaps.")

        th = vtk.vtkThreshold()
        th.SetInputData(self.Surface)
        th.SetInputArrayToProcess(0, 0, 0, 1, self.CellEntityIdsArrayName)
        th.ThresholdBetween(self.EndcapsThresholdLow, self.EndcapsThresholdHigh)
        th.Update()

        gf = vtk.vtkGeometryFilter()
        gf.SetInputConnection(th.GetOutputPort())
        gf.Update()

        self.DoubleSurface = gf.GetOutput()

    def colorSurfaceRegions(self):
        self.PrintLog("Coloring surface regions.")

        connectivityFilter = vtk.vtkPolyDataConnectivityFilter()
        connectivityFilter.SetInputData(self.DoubleSurface)
        connectivityFilter.ColorRegionsOn()
        connectivityFilter.SetExtractionModeToAllRegions()
        connectivityFilter.Update()

        assert connectivityFilter.GetNumberOfExtractedRegions() == 2

        self.ColoredSurface = connectivityFilter.GetOutput()

    def extractInnerSurface(self):
        self.PrintLog("Extracting inner surface.")

        def bnorm(data):
            bounds = data.GetBounds()
            return math.sqrt(sum((bounds[2*i+1]-bounds[2*i])**2 for i in range(3)))

        # Get bounds of entire surface
        bounds_norm = bnorm(self.Surface)

        # Currently assuming that this is the region numbers that were produced by coloring!
        # TODO: If necessary, get from array to be more robust.
        region_ids = (0, 1)

        # Extract each surface in turn to find the smallest one
        for k in region_ids:
            connectivityFilter = vtk.vtkPolyDataConnectivityFilter()
            connectivityFilter.SetInputData(self.ColoredSurface)
            connectivityFilter.SetExtractionModeToSpecifiedRegions()
            connectivityFilter.AddSpecifiedRegion(k)
            connectivityFilter.ColorRegionsOff()
            connectivityFilter.SetScalarConnectivity(0)
            connectivityFilter.Update()
            subsurface = connectivityFilter.GetOutput()

            # The inner surface has smaller bounds
            if bnorm(subsurface) < bounds_norm - 1e-12:
                self.InnerRegionId = k
                self.InnerSurface = subsurface
                break

        assert self.InnerRegionId in region_ids


if __name__ == '__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
