#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkboundarylabeler.py,v $
## Language:  Python

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


class vmtkBoundaryLabeler(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None

        self.BoundaryLabelsArrayName = 'BoundaryLabels'
        self.BoundaryPointOrderArrayName = 'BoundaryPointOrder'
        self.LabelingMode = 'boundaryextractionorder'
        self.PlaneOrigins = None
        self.PlaneNormals = None
        self.PlaneLabels = None
        self.MaximumDistanceFromPlane = 0.0
        self.MaximumDistanceFromPlaneOrigin = 0.0

        self.BoundaryLabels = None
        self.UnmatchedPlaneLabels = None
        self.NumberOfBoundaries = 0

        self.SetScriptName('vmtkboundarylabeler')
        self.SetScriptDoc('give each open boundary of a surface a label that stays with it; the labels are written into the surface\'s own point data, together with the position of each point within its boundary, so that a later filter can tell the boundaries apart without extracting them again and without depending on the order they happen to come out in; vmtkflowextensions and vmtksurfacecapper read them, which is what lets a vessel end be named once and still be the same end after an extension has replaced its boundary')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['BoundaryLabelsArrayName','boundarylabelsarray','str',1,'','name of the point data array where each boundary point\'s label has to be stored'],
            ['BoundaryPointOrderArrayName','boundarypointorderarray','str',1,'','name of the point data array where the position of each point within its own boundary has to be stored'],
            ['LabelingMode','labelingmode','str',1,'["boundaryextractionorder","closesttoplaneorigin","onplane","matchexistinglabels"]','where the labels come from: boundaryextractionorder gives boundary i the label i, i being its position in the order the boundary extraction returns the boundaries in; closesttoplaneorigin gives each plane\'s label to the boundary lying nearest its origin, the normal not being used; onplane gives a boundary a plane\'s label when every one of its points lies in that plane within maxdistancefromplane; matchexistinglabels gives a boundary the label a strict majority of its own points already carry, which is how a surface is brought back into line after a filter that knew nothing about the arrays; it matches labels rather than geometry, so a newly cut boundary gets a fresh label rather than the label of the nearest old one'],
            ['PlaneOrigins','planeorigins','float',-1,'','plane origins, three coordinates per plane; a point is a plane whose normal is never asked for, so this is also where positions go when there are no planes to give'],
            ['PlaneNormals','planenormals','float',-1,'','plane normals, three components per plane; required by the onplane mode and ignored by the others'],
            ['PlaneLabels','planelabels','int',-1,'','label to give the boundary each plane matches; plane i is taken to carry the label i when this is not given'],
            ['MaximumDistanceFromPlane','maxdistancefromplane','float',1,'(0.0,)','how far any point of a boundary may be from a plane for that boundary to count as lying in it; used by the onplane mode, which requires a positive value, a plane being infinite and every boundary lying in it otherwise'],
            ['MaximumDistanceFromPlaneOrigin','maxdistancefromplaneorigin','float',1,'(0.0,)','how far any point of a boundary may be from a plane origin for that boundary to be a candidate for that plane label; a boundary is considered only when it lies entirely within this distance, so one point of it beyond and it is passed over however well the rest fits; zero puts no limit on it; used by both the closesttoplaneorigin and the onplane modes']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the labeled surface','vmtksurfacewriter'],
            ['BoundaryLabelsArrayName','boundarylabelsarray','str',1,'','name of the point data array where each boundary point\'s label is stored'],
            ['BoundaryPointOrderArrayName','boundarypointorderarray','str',1,'','name of the point data array where the position of each point within its own boundary is stored'],
            ['NumberOfBoundaries','numberofboundaries','int',1,'','number of open boundaries found in the surface'],
            ['BoundaryLabels','boundarylabels','int',-1,'','the label given to each boundary, in the order the boundaries were extracted'],
            ['UnmatchedPlaneLabels','unmatchedplanelabels','int',-1,'','the labels of the planes that claimed no boundary']
            ])

    def BuildPoints(self, coordinates, name):
        if len(coordinates) % 3 != 0:
            self.PrintError('Error: ' + name + ' needs three coordinates per plane.')
        points = vtk.vtkPoints()
        for index in range(len(coordinates) // 3):
            points.InsertNextPoint(coordinates[3*index],coordinates[3*index+1],coordinates[3*index+2])
        return points

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        labeler = vtkvmtk.vtkvmtkPolyDataBoundaryLabeler()
        labeler.SetInputData(self.Surface)
        labeler.SetBoundaryLabelsArrayName(self.BoundaryLabelsArrayName)
        labeler.SetBoundaryPointOrderArrayName(self.BoundaryPointOrderArrayName)
        labeler.SetMaximumDistanceFromPlane(self.MaximumDistanceFromPlane)
        labeler.SetMaximumDistanceFromPlaneOrigin(self.MaximumDistanceFromPlaneOrigin)

        if self.LabelingMode == 'boundaryextractionorder':
            labeler.SetLabelingModeToBoundaryExtractionOrder()
        elif self.LabelingMode == 'closesttoplaneorigin':
            labeler.SetLabelingModeToClosestToPlaneOrigin()
        elif self.LabelingMode == 'onplane':
            labeler.SetLabelingModeToOnPlane()
        elif self.LabelingMode == 'matchexistinglabels':
            labeler.SetLabelingModeToMatchExistingLabels()
        else:
            self.PrintError('Error: unsupported labeling mode ' + self.LabelingMode + '.')

        if self.PlaneOrigins:
            labeler.SetPlaneOrigins(self.BuildPoints(self.PlaneOrigins,'planeorigins'))

        if self.PlaneNormals:
            if len(self.PlaneNormals) % 3 != 0:
                self.PrintError('Error: planenormals needs three components per plane.')
            planeNormals = vtk.vtkDoubleArray()
            planeNormals.SetNumberOfComponents(3)
            for index in range(len(self.PlaneNormals) // 3):
                planeNormals.InsertNextTuple3(self.PlaneNormals[3*index],self.PlaneNormals[3*index+1],self.PlaneNormals[3*index+2])
            labeler.SetPlaneNormals(planeNormals)

        if self.PlaneLabels:
            planeLabels = vtk.vtkIdList()
            for planeLabel in self.PlaneLabels:
                planeLabels.InsertNextId(planeLabel)
            labeler.SetPlaneLabels(planeLabels)

        labeler.Update()

        self.Surface = labeler.GetOutput()
        self.NumberOfBoundaries = labeler.GetNumberOfBoundaries()

        boundaryLabels = labeler.GetBoundaryLabels()
        self.BoundaryLabels = [boundaryLabels.GetId(index) for index in range(boundaryLabels.GetNumberOfIds())]

        unmatchedPlaneLabels = labeler.GetUnmatchedPlaneLabels()
        self.UnmatchedPlaneLabels = [unmatchedPlaneLabels.GetId(index) for index in range(unmatchedPlaneLabels.GetNumberOfIds())]

        if self.UnmatchedPlaneLabels:
            self.PrintLog('Warning: no boundary was matched to the planes labeled ' + str(self.UnmatchedPlaneLabels) + '.')


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
