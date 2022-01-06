/*=========================================================================
                                                                                                                                    
Program:   VMTK
Module:    $RCSfile: vtkvmtkDolfinWriter2.cxx,v $
Language:  C++
Date:      $$
Version:   $$
                                                                                                                                    
  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
     
  Note: this class was contributed by 
	Tangui Morvan
	Kalkulo AS
	Simula Research Laboratory
                                                                                                                                    
=========================================================================*/

// #include <fstream>

#include "vtkvmtkDolfinWriter2.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellType.h"
#include "vtkCell.h"
#include "vtkCellData.h"
#include "vtkIdTypeArray.h"
#include "vtkObjectFactory.h"
#include "vtkSortDataArray.h"
#include "vtkTetra.h"
#include "vtkvmtkConstants.h"


vtkStandardNewMacro(vtkvmtkDolfinWriter2);

vtkvmtkDolfinWriter2::vtkvmtkDolfinWriter2()
{
  this->CellEntityIdsArrayName = NULL;
  this->CellEntityIdsOffset = 0;
}

vtkvmtkDolfinWriter2::~vtkvmtkDolfinWriter2()
{
  if (this->CellEntityIdsArrayName)
    {
    delete[] this->CellEntityIdsArrayName;
    this->CellEntityIdsArrayName = NULL;
    }
}

