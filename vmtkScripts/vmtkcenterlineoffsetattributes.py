#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkcenterlineoffsetattributes.py,v $
## Language:  Python
## Date:      $Date: 2006/04/06 16:46:43 $
## Version:   $Revision: 1.8 $

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


class vmtkCenterlineOffsetAttributes(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Centerlines = None
        self.ReferenceSystems = None

        self.ReferenceGroupId = -1

        self.AbscissasArrayName = 'Abscissas'
        self.NormalsArrayName = 'ParallelTransportNormals'
        self.GroupIdsArrayName = 'GroupIds'
        self.CenterlineIdsArrayName = 'CenterlineIds'
        self.ReferenceSystemsNormalArrayName = 'Normal'

        self.ReplaceAttributes = 1

        self.OffsetAbscissasArrayName = 'OffsetAbscissas'
        self.OffsetNormalsArrayName = 'OffsetNormals'

        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.Interactive = 0

        self.SetScriptName('vmtkcenterlineoffsetattributes')
        self.SetScriptDoc('offset centerline attributes relative to a bifurcation reference system, in such a way that the abscissa of the closest point the the origin is zero, and the centerline normal at that point coincides with the bifurcation reference system normal')
        self.SetInputMembers([
            ['Centerlines','i','vtkPolyData',1,'','the input split centerlines','vmtksurfacereader'],
            ['ReferenceSystems','referencesystems','vtkPolyData',1,'','bifurcation reference systems','vmtksurfacereader'],
            ['ReferenceGroupId','referencegroupid','int',1,'','group id of the reference system to which attributes have to be offset'],
            ['ReplaceAttributes','replaceattributes','bool',1,'','overwrite the existing attributes'],
              ['AbscissasArrayName','abscissasarray','str',1,'','name of the array where centerline abscissas are stored'],
            ['NormalsArrayName','normalsarray','str',1,'','name of the array where centerline normals are stored'],
            ['GroupIdsArrayName','groupidsarray','str',1,'','name of the array where centerline group ids are stored'],
            ['CenterlineIdsArrayName','centerlineidsarray','str',1,'','name of the array where centerline ids are stored'],
            ['ReferenceSystemsNormalArrayName','referencesystemsnormalarray','str',1,'','name of the array where reference system normals are stored'],
            ['OffsetAbscissasArrayName','offsetabscissasarray','str',1,'','name of the array where offset centerline abscissas have to be stored if ReplaceAttributes is off'],
            ['OffsetNormalsArrayName','offsetnormalsarray','str',1,'','name of the array where offset centerline normals have to be stored if ReplaceAttributes is off'],
                        ['Interactive','interactive','bool',1],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Centerlines','o','vtkPolyData',1,'','the output centerlines','vmtksurfacewriter'],
            ['ReferenceGroupId','referencegroupid','int',1,'','group id of the reference system to which attributes are offset'],
            ['OffsetAbscissasArrayName','offsetabscissasarray','str',1,'','name of the array where offset centerline abscissas are stored if ReplaceAttributes is off'],
            ['OffsetNormalsArrayName','offsetnormalsarray','str',1,'','name of the array where offset centerline normals are stored if ReplaceAttributes is off'],
            ['AbscissasArrayName','abscissasarray','str',1,'','name of the array where centerline abscissas are stored'],
            ['NormalsArrayName','normalsarray','str',1,'','name of the array where centerline normals are stored']
            ])

    def GroupIdValidator(self,text):
        import string
        if not text:
            return 0
        for char in text:
            if char not in string.digits:
                return 0
        return 1

    def Execute(self):

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        if self.ReferenceSystems == None:
            self.PrintError('Error: No input reference systems.')

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

            groupIdString = self.InputText("Please input the reference groupId:\n",self.GroupIdValidator)
            self.ReferenceGroupId = int(groupIdString)

        offsetFilter = vtkvmtk.vtkvmtkCenterlineReferenceSystemAttributesOffset()
        offsetFilter.SetInputData(self.Centerlines)
        offsetFilter.SetReferenceSystems(self.ReferenceSystems)
        offsetFilter.SetAbscissasArrayName(self.AbscissasArrayName)
        offsetFilter.SetNormalsArrayName(self.NormalsArrayName)
        if not self.ReplaceAttributes:
            offsetFilter.SetOffsetAbscissasArrayName(self.OffsetAbscissasArrayName)
            offsetFilter.SetOffsetNormalsArrayName(self.OffsetNormalsArrayName)
        else:
            offsetFilter.SetOffsetAbscissasArrayName(self.AbscissasArrayName)
            offsetFilter.SetOffsetNormalsArrayName(self.NormalsArrayName)
        offsetFilter.SetGroupIdsArrayName(self.GroupIdsArrayName)
        offsetFilter.SetCenterlineIdsArrayName(self.CenterlineIdsArrayName)
        offsetFilter.SetReferenceSystemsNormalArrayName(self.ReferenceSystemsNormalArrayName)
        offsetFilter.SetReferenceSystemsGroupIdsArrayName(self.GroupIdsArrayName)
        offsetFilter.SetReferenceGroupId(self.ReferenceGroupId)
        offsetFilter.Update()

        self.Centerlines = offsetFilter.GetOutput()

        if self.ReferenceGroupId == -1:
            self.ReferenceGroupId = offsetFilter.GetReferenceGroupId()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
