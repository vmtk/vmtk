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
            ['InterfaceLabel','ilabel','float',1,'(0.0,)','label of the interface '],
            ['SizeMin','minsize','float',1,'(0.0,)','target minimum refinement size (on the mfm)'],
            ['Alpha','alpha','float',1,'(0.0,)','target alpha factor (surface refinement with h = alpha * distance ^ beta)'],
            ['Beta','beta','float',1,'(0.0,)','target beta factor (surface refinement with h = alpha * distance ^ beta)']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter'],
            ['InterfaceSurface','intsurface','vtkPolyData',1,'','the surface','vmtksurfacewriter'],
            ])

    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: No Mesh.')

        #agiungere tutti i controlli tipo se esiste l'array ecc

        from vmtk import vmtkscripts

        meshToSurface = vmtkscripts.vmtkMeshToSurface()
        meshToSurface.Mesh = self.Mesh
        meshToSurface.Execute()
        surface = meshToSurface.Surface

        threshold = vtk.vtkThreshold()
        threshold.SetInputData( self.Mesh )
        threshold.ThresholdBetween( self.InterfaceLabel - 0.5, self.InterfaceLabel + 0.5 )
        threshold.SetInputArrayToProcess( 0, 0, 0, 1, self.CellEntityIdsArrayName )
        threshold.Update()
        meshToSurface = vmtkscripts.vmtkMeshToSurface()
        meshToSurface.Mesh = threshold.GetOutput()
        meshToSurface.Execute()
        interfaceSurface = meshToSurface.Surface

        self.PrintLog("Computing refined sizing function")
        numberOfNodes = self.Mesh.GetNumberOfPoints()
        refinedSizingArray = vtk.vtkDoubleArray()
        refinedSizingArray.SetName( self.RefinedSizingFunctionArrayName )
        refinedSizingArray.SetNumberOfComponents( 1 )
        refinedSizingArray.SetNumberOfTuples( numberOfNodes )
        self.Mesh.GetPointData().AddArray( refinedSizingArray )

        distanceArray = self.Mesh.GetPointData().GetArray( self.DistanceArrayName )
        if distanceArray == None:
            self.PrintError( 'Error: No Point Data Array called ' + self.DistanceArrayName )

        mesh = self.Mesh
        for i in range( numberOfNodes ):
            distance = distanceArray.GetComponent( i, 0 )
            if self.UseMagnitude:
                distance = abs( distance )
            #refinedSizingArray.SetTuple1(i,max(self.SizeMin,min(vmtksize, self.Alpha * pow(distance,self.Beta))))
            refinedSizingArray.SetTuple1( i, max( self.SizeMin, self.Alpha * pow( distance, self.Beta ) ) )
        
        self.PrintLog("Generating refined mesh")
        tetgen=vmtkscripts.vmtkTetGen()
        tetgen.Mesh= mesh
        tetgen.GenerateCaps = 0
        tetgen.UseSizingFunction = 1
        tetgen.Refine = 1
        tetgen.SizingFunctionArrayName = self.RefinedSizingFunctionArrayName
        tetgen.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        tetgen.Order = 1
        tetgen.Quality = 1
        tetgen.PLC = 0
        tetgen.NoBoundarySplit = 0 #consente il raffinamento della superficie
        tetgen.RemoveSliver = 0
        tetgen.OutputSurfaceElements = 0
        tetgen.OutputVolumeElements = 1
        tetgen.Execute()
        
        meshToSurface.Mesh = tetgen.Mesh
        meshToSurface.Execute()
        surfaceRefined = meshToSurface.Surface
        
        self.PrintLog("Projecting surface labels")
        cellToPoint = vtk.vtkCellDataToPointData()
        cellToPoint.SetInputData( surface )
        cellToPoint.Update()
        surface2 = cellToPoint.GetPolyDataOutput()
        
        # not really robust
        for j in range( surface2.GetNumberOfPoints() ):
            if (surface2.GetPointData().GetArray( self.CellEntityIdsArrayName ).GetComponent( j, 0 ) > 10 ):
                surface2.GetPointData().GetArray( self.CellEntityIdsArrayName ).SetTuple1( j, self.InterfaceLabel )
        
        projection = vmtkscripts.vmtkSurfaceProjection()
        projection.Surface = surfaceRefined
        projection.ReferenceSurface = surface2
        projection.Execute()

        cellList = vtk.vtkIdList()
        surfaceRefined = projection.Surface
        for i in range( surfaceRefined.GetNumberOfPoints() ):
            if surfaceRefined.GetPointData().GetArray( self.CellEntityIdsArrayName ).GetComponent( i, 0 ) != self.InterfaceLabel:
                valmin = self.InterfaceLabel
                surfaceRefined.GetPointCells( i, cellList )
                for j in range( cellList.GetNumberOfIds() ):
                    pointList = vtk.vtkIdList()
                    surfaceRefined.GetCellPoints( cellList.GetId( j ), pointList )
                    for k in range(0,3):
                        if surfaceRefined.GetPointData().GetArray( self.CellEntityIdsArrayName ).GetComponent( pointList.GetId( k ), 0 ) < valmin:
                            valmin = surfaceRefined.GetPointData().GetArray( self.CellEntityIdsArrayName ).GetComponent( pointList.GetId( k ), 0 )
                surfaceRefined.GetPointData().GetArray( self.CellEntityIdsArrayName ).SetTuple1( i, valmin )
        
        surfaceRefined.GetCellData().GetArray( self.CellEntityIdsArrayName ).FillComponent( 0, self.InterfaceLabel )
        
        for i in range( surfaceRefined.GetNumberOfPoints() ):
            if surfaceRefined.GetPointData().GetArray( self.CellEntityIdsArrayName ).GetComponent( i, 0 ) != self.InterfaceLabel:
                value = surfaceRefined.GetPointData().GetArray( self.CellEntityIdsArrayName ).GetComponent( i, 0 )
                surfaceRefined.GetPointCells( i, cellList )
                for j in range( cellList.GetNumberOfIds() ):
                    surfaceRefined.GetCellData().GetArray( self.CellEntityIdsArrayName ).SetComponent( cellList.GetId( j ), 0, value )
        
        self.PrintLog("Assembling final mesh")
        appendFilter = vtkvmtk.vtkvmtkAppendFilter()
        appendFilter.AddInputData( surfaceRefined )
        appendFilter.AddInputData( tetgen.Mesh )
        appendFilter.Update()
        
        self.Mesh = appendFilter.GetOutput()
        self.InterfaceSurface = surfaceRefined

        
if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
