#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimagetonumpy.py,v $
## Language:  Python
## Date:      June 10, 2017
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo
##       The Jacobs Institute

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
from vtk.numpy_interface import dataset_adapter as dsa
import sys

from vmtk import vtkvmtk
from vmtk import vmtkrenderer
from vmtk import pypes

try:
    import numpy as np
except ImportError:
    raise ImportError('Unable to Import vmtkimagetonumpy module, numpy is not installed')


# convenience class for nested dictionaries
class vividict(dict):
    def __missing__(self, key):
        value = self[key] = type(self)()
        return value

class vmtkImageToNumpy(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None

        self.ArrayDict = vividict()

        self.SetScriptName('vmtkImageToNumpy')
        self.SetScriptDoc('Takes a vtkImageData object and returns a numpy')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader']])
        self.SetOutputMembers([
            ['ArrayDict','o','dict',1]])

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: no input image.')

        self.ArrayDict['Origin'] = self.Image.GetOrigin()
        self.ArrayDict['Dimensions'] = self.Image.GetDimensions()
        self.ArrayDict['Spacing'] = self.Image.GetSpacing()

        imageData = dsa.WrapDataObject(self.Image)
        pointData = imageData.GetPointData()
        pointDataKeys = pointData.keys()

        self.PrintLog('Converting Image to Numpy Array')
        for index, key in enumerate(pointDataKeys):
            flatArray = np.array(pointData.GetArray(index))
            reshapedArray = np.zeros(shape=self.ArrayDict['Dimensions'])

            for xId in range(self.ArrayDict['Dimensions'][0]):
                for yId in range(self.ArrayDict['Dimensions'][1]):
                    for zId in range(self.ArrayDict['Dimensions'][2]):
                        pointId = self.Image.ComputePointId((xId, yId, zId))
                        reshapedArray[xId, yId, zId] = flatArray[pointId]

            self.ArrayDict['PointData'][key] = reshapedArray.astype(flatArray.dtype)

if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()