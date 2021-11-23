#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshconnectivity.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.8 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes


class vmtkMeshConnectivity(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.ReferenceMesh = None

        self.ClosestPoint = None

        self.Method = 'largest'

        self.SetScriptName('vmtkmeshconnectivity')
        self.SetScriptDoc('extract the largest connected region or the closest point-connected region from a mesh')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['Method','method','str',1,'["largest","closest"]','connectivity method'],
            ['ClosestPoint','closestpoint','float',3,'','coordinates of the closest point'],
            ['ReferenceMesh','r','vtkUnstructuredGrid',1,'','the reference mesh, whose barycenter will be used as closest point for the connectivity filter','vmtkmeshreader']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']
            ])

    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: No input mesh.')

        barycenter = [0.0,0.0,0.0]
        if self.Method == 'closest' and self.ClosestPoint == None:
            n = self.ReferenceMesh.GetNumberOfPoints()
            for i in range(n):
                point = self.ReferenceMesh.GetPoint(i)
                barycenter[0] += point[0]
                barycenter[1] += point[1]
                barycenter[2] += point[2]
            barycenter[0] /= n
            barycenter[1] /= n
            barycenter[2] /= n

        connectivityFilter = vtk.vtkConnectivityFilter()
        connectivityFilter.SetInputData(self.Mesh)
        connectivityFilter.ColorRegionsOff()
        if self.Method == 'largest':
            connectivityFilter.SetExtractionModeToLargestRegion()
        elif self.Method == 'closest':
            connectivityFilter.SetExtractionModeToClosestPointRegion()
            if self.ClosestPoint:
                connectivityFilter.SetClosestPoint(self.ClosestPoint)
            else:
                connectivityFilter.SetClosestPoint(barycenter)
        connectivityFilter.Update()

        self.Mesh = connectivityFilter.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
