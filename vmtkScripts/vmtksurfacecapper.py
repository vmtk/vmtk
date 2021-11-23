#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacecapper.py,v $
## Language:  Python
## Date:      $Date: 2006/07/17 09:53:14 $
## Version:   $Revision: 1.8 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import sys
import vtk

from vmtk import vtkvmtk
from vmtk import pypes


class vmtkSurfaceCapper(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.TriangleOutput = 1
        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.CellEntityIdOffset = 1
        self.Interactive = 1
        self.Method = 'simple'
        self.ConstraintFactor = 1.0
        self.NumberOfRings = 8

        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.SetScriptName('vmtksurfacecapper')
        self.SetScriptDoc('add caps to the holes of a surface, assigning an id to each cap for easy specification of boundary conditions.')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['Method','method','str',1,'["simple","centerpoint","smooth","annular","concaveannular"]','capping method'],
            ['TriangleOutput','triangle','bool',1,'','toggle triangulation of the output'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'','name of the array where the id of the caps have to be stored'],
            ['CellEntityIdOffset','entityidoffset','int',1,'(0,)','offset for entity ids'],
            ['ConstraintFactor','constraint','float',1,'','amount of influence of the shape of the surface near the boundary on the shape of the cap ("smooth" method only)'],
            ['NumberOfRings','rings','int',1,'(0,)','number of rings composing the cap ("smooth" method only)'],
            ['Interactive','interactive','bool',1],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'','name of the array where the id of the caps are stored']
            ])

    def LabelValidator(self,text):
        import string
        if not text:
            return 0
        if not text.split():
            return 0
        for char in text:
            if char not in string.digits + " ":
                return 0
        return 1

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

#        cleaner = vtk.vtkCleanPolyData()
#        cleaner.SetInput(self.Surface)
#        cleaner.Update()
#
#        triangleFilter = vtk.vtkTriangleFilter()
#        triangleFilter.SetInput(cleaner.GetOutput())
#        triangleFilter.Update()
#
#        self.Surface = triangleFilter.GetOutput()

        boundaryIds = vtk.vtkIdList()

        if self.Interactive:
            if not self.vmtkRenderer:
                from vmtk import vmtkrenderer
                self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
                self.vmtkRenderer.Initialize()
                self.OwnRenderer = 1

            self.vmtkRenderer.RegisterScript(self)

            boundaryExtractor = vtkvmtk.vtkvmtkPolyDataBoundaryExtractor()
            boundaryExtractor.SetInputData(self.Surface)
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
            labelsMapper = vtk.vtkLabeledDataMapper();
            labelsMapper.SetInputData(seedPolyData)
            labelsMapper.SetLabelModeToLabelIds()
            labelsActor = vtk.vtkActor2D()
            labelsActor.SetMapper(labelsMapper)

            self.vmtkRenderer.Renderer.AddActor(labelsActor)

            surfaceMapper = vtk.vtkPolyDataMapper()
            surfaceMapper.SetInputData(self.Surface)
            surfaceMapper.ScalarVisibilityOff()
            surfaceActor = vtk.vtkActor()
            surfaceActor.SetMapper(surfaceMapper)
            surfaceActor.GetProperty().SetOpacity(0.25)

            self.vmtkRenderer.Renderer.AddActor(surfaceActor)

            #self.vmtkRenderer.Render()
            #self.vmtkRenderer.Renderer.RemoveActor(labelsActor)
            #self.vmtkRenderer.Renderer.RemoveActor(surfaceActor)

            ok = False
            while not ok:
                labelString = self.InputText("Please input boundary ids: ",self.LabelValidator)
                labels = [int(label) for label in labelString.split()]
                ok = True
                for label in labels:
                    if label not in list(range(numberOfBoundaries)):
                        ok = False

            for label in labels:
                boundaryIds.InsertNextId(label)

        if self.Method == 'simple':
            capper = vtkvmtk.vtkvmtkSimpleCapPolyData()
            capper.SetInputData(self.Surface)

        elif self.Method == 'centerpoint':
            capper = vtkvmtk.vtkvmtkCapPolyData()
            capper.SetInputData(self.Surface)
            capper.SetDisplacement(0.0)
            capper.SetInPlaneDisplacement(0.0)

        elif self.Method == 'smooth':
            triangle = vtk.vtkTriangleFilter()
            triangle.SetInputData(self.Surface)
            triangle.PassLinesOff()
            triangle.PassVertsOff()
            triangle.Update()
            capper = vtkvmtk.vtkvmtkSmoothCapPolyData()
            capper.SetInputConnection(triangle.GetOutputPort())
            capper.SetConstraintFactor(self.ConstraintFactor)
            capper.SetNumberOfRings(self.NumberOfRings)

        elif self.Method == 'annular':
            capper = vtkvmtk.vtkvmtkAnnularCapPolyData()
            capper.SetInputData(self.Surface)

        elif self.Method == 'concaveannular':
            from vmtk import vtkvmtkcontrib
            capper = vtkvmtkcontrib.vtkvmtkConcaveAnnularCapPolyData()
            capper.SetInputData(self.Surface)

        if self.Interactive:
            capper.SetBoundaryIds(boundaryIds)
        capper.SetCellEntityIdsArrayName(self.CellEntityIdsArrayName)
        capper.SetCellEntityIdOffset(self.CellEntityIdOffset)
        capper.Update()
        self.Surface = capper.GetOutput()

        if self.TriangleOutput == 1:
            triangle = vtk.vtkTriangleFilter()
            triangle.SetInputData(self.Surface)
            triangle.PassLinesOff()
            triangle.PassVertsOff()
            triangle.Update()
            self.Surface = triangle.GetOutput()

        normals = vtk.vtkPolyDataNormals()
        normals.SetInputData(self.Surface)
        normals.AutoOrientNormalsOn()
        normals.SplittingOff()
        normals.ConsistencyOn()
        normals.Update()
        self.Surface = normals.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
