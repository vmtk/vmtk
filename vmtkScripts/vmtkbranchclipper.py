#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkbranchclipper.py,v $
## Language:  Python
## Date:      $Date: 2006/02/23 09:31:39 $
## Version:   $Revision: 1.1 $

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

from vmtk import vmtkrenderer
from vmtk import vmtkcenterlineviewer


class vmtkBranchClipper(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Centerlines = None

        self.RadiusArrayName = 'MaximumInscribedSphereRadius'
        self.CutoffRadiusFactor = 1E16

        self.ClipValue = 0.0

        self.BlankingArrayName = 'Blanking'
        self.GroupIdsArrayName = 'GroupIds'

        self.GroupIds = []
        self.InsideOut = 0

        self.UseRadiusInformation = 1

        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.Interactive = 0

        self.SetScriptName('vmtkbranchclipper')
        self.SetScriptDoc('divide a surface in relation to its split and grouped centerlines')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','','vmtksurfacereader'],
            ['Centerlines','centerlines','vtkPolyData',1,'','','vmtksurfacereader'],
            ['GroupIdsArrayName','groupidsarray','str',1],
            ['GroupIds','groupids','int',-1],
            ['InsideOut','insideout','bool',1],
            ['UseRadiusInformation','useradius','bool',1],
            ['RadiusArrayName','radiusarray','str',1],
            ['BlankingArrayName','blankingarray','str',1],
            ['CutoffRadiusFactor','cutoffradiusfactor','float',1,'(0.0,)'],
            ['ClipValue','clipvalue','float',1],
            ['Interactive','interactive','bool',1],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','','vmtksurfacewriter'],
            ['Centerlines','ocenterlines','vtkPolyData',1,'','','vmtksurfacewriter']
            ])

    def GroupIdsValidator(self,text):
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

        if not self.Surface:
            self.PrintError('Error: No input surface.')

        if not self.Centerlines:
            self.PrintError('Error: No input centerlines.')

        if self.Interactive and not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        if self.Interactive:

            self.vmtkRenderer.RegisterScript(self)

            viewer = vmtkcenterlineviewer.vmtkCenterlineViewer()
            viewer.Centerlines = self.Centerlines
            viewer.CellDataArrayName = self.GroupIdsArrayName
            viewer.vmtkRenderer = self.vmtkRenderer
            viewer.InputText = self.InputText
            viewer.OutputText = self.OutputText
            viewer.PrintError = self.PrintError
            viewer.PringLog = self.PrintLog
            viewer.Display = 0
            viewer.Execute()

            groupIdsString = self.InputText("Please input groupIds to clip:\n",self.GroupIdsValidator)
            self.GroupIds = [int(groupId) for groupId in groupIdsString.split()]

        clipper = vtkvmtk.vtkvmtkPolyDataCenterlineGroupsClipper()
        clipper.SetInputData(self.Surface)
        clipper.SetCenterlines(self.Centerlines)
        clipper.SetCenterlineGroupIdsArrayName(self.GroupIdsArrayName)
        clipper.SetGroupIdsArrayName(self.GroupIdsArrayName)
        clipper.SetCenterlineRadiusArrayName(self.RadiusArrayName)
        clipper.SetBlankingArrayName(self.BlankingArrayName)
        clipper.SetCutoffRadiusFactor(self.CutoffRadiusFactor)
        clipper.SetClipValue(self.ClipValue)
        clipper.SetUseRadiusInformation(self.UseRadiusInformation)
        if self.GroupIds:
            groupIds = vtk.vtkIdList()
            for groupId in self.GroupIds:
                groupIds.InsertNextId(groupId)
            clipper.SetCenterlineGroupIds(groupIds)
            clipper.ClipAllCenterlineGroupIdsOff()
        else:
            clipper.ClipAllCenterlineGroupIdsOn()
        if not self.InsideOut:
            clipper.GenerateClippedOutputOff()
        else:
            clipper.GenerateClippedOutputOn()
        clipper.Update()

        if not self.InsideOut:
            self.Surface = clipper.GetOutput()
        else:
            self.Surface = clipper.GetClippedOutput()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
