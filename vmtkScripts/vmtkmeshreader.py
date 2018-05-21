#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshreader.py,v $
## Language:  Python
## Date:      $Date: 2006/07/17 09:53:14 $
## Version:   $Revision: 1.16 $

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


class vmtkMeshReader(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Format = ''
        self.GuessFormat = 1
        self.InputFileName = ''
        self.Mesh = 0
        self.Output = 0

        self.GhostNodes = 1
        self.VolumeElementsOnly = 0

        self.CellEntityIdsArrayName = 'CellEntityIds'

        self.SetScriptName('vmtkmeshreader')
        self.SetScriptDoc('read a mesh and stores it in a vtkUnstructuredGrid object')
        self.SetInputMembers([
            ['Format','f','str',1,'["vtkxml", "vtkxmlp", "vtk","fdneut","ngneut","tecplot","tetgen","gambit"]','file format (fdneut - FIDAP neutral format, ngneut - Netgen neutral format)'],
            ['GuessFormat','guessformat','bool',1,'','guess file format from extension'],
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh'],
            ['InputFileName','ifile','str',1,'','input file name'],
            ['GhostNodes','ghostnodes','bool',1,'','store all nodes for 9-noded quads, 7-noded triangles, 27-noded hexahedra, 18-noded wedges; otherwise, store them as 8-noded quads, 6-noded triangles, 20-noded hexahedra, 15-noded wedges - fdneut only'],
            ['VolumeElementsOnly','volumeelementsonly','bool',1,'','only read volume elements - fdneut and ngneut'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'','name of the array where entity ids have to be stored - ngneut and tetgen']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'','name of the array where entity ids have been stored - ngneut and tetgen']
            ])

    def ReadTetGenMeshFile(self):
        if (self.InputFileName == ''):
            self.PrintError('Error: no InputFileName.')
        self.PrintLog('Reading TetGen mesh file.')
        import os.path
        inputFileName = os.path.splitext(self.InputFileName)[0]
        reader = vtkvmtk.vtkvmtkTetGenReader()
        reader.SetFileName(inputFileName)
        reader.SetBoundaryDataArrayName(self.CellEntityIdsArrayName)
        reader.Update()
        self.Mesh = reader.GetOutput()

    def ReadGAMBITMeshFile(self):
        if (self.InputFileName == ''):
            self.PrintError('Error: no InputFileName.')
        self.PrintLog('Reading GAMBIT mesh file.')
        reader = vtk.vtkGAMBITReader()
        reader.SetFileName(self.InputFileName)
        reader.Update()
        self.Mesh = reader.GetOutput()

    def ReadVTKMeshFile(self):
        if (self.InputFileName == ''):
            self.PrintError('Error: no InputFileName.')
        self.PrintLog('Reading VTK mesh file.')
        reader = vtk.vtkUnstructuredGridReader()
        reader.SetFileName(self.InputFileName)
        reader.Update()
        self.Mesh = reader.GetOutput()

    def ReadVTKXMLMeshFile(self):
        if (self.InputFileName == ''):
            self.PrintError('Error: no InputFileName.')
        self.PrintLog('Reading VTK XML mesh file.')
        reader = vtk.vtkXMLUnstructuredGridReader()
        reader.SetFileName(self.InputFileName)
        reader.Update()
        self.Mesh = reader.GetOutput()

    def ReadVTKXMLPMeshFile(self):
        if (self.InputFileName == ''):
            self.PrintError('Error: no InputFileName.')
        self.PrintLog('Reading VTK XML Parallel format mesh file.')
        reader = vtk.vtkXMLPUnstructuredGridReader()
        reader.SetFileName(self.InputFileName)
        reader.Update()
        self.Mesh = reader.GetOutput()

    def ReadXdaMeshFile(self):
        if (self.InputFileName == ''):
            self.PrintError('Error: no InputFileName.')
        self.PrintError('Error: Xda reader not yet implemented.')
        return
