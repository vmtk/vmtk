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

        tetraPoints = vtk.vtkPoints()
        tetraPoints.SetNumberOfPoints(4)
        tetraPoints.InsertPoint(0, 0, 0, 0)
        tetraPoints.InsertPoint(1, 1, 0, 0)
        tetraPoints.InsertPoint(2, .5, 1, 0)
        tetraPoints.InsertPoint(3, .5, .5, 1)
        aTetra = vtk.vtkTetra()
        aTetra.GetPointIds().SetId(0, 0)
        aTetra.GetPointIds().SetId(1, 1)
        aTetra.GetPointIds().SetId(2, 2)
        aTetra.GetPointIds().SetId(3, 3)
        aTetraGrid = vtk.vtkUnstructuredGrid()
        aTetraGrid.Allocate(1, 1)
        aTetraGrid.InsertNextCell(aTetra.GetCellType(), aTetra.GetPointIds())
        aTetraGrid.SetPoints(tetraPoints)

        inputMesh = aTetraGrid
        print('Input number of cells:',inputMesh.GetNumberOfCells())

        tetraCellType = 10 # Tetrahedra
        tetraCellIdArray = vtk.vtkIdTypeArray()
        inputMesh.GetIdsOfCellsOfType(tetraCellType,tetraCellIdArray) # extract all the tetra cell

        meshPoints = vtk.vtkPoints()
        meshPoints.DeepCopy(inputMesh.GetPoints())

        numberOfPoints = inputMesh.GetNumberOfPoints()
        globalPtId = numberOfPoints

        self.Mesh = vtk.vtkUnstructuredGrid()
        self.Mesh.SetPoints(meshPoints)

        numberOfTetras = tetraCellIdArray.GetNumberOfTuples()

        for i in range(numberOfTetras):

            tetraId = tetraCellIdArray.GetValue(i) 
            tetra = inputMesh.GetCell(tetraId)
            tetraPointIds = tetra.GetPointIds()

            ptId0 = tetraPointIds.GetId(0)
            pt1Id = tetraPointIds.GetId(1)
            pt2Id = tetraPointIds.GetId(2)
            pt3Id = tetraPointIds.GetId(3)

            pt0 = np.array(meshPoints.GetPoint(ptId0))
            pt1 = np.array(meshPoints.GetPoint(pt1Id))
            pt2 = np.array(meshPoints.GetPoint(pt2Id))
            pt3 = np.array(meshPoints.GetPoint(pt3Id))

            mp01 = (pt0 + pt1) / 2.0
            mp02 = (pt0 + pt2) / 2.0
            mp03 = (pt0 + pt3) / 2.0
            mp12 = (pt1 + pt2) / 2.0
            mp13 = (pt1 + pt3) / 2.0
            mp23 = (pt2 + pt3) / 2.0

            # use barycenter
            ct012 = (pt0 + pt1 + pt2) / 3.0
            ct013 = (pt0 + pt1 + pt3) / 3.0
            ct023 = (pt0 + pt2 + pt3) / 3.0
            ct123 = (pt1 + pt2 + pt3) / 3.0

            # use barycenter
            g = (pt0 + pt1 + pt2 + pt3) / 4.0

            face0 = tetra.GetFace(0)
            face1 = tetra.GetFace(1)
            face2 = tetra.GetFace(2)
            face3 = tetra.GetFace(3)

            print('Tetrahedra:',i)
            print('\tpointIds:',ptId0,pt1Id,pt2Id,pt3Id)
            print('\tpoints:\n\t\t',pt0,'\n\t\t',pt1,'\n\t\t',pt2,'\n\t\t',pt3)
            print('\tmiddle points:\n\t\t',mp01,'\n\t\t',mp02,'\n\t\t',mp03,'\n\t\t',mp12,'\n\t\t',mp13,'\n\t\t',mp23)
            print('\tface centers:\n\t\t',ct012,'\n\t\t',ct013,'\n\t\t',ct023,'\n\t\t',ct123)
            print('\ttetra center:\n\t\t',g)

            meshPoints.InsertPoint(globalPtId, mp01)
            mp01Id = globalPtId
            globalPtId += 1
            meshPoints.InsertPoint(globalPtId, mp02)
            mp02Id = globalPtId
            globalPtId += 1
            meshPoints.InsertPoint(globalPtId, mp03)
            mp03Id = globalPtId
            globalPtId += 1
            meshPoints.InsertPoint(globalPtId, mp12)
            mp12Id = globalPtId
            globalPtId += 1
            meshPoints.InsertPoint(globalPtId, mp13)
            mp13Id = globalPtId
            globalPtId += 1
            meshPoints.InsertPoint(globalPtId, mp23)
            mp23Id = globalPtId
            globalPtId += 1

            meshPoints.InsertPoint(globalPtId, ct012)
            ct012Id = globalPtId
            globalPtId += 1
            meshPoints.InsertPoint(globalPtId, ct013)
            ct013Id = globalPtId
            globalPtId += 1
            meshPoints.InsertPoint(globalPtId, ct023)
            ct023Id = globalPtId
            globalPtId += 1
            meshPoints.InsertPoint(globalPtId, ct123)
            ct123Id = globalPtId
            globalPtId += 1

            meshPoints.InsertPoint(globalPtId, g)
            gId = globalPtId
            globalPtId += 1

            print(mp01Id,mp02Id,mp03Id,mp12Id,mp13Id,mp23Id,ct012Id,ct013Id,ct023Id,ct123Id,gId)

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
            hexa3.GetPointIds().SetId(1,ptId0)
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

        print('Final number of cells:',self.Mesh.GetNumberOfCells())


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()

