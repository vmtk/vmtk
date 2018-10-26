#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshimplicitdistance.py,v $
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



class vmtkMeshImplicitMesh(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.ReferenceSurface = None
        self.Mesh = None
        self.ImplicitDistanceArrayName = 'ImplicitDistance'
        self.ComputeUnsigned = 0
        self.UnsignedImplicitDistanceArrayName = 'UnsignedImplicitDistance'

        self.SetScriptName('vmtkmeshimplicitdistance')
        self.SetScriptDoc('define an implicit description of a reference Mesh in the input Mesh')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['ReferenceSurface','r','vtkPolyData',1,'','the reference surface','vmtksurfacereader'],
            ['ImplicitDistanceArrayName','implicitdistancearray','str',1,'','name of the array of the Mesh where the implicit distance is stored'],
            ['ComputeUnsigned','computeunsigned','bool',1,'','compute unsigned implicit distance'],
            ['UnsignedImplicitDistanceArrayName','unsignedimplicitdistancearray','str',1,'','name of the array of the Mesh where the unsigned implicit Mesh is stored']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output Mesh','vmtkmeshwriter']
            ])

    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: No Mesh.')

        if self.ReferenceSurface == None:
            self.PrintError('Error: No ReferenceSurface.')

        self.PrintLog('Computing Implicit Distance...')

        implicitPolyDataDistance = vtk.vtkImplicitPolyDataDistance()
        implicitPolyDataDistance.SetInput(self.ReferenceSurface)

        numberOfNodes = self.Mesh.GetNumberOfPoints()
        implicitDistanceArray = vtk.vtkDoubleArray()
        implicitDistanceArray.SetName(self.ImplicitDistanceArrayName)
        implicitDistanceArray.SetNumberOfComponents(1)
        implicitDistanceArray.SetNumberOfTuples(numberOfNodes)
        self.Mesh.GetPointData().AddArray(implicitDistanceArray)

        for i in range(numberOfNodes):
            implicitDistanceArray.SetComponent( i, 0, implicitPolyDataDistance.EvaluateFunction( self.Mesh.GetPoint(i) ) )

        if self.ComputeUnsigned:
            unsignedImplicitDistanceArray = vtk.vtkDoubleArray()
            unsignedImplicitDistanceArray.SetName(self.UnsignedImplicitDistanceArrayName)
            unsignedImplicitDistanceArray.SetNumberOfComponents(1)
            unsignedImplicitDistanceArray.SetNumberOfTuples(numberOfNodes)
            self.Mesh.GetPointData().AddArray(unsignedImplicitDistanceArray)

            for i in range(numberOfNodes):
                unsignedImplicitDistanceArray.SetComponent( i, 0, abs( implicitDistanceArray.GetComponent( i, 0 ) ) )



if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
