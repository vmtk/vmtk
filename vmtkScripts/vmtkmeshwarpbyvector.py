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

class vmtkMeshWarpByVector(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.CellEntityIdsArray = None
        self.WarpArrayName = None
        self.ScaleFactor = 1.0
        self.WarpArray = None

        self.SetScriptName('vmtkmeshwarpbyvector')
        self.SetScriptDoc('warp a mesh by a vector')
        self.SetInputMembers([
            ['Mesh','i','vtkPolyData',1,'','the input mesh','vmtkmeshreader'],
            ['WarpArrayName','vector','str',1,'','name of the vector used to warp the mesh'],
            ['ScaleFactor','scale','float',1,'','warping scaling factor']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkPolyData',1,'','the output mesh','vmtkmeshwriter']
            ])



    def Execute(self):
        if self.Mesh == None:
            self.PrintError('Error: no Mesh.')


        from vmtk import vmtkscripts

        warper = vmtkscripts.vmtkSurfaceWarpByVector()
        warper.Input = self.Mesh
        warper.WarpArrayName = self.WarpArrayName
        warper.Update()
        self.Mesh = warper.Input




if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()