#        self.PrintLog('Reading Xda mesh file.')
#        reader = vtkvmtk.vtkvmtkXdaReader()
#        reader.SetFileName(self.InputFileName)
#        reader.Update()
#        self.Mesh = reader.GetOutput()

    def ReadFDNEUTMeshFile(self):
        if (self.InputFileName == ''):
            self.PrintError('Error: no InputFileName.')
        self.PrintLog('Reading FDNEUT mesh file.')
        reader = vtkvmtk.vtkvmtkFDNEUTReader()
        reader.SetFileName(self.InputFileName)
        reader.SetGhostNodes(self.GhostNodes)
        reader.SetSingleCellDataEntityArrayName(self.CellEntityIdsArrayName)
        reader.SetVolumeElementsOnly(self.VolumeElementsOnly)
        reader.Update()
        self.Mesh = reader.GetOutput()

    def ReadNGNEUTMeshFile(self):
        if (self.InputFileName == ''):
            self.PrintError('Error: no InputFileName.')
        self.PrintLog('Reading ngneut mesh file.')
        f=open(self.InputFileName, 'r')
        self.Mesh = vtk.vtkUnstructuredGrid()
        self.MeshPoints = vtk.vtkPoints()
        line = f.readline()
        numberOfPoints = int(line)
        self.MeshPoints.SetNumberOfPoints(numberOfPoints)
        for i in range(numberOfPoints):
            line = f.readline()
            splitLine = line.strip().split()
            point = [float(splitLine[0]),float(splitLine[1]),float(splitLine[2])]
            self.MeshPoints.SetPoint(i,point)
        self.Mesh.SetPoints(self.MeshPoints)
        self.Mesh.Allocate(numberOfPoints*4,1000)
        line = f.readline()
        numberOfVolumeCells = int(line)
        cellEntityIdArray = vtk.vtkIntArray()
        cellEntityIdArray.SetName(self.CellEntityIdsArrayName)
        for i in range(numberOfVolumeCells):
            line = f.readline()
            splitLine = line.strip().split()
            cellType = -1
            numberOfCellPoints = len(splitLine)-1
            pointIds = vtk.vtkIdList()
            if numberOfCellPoints == 4:
                cellType = 10
#                pointIds.InsertNextId(int(splitLine[0+1])-1)
#                pointIds.InsertNextId(int(splitLine[1+1])-1)
#                pointIds.InsertNextId(int(splitLine[2+1])-1)
#                pointIds.InsertNextId(int(splitLine[3+1])-1)
                pointIds.InsertNextId(int(splitLine[1+1])-1)
                pointIds.InsertNextId(int(splitLine[2+1])-1)
                pointIds.InsertNextId(int(splitLine[3+1])-1)
                pointIds.InsertNextId(int(splitLine[0+1])-1)
            elif numberOfCellPoints == 10:
                cellType = 24
