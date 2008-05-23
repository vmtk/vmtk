/*=========================================================================
                                                                                                                                    
Program:   VMTK
Module:    $RCSfile: vtkvmtkDolfinWriter.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:47 $
Version:   $Revision: 1.6 $
                                                                                                                                    
  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
                                                                                                                                    
=========================================================================*/

// #include <fstream>

#include "vtkvmtkDolfinWriter.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellType.h"
#include "vtkCell.h"
#include "vtkCellData.h"
#include "vtkIdTypeArray.h"
#include "vtkObjectFactory.h"
#include "vtkvmtkConstants.h"

vtkCxxRevisionMacro(vtkvmtkDolfinWriter, "$Revision: 1.6 $");
vtkStandardNewMacro(vtkvmtkDolfinWriter);

vtkvmtkDolfinWriter::vtkvmtkDolfinWriter()
{
  this->BoundaryDataArrayName = NULL;
  this->BoundaryDataIdOffset = 0;
}

vtkvmtkDolfinWriter::~vtkvmtkDolfinWriter()
{
  if (this->BoundaryDataArrayName)
    {
    delete[] this->BoundaryDataArrayName;
    this->BoundaryDataArrayName = NULL;
    }
}

void vtkvmtkDolfinWriter::WriteData()
{
  vtkUnstructuredGrid *input= vtkUnstructuredGrid::SafeDownCast(this->GetInput());

  if (!this->FileName)
    {
    vtkErrorMacro(<<"FileName not set.");
    return;
    }
        
  ofstream out (this->FileName);

  if (!out.good())
    {
    vtkErrorMacro(<<"Could not open file for writing.");
    return;
    }
  
  input->BuildLinks();

  int numberOfPoints = input->GetNumberOfPoints();
  int numberOfCells = input->GetNumberOfCells();

  vtkIdTypeArray* boundaryDataArray = NULL;
  if (this->BoundaryDataArrayName)
    {
    if (input->GetCellData()->GetArray(this->BoundaryDataArrayName))
      {
      boundaryDataArray = vtkIdTypeArray::New();
      boundaryDataArray->DeepCopy(input->GetCellData()->GetArray(this->BoundaryDataArrayName));
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
  this->GetDolfinConnectivity(VTK_TETRA,dolfinConnectivity);

  vtkIdList* volumeCellIdMap = vtkIdList::New();
  volumeCellIdMap->SetNumberOfIds(numberOfCells);
  int numberOfCellPoints = 4;
  int k;
  for (i=0; i<numberOfTetras; i++)
    {
    vtkIdType tetraCellId = tetraCellIdArray->GetValue(i);
    volumeCellIdMap->SetId(tetraCellId,i);
    vtkIdList* cellPointIds = input->GetCell(tetraCellId)->GetPointIds();
    out << "      <tetrahedron index=\"" << i << "\" "; 
    for (k=0; k<numberOfCellPoints; k++)
      {
      out << "v" << k << "=\"" << cellPointIds->GetId(dolfinConnectivity->GetId(k)) << "\" ";
      }
    out << "/>" << endl;
    }
  
  out << "    </cells>" << endl;

  if (boundaryDataArray)
    {
    vtkIdTypeArray* triangleCellIdArray = vtkIdTypeArray::New();
    input->GetIdsOfCellsOfType(VTK_TRIANGLE,triangleCellIdArray);
    int numberOfTriangles = triangleCellIdArray->GetNumberOfTuples();
    vtkIdList* boundaryFaceCells = vtkIdList::New();
    boundaryFaceCells->SetNumberOfIds(numberOfTriangles);
    vtkIdList* boundaryFaceIds = vtkIdList::New();
    boundaryFaceIds->SetNumberOfIds(numberOfTriangles);

    vtkIdType triangleCellId;
    for (i=0; i<numberOfTriangles; i++)
      {
      triangleCellId = triangleCellIdArray->GetValue(i);
      vtkIdList* faceCellPoints = vtkIdList::New();
      input->GetCellPoints(triangleCellId,faceCellPoints);

      vtkIdList* cellIds = vtkIdList::New();
      input->GetCellNeighbors(triangleCellId,faceCellPoints,cellIds);

      if (cellIds->GetNumberOfIds() != 1)
        {
        vtkWarningMacro("Boundary cell not on boundary!");
        }

      vtkIdType cellId = cellIds->GetId(0);
      vtkCell* cell = input->GetCell(cellId);
      int cellType = cell->GetCellType();
      int numberOfFaces = cell->GetNumberOfFaces();
      vtkIdType faceId = -1;
      int j;
      for (j=0; j<numberOfFaces; j++)
        {
        vtkCell* face = cell->GetFace(j);
        vtkIdList* matchingPointIds = vtkIdList::New();
        matchingPointIds->DeepCopy(face->GetPointIds());
        matchingPointIds->IntersectWith(*faceCellPoints);
        int numberOfNonMatching = face->GetNumberOfPoints() - matchingPointIds->GetNumberOfIds();
        matchingPointIds->Delete();

        if (numberOfNonMatching==0)
          {
          faceId = j;
          break;
          }
        }

      vtkIdList* dolfinFaceOrder = vtkIdList::New();
      this->GetDolfinFaceOrder(cellType,dolfinFaceOrder);
      vtkIdType dolfinFaceId = dolfinFaceOrder->GetId(faceId);
      dolfinFaceOrder->Delete();

      boundaryFaceCells->SetId(i,volumeCellIdMap->GetId(cellId));
      boundaryFaceIds->SetId(i,dolfinFaceId);

      faceCellPoints->Delete();
      }

    out << "    <data>" << endl;
    out << "      <array name=\"boundary facet cells\" type=\"uint\" size=\"" << numberOfTriangles << "\">" << endl;
    for (i=0; i<numberOfTriangles; i++)
      {
      triangleCellId = triangleCellIdArray->GetValue(i);
      out << "        <element index=\"" << i << "\" "; 
      out << "value=\"" << boundaryFaceCells->GetId(triangleCellId) << "\" "; 
      out << "/>" << endl;
      }
    out << "      </array>" << endl;

    out << "      <array name=\"boundary facet numbers\" type=\"uint\" size=\"" << numberOfTriangles << "\">" << endl;
    for (i=0; i<numberOfTriangles; i++)
      {
      triangleCellId = triangleCellIdArray->GetValue(i);
      out << "        <element index=\"" << i << "\" "; 
      out << "value=\"" << boundaryFaceIds->GetId(triangleCellId) << "\" "; 
      out << "/>" << endl;
      }
    out << "      </array>" << endl;

    out << "      <array name=\"boundary indicators\" type=\"uint\" size=\"" << numberOfTriangles << "\">" << endl;
    for (i=0; i<numberOfTriangles; i++)
      {
      triangleCellId = triangleCellIdArray->GetValue(i);
      out << "        <element index=\"" << i << "\" "; 
      out << "value=\"" << boundaryDataArray->GetValue(triangleCellId) + this->BoundaryDataIdOffset << "\" "; 
      out << "/>" << endl;
      }
    out << "      </array>" << endl;
  
    out << "    </data>" << endl;
  
    triangleCellIdArray->Delete();
    boundaryFaceCells->Delete();
    boundaryFaceIds->Delete();
    }

  out << "  </mesh>" << endl;
  out << "</dolfin>" << endl;

  tetraCellIdArray->Delete();
  dolfinConnectivity->Delete();
  volumeCellIdMap->Delete();

  if (boundaryDataArray)
    {
    boundaryDataArray->Delete();
    }
}

void vtkvmtkDolfinWriter::GetDolfinConnectivity(int cellType, vtkIdList* dolfinConnectivity)
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
      cerr<<"Element type not currently supported in dolfin. Skipping element."<<endl;
      break;
    }
}

void vtkvmtkDolfinWriter::GetDolfinFaceOrder(int cellType, vtkIdList* dolfinFaceOrder)
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
      cerr<<"Element type not currently supported in dolfin. Skipping element."<<endl;
      break;
    }
}

void vtkvmtkDolfinWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkUnstructuredGridWriter::PrintSelf(os,indent);
}
