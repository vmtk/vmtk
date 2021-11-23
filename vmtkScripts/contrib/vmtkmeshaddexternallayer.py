#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshaddexternallayer.py,v $
## Language:  Python
## Date:      $$
## Version:   $$

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Tangui Morvan
##       Kalkulo AS
##       Simula Research Laboratory

## This class builds an external layer around a mesh.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
from vmtk import vtkvmtk
import sys
from vmtk import vmtkscripts

from vmtk import pypes


class vmtkMeshAddExternalLayer(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None

        self.CellEntityIdsArrayName = 'CellEntityIds'

        #Id of the first wall (mesh surface)
        self.SurfaceCellEntityId = 1
        #Id of the first openprofile in the walls
        self.InletOutletCellEntityId = 2
        #Id of the surface cells to extrude
        self.ExtrudeCellEntityId = 1

        self.ThicknessArrayName = ''

        self.Thickness = 1.0
        self.ThicknessRatio = 0.1
        self.MaximumThickness = 1E10

        self.NumberOfSubLayers = 1
        self.SubLayerRatio = 1.0

        self.ConstantThickness = 1;
        self.IncludeSurfaceCells = 0
        self.NegateWarpVectors = 0

        self.IncludeExtrudedOpenProfilesCells = 1
        self.IncludeExtrudedSurfaceCells = 1
        self.IncludeOriginalSurfaceCells = 0

        self.SetScriptName('vmtkmeshaddexternallayer')
        self.SetScriptDoc('create an external prismatic layer from the wall of a mesh and the normals on the wall.')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,''],
            ['SurfaceCellEntityId','surfacecellentityid','int',1,'','id of the first surface cells in the entityids list'],
            ['InletOutletCellEntityId','inletoutletcellentityid','int',1],
            ['ExtrudeCellEntityId','extrudecellentityid','int',1,'','id of the surface cells to extrude'],
            ['ThicknessArrayName','thicknessarray','str',1,'','name of the array where scalars defining boundary layer thickness are stored'],
            ['Thickness','thickness','float',1,'','value of constant boundary layer thickness'],
            ['ThicknessRatio','thicknessratio','float',1,'(0.0,)','multiplying factor for boundary layer thickness'],
            ['MaximumThickness','maximumthickness','float',1,'','maximum allowed value for boundary layer thickness'],
            ['NumberOfSubLayers','numberofsublayers','int',1,'(0,)','number of sublayers which the boundary layer has to be made of'],
            ['SubLayerRatio','sublayerratio','float',1,'(0.0,)','ratio between the thickness of two successive boundary layers'],
            ['ConstantThickness','constantthickness','bool',1,'','toggle constant boundary layer thickness'],
            ['IncludeSurfaceCells','includesurfacecells','bool',0,'','include all surface cells in output mesh'],
            ['IncludeExtrudedOpenProfilesCells','includeextrudedopenprofilescells','bool',1,'','include the cells from the open profiles extruded surface in output mesh'],
            ['IncludeExtrudedSurfaceCells','includeextrudedsurfacecells','bool',1,'','include the cells from the extruded surface in output mesh'],
            ['IncludeOriginalSurfaceCells','includeoriginalsurfacecells','bool',1,'','include the cells from the original surfacein output mesh']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter'],
            ])

    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: No input mesh.')

        if not self.CellEntityIdsArrayName:
            self.PrintError('Error: No input CellEntityIdsArrayName.')
            return

        cellEntityIdsArray = self.Mesh.GetCellData().GetArray(self.CellEntityIdsArrayName)

        #cut off the volumetric elements
        wallThreshold = vtk.vtkThreshold()
        wallThreshold.SetInputData(self.Mesh)
        wallThreshold.ThresholdByUpper(self.SurfaceCellEntityId-0.5)
        wallThreshold.SetInputArrayToProcess(0,0,0,1,self.CellEntityIdsArrayName)
        wallThreshold.Update()

        meshToSurface = vmtkscripts.vmtkMeshToSurface()
        meshToSurface.Mesh = wallThreshold.GetOutput()
        meshToSurface.Execute()

        #Compute the normals for this surface, orientation should be right because the surface is closed
        #TODO: Add option for cell normals in vmtksurfacenormals
        normalsFilter = vtk.vtkPolyDataNormals()
        normalsFilter.SetInputData(meshToSurface.Surface)
        normalsFilter.SetAutoOrientNormals(1)
        normalsFilter.SetFlipNormals(0)
        normalsFilter.SetConsistency(1)
        normalsFilter.SplittingOff()
        normalsFilter.ComputePointNormalsOff()
        normalsFilter.ComputeCellNormalsOn()
        normalsFilter.Update()

        surfaceToMesh = vmtkscripts.vmtkSurfaceToMesh()
        surfaceToMesh.Surface = normalsFilter.GetOutput()
        surfaceToMesh.Execute()

        #Save the current normals
        wallWithBoundariesMesh = surfaceToMesh.Mesh
        savedNormals = vtk.vtkDoubleArray()
        savedNormals.DeepCopy(wallWithBoundariesMesh.GetCellData().GetNormals())
        savedNormals.SetName('SavedNormals')
        wallWithBoundariesMesh.GetCellData().AddArray(savedNormals)

        #cut off the boundaries and other surfaces
        extrudeThresholdLower = vtk.vtkThreshold()
        extrudeThresholdLower.SetInputData(wallWithBoundariesMesh)
        extrudeThresholdLower.ThresholdByLower(self.ExtrudeCellEntityId+0.5)
        extrudeThresholdLower.SetInputArrayToProcess(0,0,0,1,self.CellEntityIdsArrayName)
        extrudeThresholdLower.Update()

        extrudeThresholdUpper = vtk.vtkThreshold()
        extrudeThresholdUpper.SetInputConnection(extrudeThresholdLower.GetOutputPort())
        extrudeThresholdUpper.ThresholdByUpper(self.ExtrudeCellEntityId-0.5)
        extrudeThresholdUpper.SetInputArrayToProcess(0,0,0,1,self.CellEntityIdsArrayName)
        extrudeThresholdUpper.Update()

        meshToSurface = vmtkscripts.vmtkMeshToSurface()
        meshToSurface.Mesh = extrudeThresholdUpper.GetOutput()
        meshToSurface.Execute()

        #Compute cell normals without boundaries
        normalsFilter = vtk.vtkPolyDataNormals()
        normalsFilter.SetInputData(meshToSurface.Surface)
        normalsFilter.SetAutoOrientNormals(1)
        normalsFilter.SetFlipNormals(0)
        normalsFilter.SetConsistency(1)
        normalsFilter.SplittingOff()
        normalsFilter.ComputePointNormalsOn()
        normalsFilter.ComputeCellNormalsOn()
        normalsFilter.Update()

        wallWithoutBoundariesSurface = normalsFilter.GetOutput()

        normals = wallWithoutBoundariesSurface.GetCellData().GetNormals()
        savedNormals = wallWithoutBoundariesSurface.GetCellData().GetArray('SavedNormals')

        math = vtk.vtkMath()

        #If the normal are inverted, recompute the normals with flipping on
        if normals.GetNumberOfTuples() > 0 and math.Dot(normals.GetTuple3(0),savedNormals.GetTuple3(0)) < 0:
            normalsFilter = vtk.vtkPolyDataNormals()
            normalsFilter.SetInputData(meshToSurface.Surface)
            normalsFilter.SetAutoOrientNormals(1)
            normalsFilter.SetFlipNormals(1)
            normalsFilter.SetConsistency(1)
            normalsFilter.SplittingOff()
            normalsFilter.ComputePointNormalsOn()
            normalsFilter.ComputeCellNormalsOn()
            normalsFilter.Update()
            wallWithoutBoundariesSurface = normalsFilter.GetOutput()

        wallWithoutBoundariesSurface.GetPointData().GetNormals().SetName('Normals')

        wallWithoutBoundariesSurface.GetCellData().RemoveArray('SavedNormals')

        surfaceToMesh = vmtkscripts.vmtkSurfaceToMesh()
        surfaceToMesh.Surface = wallWithoutBoundariesSurface
        surfaceToMesh.Execute()

        #Offset to apply to the array
        wallOffset = 0
        if self.IncludeSurfaceCells or self.IncludeOriginalSurfaceCells:
            wallOffset += 1
        if self.IncludeSurfaceCells or self.IncludeExtrudedSurfaceCells:
            wallOffset+=1

        boundaryLayer = vmtkscripts.vmtkBoundaryLayer2()
        boundaryLayer.Mesh = surfaceToMesh.Mesh
        boundaryLayer.WarpVectorsArrayName = 'Normals'
        boundaryLayer.NegateWarpVectors = False
        boundaryLayer.ThicknessArrayName = self.ThicknessArrayName
        boundaryLayer.ConstantThickness = self.ConstantThickness
        boundaryLayer.IncludeSurfaceCells = self.IncludeSurfaceCells
        boundaryLayer.NumberOfSubLayers = self.NumberOfSubLayers
        boundaryLayer.SubLayerRatio = self.SubLayerRatio
        boundaryLayer.Thickness = self.Thickness
        boundaryLayer.ThicknessRatio = self.Thickness
        boundaryLayer.MaximumThickness = self.MaximumThickness
        boundaryLayer.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        boundaryLayer.IncludeExtrudedOpenProfilesCells = self.IncludeExtrudedOpenProfilesCells
        boundaryLayer.IncludeExtrudedSurfaceCells = self.IncludeExtrudedSurfaceCells
        boundaryLayer.IncludeOriginalSurfaceCells = self.IncludeOriginalSurfaceCells
        boundaryLayer.LayerEntityId = self.SurfaceCellEntityId
        boundaryLayer.SurfaceEntityId = self.InletOutletCellEntityId + 1
        if cellEntityIdsArray != None:
            #Append the new surface ids
            idRange = cellEntityIdsArray.GetRange()
            boundaryLayer.OpenProfilesEntityId = idRange[1] + wallOffset + 2
        boundaryLayer.Execute()

        if cellEntityIdsArray != None:
            #offset the previous cellentityids to make room for the new ones
            arrayCalculator = vtk.vtkArrayCalculator()
            arrayCalculator.SetInputData(self.Mesh)
            if vtk.vtkVersion.GetVTKMajorVersion()>=9 or (vtk.vtkVersion.GetVTKMajorVersion()>=8 and vtk.vtkVersion.GetVTKMinorVersion()>=1):
                arrayCalculator.SetAttributeTypeToCellData()
            else:
                arrayCalculator.SetAttributeModeToUseCellData()
            arrayCalculator.AddScalarVariable("entityid",self.CellEntityIdsArrayName,0)
            arrayCalculator.SetFunction("if( entityid > " + str(self.InletOutletCellEntityId-1) +", entityid + " + str(wallOffset) + ", entityid)")
            arrayCalculator.SetResultArrayName('CalculatorResult')
            arrayCalculator.Update()

            #This need to be copied in order to be of the right type (int)
            cellEntityIdsArray.DeepCopy(arrayCalculator.GetOutput().GetCellData().GetArray('CalculatorResult'))

            arrayCalculator.SetFunction("if( entityid > " + str(self.SurfaceCellEntityId-1) +", entityid + 1, entityid)")
            arrayCalculator.Update()

            ##This need to be copied in order to be of the right type (int)
            cellEntityIdsArray.DeepCopy(arrayCalculator.GetOutput().GetCellData().GetArray('CalculatorResult'))

        appendFilter = vtkvmtk.vtkvmtkAppendFilter()
        appendFilter.AddInput(self.Mesh)
        appendFilter.AddInput(boundaryLayer.Mesh)
        appendFilter.Update()

        self.Mesh = appendFilter.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
