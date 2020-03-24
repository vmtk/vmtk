#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacewriter.py,v $
## Language:  Python
## Date:      $Date: 2006/07/27 08:27:40 $
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


class vmtkSurfaceWriter(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Format = ''
        self.GuessFormat = 1
        self.OutputFileName = ''
        self.Surface = None
        self.Input = None
        self.CellData = 0
        self.Mode = "binary"

        self.SetScriptName('vmtksurfacewriter')
        self.SetScriptDoc('write surface to disk')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['Format','f','str',1,'["vtkxml","vtk","stl","ply","pointdata","tecplot", "wavefront","vtm"]','file format'],
            ['GuessFormat','guessformat','bool',1,'','guess file format from extension'],
            ['CellData','celldata','bool',1,'','write CellData when using pointdata format'],
            ['Mode','mode','str',1,'["ascii","binary"]','write files in ASCII or binary mode'],
            ['OutputFileName','ofile','str',1,'','output file name'],
            ['OutputFileName','o','str',1,'','output file name (deprecated: use -ofile)']
            ])
        self.SetOutputMembers([])

    def WriteVTKXMLMultiBlockDataFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')

        self.PrintLog('Writing VTK XMLMultiBlock File.')
        writer = vtk.vtkXMLMultiBlockDataWriter()
        writer.SetInputData(self.Surface)
        writer.SetFileName(self.OutputFileName)
        if self.Mode == "binary":
           writer.SetDataModeToBinary()
        elif self.Mode == "ascii":
           writer.SetDataModeToAscii()
        writer.Write()

    def WriteVTKSurfaceFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing VTK surface file.')
        writer = vtk.vtkPolyDataWriter()
        writer.SetInputData(self.Surface)
        writer.SetFileName(self.OutputFileName)
        if self.Mode == "binary":
            writer.SetFileTypeToBinary()
        elif self.Mode == "ascii":
            writer.SetFileTypeToASCII()
        writer.Write()

    def WriteVTKXMLSurfaceFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing VTK XML surface file.')
        writer = vtk.vtkXMLPolyDataWriter()
        writer.SetInputData(self.Surface)
        writer.SetFileName(self.OutputFileName)
        if self.Mode == "binary":
            writer.SetDataModeToBinary()
        elif self.Mode == "ascii":
            writer.SetDataModeToAscii()
        writer.Write()

    def WriteSTLSurfaceFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing STL surface file.')
        writer = vtk.vtkSTLWriter()
        writer.SetInputData(self.Surface)
        writer.SetFileName(self.OutputFileName)
        if self.Mode == "binary":
            writer.SetFileTypeToBinary()
        elif self.Mode == "ascii":
            writer.SetFileTypeToASCII()
        writer.Write()

    def WritePLYSurfaceFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing PLY surface file.')
        writer = vtk.vtkPLYWriter()
        writer.SetInputData(self.Surface)
        writer.SetFileName(self.OutputFileName)
        if self.Mode == "binary":
            writer.SetFileTypeToBinary()
        elif self.Mode == "ascii":
            writer.SetFileTypeToASCII()
        writer.Write()

    def WritePointDataSurfaceFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing PointData file.')
        f=open(self.OutputFileName, 'w')
        line = "X Y Z"
        arrayNames = []
        dataArrays = self.Surface.GetPointData()
        if self.CellData:
            dataArrays = self.Surface.GetCellData();
        for i in range(dataArrays.GetNumberOfArrays()):
            array = dataArrays.GetArray(i)
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
        numberOfLines = self.Surface.GetNumberOfPoints()
        if self.CellData:
            numberOfLines = self.Surface.GetNumberOfCells()
        for i in range(numberOfLines):
            point = None
            if not self.CellData:
                point = self.Surface.GetPoint(i)
            else:
                point = self.Surface.GetCell(i).GetPoints().GetPoint(0)
            line = str(point[0]) + ' ' + str(point[1]) + ' ' + str(point[2])
            for arrayName in arrayNames:
                array = dataArrays.GetArray(arrayName)
                for j in range(array.GetNumberOfComponents()):
                    line = line + ' ' + str(array.GetComponent(i,j))
            line = line + '\n'
            f.write(line)

    def WriteTecplotSurfaceFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing Tecplot file.')
        triangleFilter = vtk.vtkTriangleFilter()
        triangleFilter.SetInputData(self.Surface)
        triangleFilter.PassVertsOff()
        triangleFilter.PassLinesOff()
        triangleFilter.Update()
        self.Surface = triangleFilter.GetOutput()
        f=open(self.OutputFileName, 'w')
        line = "VARIABLES = X,Y,Z"
        arrayNames = []
        for i in range(self.Surface.GetPointData().GetNumberOfArrays()):
            array = self.Surface.GetPointData().GetArray(i)
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
        line = "ZONE " + "N=" + str(self.Surface.GetNumberOfPoints()) + ',' + "E=" + str(self.Surface.GetNumberOfCells()) + ',' + "F=FEPOINT" + ',' + "ET=TRIANGLE" + '\n'
        f.write(line)
        for i in range(self.Surface.GetNumberOfPoints()):
            point = self.Surface.GetPoint(i)
            line = str(point[0]) + ' ' + str(point[1]) + ' ' + str(point[2])
            for arrayName in arrayNames:
                array = self.Surface.GetPointData().GetArray(arrayName)
                for j in range(array.GetNumberOfComponents()):
                    line = line + ' ' + str(array.GetComponent(i,j))
            line = line + '\n'
            f.write(line)
        for i in range(self.Surface.GetNumberOfCells()):
            cellPointIds = self.Surface.GetCell(i).GetPointIds()
            line = ''
            for j in range(cellPointIds.GetNumberOfIds()):
                if (j>0):
                    line = line + ' '
                line = line + str(cellPointIds.GetId(j)+1)
            line = line + '\n'
            f.write(line)

    def WriteOBJSurfaceFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing wavefront obj surface file.')
        writer = vtk.vtkOBJWriter()
        writer.SetInputData(self.Surface)
        writer.SetFileName(self.OutputFileName)
        writer.Write()

    def Execute(self):

        if self.Surface == None:
            if self.Input == None:
                self.PrintError('Error: no Surface.')
            self.Surface = self.Input

        extensionFormats = {'vtp':'vtkxml',
                            'vtkxml':'vtkxml',
                            'vtk':'vtk',
                            'stl':'stl',
                            'ply':'ply',
                            'tec':'tecplot',
                            'dat':'pointdata',
                            'obj':'wavefront',
                            'vtm':'vtm'}

        if self.OutputFileName == 'BROWSER':
            import tkinter.filedialog
            import os.path
            initialDir = pypes.pypeScript.lastVisitedPath
            self.OutputFileName = tkinter.filedialog.asksaveasfilename(title="Output surface",initialdir=initialDir)
            pypes.pypeScript.lastVisitedPath = os.path.dirname(self.OutputFileName)
            if not self.OutputFileName:
                self.PrintError('Error: no OutputFileName.')

        if self.GuessFormat and self.OutputFileName and not self.Format:
            import os.path
            extension = os.path.splitext(self.OutputFileName)[1].lower()
            if extension:
                extension = extension[1:]
                if extension in list(extensionFormats.keys()):
                    self.Format = extensionFormats[extension]

        if (self.Format == 'vtk'):
            self.WriteVTKSurfaceFile()
        elif (self.Format == 'vtkxml'):
            self.WriteVTKXMLSurfaceFile()
        elif (self.Format == 'stl'):
            self.WriteSTLSurfaceFile()
        elif (self.Format == 'ply'):
            self.WritePLYSurfaceFile()
        elif (self.Format == 'pointdata'):
            self.WritePointDataSurfaceFile()
        elif (self.Format == 'tecplot'):
            self.WriteTecplotSurfaceFile()
        elif (self.Format == 'wavefront'):
            if(vtk.vtkVersion.GetVTKMajorVersion() < 9):
                self.PrintError("Error: the VTK version installed doesn't support writing this format: " +
                                vtk.vtkVersion.GetVTKSourceVersion() + '.')
            self.WriteOBJSurfaceFile()
        elif (self.Format == 'vtm'):
            self.WriteVTKXMLMultiBlockDataFile()
        else:
            self.PrintError('Error: unsupported format '+ self.Format + '.')


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
