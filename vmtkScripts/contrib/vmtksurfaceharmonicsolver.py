#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceviewer.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.10 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

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


vmtksurfaceharmonicsolver = 'vmtkSurfaceHarmonicSolver'

class vmtkSurfaceHarmonicSolver(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Rings = None

        self.Interactive = True

        self.RingsBCsArray = 'BCs'

        self.ExcludeIds = []
        self.ExcludeIdsArray = None

        self.CellEntityIdsArrayName = 'CellEntityIds'

        self.vmtkRenderer = None


        self.SetScriptName('vmtksurfaceharmonicsolver')
        self.SetScriptDoc('solve a Laplace-Beltrami equation on a surface; boundary conditions can be set either interactively choosing a constant value at each boundary ring or through input rings where an array with the boundary values is defined')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['Interactive','interactive','bool',1,'','toggle opening an interactive window to choose the constant boundary conditions at each boundary ring'],
            ['Rings','irings','vtkPolyData',1,'','the optional input rings where an array storing values to impose as BCs is defined (more than one ring can be passed, provided that they are stored all together in a unique vtkPolyData)','vmtksurfacereader'],
            ['RingsBCsArray', 'ringsbcsarray', 'str', 1, '','name of the array where the BCs values are stored'],
            ['ExcludeIds','excludeids','int',-1,'','entity ids excluded by the solution of the equation'],
            ['ExcludeIdsArray', 'excludeidsarray', 'str', 1, '','name of the array defined on the input surface that replaces the solution on the excluded ids'],
            ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, '','name of the array where entity ids have been stored'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])


    def Execute(self):
        from vmtk import vtkvmtk
        from vmtk import vmtkscripts
        from vmtk import vmtkcontribscripts

        if self.Surface == None:
            self.PrintError('Error: no Surface.')



if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()