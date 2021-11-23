#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacescaling.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.4 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes


class vmtkSurfaceScaling(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None

        self.ScaleFactor = None
        self.ScaleFactorX = None
        self.ScaleFactorY = None
        self.ScaleFactorZ = None

        self.SetScriptName('vmtksurfacescaling')
        self.SetScriptDoc('scale a surface by an isotropic factor, or x,y,z directions by separate factors')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['ScaleFactor','scale','float',1,'(0.0,)','isotropic scaling factor'],
            ['ScaleFactorX','scalex','float',1,'(0.0,)','scaling factor in x direction'],
            ['ScaleFactorY','scaley','float',1,'(0.0,)','scaling factor in y direction'],
            ['ScaleFactorZ','scalez','float',1,'(0.0,)','scaling factor in z direction'],
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def Execute(self):

        if (self.Surface == None):
            self.PrintError('Error: no Surface.')

        transform = vtk.vtkTransform()
        s = self.ScaleFactor or 1.0
        x = self.ScaleFactorX or 1.0
        y = self.ScaleFactorY or 1.0
        z = self.ScaleFactorZ or 1.0
        transform.Scale(s*x, s*y, s*z)

        transformFilter = vtk.vtkTransformPolyDataFilter()
        transformFilter.SetInputData(self.Surface)
        transformFilter.SetTransform(transform)
        transformFilter.Update()

        self.Surface = transformFilter.GetOutput()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
