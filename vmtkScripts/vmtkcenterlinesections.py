#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkcenterlinesections.py,v $
## Language:  Python
## Date:      $Date: 2006/10/17 15:16:16 $
## Version:   $Revision: 1.1 $

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


class vmtkCenterlineSections(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Centerlines = None
        self.CenterlineSections = None

        self.CenterlineSectionAreaArrayName = 'CenterlineSectionArea'
        self.CenterlineSectionMinSizeArrayName = 'CenterlineSectionMinSize'
        self.CenterlineSectionMaxSizeArrayName = 'CenterlineSectionMaxSize'
        self.CenterlineSectionShapeArrayName = 'CenterlineSectionShape'
        self.CenterlineSectionClosedArrayName = 'CenterlineSectionClosed'

        self.SetScriptName('vmtkcenterlinesections')
        self.SetScriptDoc('compute geometric properties of sections located along centerlines. The script takes in input the surface and the relative centerlines.')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['Centerlines','centerlines','vtkPolyData',1,'','the input centerlines','vmtksurfacereader'],
            ['CenterlineSectionAreaArrayName','branchsectionarea','str',1,'','name of the array where the area of bifurcation sections have to be stored'],
            ['CenterlineSectionMinSizeArrayName','branchsectionminsize','str',1,'','name of the array where the minimum diameter of each section has to be stored'],
            ['CenterlineSectionMaxSizeArrayName','branchsectionmaxsize','str',1,'','name of the array where the maximum diameter of each bifurcation sections has to be stored'],
            ['CenterlineSectionShapeArrayName','centerlinesectionshape','str',1,'','name of the array where the shape index, i.e. the ratio between minimum and maximum diameter, of each bifurcation section has to be stored'],
            ['CenterlineSectionClosedArrayName','branchsectionclosed','str',1,'','name of the array containing 1 if a section is closed and 0 otherwise']
            ])
        self.SetOutputMembers([
            ['CenterlineSections','o','vtkPolyData',1,'','the output sections','vmtksurfacewriter'],
            ['Centerlines','ocenterlines','vtkPolyData',1,'','the output centerlines','vmtksurfacewriter'],
            ['CenterlineSectionAreaArrayName','branchsectionarea','str',1,'','name of the array where the area of bifurcation sections are stored'],
            ['CenterlineSectionMinSizeArrayName','branchsectionminsize','str',1,'','name of the array where the minimum diameter of each section are stored'],
            ['CenterlineSectionMaxSizeArrayName','branchsectionmaxsize','str',1,'','name of the array where the minimum diameter of each bifurcation sections has to be stored'],
            ['CenterlineSectionShapeArrayName','centerlinesectionshape','str',1,'','name of the array where the shape index, i.e. the ratio between minimum and maximum diameter, of each bifurcation section are stored'],
            ['CenterlineSectionClosedArrayName','branchsectionclosed','str',1,'','name of the array containing 1 if a section is closed and 0 otherwise']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        centerlineSections = vtkvmtk.vtkvmtkPolyDataCenterlineSections()
        centerlineSections.SetInputData(self.Surface)
        centerlineSections.SetCenterlines(self.Centerlines)
        centerlineSections.SetCenterlineSectionAreaArrayName(self.CenterlineSectionAreaArrayName)
        centerlineSections.SetCenterlineSectionMinSizeArrayName(self.CenterlineSectionMinSizeArrayName)
        centerlineSections.SetCenterlineSectionMaxSizeArrayName(self.CenterlineSectionMaxSizeArrayName)
        centerlineSections.SetCenterlineSectionShapeArrayName(self.CenterlineSectionShapeArrayName)
        centerlineSections.SetCenterlineSectionClosedArrayName(self.CenterlineSectionClosedArrayName)
        centerlineSections.Update()

        self.CenterlineSections = centerlineSections.GetOutput()
        self.Centerlines = centerlineSections.GetCenterlines()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
