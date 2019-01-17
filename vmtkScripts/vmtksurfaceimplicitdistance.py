#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceimplicitdistance.py,v $
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



class vmtkSurfaceImplicitDistance(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.ReferenceSurface = None
        self.Surface = None
        self.Input = None
        self.ArrayName = 'ImplicitDistance'
        self.Array = None
        self.ComputeSignedDistance = 1
        self.Binary = 0
        self.OutsideValue = 1.0
        self.InsideValue = 0.0
        self.CellData = 0
        self.OverwriteOutsideValue = 1

        self.SetScriptName('vmtksurfaceimplicitdistance')
        self.SetScriptDoc('compute distance from a reference surface in an input surface')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['ReferenceSurface','r','vtkPolyData',1,'','the reference surface','vmtksurfacereader'],
            ['ArrayName','array','str',1,'','name of the array of the surface where the computed values are stored'],
            ['ComputeSignedDistance','signeddistance','bool',1,'','if true compute signed distance, else unsigned distance'],
            ['Binary','binary','bool',1,'','fill the distance array with inside/outside values instead of distance values (overwrite the signeddistance value)  '],
            ['InsideValue','inside','float',1,'','value with which the surface is filled where the distance is negative (binary only)'],
            ['OutsideValue','outside','float',1,'','value with which the surface is filled where the distance is positive (binary only)'],
            ['OverwriteOutsideValue','overwriteoutside','bool',1,'','overwrite outside value also when the array already exists in the input surface (binary only)'],
            ['CellData','celldata','bool',1,'','output in a Cell Data array (instead of a Point Data array)']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])


    def Update(self):
        self.PrintLog('Computing Implicit Distance...')

        implicitPolyDataDistance = vtk.vtkImplicitPolyDataDistance()
        implicitPolyDataDistance.SetInput( self.ReferenceSurface )

        if self.CellData:
            numberOfNodes = self.Input.GetNumberOfCells()
            self.Array = self.Input.GetCellData().GetArray( self.ArrayName )
        else:
            numberOfNodes = self.Input.GetNumberOfPoints()
            self.Array = self.Input.GetPointData().GetArray( self.ArrayName )

        overwriteArray = False
        if self.Array == None or self.OverwriteOutsideValue:
            overwriteArray = True

        if overwriteArray:
            self.Array = vtk.vtkDoubleArray()
            self.Array.SetName( self.ArrayName )
            self.Array.SetNumberOfComponents( 1 )
            self.Array.SetNumberOfTuples( numberOfNodes )
            if self.CellData:
                self.Input.GetCellData().AddArray( self.Array )
                cellCenterParametric = [0., 0., 0.]
                cellCenter = [0., 0., 0.]
                cellWeights = [0., 0., 0.]
            else:
                self.Input.GetPointData().AddArray( self.Array )

        for i in range( numberOfNodes ):
            if self.CellData:
                cellSubId = self.Input.GetCell(i).GetParametricCenter( cellCenterParametric )
                subId = vtk.mutable( cellSubId )
                self.Input.GetCell(i).EvaluateLocation( subId, cellCenterParametric, cellCenter, cellWeights )
                inputPoint = cellCenter
            else:
                inputPoint = self.Input.GetPoint(i)
            signedDistance = implicitPolyDataDistance.EvaluateFunction( inputPoint )
            if self.Binary:
                if signedDistance < 0.:
                    value = self.InsideValue
                elif overwriteArray:
                    value = self.OutsideValue
                else:
                    value = self.Array.GetValue( i )
            elif self.ComputeSignedDistance:
                value = signedDistance
            else:
                value = abs( signedDistance )
            self.Array.SetComponent( i, 0, value )


    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No Surface.')

        if self.ReferenceSurface == None:
            self.PrintError('Error: No ReferenceSurface.')

        self.Input = self.Surface

        self.Update()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
