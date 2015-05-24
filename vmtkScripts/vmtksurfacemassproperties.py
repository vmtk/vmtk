#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacemassproperties.py,v $
## Language:  Python
## Date:      $Date: 2006/07/17 09:53:14 $
## Version:   $Revision: 1.8 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.


import sys
import vtk

import pypes

vmtksurfacemassproperties = 'vmtkSurfaceMassProperties'

class vmtkSurfaceMassProperties(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Surface = None

        self.SurfaceArea = 0.0
        self.Volume = 0.0
        self.ShapeIndex = 0.0

        self.SetScriptName('vmtksurfacemassproperties')
        self.SetScriptDoc('compute the volume of a closed surface.')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader']
            ])
        self.SetOutputMembers([
            ['SurfaceArea','area','float',1,''],
            ['Volume','volume','float',1,''],
            ['ShapeIndex','shape','float',1,'']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        cleaner = vtk.vtkCleanPolyData()
        cleaner.SetInputData(self.Surface)
        cleaner.Update()

        triangleFilter = vtk.vtkTriangleFilter()
        triangleFilter.SetInputConnection(cleaner.GetOutputPort())
        triangleFilter.Update()

        massProps = vtk.vtkMassProperties()
        massProps.SetInputConnection(triangleFilter.GetOutputPort())
        massProps.Update()

        self.SurfaceArea = massProps.GetSurfaceArea()
        self.Volume = massProps.GetVolume()
        self.ShapeIndex = massProps.GetNormalizedShapeIndex()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()