#                pointIds.InsertNextId(int(splitLine[0+1])-1)
#                pointIds.InsertNextId(int(splitLine[1+1])-1)
#                pointIds.InsertNextId(int(splitLine[2+1])-1)
#                pointIds.InsertNextId(int(splitLine[3+1])-1)
#                pointIds.InsertNextId(int(splitLine[4+1])-1)
#                pointIds.InsertNextId(int(splitLine[7+1])-1)
#                pointIds.InsertNextId(int(splitLine[5+1])-1)
#                pointIds.InsertNextId(int(splitLine[6+1])-1)
#                pointIds.InsertNextId(int(splitLine[8+1])-1)
#                pointIds.InsertNextId(int(splitLine[9+1])-1)
                pointIds.InsertNextId(int(splitLine[1+1])-1)
                pointIds.InsertNextId(int(splitLine[2+1])-1)
                pointIds.InsertNextId(int(splitLine[3+1])-1)
                pointIds.InsertNextId(int(splitLine[0+1])-1)
                pointIds.InsertNextId(int(splitLine[7+1])-1)
                pointIds.InsertNextId(int(splitLine[9+1])-1)
                pointIds.InsertNextId(int(splitLine[8+1])-1)
                pointIds.InsertNextId(int(splitLine[4+1])-1)
                pointIds.InsertNextId(int(splitLine[5+1])-1)
                pointIds.InsertNextId(int(splitLine[6+1])-1)
            entityId = int(splitLine[0])
            cellEntityIdArray.InsertNextValue(entityId)
            self.Mesh.InsertNextCell(cellType,pointIds)
        if self.VolumeElementsOnly == 0:
            line = f.readline()
            numberOfSurfaceCells = int(line)
            for i in range(numberOfSurfaceCells):
                line = f.readline()
                splitLine = line.strip().split()
                cellType = -1
                numberOfCellPoints = len(splitLine)-1
                pointIds = vtk.vtkIdList()
                if numberOfCellPoints == 3:
                    cellType = 5
                    for j in range(numberOfCellPoints):
                        pointIds.InsertNextId(int(splitLine[j+1])-1)
                elif numberOfCellPoints == 6:
                    cellType = 22
                    for j in range(3):
                        pointIds.InsertNextId(int(splitLine[j+1])-1)
                    pointIds.InsertNextId(int(splitLine[5+1])-1)
                    pointIds.InsertNextId(int(splitLine[3+1])-1)
                    pointIds.InsertNextId(int(splitLine[4+1])-1)
                entityId = int(splitLine[0])
                cellEntityIdArray.InsertNextValue(entityId)
                self.Mesh.InsertNextCell(cellType,pointIds)
        self.Mesh.Squeeze()
        self.Mesh.GetCellData().AddArray(cellEntityIdArray)

    def ReadTecplotMeshFile(self):
        self.PrintLog('Reading Tecplot surface file.')
        if self.InputFileName[-3:] == '.gz':
            import gzip
            f=gzip.open(self.InputFileName, 'r')
        else:
            f=open(self.InputFileName, 'r')
        line = f.readline()
        if line.split()[0] == 'TITLE':
            line = f.readline()
        if (line.split()[0] == 'VARIABLES') | (line.split('=')[0] == 'VARIABLES'):
            arrayNames = line.split('=')[1].strip().split(',')
            arrayNames[0:3] = []
            self.PrintLog("ArrayNames" + str(arrayNames))
            line = f.readline()
        if line.split()[0] == 'ZONE':
            lineNid = line.find('N=')
            lineN = line[lineNid : lineNid+line[lineNid:].find(',') ].split('=')[1]
            numberOfNodes = int(lineN)
            lineEid = line.find('E=')
            lineE = line[lineEid : lineEid+line[lineEid:].find(',') ].split('=')[1]
            numberOfElements = int(lineE)
        self.PrintLog("Reading " + str(numberOfNodes)+" nodes.")
        points = vtk.vtkPoints()
        cells = vtk.vtkCellArray()
        points.SetNumberOfPoints(numberOfNodes)
        self.Mesh = vtk.vtkUnstructuredGrid()
        self.Mesh.SetPoints(points)
        for arrayName in arrayNames:
            array = vtk.vtkDoubleArray()
            array.SetName(arrayName)
            array.SetNumberOfTuples(numberOfNodes)
            self.Mesh.GetPointData().AddArray(array)
        data = f.read().split()
        dataCounter = 0
        for i in range(numberOfNodes):
            point = [float(data[dataCounter]),float(data[dataCounter+1]),float(data[dataCounter+2])]
            dataCounter += 3
            points.SetPoint(i,point)
            for j in range(len(arrayNames)):
                self.Mesh.GetPointData().GetArray(arrayNames[j]).SetComponent(i,0,float(data[dataCounter]))
                dataCounter += 1
        self.PrintLog("Reading " + str(numberOfElements)+" elements.")
        cellIds = vtk.vtkIdList()
        for i in range(numberOfElements):
            cellIds.Initialize()
            cellIds.InsertNextId(int(data[dataCounter])-1)
            dataCounter += 1
            cellIds.InsertNextId(int(data[dataCounter])-1)
            dataCounter += 1
            cellIds.InsertNextId(int(data[dataCounter])-1)
            dataCounter += 1
            cellIds.InsertNextId(int(data[dataCounter])-1)
            dataCounter += 1
            legal = 1
            for j in range(cellIds.GetNumberOfIds()):
                if cellIds.GetId(j) < 0:
                    legal = 0
                    break
            if not legal:
                continue
            cells.InsertNextCell(cellIds)
        self.Mesh.SetCells(10,cells)
        self.Mesh.Update()

    def Execute(self):

        extensionFormats = {'vtu':'vtkxml',
                            'vtkxml':'vtkxml',
                            'pvtu':'vtkxmlp',
                            'vtk':'vtk',
                            'FDNEUT':'fdneut',
                            'xda':'xda',
                            'neu':'ngneut',
                            'gneu':'gambit',
                            'tec':'tecplot',
                            'node':'tetgen',
                            'ele':'tetgen'}

        if self.InputFileName == 'BROWSER':
            import tkinter.filedialog
            import os.path
            initialDir = pypes.pypeScript.lastVisitedPath
            self.InputFileName = tkinter.filedialog.askopenfilename(title="Input mesh",initialdir=initialDir)
            pypes.pypeScript.lastVisitedPath = os.path.dirname(self.InputFileName)
            if not self.InputFileName:
                self.PrintError('Error: no InputFileName.')

        if self.GuessFormat and self.InputFileName and not self.Format:
            import os.path
            extension = os.path.splitext(self.InputFileName)[1]
            if extension:
                extension = extension[1:]
                if extension in list(extensionFormats.keys()):
                    self.Format = extensionFormats[extension]

        if (self.Format == 'vtk'):
            self.ReadVTKMeshFile()
        elif (self.Format == 'vtkxml'):
            self.ReadVTKXMLMeshFile()
        elif (self.Format == 'vtkxmlp'):
            self.ReadVTKXMLPMeshFile()
        elif (self.Format == 'gambit'):
            self.ReadGAMBITMeshFile()
        elif (self.Format == 'fdneut'):
            self.ReadFDNEUTMeshFile()
        elif (self.Format == 'ngneut'):
            self.ReadNGNEUTMeshFile()
        elif (self.Format == 'xda'):
            self.ReadXdaMeshFile()
        elif (self.Format == 'tecplot'):
            self.ReadTecplotMeshFile()
        elif (self.Format == 'tetgen'):
            self.ReadTetGenMeshFile()
        else:
            self.PrintError('Error: unsupported format '+ self.Format + '.')

        self.Output = self.Mesh


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
