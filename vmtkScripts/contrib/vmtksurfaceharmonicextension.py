#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceviewer.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.10 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by 
##       Elena Faggiano (elena.faggiano@gmail.com)
##       Politecnico di Milano

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys


from vmtk import pypes


vmtksurfaceharmonicextension = 'vmtkSurfaceHarmonicExtension'

class vmtkSurfaceHarmonicExtension(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.InputArrayName = 'Displacement'
        self.OutputArrayName = 'Displacement'
        self.InputArray = None
        self.OutputArray = None
        self.RangeIds = None
        self.OutletIds = []
        self.MethodX = "harmonic"
        self.MethodY = "harmonic"
        self.MethodZ = "harmonic"
        self.BoundaryConditions = []
        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.CellEntityIdsArray = None


        self.SetScriptName('vmtksurfaceharmonicextension')
        self.SetScriptDoc('extend an input vector harmonically on a surface')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['InputArrayName','inputarray','str',1,'','input array to be extended on some tags'],
            ['OutputArrayName','inputarray','str',1,'','output array name'],
            ['RangeIds','rangeids','int',2,'','range of ids where to extend the input array'],
            ['OutletIds','outletids','int',-1,'','ids where to impose the heat equation bcs (these ids must be in rangeids)'],
            ['MethodX','methodx','str',1,'["harmonic","closestpoint","ring"]','possible extensions methods'],
            ['MethodY','methody','str',1,'["harmonic","closestpoint","ring"]','possible extensions methods'],
            ['MethodZ','methodz','str',1,'["harmonic","closestpoint","ring"]','possible extensions methods'],
            ['BoundaryConditions','bcs','float',-1,'','list of bcs for the harmonic extension outlets, ordered as boundary id'],
            ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, '','name of the array where entity ids have been stored'],
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])


    def Execute(self): 
        from vmtk import vtkvmtk
        from vmtk import vmtkscripts
        from vmtk import vmtkcontribscripts

        if self.Surface == None:
            self.PrintError('Error: no Surface.')

        self.CellEntityIdsArray = self.Surface.GetCellData().GetArray(self.CellEntityIdsArrayName)

        tags = set()
        for i in range(self.Surface.GetNumberOfCells()):
            tags.add(self.CellEntityIdsArray.GetComponent(i,0))
        tags = sorted(tags)
        self.PrintLog('Tags of the input surface: '+str(tags))


        def surfaceThreshold(surface,low,high):
            th = vmtkcontribscripts.vmtkThreshold()
            th.Surface = surface
            th.CellEntityIdsArrayName = self.CellEntityIdsArrayName
            th.LowThreshold = low
            th.HighThreshold = high
            th.Execute()
            surf = th.Surface
            return surf

        def surfaceAppend(surface1,surface2):
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


        surfaceHarmonicCaps = None
        surfaceHarmonicDomain = None
        surfaceNotProcessed = None

        for item in tags:
            surfaceTh = surfaceThreshold(self.Surface,item,item)
            if item <= self.RangeIds[1] and item >= self.RangeIds[0]:
                if item in self.OutletIds:
                    surfaceHarmonicCaps = surfaceAppend(surfaceHarmonicCaps,surfaceTh)
                else:
                    surfaceHarmonicDomain = surfaceAppend(surfaceHarmonicDomain,surfaceTh)
            else:
                surfaceNotProcessed = surfaceAppend(surfaceNotProcessed,surfaceTh)

        hs = vmtkcontribscripts.vmtkSurfaceHarmonicSections()
        hs.Surface = surfaceHarmonicDomain
        hs.ComputeSections = 0
        hs.BoundaryConditions = self.BoundaryConditions
        hs.Execute()
        surfaceHarmonicDomain = hs.SurfaceHarmonic

        uCaps = vtk.vtkDoubleArray()
        uCaps.SetNumberOfComponents(1)
        uCaps.SetNumberOfTuples(surfaceHarmonicCaps.GetNumberOfPoints())
        uCaps.SetName('HarmonicMappedTemperature')
        uCaps.FillComponent(0,0.0)
        surfaceHarmonicCaps.GetPointData().AddArray(uCaps)

        uNotProcessed = vtk.vtkDoubleArray()
        uNotProcessed.SetNumberOfComponents(1)
        uNotProcessed.SetNumberOfTuples(surfaceNotProcessed.GetNumberOfPoints())
        uNotProcessed.SetName('HarmonicMappedTemperature')
        uNotProcessed.FillComponent(0,1.0)
        surfaceNotProcessed.GetPointData().AddArray(uNotProcessed)

        self.Surface = surfaceAppend(surfaceHarmonicDomain,surfaceHarmonicCaps)
        self.Surface = surfaceAppend(self.Surface,surfaceNotProcessed)

        # self.Surface = surfaceHarmonicDomain

        


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
