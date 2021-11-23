#!/usr/bin/env python

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import sys
import vtk
from vmtk import vtkvmtk
from vmtk import pypes


class vmtkImageCompare(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None
        self.ReferenceImage = None
        self.Method = ''
        self.Tolerance = 1E-8
        self.Result = ''
        self.ResultLog = ''

        self.SetScriptName('vmtkimagecompare')
        self.SetScriptDoc('compares an image against a reference')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['ReferenceImage','r','vtkImageData',1,'','the reference image to compare against','vmtkimagereader'],
            ['Method','method','str',1,'["subtraction","range"]','method of the test'],
            ['Tolerance','tolerance','float',1,'','tolerance for numerical comparisons'],
            ])
        self.SetOutputMembers([
            ['Result','result','bool',1,'','Output boolean stating if images are equal or not'],
            ['ResultLog','log','str',1,'','Result Log']
            ])

    def rangeCompare(self):

        imageRange = self.Image.GetPointData().GetScalars().GetRange()
        referenceRange = self.ReferenceImage.GetPointData().GetScalars().GetRange()
        rangeDiff = (imageRange[0] - referenceRange[0], imageRange[1] - referenceRange[1])

        self.InputInfo('Image Range: '+ str(imageRange))
        self.InputInfo('Reference Image Range: '+ str(referenceRange))
        self.InputInfo('Range Difference: '+ str(rangeDiff))

        if max([abs(d) for d in rangeDiff]) < self.Tolerance:
            return True

        return False

    def subtractionCompare(self):

        imagePoints = self.Image.GetNumberOfPoints()
        referencePoints = self.ReferenceImage.GetNumberOfPoints()

        self.InputInfo('Image Points: ' + str(imagePoints))
        self.InputInfo('Reference Image Points: ' + str(referencePoints))

        if abs(imagePoints - referencePoints) > 0 :
            self.ResultLog = 'Uneven NumberOfPoints'
            return False

        imageScalarType = self.Image.GetScalarType()
        referenceScalarType = self.ReferenceImage.GetScalarType()
        if imageScalarType != referenceScalarType:
            self.PrintError('Error: Input image and reference image are not of \
                the same type. Please cast images to the same type.')

        imageMath = vtk.vtkImageMathematics()
        imageMath.SetInput1Data(self.Image)
        imageMath.SetInput2Data(self.ReferenceImage)
        imageMath.SetOperationToSubtract()
        imageMath.Update()
        differenceImage = imageMath.GetOutput()
        differenceRange = differenceImage.GetPointData().GetScalars().GetRange()

        self.InputInfo('Difference Range: ' + str(differenceRange))

        if max([abs(d) for d in differenceRange]) < self.Tolerance:
            return True

        return False

    def Execute(self):

        if not self.Image:
            self.PrintError('Error: No image.')
        if not self.ReferenceImage:
            self.PrintError('Error: No reference image.')
        if not self.Method:
            self.PrintError('Error: No method.')

        if (self.Method == 'subtraction'):
            self.Result = self.subtractionCompare()
        elif (self.Method == 'range'):
            self.Result = self.rangeCompare()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
