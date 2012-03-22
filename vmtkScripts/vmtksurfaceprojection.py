#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceprojection.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.6 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.


import vtk
import vtkvmtk
import sys

import pypes

vmtksurfaceprojection = 'vmtkSurfaceProjection'

class vmtkSurfaceProjection(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.ReferenceSurface = None
        self.Surface = None

        self.SetScriptName('vmtksurfaceprojection')
        self.SetScriptDoc('interpolates the point data of a reference surface onto the input surface based on minimum distance criterion')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['ReferenceSurface','r','vtkPolyData',1,'','the reference surface','vmtksurfacereader']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No Surface.')

        if self.ReferenceSurface == None:
            self.PrintError('Error: No ReferenceSurface.')

        self.InputInfo('Computing projection.')
        surfaceProjection = vtkvmtk.vtkvmtkSurfaceProjection()
        surfaceProjection.SetInput(self.Surface)
        surfaceProjection.SetReferenceSurface(self.ReferenceSurface)
        surfaceProjection.Update()
        self.Surface = surfaceProjection.GetOutput()

if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
