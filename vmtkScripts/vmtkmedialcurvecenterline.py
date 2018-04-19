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
from vmtk import pypes

class vmtkMedialCurveCenterline(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Surface = None
        self.BinaryImage = None

        self.Sigma = 0.5
        self.Threshold = 0.0
        self.PolyDataToImageDataSpacing = [0.5, 0.5, 0.5]

        self.OutputImage = None

        self.SetScriptName('vmtkmedialcurvecenterline')
        self.SetScriptDoc('Automatically extract a centerline from a surface using a medial curve flow algorithm.')
        self.SetInputMembers([
            ['Surface','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['Sigma','sigma','float',1,'(0.0,)','the kernal width of the gaussian used for image smoothing'],
            ['Threshold','threshold','float',1,'(0.0,)','the threshold to filter the image at'],
            ['PolyDataToImageDataSpacing','spacing','list',1,'[0.5, 0.5, 0.5]','the sample spacing for the polydata to image data conversion']
            ])
        self.SetOutputMembers([
            ['BinaryImage','o2','vtkImageData',1,'','the binary surface image','vmtkimagewriter'],
            ['OutputImage','o','vtkImageData',1,'','the output centerline image','vmtkimagewriter']
            ])

    def Execute(self):
        if self.Surface == None:
            self.PrintError('Error: No Input Surface.')

        bounds = self.Surface.GetBounds()
        dim = []
        for i in range(3):
            dim.append(math.ceil((bounds[i * 2 + 1] - bounds[i * 2]) / self.PolyDataToImageDataSpacing[i]))
            
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

        inval = 255
        outval = 0

        npFillImagePoints = np.zeros(whiteImage.GetNumberOfPoints(), dtype=np.uint8)
        npFillImagePoints[:] = 255

        pointDataArray = dsa.numpyTovtkDataArray(npFillImagePoints, name='ImageScalars', array_type=vtk.VTK_UNSIGNED_CHAR)

        whiteImage.GetPointData().SetActiveScalars('ImageScalars')
        whiteImage.GetPointData().SetScalars(pointDataArray)

        polyDataToImageDataFilter = vtk.vtkPolyDataToImageStencil()
        polyDataToImageDataFilter.SetInputData(self.Surface)
        polyDataToImageDataFilter.SetOutputSpacing(self.PolyDataToImageDataSpacing[0], 
                                                   self.PolyDataToImageDataSpacing[1], 
                                                   self.PolyDataToImageDataSpacing[2])
        polyDataToImageDataFilter.SetOutputOrigin(origin[0], origin[1], origin[2])
        polyDataToImageDataFilter.Update()

        imageStencil = vtk.vtkImageStencil()
        imageStencil.SetInputData(whiteImage)
        imageStencil.SetStencilConnection(polyDataToImageDataFilter.GetOutputPort())
        imageStencil.ReverseStencilOff()
        imageStencil.SetBackgroundValue(outval)
        imageStencil.Update()

        self.BinaryImage = imageStencil.GetOutput()



if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()