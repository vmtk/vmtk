#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmarchingcubes.py,v $
## Language:  Python
## Date:      $Date: 2006/02/06 09:49:42 $
## Version:   $Revision: 1.7 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes


class vmtkMarchingCubes(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None
        self.Surface = None
        self.ArrayName = ''
        self.Level = 0.0
        self.Connectivity = 0

        self.SetScriptName('vmtkmarchingcubes')
        self.SetScriptDoc('generate an isosurface of given level from a 3D image')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['ArrayName','array','str',1,'','name of the array to work with'],
            ['Level','l','float',1,'','graylevel to generate the isosurface at'],
            ['Connectivity','connectivity','bool',1,'','only output the largest connected region of the isosurface']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: No Image.')

        extent = self.Image.GetExtent()
        translateExtent = vtk.vtkImageTranslateExtent()
        translateExtent.SetInputData(self.Image)
        translateExtent.SetTranslation(-extent[0],-extent[2],-extent[4])
        translateExtent.Update()

        if (self.ArrayName != ''):
            translateExtent.GetOutput().GetPointData().SetActiveScalars(self.ArrayName)

        marchingCubes = vtk.vtkMarchingCubes()
        marchingCubes.SetInputConnection(translateExtent.GetOutputPort())
        marchingCubes.SetValue(0,self.Level)
        marchingCubes.Update()

        self.Surface = marchingCubes.GetOutput()

        if self.Connectivity == 1:
            connectivityFilter = vtk.vtkPolyDataConnectivityFilter()
            connectivityFilter.SetInputData(self.Surface)
            connectivityFilter.SetExtractionModeToLargestRegion()
            connectivityFilter.Update()
            self.Surface = connectivityFilter.GetOutput()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
