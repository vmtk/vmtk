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


class vmtkMeshImplicitDistance(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.ReferenceSurface = None
        self.Input = None
        self.Mesh = None
        self.ArrayName = 'ImplicitDistance'
        self.Array = None
        self.ComputeSignedDistance = 1
        self.DistanceThreshold = None
        self.Binary = 0
        self.OutsideValue = 1.0
        self.InsideValue = 0.0
        self.CellData = 0
        self.OverwriteOutsideValue = 1

        self.SetScriptName('vmtkmeshimplicitdistance')
        self.SetScriptDoc('compute distance from a reference surface in an input surface')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['ReferenceSurface','r','vtkPolyData',1,'','the reference surface','vmtksurfacereader'],
            ['ArrayName','array','str',1,'','name of the array of the surface where the computed values are stored'],
            ['ComputeSignedDistance','signeddistance','bool',1,'','if true compute signed distance, else unsigned distance'],
            ['DistanceThreshold','distancethreshold','float',1,'(0.0,)','if set, point more distant than this threshold are taken constant'],
            ['Binary','binary','bool',1,'','fill the distance array with inside/outside values instead of distance values (overwrite the signeddistance value)  '],
            ['InsideValue','inside','float',1,'','value with which the surface is filled where the distance is negative (binary only)'],
            ['OutsideValue','outside','float',1,'','value with which the surface is filled where the distance is positive (binary only)'],
            ['OverwriteOutsideValue','overwriteoutside','bool',1,'','overwrite outside value also when the array already exists in the input surface (binary only)'],
            ['CellData','celldata','bool',1,'','output in a Cell Data array (instead of a Point Data array)']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']
            ])

    def Execute(self):
        if self.Mesh == None:
            self.PrintError('Error: No Mesh.')

        if self.ReferenceSurface == None:
            self.PrintError('Error: No ReferenceSurface.')

        from vmtk import vmtkcontribscripts

        implicitDistance = vmtkcontribscripts.vmtkSurfaceImplicitDistance()
        implicitDistance.Input = self.Mesh
        implicitDistance.ReferenceSurface = self.ReferenceSurface
        implicitDistance.ArrayName = self.ArrayName
        implicitDistance.ComputeSignedDistance = self.ComputeSignedDistance
        implicitDistance.DistanceThreshold = self.DistanceThreshold
        implicitDistance.Binary = self.Binary
        implicitDistance.OutsideValue = self.OutsideValue
        implicitDistance.InsideValue = self.InsideValue
        implicitDistance.CellData = self.CellData
        implicitDistance.OverwriteOutsideValue = self.OverwriteOutsideValue

        implicitDistance.Update()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
