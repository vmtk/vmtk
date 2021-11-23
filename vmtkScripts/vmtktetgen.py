#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtktetgen.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.7 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import sys
import vtk
from vmtk import vtkvmtk

from vmtk import pypes


class vmtkTetGen(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.GenerateCaps = 0

        self.PLC = 1
        self.Refine = 0
        self.Coarsen = 0
        self.NoBoundarySplit = 0
        self.Quality = 1
        self.MinRatio = 1.414
        self.MinDihedral = 10.0
        self.MaxDihedral = 165.0
        self.VarVolume = 0
        self.FixedVolume = 0
        self.MaxVolume = 1E-1
        self.RemoveSliver = 0
        self.RegionAttrib = 0
        self.Epsilon = 1E-8
        self.NoMerge = 0
        self.DetectInter = 0
        self.CheckClosure = 0
        self.Order = 1
        self.DoCheck = 0
        self.Verbose = 0
        self.UseSizingFunction = 0
        self.SizingFunctionArrayName = 'VolumeSizingFunction'

        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.TetrahedronVolumeArrayName = 'TetrahedronVolume'

        self.OutputSurfaceElements = 1
        self.OutputVolumeElements = 1

        self.SetScriptName('vmtktetgen')
        self.SetScriptDoc('wrapper around TetGen tetrahedral mesh generator by Hang Si (http://tetgen.berlios.de/)')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['GenerateCaps','caps','bool',1,'','close surface holes with caps before meshing'],
            ['PLC','plc','int',1,'','see TetGen documentation'],
            ['Refine','refine','int',1,'','see TetGen documentation'],
            ['Coarsen','coarsen','int',1,'','see TetGen documentation'],
            ['NoBoundarySplit','noboundarysplit','int',1,'','see TetGen documentation'],
            ['Quality','quality','int',1,'','see TetGen documentation'],
            ['MinRatio','minratio','float',1,'','see TetGen documentation'],
            ['MinDihedral','mindihedral','float',1,'','see TetGen documentation'],
            ['MaxDihedral','maxdihedral','float',1,'','see TetGen documentation'],
            ['VarVolume','varvolume','int',1,'','see TetGen documentation'],
            ['FixedVolume','fixedvolume','int',1,'','see TetGen documentation'],
            ['MaxVolume','maxvolume','float',1,'','see TetGen documentation'],
            ['RemoveSliver','removesliver','int',1,'','see TetGen documentation'],
            ['RegionAttrib','regionattrib','int',1,'','see TetGen documentation'],
            ['Epsilon','epsilon','float',1,'','see TetGen documentation'],
            ['NoMerge','nomerge','int',1,'','see TetGen documentation'],
            ['DetectInter','detectinter','int',1,'','see TetGen documentation'],
            ['CheckClosure','checkclosure','int',1,'','see TetGen documentation'],
            ['Order','order','int',1,'','see TetGen documentation'],
            ['DoCheck','docheck','int',1,'','see TetGen documentation'],
            ['Verbose','verbose','int',1,'','see TetGen documentation'],
            ['UseSizingFunction','usesizingfunction','int',1,'','see TetGen documentation'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'','name of the array where cell entity ids are stored'],
            ['TetrahedronVolumeArrayName','tetravolumearray','str',1,'','name of the array where volumes of tetrahedra are stored'],
            ['SizingFunctionArrayName','sizingfunctionarray','str',1,'','name of the array where sizing function values are stored'],
            ['OutputSurfaceElements','surfaceelements','int',1,'','toggle output surface elements'],
            ['OutputVolumeElements','volumeelements','int',1,'','toggle output volume elements']
            ])

        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'','name of the array where cell entity ids are stored'],
            ['TetrahedronVolumeArrayName','tetravolumearray','str',1,'','name of the array where volumes of tetrahedra are stored']
            ])

    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: No input mesh.')

        if self.GenerateCaps == 1:
            if not ((self.Mesh.IsHomogeneous() == 1) & (self.Mesh.GetCellType(0) == 5)):
                self.PrintError('Error: In order to generate caps, all input mesh elements must be triangles.')
            meshToSurfaceFilter = vtk.vtkGeometryFilter()
            meshToSurfaceFilter.SetInputData(self.Mesh)
            meshToSurfaceFilter.Update()
            cap = vtkvmtk.vtkvmtkSimpleCapPolyData()
            cap.SetInputConnection(meshToSurfaceFilter.GetOutputPort())
            cap.SetCellMarkerArrayName(self.FacetMarkerArrayName)
            cap.Update()
            surfacetomesh = vtkvmtk.vtkvmtkPolyDataToUnstructuredGridFilter()
            surfacetomesh.SetInputConnection(cap.GetOutputPort())
            surfacetomesh.Update()
            self.Mesh = surfacetomesh.GetOutput()

        tetgen = vtkvmtk.vtkvmtkTetGenWrapper()
        tetgen.SetInputData(self.Mesh)
        tetgen.SetPLC(self.PLC)
        tetgen.SetRefine(self.Refine)
        tetgen.SetCoarsen(self.Coarsen)
        tetgen.SetNoBoundarySplit(self.NoBoundarySplit)
        tetgen.SetQuality(self.Quality)
        tetgen.SetMinRatio(self.MinRatio)
        tetgen.SetMinDihedral(self.MinDihedral)
        tetgen.SetMaxDihedral(self.MaxDihedral)
        tetgen.SetVarVolume(self.VarVolume)
        tetgen.SetFixedVolume(self.FixedVolume)
        tetgen.SetMaxVolume(self.MaxVolume)
        tetgen.SetRemoveSliver(self.RemoveSliver)
        tetgen.SetRegionAttrib(self.RegionAttrib)
        tetgen.SetEpsilon(self.Epsilon)
        tetgen.SetNoMerge(self.NoMerge)
        tetgen.SetDetectInter(self.DetectInter)
        tetgen.SetCheckClosure(self.CheckClosure)
        tetgen.SetOrder(self.Order)
        tetgen.SetDoCheck(self.DoCheck)
        tetgen.SetVerbose(self.Verbose)
        tetgen.SetUseSizingFunction(self.UseSizingFunction)
        tetgen.SetCellEntityIdsArrayName(self.CellEntityIdsArrayName)
        tetgen.SetTetrahedronVolumeArrayName(self.TetrahedronVolumeArrayName)
        tetgen.SetSizingFunctionArrayName(self.SizingFunctionArrayName)
        tetgen.SetOutputSurfaceElements(self.OutputSurfaceElements)
        tetgen.SetOutputVolumeElements(self.OutputVolumeElements)
        tetgen.Update()

        self.Mesh = tetgen.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
