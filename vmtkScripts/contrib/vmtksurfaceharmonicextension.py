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


vmtksurfaceharmonicextension = 'vmtkSurfaceHarmonicExtension'

class vmtkSurfaceHarmonicExtension(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.InputArrayName = 'Displacement'
        self.OutputArrayName = 'DisplacementOut'
        self.OutputArray = None
        self.ArraySuffixes = []

        self.Ids = set()
        self.ExtensionIds = []
        self.ExcludeIds = []
        self.ExcludeIdsForBCs = []

        self.UseNullDirichletBCs = 1

        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.CellEntityIdsArray = None

        self.Display = 0
        self.vmtkRenderer = None


        self.SetScriptName('vmtksurfaceharmonicextension')
        self.SetScriptDoc('extend an input array harmonically on a surface; the extension takes effect on a subset of the input surface selected using  entity ids; the output array remains the same of the input one outside the extension domain and the values of this array at the boundary rings of the excluded domain are used as Dirichlet BCs for the harmonic extension')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['InputArrayName','inputarray','str',1,'','input array to be extended on some tags'],
            ['OutputArrayName','outputarray','str',1,'','output array name'],
            ['ArraySuffixes','arraysuffixes','str',-1,'','set of suffixes for the input/output arrays in order to process a set of arrays instead only one (if not set, only a single inputarray is processed)'],
            ['ExtensionIds','extensionids','int',-1,'','entity ids of the surface identifying the extension domain, i.e. where to extend the input array'],
            ['ExcludeIdsForBCs','excludeidsforbcs','int',-1,'','subset of extension domain to be excluded only for the boundary rings definition, where to set the Dirichlet BCs; this option only takes effect if "UseNullDirichletBCs" is true'],
            ['UseNullDirichletBCs','dirichletbcs','bool',1,'','toggle imposing homogeneous Dirichlet BCs on the rings of the extension domain; "ExcludeIdsForBCs" option can be exploited to define these rings'],
            ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, '','name of the array where entity ids have been stored'],
            ['Display','display','bool',1,'','toggle rendering'],
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

        self.CellEntityIdsArray = self.Surface.GetCellData().GetArray(self.CellEntityIdsArrayName)

        for i in range(self.Surface.GetNumberOfCells()):
            self.Ids.add(int(self.CellEntityIdsArray.GetComponent(i,0)))
        self.Ids = sorted(self.Ids)

        for item in self.Ids:
            if item not in self.ExtensionIds:
                self.ExcludeIds.append(item)


        # add indexes to input/output arrays
        inputArrayNames = []
        outputArrayNames = []
        if self.ArraySuffixes == []:
            inputArrayNames.append(self.InputArrayName)
            outputArrayNames.append(self.OutputArrayName)
        else:
            for suffix in self.ArraySuffixes:
                inputArrayNames.append(self.InputArrayName+suffix)
                outputArrayNames.append(self.OutputArrayName+suffix)

        self.PrintLog("Input arrays:  "+str(inputArrayNames))
        self.PrintLog("Output arrays: "+str(outputArrayNames))

        for i in range(len(inputArrayNames)):
            inputArray = self.Surface.GetPointData().GetArray(inputArrayNames[i])
            numberOfComponents = inputArray.GetNumberOfComponents()

            harmonicSolver = vmtkcontribscripts.vmtkSurfaceHarmonicSolver()
            harmonicSolver.Surface = self.Surface
            harmonicSolver.Interactive = False
            if numberOfComponents == 3:
                harmonicSolver.VectorialEq = True
            else:
                harmonicSolver.VectorialEq = False
            harmonicSolver.SolutionArrayName = outputArrayNames[i]
            harmonicSolver.ExcludeIds = self.ExcludeIds
            harmonicSolver.ExcludeIdsArrayName = inputArrayNames[i]
            harmonicSolver.ExcludeIdsForBCs = self.ExcludeIdsForBCs
            if self.UseNullDirichletBCs:
                harmonicSolver.InitWithZeroDirBCs = True
            else:
                harmonicSolver.InitWithZeroDirBCs = False
            harmonicSolver.CellEntityIdsArrayName = self.CellEntityIdsArrayName
            harmonicSolver.Display = self.Display
            harmonicSolver.vmtkRenderer = self.vmtkRenderer

            # extract rings between includedDomain and excludedDomain 
            [includedDomain, excludedDomain] = harmonicSolver.ExtractDomain(self.Surface,self.ExcludeIds)
            rings = harmonicSolver.ExtractBoundaries(excludedDomain) # THIS IS TO FIX
            harmonicSolver.InputRings = rings
            harmonicSolver.InputRingsBCsArrayName = inputArrayNames[i]

            harmonicSolver.Execute()
            self.Surface = harmonicSolver.Surface
        


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
