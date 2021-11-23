#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkicpregistration.py,v $
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
from vmtk import vtkvmtk
import sys

from vmtk import pypes


class vmtkICPRegistration(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.ReferenceSurface = None
        self.Surface = None
        self.DistanceArrayName = ''
        self.SignedDistanceArrayName = ''
        self.FarThreshold = 0.0
        self.Level = 0.0
        self.MaximumMeanDistance = 1E-2
        self.MaximumNumberOfLandmarks = 1000
        self.MaximumNumberOfIterations = 100
        self.Matrix4x4 = None
        self.MatrixCoefficients = None

        self.FlipNormals = 0

        self.SetScriptName('vmtkicpregistration')
        self.SetScriptDoc('register a surface to a reference surface using the ICP algorithm')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['ReferenceSurface','r','vtkPolyData',1,'','the reference surface','vmtksurfacereader'],
            ['DistanceArrayName','distancearray','str',1,'','name of the array where the distance of the input surface to the reference surface has to be stored'],
            ['SignedDistanceArrayName','signeddistancearray','str',1,'','name of the array where the signed distance of the input surface to the reference surface is stored; distance is positive if distance vector and normal to the reference surface have negative dot product, i.e. if the input surface is outer with respect to the reference surface'],
            ['FarThreshold','farthreshold','float',1,'','threshold distance beyond which points are discarded during optimization'],
            ['FlipNormals','flipnormals','bool',1,'','flip normals to the reference surface after computing them'],
            ['MaximumNumberOfLandmarks','landmarks','int',1,'','maximum number of landmarks sampled from the two surfaces for evaluation of the registration metric'],
            ['MaximumNumberOfIterations','iterations','int',1,'','maximum number of iterations for the optimization problems'],
            ['MaximumMeanDistance','maxmeandistance','float',1,'','convergence threshold based on the maximum mean distance between the two surfaces']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ['MatrixCoefficients','omatrixcoefficients','float',16,'','the output transform matrix coefficients'],
            ['Matrix4x4','omatrix4x4','vtkMatrix4x4',1,'','the output transform matrix']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No Surface.')

        if self.ReferenceSurface == None:
            self.PrintError('Error: No ReferenceSurface.')

##         if (self.SignedDistanceArrayName != '') & (self.ReferenceSurface.GetPointData().GetNormals() == None):
        if (self.SignedDistanceArrayName != ''):
            normalsFilter = vtk.vtkPolyDataNormals()
            normalsFilter.SetInputData(self.ReferenceSurface)
            normalsFilter.AutoOrientNormalsOn()
            normalsFilter.ConsistencyOn()
            normalsFilter.SplittingOff()
            normalsFilter.SetFlipNormals(self.FlipNormals)
            normalsFilter.Update()
            self.ReferenceSurface.GetPointData().SetNormals(normalsFilter.GetOutput().GetPointData().GetNormals())

        self.PrintLog('Computing ICP transform.')

        icpTransform = vtkvmtk.vtkvmtkIterativeClosestPointTransform()
        icpTransform.SetSource(self.Surface)
        icpTransform.SetTarget(self.ReferenceSurface)
        icpTransform.GetLandmarkTransform().SetModeToRigidBody()
        icpTransform.StartByMatchingCentroidsOn()
        icpTransform.CheckMeanDistanceOn()
        icpTransform.SetMaximumNumberOfLandmarks(self.MaximumNumberOfLandmarks)
        icpTransform.SetMaximumNumberOfIterations(self.MaximumNumberOfIterations)
        icpTransform.SetMaximumMeanDistance(self.MaximumMeanDistance)
        if self.FarThreshold > 0.0:
            icpTransform.UseFarThresholdOn()
            icpTransform.SetFarThreshold(self.FarThreshold)
        else:
            icpTransform.UseFarThresholdOff()

        transformFilter = vtk.vtkTransformPolyDataFilter()
        transformFilter.SetInputData(self.Surface)
        transformFilter.SetTransform(icpTransform)
        transformFilter.Update()

        self.PrintLog('Mean distance: '+str(icpTransform.GetMeanDistance()))

        self.Surface = transformFilter.GetOutput()
        self.Matrix4x4 = icpTransform.GetMatrix()

        matrix = self.Matrix4x4
        self.MatrixCoefficients = [
            matrix.GetElement(0,0), matrix.GetElement(0,1), matrix.GetElement(0,2), matrix.GetElement(0,3),
            matrix.GetElement(1,0), matrix.GetElement(1,1), matrix.GetElement(1,2), matrix.GetElement(1,3),
            matrix.GetElement(2,0), matrix.GetElement(2,1), matrix.GetElement(2,2), matrix.GetElement(2,3),
            matrix.GetElement(3,0), matrix.GetElement(3,1), matrix.GetElement(3,2), matrix.GetElement(3,3)]
        self.PrintLog('Transform matrix:\n  %f %f %f %f\n  %f %f %f %f\n  %f %f %f %f\n  %f %f %f %f' % tuple(self.MatrixCoefficients))

        if (self.DistanceArrayName != '') | (self.SignedDistanceArrayName != ''):
            self.PrintLog('Computing distance.')
            surfaceDistance = vtkvmtk.vtkvmtkSurfaceDistance()
            surfaceDistance.SetInputData(self.Surface)
            surfaceDistance.SetReferenceSurface(self.ReferenceSurface)
            if (self.DistanceArrayName != ''):
                surfaceDistance.SetDistanceArrayName(self.DistanceArrayName)
            if (self.SignedDistanceArrayName != ''):
                surfaceDistance.SetSignedDistanceArrayName(self.SignedDistanceArrayName)
            surfaceDistance.Update()
            self.Surface = surfaceDistance.GetOutput()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
