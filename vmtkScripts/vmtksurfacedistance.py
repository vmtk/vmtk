#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacedistance.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.6 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
from vmtk import vtkvmtk
import sys

from vmtk import pypes


class vmtkSurfaceDistance(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.ReferenceSurface = None
        self.Surface = None
        self.DistanceArrayName = ''
        self.DistanceVectorsArrayName = ''
        self.SignedDistanceArrayName = ''

        self.FlipNormals = 0

        self.SetScriptName('vmtksurfacedistance')
        self.SetScriptDoc('compute the pointwise minimum distance of the input surface from a reference surface')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['ReferenceSurface','r','vtkPolyData',1,'','the reference surface','vmtksurfacereader'],
            ['DistanceArrayName','distancearray','str',1,'','name of the array where the distance of the input surface to the reference surface has to be stored'],
            ['DistanceVectorsArrayName','distancevectorsarray','str',1,'','name of the array where the distance vectors of the input surface to the reference surface has to be stored'],
            ['SignedDistanceArrayName','signeddistancearray','str',1,'','name of the array where the signed distance of the input surface to the reference surface is stored; distance is positive if distance vector and normal to the reference surface have negative dot product, i.e. if the input surface is outer with respect to the reference surface'],
            ['FlipNormals','flipnormals','bool',1,'','flip normals to the reference surface after computing them']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No Surface.')

        if self.ReferenceSurface == None:
            self.PrintError('Error: No ReferenceSurface.')

        if self.SignedDistanceArrayName != '':
            normalsFilter = vtk.vtkPolyDataNormals()
            normalsFilter.SetInputData(self.ReferenceSurface)
            normalsFilter.AutoOrientNormalsOn()
            normalsFilter.SetFlipNormals(self.FlipNormals)
            normalsFilter.Update()
            self.ReferenceSurface.GetPointData().SetNormals(normalsFilter.GetOutput().GetPointData().GetNormals())

        if self.DistanceArrayName != '' or self.DistanceVectorsArrayName != '' or self.SignedDistanceArrayName != '':
            self.PrintLog('Computing distance.')
            surfaceDistance = vtkvmtk.vtkvmtkSurfaceDistance()
            surfaceDistance.SetInputData(self.Surface)
            surfaceDistance.SetReferenceSurface(self.ReferenceSurface)
            if self.DistanceArrayName != '':
                surfaceDistance.SetDistanceArrayName(self.DistanceArrayName)
            if self.DistanceVectorsArrayName != '':
                surfaceDistance.SetDistanceVectorsArrayName(self.DistanceVectorsArrayName)
            if self.SignedDistanceArrayName != '':
                surfaceDistance.SetSignedDistanceArrayName(self.SignedDistanceArrayName)
            surfaceDistance.Update()
            self.Surface = surfaceDistance.GetOutput()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
