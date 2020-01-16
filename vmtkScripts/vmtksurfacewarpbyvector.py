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



    def Execute(self):
        from vmtk import vmtkscripts

        if self.Surface == None:
            self.PrintError('Error: no Surface.')

        self.WarpArray = self.Surface.GetPointData().GetArray(self.WarpArrayName)
        if self.WarpArray == None:
            self.PrintError('Error: no vector called '+self.VectorName+' defined on the surface')

        warper = vtk.vtkWarpVector()
        self.Surface.GetPointData().SetActiveVectors(self.WarpArrayName)
        warper.SetInputData(self.Surface)
        warper.SetScaleFactor(self.ScaleFactor)
        warper.Update()

        self.Surface = warper.GetOutput()




if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()


