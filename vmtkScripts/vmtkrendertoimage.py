#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkrendertoimage.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.10 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import vmtkrenderer
from vmtk import pypes


class vmtkRenderToImage(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.vmtkRenderer = None
        self.Magnification = 1

        self.Image = None

        self.SetScriptName('vmtkrendertoimage')
        self.SetScriptDoc('takes a renderer in input and saves the rendering into an image file')
        self.SetInputMembers([
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','the input renderer'],
            ['Magnification','magnification','int',1,'','the magnification factor relative to the rendering window']])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter']
            ])

    def Execute(self):

        if not self.vmtkRenderer:
            self.PrintError('Error: no Renderer.')

        windowToImage = vtk.vtkWindowToImageFilter()
        windowToImage.SetInput(self.vmtkRenderer.RenderWindow)
        windowToImage.SetMagnification(self.Magnification)
        windowToImage.Update()
        self.vmtkRenderer.RenderWindow.Render()

        self.Image = windowToImage.GetOutput()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
