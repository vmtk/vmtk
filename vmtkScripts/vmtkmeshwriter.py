#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshwriter.py,v $
## Language:  Python
## Date:      $Date: 2006/07/27 08:27:40 $
## Version:   $Revision: 1.13 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

import sys
import vtk
import vtkvmtk

import pypes

vmtkmeshwriter = 'vmtkMeshWriter'

class vmtkMeshWriter(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Format = ''
        self.GuessFormat = 1
        self.OutputFileName = ''
        self.Mesh = None
        self.Input = None

        self.CellEntityIdsArrayName = ''

        self.SetScriptName('vmtkmeshwriter')
        self.SetScriptDoc('write a mesh to disk')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['Format','f','str',1,'["vtkxml","vtk","xda","fdneut","tecplot","lifev","dolfin","pointdata"]','file format (xda - libmesh ASCII format, fdneut - FIDAP neutral format)'],
            ['GuessFormat','guessformat','bool',1,'','guess file format from extension'],
            ['OutputFileName','ofile','str',1,'','output file name'],
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'','name of the array where entity ids are stored']
            ])
        self.SetOutputMembers([])

    def WriteVTKMeshFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing VTK mesh file.')
        writer = vtk.vtkUnstructuredGridWriter()
        writer.SetInput(self.Mesh)
        writer.SetFileName(self.OutputFileName)
        writer.Write()

    def WriteVTKXMLMeshFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing VTK XML mesh file.')
        writer = vtk.vtkXMLUnstructuredGridWriter()
        writer.SetInput(self.Mesh)
        writer.SetFileName(self.OutputFileName)
        writer.Write()

    def WriteXdaMeshFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing Xda mesh file.')
        writer = vtkvmtk.vtkvmtkXdaWriter()
        writer.SetInput(self.Mesh)
        writer.SetFileName(self.OutputFileName)
        if self.CellEntityIdsArrayName != '':
            writer.SetBoundaryDataArrayName(self.CellEntityIdsArrayName)
        writer.Write()

    def WriteFDNEUTMeshFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing FDNEUT mesh file.')
        writer = vtkvmtk.vtkvmtkFDNEUTWriter()
        writer.SetInput(self.Mesh)
        writer.SetFileName(self.OutputFileName)
        writer.Write()

    def WriteTecplotMeshFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing Tecplot file.')
        triangleFilter = vtk.vtkDataSetTriangleFilter()
        triangleFilter.SetInput(self.Mesh)
        triangleFilter.Update()
        self.Mesh = triangleFilter.GetOutput()
        f=open(self.OutputFileName, 'w')
        line = "VARIABLES = X,Y,Z"
        arrayNames = []
        for i in range(self.Mesh.GetPointData().GetNumberOfArrays()):
            array = self.Mesh.GetPointData().GetArray(i)
            arrayName = array.GetName()
            if arrayName == None:
                continue
            if (arrayName[-1]=='_'):
                continue
            arrayNames.append(arrayName)
            if (array.GetNumberOfComponents() == 1):
                line = line + ',' + arrayName
            else:
                for j in range(array.GetNumberOfComponents()):
                    line = line + ',' + arrayName + str(j)
        line = line + '\n'
        f.write(line)
        tetraCellIdArray = vtk.vtkIdTypeArray()
        tetraCellType = 10
        self.Mesh.GetIdsOfCellsOfType(tetraCellType,tetraCellIdArray)
        numberOfTetras = tetraCellIdArray.GetNumberOfTuples()
        line = "ZONE " + "N=" + str(self.Mesh.GetNumberOfPoints()) + ',' + "E=" + str(numberOfTetras) + ',' + "F=FEPOINT" + ',' + "ET=TETRAHEDRON" + '\n'
        f.write(line)
        for i in range(self.Mesh.GetNumberOfPoints()):
            point = self.Mesh.GetPoint(i)
            line = str(point[0]) + ' ' + str(point[1]) + ' ' + str(point[2])
            for arrayName in arrayNames:
                array = self.Mesh.GetPointData().GetArray(arrayName)
                for j in range(array.GetNumberOfComponents()):
                    line = line + ' ' + str(array.GetComponent(i,j))
            line = line + '\n'
            f.write(line)
        for i in range(numberOfTetras):
            cellPointIds = self.Mesh.GetCell(tetraCellIdArray.GetValue(i)).GetPointIds()
            line = ''
            for j in range(cellPointIds.GetNumberOfIds()):
                if (j>0):
                    line = line + ' '
                line = line + str(cellPointIds.GetId(j)+1)
            line = line + '\n'
            f.write(line)

    def WriteLifeVMeshFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing LifeV file.')

        self.Mesh.BuildLinks()

        cellEntityIdsArray = vtk.vtkIntArray()
        cellEntityIdsArray.DeepCopy(self.Mesh.GetCellData().GetArray(self.CellEntityIdsArrayName))

        f=open(self.OutputFileName, 'w')
        line = "MeshVersionFormatted 1\n\n"
        line += "Dimension\n"
        line += "3\n\n"
        line += "Vertices\n"
        line += "%d\n" % self.Mesh.GetNumberOfPoints()
        f.write(line)
        for i in range(self.Mesh.GetNumberOfPoints()):
            point = self.Mesh.GetPoint(i)
            pointCells = vtk.vtkIdList()
            self.Mesh.GetPointCells(i,pointCells)
            maxCellEntityId = -1
            for j in range(pointCells.GetNumberOfIds()):
                cellEntityId = cellEntityIdsArray.GetValue(pointCells.GetId(j))
                if cellEntityId > maxCellEntityId:
                    maxCellEntityId = cellEntityId
            line = "%f  %f  %f  %d\n" % (point[0], point[1], point[2], maxCellEntityId)
            f.write(line)
        line = "\n"

        tetraCellIdArray = vtk.vtkIdTypeArray()
        tetraCellType = 10
        self.Mesh.GetIdsOfCellsOfType(tetraCellType,tetraCellIdArray)
        numberOfTetras = tetraCellIdArray.GetNumberOfTuples()
        line += "Tetrahedra\n"
        line += "%d\n" % numberOfTetras
        f.write(line)
        for i in range(numberOfTetras):
            tetraCellId = tetraCellIdArray.GetValue(i) 
            cellPointIds = self.Mesh.GetCell(tetraCellId).GetPointIds()
            line = ''
            for j in range(cellPointIds.GetNumberOfIds()):
                if j>0:
                    line += '  '
                line += "%d" % (cellPointIds.GetId(j)+1)
            line += '  %d\n' % 1
            f.write(line)
        line = "\n"

        triangleCellIdArray = vtk.vtkIdTypeArray()
        triangleCellType = 5
        self.Mesh.GetIdsOfCellsOfType(triangleCellType,triangleCellIdArray)
        numberOfTriangles = triangleCellIdArray.GetNumberOfTuples()
        line += "Triangles\n"
        line += "%d\n" % numberOfTriangles
        f.write(line)
        for i in range(numberOfTriangles):
            triangleCellId = triangleCellIdArray.GetValue(i)
            cellPointIds = self.Mesh.GetCell(triangleCellId).GetPointIds()
            line = ''
            for j in range(cellPointIds.GetNumberOfIds()):
                if j>0:
                    line += '  '
                line += "%d" % (cellPointIds.GetId(j)+1)
            cellEntityId = cellEntityIdsArray.GetValue(triangleCellId)
            line += '  %d\n' % cellEntityId
            f.write(line)

    def WriteDolfinMeshFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing Dolfin file.')
        writer = vtkvmtk.vtkvmtkDolfinWriter()
        writer.SetInput(self.Mesh)
        writer.SetFileName(self.OutputFileName)
        if self.CellEntityIdsArrayName != '':
            writer.SetBoundaryDataArrayName(self.CellEntityIdsArrayName)
        writer.Write()

    def WritePointDataMeshFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing PointData file.')
        f=open(self.OutputFileName, 'w')
        line = "X Y Z"
        arrayNames = []
        for i in range(self.Mesh.GetPointData().GetNumberOfArrays()):
            array = self.Mesh.GetPointData().GetArray(i)
            arrayName = array.GetName()
            if arrayName == None:
                continue
            if (arrayName[-1]=='_'):
                continue
            arrayNames.append(arrayName)
            if (array.GetNumberOfComponents() == 1):
                line = line + ' ' + arrayName
            else:
                for j in range(array.GetNumberOfComponents()):
                    line = line + ' ' + arrayName + str(j)
        line = line + '\n'
        f.write(line)
        for i in range(self.Mesh.GetNumberOfPoints()):
            point = self.Mesh.GetPoint(i)
            line = str(point[0]) + ' ' + str(point[1]) + ' ' + str(point[2])
            for arrayName in arrayNames:
                array = self.Mesh.GetPointData().GetArray(arrayName)
                for j in range(array.GetNumberOfComponents()):
                    line = line + ' ' + str(array.GetComponent(i,j))
            line = line + '\n'
            f.write(line)

    def Execute(self):

        if self.Mesh == None:
            if self.Input == None:
                self.PrintError('Error: no Mesh.')
            self.Mesh = self.Input

        extensionFormats = {'vtu':'vtkxml', 
                            'vtkxml':'vtkxml', 
                            'vtk':'vtk',
                            'xda':'xda',
                            'FDNEUT':'fdneut',
                            'lifev':'lifev',
                            'xml':'dolfin',
                            'tec':'tecplot',
                            'dat':'pointdata'}

        if self.OutputFileName == 'BROWSER':
            import tkFileDialog
            initialDir = '.'
            self.OutputFileName = tkFileDialog.asksaveasfilename(title="Output mesh",initialdir=initialDir)
            if not self.OutputFileName:
                self.PrintError('Error: no OutputFileName.')

        if self.GuessFormat and self.OutputFileName and not self.Format:
            import os.path
            extension = os.path.splitext(self.OutputFileName)[1]
            if extension:
                extension = extension[1:]
                if extension in extensionFormats.keys():
                    self.Format = extensionFormats[extension]

        if (self.Format == 'vtk'):
            self.WriteVTKMeshFile()
        elif (self.Format == 'vtkxml'):
            self.WriteVTKXMLMeshFile()
        elif (self.Format == 'xda'):
            self.WriteXdaMeshFile()
        elif (self.Format == 'fdneut'):
            self.WriteFDNEUTMeshFile()
        elif (self.Format == 'lifev'):
            self.WriteLifeVMeshFile()
        elif (self.Format == 'dolfin'):
            self.WriteDolfinMeshFile()
        elif (self.Format == 'tecplot'):
            self.WriteTecplotMeshFile()
        elif (self.Format == 'pointdata'):
            self.WritePointDataMeshFile()
        else:
            self.PrintError('Error: unsupported format '+ self.Format + '.')


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
