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
            ['ArrayDict','o','dict',1,'','the output dictionary','vmtknumpywriter']])

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: no input image.')

        self.PrintLog('wrapping vtkDataObject')
        imWrapped = dsa.WrapDataObject(self.Image)

        self.PrintLog('setting origin')
        self.PrintLog('setting dimensions')
        self.PrintLog('setting spacing')

        self.ArrayDict['Origin'] = np.array(self.Image.GetOrigin())
        self.ArrayDict['Dimensions'] = np.array(self.Image.GetDimensions())
        self.ArrayDict['Spacing'] = np.array(self.Image.GetSpacing())

        self.PrintLog('writing point data: ')
        for pointKey in imWrapped.PointData.keys():
            flatPointArray = np.array(imWrapped.PointData.GetArray(pointKey))
            pointArray = np.empty(shape=self.ArrayDict['Dimensions'], dtype=flatPointArray.dtype)

            # this is an efficient ndarray iterator method. the loop "for x in it" pulls an element out of
            # the cellArray iterator (it) and with the elipses syntax (x[...]) writes the point id.
            # this is equivalent to writing
            # pointData = imageData.GetPointData()
            # pointDataKeys = pointData.keys()
            #
            # self.PrintLog('Converting Image to Numpy Array')
            # for index, key in enumerate(pointDataKeys):
            #     flatArray = np.array(pointData.GetArray(index))
            #     reshapedArray = np.zeros(shape=self.ArrayDict['Dimensions'])
            #
            #     for xId in range(self.ArrayDict['Dimensions'][0]):
            #         for yId in range(self.ArrayDict['Dimensions'][1]):
            #             for zId in range(self.ArrayDict['Dimensions'][2]):
            #                 pointId = self.Image.ComputePointId((xId, yId, zId))
            #                 reshapedArray[xId, yId, zId] = flatArray[pointId]
            #
            #     self.ArrayDict['PointData'][key] = reshapedArray.astype(flatArray.dtype)

            it = np.nditer(pointArray, flags=['multi_index'], op_flags=['readwrite'])
            for x in it:
                x[...] = flatPointArray[self.Image.ComputePointId(it.multi_index)]

            self.ArrayDict['PointData'][pointKey] = pointArray

if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()