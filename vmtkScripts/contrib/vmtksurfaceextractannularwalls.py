#!/usr/bin/env python

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import sys
import math
import numpy
import vtk
from vmtk import pypes
from vmtk import vmtkscripts


class VmtkSurfaceExtractAnnularWalls(pypes.pypeScript):
    def __init__(self):
        pypes.pypeScript.__init__(self)

        self.SetScriptName('vmtksurfaceextractannularwalls')
        self.SetScriptDoc('Extract wall surfaces from an annular-cylindric surface.')

        # Define members
        self.Surface = None

        self.DoubleSurface = None
        self.ColoredSurface = None
        self.InnerSurface = None
        self.OuterSurface = None

        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.EndcapsThresholdLow = 0
        self.EndcapsThresholdHigh = 1

        # Potential output values
        self.InnerRegionId = None
        self.OuterRegionId = None

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
                ['ColoredSurface', 'o', 'vtkPolyData', 1, '',
                 'the colored surface', 'vmtksurfacewriter'],
                ['InnerSurface', 'innersurface', 'vtkPolyData', 1, '',
                 'the innermost surface', 'vmtksurfacewriter'],
                ['OuterSurface', 'outersurface', 'vtkPolyData', 1, '',
                 'the outermost surface', 'vmtksurfacewriter'],
                ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, '',
                 'name of the array where entity ids have been stored'],
                ])

    def Execute(self):
        if self.Surface == None:
            self.PrintError('Error: No Surface.')

        self.removeEndCaps()
        self.colorSurfaceRegions()
        self.extractSurfaces()

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

    def extractSurfaces(self):
        self.PrintLog("Extracting surfaces.")

        def bnorm(data):
            bounds = data.GetBounds()
            return math.sqrt(sum((bounds[2*i+1]-bounds[2*i])**2 for i in range(3)))

        # Get bounds of entire surface
        bounds_norm = bnorm(self.Surface)

        # Currently assuming that this is the region numbers that were produced by coloring!
        # TODO: If necessary, get from array to be more robust.
        region_ids = (0, 1)

        # Extract each surface in turn to find the smallest one
        subsurfaces = {}
        for k in region_ids:
            connectivityFilter = vtk.vtkPolyDataConnectivityFilter()
            connectivityFilter.SetInputData(self.ColoredSurface)
            connectivityFilter.SetExtractionModeToSpecifiedRegions()
            connectivityFilter.AddSpecifiedRegion(k)
            connectivityFilter.ColorRegionsOff()
            connectivityFilter.SetScalarConnectivity(0)
            connectivityFilter.Update()
            subsurfaces[k] = connectivityFilter.GetOutput()

        # The inner surface has smaller bounds
        if bnorm(subsurfaces[region_ids[0]]) < bnorm(subsurfaces[region_ids[1]]):
            self.InnerRegionId = region_ids[0]
            self.OuterRegionId = region_ids[1]
        else:
            self.InnerRegionId = region_ids[1]
            self.OuterRegionId = region_ids[0]
        self.InnerSurface = subsurfaces[self.InnerRegionId]
        self.OuterSurface = subsurfaces[self.OuterRegionId]


if __name__ == '__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
