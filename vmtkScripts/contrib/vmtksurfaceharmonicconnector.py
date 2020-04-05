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

        self.Ring = None
        self.ReferenceRing = None
        self.RingsForBCs = None

        self.Ids = set()
        self.DeformableIds = []

        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.CellEntityIdsArray = None

        self.SkipRemeshing = 0
        self.RemeshingEdgeLength = 1.0
        self.RemeshingIterations = 10

        self.SkipConnection = 0

        self.Projection = 0

        self.CleanOutput = 1

        self.vmtkRenderer = None



        self.SetScriptName('vmtksurfaceharmonicconnector')
        self.SetScriptDoc('connect to a reference surface harmonically deform the input surface onto the reference one; note that the deformation depends on the distance between two selected boundary rings of the two surfaces; thus, the surfaces must be open surfaces; if a surface have more than a boundary ring, an interactive interface allows to select the ring to connect')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['ReferenceSurface','r','vtkPolyData',1,'','the reference surface with which you want to connect','vmtksurfacereader'],
            ['DeformableIds','deformableids','int',-1,'','input surface entity ids to deform onto the reference surface'],
            ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, '','name of the array where entity ids have been stored'],
            ['SkipRemeshing','skipremeshing','bool',1,'','toggle skipping remeshing the deformed part of the final surface'],
            ['RemeshingEdgeLength','remeshingedgelength','float',1,'(0.0,)'],
            ['RemeshingIterations','remeshingiterations','int',1,'(0,)'],
            ['SkipConnection','skipconnection','bool',1,'','deform input surface onto the reference one without connecting to it'],
            ['Projection','projection','bool',1,'','toggle performing a projection of the input surface point-data arrays onto the output surface'],
            ['CleanOutput','cleanoutput','bool',1,'','toggle cleaning the unused points'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ['DeformedSurface','odeformed','vtkPolyData',1,'','the deformed surface before connection and remeshing','vmtksurfacewriter'],
            ['RingsForBCs','obcs','vtkPolyData',1,'','the rings containing the array used for BCs of the Laplace-Beltrami equation','vmtksurfacewriter']
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

        self.CellEntityIdsArray = self.Surface.GetCellData().GetArray(self.CellEntityIdsArrayName)

        for i in range(self.Surface.GetNumberOfCells()):
            self.Ids.add(self.CellEntityIdsArray.GetComponent(i,0))
        self.Ids = sorted(self.Ids)
        self.PrintLog('Tags of the input surface: '+str(self.Ids))

        for item in self.DeformableIds:
            if item not in self.Ids:
                self.PrintError('Error: entity id '+str(item)+ ' not defined on the input Surface.')

        # 1. Extract rings using vmtkSurfaceConnector
        connector = vmtkcontribscripts.vmtkSurfaceConnector()
        connector.Surface = self.Surface
        connector.Surface2 = self.ReferenceSurface
        connector.vmtkRenderer = self.vmtkRenderer

        [boundaries,boundaryIds,tmp] = connector.InteractiveRingExtraction(self.Surface)
        self.Ring =  boundaries[boundaryIds.GetId(0)]

        [boundaries,boundaryIds,tmp] = connector.InteractiveRingExtraction(self.ReferenceSurface)
        self.ReferenceRing = boundaries[boundaryIds.GetId(0)]

        # 2. Compute distance from Ring to ReferenceRing
        distance = vmtkscripts.vmtkSurfaceDistance()
        distance.Surface = self.Ring
        distance.ReferenceSurface = self.ReferenceRing
        distance.DistanceVectorsArrayName = 'BCs'
        distance.Execute()

        self.Ring = distance.Surface

        # 3. Extract boundaries of the deformable subset of the surface
        surfaceSubset = None
        for item in self.DeformableIds:
            singleId = self.SurfaceThreshold(self.Surface,item,item)
            surfaceSubset = self.SurfaceAppend(singleId,surfaceSubset)

        extractBoundaries = vtk.vtkFeatureEdges()
        extractBoundaries.BoundaryEdgesOn()
        extractBoundaries.FeatureEdgesOff()
        extractBoundaries.NonManifoldEdgesOff()
        extractBoundaries.ManifoldEdgesOff()
        extractBoundaries.ColoringOff()
        extractBoundaries.SetInputData(surfaceSubset)
        extractBoundaries.CreateDefaultLocator()
        extractBoundaries.Update()
        self.RingsForBCs = extractBoundaries.GetOutput()

        # 4. Fill the boundary rings array using the distance at the ring
        #    to deform and 0.0 otherwise
        arrayForBCs = vtk.vtkDoubleArray()
        arrayForBCs.SetNumberOfComponents(3)
        arrayForBCs.SetNumberOfTuples(self.RingsForBCs.GetNumberOfPoints())
        arrayForBCs.SetName('BCs')
        for k in range(3):
            arrayForBCs.FillComponent(k,0.0)
        self.RingsForBCs.GetPointData().AddArray(arrayForBCs)

        self.RingsForBCs = self.SurfaceAppend(self.Ring,self.RingsForBCs) # this overwrite the zeros on the self.Ring

        # 3. Extend harmonically the distance from the ring to the DeformableIds
        harmonicSolver = vmtkcontribscripts.vmtkSurfaceHarmonicSolver()
        harmonicSolver.Surface = self.Surface
        harmonicSolver.Rings = self.RingsForBCs
        harmonicSolver.Interactive = False
        harmonicSolver.VectorialEq = True
        harmonicSolver.SolutionArrayName = 'WarpVector'
        harmonicSolver.Execute()

        self.Surface = harmonicSolver.Surface

        # 4. Warp by vector the surface
        warper = vmtkscripts.vmtkSurfaceWarpByVector()
        warper.Surface = self.Surface
        warper.WarpArrayName = 'WarpVector'
        warper.Execute()
        self.Surface = warper.Surface
        self.DeformedSurface = self.Surface

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
                excludedIds = []
                for item in self.Ids:
                    if item not in self.DeformableIds:
                        excludedIds.append(int(item)) # dangerous cast to int
                print("Excluded Ids: ",excludedIds)

                remeshing = vmtkscripts.vmtkSurfaceRemeshing()
                remeshing.Surface = self.Surface
                remeshing.CellEntityIdsArrayName = self.CellEntityIdsArrayName
                remeshing.ElementSizeMode = 'edgelength'
                remeshing.TargetEdgeLength = self.RemeshingEdgeLength
                remeshing.NumberOfIterations = int((self.RemeshingIterations+1)/2)
                remeshing.ExcludeEntityIds = excludedIds
                remeshing.CleanOutput = self.CleanOutput
                remeshing.Execute()

                self.Surface = remeshing.Surface

            connector = vmtkcontribscripts.vmtkSurfaceConnector()
            connector.Surface = self.Surface
            connector.Surface2 = self.ReferenceSurface
            connector.IdValue = self.DeformableIds[0] # How to fix this?
            connector.Execute()
            self.Surface = connector.OutputSurface

            if not self.SkipRemeshing:
                remeshing.Surface = self.Surface
                remeshing.NumberOfIterations = self.RemeshingIterations
                remeshing.Execute()
                self.Surface = remeshing.Surface

            if self.Projection:
                projector = vmtkscripts.vmtkSurfaceProjection()
                projector.Surface = self.Surface
                projector.ReferenceSurface = self.DeformedSurface
                projector.Execute()
                self.Surface = projector.Surface




if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
