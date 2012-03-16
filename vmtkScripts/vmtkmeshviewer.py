#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshviewer.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.9 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

import vtk
import sys

import vmtkrenderer
import pypes

vmtkmeshviewer = 'vmtkMeshViewer'

class vmtkMeshViewer(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.Display = 1
        self.Opacity = 1.0
        self.ArrayName = ''
        self.ScalarRange = [0.0, 0.0]
        self.Legend = 0
        self.Grayscale = 0
        self.FlatInterpolation = 0
	
        self.Actor = None
        self.ScalarBarActor = None

        self.SetScriptName('vmtkmeshviewer')
        self.SetScriptDoc('display a mesh')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer'],
            ['Display','display','bool',1,'','toggle rendering'],
            ['Opacity','opacity','float',1,'(0.0,1.0)','object opacity in the scene'],
            ['ArrayName','array','str',1,'','name of the array where the scalars to be displayed are stored'],
            ['ScalarRange','scalarrange','float',2,'','range of the scalar map'],
            ['Legend','legend','bool',1,'','toggle scalar bar'],
            ['Grayscale','grayscale','bool',1,'','toggle color or grayscale'],
            ['FlatInterpolation','flat','bool',1,'','toggle flat or shaded surface display']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']
            ])

    def BuildView(self):

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self) 

        if self.Actor != None:
            self.vmtkRenderer.Renderer.RemoveActor(self.Actor)

        if self.ScalarBarActor != None:
            self.vmtkRenderer.Renderer.RemoveActor(self.ScalarBarActor)

        if self.Mesh != None:
            mapper = vtk.vtkDataSetMapper()
            mapper.SetInput(self.Mesh)
            if (self.ArrayName != ''):
                self.Mesh.GetPointData().SetActiveScalars(self.ArrayName)
            if (self.Mesh.GetPointData().GetScalars() != None):
                array = self.Mesh.GetPointData().GetScalars()
                if (self.ScalarRange[1] > self.ScalarRange[0]):
                    mapper.SetScalarRange(self.ScalarRange)
                else:
                    mapper.SetScalarRange(array.GetRange(0))
                if (self.Grayscale == 1):
                    lut = vtk.vtkLookupTable()
                    lut.SetValueRange(0.0,1.0)
                    lut.SetSaturationRange(0.0,0.0)
                    mapper.SetLookupTable(lut)
            self.Actor = vtk.vtkActor()
            self.Actor.SetMapper(mapper)
            if (self.FlatInterpolation == 1):
                self.Actor.GetProperty().SetInterpolationToFlat()
            self.Actor.GetProperty().SetOpacity(self.Opacity)
            self.vmtkRenderer.Renderer.AddActor(self.Actor)

        if (self.Legend == 1) & (self.Actor != None):
            self.ScalarBarActor = vtk.vtkScalarBarActor()
            self.ScalarBarActor.SetLookupTable(self.Actor.GetMapper().GetLookupTable())
            self.ScalarBarActor.GetLabelTextProperty().ItalicOff()
            self.ScalarBarActor.GetLabelTextProperty().BoldOff()
            self.ScalarBarActor.GetLabelTextProperty().ShadowOff()
##             self.ScalarBarActor.GetLabelTextProperty().SetColor(0.0,0.0,0.0)
            self.ScalarBarActor.SetLabelFormat('%.2f')
            self.vmtkRenderer.Renderer.AddActor(self.ScalarBarActor)

        if (self.Display == 1):
            self.vmtkRenderer.Render()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()

    def Execute(self):

        if (self.Mesh == None) & (self.Display == 1):
            self.PrintError('Error: no Mesh.')

        self.BuildView()
        
if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