void vtkvmtkDolfinWriter2::WriteData()
{
  vtkUnstructuredGrid *input= vtkUnstructuredGrid::SafeDownCast(this->GetInput());

  if (!this->FileName)
    {
    vtkErrorMacro(<<"FileName not set.");
    return;
    }
        
  std::ofstream out (this->FileName);

  if (!out.good())
    {
    vtkErrorMacro(<<"Could not open file for writing.");
    return;
    }
  
  input->BuildLinks();

  int numberOfPoints = input->GetNumberOfPoints();
  int numberOfCells = input->GetNumberOfCells();

  vtkIdTypeArray* cellEntityIdsArray = NULL;
  if (this->CellEntityIdsArrayName)
    {
    if (input->GetCellData()->GetArray(this->CellEntityIdsArrayName))
      {
      cellEntityIdsArray = vtkIdTypeArray::New();
      cellEntityIdsArray->DeepCopy(input->GetCellData()->GetArray(this->CellEntityIdsArrayName));
      }
    else
      {
      vtkErrorMacro(<<"BoundaryDataArray with name specified does not exist");
      }
    }

  vtkIdTypeArray* tetraCellIdArray = vtkIdTypeArray::New();
  input->GetIdsOfCellsOfType(VTK_TETRA,tetraCellIdArray);
  int numberOfTetras = tetraCellIdArray->GetNumberOfTuples();

  out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl << endl;
  out << "<dolfin xmlns:dolfin=\"http://www.phi.chalmers.se/dolfin/\">" << endl;
  out << "  <mesh celltype=\"tetrahedron\" dim=\"3\">" << endl;
  out << "    <vertices size=\""<< numberOfPoints << "\">" << endl;

  int i;
  double point[3];
  for (i=0; i<numberOfPoints; i++)
    {
    input->GetPoint(i,point);
    out << "      <vertex index=\"" << i << "\" ";
    out << "x=\"" << point[0] << "\" ";
    out << "y=\"" << point[1] << "\" ";
    out << "z=\"" << point[2] << "\" ";
    out << "/>" <<endl;
    }

  out << "    </vertices>" << endl;
  out << "    <cells size=\"" << numberOfTetras << "\">" << endl;

  vtkIdList* dolfinConnectivity = vtkIdList::New();
//   this->GetDolfinConnectivity(VTK_TETRA,dolfinConnectivity);

  vtkIdList* volumeCellIdMap = vtkIdList::New();
  volumeCellIdMap->SetNumberOfIds(numberOfCells);
  int numberOfCellPoints = 4;
  int k;
  
  for (i=0; i<numberOfTetras; i++)
    {
    vtkIdType tetraCellId = tetraCellIdArray->GetValue(i);
    volumeCellIdMap->SetId(tetraCellId,i);
    vtkCell* tetra = input->GetCell(tetraCellId);
    this->GetDolfinCellConnectivity(tetra,dolfinConnectivity);
    vtkIdList* cellPointIds = input->GetCell(tetraCellId)->GetPointIds();
    
    out << "      <tetrahedron index=\"" << i << "\" "; 
    for (k=0; k<numberOfCellPoints; k++)
      {
      out << "v" << k << "=\"" << cellPointIds->GetId(dolfinConnectivity->GetId(k)) << "\" ";
      }
    out << "/>" << endl;
    }
  
  out << "    </cells>" << endl;

  if (cellEntityIdsArray)
    {
    
    
    out << "    <data>" << endl;
    
    //Set the id of the tetrahedra
    out << "      <array name=\"region indicators\" type=\"uint\" size=\"" << numberOfTetras << "\">" << endl;
    
    for (i=0; i<numberOfTetras; i++)
      {
      vtkIdType tetraCellId = tetraCellIdArray->GetValue(i);
      out << "        <element index=\"" << i << "\" "; 
      out << "value=\"" << cellEntityIdsArray->GetValue(tetraCellId) + this->CellEntityIdsOffset << "\" "; 
      out << "/>" << endl;
      }
    out << "      </array>" << endl;
    
    vtkIdTypeArray* triangleCellIdArray = vtkIdTypeArray::New();
    input->GetIdsOfCellsOfType(VTK_TRIANGLE,triangleCellIdArray);
    int numberOfTriangles = triangleCellIdArray->GetNumberOfTuples();
    vtkIdList* boundaryFaceCells = vtkIdList::New();
//     boundaryFaceCells->SetNumberOfIds(numberOfTriangles);
    vtkIdList* boundaryFaceIds = vtkIdList::New();
//     boundaryFaceIds->SetNumberOfIds(numberOfTriangles);
    vtkIdList* boundaryFaceIndicators = vtkIdList::New();

    vtkIdType triangleCellId;
    for (i=0; i<numberOfTriangles; i++)
      {
      triangleCellId = triangleCellIdArray->GetValue(i);
      vtkIdList* faceCellPoints = vtkIdList::New();
      input->GetCellPoints(triangleCellId,faceCellPoints);

      vtkIdList* cellIds = vtkIdList::New();
      input->GetCellNeighbors(triangleCellId,faceCellPoints,cellIds);
      vtkIdType nNeighbors = cellIds->GetNumberOfIds();

      for (int j=0; j<nNeighbors; j++)
        {
        vtkIdType cellId = cellIds->GetId(j);
        vtkCell* cell = input->GetCell(cellId);
        int cellType = cell->GetCellType();
        
        if (cellType != VTK_TETRA)
          {
          vtkErrorMacro(<<"Volume cell adjacent to triangle is not tetrahedron (volume cell id: "<<cellId <<") and it is unsupported by Dolfin. Skipping face.");
          continue;
          }
          
          
        int numberOfFaces = cell->GetNumberOfFaces();
        vtkIdType faceId = -1;
        int k;
        for (k=0; k<numberOfFaces; k++)
          {
          vtkCell* face = cell->GetFace(k);
          vtkIdList* matchingPointIds = vtkIdList::New();
          matchingPointIds->DeepCopy(face->GetPointIds());
          matchingPointIds->IntersectWith(*faceCellPoints);
          int numberOfNonMatching = face->GetNumberOfPoints() - matchingPointIds->GetNumberOfIds();
          matchingPointIds->Delete();

          if (numberOfNonMatching==0)
            {
            faceId = k;
            break;
            }
          }

        vtkIdList* dolfinFaceOrder = vtkIdList::New();
//         this->GetDolfinFaceOrder(cellType,dolfinFaceOrder);
        this->GetDolfinCellFaceOrder(cell,dolfinFaceOrder);
        vtkIdType dolfinFaceId = dolfinFaceOrder->GetId(faceId);
        dolfinFaceOrder->Delete();

        boundaryFaceCells->InsertNextId(volumeCellIdMap->GetId(cellId));
        boundaryFaceIds->InsertNextId(dolfinFaceId);
        boundaryFaceIndicators->InsertNextId(cellEntityIdsArray->GetValue(triangleCellId) + this->CellEntityIdsOffset);
        }
      
      faceCellPoints->Delete();
      cellIds->Delete();
      }

     vtkIdType numberOfBoundaryFacets = boundaryFaceCells->GetNumberOfIds();
    
    
    out << "      <array name=\"boundary facet cells\" type=\"uint\" size=\"" << numberOfBoundaryFacets << "\">" << endl;
    for (i=0; i<numberOfBoundaryFacets; i++)
      {
      out << "        <element index=\"" << i << "\" "; 
      out << "value=\"" << boundaryFaceCells->GetId(i) << "\" "; 
      out << "/>" << endl;
      }
    out << "      </array>" << endl;

    out << "      <array name=\"boundary facet numbers\" type=\"uint\" size=\"" << numberOfBoundaryFacets << "\">" << endl;
    for (i=0; i<numberOfBoundaryFacets; i++)
      {
      out << "        <element index=\"" << i << "\" "; 
      out << "value=\"" << boundaryFaceIds->GetId(i) << "\" "; 
      out << "/>" << endl;
      }
    out << "      </array>" << endl;

    out << "      <array name=\"boundary indicators\" type=\"uint\" size=\"" << numberOfBoundaryFacets << "\">" << endl;
    for (i=0; i<numberOfBoundaryFacets; i++)
      {
      out << "        <element index=\"" << i << "\" "; 
      out << "value=\"" << boundaryFaceIndicators->GetId(i) << "\" "; 
      out << "/>" << endl;
      }
    out << "      </array>" << endl;
  
    out << "    </data>" << endl;
  
    triangleCellIdArray->Delete();
    boundaryFaceCells->Delete();
    boundaryFaceIds->Delete();
    boundaryFaceIndicators->Delete();
    }

  out << "  </mesh>" << endl;
  out << "</dolfin>" << endl;

  tetraCellIdArray->Delete();
  dolfinConnectivity->Delete();
  volumeCellIdMap->Delete();

  if (cellEntityIdsArray)
    {
    cellEntityIdsArray->Delete();
    }
}




