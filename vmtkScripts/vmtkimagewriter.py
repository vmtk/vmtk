#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimagewriter.py,v $
## Language:  Python
## Date:      $Date: 2006/07/27 08:27:40 $
## Version:   $Revision: 1.18 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

import vtk
import vtkvmtk
import sys

import pypes

vmtkimagewriter = 'vmtkImageWriter'

class vmtkImageWriter(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Format = 'meta'
        self.GuessFormat = 1
        self.OutputFileName = ''
        self.OutputRawFileName = ''
        self.OutputDirectoryName = ''
      	self.PixelRepresentation = ''
        self.Image = None
        self.Input = None
      	self.WindowLevel = [0.0, 0.0]

        self.SetScriptName('vmtkimagewriter')
        self.SetScriptDoc('write an image to disk')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'the input image','vmtkimagereader'],
            ['Format','f','str',1,'file format (vtkxml, vtk, meta image, tiff, png, point data)'],
            ['GuessFormat','guessformat','int',1,'guess file format from extension'],
            ['OutputFileName','ofile','str',1,'output file name'],
            ['OutputFileName','o','str',1,'output file name (deprecated: use -ofile)'],
            ['OutputRawFileName','rawfile','str',1,'name of the output raw file - meta image only'],
            ['OutputDirectoryName','d','str',1,'output directory name - png, tiff'],
            ['PixelRepresentation','r','str',1,'output scalar type (double, float, short)'],
            ['WindowLevel','windowlevel','float',2,'window and level for mapping graylevels to 0-255 before writing - png, tiff']
            ])
        self.SetOutputMembers([])

    def WriteVTKImageFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing VTK image file.')
        writer = vtk.vtkStructuredPointsWriter()
        writer.SetInput(self.Image)
        writer.SetFileName(self.OutputFileName)
        writer.Write()

    def WriteVTKXMLImageFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing VTK XML image file.')
        writer = vtk.vtkXMLImageDataWriter()
        writer.SetInput(self.Image)
        writer.SetFileName(self.OutputFileName)
        writer.Write()

    def WriteMetaImageFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing meta image file.')
        writer = vtk.vtkMetaImageWriter()
        writer.SetInput(self.Image)
        writer.SetFileName(self.OutputFileName)
        if (self.OutputRawFileName != ''):
            writer.SetRAWFileName(self.OutputRawFileName)
        writer.Write()

    def WritePNGImageFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing PNG image file.')
        outputImage = self.Image
        shiftScale = vtk.vtkImageShiftScale()
        shiftScale.SetInput(self.Image)
        if self.WindowLevel[0] == 0.0 and self.Image.GetScalarTypeAsString() != 'unsigned char':
            scalarRange = self.Image.GetScalarRange()
            shiftScale.SetShift(-scalarRange[0])
            shiftScale.SetScale(255.0/(scalarRange[1]-scalarRange[0]))
      	else: 
            shiftScale.SetShift(-(self.WindowLevel[1]-self.WindowLevel[0]/2.0))
            shiftScale.SetScale(255.0/self.WindowLevel[0])
        shiftScale.SetOutputScalarTypeToUnsignedChar()
        shiftScale.ClampOverflowOn()
        shiftScale.Update()
        cast = vtk.vtkImageCast()
        cast.SetInput(shiftScale.GetOutput())
        cast.SetOutputScalarTypeToUnsignedChar()
        cast.Update()
        outputImage = cast.GetOutput()
        writer = vtk.vtkPNGWriter()
        writer.SetInput(outputImage)
        if self.Image.GetDimensions()[2] == 1:
            writer.SetFileName(self.OutputFileName)
        else:
            writer.SetFilePrefix(self.OutputFileName)
            writer.SetFilePattern("%s%04d.png")
        writer.Write()

    def WriteTIFFImageFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing TIFF image file.')
        outputImage = self.Image
        shiftScale = vtk.vtkImageShiftScale()
        shiftScale.SetInput(self.Image)
        if self.WindowLevel[0] == 0.0 and self.Image.GetScalarTypeAsString() != 'unsigned char':
            scalarRange = self.Image.GetScalarRange()
            shiftScale.SetShift(-scalarRange[0])
            shiftScale.SetScale(255.0/(scalarRange[1]-scalarRange[0]))
      	else: 
            shiftScale.SetShift(-(self.WindowLevel[1]-self.WindowLevel[0]/2.0))
            shiftScale.SetScale(255.0/self.WindowLevel[0])
        shiftScale.SetOutputScalarTypeToUnsignedChar()
        shiftScale.ClampOverflowOn()
        shiftScale.Update()
        cast = vtk.vtkImageCast()
        cast.SetInput(shiftScale.GetOutput())
        cast.SetOutputScalarTypeToUnsignedChar()
        cast.Update()
        outputImage = cast.GetOutput()
        writer = vtk.vtkTIFFWriter()
        writer.SetInput(outputImage)
        if self.Image.GetDimensions()[2] == 1:
            writer.SetFileName(self.OutputFileName)
        else:
            writer.SetFilePrefix(self.OutputFileName)
            writer.SetFilePattern("%s%04d.tif")
        writer.Write()

    def WritePointDataImageFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing PointData file.')
        f=open(self.OutputFileName, 'w')
        line = "X Y Z"
        arrayNames = []
        for i in range(self.Image.GetPointData().GetNumberOfArrays()):
            array = self.Image.GetPointData().GetArray(i)
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
        for i in range(self.Image.GetNumberOfPoints()):
            point = self.Image.GetPoint(i)
            line = str(point[0]) + ' ' + str(point[1]) + ' ' + str(point[2])
            for arrayName in arrayNames:
                array = self.Image.GetPointData().GetArray(arrayName)
                for j in range(array.GetNumberOfComponents()):
                    line = line + ' ' + str(array.GetComponent(i,j))
            line = line + '\n'
            f.write(line)

    def Execute(self):

        if self.Image == None:
            if self.Input == None:
                self.PrintError('Error: no Image.')
            self.Image = self.Input

        extensionFormats = {'vti':'vtkxml', 
                            'vtkxml':'vtkxml', 
                            'vtk':'vtk',
                            'mhd':'meta',
                            'mha':'meta',
                            'tif':'tiff',
                            'png':'png',
                            'dat':'pointdata'}

        if self.GuessFormat and self.OutputFileName:
            import os.path
            extension = os.path.splitext(self.OutputFileName)[1]
            if extension:
                extension = extension[1:]
                if extension in extensionFormats.keys():
                    self.Format = extensionFormats[extension]

      	if self.PixelRepresentation != '':
            cast = vtk.vtkImageCast()
            cast.SetInput(self.Image)
            if self.PixelRepresentation == 'double':
                cast.SetOutputScalarTypeToDouble()
            elif self.PixelRepresentation == 'float':
                cast.SetOutputScalarTypeToFloat()
            elif self.PixelRepresentation == 'short':
                cast.SetOutputScalarTypeToShort()
      	    else:
                self.PrintError('Error: unsupported pixel representation '+ self.PixelRepresentation + '.')
            cast.Update()
            self.Image = cast.GetOutput()
	
        if (self.Format == 'vtkxml'):
            self.WriteVTKXMLImageFile()
        elif (self.Format == 'vtk'):
            self.WriteVTKImageFile()
        elif (self.Format == 'meta'):
            self.WriteMetaImageFile()
        elif (self.Format == 'png'):
            self.WritePNGImageFile()
        elif (self.Format == 'tiff'):
            self.WriteTIFFImageFile()
        elif (self.Format == 'pointdata'):
            self.WritePointDataImageFile()
        else:
            self.PrintError('Error: unsupported format '+ self.Format + '.')


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
