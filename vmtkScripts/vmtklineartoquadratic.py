#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtklineartoquadratic.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.7 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.


import vtk
import vtkvmtk
import sys

import pypes

vmtklineartoquadratic = 'vmtkLinearToQuadratic'

class vmtkLinearToQuadratic(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Mesh = None
        self.Mode = 'volume'
        self.SubdivisionMethod = 'linear'
        self.UseBiquadraticWedge = True

        self.SetScriptName('vmtklineartoquadratic')
        self.SetScriptDoc('convert the elements of a mesh from linear to quadratic')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['Mode','mode','str',1,'["volume","surface"]','kind of elements to work with'],
            ['UseBiquadraticWedge','biquadraticwedge','bool',1,'','if on, convert linear wedges to 18-noded biquadratic quadratic wedges, otherwise use 15-noded quadratic wedges'],
            ['SubdivisionMethod','subdivisionmethod','str',1,'["linear","butterfly"]','subdivision method for surface elements']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']])

    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: No input mesh.')

        linearToQuadraticFilter = None

        if self.Mode == 'volume':
            linearToQuadraticFilter = vtkvmtk.vtkvmtkLinearToQuadraticMeshFilter()
            linearToQuadraticFilter.SetUseBiquadraticWedge(self.UseBiquadraticWedge)
        elif self.Mode == 'surface':
            linearToQuadraticFilter = vtkvmtk.vtkvmtkLinearToQuadraticSurfaceMeshFilter()
            if self.SubdivisionMethod == 'linear':
                linearToQuadraticFilter.SetSubdivisionMethodToLinear()
            elif self.SubdivisionMethod == 'butterfly':
                linearToQuadraticFilter.SetSubdivisionMethodToButterfly()
            else:
                self.PrintError('Unsupported subdivision method.')
        else:
            self.PrintError('Unsupported mode.')

        linearToQuadraticFilter.SetInput(self.Mesh)
        linearToQuadraticFilter.Update()

        self.Mesh = linearToQuadraticFilter.GetOutput()

        if self.Mesh.GetSource():
            self.Mesh.GetSource().UnRegisterAllOutputs()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
