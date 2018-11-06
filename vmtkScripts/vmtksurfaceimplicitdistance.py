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
        self.ImplicitDistanceArrayName = 'ImplicitDistance'
        self.ComputeSignedDistance = 1
        self.Binary = 0
        self.OutsideValue = 1.0
        self.InsideValue = 0.0
        self.CellData = 0

        self.SetScriptName('vmtksurfaceimplicitdistance')
        self.SetScriptDoc('compute distance from a reference surface in an input surface')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['ReferenceSurface','r','vtkPolyData',1,'','the reference surface','vmtksurfacereader'],
            ['ImplicitDistanceArrayName','distancearray','str',1,'','name of the array of the surface where the implicit distance is stored'],
            ['ComputeSignedDistance','signeddistance','bool',1,'','if true compute signed distance, else unsigned distance'],
            ['Binary','binary','bool',1,'','fill the distance array with inside/outside values instead of distance values (overwrite the signeddistance value)  '],
            ['InsideValue','inside','float',1,'','value with which the surface is filled where the distance is negative'],
            ['OutsideValue','outside','float',1,'','value with which the surface is filled where the distance is positive'],
            ['CellData','celldata','bool',1,'','output in a Cell Data array (instead of a Point Data array)'],
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No Surface.')

        if self.ReferenceSurface == None:
            self.PrintError('Error: No ReferenceSurface.')

        self.PrintLog('Computing Implicit Distance...')

        implicitPolyDataDistance = vtk.vtkImplicitPolyDataDistance()
        implicitPolyDataDistance.SetInput( self.ReferenceSurface )

        if self.CellData:
            numberOfNodes = self.Surface.GetNumberOfCells()
        else:
            numberOfNodes = self.Surface.GetNumberOfPoints()
        implicitDistanceArray = vtk.vtkDoubleArray()
        implicitDistanceArray.SetName( self.ImplicitDistanceArrayName )
        implicitDistanceArray.SetNumberOfComponents( 1 )
        implicitDistanceArray.SetNumberOfTuples( numberOfNodes )
        if self.CellData:
            self.Surface.GetCellData().AddArray( implicitDistanceArray )
            cellCenterParametric = [0., 0., 0.]
            cellCenter = [0., 0., 0.]
            cellWeights = [0., 0., 0.]
        else:
            self.Surface.GetPointData().AddArray( implicitDistanceArray )

        for i in range( numberOfNodes ):
            if self.CellData:
                cellSubId = self.Surface.GetCell(i).GetParametricCenter( cellCenterParametric )
                subId = vtk.mutable( cellSubId )
                self.Surface.GetCell(i).EvaluateLocation( subId, cellCenterParametric, cellCenter, cellWeights ) 
                inputPoint = cellCenter
            else:
                inputPoint = self.Surface.GetPoint(i)
            signedDistance = implicitPolyDataDistance.EvaluateFunction( inputPoint )
            if self.Binary:
                if signedDistance < 0.:
                    value = self.InsideValue
                else:
                    value = self.OutsideValue
            elif self.ComputeSignedDistance:
                value = signedDistance
            else:
                value = abs( signedDistance )
            implicitDistanceArray.SetComponent( i, 0, value )



if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
