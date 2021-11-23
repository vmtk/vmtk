#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkbranchpatching.py,v $
## Language:  Python
## Date:      $Date: 2006/07/07 10:46:17 $
## Version:   $Revision: 1.9 $

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


class vmtkBranchPatching(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.PatchedData = None

        self.CircularPatching = 1
        self.UseConnectivity = 1

        self.LongitudinalPatchSize = 1.0
        self.CircularNumberOfPatches = 1

        self.PatchSize = [0.0, 0.0]

        self.GroupIdsArrayName = 'GroupIds'
        self.LongitudinalMappingArrayName = 'AbscissaMetric'
        self.CircularMappingArrayName = 'AngularMetric'

        self.LongitudinalPatchNumberArrayName = 'Slab'
        self.CircularPatchNumberArrayName = 'Sector'
        self.PatchAreaArrayName = 'PatchArea'

        self.SetScriptName('vmtkbranchpatching')
        self.SetScriptDoc('cut a set of contiguous rectangular regions on a surface that follow iso-contours in the StretchedMapping and AngularMetric arrays')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','','vmtksurfacereader'],
            ['PatchSize','patchsize','float',2,'(0.0,)'],
            ['LongitudinalPatchSize','longitudinalpatchsize','float',1,'(0.0,)'],
            ['CircularNumberOfPatches','circularpatches','int',1,'(0,)'],
            ['CircularPatching','circularpatching','bool',1],
            ['UseConnectivity','connectivity','bool',1],
            ['GroupIdsArrayName','groupidsarray','str',1],
            ['LongitudinalMappingArrayName','longitudinalmappingarray','str',1],
            ['CircularMappingArrayName','circularmappingarray','str',1],
            ['LongitudinalPatchNumberArrayName','longitudinalpatchnumberarray','str',1],
            ['CircularPatchNumberArrayName','circularpatchnumberarray','str',1],
            ['PatchAreaArrayName','patchareaarray','str',1]
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','','vmtksurfacewriter'],
            ['PatchedData','patcheddata','vtkImageData',1,'','','vmtkimagewriter'],
            ['PatchSize','patchsize','float',2],
            ['LongitudinalPatchNumberArrayName','longitudinalpatchnumberarray','str',1],
            ['CircularPatchNumberArrayName','circularpatchnumberarray','str',1],
            ['PatchAreaArrayName','patchareaarray','str',1]
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        self.PatchSize = [self.LongitudinalPatchSize, 1.0/float(self.CircularNumberOfPatches)]

        patchingFilter = vtkvmtk.vtkvmtkPolyDataPatchingFilter()
        patchingFilter.SetInputData(self.Surface)
        patchingFilter.SetCircularPatching(self.CircularPatching)
        patchingFilter.SetUseConnectivity(self.UseConnectivity)
        patchingFilter.SetLongitudinalMappingArrayName(self.LongitudinalMappingArrayName)
        patchingFilter.SetCircularMappingArrayName(self.CircularMappingArrayName)
        patchingFilter.SetLongitudinalPatchNumberArrayName(self.LongitudinalPatchNumberArrayName)
        patchingFilter.SetCircularPatchNumberArrayName(self.CircularPatchNumberArrayName)
        patchingFilter.SetPatchAreaArrayName(self.PatchAreaArrayName)
        patchingFilter.SetGroupIdsArrayName(self.GroupIdsArrayName)
        patchingFilter.SetPatchSize(self.PatchSize)
        patchingFilter.Update()

        self.Surface = patchingFilter.GetOutput()

        self.PatchedData = patchingFilter.GetPatchedData()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
