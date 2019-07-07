#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceclipper.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.9 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by 
##       Marco Fedele (marco.fedele@polimi.it)
##       Politecnico di Milano

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes

class vmtkSurfaceWarpByVector(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Input = None
        self.Surface = None
        self.CellEntityIdsArray = None
        self.WarpArrayName = None
        self.ScaleFactor = 1.0
        self.WarpArray = None

        self.SetScriptName('vmtksurfacewarpbyvector')
        self.SetScriptDoc('warp a surface by a vector')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['WarpArrayName','vector','str',1,'','name of the vector used to warp the surface'],
            ['ScaleFactor','scale','float',1,'','warping scaling factor']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def Update(self):
        self.WarpArray = self.Input.GetPointData().GetArray(self.WarpArrayName)
        if self.WarpArray == None:
            self.PrintError('Error: no vector called '+self.WarpArrayName+' defined on the surface')

        warper = vtk.vtkWarpVector()
        self.Input.GetPointData().SetActiveVectors(self.WarpArrayName)
        warper.SetInputData(self.Input)
        warper.SetScaleFactor(self.ScaleFactor)
        warper.Update()

        self.Input = warper.GetOutput()


    def Execute(self):
        if self.Surface == None:
            self.PrintError('Error: no Surface.')

        self.Input = self.Surface
        self.Update()
        self.Surface = self.Input




if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()


