#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimagereslice.py,v $
## Language:  Python
## Date:      $Date: 2006/07/17 09:53:14 $
## Version:   $Revision: 1.8 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was improved by
##       Hugo Gratama van Andel
##       Academic Medical Centre - University of Amsterdam
##       Dept. Biomedical Engineering  & Physics

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys
import math
import numpy as np

from vmtk import pypes


class vmtkImageReslice(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None
        self.ReferenceImage = None

        self.OutputSpacing = []
        self.OutputOrigin = []
        self.OutputExtent = []

        self.Interpolation = 'linear'
        self.Cast = 1

        self.BackgroundLevel = 0.0

        self.MatrixCoefficients = []
        self.InvertMatrix = 0
        self.Matrix4x4 = None
        self.Rotation = [0.0,0.0,0.0]
        self.Translation = [0.0,0.0,0.0]
        self.Scaling = [1.0,1.0,1.0]
        self.NewZDirection = [0.0,0.0,1.0]
        self.NewZDirectionInteractive = 0
        self.ImageExpansion = 0.0

        self.TransformInputSampling = 1

        self.SetScriptName('vmtkimagereslice')
        self.SetScriptDoc('reslice an image based on user-specified parameters or on a reference image')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['ReferenceImage','r','vtkImageData',1,'','the reference image','vmtkimagereader'],
            ['OutputSpacing','spacing','float',3,'','the output spacing'],
            ['OutputOrigin','origin','float',3,'','the output origin'],
            ['OutputExtent','extent','int',6,'','the output extent'],
            ['Interpolation','interpolation','str',1,'["nearestneighbor","linear","cubic"]','interpolation during reslice'],
            ['Cast','cast','bool',1,'','toggle cast image to float type'],
            ['BackgroundLevel','background','float',1,'','the output image background'],
            ['Matrix4x4','matrix4x4','vtkMatrix4x4',1,'','the input transform matrix'],
            ['MatrixCoefficients','matrix','float',16,'','coefficients of transform matrix'],
            ['InvertMatrix','invert','bool',1,'','invert matrix before applying transformation'],
            ['Rotation','rotation','float',3,'','rotations around the x-,y- and z-axis'],
            ['Translation','translation','float',3,'','translation in the x-,y- and z-directions'],
            ['Scaling','scaling','float',3,'','scaling of the x-,y- and z-directions'],
            ['NewZDirection','zdirection','float',3,'','direction of the new z-axis after rotation (alternative to rotation/translation/scaling)'],
            ['NewZDirectionInteractive','zdirectioninteractive','bool',1,'','Interactive selecting two points to fix the new z-axis direction or three points to fix its orthogonal plane (alternative to rotation/translation/scaling)'],
            ['ImageExpansion','imageexpansion','float',1,'(0.0,)','expansion (in mm) of the output image bounds compared to the input image'],
            ['TransformInputSampling','transforminputsampling','bool',1,'','transform spacing, origin and extent of the Input (or the InformationInput) according to the direction cosines and origin of the ResliceAxes before applying them as the default output spacing, origin and extent']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter'],
            ['Matrix4x4','matrix4x4','vtkMatrix4x4',1,'','the output transform matrix'],
            ['MatrixCoefficients','matrix','float',16,'','coefficients of transform matrix']
            ])

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: No input image.')

        if self.Cast:
            cast = vtk.vtkImageCast()
            cast.SetInputData(self.Image)
            cast.SetOutputScalarTypeToFloat()
            cast.Update()
            self.Image = cast.GetOutput()

        if self.ImageExpansion != 0.0:
            self.PrintLog('Expanding input image before transforming ...')

            bounds = np.array( self.Image.GetBounds() )
            bounds[0] = bounds[0] - self.ImageExpansion
            bounds[1] = bounds[1] + self.ImageExpansion
            bounds[2] = bounds[2] - self.ImageExpansion
            bounds[3] = bounds[3] + self.ImageExpansion
            bounds[4] = bounds[4] - self.ImageExpansion
            bounds[5] = bounds[5] + self.ImageExpansion

            newImage =  vtk.vtkImageData()
            newImage.SetOrigin(bounds[0],bounds[2],bounds[4])
            spacing = self.Image.GetSpacing()
            newImage.SetSpacing(spacing)
            newImage.SetExtent(
                0, int(math.ceil((bounds[1]-bounds[0])/spacing[0])),
                0, int(math.ceil((bounds[3]-bounds[2])/spacing[1])),
                0, int(math.ceil((bounds[5]-bounds[4])/spacing[2]))
            )
            newImage.AllocateScalars( vtk.VTK_FLOAT, 1 )

            scalars = self.Image.GetPointData().GetScalars()
            dims = newImage.GetDimensions()
            imageVoxelExpansion = [math.ceil(self.ImageExpansion/spacing[0]),math.ceil(self.ImageExpansion/spacing[1]),math.ceil(self.ImageExpansion/spacing[2])]
            for z in range(dims[2]):
                for y in range(dims[1]):
                    for x in range(dims[0]):
                        if x<imageVoxelExpansion[0] or x>=dims[0]-imageVoxelExpansion[0] or y<imageVoxelExpansion[1] or y>=dims[1]-imageVoxelExpansion[1] or  z<imageVoxelExpansion[2] or z>=dims[2]-imageVoxelExpansion[2]:
                            newImage.SetScalarComponentFromFloat(x,y,z,0,self.BackgroundLevel)
                        else:
                            value = self.Image.GetScalarComponentAsFloat(x-imageVoxelExpansion[0],y-imageVoxelExpansion[1],z-imageVoxelExpansion[2],0)
                            newImage.SetScalarComponentFromFloat(x,y,z,0,value)
            self.Image = newImage

        resliceFilter = vtk.vtkImageReslice()
        resliceFilter.SetInputData(self.Image)
        if self.ReferenceImage:
            resliceFilter.SetInformationInput(self.ReferenceImage)
        else:
            if self.OutputSpacing:
                resliceFilter.SetOutputSpacing(self.OutputSpacing)
            if self.OutputOrigin:
                resliceFilter.SetOutputOrigin(self.OutputOrigin)
            if self.OutputExtent:
                resliceFilter.SetOutputExtent(self.OutputExtent)
        if self.Interpolation == 'nearestneighbor':
            resliceFilter.SetInterpolationModeToNearestNeighbor()
        elif self.Interpolation == 'linear':
            resliceFilter.SetInterpolationModeToLinear()
        elif self.Interpolation == 'cubic':
            resliceFilter.SetInterpolationModeToCubic()
        else:
            self.PrintError('Error: unsupported interpolation mode')
        resliceFilter.SetBackgroundLevel(self.BackgroundLevel)

        if self.TransformInputSampling:
            resliceFilter.TransformInputSamplingOn()
        else:
            resliceFilter.TransformInputSamplingOff()

        if not self.Matrix4x4:

            if self.MatrixCoefficients != []:
                self.PrintLog('Setting up transform matrix using specified coefficients')
                self.Matrix4x4 = vtk.vtkMatrix4x4()
                self.Matrix4x4.DeepCopy(self.MatrixCoefficients)

            elif self.Translation != [0.0,0.0,0.0] or self.Rotation != [0.0,0.0,0.0] or self.Scaling != [1.0,1.0,1.0]:
                self.PrintLog('Setting up transform matrix using specified translation, rotation and/or scaling')
                transform = vtk.vtkTransform()
                transform.RotateX(self.Rotation[0])
                transform.RotateY(self.Rotation[1])
                transform.RotateZ(self.Rotation[2])
                transform.Translate(self.Translation[0], self.Translation[1], self.Translation[2])
                transform.Scale(self.Scaling[0], self.Scaling[1], self.Scaling[2])
                self.Matrix4x4 = vtk.vtkMatrix4x4()
                self.Matrix4x4.DeepCopy(transform.GetMatrix())

            elif self.NewZDirection != [0.0,0.0,1.0] or self.NewZDirectionInteractive == 1:

                if self.NewZDirectionInteractive == 1:
                    self.PrintLog('Interactive selecting points to fix the new z-axis direction')
                    from vmtk import vmtkscripts
                    imageSeeder = vmtkscripts.vmtkImageSeeder()
                    imageSeeder.Image = self.Image
                    imageSeeder.Execute()
                    numSeeds = imageSeeder.Seeds.GetNumberOfPoints()
                    if numSeeds < 2:
                        self.PrintError('Error: selected less than two points')
                    point1 = imageSeeder.Seeds.GetPoint(0)
                    point2 = imageSeeder.Seeds.GetPoint(1)
                    if numSeeds > 2:
                        point3 = imageSeeder.Seeds.GetPoint(2)
                        vectorA = [point2[0]-point1[0],point2[1]-point1[1],point2[2]-point1[2]]
                        vectorB = [point3[0]-point1[0],point3[1]-point1[1],point3[2]-point1[2]]
                        self.NewZDirection = np.cross(vectorA,vectorB)
                    else:
                        self.NewZDirection = [point2[0]-point1[0],point2[1]-point1[1],point2[2]-point1[2]]

                self.PrintLog('Setting up transform matrix using the prescribed direction of the new z-axis')
                norm2 = np.linalg.norm(self.NewZDirection)
                newZVersor = [self.NewZDirection[0]/norm2,self.NewZDirection[1]/norm2,self.NewZDirection[2]/norm2]
                rotationAxis = [newZVersor[1],-newZVersor[0],0.0]
                theta = math.degrees( math.acos( np.dot([0.0,0.0,1.0],newZVersor) ) )
                bounds = self.Image.GetBounds()
                offset = [
                    bounds[0]+(bounds[1]-bounds[0])/2.0,
                    bounds[2]+(bounds[3]-bounds[2])/2.0,
                    bounds[4]+(bounds[5]-bounds[4])/2.0
                ]
                print ("new z-axis versor = ",newZVersor)
                print ("rotation axis = ",rotationAxis)
                print ("theta = ",theta)
                transform = vtk.vtkTransform()
                transform.Translate(offset)
                transform.RotateWXYZ(theta,rotationAxis)
                transform.Translate(-offset[0],-offset[1],-offset[2])
                self.Matrix4x4 = vtk.vtkMatrix4x4()
                self.Matrix4x4.DeepCopy(transform.GetMatrix())

        if self.InvertMatrix and self.Matrix4x4:
            self.Matrix4x4.Invert()

        # just to print them at the end of the script
        if self.Matrix4x4:
            transform = vtk.vtkMatrixToLinearTransform()
            transform.SetInput(self.Matrix4x4)
            resliceFilter.SetResliceTransform(transform)
            for i in range(4):
                for j in range(4):
                    self.MatrixCoefficients.append(self.Matrix4x4.GetElement(i,j))

        resliceFilter.Update()

        self.Image = resliceFilter.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
