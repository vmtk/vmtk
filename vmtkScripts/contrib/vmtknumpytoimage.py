#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtknumpytoimage.py,v $
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

from __future__ import absolute_import  # NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
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


class vmtkNumpyToImage(pypes.pypeScript):
    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.ArrayDict = None

        self.SetScriptName('vmtkNumpyToImage')
        self.SetScriptDoc('Takes a dictionary of numpy arrays and returns a vtkImageData object.')
        self.SetInputMembers([
            ['ArrayDict','i','dict',1,'','the input array dictionary','vmtknumpyreader']])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter']])

    def Execute(self):

        self.PrintLog('Converting Numpy Array to vtkImageData')
        self.Image = vtk.vtkImageData()
        self.Image.SetDimensions(self.ArrayDict['Dimensions'])
        self.Image.SetOrigin(self.ArrayDict['Origin'])
        self.Image.SetSpacing(self.ArrayDict['Spacing'])
        self.Image.SetExtent((0, self.ArrayDict['Dimensions'][0] - 1,
                                0, self.ArrayDict['Dimensions'][1] - 1,
                                0, self.ArrayDict['Dimensions'][2] - 1,))


        self.PrintLog('converting point data')
        for pointKey in self.ArrayDict['PointData'].keys():
            if np.issubdtype(self.ArrayDict['PointData'][pointKey].dtype, float):
                pointDataArray = vtk.vtkFloatArray()
            else:
                for checkDt in [int, np.uint8, np.uint16, np.uint32, np.uint64]:
                    if np.issubdtype(self.ArrayDict['PointData'][pointKey].dtype, checkDt):
                        pointDataArray = vtk.vtkIntArray()
                        break
                    else:
                        continue

            flatArray = self.ArrayDict['PointData'][pointKey].ravel(order='F')

            pointDataArray.SetNumberOfComponents(1)
            pointDataArray.SetName(pointKey)
            pointDataArray.SetNumberOfValues(flatArray.size)

            it = np.nditer(flatArray, flags=['multi_index'])
            for x in it:
                pointDataArray.SetValue(it.multi_index[0], x)
            self.Image.GetPointData().SetActiveScalars(pointKey)
            self.Image.GetPointData().SetScalars(pointDataArray)


if __name__ == '__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()