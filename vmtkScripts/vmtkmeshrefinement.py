#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshrefinement.py,v $
## Language:  Python
## Date:      $Date: 2014/10/24 16:35:13 $
## Version:   $Revision: 1.10 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by 
##       Marco Fedele (marco.fedele@polimi.it)
##       Politecnico di Milano

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY

import vtk
import sys

from vmtk import vtkvmtk
from vmtk import pypes

vmtkmeshrefinement = 'vmtkMeshRefinement'

class vmtkMeshRefinement(pypes.pypeScript):
    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.DistanceArrayName = 'ImplicitDistance'
        self.UseMagnitude = 0
        self.SizingFunctionArrayName = 'VolumeSizingFunction'
        self.RefinedSizingFunctionArrayName = 'RefinedSizingFunction'
        self.VolumeElementScaleFactor = 0.8
        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.InterfaceLabel = 200
        self.SizeMin = 0.01
        self.Alpha = 0.1
        self.Beta = 1.0
        
        self.InterfaceSurface = None
        
        self.SetScriptName('vmtkmeshrefinement')
        self.SetScriptDoc('refine a tetra mesh near the level zero of an array (e.g. a distance from a surface)')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['DistanceArrayName','distancearray','str',1,'','name of the array of the mesh where the distance is stored'],
            ['UseMagnitude','usemagnitude','bool',1,'','use magnitude of the input distance array'],
            ['SizeMin','minsize','float',1,'(0.0,)','target minimum refinement size (on the mfm)'],
            ['Alpha','alpha','float',1,'(0.0,)','target alpha factor (surface refinement with h = alpha * distance ^ beta)'],
            ['Beta','beta','float',1,'(0.0,)','target beta factor (surface refinement with h = alpha * distance ^ beta)']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']
            ])

    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: No Mesh.')

        from vmtk import vmtkscripts

        self.PrintLog("Computing refined sizing function ...")
        numberOfNodes = self.Mesh.GetNumberOfPoints()
        refinedSizingArray = vtk.vtkDoubleArray()
        refinedSizingArray.SetName( self.RefinedSizingFunctionArrayName )
        refinedSizingArray.SetNumberOfComponents( 1 )
        refinedSizingArray.SetNumberOfTuples( numberOfNodes )
        self.Mesh.GetPointData().AddArray( refinedSizingArray )

        distanceArray = self.Mesh.GetPointData().GetArray( self.DistanceArrayName )
        if distanceArray == None:
            self.PrintError( 'Error: No Point Data Array called ' + self.DistanceArrayName )

        for i in range( numberOfNodes ):
            distance = distanceArray.GetComponent( i, 0 )
            if self.UseMagnitude:
                distance = abs( distance )
            #refinedSizingArray.SetTuple1(i,max(self.SizeMin,min(vmtksize, self.Alpha * pow(distance,self.Beta))))
            refinedSizingArray.SetTuple1( i, max( self.SizeMin, self.Alpha * pow( distance, self.Beta ) ) )

        inputMesh =  vtk.vtkUnstructuredGrid()
        inputMesh.DeepCopy(self.Mesh)

        self.PrintLog("Generating refined mesh ...")
        tetgen = vmtkscripts.vmtkTetGen()
        tetgen.Mesh = self.Mesh
        tetgen.GenerateCaps = 0
        tetgen.UseSizingFunction = 1
        tetgen.Refine = 1
        tetgen.SizingFunctionArrayName = self.RefinedSizingFunctionArrayName
        tetgen.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        tetgen.Order = 1
        tetgen.Quality = 1
        tetgen.PLC = 0
        tetgen.NoBoundarySplit = 0 # to allow surface-refinemnt
        tetgen.RemoveSliver = 0
        tetgen.OutputSurfaceElements = 0
        tetgen.OutputVolumeElements = 1
        tetgen.Execute()

        self.PrintLog("Projecting entityids ...")
        mts = vmtkscripts.vmtkMeshToSurface()
        mts.Mesh = tetgen.Mesh
        mts.CleanOutput = 1
        mts.Execute()

        af = vtkvmtk.vtkvmtkAppendFilter()
        af.AddInputData(tetgen.Mesh)
        af.AddInputData(mts.Surface)
        af.Update()
        self.Mesh = af.GetOutput()

        meshProj = vmtkscripts.vmtkMeshProjection()
        meshProj.Mesh =  self.Mesh
        meshProj.ReferenceMesh = inputMesh
        meshProj.Method = 'closestpoint'
        meshProj.ActiveArrays = [self.CellEntityIdsArrayName]
        meshProj.LineSurfaceVolume = 1
        meshProj.Execute()

        self.Mesh = meshProj.Mesh



if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
