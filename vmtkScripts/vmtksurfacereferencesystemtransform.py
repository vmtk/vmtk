#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacereferencesystemtransform.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.7 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import math
import sys

from vmtk import pypes


class vmtkSurfaceReferenceSystemTransform(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.ReferenceSystems = None

        self.ReferenceSystemId = -1

        self.ReferenceSystemsIdArrayName = ''
        self.ReferenceSystemsNormal1ArrayName = ''
        self.ReferenceSystemsNormal2ArrayName = ''

        self.ReferenceOrigin = [0.0, 0.0, 0.0]
        self.ReferenceNormal1 = [0.0, 0.0, 1.0]
        self.ReferenceNormal2 = [0.0, 1.0, 0.0]

        self.SetScriptName('vmtksurfacereferencesystemtransform')
        self.SetScriptDoc('translate and rotate a surface in order to orient its reference system with a target reference system')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['ReferenceOrigin','referenceorigin','float',3,'','origin of the target reference system'],
            ['ReferenceNormal1','referencenormal1','float',3,'','first normal of the target reference system'],
            ['ReferenceNormal2','referencenormal2','float',3,'','second normal of the target reference system'],
            ['ReferenceSystems','referencesystems','vtkPolyData',1,'','reference systems of the input surface represented by a set of points with two normals each','vmtksurfacereader'],
            ['ReferenceSystemId','referencesystemid','int',1,'','id of the reference system to use'],
            ['ReferenceSystemsIdArrayName','referencesystemidsarray','str',1,'','name of the array where reference system ids are stored'],
            ['ReferenceSystemsNormal1ArrayName','normal1array','str',1,'','name of the array where the first normals defining the reference systems are stored'],
            ['ReferenceSystemsNormal2ArrayName','normal2array','str',1,'','name of the array where the second normals defining the reference systems are stored']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def ComputeAngle(self,in_vector0,in_vector1):
        vector0 = [in_vector0[0], in_vector0[1], in_vector0[2]]
        vector1 = [in_vector1[0], in_vector1[1], in_vector1[2]]
        vtk.vtkMath.Normalize(vector0)
        vtk.vtkMath.Normalize(vector1)
        sum = [
            vector0[0] + vector1[0],
            vector0[1] + vector1[1],
            vector0[2] + vector1[2]]
        difference = [
            vector0[0] - vector1[0],
            vector0[1] - vector1[1],
            vector0[2] - vector1[2]]
        sumNorm = vtk.vtkMath.Norm(sum)
        differenceNorm = vtk.vtkMath.Norm(difference)
##         angle = 2.0 * math.atan2(differenceNorm,sumNorm) + math.pi
        angle = 2.0 * math.atan2(differenceNorm,sumNorm)
        return angle

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No Surface.')

        if self.ReferenceSystems == None:
            self.PrintError('Error: No ReferenceSystems.')

        referenceSystemsNormal1Array = self.ReferenceSystems.GetPointData().GetArray(self.ReferenceSystemsNormal1ArrayName)
        referenceSystemsNormal2Array = self.ReferenceSystems.GetPointData().GetArray(self.ReferenceSystemsNormal2ArrayName)

        referenceSystemPointId = 0

        if self.ReferenceSystemId != -1:
            referenceSystemsIdArray = self.ReferenceSystems.GetPointData().GetArray(self.ReferenceSystemsIdArrayName)
            for i in range(self.ReferenceSystems.GetNumberOfPoints()):
                referenceSystemId = int(referenceSystemsIdArray.GetTuple1(i))
                if referenceSystemId == self.ReferenceSystemId:
                    referenceSystemPointId = i
                    break

        currentOrigin = self.ReferenceSystems.GetPoint(referenceSystemPointId)
        currentNormal1 = referenceSystemsNormal1Array.GetTuple3(referenceSystemPointId)
        currentNormal2 = referenceSystemsNormal2Array.GetTuple3(referenceSystemPointId)

        transform = vtk.vtkTransform()
        transform.PostMultiply()

        translation = [0.0, 0.0, 0.0]
        translation[0] = self.ReferenceOrigin[0] - currentOrigin[0]
        translation[1] = self.ReferenceOrigin[1] - currentOrigin[1]
        translation[2] = self.ReferenceOrigin[2] - currentOrigin[2]

        transform.Translate(translation)

        cross = [0.0,0.0,0.0]

        vtk.vtkMath.Cross(currentNormal1,self.ReferenceNormal1,cross)
        vtk.vtkMath.Normalize(cross)
        angle = self.ComputeAngle(currentNormal1,self.ReferenceNormal1)
        angle = angle / (2.0*vtk.vtkMath.Pi()) * 360.0;
        transform.RotateWXYZ(angle,cross)

        transformedNormal2 = transform.TransformNormal(currentNormal2)
        vtk.vtkMath.Cross(transformedNormal2,self.ReferenceNormal2,cross)
        vtk.vtkMath.Normalize(cross)
        angle = self.ComputeAngle(transformedNormal2,self.ReferenceNormal2)
        angle = angle / (2.0*vtk.vtkMath.Pi()) * 360.0;
        transform.RotateWXYZ(angle,cross)

        transformFilter = vtk.vtkTransformPolyDataFilter()
        transformFilter.SetInputData(self.Surface)
        transformFilter.SetTransform(transform)
        transformFilter.Update()

        self.Surface = transformFilter.GetOutput()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
