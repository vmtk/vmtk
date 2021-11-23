#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkcenterlinemeshsections.py,v $
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


class vmtkCenterlineMeshSections(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.Centerlines = None
        self.SectionSource = None
        self.CenterlineSections = None
        self.TransformSections = False
        self.UseSectionSource = False
        self.SourceScaling = False
        self.SectionIdsArrayName = 'SectionIds'
        self.SectionNormalsArrayName = 'SectionNormals'
        self.SectionUpNormalsArrayName = ''
        self.AdditionalNormalsArrayName = None
        self.AdditionalScalarsArrayName = None
        self.OriginOffset = [0.0, 0.0, 0.0]
        self.VectorsArrayName = ''

        self.SetScriptName('vmtkcenterlinemeshsections')
        self.SetScriptDoc('extract mesh sections along centerlines. The script takes in input the mesh and the relative centerlines.')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['Centerlines','centerlines','vtkPolyData',1,'','the input centerlines','vmtksurfacereader'],
            ['SectionSource','source','vtkPolyData',1,'','the input section source with which to probe the mesh (optional)','vmtksurfacereader'],
            ['UseSectionSource','usesource','bool',1,'','if off, slice mesh with plane to generate sections; if on, use the SectionSource to probe the mesh'],
            ['SourceScaling','sourcescaling','bool',1,'','toggle scaling the source with the local radius'],
            ['TransformSections','transformsections','bool',1,'','transform sections so that they are at the origin, with normal 0,0,1 and upNormal 0,1,0'],
            ['SectionIdsArrayName','sectionidsarray','str',1,'','the name of the array where the ids identifying sections are stored'],
            ['SectionNormalsArrayName','normalsarray','str',1,'','the name of the array where normals determining the section planes are stored'],
            ['SectionUpNormalsArrayName','upnormalsarray','str',1,'','the name of the array where normals determining the "up" orientation of sections are stored'],
            ['AdditionalNormalsArrayName','additionalnormalsarray','str',1,'','the name of the array that contains normals that will be transformed and assigned to additional data points'],
            ['AdditionalScalarsArrayName','additionalscalarsarray','str',1,'','the name of the array that contains scalars that will be assigned to additional data points'],
            ['VectorsArrayName','vectorsarray','str',1,'','the name of the array where vectors, e.g. velocity vectors, are stored'],
            ['OriginOffset','originoffset','float',3,'','offset of subsequent sections after transformation']
            ])
        self.SetOutputMembers([
            ['CenterlineSections','o','vtkPolyData',1,'','the output sections','vmtksurfacewriter'],
            ['SectionPointsPolyData','sectionpoints','vtkPolyData',1,'','the additional output poly data storing information about the location and orientation of sections','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: No input mesh.')

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        centerlineSections = vtkvmtk.vtkvmtkUnstructuredGridCenterlineSections()
        centerlineSections.SetInputData(self.Mesh)
        centerlineSections.SetCenterlines(self.Centerlines)
        centerlineSections.SetSectionSource(self.SectionSource)
        centerlineSections.SetUseSectionSource(self.UseSectionSource)
        centerlineSections.SetSourceScaling(self.SourceScaling)
        centerlineSections.SetTransformSections(self.TransformSections)
        centerlineSections.SetOriginOffset(self.OriginOffset)
        if self.VectorsArrayName:
            centerlineSections.SetVectorsArrayName(self.VectorsArrayName)
        centerlineSections.SetSectionIdsArrayName(self.SectionIdsArrayName)
        centerlineSections.SetSectionNormalsArrayName(self.SectionNormalsArrayName)
        if self.SectionUpNormalsArrayName:
            centerlineSections.SetSectionUpNormalsArrayName(self.SectionUpNormalsArrayName)
        if self.AdditionalNormalsArrayName:
            centerlineSections.SetAdditionalNormalsArrayName(self.AdditionalNormalsArrayName)
        if self.AdditionalScalarsArrayName:
            centerlineSections.SetAdditionalScalarsArrayName(self.AdditionalScalarsArrayName)
        centerlineSections.Update()

        self.CenterlineSections = centerlineSections.GetOutput()

        self.SectionPointsPolyData = centerlineSections.GetSectionPointsPolyData()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
