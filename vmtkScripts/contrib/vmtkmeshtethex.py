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
        self.GlobalPtId = 0
        self.NewPointSet = dict()

        self.SetScriptName('vmtkmeshtethex')
        self.SetScriptDoc('generate hexahedral mesh from a tetrahedral one, splitting each triangle in three quads and each tetrahedron in four hexahedron')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['CellEntityIdsArrayName','entityidsarray','str',1]
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

        inputMesh = self.Mesh
        print('Input number of cells:',inputMesh.GetNumberOfCells())

        meshPoints = vtk.vtkPoints()
        meshPoints.DeepCopy(inputMesh.GetPoints())

        numberOfPoints = inputMesh.GetNumberOfPoints()
        self.GlobalPtId = numberOfPoints

        self.Mesh = vtk.vtkUnstructuredGrid()
        self.Mesh.SetPoints(meshPoints)

        tetraType = 10 # Tetrahedra
        tetraIdArray = vtk.vtkIdTypeArray()
        inputMesh.GetIdsOfCellsOfType(tetraType,tetraIdArray) # extract all the tetra cell
        numberOfTetras = tetraIdArray.GetNumberOfTuples()

        for i in range(numberOfTetras):

            tetraId = tetraIdArray.GetValue(i) 
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

            hexa1 = vtk.vtkHexahedron()
            hexa1.GetPointIds().SetId(0,mp01Id)
            hexa1.GetPointIds().SetId(1,pt1Id)
            hexa1.GetPointIds().SetId(2,mp12Id)
            hexa1.GetPointIds().SetId(3,ct012Id)
            hexa1.GetPointIds().SetId(4,ct013Id)
            hexa1.GetPointIds().SetId(5,mp13Id)
            hexa1.GetPointIds().SetId(6,ct123Id)
            hexa1.GetPointIds().SetId(7,gId)

            hexa2 = vtk.vtkHexahedron()
            hexa2.GetPointIds().SetId(0,mp12Id)
            hexa2.GetPointIds().SetId(1,pt2Id)
            hexa2.GetPointIds().SetId(2,mp02Id)
            hexa2.GetPointIds().SetId(3,ct012Id)
            hexa2.GetPointIds().SetId(4,ct123Id)
            hexa2.GetPointIds().SetId(5,mp23Id)
            hexa2.GetPointIds().SetId(6,ct023Id)
            hexa2.GetPointIds().SetId(7,gId)

            hexa3 = vtk.vtkHexahedron()
            hexa3.GetPointIds().SetId(0,mp02Id)
            hexa3.GetPointIds().SetId(1,pt0Id)
            hexa3.GetPointIds().SetId(2,mp01Id)
            hexa3.GetPointIds().SetId(3,ct012Id)
            hexa3.GetPointIds().SetId(4,ct023Id)
            hexa3.GetPointIds().SetId(5,mp03Id)
            hexa3.GetPointIds().SetId(6,ct013Id)
            hexa3.GetPointIds().SetId(7,gId)

            hexa4 = vtk.vtkHexahedron()
            hexa4.GetPointIds().SetId(0,mp23Id)
            hexa4.GetPointIds().SetId(1,pt3Id)
            hexa4.GetPointIds().SetId(2,mp03Id)
            hexa4.GetPointIds().SetId(3,ct023Id)
            hexa4.GetPointIds().SetId(4,ct123Id)
            hexa4.GetPointIds().SetId(5,mp13Id)
            hexa4.GetPointIds().SetId(6,ct013Id)
            hexa4.GetPointIds().SetId(7,gId)

            self.Mesh.InsertNextCell(hexa1.GetCellType(),hexa1.GetPointIds())
            self.Mesh.InsertNextCell(hexa2.GetCellType(),hexa2.GetPointIds())
            self.Mesh.InsertNextCell(hexa3.GetCellType(),hexa3.GetPointIds())
            self.Mesh.InsertNextCell(hexa4.GetCellType(),hexa4.GetPointIds())

        triType = 5 # Triangles
        triIdArray = vtk.vtkIdTypeArray()
        inputMesh.GetIdsOfCellsOfType(triType,triIdArray) # extract all the tetra cell
        numberOfTris = triIdArray.GetNumberOfTuples()

        for i in range(numberOfTris):

            triId = triIdArray.GetValue(i) 
            tri = inputMesh.GetCell(triId)
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

            quad1 = vtk.vtkQuad()
            quad1.GetPointIds().SetId(0,mp02Id)
            quad1.GetPointIds().SetId(1,pt0Id)
            quad1.GetPointIds().SetId(2,mp01Id)
            quad1.GetPointIds().SetId(3,gId)

            quad2 = vtk.vtkQuad()
            quad2.GetPointIds().SetId(0,mp01Id)
            quad2.GetPointIds().SetId(1,pt1Id)
            quad2.GetPointIds().SetId(2,mp12Id)
            quad2.GetPointIds().SetId(3,gId)

            quad3 = vtk.vtkQuad()
            quad3.GetPointIds().SetId(0,mp12Id)
            quad3.GetPointIds().SetId(1,pt2Id)
            quad3.GetPointIds().SetId(2,mp02Id)
            quad3.GetPointIds().SetId(3,gId)

            self.Mesh.InsertNextCell(quad1.GetCellType(),quad1.GetPointIds())
            self.Mesh.InsertNextCell(quad2.GetCellType(),quad2.GetPointIds())
            self.Mesh.InsertNextCell(quad3.GetCellType(),quad3.GetPointIds())

        lineType = 3 # Lines
        lineIdArray = vtk.vtkIdTypeArray()
        inputMesh.GetIdsOfCellsOfType(lineType,lineIdArray) # extract all the tetra cell
        numberOfLines = lineIdArray.GetNumberOfTuples()

        for i in range(numberOfLines):

            lineId = lineIdArray.GetValue(i) 
            line = inputMesh.GetCell(triId)
            linePointIds = line.GetPointIds()

            pt0Id = linePointIds.GetId(0)
            pt1Id = linePointIds.GetId(1)

            pt0 = np.array(meshPoints.GetPoint(pt0Id))
            pt1 = np.array(meshPoints.GetPoint(pt1Id))

            g = (pt0 + pt1) / 2.0

            gId = InsertPoint(g)

            line1 = vtk.vtkLine()
            line1.GetPointIds().SetId(0,pt0Id)
            line1.GetPointIds().SetId(1,gId)

            line2 = vtk.vtkLine()
            line2.GetPointIds().SetId(0,gId)
            line2.GetPointIds().SetId(1,pt1Id)

            self.Mesh.InsertNextCell(line1.GetCellType(),line1.GetPointIds())
            self.Mesh.InsertNextCell(line2.GetCellType(),line2.GetPointIds())

        print('Final number of cells:',self.Mesh.GetNumberOfCells())
        hexaIdArray = vtk.vtkIdTypeArray()
        self.Mesh.GetIdsOfCellsOfType(12,hexaIdArray)
        print('\tnumber of hexahedra:',hexaIdArray.GetNumberOfTuples())
        quadIdArray = vtk.vtkIdTypeArray()
        self.Mesh.GetIdsOfCellsOfType(9,quadIdArray)
        print('\tnumber of quads:',quadIdArray.GetNumberOfTuples())
        lineIdArray = vtk.vtkIdTypeArray()
        self.Mesh.GetIdsOfCellsOfType(3,lineIdArray)
        print('\tnumber of lines:',lineIdArray.GetNumberOfTuples())


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()

