#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceconnectivity.py,v $
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


class vmtkSurfaceConnectivity(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.ReferenceSurface = None

        self.ClosestPoint = None

        self.Method = 'largest'

        self.GroupId = -1
        self.GroupIdsArrayName = 'GroupIds'

        self.CleanOutput = 0

        self.SetScriptName('vmtksurfaceconnectivity')
        self.SetScriptDoc('extract the largest connected region, the closest point-connected region, the scalar-connected region from a surface, or all connected regions of a surface tagged with an id')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['Method','method','str',1,'["largest","closest", "all"]','connectivity method'],
            ['ClosestPoint','closestpoint','float',3,'','coordinates of the closest point'],
            ['ReferenceSurface','r','vtkPolyData',1,'','the reference surface, whose barycenter will be used as closest point for the connectivity filter','vmtksurfacereader'],
            ['CleanOutput','cleanoutput','bool',1,'','clean the unused points in the output'],
            ['GroupIdsArrayName','groupidsarray','str',1,'','name of the array containing the connectivity scalar'],
            ['GroupId','groupid','int',1,'','value of the connectivity scalar']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        if (self.GroupId != -1) and (self.GroupIdsArrayName!=''):
            self.Surface.GetPointData().SetActiveScalars(self.GroupIdsArrayName)

        barycenter = [0.0,0.0,0.0]
        if self.Method == 'closest' and self.ClosestPoint == None:
            n = self.ReferenceSurface.GetNumberOfPoints()
            for i in range(n):
                point = self.ReferenceSurface.GetPoint(i)
                barycenter[0] += point[0]
                barycenter[1] += point[1]
                barycenter[2] += point[2]
            barycenter[0] /= n
            barycenter[1] /= n
            barycenter[2] /= n

        connectivityFilter = vtk.vtkPolyDataConnectivityFilter()
        connectivityFilter.SetInputData(self.Surface)
        connectivityFilter.ColorRegionsOff()
        if self.Method == 'largest':
            connectivityFilter.SetExtractionModeToLargestRegion()
        elif self.Method == 'closest':
            connectivityFilter.SetExtractionModeToClosestPointRegion()
            if self.ClosestPoint:
                connectivityFilter.SetClosestPoint(self.ClosestPoint)
            else:
                connectivityFilter.SetClosestPoint(barycenter)
        elif self.Method == 'all':
            connectivityFilter.SetExtractionModeToAllRegions()
            connectivityFilter.ColorRegionsOn()

        if self.GroupId != -1:
            connectivityFilter.ScalarConnectivityOn()
            scalarRange = [self.GroupId,self.GroupId]
            connectivityFilter.SetScalarRange(scalarRange)
        connectivityFilter.Update()

        self.Surface = connectivityFilter.GetOutput()

        if self.CleanOutput == 1:
            cleaner = vtk.vtkCleanPolyData()
            cleaner.SetInputConnection(connectivityFilter.GetOutputPort())
            cleaner.Update()

            self.Surface = cleaner.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
