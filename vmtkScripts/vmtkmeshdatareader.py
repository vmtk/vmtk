#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshdatareader.py,v $
## Language:  Python
## Date:      $Date: 2006/07/27 08:27:40 $
## Version:   $Revision: 1.13 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import sys
import os
import gzip
import vtk
from vmtk import vtkvmtk

from vmtk import pypes


class vmtkMeshDataReader(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.TetrInFileName = ''
        self.Mesh = 0

        self.DataFileDirectory = '.'
        self.DataFilePrefix = ''
        self.DataFileType = 'tetr.vel'

        self.DataFileName = ''

        self.ArrayNamePrefix = ''

        self.Compressed = 1
        self.UnNormalize = 1

        self.MinVariableId = -1
        self.MaxVariableId = -1

        self.Radius = 1.0
        self.Viscosity = 1.0
        self.Density = 1.0
        self.ReD = 0.0
        self.Alpha = 0.0

        self.SetScriptName('vmtkmeshdatareader')
        self.SetScriptDoc('read data associated with a mesh')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['DataFileDirectory','directory','str',1,'','directory where the data files reside'],
            ['DataFilePrefix','prefix','str',1,'','data file name prefix (e.g. foo_)'],
            ['DataFileName','datafile','str',1,'','data file name (e.g. foo.dat) excluding directory - overrides prefix'],
            ['DataFileType','filetype','str',1,'','data file name type (tetr.out, tetr.vel, tetr.wss, tetr.pres, tetr.ini, pointdata)'],
            ['ArrayNamePrefix','arrayprefix','str',1,'','prefix to prepend to array names in output'],
            ['TetrInFileName','tetrinfile','str',1,'','name of the tetr.in file'],
            ['Compressed','compressed','bool',1,'','toggle reading gzip compressed file'],
            ['UnNormalize','unnormalize','bool',1,'','unnormalize quantities - tetr.* only'],
            ['MinVariableId','minvarid','int',1,'(0,)','read variables starting from the nth column - pointdata only'],
            ['MaxVariableId','maxvarid','int',1,'(0,)','read variables up to the nth column - pointdata only']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']
            ])

    def ReadTetrInDimensionalParameters(self):

        tetrinf=open(self.TetrInFileName, 'r')
        tetrinline = ''
        while tetrinline.strip() != '$radius':
            tetrinline = tetrinf.readline()
            if tetrinline == '':
                break
        if tetrinline.strip() == '$radius':
            tetrinline = tetrinf.readline()
            self.Radius = float(tetrinline.strip())

        tetrinf.seek(0)
        tetrinline = ''
        while tetrinline.strip() != '$viscosity':
            tetrinline = tetrinf.readline()
            if tetrinline == '':
                break
        if tetrinline.strip() == '$viscosity':
            tetrinline = tetrinf.readline()
            self.Viscosity = float(tetrinline.strip())

        tetrinf.seek(0)
        tetrinline = ''
        while tetrinline.strip() != '$density':
            tetrinline = tetrinf.readline()
            if tetrinline == '':
                break
        if tetrinline.strip() == '$density':
            tetrinline = tetrinf.readline()
            self.Density = float(tetrinline.strip())

        tetrinf.seek(0)
        tetrinline = ''
        while tetrinline.strip() != '$red':
            tetrinline = tetrinf.readline()
            if tetrinline == '':
                break
        if tetrinline.strip() == '$red':
            tetrinline = tetrinf.readline()
            self.ReD = float(tetrinline.strip())
        else:
            tetrinf.seek(0)
            while tetrinline.strip() != '$rer':
                tetrinline = tetrinf.readline()
                if tetrinline == '':
                    break
            if tetrinline.strip() == '$rer':
                tetrinline = tetrinf.readline()
                self.ReD = float(tetrinline.strip())*2.0

        tetrinf.seek(0)
        tetrinline = ''
        while tetrinline.strip() != '$alpha':
            tetrinline = tetrinf.readline()
            if tetrinline == '':
                break
        if tetrinline.strip() == '$alpha':
            tetrinline = tetrinf.readline()
            self.Alpha = float(tetrinline.strip())

    def ReadTetrOutFile(self,filename,arrayname):
        self.PrintLog('Reading '+filename+'.')
        if self.UnNormalize == 1:
            self.ReadTetrInDimensionalParameters()
        f = None
        if (self.Compressed == 1):
            f=gzip.open(filename, 'r')
        else:
            f=open(filename, 'r')
        line = f.readline()
        while (line!='$output'):
            line = f.readline()
        line = f.readline()
        splitline = line.split(' ')
        numberOfTuples = int(splitline[0])
        outputArray = vtk.vtkDoubleArray()
        outputArray.SetName(arrayname)
        outputArray.SetNumberOfComponents(3)
        outputArray.SetNumberOfTuples(numberOfTuples)
        velocityUnNormalizationFactor = self.ReD / (2.0 * self.Radius) * self.Viscosity / self.Density
        for i in range(numberOfTuples):
            line = f.readline()
            splitline = line.split(' ')
            value0 = float(splitline[5])
            value1 = float(splitline[6])
            value2 = float(splitline[7])
            if self.UnNormalize ==1:
                value0 *= velocityUnNormalizationFactor
                value1 *= velocityUnNormalizationFactor
                value2 *= velocityUnNormalizationFactor
            outputArray.SetComponent(i,0,value0)
            outputArray.SetComponent(i,1,value1)
            outputArray.SetComponent(i,2,value2)
        self.Mesh.GetPointData().AddArray(outputArray)

    def ReadTetrVelFile(self,filename,arrayname):
        self.PrintLog('Reading '+filename+'.')
        if self.UnNormalize == 1:
            self.ReadTetrInDimensionalParameters()
        if (self.Compressed == 1):
            f=gzip.open(filename, 'r')
        else:
            f=open(filename, 'r')
        lines = f.readlines()
        line = lines[0]
        lineoffset = 1
        splitline = line.split(' ')
        numberOfTuples = int(splitline[0])
        iteration = float(splitline[1])
        outputArray = vtk.vtkDoubleArray()
        outputArray.SetName(arrayname)
        outputArray.SetNumberOfComponents(3)
        outputArray.SetNumberOfTuples(numberOfTuples)
        velocityUnNormalizationFactor = self.ReD / (2.0 * self.Radius) * self.Viscosity / self.Density
        for i in range(numberOfTuples):
            line = lines[i+lineoffset]
            splitline = line.split(' ')
            value0 = float(splitline[0])
            value1 = float(splitline[1])
            value2 = float(splitline[2])
            if self.UnNormalize ==1:
                value0 *= velocityUnNormalizationFactor
                value1 *= velocityUnNormalizationFactor
                value2 *= velocityUnNormalizationFactor
            outputArray.SetComponent(i,0,value0)
            outputArray.SetComponent(i,1,value1)
            outputArray.SetComponent(i,2,value2)
        self.Mesh.GetPointData().AddArray(outputArray)

    def ReadTetrWSSFile(self,filename,arrayname):
        self.PrintLog('Reading '+filename+'.')
        if self.UnNormalize == 1:
            self.ReadTetrInDimensionalParameters()
        if (self.Compressed == 1):
            f=gzip.open(filename, 'r')
        else:
            f=open(filename, 'r')
        lines = f.readlines()
        line = lines[1]
        lineoffset = 2
        numberOfTuples = int(line)
        numberOfPoints = self.Mesh.GetNumberOfPoints()
        outputArray = vtk.vtkDoubleArray()
        outputArray.SetName(arrayname)
        outputArray.SetNumberOfComponents(3)
        outputArray.SetNumberOfTuples(numberOfPoints)
        outputArray.FillComponent(0,0.0)
        outputArray.FillComponent(1,0.0)
        outputArray.FillComponent(2,0.0)
        tetrinf=open(self.TetrInFileName, 'r')
        tetrinline = ''
        while tetrinline.strip() != '$wnode':
            tetrinline = tetrinf.readline()
        tetrinline = tetrinf.readline()
        wssUnNormalizationFactor = 4.0 * self.Viscosity * (self.ReD / (2.0 * self.Radius) * self.Viscosity / self.Density) / self.Radius
        for i in range(numberOfTuples):
            line = lines[i+lineoffset]
            splitline = line.split(' ')
            value0 = float(splitline[0])
            value1 = float(splitline[1])
            value2 = float(splitline[2])
            if self.UnNormalize ==1:
                value0 *= wssUnNormalizationFactor
                value1 *= wssUnNormalizationFactor
                value2 *= wssUnNormalizationFactor
            tetrinline = tetrinf.readline()
            pointId = int(tetrinline.strip().split(' ')[1]) - 1
            outputArray.SetComponent(pointId,0,value0)
            outputArray.SetComponent(pointId,1,value1)
            outputArray.SetComponent(pointId,2,value2)
        self.Mesh.GetPointData().AddArray(outputArray)

    def ReadTetrIniFile(self,filename,arrayname):
        self.PrintLog('Reading '+filename+'.')
        if (self.Compressed == 1):
            f=gzip.open(filename, 'r')
        else:
            f=open(filename, 'r')
        line = f.readline()
        while (line!='$vel_old1'):
            line = f.readline()
        line = f.readline()
        line = f.readline()
        numberOfTuples = int(line)
        outputArray = vtk.vtkDoubleArray()
        outputArray.SetName(arrayname)
        outputArray.SetNumberOfComponents(3)
        outputArray.SetNumberOfTuples(numberOfTuples)
        for i in range(numberOfTuples):
            line = f.readline()
            splitline = line.split(' ')
            value0 = float(splitline[0])
            value1 = float(splitline[1])
            value2 = float(splitline[2])
            outputArray.SetComponent(i,0,value0)
            outputArray.SetComponent(i,1,value1)
            outputArray.SetComponent(i,2,value2)
        self.Mesh.GetPointData().AddArray(outputArray)

    def ReadPointDataFile(self,filename):
        self.PrintLog('Reading '+filename+'.')
        if (self.Compressed == 1):
            f=gzip.open(filename, 'r')
        else:
            f=open(filename, 'r')
        lines = f.readlines()
        line = lines[0]
        splitline = line.strip().split(' ')
        numberOfVariables = len(splitline)
        numberOfPoints = self.Mesh.GetNumberOfPoints()
        arrayNames = []
        for i in range(numberOfVariables):
            arrayNames.append(filename+'_'+splitline[i])
            if (self.MinVariableId != -1) & (i < self.MinVariableId):
                continue
            if (self.MaxVariableId != -1) & (i > self.MaxVariableId):
                continue
            outputArray = vtk.vtkDoubleArray()
            outputArray.SetName(arrayNames[i])
            outputArray.SetNumberOfComponents(1)
            outputArray.SetNumberOfTuples(numberOfPoints)
            self.Mesh.GetPointData().AddArray(outputArray)
        pointId = 0
        for i in range(1,len(lines)):
            line = lines[i]
            splitline = line.strip().split(' ')
            if len(splitline) != numberOfVariables:
                self.PrintError('Error: pointdata file corrupted.');
            for j in range(numberOfVariables):
                if (self.MinVariableId != -1) & (i < self.MinVariableId):
                    continue
                if (self.MaxVariableId != -1) & (i > self.MaxVariableId):
                    continue
                self.Mesh.GetPointData().GetArray(arrayNames[j]).SetComponent(pointId,0,float(splitline[j]))
            pointId += 1

    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: No input mesh.')

        dataFilenames = []

        if not self.DataFileName:
            if self.DataFilePrefix == '':
                self.DataFilePrefix = self.DataFileType
            filenames = os.listdir(self.DataFileDirectory)
            for filename in filenames:
                if filename.find(self.DataFilePrefix) != -1:
                    dataFilenames.append(filename)
            dataFilenames.sort()
        else:
            dataFilenames.append(self.DataFileName)

        for name in dataFilenames:
            filename = self.DataFileDirectory + '/' + name
            arrayname = self.ArrayNamePrefix + name
            if (self.DataFileType == 'tetr.out'):
                self.ReadTetrOutFile(filename,arrayname)
            elif (self.DataFileType == 'tetr.vel'):
                self.ReadTetrVelFile(filename,arrayname)
            elif (self.DataFileType == 'tetr.wss'):
                self.ReadTetrWSSFile(filename,arrayname)
            elif (self.DataFileType == 'tetr.ini'):
                self.ReadTetrIniFile(filename,arrayname)
            elif (self.DataFileType == 'pointdata'):
                if self.UnNormalize == 1:
                    self.PrintError('Error: UnNormalize not supported for pointdata DataFileType.')
                self.ReadPointDataFile(filename)
            else:
                self.PrintError('Unsupported DataFileType.')


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
