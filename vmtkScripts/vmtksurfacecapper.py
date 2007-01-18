#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacecapper.py,v $
## Language:  Python
## Date:      $Date: 2006/07/17 09:53:14 $
## Version:   $Revision: 1.8 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.


import sys
import vtk

import vtkvmtk
import pypes

vmtksurfacecapper = 'vmtkSurfaceCapper'

class vmtkSurfaceCapper(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Surface = None
        self.TriangleOutput = 1
        self.CellMarkerArrayName = ''
        self.Interactive = 1

        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.SetScriptName('vmtksurfacecapper')
        self.SetScriptDoc('add caps to the holes of a surface, assigning an id to each cap for easy specification of boundary conditions')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'the input surface','vmtksurfacereader'],
            ['TriangleOutput','triangle','int',1,'toggle triangulation of the output'],
            ['CellMarkerArrayName','cellmarkerarray','str',1,'name of the array where the id of the caps are stored'],
            ['Interactive','interactive','int',1],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'external renderer']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'the output surface','vmtksurfacewriter']])

    def LabelValidator(self,text):
        import string
        if not text:
            return 0
        for char in text:
            if char not in string.digits + " ":
                return 0
        return 1

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        boundaryIds = vtk.vtkIdList()

        if self.Interactive:
            if not self.vmtkRenderer:
                import vmtkrenderer
                self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
                self.vmtkRenderer.Initialize()
                self.OwnRenderer = 1

            boundaryExtractor = vtkvmtk.vtkvmtkPolyDataBoundaryExtractor()
            boundaryExtractor.SetInput(self.Surface)
            boundaryExtractor.Update()
            boundaries = boundaryExtractor.GetOutput()
            numberOfBoundaries = boundaries.GetNumberOfCells()
            seedPoints = vtk.vtkPoints()
            for i in range(numberOfBoundaries):
                barycenter = [0.0, 0.0, 0.0]
                vtkvmtk.vtkvmtkBoundaryReferenceSystems.ComputeBoundaryBarycenter(boundaries.GetCell(i).GetPoints(),barycenter)
                seedPoints.InsertNextPoint(barycenter)
            seedPolyData = vtk.vtkPolyData()
            seedPolyData.SetPoints(seedPoints)
            seedPolyData.Update()
            labelsMapper = vtk.vtkLabeledDataMapper();
            labelsMapper.SetInput(seedPolyData)
            labelsMapper.SetLabelModeToLabelIds()
            labelsActor = vtk.vtkActor2D()
            labelsActor.SetMapper(labelsMapper)
    
            self.vmtkRenderer.Renderer.AddActor(labelsActor)
    
            surfaceMapper = vtk.vtkPolyDataMapper()
            surfaceMapper.SetInput(self.Surface)
            surfaceMapper.ScalarVisibilityOff()
            surfaceActor = vtk.vtkActor()
            surfaceActor.SetMapper(surfaceMapper)
            surfaceActor.GetProperty().SetOpacity(0.25)
    
            self.vmtkRenderer.Renderer.AddActor(surfaceActor)
    
            self.vmtkRenderer.Render()
    
            self.vmtkRenderer.Renderer.RemoveActor(labelsActor)
            self.vmtkRenderer.Renderer.RemoveActor(surfaceActor)
            
            ok = False
            while not ok:
                labelString = self.InputText("Please input boundary ids: ",self.LabelValidator)
                labels = [int(label) for label in labelString.split()]
                ok = True
                for label in labels:
                    if label not in range(numberOfBoundaries):
                        ok = False

            for label in labels:
                boundaryIds.InsertNextId(label)

        capper = vtkvmtk.vtkvmtkSimpleCapPolyData()
        capper.SetInput(self.Surface)
        if self.Interactive:
            capper.SetBoundaryIds(boundaryIds)
        capper.SetCellMarkerArrayName(self.CellMarkerArrayName)
        capper.Update()

        self.Surface = capper.GetOutput()

        if self.TriangleOutput == 1:
            triangle = vtk.vtkTriangleFilter()
            triangle.SetInput(self.Surface)
            triangle.PassLinesOff()
            triangle.PassVertsOff()
            triangle.Update()
            self.Surface = triangle.GetOutput()

        normals = vtk.vtkPolyDataNormals()
        normals.SetInput(self.Surface)
        normals.AutoOrientNormalsOn()
      	normals.SplittingOff()
      	normals.ConsistencyOn()
        normals.Update()
        self.Surface = normals.GetOutput()

        if self.Surface.GetSource():
            self.Surface.GetSource().UnRegisterAllOutputs()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
