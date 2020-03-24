#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacereader.py,v $
## Language:  Python
## Date:      $Date: 2006/05/22 08:33:12 $
## Version:   $Revision: 1.13 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes


class vmtkSurfaceReader(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Format = ''
        self.GuessFormat = 1
        self.InputFileName = ''
        self.Surface = 0
        self.Output = 0

        self.SetScriptName('vmtksurfacereader')
        self.SetScriptDoc('read a surface and store it in a vtkPolyData object')
        self.SetInputMembers([
            ['Format','f','str',1,'["vtkxml","vtk","stl","ply","tecplot", "wavefront", "vtm"]','file format'],
            ['GuessFormat','guessformat','bool',1,'','guess file format from extension'],
            ['Surface','i','vtkPolyData',1,'','the input surface'],
            ['InputFileName','ifile','str',1,'','input file name']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def ReadVTKSurfaceFile(self):
        if (self.InputFileName == ''):
            self.PrintError('Error: no InputFileName.')
        self.PrintLog('Reading VTK surface file.')
        reader = vtk.vtkPolyDataReader()
        reader.SetFileName(self.InputFileName)
        reader.Update()
        self.Surface = reader.GetOutput()

    def ReadVTKXMLMultiBlockDataFile(self):
        if (self.InputFileName == ''):
            self.PrintError('Error: no InputFileName.')
        self.PrintLog('Reading MultiBlockData File.')
        reader = vtk.vtkXMLMultiBlockDataReader()
        reader.SetFileName(self.InputFileName)
        reader.Update()
        self.Surface = reader.GetOutput()

    def ReadVTKXMLSurfaceFile(self):
        if (self.InputFileName == ''):
            self.PrintError('Error: no InputFileName.')
        self.PrintLog('Reading VTK XML surface file.')
        reader = vtk.vtkXMLPolyDataReader()
        reader.SetFileName(self.InputFileName)
        reader.Update()
        self.Surface = reader.GetOutput()

    def ReadSTLSurfaceFile(self):
        if (self.InputFileName == ''):
            self.PrintError('Error: no InputFileName.')
        self.PrintLog('Reading STL surface file.')
        reader = vtk.vtkSTLReader()
        reader.SetFileName(self.InputFileName)
        reader.Update()
        self.Surface = reader.GetOutput()

    def ReadPLYSurfaceFile(self):
        if (self.InputFileName == ''):
            self.PrintError('Error: no InputFileName.')
        self.PrintLog('Reading PLY surface file.')
        reader = vtk.vtkPLYReader()
        reader.SetFileName(self.InputFileName)
        reader.Update()
        self.Surface = reader.GetOutput()

    def ReadTecplotSurfaceFile(self):
        self.PrintLog('Reading Tecplot surface file.')
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
            elementType = 'TRIANGLE'
            if line.find('ET=') != -1:
                if 'TRIANGLE' in line:
                    elementType = 'TRIANGLE'
                elif 'QUADRILATERAL' in line:
                    elementType = 'QUADRILATERAL'
        self.PrintLog("Reading " + str(numberOfNodes)+" nodes.")
        points = vtk.vtkPoints()
        cells = vtk.vtkCellArray()
        points.SetNumberOfPoints(numberOfNodes)
        self.Surface = vtk.vtkPolyData()
        self.Surface.SetPoints(points)
        self.Surface.SetPolys(cells)
        for arrayName in arrayNames:
            array = vtk.vtkDoubleArray()
            array.SetName(arrayName)
            array.SetNumberOfTuples(numberOfNodes)
            self.Surface.GetPointData().AddArray(array)
        data = f.read().split()
        dataCounter = 0
        for i in range(numberOfNodes):
            point = [float(data[dataCounter]),float(data[dataCounter+1]),float(data[dataCounter+2])]
            dataCounter += 3
            points.SetPoint(i,point)
            for j in range(len(arrayNames)):
                self.Surface.GetPointData().GetArray(arrayNames[j]).SetComponent(i,0,float(data[dataCounter]))
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
            if elementType == "QUADRILATERAL":
                cellIds.InsertNextId(int(data[dataCounter])-1)
                dataCounter += 1
            cells.InsertNextCell(cellIds)
##         self.PrintLog('Reading Tecplot surface file.')
##         f=open(self.InputFileName, 'r')
##         line = f.readline()
##         arrayNames = line.split('=')[1].strip().split(',')
##         arrayNames[0:3] = []
##         self.PrintLog("ArrayNames" + str(arrayNames))
##         line = f.readline()
##         splitLine = line.split('=')
##         numberOfNodes = int(splitLine[1].split(',')[0])
##         numberOfElements = int(splitLine[2].split(',')[0])
##         self.PrintLog("Reading " + str(numberOfNodes)+" nodes.")
##         points = vtk.vtkPoints()
##         cells = vtk.vtkCellArray()
##         points.SetNumberOfPoints(numberOfNodes)
##         self.Surface = vtk.vtkPolyData()
##         self.Surface.SetPoints(points)
##         self.Surface.SetPolys(cells)
##         for arrayName in arrayNames:
##             array = vtk.vtkDoubleArray()
##             array.SetName(arrayName)
##             array.SetNumberOfTuples(numberOfNodes)
##             self.Surface.GetPointData().AddArray(array)
##         for i in range(numberOfNodes):
##             line = f.readline()
##             splitLine = line.strip().split(' ')
##             point = [float(splitLine[0]),float(splitLine[1]),float(splitLine[2])]
##             points.SetPoint(i,point)
##             for j in range(len(arrayNames)):
##                 self.Surface.GetPointData().GetArray(arrayNames[j]).SetComponent(i,0,float(splitLine[j+3]))
##         self.PrintLog("Reading " + str(numberOfElements)+" elements.")
##         cellIds = vtk.vtkIdList()
##         for i in range(numberOfElements):
##             cellIds.Initialize()
##             line = f.readline()
##             splitLine = line.strip().split(' ')
##             cellIds.InsertNextId(int(splitLine[0])-1)
##             cellIds.InsertNextId(int(splitLine[1])-1)
##             cellIds.InsertNextId(int(splitLine[2])-1)
##             cells.InsertNextCell(cellIds)

    def ReadOBJSurfaceFile(self):
        if (self.InputFileName == ''):
            self.PrintError('Error: no InputFileName.')
        self.PrintLog('Reading wavefront surface file.')
        reader = vtk.vtkOBJReader()
        reader.SetFileName(self.InputFileName)
        reader.Update()
        self.Surface = reader.GetOutput()

    def Execute(self):

        extensionFormats = {'vtp':'vtkxml',
                            'vtkxml':'vtkxml',
                            'vtk':'vtk',
                            'stl':'stl',
                            'ply':'ply',
                            'tec':'tecplot',
                            'dat':'tecplot',
                            'obj':'wavefront',
                            'vtm':'vtm'}

        if self.InputFileName == 'BROWSER':
            import tkinter.filedialog
            import os.path
            initialDir = pypes.pypeScript.lastVisitedPath
            self.InputFileName = tkinter.filedialog.askopenfilename(title="Input surface",initialdir=initialDir)
            pypes.pypeScript.lastVisitedPath = os.path.dirname(self.InputFileName)
            if not self.InputFileName:
                self.PrintError('Error: no InputFileName.')

        if self.GuessFormat and self.InputFileName and not self.Format:
            import os.path
            extension = os.path.splitext(self.InputFileName)[1].lower()
            if extension:
                extension = extension[1:]
                if extension in list(extensionFormats.keys()):
                    self.Format = extensionFormats[extension]

        if (self.Format == 'vtk'):
            self.ReadVTKSurfaceFile()
        elif (self.Format == 'vtkxml'):
            self.ReadVTKXMLSurfaceFile()
        elif (self.Format == 'stl'):
            self.ReadSTLSurfaceFile()
        elif (self.Format == 'ply'):
            self.ReadPLYSurfaceFile()
        elif (self.Format == 'tecplot'):
            self.ReadTecplotSurfaceFile()
        elif (self.Format == 'wavefront'):
            self.ReadOBJSurfaceFile()
        elif (self.Format == 'vtm'):
            self.ReadVTKXMLMultiBlockDataFile()
        else:
            self.PrintError('Error: unsupported format '+ self.Format + '.')

        self.Output = self.Surface


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
