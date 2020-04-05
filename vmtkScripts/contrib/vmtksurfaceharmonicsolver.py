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
        self.ExcludeSurface = None
        self.IncludeSurface = None
        
        self.Boundaries = None
        self.InputRings = None
        self.DirichletBoundaries = None

        self.Interactive = True
        self.VectorialEq = False

        self.SolutionArrayName = 'Temperature'
        self.SolutionArray = None

        self.InputRingsBCsArrayName = None
        self.RingsBCsArray = None

        self.ExcludeIds = []
        self.ExcludeIdsArrayName = None
        self.ExcludeIdsArray = None

        self.InitWithZeroDirBCs = 0

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
            ['InputRings','irings','vtkPolyData',1,'','the optional input rings where an array storing values to impose as BCs is defined; more than one ring can be passed, provided that they are stored all together in a unique vtkPolyData','vmtksurfacereader'],
            ['InputRingsBCsArrayName', 'ringsbcsarray', 'str', 1, '','name of the point-data array where the BCs values are stored'],
            ['ExcludeIds','excludeids','int',-1,'','entity ids excluded by the solution of the equation'],
            ['ExcludeIdsArrayName', 'excludeidsarray', 'str', 1, '','name of the point-data array defined on the input surface that replaces the solution on the excluded ids; if None, the solutions is set to zero on these ids'],
            ['InitWithZeroDirBCs','zerodirbcs','bool',1,'','toggle initializing all the boundary rings with an homogeneous Dirichlet condition'],
            ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, '','name of the cell-data array where entity ids have been stored'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])


    def SurfaceThreshold(self,surface,low,high):
        from vmtk import vmtkcontribscripts
        th = vmtkcontribscripts.vmtkThreshold()
        th.Surface = surface
        th.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        th.LowThreshold = low
        th.HighThreshold = high
        th.Execute()
        surf = th.Surface
        return surf

    def SurfaceAppend(self,surface1,surface2):
        from vmtk import vmtkscripts
        if surface1 == None:
            surf = surface2
        elif surface2 == None:
            surf = surface1
        else:
            a = vmtkscripts.vmtkSurfaceAppend()
            a.Surface = surface1
            a.Surface2 = surface2
            a.Execute()
            surf = a.Surface
            tr = vmtkscripts.vmtkSurfaceTriangle()
            tr.Surface = surf
            tr.Execute()
            surf = tr.Surface
        return surf


    def ExtractDomain(self,surface,excludeIds):
        if self.ExcludeIds!=[]:

            self.CellEntityIdsArray = self.Surface.GetCellData().GetArray(self.CellEntityIdsArrayName)

            ids = set()
            includeIds = []

            for i in range(self.Surface.GetNumberOfCells()):
                ids.add(int(self.CellEntityIdsArray.GetComponent(i,0)))
            ids = sorted(ids)
            self.PrintLog('Entity Ids:\t'+str(ids))

            for item in excludeIds:
                if item not in ids:
                    self.PrintError('Error: entity id '+str(item)+ ' not defined on the input Surface.')

            for item in ids:
                if item not in excludeIds:
                    includeIds.append(item)

            self.PrintLog('Excluded Ids:\t'+str(excludeIds))
            self.PrintLog('Included Ids:\t'+str(includeIds))

            excludeSurface = None
            includeSurface = None

            for item in excludeIds:
                singleId =  self.SurfaceThreshold(self.Surface,item,item)
                excludeSurface = self.SurfaceAppend(singleId,excludeSurface)

            for item in includeIds:
                singleId =  self.SurfaceThreshold(self.Surface,item,item)
                includeSurface = self.SurfaceAppend(singleId,includeSurface)

        else:
            includeSurface = self.Surface
            excludeSurface = None

        return [includeSurface,excludeSurface]


    def ExtractBoundaries(self,surface):
        extractBoundaries = vtk.vtkFeatureEdges()
        extractBoundaries.BoundaryEdgesOn()
        extractBoundaries.FeatureEdgesOff()
        extractBoundaries.NonManifoldEdgesOff()
        extractBoundaries.ManifoldEdgesOff()
        extractBoundaries.ColoringOff()
        extractBoundaries.SetInputData(surface)
        extractBoundaries.CreateDefaultLocator()
        extractBoundaries.Update()
        return extractBoundaries.GetOutput()

        
    def DefineDirichletBCs(self,boundaries):
        dirichletBoundaries = None

        if self.VectorialEq:
            numberOfComponents = 3
        else:
            numberOfComponents = 1

        if self.InitWithZeroDirBCs:
            zeroBCsArray = vtk.vtkDoubleArray()
            zeroBCsArray.SetNumberOfComponents(numberOfComponents)
            zeroBCsArray.SetName('DirichletBCs')
            zeroBCsArray.SetNumberOfTuples(boundaries.GetNumberOfPoints())
            for k in range(numberOfComponents):
                zeroBCsArray.FillComponent(k,0.0)
            dirichletBoundaries = vtk.vtkPolyData()
            dirichletBoundaries.DeepCopy(boundaries)
            dirichletBoundaries.GetPointData().AddArray(zeroBCsArray)

        if self.Interactive:
            dirichletBoundaries = self.AddInteractiveBCs(boundaries)

        if self.InputRings!=None:
            dirichletBoundaries = self.AddInputRingsBCs(dirichletBoundaries)

        if dirichletBoundaries==None or dirichletBoundaries.GetPointData().GetArray('DirichletBCs')==None:
            self.PrintError("Error: no BCs defined.")

        return dirichletBoundaries


    def AddInputRingsBCs(self,dirichletBoundaries):
        inputRingsBCsArray = self.InputRings.GetPointData().GetArray(self.InputRingsBCsArrayName)
        inputRingsBCsArray.SetName('DirichletBCs')

        numberOfComponents = inputRingsBCsArray.GetNumberOfComponents()

        if self.VectorialEq and numberOfComponents!=3:
            self.PrintError('Error: vectorial equation need a three-component array defined on InputRings as BCs.')

        if not self.VectorialEq and numberOfComponents!=1:
            self.PrintError('Error: scalar equation need a single-component array defined on InputRings as BCs.')

        return self.SurfaceAppend(self.InputRings,dirichletBoundaries)


    def AddInteractiveBCs(self,boundaries):
        from vmtk import vmtkscripts
        # extract the boundary rings and interactively set
        # a constant BCs on it


    def Execute(self):
        from vmtk import vtkvmtk
        from vmtk import vmtkscripts
        from vmtk import vmtkcontribscripts

        if self.Surface == None:
            self.PrintError('Error: no Surface.')

        [self.IncludeSurface, self.ExcludeSurface] = self.ExtractDomain(self.Surface,self.ExcludeIds)

        self.Boundaries = self.ExtractBoundaries(self.IncludeSurface)

        self.DirichletBoundaries = self.DefineDirichletBCs(self.Boundaries)

        bcsArray = self.DirichletBoundaries.GetPointData().GetArray('DirichletBCs')

        if self.VectorialEq:
            numberOfComponents = 3
        else:
            numberOfComponents = 1

        pointLocator = vtk.vtkPointLocator()
        pointLocator.SetDataSet(self.IncludeSurface)
        pointLocator.BuildLocator()

        harmonicOutput = vtk.vtkPolyData()
        harmonicOutput.DeepCopy(self.IncludeSurface)

        for k in range(numberOfComponents):
            print("Solving Laplace-Beltrami equation for component ",k)
            
            boundaryIds = vtk.vtkIdList()
            boundaryValues = vtk.vtkDoubleArray()
            boundaryValues.SetNumberOfComponents(1)

            for i in range(self.DirichletBoundaries.GetNumberOfPoints()):
                boundaryId = pointLocator.FindClosestPoint(self.DirichletBoundaries.GetPoint(i))
                boundaryIds.InsertNextId(boundaryId)
                boundaryValues.InsertNextTuple1(bcsArray.GetComponent(i,k))

            # perform harmonic mapping using temperature as boundary condition
            harmonicSolver = vtkvmtk.vtkvmtkPolyDataHarmonicMappingFilter()
            harmonicSolver.SetInputData(harmonicOutput)
            harmonicSolver.SetHarmonicMappingArrayName(self.SolutionArrayName+str(k))
            harmonicSolver.SetBoundaryPointIds(boundaryIds)
            harmonicSolver.SetBoundaryValues(boundaryValues)
            harmonicSolver.SetAssemblyModeToFiniteElements()
            harmonicSolver.Update()

            harmonicOutput = harmonicSolver.GetOutput()

        solutionArray = vtk.vtkDoubleArray()
        solutionArray.SetNumberOfComponents(numberOfComponents)
        solutionArray.SetNumberOfTuples(self.IncludeSurface.GetNumberOfPoints())
        solutionArray.SetName(self.SolutionArrayName)

        for i in range(self.IncludeSurface.GetNumberOfPoints()):
            for k in range(numberOfComponents):
                value = harmonicOutput.GetPointData().GetArray(self.SolutionArrayName+str(k)).GetComponent(i,0)
                solutionArray.SetComponent(i,k,value)

        self.IncludeSurface.GetPointData().AddArray(solutionArray)

        if self.ExcludeSurface!=None:
            solutionArray = vtk.vtkDoubleArray()
            solutionArray.SetNumberOfComponents(numberOfComponents)
            solutionArray.SetNumberOfTuples(self.ExcludeSurface.GetNumberOfPoints())
            solutionArray.SetName(self.SolutionArrayName)

            if self.ExcludeIdsArrayName==None:
                for k in range(numberOfComponents):
                    solutionArray.FillComponent(k,0.0)
            else:
                excludeIdsArray = self.ExcludeSurface.GetPointData().GetArray(self.ExcludeIdsArrayName)
                if excludeIdsArray.GetNumberOfComponents()!=numberOfComponents:
                    self.PrintError('Error: ExcludeIdsArray has a not compatible number of components')
                for i in range(self.ExcludeSurface.GetNumberOfPoints()):
                    for k in range(numberOfComponents):
                        value = excludeIdsArray.GetComponent(i,k)
                        solutionArray.SetComponent(i,k,value)

            self.ExcludeSurface.GetPointData().AddArray(solutionArray)

        self.Surface = self.SurfaceAppend(self.IncludeSurface,self.ExcludeSurface)



if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()