#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtktetgen.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.7 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.


import sys
import vtk
import vtkvmtk

import pypes

vmtktetgen = 'vmtkTetGen'

class vmtkTetGen(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Mesh = None
        self.GenerateCaps = 0

        self.PLC = 0
        self.Refine = 0
        self.NoBoundarySplit = 0
        self.Quality = 0
        self.MinRatio = 1.414
        self.VarVolume = 0
        self.FixedVolume = 0
        self.MaxVolume = 1E-1
        self.RemoveSliver = 0
        self.MaxDihedral = 175.0
        self.InsertAddPoints = 0
        self.RegionAttrib = 0
        self.Epsilon = 1E-8
        self.NoMerge = 0
        self.DetectInter = 0
        self.CheckClosure = 0
        self.Order = 1
        self.DoCheck = 0
      	self.Verbose = 0
        
        self.PointMarkerArrayName = 'PointMarkerArray'
        self.FacetMarkerArrayName = 'FacetMarkerArray'
        self.TetrahedronVolumeArrayName = 'TetrahedronVolumeArray'
        
        self.AdditionalPoints = None
        
        self.OutputSurfaceElements = 1
        self.OutputVolumeElements = 1
        self.GenerateEntityArrays = 0

        self.EntityArrayNamePrefix = 'Entity'

        self.SetScriptName('vmtktetgen')
        self.SetScriptDoc('wrapper around TetGen tetrahedral mesh generator by Hang Si (http://tetgen.berlios.de/)')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'the input mesh','vmtkmeshreader'],
            ['GenerateCaps','caps','int',1,'close surface holes with caps before meshing'],
            ['PLC','plc','int',1,'see TetGen documentation'],
            ['Refine','refine','int',1,'see TetGen documentation'],
            ['NoBoundarySplit','noboundarysplit','int',1,'see TetGen documentation'],
            ['Quality','quality','int',1,'see TetGen documentation'],
            ['MinRatio','minratio','float',1,'see TetGen documentation'],
            ['VarVolume','varvolume','int',1,'see TetGen documentation'],
            ['FixedVolume','fixedvolume','int',1,'see TetGen documentation'],
            ['MaxVolume','maxvolume','float',1,'see TetGen documentation'],
            ['RemoveSliver','removesliver','int',1,'see TetGen documentation'],
            ['MaxDihedral','maxdihedral','float',1,'see TetGen documentation'],
            ['InsertAddPoints','insertaddpoints','int',1,'see TetGen documentation'],
            ['RegionAttrib','regionattrib','int',1,'see TetGen documentation'],
            ['Epsilon','epsilon','float',1,'see TetGen documentation'],
            ['NoMerge','nomerge','int',1,'see TetGen documentation'],
            ['DetectInter','detectinter','int',1,'see TetGen documentation'],
            ['CheckClosure','checkclosure','int',1,'see TetGen documentation'],
            ['Order','order','int',1,'see TetGen documentation'],
            ['DoCheck','docheck','int',1,'see TetGen documentation'],
            ['Verbose','verbose','int',1,'see TetGen documentation'],
            ['PointMarkerArrayName','pointmarkerarray','str',1,'name of the array where point markers are stored'],
            ['FacetMarkerArrayName','facetmarkerarray','str',1,'name of the array where surface cell markers are stored'],
            ['TetrahedronVolumeArrayName','tetravolumearray','str',1,'name of the array where volumes of tetrahedra are stored'],
            ['AdditionalPoints','addpoints','vtkUnstructuredGrid',1,'dataset containing additional points output by TetGen'],
            ['OutputSurfaceElements','surfaceelements','int',1,'toggle output surface elements'], 
            ['OutputVolumeElements','volumeelements','int',1,'toggle output volume elements'],
            ['GenerateEntityArrays','entityarrays','int',1,'toggle generate arrays with entity information (one point data array for each entity containing a boolean)'],
            ['EntityArrayNamePrefix','entityarrayprefix','str',1,'prefix of entity array names']
            ])
        self.SetOutputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'the output mesh','vmtkmeshwriter'],
            ['PointMarkerArrayName','pointmarkerarray','str',1,'name of the array where point markers are stored'],
            ['FacetMarkerArrayName','facetmarkerarray','str',1,'name of the array where surface cell markers are stored'],
            ['TetrahedronVolumeArrayName','tetravolumearray','str',1,'name of the array where volumes of tetrahedra are stored'],
            ['EntityArrayNamePrefix','entityarrayprefix','str',1,'prefix of entity array names']
            ])

    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: No input mesh.')

        if self.GenerateCaps == 1:
            if not ((self.Mesh.IsHomogeneous() == 1) & (self.Mesh.GetCellType(0) == 5)):
                self.PrintError('Error: In order to generate caps, all input mesh elements must be triangles.')
            meshToSurfaceFilter = vtk.vtkGeometryFilter()
            meshToSurfaceFilter.SetInput(self.Mesh)
            meshToSurfaceFilter.Update()
            cap = vtkvmtk.vtkvmtkSimpleCapPolyData()
            cap.SetInput(meshToSurfaceFilter.GetOutput())
            cap.SetCellMarkerArrayName(self.FacetMarkerArrayName)
            cap.Update()
            surfacetomesh = vtkvmtk.vtkvmtkPolyDataToUnstructuredGridFilter()
            surfacetomesh.SetInput(cap.GetOutput())
            surfacetomesh.Update()
            self.Mesh = surfacetomesh.GetOutput()

        tetgen = vtkvmtk.vtkvmtkTetGenWrapper()
        tetgen.SetInput(self.Mesh)
        tetgen.SetFacetMarkerArrayName(self.FacetMarkerArrayName)
        tetgen.SetPLC(self.PLC)
        tetgen.SetRefine(self.Refine)
        tetgen.SetNoBoundarySplit(self.NoBoundarySplit)
        tetgen.SetQuality(self.Quality)
        tetgen.SetMinRatio(self.MinRatio)
        tetgen.SetVarVolume(self.VarVolume)
        tetgen.SetFixedVolume(self.FixedVolume)
        tetgen.SetMaxVolume(self.MaxVolume)
        tetgen.SetRemoveSliver(self.RemoveSliver)
        tetgen.SetMaxDihedral(self.MaxDihedral)
        tetgen.SetInsertAddPoints(self.InsertAddPoints)
        tetgen.SetRegionAttrib(self.RegionAttrib)
        tetgen.SetEpsilon(self.Epsilon)
        tetgen.SetNoMerge(self.NoMerge)
        tetgen.SetDetectInter(self.DetectInter)
        tetgen.SetCheckClosure(self.CheckClosure)
        tetgen.SetOrder(self.Order)
        tetgen.SetDoCheck(self.DoCheck)
        tetgen.SetVerbose(self.Verbose)
        tetgen.SetPointMarkerArrayName(self.PointMarkerArrayName)
        tetgen.SetFacetMarkerArrayName(self.FacetMarkerArrayName)
        tetgen.SetTetrahedronVolumeArrayName(self.TetrahedronVolumeArrayName)
        tetgen.SetAdditionalPoints(self.AdditionalPoints)
        tetgen.SetOutputSurfaceElements(self.OutputSurfaceElements)
        tetgen.SetOutputVolumeElements(self.OutputVolumeElements)
        tetgen.SetGenerateEntityArrays(self.GenerateEntityArrays)
        tetgen.SetEntityArrayNamePrefix(self.EntityArrayNamePrefix)
        tetgen.Update()

        self.Mesh = tetgen.GetOutput()

        if self.Mesh.GetSource():
            self.Mesh.GetSource().UnRegisterAllOutputs()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
