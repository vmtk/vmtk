#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacetobinaryimage.py,v $
## Language:  Python
## Date:      $Date: 2018/04/18 09:52:56 $
## Version:   $Revision: 1.40 $

##   Copyright (c) Richard Izzo, Luca Antiga. All rights reserved.
##   See LICENSE file for details.

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
from vmtk import pypes


class vmtkSurfaceToBinaryImage(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Image = None

        self.InsideValue = 1
        self.OutsideValue = 0
        self.PolyDataToImageDataSpacing = [0.3, 0.3, 0.3]

        self.SetScriptName('vmtksurfacetobinaryimage')
        self.SetScriptDoc('Convert a surface to a binary image mask where voxels are labeled if they lie inside the surface or not.')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['InsideValue','invalue','int',1,'(0,255)','the value to label voxels which contain points inside the surface'],
            ['OutsideValue','outvalue','int',1,'(0,255)','the value to label voxels which contain points inside the surface'],
            ['PolyDataToImageDataSpacing','imagespacing','float',3,'(0.0,)','the sample spacing for the polydata to image data conversion']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the binary surface image','vmtkimagewriter']
            ])

    def Execute(self):
        if self.Surface == None:
            self.PrintError('Error: No Input Surface.')

        if self.InsideValue > 255:
            self.PrintError('Error: Cannot assign InsideValue of image to value greater than 255')

        # Step 1: Convert the input surface into an image mask of unsigned char type and spacing = PolyDataToImageDataSpacing
        #         Where voxels lying inside the surface are set to 255 and voxels outside the image are set to value 0.

        # since we are creating a new image container from nothing, calculate the origin, extent, and dimensions for the
        # vtkImageDataObject from the surface parameters.
        bounds = self.Surface.GetBounds()
        dim = []   # list of size: 3, type: int
        for i in range(3):
            dim.append(int(math.ceil((bounds[i * 2 + 1] - bounds[i * 2]) / self.PolyDataToImageDataSpacing[i])))

        origin = [bounds[0] + self.PolyDataToImageDataSpacing[0] / 2,
                  bounds[2] + self.PolyDataToImageDataSpacing[1] / 2,
                  bounds[4] + self.PolyDataToImageDataSpacing[2] / 2]

        extent = [0, dim[0] - 1,
                  0, dim[1] - 1,
                  0, dim[2] - 1]

        whiteImage = vtk.vtkImageData()
        whiteImage.SetSpacing(self.PolyDataToImageDataSpacing[0],
                              self.PolyDataToImageDataSpacing[1],
                              self.PolyDataToImageDataSpacing[2])
        whiteImage.SetDimensions(dim[0], dim[1], dim[2])
        whiteImage.SetExtent(extent[0], extent[1],
                             extent[2], extent[3],
                             extent[4], extent[5])
        whiteImage.SetOrigin(origin[0], origin[1], origin[2])
        whiteImage.AllocateScalars(vtk.VTK_UNSIGNED_CHAR, 1)

        # initially set all values of the image to a value self.InsideValue
        npFillImagePoints = np.zeros(whiteImage.GetNumberOfPoints(), dtype=np.uint8)
        npFillImagePoints[:] = self.InsideValue
        # it is much faster to use the vtk data set adaptor functions to fill the point data tupples that it is to
        # loop over each index and set values individually.
        pointDataArray = dsa.numpyTovtkDataArray(npFillImagePoints, name='ImageScalars', array_type=vtk.VTK_UNSIGNED_CHAR)
        whiteImage.GetPointData().SetActiveScalars('ImageScalars')
        whiteImage.GetPointData().SetScalars(pointDataArray)

        # The vtkPolyDataToImageStencil class will convert polydata into an image stencil, masking an image.
        # The polydata can either be a closed surface mesh or a series of polyline contours (one contour per slice).
        polyDataToImageStencilFilter = vtk.vtkPolyDataToImageStencil()
        polyDataToImageStencilFilter.SetInputData(self.Surface)
        polyDataToImageStencilFilter.SetOutputSpacing(self.PolyDataToImageDataSpacing[0],
                                                      self.PolyDataToImageDataSpacing[1],
                                                      self.PolyDataToImageDataSpacing[2])
        polyDataToImageStencilFilter.SetOutputOrigin(origin[0], origin[1], origin[2])
        polyDataToImageStencilFilter.Update()

        # vtkImageStencil combines to images together by using a "cookie-cutter" operation.
        imageStencil = vtk.vtkImageStencil()
        imageStencil.SetInputData(whiteImage)
        imageStencil.SetStencilConnection(polyDataToImageStencilFilter.GetOutputPort())
        imageStencil.ReverseStencilOff()
        imageStencil.SetBackgroundValue(self.OutsideValue)
        imageStencil.Update()

        self.Image = imageStencil.GetOutput()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
