#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshprojection.py,v $
## Language:  Python
## Date:      $Date: 2006/07/17 09:53:14 $
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


class vmtkMeshProjection(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.ReferenceMesh = None
        self.Mesh = None
        self.Method = 'linear'
        self.ActiveArrays = []
        self.LineSurfaceVolume = 0
        self.Tolerance = 1E-8

        self.SetScriptName('vmtkmeshprojection')
        self.SetScriptDoc('interpolates the point data of a reference mesh onto the input mesh')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['ReferenceMesh','r','vtkUnstructuredGrid',1,'','the reference mesh','vmtkmeshreader'],
            ['Method','method','str',1,'["linear","closestpoint"]',"'linear': a linear interpolation/projection is performed on the points of the input surface (only works with point data arrays); 'closestpoint': in each cell/point of the input surface the value of the closest cell/point is projected (usefull to project discrete arrays like entityids)"],
            ['ActiveArrays','activearrays','str',-1,'','list of the point-data arrays to project (if empty, all the arrays are projected)'],
            ['Tolerance','tolerance','double',1,'(0.0,)','locator tolerance (linear method only)'],
            ['LineSurfaceVolume','linesurfacevolume','bool',1,'','(closestpoint method only) toggle performing a line-to-line, surface-to-surface, volume-to-volume projection, usefull for entityids']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']
            ])


    def ExtractLinesSurfaceVolume(self,mesh):

        # only linear elements
        volumeTypes = (vtk.VTK_TETRA,vtk.VTK_VOXEL,vtk.VTK_HEXAHEDRON,vtk.VTK_WEDGE,vtk.VTK_PYRAMID,vtk.VTK_PENTAGONAL_PRISM,vtk.VTK_HEXAGONAL_PRISM)
        surfaceTypes = (vtk.VTK_TRIANGLE,vtk.VTK_TRIANGLE_STRIP,vtk.VTK_POLYGON,vtk.VTK_PIXEL,vtk.VTK_QUAD)
        lineTypes = (vtk.VTK_LINE,vtk.VTK_POLY_LINE)

        cellTypeArray = vtk.vtkIntArray()
        cellTypeArray.SetName('tmpCellTypeArray')
        cellTypeArray.SetNumberOfTuples(mesh.GetNumberOfCells())
        cellTypeArray.FillComponent(0,0)
        mesh.GetCellData().AddArray(cellTypeArray)
        
        for i in range(mesh.GetNumberOfCells()):
            if mesh.GetCell(i).GetCellType() in volumeTypes:
                cellTypeArray.SetComponent(i,0,3)
            elif mesh.GetCell(i).GetCellType() in surfaceTypes:
                cellTypeArray.SetComponent(i,0,2)
            elif mesh.GetCell(i).GetCellType() in lineTypes:
                cellTypeArray.SetComponent(i,0,1)

        def threshold(value):
            th = vtk.vtkThreshold()
            th.SetInputData(mesh)
            th.SetInputArrayToProcess(0, 0, 0, 1, 'tmpCellTypeArray')
            th.ThresholdBetween(value,value)
            th.Update()
            return th.GetOutput()

        volume = threshold(3)
        volume.GetCellData().RemoveArray('tmpCellTypeArray')

        surface = threshold(2)
        surface.GetCellData().RemoveArray('tmpCellTypeArray')

        lines = threshold(1)
        lines.GetCellData().RemoveArray('tmpCellTypeArray')

        return lines,surface,volume


    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: No Mesh.')

        if self.ReferenceMesh == None:
            self.PrintError('Error: No ReferenceMesh.')

        if len(self.ActiveArrays) != 0:        
            passArray = vtk.vtkPassArrays()
            passArray.SetInputData(self.ReferenceMesh)

            for name in self.ActiveArrays:
                passArray.AddPointDataArray(name)

            passArray.Update()
            self.ReferenceMesh = passArray.GetOutput()

        if self.Method == 'linear':
            self.PrintLog('Computing linear projection ...')
            meshProjection = vtkvmtk.vtkvmtkMeshProjection()
            meshProjection.SetInputData(self.Mesh)
            meshProjection.SetReferenceMesh(self.ReferenceMesh)
            meshProjection.SetTolerance(self.Tolerance)
            meshProjection.Update()
            self.Mesh = meshProjection.GetOutput()

        elif self.Method == 'closestpoint':
            from vmtk import vmtkscripts

            self.PrintLog('Computing closest point projection ...')
            cpProj = vmtkscripts.vmtkSurfaceProjection()
            
            if self.LineSurfaceVolume:
                lin,sur,vol = self.ExtractLinesSurfaceVolume(self.Mesh)
                rlin,rsur,rvol = self.ExtractLinesSurfaceVolume(self.ReferenceMesh)

                if lin.GetNumberOfCells()>0 and rlin.GetNumberOfCells()>0:
                    lin = cpProj.ClosestPointProjection(lin,rlin)
                    self.PrintLog('\tprojecting lines-to-lines ...')
                if sur.GetNumberOfCells()>0 and rsur.GetNumberOfCells()>0:
                    self.PrintLog('\tprojecting surface-to-surface ...')
                    sur = cpProj.ClosestPointProjection(sur,rsur)
                if vol.GetNumberOfCells()>0 and rvol.GetNumberOfCells()>0:
                    self.PrintLog('\tprojecting volume-to-volume ...')
                    vol = cpProj.ClosestPointProjection(vol,rvol)

                af = vtkvmtk.vtkvmtkAppendFilter()
                af.AddInputData(lin)
                af.AddInputData(sur)
                af.AddInputData(vol)
                af.Update()
                self.Mesh = af.GetOutput()
            else:
                cpProj.ClosestPointProjection(self.Mesh,self.ReferenceMesh)



if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
