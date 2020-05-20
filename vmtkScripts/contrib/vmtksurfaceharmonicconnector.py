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


vmtksurfaceharmoniconnector = 'vmtkSurfaceHarmonicConnector'

class vmtkSurfaceHarmonicConnector(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.DeformedSurface = None
        self.ReferenceSurface = None

        self.Valve = None
        self.ValveDistanceThreshold = 1

        self.Ring = None
        self.ReferenceRing = None
        self.RingsForBCs = None

        self.Ids = set()
        self.ExcludeIds = []
        self.ConnectorId = 1

        self.UseNullDirichletBCs = 1

        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.CellEntityIdsArray = None

        self.PreICP = 0
        self.ICPIters = 100

        self.SkipRemeshing = 0
        self.RemeshingEdgeLength = 1.0
        self.RemeshingIterations = 10

        self.SkipConnection = 0

        self.ProjectionFromInput = 0
        self.ProjectionFromReference = 0

        self.Display = 0
        self.vmtkRenderer = None



        self.SetScriptName('vmtksurfaceharmonicconnector')
        self.SetScriptDoc('connect to a reference surface harmonically deforming the input surface onto the reference one; note that the deformation depends on the distance between two selected boundary rings of the two surfaces; thus, the surfaces must be open surfaces; if a surface have more than a boundary ring, an interactive interface allows to select the rings to be connected')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['ReferenceSurface','r','vtkPolyData',1,'','the reference surface with which you want to connect','vmtksurfacereader'],
            ['Valve','ivalve','vtkPolyData',1,'','an optional additional surface near the input surface (e.g. a valve) deformed coherently with the input surface','vmtksurfacereader'],
            ['ExcludeIds','excludeids','int',-1,'','entity ids of the input surface excluded by the deformation, where a null deformation is imposed; this option only takes effect if "dirichletbcs" is true'],
            ['UseNullDirichletBCs','dirichletbcs','bool',1,'','toggle imposing homogeneous Dirichlet BCs at the boundary rings of the deformation domain; note that these BCs can be applied only if, on the deformation domain, there is at least a boundary ring other than the one to be connected'],
            ['ConnectorId','connectorid','int',1,'','entity id for the thin ring of elements connecting the two surfaces'],
            ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, '','name of the array where entity ids have been stored'],
            ['ValveDistanceThreshold','valvedistancethreshold','float',1,'(0,)','if Valve is set, distance threshold from the input surface used to set the region where to impose a Dirichlet BC on it'],
            ['PreICP','preicp','bool',1,'','toggle rigidly transforming the input surface using an icp registration on the rings to be connected, before computing the harmonic deformation'],
            ['ICPIters','icpiterations','int',1,'(0,)','number of icp iterations'],
            ['SkipRemeshing','skipremeshing','bool',1,'','toggle skipping remeshing the deformed part of the final surface'],
            ['RemeshingEdgeLength','remeshingedgelength','float',1,'(0.0,)'],
            ['RemeshingIterations','remeshingiterations','int',1,'(0,)'],
            ['SkipConnection','skipconnection','bool',1,'','deform input surface onto the reference one without connecting to it'],
            ['ProjectionFromInput','iprojection','bool',1,'','toggle performing a projection of the input surface point-data arrays onto the output surface'],
            ['ProjectionFromReference','rprojection','bool',1,'','toggle performing a projection of the reference surface point-data arrays onto the output surface'],
            ['Display','display','bool',1,'','toggle rendering the harmonic solver'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ['DeformedSurface','odeformed','vtkPolyData',1,'','the deformed surface before connection and remeshing','vmtksurfacewriter'],
            ['Valve','ovalve','vtkPolyData',1,'','the deformed additional surface (e.g. a valve)','vmtksurfacewriter'],
            ['RingsForBCs','obcs','vtkPolyData',1,'','the rings containing the array used for BCs of the Laplace-Beltrami equation','vmtksurfacewriter']
            ])


    def SurfaceProjection(self,isurface,rsurface):
        from vmtk import vmtkscripts
        proj = vmtkscripts.vmtkSurfaceProjection()
        proj.Surface = isurface
        proj.ReferenceSurface = rsurface
        proj.Execute()
        return proj.Surface


    def Execute(self):
        from vmtk import vtkvmtk
        from vmtk import vmtkscripts
        from vmtk import vmtkcontribscripts

        if self.Surface == None:
            self.PrintError('Error: no Surface.')

        if self.ReferenceSurface == None:
            self.PrintError('Error: no ReferenceSurface.')

        # 1. Extract rings using vmtkSurfaceConnector and optionally perform icp
        connector = vmtkcontribscripts.vmtkSurfaceConnector()
        connector.Surface = self.Surface
        connector.Surface2 = self.ReferenceSurface
        connector.vmtkRenderer = self.vmtkRenderer

        [boundaries,boundaryIds,tmp] = connector.InteractiveRingExtraction(self.Surface)
        self.Ring =  boundaries[boundaryIds.GetId(0)]

        [boundaries,boundaryIds,tmp] = connector.InteractiveRingExtraction(self.ReferenceSurface)
        self.ReferenceRing = boundaries[boundaryIds.GetId(0)]

        if self.PreICP:
            icp = vmtkscripts.vmtkICPRegistration()
            icp.Surface = self.Ring
            icp.ReferenceSurface = self.ReferenceRing
            icp.MaximumNumberOfIterations = self.ICPIters
            icp.Execute()
            self.Ring = icp.Surface

            transform = vmtkscripts.vmtkSurfaceTransform()
            transform.Surface = self.Surface
            transform.Matrix4x4 = icp.Matrix4x4
            transform.Execute()
            self.Surface = transform.Surface

            if self.Valve != None:
                transform.Surface = self.Valve
                transform.Execute()
                self.Valve = transform.Surface


        # 2. Compute distance from Ring to ReferenceRing
        distance = vmtkscripts.vmtkSurfaceDistance()
        distance.Surface = self.Ring
        distance.ReferenceSurface = self.ReferenceRing
        distance.DistanceVectorsArrayName = 'DistanceToReference'
        distance.Execute()

        self.Ring = distance.Surface

        # 3. Extend harmonically the distance from the ring
        harmonicSolver = vmtkcontribscripts.vmtkSurfaceHarmonicSolver()
        harmonicSolver.Surface = self.Surface
        harmonicSolver.InputRings = self.Ring
        harmonicSolver.Interactive = 0
        harmonicSolver.Display = self.Display
        harmonicSolver.VectorialEq = 1
        harmonicSolver.SolutionArrayName = 'WarpVector'
        harmonicSolver.InputRingsBCsArrayName = 'DistanceToReference'
        if self.UseNullDirichletBCs:
            harmonicSolver.InitWithZeroDirBCs = 1
            harmonicSolver.ExcludeIds = self.ExcludeIds
        else:
            harmonicSolver.InitWithZeroDirBCs = 0
        harmonicSolver.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        harmonicSolver.Execute()

        self.Surface = harmonicSolver.Surface
        self.RingsForBCs = harmonicSolver.DirichletBoundaries

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

            harmonicValveSolver = vmtkcontribscripts.vmtkSurfaceHarmonicSolver()
            harmonicValveSolver.Surface = self.Valve
            harmonicValveSolver.InputRings = valveForBCs
            harmonicValveSolver.Interactive = 0
            harmonicValveSolver.Display = self.Display
            harmonicValveSolver.VectorialEq = 1
            harmonicValveSolver.SolutionArrayName = 'WarpVector'
            harmonicValveSolver.InputRingsBCsArrayName = 'WarpVector'
            harmonicValveSolver.InitWithZeroDirBCs = 0
            harmonicValveSolver.CellEntityIdsArrayName = self.CellEntityIdsArrayName
            harmonicValveSolver.ExcludeIds = []
            harmonicValveSolver.Execute()
            self.Valve = harmonicValveSolver.Surface


        # 4. Warp by vector the surface
        warper = vmtkscripts.vmtkSurfaceWarpByVector()
        warper.Surface = self.Surface
        warper.WarpArrayName = 'WarpVector'
        warper.Execute()
        self.Surface = warper.Surface
        self.DeformedSurface = self.Surface

        if self.Valve!=None:
            warper.Surface = self.Valve
            warper.Execute()
            self.Valve = warper.Surface

        # 5. Connecting to the reference surface
        if not self.SkipConnection:
            distance = vmtkscripts.vmtkSurfaceDistance()
            distance.Surface = self.Surface
            distance.ReferenceSurface = self.ReferenceRing
            distance.DistanceArrayName = 'DistanceToReference'
            distance.Execute()
            self.Surface = distance.Surface

            clipper = vmtkscripts.vmtkSurfaceClipper()
            clipper.Surface = self.Surface
            clipper.ClipArrayName = 'DistanceToReference'
            clipper.ClipValue = self.RemeshingEdgeLength
            clipper.Interactive = 0
            clipper.CleanOutput = 1
            clipper.Execute()
            self.Surface = clipper.Surface

            if not self.SkipRemeshing:
                remeshing = vmtkscripts.vmtkSurfaceRemeshing()
                remeshing.Surface = self.Surface
                remeshing.CellEntityIdsArrayName = self.CellEntityIdsArrayName
                remeshing.ElementSizeMode = 'edgelength'
                remeshing.TargetEdgeLength = self.RemeshingEdgeLength
                remeshing.NumberOfIterations = int((self.RemeshingIterations+1)/2)
                if self.UseNullDirichletBCs:
                    remeshing.ExcludeEntityIds = self.ExcludeIds
                remeshing.CleanOutput = 1
                remeshing.Execute()

                self.Surface = remeshing.Surface

            connector = vmtkcontribscripts.vmtkSurfaceConnector()
            connector.Surface = self.Surface
            connector.Surface2 = self.ReferenceSurface
            connector.IdValue = self.ConnectorId
            connector.Execute()
            self.Surface = connector.OutputSurface

            if not self.SkipRemeshing:
                remeshing.Surface = self.Surface
                remeshing.NumberOfIterations = self.RemeshingIterations
                remeshing.Execute()
                self.Surface = remeshing.Surface

            if self.ProjectionFromReference:
                self.Surface = self.SurfaceProjection(self.Surface,self.ReferenceSurface)

            if self.ProjectionFromInput:
                self.Surface = self.SurfaceProjection(self.Surface,self.DeformedSurface)




if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
