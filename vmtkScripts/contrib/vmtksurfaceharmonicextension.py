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
        self.InputArrayNames = []
        self.OutputArrayNames = []
        self.OutputArray = None

        self.Ids = set()
        self.ExtensionIds = []
        self.ExcludeIds = []
        self.ExcludeIdsForBCs = []

        self.UseNullDirichletBCs = 1

        self.Valve = None
        self.ValveDistanceThreshold = 1

        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.CellEntityIdsArray = None

        self.Display = 0
        self.vmtkRenderer = None


        self.SetScriptName('vmtksurfaceharmonicextension')
        self.SetScriptDoc('extend point data arrays harmonically on a surface; the extension takes effect on a subset of the input surface selected using  entity ids; the output array remains the same of the input one outside the extension domain and the values of this array at the boundary rings of the excluded domain are used as Dirichlet BCs for the harmonic extension')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['Valve','ivalve','vtkPolyData',1,'','an optional additional surface near the input surface (e.g. a valve) where the extension is also performed','vmtksurfacereader'],
            ['InputArrayNames','iarrays','str',1,'','input arrays to be extended (if not set, all the point data arrays are extended)'],
            ['OutputArrayNames','oarrays','str',1,'','output arrays (if not set, output arrays overwrite input ones)'],
            ['ExtensionIds','extensionids','int',-1,'','entity ids of the surface identifying the extension domain, i.e. where to extend the input array'],
            ['ExcludeIdsForBCs','excludeidsforbcs','int',-1,'','subset of extension domain to be excluded only for the boundary rings definition, where to set the Dirichlet BCs; this option only takes effect if "UseNullDirichletBCs" is true'],
            ['UseNullDirichletBCs','dirichletbcs','bool',1,'','toggle imposing homogeneous Dirichlet BCs on the rings of the extension domain; "ExcludeIdsForBCs" option can be exploited to define these rings'],
            ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, '','name of the array where entity ids have been stored'],
            ['ValveDistanceThreshold','valvedistancethreshold','float',1,'(0,)','if Valve is set, distance threshold from the input surface used to set the region where to impose a Dirichlet BC on it'],
            ['Display','display','bool',1,'','toggle rendering'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ['Valve','ovalve','vtkPolyData',1,'','the optional additional surface (e.g. a valve)','vmtksurfacewriter']
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

        # set input/output arrays
        if self.InputArrayNames == []:
            for i in range(self.Surface.GetPointData().GetNumberOfArrays()):
                self.InputArrayNames.append(self.Surface.GetPointData().GetArrayName(i))

        if self.OutputArrayNames == []:
            self.OutputArrayNames = self.InputArrayNames
        elif len(self.OutputArrayNames)!=len(self.InputArrayNames):
            self.PrintError('Different number of input and output arrays')

        self.PrintLog("Input arrays:  "+str(self.InputArrayNames))
        self.PrintLog("Output arrays: "+str(self.OutputArrayNames))

        for i in range(len(self.InputArrayNames)):
            self.PrintLog('\nProcessing array "'+self.InputArrayNames[i]+'"\n')
            inputArray = self.Surface.GetPointData().GetArray(self.InputArrayNames[i])
            numberOfComponents = inputArray.GetNumberOfComponents()

            harmonicSolver = vmtkcontribscripts.vmtkSurfaceHarmonicSolver()
            harmonicSolver.Surface = self.Surface
            harmonicSolver.Interactive = False
            if numberOfComponents == 3:
                harmonicSolver.VectorialEq = True
            else:
                harmonicSolver.VectorialEq = False
            harmonicSolver.SolutionArrayName = self.OutputArrayNames[i]
            harmonicSolver.ExcludeIds = self.ExcludeIds
            harmonicSolver.ExcludeIdsArrayName = self.InputArrayNames[i]
            harmonicSolver.ExcludeIdsForBCs = self.ExcludeIdsForBCs
            if self.UseNullDirichletBCs:
                harmonicSolver.InitWithZeroDirBCs = True
            else:
                harmonicSolver.InitWithZeroDirBCs = False
            harmonicSolver.CellEntityIdsArrayName = self.CellEntityIdsArrayName
            harmonicSolver.Display = self.Display
            harmonicSolver.vmtkRenderer = self.vmtkRenderer

            # extract rings between includedDomain and excludedDomain
            # using distance between the two sets of boundary rings
            [includedDomain, excludedDomain] = harmonicSolver.ExtractDomain(self.Surface,self.ExcludeIds)
            excludedDomainRings = harmonicSolver.ExtractBoundaries(excludedDomain)
            includedDomainRings = harmonicSolver.ExtractBoundaries(includedDomain)

            distance = vmtkscripts.vmtkSurfaceDistance()
            distance.Surface = excludedDomainRings
            distance.ReferenceSurface = includedDomainRings
            distance.DistanceArrayName = 'distanceFromIncludedRings'
            distance.Execute()
            excludedDomainRings = distance.Surface

            ringsBetween = harmonicSolver.SurfaceThreshold(excludedDomainRings,0,0,'distanceFromIncludedRings',False)

            harmonicSolver.InputRings = ringsBetween
            harmonicSolver.InputRingsBCsArrayName = self.InputArrayNames[i]

            harmonicSolver.Execute()
            self.Surface = harmonicSolver.Surface


        if self.Valve != None:

            print("\n\n\nsolving for valve\n\n\n")

            computeDistance = vmtkscripts.vmtkSurfaceDistance()
            computeDistance.Surface = self.Valve
            computeDistance.ReferenceSurface = self.Surface
            computeDistance.DistanceArrayName = 'DistanceFromSurface'
            computeDistance.Execute()
            self.Valve = computeDistance.Surface

            threshold = vmtkcontribscripts.vmtkThreshold()
            threshold.Surface = self.Valve
            threshold.ArrayName = 'DistanceFromSurface'
            threshold.CellData = 0
            threshold.LowThreshold = -1 # distance is positive
            threshold.HighThreshold = self.ValveDistanceThreshold
            threshold.Execute()
            valveForBCs = threshold.Surface

            projector = vmtkscripts.vmtkSurfaceProjection()
            projector.Surface = valveForBCs
            projector.ReferenceSurface = self.Surface
            projector.Execute()
            valveForBCs = projector.Surface

            for i in range(len(self.InputArrayNames)):
                self.PrintLog('\nProcessing array "'+self.InputArrayNames[i]+'"\n')
                inputArray = self.Surface.GetPointData().GetArray(self.InputArrayNames[i])
                numberOfComponents = inputArray.GetNumberOfComponents()

                harmonicSolver = vmtkcontribscripts.vmtkSurfaceHarmonicSolver()
                harmonicSolver.Surface = self.Valve
                harmonicSolver.InputRings = valveForBCs
                harmonicSolver.InputRingsBCsArrayName = self.OutputArrayNames[i]
                harmonicSolver.Interactive = False
                if numberOfComponents == 3:
                    harmonicSolver.VectorialEq = True
                else:
                    harmonicSolver.VectorialEq = False
                harmonicSolver.SolutionArrayName = self.OutputArrayNames[i]
                harmonicSolver.InitWithZeroDirBCs = False
                harmonicSolver.CellEntityIdsArrayName = self.CellEntityIdsArrayName
                harmonicSolver.Display = self.Display
                harmonicSolver.vmtkRenderer = self.vmtkRenderer
                harmonicSolver.Execute()
                self.Valve = harmonicSolver.Surface



if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
