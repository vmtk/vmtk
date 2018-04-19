#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmedialcurvecenterline.py,v $
## Language:  Python
## Date:      $Date: 2018/04/18 09:52:56 $
## Version:   $Revision: 1.40 $

##   Copyright (c) Richard Izzo, Luca Antiga. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
from vtk.numpy_interface import dataset_adapter as dsa
import numpy as np
import math
import sys

from vmtk import vtkvmtk
from vmtk import vmtkrenderer
from vmtk import vmtksurfacetobinaryimage, vmtksurfacecapper
from vmtk import pypes

class vmtkMedialCurveCenterline(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Surface = None
        self.BinaryImage = None
        self.SkeletonImage = None

        self.Sigma = 0.5
        self.Threshold = 0.0
        self.PolyDataToImageDataSpacing = [0.3, 0.3, 0.3]

        self.SetScriptName('vmtkmedialcurvecenterline')
        self.SetScriptDoc('Automatically extract a centerline from a surface using a medial curve flow algorithm.')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['Sigma','sigma','float',1,'(0.0,)','the kernal width of the gaussian used for image smoothing'],
            ['Threshold','threshold','float',1,'(0.0,1.0)','the threshold to filter the output skeleton image at'],
            ['PolyDataToImageDataSpacing','spacing','float',3,'(0.0,)','the sample spacing for the polydata to image data conversion']
            ])
        self.SetOutputMembers([
            ['BinaryImage','o2','vtkImageData',1,'','the binary surface image','vmtkimagewriter'],
            ['SkeletonImage','o','vtkImageData',1,'','the output centerline image','vmtkimagewriter']
            ])

    def Execute(self):
        if self.Surface == None:
            self.PrintError('Error: No Input Surface.')

        # Step 0: Check if the surface has any unfilled holes in it. if it does, cap them. 
        fedges = vtk.vtkFeatureEdges()
        fedges.BoundaryEdgesOn()
        fedges.FeatureEdgesOff()
        fedges.ManifoldEdgesOff()
        fedges.SetInputData(self.Surface)
        fedges.Update()

        ofedges = fedges.GetOutput()
        # if the following is not == 0, then the surface contains unfilled holes. 
        numEdges = ofedges.GetNumberOfPoints()
        if numEdges >= 1:
            self.PrintLog('Capping unclosed holes in surface.')
            tempcapper = vmtksurfacecapper.vmtkSurfaceCapper()
            tempcapper.Surface = self.Surface
            tempcapper.Interactive = 0
            tempcapper.Execute()
            self.Surface = tempcapper.Surface

        # Step 1: Convert the input surface into an image mask of unsigned char type and spacing = PolyDataToImageDataSpacing
        #         Where voxels lying inside the surface are set to 255 and voxels outside the image are set to value 0. 

        # since we are creating a new image container from nothing, calculate the origin, extent, and dimensions for the
        # vtkImageDataObject from the surface parameters.

        self.PrintLog('Converting Surface to Image Mask')
        binaryImageFilter = vmtksurfacetobinaryimage.vmtkSurfaceToBinaryImage()
        binaryImageFilter.Surface = self.Surface
        binaryImageFilter.InsideValue = 255
        binaryImageFilter.OutsideValue = 0
        binaryImageFilter.PolyDataToImageDataSpacing = self.PolyDataToImageDataSpacing
        binaryImageFilter.Execute()

        self.BinaryImage = binaryImageFilter.Image

        # Step 2: Feed into the vtkvmtkMedialCurveFilter
        #         This takes the binary image and computes the average outward flux of the image. This is then
        #         used to compute the skeleton image. It returns a binary image where values of 1 are skeleton points
        #         and values of 0 are outside the skeleton. The execution speed of this algorithm is fairly sensetive to
        #         the extent of the input image.
        self.PrintLog('Extracting Centerline Skeleton from Image Mask')
        medialCurveFilter = vtkvmtk.vtkvmtkMedialCurveFilter()
        medialCurveFilter.SetInputData(self.BinaryImage)
        medialCurveFilter.SetThreshold(self.Threshold)
        medialCurveFilter.SetSigma(self.Sigma)
        medialCurveFilter.Update()

        self.SkeletonImage = medialCurveFilter.GetOutput()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()