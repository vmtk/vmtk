#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkcenterlinelabeler.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:34:45 $
## Version:   $Revision: 1.2 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import vtkvmtk
from vmtk import vmtkrenderer
from vmtk import pypes

from vmtk import vmtkcenterlineviewer


class vmtkCenterlineLabeler(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Centerlines = None
        self.GroupIdsArrayName = 'GroupIds'
        self.LabelIdsArrayName = 'LabelIds'

        self.Labeling = []

        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.SetScriptName('vmtkcenterlinelabeler')
        self.SetScriptDoc('interactively change the group ids of a split centerline')
        self.SetInputMembers([
            ['Centerlines','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['GroupIdsArrayName','groupidsarray','str',1,''],
            ['LabelIdsArrayName','labelidsarray','str',1,''],
            ['Labeling','labeling','int',-1,''],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']])
        self.SetOutputMembers([
            ['Centerlines','o','vtkPolyData',1,'','the output centerlines','vmtksurfacewriter'],
            ['LabelIdsArrayName','labelidsarray','str',1,'']])

    def LabelValidator(self,text):
        import string
        if not text:
            return 0
        if not text.split():
            return 0
        for char in text:
            if char not in string.digits + " ":
                return 0
        return 1

    def Execute(self):

        if not self.Centerlines:
            self.PrintError('Error: No input centerlines.')

        if not self.GroupIdsArrayName:
            self.PrintError('Error: GroupIdsArrayName not specified.')

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self)

        groupIdsArray = self.Centerlines.GetCellData().GetArray(self.GroupIdsArrayName)
        groupIds = []
        for i in range(groupIdsArray.GetNumberOfTuples()):
            groupIds.append(int(groupIdsArray.GetComponent(i,0)))
        groupIds.sort()
        uniqueGroupIds = []
        for groupId in groupIds:
            if groupId not in uniqueGroupIds:
                uniqueGroupIds.append(groupId)

        labelMap = {}

        if not self.Labeling:

            viewer = vmtkcenterlineviewer.vmtkCenterlineViewer()
            viewer.Centerlines = self.Centerlines
            viewer.CellDataArrayName = self.GroupIdsArrayName
            viewer.vmtkRenderer = self.vmtkRenderer
            viewer.InputStream = self.InputStream
            viewer.OutputStream = self.OutputStream
            #viewer.InputText = self.InputText
            #viewer.OutputText = self.OutputText
            #viewer.PrintError = self.PrintError
            #viewer.PringLog = self.PrintLog
            viewer.Execute()

            ok = False
            while not ok:
                labelString = self.InputText("Please input labels for the following groupIds:\n%s\n" % " ".join([str(groupId) for groupId in uniqueGroupIds]),self.LabelValidator)
                labels = [int(label) for label in labelString.split()]
                if len(labels) == len(uniqueGroupIds):
                    ok = True

            for groupId in uniqueGroupIds:
                labelMap[groupId] = labels[uniqueGroupIds.index(groupId)]

        else:

            if len(self.Labeling) != 2 * len(uniqueGroupIds):
                self.PrintError('Error: incorrect labeling specified')

            for i in range(len(self.Labeling)//2):
                groupId = self.Labeling[2*i]
                labelId = self.Labeling[2*i+1]
                if not groupId in uniqueGroupIds:
                    self.PrintError('Error: groupId %d does not exist' % groupId)
                labelMap[groupId] = labelId

        labelIdsArray = vtk.vtkIntArray()
        labelIdsArray.SetName(self.LabelIdsArrayName)
        labelIdsArray.SetNumberOfComponents(1)
        labelIdsArray.SetNumberOfTuples(self.Centerlines.GetNumberOfCells())

        groupIdsArray = self.Centerlines.GetCellData().GetArray(self.GroupIdsArrayName)

        for i in range(groupIdsArray.GetNumberOfTuples()):
            groupId = int(groupIdsArray.GetComponent(i,0))
            labelIdsArray.SetComponent(i,0,labelMap[groupId])

        self.Centerlines.GetCellData().AddArray(labelIdsArray)

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
