#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkcenterlinemeshsections.py,v $
## Language:  Python
## Date:      $Date: 2006/10/17 15:16:16 $
## Version:   $Revision: 1.1 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.


import vtk
import vtkvmtk
import sys

import pypes

vmtkcenterlinemeshsections = 'vmtkCenterlineMeshSections'

class vmtkCenterlineMeshSections(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
       
        self.Mesh = None
        self.Centerlines = None
        self.CenterlineSections = None
        self.TransformSections = False
        self.UpNormalsArrayName = ''
        self.AdditionalNormalsArrayName = None
        self.AdditionalScalarsArrayName = None
        self.OriginOffset = [0.0, 0.0, 0.0]
        self.VectorsArrayName = ''

        self.SetScriptName('vmtkcenterlinemeshsections')
        self.SetScriptDoc('extract mesh sections along centerlines. The script takes in input the mesh and the relative centerlines.')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['Centerlines','centerlines','vtkPolyData',1,'','the input centerlines','vmtksurfacereader'],
            ['TransformSections','transformsections','bool',1,'','transform sections so that they are at the origin, with normal 0,0,1 and upNormal 0,1,0'],
            ['UpNormalsArrayName','upnormalsarray','str',1,'','the name of the array where normals determining the "up" orientation of sections are stored'],
            ['AdditionalNormalsArrayName','additionalnormalsarray','str',1,'','the name of the array that contains normals that will be transformed and assigned to additional data points'],
            ['AdditionalScalarsArrayName','additionalscalarsarray','str',1,'','the name of the array that contains scalars that will be assigned to additional data points'],
            ['VectorsArrayName','vectorsarray','str',1,'','the name of the array where vectors, e.g. velocity vectors, are stored'],
            ['OriginOffset','originoffset','float',3,'','offset of subsequent sections after transformation']
            ])
        self.SetOutputMembers([
            ['CenterlineSections','o','vtkPolyData',1,'','the output sections','vmtksurfacewriter'],
            ['AdditionalNormalsPolyData','additionalnormalsdata','vtkPolyData',1,'','the output additional normals poly data','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: No input mesh.')

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        centerlineSections = vtkvmtk.vtkvmtkUnstructuredGridCenterlineSections()
        centerlineSections.SetInput(self.Mesh)
        centerlineSections.SetCenterlines(self.Centerlines)
        centerlineSections.SetTransformSections(self.TransformSections)
        centerlineSections.SetOriginOffset(self.OriginOffset)
        if self.VectorsArrayName:
            centerlineSections.SetVectorsArrayName(self.VectorsArrayName)
        if self.UpNormalsArrayName:
            centerlineSections.SetUpNormalsArrayName(self.UpNormalsArrayName)
        if self.AdditionalNormalsArrayName:
            centerlineSections.SetAdditionalNormalsArrayName(self.AdditionalNormalsArrayName)
        if self.AdditionalScalarsArrayName:
            centerlineSections.SetAdditionalScalarsArrayName(self.AdditionalScalarsArrayName)
        centerlineSections.Update()

        self.CenterlineSections = centerlineSections.GetOutput()

        self.AdditionalNormalsPolyData = centerlineSections.GetAdditionalNormalsPolyData()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
