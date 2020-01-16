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
import numpy as np

from vmtk import pypes

class vmtkMeshTetHex(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.CellEntityIdsArray = None
        self.NumberOfRefineBySplitting = 0
        self.GlobalPtId = 0
        self.NewPointSet = dict()

        self.SetScriptName('vmtkmeshtethex')
        self.SetScriptDoc('generate hexahedral mesh from a tetrahedral one, splitting each triangle in three quads and each tetrahedron in four hexahedron')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['CellEntityIdsArrayName','entityidsarray','str',1],
            ['NumberOfRefineBySplitting','refinebysplitting','int',1,'','number of refine-by-splitting performed after first hexahedral mesh generation'],
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']
            ])

    def Execute(self):

        def InsertPoint(pt):
            dictKey = np.array2string(pt,None,10)
            if dictKey not in self.NewPointSet:
                meshPoints.InsertPoint(self.GlobalPtId, pt)
                ptId = self.GlobalPtId
                self.NewPointSet[dictKey] = ptId
                self.GlobalPtId += 1
            else:
                ptId = self.NewPointSet[dictKey]
            return ptId

        def InsertHexa(id0,id1,id2,id3,id4,id5,id6,id7):
            hexa = vtk.vtkHexahedron()
            hexa.GetPointIds().SetId(0,id0)
            hexa.GetPointIds().SetId(1,id1)
            hexa.GetPointIds().SetId(2,id2)
            hexa.GetPointIds().SetId(3,id3)
            hexa.GetPointIds().SetId(4,id4)
            hexa.GetPointIds().SetId(5,id5)
            hexa.GetPointIds().SetId(6,id6)
            hexa.GetPointIds().SetId(7,id7)
            self.Mesh.InsertNextCell(hexa.GetCellType(),hexa.GetPointIds())

        def InsertQuad(id0,id1,id2,id3):
            quad = vtk.vtkQuad()
            quad.GetPointIds().SetId(0,id0)
            quad.GetPointIds().SetId(1,id1)
            quad.GetPointIds().SetId(2,id2)
            quad.GetPointIds().SetId(3,id3)
            self.Mesh.InsertNextCell(quad.GetCellType(),quad.GetPointIds())

        def InsertLine(id0,id1):
            line = vtk.vtkLine()
            line.GetPointIds().SetId(0,id0)
            line.GetPointIds().SetId(1,id1)
            self.Mesh.InsertNextCell(line.GetCellType(),line.GetPointIds())


        self.PrintLog("    Generation of the hexahedral mesh:")
        # initialize toolbar
        inputCells = self.Mesh.GetNumberOfCells()
        toolbar_width = min(40,inputCells)
        sys.stdout.write( "    [%s]" % (" " * toolbar_width) )
        sys.stdout.flush()
        sys.stdout.write( "\b" * ( toolbar_width + 1 ) ) # return to start of line, after '['

        inputMesh = self.Mesh
        inputCellEntityIdsArray = inputMesh.GetCellData().GetArray(self.CellEntityIdsArrayName)

        meshPoints = vtk.vtkPoints()
        meshPoints.DeepCopy(inputMesh.GetPoints())

        numberOfPoints = inputMesh.GetNumberOfPoints()
        self.GlobalPtId = numberOfPoints

        self.Mesh = vtk.vtkUnstructuredGrid()
        self.Mesh.SetPoints(meshPoints)
        self.CellEntityIdsArray = vtk.vtkIntArray()
        self.CellEntityIdsArray.SetName(self.CellEntityIdsArrayName)
        self.Mesh.GetCellData().AddArray(self.CellEntityIdsArray)
        numberOfCells = 0

        tetraType = 10 # Tetrahedra
        tetraIdArray = vtk.vtkIdTypeArray()
        inputMesh.GetIdsOfCellsOfType(tetraType,tetraIdArray) # extract all the tetra cell
        numberOfTetras = tetraIdArray.GetNumberOfTuples()

        for i in range(numberOfTetras):

            tetraId = tetraIdArray.GetValue(i)
            if inputCellEntityIdsArray != None:
                tetraEntityId = inputCellEntityIdsArray.GetValue(tetraId)
            else:
                tetraEntityId = 1
            tetra = inputMesh.GetCell(tetraId)
            tetraPointIds = tetra.GetPointIds()

            pt0Id = tetraPointIds.GetId(0)
            pt1Id = tetraPointIds.GetId(1)
            pt2Id = tetraPointIds.GetId(2)
            pt3Id = tetraPointIds.GetId(3)

            pt0 = np.array(meshPoints.GetPoint(pt0Id))
            pt1 = np.array(meshPoints.GetPoint(pt1Id))
            pt2 = np.array(meshPoints.GetPoint(pt2Id))
            pt3 = np.array(meshPoints.GetPoint(pt3Id))

            mp01 = (pt0 + pt1) / 2.0
            mp02 = (pt0 + pt2) / 2.0
            mp03 = (pt0 + pt3) / 2.0
            mp12 = (pt1 + pt2) / 2.0
            mp13 = (pt1 + pt3) / 2.0
            mp23 = (pt2 + pt3) / 2.0

            ct012 = (pt0 + pt1 + pt2) / 3.0
            ct013 = (pt0 + pt1 + pt3) / 3.0
            ct023 = (pt0 + pt2 + pt3) / 3.0
            ct123 = (pt1 + pt2 + pt3) / 3.0

            g = (pt0 + pt1 + pt2 + pt3) / 4.0

            mp01Id = InsertPoint(mp01)
            mp02Id = InsertPoint(mp02)
            mp03Id = InsertPoint(mp03)
            mp12Id = InsertPoint(mp12)
            mp13Id = InsertPoint(mp13)
            mp23Id = InsertPoint(mp23)

            ct012Id = InsertPoint(ct012)
            ct013Id = InsertPoint(ct013)
            ct023Id = InsertPoint(ct023)
            ct123Id = InsertPoint(ct123)

            gId = InsertPoint(g)

            InsertHexa(mp01Id,pt1Id,mp12Id,ct012Id,ct013Id,mp13Id,ct123Id,gId)
            InsertHexa(mp12Id,pt2Id,mp02Id,ct012Id,ct123Id,mp23Id,ct023Id,gId)
            InsertHexa(mp02Id,pt0Id,mp01Id,ct012Id,ct023Id,mp03Id,ct013Id,gId)
            InsertHexa(mp23Id,pt3Id,mp03Id,ct023Id,ct123Id,mp13Id,ct013Id,gId)

            for j in range(4):
                self.CellEntityIdsArray.InsertNextTuple1(tetraEntityId)

            # print toolbar
            if ( i % ( int( inputCells / toolbar_width ) ) == 0 ):
                sys.stdout.write("-")
                sys.stdout.flush()


        triType = 5 # Triangles
        triIdArray = vtk.vtkIdTypeArray()
        inputMesh.GetIdsOfCellsOfType(triType,triIdArray) # extract all the tetra cell
        numberOfTris = triIdArray.GetNumberOfTuples()

        for i in range(numberOfTris):

            triId = triIdArray.GetValue(i) 
            tri = inputMesh.GetCell(triId)
            if inputCellEntityIdsArray != None:
                triEntityId = inputCellEntityIdsArray.GetValue(triId)
            else:
                triEntityId = 2
            triPointIds = tri.GetPointIds()

            pt0Id = triPointIds.GetId(0)
            pt1Id = triPointIds.GetId(1)
            pt2Id = triPointIds.GetId(2)

            pt0 = np.array(meshPoints.GetPoint(pt0Id))
            pt1 = np.array(meshPoints.GetPoint(pt1Id))
            pt2 = np.array(meshPoints.GetPoint(pt2Id))

            mp01 = (pt0 + pt1) / 2.0
            mp02 = (pt0 + pt2) / 2.0
            mp12 = (pt1 + pt2) / 2.0

            g = (pt0 + pt1 + pt2) / 3.0

            mp01Id = InsertPoint(mp01)
            mp02Id = InsertPoint(mp02)
            mp12Id = InsertPoint(mp12)

            gId = InsertPoint(g)

            InsertQuad(mp02Id,pt0Id,mp01Id,gId)
            InsertQuad(mp01Id,pt1Id,mp12Id,gId)
            InsertQuad(mp12Id,pt2Id,mp02Id,gId)

            for j in range(3):
                self.CellEntityIdsArray.InsertNextTuple1(triEntityId)

            # print toolbar
            if ( (i + numberOfTetras) % ( int( inputCells / toolbar_width ) ) == 0 ):
                sys.stdout.write("-")
                sys.stdout.flush()


        def SplitLine(inputMesh,cellCounterShift=0):
            lineType = 3 # Lines
            lineIdArray = vtk.vtkIdTypeArray()
            inputMesh.GetIdsOfCellsOfType(lineType,lineIdArray) # extract all the line cell
            numberOfLines = lineIdArray.GetNumberOfTuples()

            for i in range(numberOfLines):

                lineId = lineIdArray.GetValue(i) 
                line = inputMesh.GetCell(triId)
                if inputCellEntityIdsArray != None:
                    lineEntityId = inputCellEntityIdsArray.GetValue(lineId)
                else:
                    lineEntityId = 3
                linePointIds = line.GetPointIds()

                pt0Id = linePointIds.GetId(0)
                pt1Id = linePointIds.GetId(1)

                pt0 = np.array(meshPoints.GetPoint(pt0Id))
                pt1 = np.array(meshPoints.GetPoint(pt1Id))

                g = (pt0 + pt1) / 2.0

                gId = InsertPoint(g)

                InsertLine(pt0Id,gId)
                InsertLine(gId,pt1Id)

                for j in range(2):
                    self.CellEntityIdsArray.InsertNextTuple1(lineEntityId)

                # print toolbar
                if ( (i + cellCounterShift) % ( int( inputCells / toolbar_width ) ) == 0 ):
                    sys.stdout.write("-")
                    sys.stdout.flush()

        SplitLine(inputMesh,numberOfTetras+numberOfTris)

        # close toolbar
        sys.stdout.write("]\n\n")

        if self.Mesh.GetNumberOfCells() == 0:
            self.Mesh = inputMesh

        # perform a refine-by-splitting
        for i in range(self.NumberOfRefineBySplitting):

            self.PrintLog("    refine-by-splitting number "+str(i+1))
            # repeated code ...
            # initialize toolbar
            inputCells = self.Mesh.GetNumberOfCells()
            toolbar_width = min(40,inputCells)
            sys.stdout.write( "    [%s]" % (" " * toolbar_width) )
            sys.stdout.flush()
            sys.stdout.write( "\b" * ( toolbar_width + 1 ) ) # return to start of line, after '['

            inputMesh = self.Mesh
            inputCellEntityIdsArray = inputMesh.GetCellData().GetArray(self.CellEntityIdsArrayName)

            meshPoints = vtk.vtkPoints()
            meshPoints.DeepCopy(inputMesh.GetPoints())

            numberOfPoints = inputMesh.GetNumberOfPoints()
            self.GlobalPtId = numberOfPoints

            self.Mesh = vtk.vtkUnstructuredGrid()
            self.Mesh.SetPoints(meshPoints)
            self.CellEntityIdsArray = vtk.vtkIntArray()
            self.CellEntityIdsArray.SetName(self.CellEntityIdsArrayName)
            self.Mesh.GetCellData().AddArray(self.CellEntityIdsArray)
            numberOfCells = 0
            # ... till here

            SplitLine(inputMesh)

            quadType = 9 # Quad
            quadIdArray = vtk.vtkIdTypeArray()
            inputMesh.GetIdsOfCellsOfType(quadType,quadIdArray) # extract all the tetra cell
            numberOfQuads = quadIdArray.GetNumberOfTuples()

            for i in range(numberOfQuads):

                quadId = quadIdArray.GetValue(i) 
                quad = inputMesh.GetCell(quadId)
                if inputCellEntityIdsArray != None:
                    quadEntityId = inputCellEntityIdsArray.GetValue(quadId)
                else:
                    quadEntityId = 2
                quadPointIds = quad.GetPointIds()

                pt0Id = quadPointIds.GetId(0)
                pt1Id = quadPointIds.GetId(1)
                pt2Id = quadPointIds.GetId(2)
                pt3Id = quadPointIds.GetId(3)

                pt0 = np.array(meshPoints.GetPoint(pt0Id))
                pt1 = np.array(meshPoints.GetPoint(pt1Id))
                pt2 = np.array(meshPoints.GetPoint(pt2Id))
                pt3 = np.array(meshPoints.GetPoint(pt3Id))

                mp01 = (pt0 + pt1) / 2.0
                mp03 = (pt0 + pt3) / 2.0
                mp12 = (pt1 + pt2) / 2.0
                mp23 = (pt2 + pt3) / 2.0

                g = (pt0 + pt1 + pt2 + pt3) / 4.0

                mp01Id = InsertPoint(mp01)
                mp03Id = InsertPoint(mp03)
                mp12Id = InsertPoint(mp12)
                mp23Id = InsertPoint(mp23)

                gId = InsertPoint(g)

                InsertQuad(mp03Id,pt0Id,mp01Id,gId)
                InsertQuad(mp01Id,pt1Id,mp12Id,gId)
                InsertQuad(mp12Id,pt2Id,mp23Id,gId)
                InsertQuad(mp23Id,pt3Id,mp03Id,gId)

                for j in range(4):
                    self.CellEntityIdsArray.InsertNextTuple1(quadEntityId)

                # print toolbar
                if ( (i + numberOfQuads) % ( int( inputCells / toolbar_width ) ) == 0 ):
                    sys.stdout.write("-")
                    sys.stdout.flush()

            # TODO: divide hexahedra

            # close toolbar
            sys.stdout.write("]\n\n")


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()

