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
        self.VectorialEq = False

        self.SolutionArrayName = 'Temperature'
        self.SolutionArray = None

        self.RingsBCsArrayName = 'BCs'
        self.RingsBCsArray = None

        self.ExcludeIds = []
        self.ExcludeIdsArrayName = None
        self.ExcludeIdsArray = None

        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.CellEntityIdsArray = None

        self.vmtkRenderer = None


        self.SetScriptName('vmtksurfaceharmonicsolver')
        self.SetScriptDoc('solve a Laplace-Beltrami equation on a surface; boundary conditions can be set either interactively choosing a constant value at each boundary ring or through input rings where an array with the boundary values is defined; the equation can be solved both for a scalar or a vectorial field')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['Interactive','interactive','bool',1,'','toggle opening an interactive window to choose the constant boundary conditions at each boundary ring'],
            ['VectorialEq','vectorialeq','bool',1,'','toggle solving the scalar or the vectorial Laplace-Beltrami equation'],
            ['SolutionArrayName', 'solutionarray', 'str', 1, '','name of the point-data array where the solution is stored'],
            ['Rings','irings','vtkPolyData',1,'','the optional input rings where an array storing values to impose as BCs is defined (more than one ring can be passed, provided that they are stored all together in a unique vtkPolyData)','vmtksurfacereader'],
            ['RingsBCsArrayName', 'ringsbcsarray', 'str', 1, '','name of the point-data array where the BCs values are stored'],
            ['ExcludeIds','excludeids','int',-1,'','entity ids excluded by the solution of the equation'],
            ['ExcludeIdsArrayName', 'excludeidsarray', 'str', 1, '','name of the point-data array defined on the input surface that replaces the solution on the excluded ids'],
            ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, '','name of the cell-data array where entity ids have been stored'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])


    def InteractiveBCs(self):
        from vmtk import vmtkscripts
        # extract the boundary rings and interactively set
        # a constant BCs on it


    def Execute(self):
        from vmtk import vtkvmtk
        from vmtk import vmtkscripts
        from vmtk import vmtkcontribscripts

        if self.Surface == None:
            self.PrintError('Error: no Surface.')

        if self.Interactive:
            self.InteractiveBCs()

        if self.Rings == None:
            self.PrintError('Error: no BCs defined.')

        self.RingsBCsArray = self.Rings.GetPointData().GetArray(self.RingsBCsArrayName)

        pointLocator = vtk.vtkPointLocator()
        pointLocator.SetDataSet(self.Surface)
        pointLocator.BuildLocator()

        numberOfComponents = self.RingsBCsArray.GetNumberOfComponents()

        if self.VectorialEq and numberOfComponents!=3:
            self.PrintError('Error: vectorial equation need a three-component array as BCs.')

        if not self.VectorialEq and numberOfComponents!=1:
            self.PrintError('Error: scalar equation need a single-component array as BCs.')

        for k in range(numberOfComponents):
            print("Solving Laplace-Beltrami equation for component ",k)
            
            boundaryIds = vtk.vtkIdList()
            boundaryValues = vtk.vtkDoubleArray()
            boundaryValues.SetNumberOfComponents(1)

            for i in range(self.Rings.GetNumberOfPoints()):
                boundaryId = pointLocator.FindClosestPoint(self.Rings.GetPoint(i))
                boundaryIds.InsertNextId(boundaryId)
                boundaryValues.InsertNextTuple1(self.RingsBCsArray.GetComponent(i,k))

            # perform harmonic mapping using temperature as boundary condition
            harmonicSolver = vtkvmtk.vtkvmtkPolyDataHarmonicMappingFilter()
            harmonicSolver.SetInputData(self.Surface)
            harmonicSolver.SetHarmonicMappingArrayName(self.SolutionArrayName+str(k))
            harmonicSolver.SetBoundaryPointIds(boundaryIds)
            harmonicSolver.SetBoundaryValues(boundaryValues)
            harmonicSolver.SetAssemblyModeToFiniteElements()
            harmonicSolver.Update()

            self.Surface = harmonicSolver.GetOutput()



if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()