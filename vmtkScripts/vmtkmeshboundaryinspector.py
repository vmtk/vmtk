#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshboundaryinspector.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.3 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.


import vtk
import sys

import vtkvmtk
import vmtkrenderer
import pypes

vmtkmeshboundaryinspector = 'vmtkMeshBoundaryInspector'

class vmtkMeshBoundaryInspector(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Mesh = None

        self.CellEntityIdsArrayName = ''

        self.VolumeCellEntityId = 0
        self.WallCellEntityId = 1

        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.ReferenceSystems = None

        self.SetScriptName('vmtkmeshboundaryinspector')
        self.SetScriptDoc('')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,''],
            ['VolumeCellEntityId','volumeid','int',1],
            ['WallCellEntityId','wallid','int',1],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']])
        self.SetOutputMembers([
            ['ReferenceSystems','o','vtkPolyData',1,'','the output reference systems with boundary information','vmtksurfacewriter']
            ])

    def Execute(self):
 
        if not self.Mesh:
            self.PrintError('Error: No input mesh.')
            return
 
        if not self.CellEntityIdsArrayName:
            self.PrintError('Error: No input CellEntityIdsArrayName.')
            return
 
        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self) 

        threshold = vtk.vtkThreshold()
        threshold.SetInput(self.Mesh)
        threshold.ThresholdByUpper(self.VolumeCellEntityId+0.5)
        threshold.SetInputArrayToProcess(0,0,0,1,self.CellEntityIdsArrayName)
        threshold.Update()

        boundaryMesh = threshold.GetOutput()
        boundaryMesh.GetCellData().SetActiveScalars(self.CellEntityIdsArrayName)

        boundaryMapper = vtk.vtkDataSetMapper()
        boundaryMapper.SetInput(boundaryMesh)
        boundaryMapper.ScalarVisibilityOn()
        boundaryMapper.SetScalarModeToUseCellData()
        boundaryMapper.SetScalarRange(boundaryMesh.GetCellData().GetScalars().GetRange())

        boundaryActor = vtk.vtkActor()
        boundaryActor.SetMapper(boundaryMapper)
        self.vmtkRenderer.Renderer.AddActor(boundaryActor)

        wallThreshold = vtk.vtkThreshold()
        wallThreshold.SetInput(boundaryMesh)
        wallThreshold.ThresholdByLower(self.WallCellEntityId+0.5)
        wallThreshold.SetInputArrayToProcess(0,0,0,1,self.CellEntityIdsArrayName)
        wallThreshold.Update()

        wallMeshToSurface = vtk.vtkGeometryFilter()
        wallMeshToSurface.SetInput(wallThreshold.GetOutput())
        wallMeshToSurface.Update()

        boundaryReferenceSystems = vtkvmtk.vtkvmtkBoundaryReferenceSystems()
        boundaryReferenceSystems.SetInput(wallMeshToSurface.GetOutput())
        boundaryReferenceSystems.SetBoundaryRadiusArrayName("BoundaryRadius")
        boundaryReferenceSystems.SetBoundaryNormalsArrayName("BoundaryNormals")
        boundaryReferenceSystems.SetPoint1ArrayName("Point1Array")
        boundaryReferenceSystems.SetPoint2ArrayName("Point2Array")
        boundaryReferenceSystems.Update()

        self.ReferenceSystems = boundaryReferenceSystems.GetOutput()

        cellEntityIdsArray = vtk.vtkIntArray()
        cellEntityIdsArray.SetName(self.CellEntityIdsArrayName)
        cellEntityIdsArray.SetNumberOfTuples(self.ReferenceSystems.GetNumberOfPoints())

        self.ReferenceSystems.GetPointData().AddArray(cellEntityIdsArray)

        boundaryThreshold = vtk.vtkThreshold()
        boundaryThreshold.SetInput(boundaryMesh)
        boundaryThreshold.ThresholdByUpper(self.WallCellEntityId+0.5)
        boundaryThreshold.SetInputArrayToProcess(0,0,0,1,self.CellEntityIdsArrayName)
        boundaryThreshold.Update()

        boundaryMeshToSurface = vtk.vtkGeometryFilter()
        boundaryMeshToSurface.SetInput(boundaryThreshold.GetOutput())
        boundaryMeshToSurface.Update()

        boundarySurface = boundaryMeshToSurface.GetOutput()
        pointCells = vtk.vtkIdList()

        surfaceCellEntityIdsArray = vtk.vtkIntArray()
        surfaceCellEntityIdsArray.DeepCopy(boundarySurface.GetCellData().GetArray(self.CellEntityIdsArrayName))

        self.PrintLog('')
        for i in range(self.ReferenceSystems.GetNumberOfPoints()):
            pointId = boundarySurface.FindPoint(self.ReferenceSystems.GetPoint(i))
            boundarySurface.GetPointCells(pointId,pointCells)
            cellId = pointCells.GetId(0)
            cellEntityId = surfaceCellEntityIdsArray.GetValue(cellId)
            cellEntityIdsArray.SetValue(i,cellEntityId)
            origin = self.ReferenceSystems.GetPoint(i)
            normal = self.ReferenceSystems.GetPointData().GetArray("BoundaryNormals").GetTuple3(i)
            radius = self.ReferenceSystems.GetPointData().GetArray("BoundaryRadius").GetTuple1(i)
            logLine = 'CellEntityId: %d\n' % cellEntityId
            logLine += '  Origin: %f, %f, %f\n' % (origin[0],origin[1],origin[2])
            logLine += '  Normal: %f, %f, %f\n' % (normal[0],normal[1],normal[2])
            logLine += '  Radius: %f\n' % radius
            self.PrintLog(logLine)

        self.ReferenceSystems.GetPointData().SetActiveScalars(self.CellEntityIdsArrayName)

        labelsMapper = vtk.vtkLabeledDataMapper();
        labelsMapper.SetInput(self.ReferenceSystems)
        labelsMapper.SetLabelModeToLabelScalars()
        labelsActor = vtk.vtkActor2D()
        labelsActor.SetMapper(labelsMapper)
        self.vmtkRenderer.Renderer.AddActor(labelsActor)

        self.vmtkRenderer.Render()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()

