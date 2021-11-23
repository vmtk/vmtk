#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimagewriter.py,v $
## Language:  Python
## Date:      $Date: 2006/07/27 08:27:40 $
## Version:   $Revision: 1.18 $

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


class vmtkImageWriter(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Format = ''
        self.GuessFormat = 1
        self.UseITKIO = 1
        self.ApplyTransform = 0
        self.OutputFileName = ''
        self.OutputRawFileName = ''
        self.OutputDirectoryName = ''
        self.PixelRepresentation = ''
        self.Image = None
        self.Input = None
        self.WindowLevel = [1.0, 0.0]
        self.RasToIjkMatrixCoefficients = None #[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]

        self.SetScriptName('vmtkimagewriter')
        self.SetScriptDoc('write an image to disk')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['Format','f','str',1,'["vtkxml","vtk","meta","tiff","png","pointdata", "vtsxml"]','file format'],
            ['GuessFormat','guessformat','bool',1,'','guess file format from extension'],
            ['UseITKIO','useitk','bool',1,'','use ITKIO mechanism'],
            ['ApplyTransform','transform','bool',1,'','apply transform on writing - ITKIO only'],
            ['OutputFileName','ofile','str',1,'','output file name'],
            ['OutputFileName','o','str',1,'','output file name (deprecated: use -ofile)'],
            ['OutputRawFileName','rawfile','str',1,'','name of the output raw file - meta image only'],
            ['OutputDirectoryName','d','str',1,'','output directory name - png, tiff'],
            ['PixelRepresentation','r','str',1,'["double","float","short"]','output scalar type'],
            ['WindowLevel','windowlevel','float',2,'','window and level for mapping graylevels to 0-255 before writing - png, tiff'],
            ['RasToIjkMatrixCoefficients','matrix','float',16]
            ])
        self.SetOutputMembers([])

    def WriteVTKImageFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing VTK image file.')
        writer = vtk.vtkStructuredPointsWriter()
        writer.SetInputData(self.Image)
        writer.SetFileName(self.OutputFileName)
        writer.Write()

    def WriteVTSXMLVolumeFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing VTS XML grid file.')
        if self.ApplyTransform == 0:
            origin = self.Image.GetOrigin()
            spacing = self.Image.GetSpacing()
            matrix = vtk.vtkMatrix4x4()
            matrix.DeepCopy((1/spacing[0], 0, 0, - origin[0]/spacing[0],
                         0, 1/spacing[1], 0, - origin[1]/spacing[1],
                         0, 0, 1/spacing[2], - origin[2]/spacing[2],
                         0, 0, 0, 1)) #LPI convention with correct origin and spacing
        else:
            if self.RasToIjkMatrixCoefficients == None:
                self.PrintError('Error: no RasToIjkMatrixCoefficients.')
            matrix = vtk.vtkMatrix4x4()
            matrix.DeepCopy(self.RasToIjkMatrixCoefficients)
        trans = vtk.vtkTransform()
        trans.SetMatrix(matrix)
        trans_filt = vtk.vtkTransformFilter()
        trans_filt.SetTransform(trans)
        trans_filt.SetInputData(self.Image)
        trans_filt.Update()
        writer = vtk.vtkXMLStructuredGridWriter()
        writer.SetInputConnection(trans_filt.GetOutputPort())
        writer.SetFileName(self.OutputFileName)
        writer.Write()

    def WriteVTKXMLImageFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing VTK XML image file.')
        writer = vtk.vtkXMLImageDataWriter()
        writer.SetInputData(self.Image)
        writer.SetFileName(self.OutputFileName)
        writer.Write()

    def WriteMetaImageFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing meta image file.')
        writer = vtk.vtkMetaImageWriter()
        writer.SetInputData(self.Image)
        writer.SetFileName(self.OutputFileName)
        if (self.OutputRawFileName != ''):
            writer.SetRAWFileName(self.OutputRawFileName)
        writer.Write()

    def WritePNGImageFile(self):
        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing PNG image file.')
        outputImage = self.Image
        if self.Image.GetScalarTypeAsString() != 'unsigned char':
            shiftScale = vtk.vtkImageShiftScale()
            shiftScale.SetInputData(self.Image)
            if self.WindowLevel[0] == 0.0:
                scalarRange = self.Image.GetScalarRange()
                shiftScale.SetShift(-scalarRange[0])
                shiftScale.SetScale(255.0/(scalarRange[1]-scalarRange[0]))
            else:
                shiftScale.SetShift(-(self.WindowLevel[1]-self.WindowLevel[0]/2.0))
                shiftScale.SetScale(255.0/self.WindowLevel[0])
            shiftScale.SetOutputScalarTypeToUnsignedChar()
            shiftScale.ClampOverflowOn()
            shiftScale.Update()
            outputImage = shiftScale.GetOutput()
        writer = vtk.vtkPNGWriter()
        writer.SetInputData(outputImage)
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
        if self.Image.GetScalarTypeAsString() != 'unsigned char':
            shiftScale = vtk.vtkImageShiftScale()
            shiftScale.SetInputData(self.Image)
            if self.WindowLevel[0] == 0.0:
                scalarRange = self.Image.GetScalarRange()
                shiftScale.SetShift(-scalarRange[0])
                shiftScale.SetScale(255.0/(scalarRange[1]-scalarRange[0]))
            else:
                shiftScale.SetShift(-(self.WindowLevel[1]-self.WindowLevel[0]/2.0))
                shiftScale.SetScale(255.0/self.WindowLevel[0])
            shiftScale.SetOutputScalarTypeToUnsignedChar()
            shiftScale.ClampOverflowOn()
            shiftScale.Update()
            outputImage = shiftScale.GetOutput()
        writer = vtk.vtkTIFFWriter()
        writer.SetInputData(outputImage)
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
        if self.Image.GetPointData().GetScalars().GetName() == None:
            self.Image.GetPointData().GetScalars().SetName('__Scalars')
        for i in range(self.Image.GetPointData().GetNumberOfArrays()):
            array = self.Image.GetPointData().GetArray(i)
            arrayName = array.GetName()
            if arrayName == None:
                continue
            if arrayName[-1]=='_':
                continue
            arrayNames.append(arrayName)
            if array.GetNumberOfComponents() == 1:
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

    def WriteITKIO(self):
        if self.OutputFileName == '':
            self.PrintError('Error: no OutputFileName.')
        writer = vtkvmtk.vtkvmtkITKImageWriter()
        writer.SetInputData(self.Image)
        writer.SetFileName(self.OutputFileName)
        writer.SetUseCompression(1)
        if self.ApplyTransform == 0:
            origin = self.Image.GetOrigin()
            spacing = self.Image.GetSpacing()
            matrix = vtk.vtkMatrix4x4()
            matrix.DeepCopy((1/spacing[0], 0, 0, - origin[0]/spacing[0],
                         0, 1/spacing[1], 0, - origin[1]/spacing[1],
                         0, 0, 1/spacing[2], - origin[2]/spacing[2],
                         0, 0, 0, 1)) #LPI convention with correct origin and spacing
        else:
            if self.RasToIjkMatrixCoefficients == None:
                self.PrintError('Error: no RasToIjkMatrixCoefficients.')
            matrix = vtk.vtkMatrix4x4()
            matrix.DeepCopy(self.RasToIjkMatrixCoefficients)
        writer.SetRasToIJKMatrix(matrix)
        writer.Write()

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
                            'dat':'pointdata',
                            'vts':'vtsxml' }

        if self.OutputFileName == 'BROWSER':
            import tkinter.filedialog
            import os.path
            initialDir = pypes.pypeScript.lastVisitedPath
            self.OutputFileName = tkinter.filedialog.asksaveasfilename(title="Output image",initialdir=initialDir)
            pypes.pypeScript.lastVisitedPath = os.path.dirname(self.OutputFileName)
            if not self.OutputFileName:
                self.PrintError('Error: no OutputFileName.')

        if self.OutputDirectoryName == 'BROWSER':
            import tkinter.filedialog
            initialDir = pypes.pypeScript.lastVisitedPath
            self.OutputDirectoryName = tkinter.filedialog.askdirectory(title="Output directory",initialdir=initialDir)
            pypes.pypeScript.lastVisitedPath = self.OutputDirectoryName
            if not self.OutputDirectoryName:
                self.PrintError('Error: no OutputDirectoryName.')

        if self.GuessFormat and self.OutputFileName and not self.Format:
            import os.path
            extension = os.path.splitext(self.OutputFileName)[1]
            if extension:
                extension = extension[1:]
                if extension in list(extensionFormats.keys()):
                    self.Format = extensionFormats[extension]

        if self.PixelRepresentation != '':
            cast = vtk.vtkImageCast()
            cast.SetInputData(self.Image)
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

        if self.UseITKIO and self.Format not in ['vtkxml','vtk','tiff','png','dat', 'vtsxml']:
            self.WriteITKIO()
        else:
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
            elif (self.Format == 'vtsxml'):
                self.WriteVTSXMLVolumeFile()
            else:
                self.PrintError('Error: unsupported format '+ self.Format + '.')


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
