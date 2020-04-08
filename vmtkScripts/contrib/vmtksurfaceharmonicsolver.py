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
import string

from vmtk import vmtkrenderer
from vmtk import vtkvmtk
from vmtk import pypes


vmtksurfaceharmonicsolver = 'vmtkSurfaceHarmonicSolver'

class vmtkSurfaceHarmonicSolver(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.ExcludeSurface = None
        self.IncludeSurface = None
        self.SurfaceForBCs = None
        
        self.Boundaries = None
        self.InputRings = None
        self.DirichletBoundaries = None

        self.Interactive = True
        self.VectorialEq = False

        self.SolutionArrayName = 'HarmonicSolution'
        self.SolutionArray = None

        self.InputRingsBCsArrayName = None
        self.RingsBCsArray = None

        self.ExcludeIds = []
        self.ExcludeIdsArrayName = None
        self.ExcludeIdsArray = None
        self.ExcludeIdsForBCs = []

        self.InitWithZeroDirBCs = 0

        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.CellEntityIdsArray = None

        self.Display = 1
        self.vmtkRenderer = None
        self.OwnRenderer = 0


        self.SetScriptName('vmtksurfaceharmonicsolver')
        self.SetScriptDoc('solve a Laplace-Beltrami equation on a surface; boundary conditions can be set either interactively choosing a constant value at each boundary ring or through input rings where an array with the boundary values is defined; the equation can be solved both for a scalar or a vectorial field')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['Interactive','interactive','bool',1,'','toggle opening an interactive window to choose the constant boundary conditions at each boundary ring'],
            ['VectorialEq','vectorialeq','bool',1,'','toggle solving the scalar or the vectorial Laplace-Beltrami equation'],
            ['SolutionArrayName', 'solutionarray', 'str', 1, '','name of the point-data array where the solution is stored'],
            ['InputRings','irings','vtkPolyData',1,'','the optional input rings where an array storing values to impose as BCs is defined; more than one ring can be passed, provided that they are stored all together in a unique vtkPolyData','vmtksurfacereader'],
            ['InputRingsBCsArrayName', 'ringsbcsarray', 'str', 1, '','name of the point-data array where the BCs values are stored'],
            ['ExcludeIds','excludeids','int',-1,'','entity ids excluded by the equation domain'],
            ['ExcludeIdsArrayName', 'excludeidsarray', 'str', 1, '','name of the point-data array defined on the input surface that replaces the solution on the excluded ids; if None, the solutions is set to zero on these ids'],
            ['ExcludeIdsForBCs','excludeidsforbcs','int',-1,'','entity ids excluded by the domain only for the definition of the rings where to set the Dirichlet BCs; these ids are not excluded by the equation domain; this options has some effects only if "Interactive" or "InitWithZeroDirBCs" are true'],
            ['InitWithZeroDirBCs','zerodirbcs','bool',1,'','toggle initializing all the boundary rings with an homogeneous Dirichlet condition'],
            ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, '','name of the cell-data array where entity ids have been stored'],
            ['Display','display','bool',1,'','toggle rendering'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ['DirichletBoundaries','odirbcs','vtkPolyData',1,'','the rings where the applied Dirichlet BCs have been defined','vmtksurfacewriter'],

            ])


    def SurfaceThreshold(self,surface,low,high,arrayName=None):
        from vmtk import vmtkcontribscripts
        if arrayName==None:
            arrayName=self.CellEntityIdsArrayName
        th = vmtkcontribscripts.vmtkThreshold()
        th.Surface = surface
        th.CellEntityIdsArrayName = arrayName
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
        if excludeIds!=[]:

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

        
    def DefineDirichletBCs(self,boundaries,domain):
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
            dirichletBoundaries = self.AddInteractiveBCs(boundaries,domain,dirichletBoundaries)

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


    def LabelValidator(self,text):
        import string
        if text=='':
            return 1
        if not text:
            return 0
        if not text.split():
            return 0
        allowedChars = string.digits+" .-"
        for char in text:
            if char not in allowedChars:
                return 0
        return 1


    def AddInteractiveBCs(self,boundaries,domain,dirichletBoundaries):

        rings = vtk.vtkPolyData()
        rings.DeepCopy(boundaries)

        connectivity = vtk.vtkConnectivityFilter()
        connectivity.SetInputData(rings)
        connectivity.SetExtractionModeToAllRegions()
        connectivity.ColorRegionsOn()
        connectivity.Update()
        rings = connectivity.GetOutput()

        ringIdsArray = rings.GetCellData().GetArray('RegionId')

        ringIds = set()
        for i in range(rings.GetNumberOfCells()):
            ringIds.add(ringIdsArray.GetComponent(i,0))
        ringIds = sorted(ringIds)
        numberOfRings = len(ringIds)

        ringList = []
        for item in ringIds:
            ringList.append(self.SurfaceThreshold(rings,item,item,'RegionId'))

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self)

        # render numbers at each boundary ring 
        seedPoints = vtk.vtkPoints()
        for i in range(numberOfRings):
            barycenter = [0.0, 0.0, 0.0]
            vtkvmtk.vtkvmtkBoundaryReferenceSystems.ComputeBoundaryBarycenter(ringList[i].GetPoints(),barycenter)
            seedPoints.InsertNextPoint(barycenter)
        seedPolyData = vtk.vtkPolyData()
        seedPolyData.SetPoints(seedPoints)
        labelsMapper = vtk.vtkLabeledDataMapper();
        labelsMapper.SetInputData(seedPolyData)
        labelsMapper.SetLabelModeToLabelIds()
        labelsActor = vtk.vtkActor2D()
        labelsActor.SetMapper(labelsMapper)
        self.vmtkRenderer.Renderer.AddActor(labelsActor)

        # render the domain in transparency
        surfaceMapper = vtk.vtkPolyDataMapper()
        surfaceMapper.SetInputData(domain)
        surfaceMapper.ScalarVisibilityOff()
        surfaceActor = vtk.vtkActor()
        surfaceActor.SetMapper(surfaceMapper)
        surfaceActor.GetProperty().SetOpacity(0.25)
        surfaceActor.GetProperty().SetColor(1.0,1.0,1.0)
        self.vmtkRenderer.Renderer.AddActor(surfaceActor)

        bcString='whatever'
        bcId = []
        bcValue = []
        while bcString!='':
            ok = False
            while not ok:
                inputString = "Please input an id followed by a constant BC, e.g.:  0  -0.5"
                if self.VectorialEq:
                    inputString += "  -1.3  2.1"
                inputString += "\nJust type 'return' if all BCs have been assigned:\n"

                bcString = self.InputText(inputString,self.LabelValidator)
                ok = True
                if bcString!='':
                    bcAsList = bcString.split()
                    # check dimension
                    if self.VectorialEq:
                        if len(bcAsList)!=4:
                            ok = False
                    else:
                        if len(bcAsList)!=2:
                            ok = False
                    # check the Id is an integer number and if it is in ringIds
                    if not bcAsList[0].isdigit():
                        ok = False
                    elif int(bcAsList[0]) not in ringIds:
                        ok = False
                    # check if BC is made of float
                    for item in bcAsList:
                        try:
                            float(item)
                        except:
                            ok = False
                    if ok:
                        bcId.append(int(bcAsList[0]))
                        bcValue.append([float(item) for item in bcAsList[1:]])
                        self.PrintLog('BC '+bcString+' correctly inserted\n')
                    else:
                        self.PrintLog('Not allowed string\n')

        print('BCs Ids   : ',bcId)
        print('BCs values: ',bcValue)

        if self.VectorialEq:
            numberOfComponents = 3
        else:
            numberOfComponents = 1

        for i,item in enumerate(bcId):
            constantBCsArray = vtk.vtkDoubleArray()
            constantBCsArray.SetNumberOfComponents(numberOfComponents)
            constantBCsArray.SetName('DirichletBCs')
            constantBCsArray.SetNumberOfTuples(ringList[item].GetNumberOfPoints())
            for k in range(numberOfComponents):
                constantBCsArray.FillComponent(k,bcValue[i][k])
            ringList[item].GetPointData().AddArray(constantBCsArray)
            dirichletBoundaries = self.SurfaceAppend(ringList[item],dirichletBoundaries)

        # render the domain in transparency and the rings with the constant BC
        surfaceActor.GetProperty().SetOpacity(0.1)

        self.vmtkRenderer.Renderer.RemoveActor(labelsActor)
        self.vmtkRenderer.Renderer.RemoveActor(surfaceActor)

        
        # self.vmtkRenderer.Render()

        if self.OwnRenderer and not self.Display:
            self.vmtkRenderer.Deallocate()

        return dirichletBoundaries


    def SolveHarmonicProblem(self,domainSurface,dirichletBoundaries):

        bcsArray = dirichletBoundaries.GetPointData().GetArray('DirichletBCs')

        if self.VectorialEq:
            numberOfComponents = 3
        else:
            numberOfComponents = 1

        pointLocator = vtk.vtkPointLocator()
        pointLocator.SetDataSet(domainSurface)
        pointLocator.BuildLocator()

        harmonicOutput = vtk.vtkPolyData()
        harmonicOutput.DeepCopy(domainSurface)

        for k in range(numberOfComponents):
            print("Solving Laplace-Beltrami equation for component ",k)
            
            boundaryIds = vtk.vtkIdList()
            boundaryValues = vtk.vtkDoubleArray()
            boundaryValues.SetNumberOfComponents(1)

            for i in range(dirichletBoundaries.GetNumberOfPoints()):
                boundaryId = pointLocator.FindClosestPoint(dirichletBoundaries.GetPoint(i))
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
        solutionArray.SetNumberOfTuples(domainSurface.GetNumberOfPoints())
        solutionArray.SetName(self.SolutionArrayName)

        for i in range(domainSurface.GetNumberOfPoints()):
            for k in range(numberOfComponents):
                value = harmonicOutput.GetPointData().GetArray(self.SolutionArrayName+str(k)).GetComponent(i,0)
                solutionArray.SetComponent(i,k,value)

        domainSurface.GetPointData().AddArray(solutionArray)

        return domainSurface


    def ExtendSolutionOnExcludedDomain(self,excludedDomain):

        if self.VectorialEq:
            numberOfComponents = 3
        else:
            numberOfComponents = 1

        solutionArray = vtk.vtkDoubleArray()
        solutionArray.SetNumberOfComponents(numberOfComponents)
        solutionArray.SetNumberOfTuples(excludedDomain.GetNumberOfPoints())
        solutionArray.SetName(self.SolutionArrayName)

        if self.ExcludeIdsArrayName==None:
            for k in range(numberOfComponents):
                solutionArray.FillComponent(k,0.0)
        else:
            excludeIdsArray = excludedDomain.GetPointData().GetArray(self.ExcludeIdsArrayName)
            if excludeIdsArray.GetNumberOfComponents()!=numberOfComponents:
                self.PrintError('Error: ExcludeIdsArray has a not compatible number of components')
            for i in range(self.ExcludeSurface.GetNumberOfPoints()):
                for k in range(numberOfComponents):
                    value = excludeIdsArray.GetComponent(i,k)
                    solutionArray.SetComponent(i,k,value)

        excludedDomain.GetPointData().AddArray(solutionArray)

        return excludedDomain


    def DisplayDirichletBCs(self,domain,dirichletBoundaries):
        from vmtk import vmtkscripts

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self)

        # render the domain in transparency
        surfaceMapper = vtk.vtkPolyDataMapper()
        surfaceMapper.SetInputData(domain)
        surfaceMapper.ScalarVisibilityOff()
        surfaceActor = vtk.vtkActor()
        surfaceActor.SetMapper(surfaceMapper)
        surfaceActor.GetProperty().SetOpacity(0.25)
        surfaceActor.GetProperty().SetColor(1.0,1.0,1.0)
        self.vmtkRenderer.Renderer.AddActor(surfaceActor)

        surfaceViewer = vmtkscripts.vmtkSurfaceViewer()
        surfaceViewer.Surface = dirichletBoundaries
        surfaceViewer.ArrayName = 'DirichletBCs'
        surfaceViewer.Representation = 'surface'
        surfaceViewer.LineWidth = 3
        surfaceViewer.Legend = 1
        surfaceViewer.LegendTitle = 'Dirichlet BCs'
        surfaceViewer.ColorMap = 'rainbow'
        surfaceViewer.vmtkRenderer = self.vmtkRenderer
        surfaceViewer.Execute()

        # self.vmtkRenderer.Renderer.RemoveActor(surfaceViewer.Surface)
        self.vmtkRenderer.Renderer.RemoveActor(surfaceActor)
        self.vmtkRenderer.Renderer.RemoveActor(surfaceViewer.Actor)
        self.vmtkRenderer.Renderer.RemoveActor(surfaceViewer.ScalarBarActor)


    def DisplaySolution(self,surface,dirichletBoundaries):
        from vmtk import vmtkscripts

        surfaceViewer = vmtkscripts.vmtkSurfaceViewer()
        surfaceViewer.Surface = surface
        surfaceViewer.ArrayName = self.SolutionArrayName
        surfaceViewer.Representation = 'surface'
        surfaceViewer.Legend = 1
        surfaceViewer.LegendTitle = self.SolutionArrayName
        surfaceViewer.ColorMap = 'rainbow'
        surfaceViewer.vmtkRenderer = self.vmtkRenderer
        surfaceViewer.Display = 0
        surfaceViewer.Execute()

        scalarRange = list(surface.GetPointData().GetArray(self.SolutionArrayName).GetRange(-1))

        surfaceViewer2 = vmtkscripts.vmtkSurfaceViewer()
        surfaceViewer2.Surface = dirichletBoundaries
        surfaceViewer2.ArrayName = 'DirichletBCs'
        surfaceViewer2.ScalarRange = scalarRange
        surfaceViewer2.Representation = 'surface'
        surfaceViewer2.LineWidth = 5
        surfaceViewer2.Legend = 0
        surfaceViewer2.ColorMap = 'rainbow'
        surfaceViewer2.vmtkRenderer = self.vmtkRenderer
        surfaceViewer2.Execute()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: no Surface.')

        [self.IncludeSurface, self.ExcludeSurface] = self.ExtractDomain(self.Surface,self.ExcludeIds)
        self.ExcludeIdsForBCs.extend(self.ExcludeIds)
        self.SurfaceForBCs = self.ExtractDomain(self.Surface,self.ExcludeIdsForBCs)[0]

        self.Boundaries = self.ExtractBoundaries(self.SurfaceForBCs)

        self.DirichletBoundaries = self.DefineDirichletBCs(self.Boundaries,self.SurfaceForBCs)

        if self.Display:
            self.DisplayDirichletBCs(self.SurfaceForBCs,self.DirichletBoundaries)

        self.IncludeSurface = self.SolveHarmonicProblem(self.IncludeSurface, self.DirichletBoundaries)

        if self.ExcludeSurface!=None:
            self.ExcludeSurface = self.ExtendSolutionOnExcludedDomain(self.ExcludeSurface)

        self.Surface = self.SurfaceAppend(self.IncludeSurface,self.ExcludeSurface)

        if self.Display:
            self.DisplaySolution(self.Surface,self.DirichletBoundaries)



if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()