void vtkvmtkDolfinWriter2::GetDolfinConnectivity(int cellType, vtkIdList* dolfinConnectivity)
{
  dolfinConnectivity->Initialize();

  switch(cellType)
    {
    case VTK_TETRA:
      dolfinConnectivity->SetNumberOfIds(4);
      dolfinConnectivity->SetId(0,0);
      dolfinConnectivity->SetId(1,1);
      dolfinConnectivity->SetId(2,2);
      dolfinConnectivity->SetId(3,3);
      break;
    default:
      cerr<<"Element type not currently supported in dolfin. Skipping element for connectivity."<<endl;
      break;
    }
}

void vtkvmtkDolfinWriter2::GetDolfinFaceOrder(int cellType, vtkIdList* dolfinFaceOrder)
{
  dolfinFaceOrder->Initialize();

  switch(cellType)
    {
    case VTK_TETRA:
      dolfinFaceOrder->SetNumberOfIds(4);
      dolfinFaceOrder->SetId(0,2);
      dolfinFaceOrder->SetId(1,0);
      dolfinFaceOrder->SetId(2,1);
      dolfinFaceOrder->SetId(3,3);
      break;
    default:
      cerr<<"Element type not currently supported in dolfin. Skipping element for face ordering."<<endl;
      break;
    }
}

void vtkvmtkDolfinWriter2::GetDolfinCellConnectivity(vtkCell* cell, vtkIdList* dolfinConnectivity)
{
  dolfinConnectivity->Initialize();
  int cellType = cell->GetCellType();
  vtkIdTypeArray* pointIds = vtkIdTypeArray::New();
  switch(cellType)
    {
    case VTK_TETRA:
      pointIds->SetNumberOfTuples(4);
      pointIds->SetValue(0,cell->GetPointId(0));
      pointIds->SetValue(1,cell->GetPointId(1));
      pointIds->SetValue(2,cell->GetPointId(2));
      pointIds->SetValue(3,cell->GetPointId(3));
      dolfinConnectivity->SetNumberOfIds(4);
      dolfinConnectivity->SetId(0,0);
      dolfinConnectivity->SetId(1,1);
      dolfinConnectivity->SetId(2,2);
      dolfinConnectivity->SetId(3,3);
      vtkSortDataArray::Sort(pointIds,dolfinConnectivity);
      break;
    default:
      cerr<<"Element type not currently supported in dolfin. Skipping element for connectivity."<<endl;
      break;
    }
  pointIds->Delete();
}


void vtkvmtkDolfinWriter2::GetDolfinCellFaceOrder(vtkCell* cell, vtkIdList *dolfinFaceOrder) 
{
  dolfinFaceOrder->Initialize();
  
  int cellType = cell->GetCellType();
  
  vtkIdList* dolfinConnectivity = vtkIdList::New();
  GetDolfinCellConnectivity(cell,dolfinConnectivity);
  vtkIdList* inverseDolfinConnectivity = vtkIdList::New();
  inverseDolfinConnectivity->SetNumberOfIds(dolfinConnectivity->GetNumberOfIds());
  for (int i=0; i<dolfinConnectivity->GetNumberOfIds(); i++)
    {
    inverseDolfinConnectivity->SetId(dolfinConnectivity->GetId(i),i);
    }
  
  
  const vtkIdType *faceArray;
  switch(cellType)
    {
    case VTK_TETRA:
      dolfinFaceOrder->SetNumberOfIds(4);
      for (int i=0; i<4; i++)
        {
        faceArray = vtkTetra::GetFaceArray(i);
        for (int j=0; j<4;j++)
          {
          bool inFaceArray = false;
          for (int k=0;k<3;k++) 
            {
            if (inverseDolfinConnectivity->GetId(faceArray[k])==j)
              {
                inFaceArray = true;
              }
            }
          if (!inFaceArray)
            {
            dolfinFaceOrder->SetId(i,j);
            break;
            }
          }
        }
      break;
    default:
      cerr<<"Element type not currently supported in dolfin. Skipping element for face ordering."<<endl;
      break;
    }
    
  dolfinConnectivity->Delete();
}

void vtkvmtkDolfinWriter2::PrintSelf(std::ostream& os, vtkIndent indent)
{
  vtkUnstructuredGridWriter::PrintSelf(os,indent);
}
