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
#include <cassert>

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

static void GetDolfinConnectivity(int cellType, vtkIdList* dolfinConnectivity)
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
      cerr << "Element type not currently supported in dolfin. Skipping element for connectivity." << endl;
      break;
    }
}

static void GetDolfinFaceOrder(int cellType, vtkIdList* dolfinFaceOrder)
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
      cerr << "Element type not currently supported in dolfin. Skipping element for face ordering." << endl;
      break;
    }
}

vtkvmtkDolfinWriter::vtkvmtkDolfinWriter()
{
  this->BoundaryDataArrayName = NULL;
  this->BoundaryDataIdOffset = 0;
  this->StoreCellMarkers = 0;
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

  const int numberOfPoints = input->GetNumberOfPoints();
  const int numberOfCells = input->GetNumberOfCells();

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
  const int numberOfTetras = tetraCellIdArray->GetNumberOfTuples();

  out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl << endl;
  out << "<dolfin xmlns:dolfin=\"http://www.fenicsproject.org\">" << endl;
  out << "  <mesh celltype=\"tetrahedron\" dim=\"3\">" << endl;

  out << "    <vertices size=\""<< numberOfPoints << "\">" << endl;
  for (int i=0; i<numberOfPoints; i++)
    {
    double point[3];
    input->GetPoint(i,point);
    out << "      <vertex index=\"" << i << "\" ";
    out << "x=\"" << point[0] << "\" ";
    out << "y=\"" << point[1] << "\" ";
    out << "z=\"" << point[2] << "\" ";
    out << "/>" <<endl;
    }
  out << "    </vertices>" << endl;

  vtkIdList* dolfinConnectivity = vtkIdList::New();
  GetDolfinConnectivity(VTK_TETRA,dolfinConnectivity);

  vtkIdList* volumeCellIdMap = vtkIdList::New();
  volumeCellIdMap->SetNumberOfIds(numberOfCells);

  out << "    <cells size=\"" << numberOfTetras << "\">" << endl;
  for (int i=0; i<numberOfTetras; i++)
    {
    vtkIdType tetraCellId = tetraCellIdArray->GetValue(i);

    volumeCellIdMap->SetId(tetraCellId,i);

    vtkIdList* cellPointIds = input->GetCell(tetraCellId)->GetPointIds();
    out << "      <tetrahedron index=\"" << i << "\" "; 

    const int numberOfCellPoints = 4; // Points in a tetrahedron
    for (int k=0; k<numberOfCellPoints; k++)
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
    const int numberOfTriangles = triangleCellIdArray->GetNumberOfTuples();

    vtkIdList* boundaryFaceCells = vtkIdList::New();
    boundaryFaceCells->SetNumberOfIds(numberOfTriangles);

    vtkIdList* boundaryFaceIds = vtkIdList::New();
    boundaryFaceIds->SetNumberOfIds(numberOfTriangles);

    int interiorBoundaryFacetsFound = 0;
    int exteriorBoundaryFacetsFound = 0;
    for (int i=0; i<numberOfTriangles; i++)
      {
      const vtkIdType triangleCellId = triangleCellIdArray->GetValue(i);

      vtkIdList* faceCellPoints = vtkIdList::New();
      input->GetCellPoints(triangleCellId,faceCellPoints);

      vtkIdList* cellIds = vtkIdList::New();
      input->GetCellNeighbors(triangleCellId,faceCellPoints,cellIds);

      if (cellIds->GetNumberOfIds() != 1)
        {
        interiorBoundaryFacetsFound++;
        }
      else
        {
        exteriorBoundaryFacetsFound++;
        }

      // Get neighbor cell to facet
      vtkIdType cellId = cellIds->GetId(0);
      vtkCell* cell = input->GetCell(cellId);
      int cellType = cell->GetCellType();

      // Check that all neighbor cells are tets
      if (cellType != VTK_TETRA)
        {
        vtkErrorMacro(<<"Volume cell adjacent to triangle is not tetrahedron (volume cell id: "<<cellId <<") and it is unsupported by Dolfin. Skipping face.");
        faceCellPoints->Delete();
        cellIds->Delete();
        continue;
        }
      if (cellIds->GetNumberOfIds() == 2)
	{
        vtkIdType cellId2 = cellIds->GetId(0);
        vtkCell* cell2 = input->GetCell(cellId2);
        int cellType2 = cell2->GetCellType();
	if (cellType2 != VTK_TETRA)
	  {
          vtkErrorMacro(<<"Volume cell number two adjacent to triangle is not tetrahedron (volume cell id: "<<cellId2 <<") and it is unsupported by Dolfin. Not skipping face!");
          }
        }

      // Find match points to local faceId on vtk cell
      vtkIdType faceId = -1;
      for (int j=0; j<cell->GetNumberOfFaces(); j++)
        {
        vtkCell* face = cell->GetFace(j);
        vtkIdList* matchingPointIds = vtkIdList::New();
        matchingPointIds->DeepCopy(face->GetPointIds());
        matchingPointIds->IntersectWith(*faceCellPoints);
        const int numberOfNonMatching = face->GetNumberOfPoints() - matchingPointIds->GetNumberOfIds();
        matchingPointIds->Delete();

        if (numberOfNonMatching == 0)
          {
          faceId = j;
          break;
          }
        }

      // Find dolfinFaceId from vtk faceId
      vtkIdList* dolfinFaceOrder = vtkIdList::New();
      GetDolfinFaceOrder(cellType,dolfinFaceOrder);
      vtkIdType dolfinFaceId = dolfinFaceOrder->GetId(faceId);
      dolfinFaceOrder->Delete();

      // Store tetrahedron number for vtk triangle i
      boundaryFaceCells->SetId(i, volumeCellIdMap->GetId(cellId));
      // Store local dolfin facet number for vtk triangle i
      boundaryFaceIds->SetId(i, dolfinFaceId);

      // Cleanup
      faceCellPoints->Delete();
      cellIds->Delete();
      }

    bool old=false;
    if(old) 
      {  
      out << "      <data>" << endl;
      out << "        <data_entry name=\"boundary_facet_cells\">" << endl;
      out << "          <array type=\"uint\" size=\"" << numberOfTriangles << "\">" << endl;
      for (int i=0; i<numberOfTriangles; i++)
        {
        out << "            <element index=\"" << i << "\" "; 
        out << "value=\"" << boundaryFaceCells->GetId(i) << "\" "; 
        out << "/>" << endl;
        }
      out << "          </array>" << endl;
      out << "        </data_entry>" << endl;
      out << "        <data_entry name=\"boundary_facet_numbers\">" << endl;
      out << "          <array type=\"uint\" size=\"" << numberOfTriangles << "\">" << endl;
      for (int i=0; i<numberOfTriangles; i++)
        {
        out << "            <element index=\"" << i << "\" "; 
        out << "value=\"" << boundaryFaceIds->GetId(i) << "\" "; 
        out << "/>" << endl;
        }
      out << "          </array>" << endl;
      out << "        </data_entry>" << endl;
  
      out << "        <data_entry name=\"boundary_indicators\">" << endl;
      out << "          <array type=\"uint\" size=\"" << numberOfTriangles << "\">" << endl;
      for (int i=0; i<numberOfTriangles; i++)
        {
        const vtkIdType triangleCellId = triangleCellIdArray->GetValue(i);
        out << "            <element index=\"" << i << "\" "; 
        out << "value=\"" << boundaryDataArray->GetValue(triangleCellId) + this->BoundaryDataIdOffset << "\" "; 
        out << "/>" << endl;
        }
      out << "          </array>" << endl;
      out << "        </data_entry>" << endl;
  
      out << "      </data>" << endl;
      }
    else 
      {
      out << "      <domains>" << endl;

      out << "        <mesh_value_collection type=\"uint\" dim=\"2\" size=\""<< numberOfTriangles<< "\">" << endl;
      for (int i=0; i<numberOfTriangles; i++)
        {
        const vtkIdType triangleCellId = triangleCellIdArray->GetValue(i);
	const vtkIdType value = boundaryDataArray->GetValue(triangleCellId) + this->BoundaryDataIdOffset;
        out << "            <value cell_index=\"" << boundaryFaceCells->GetId(i) <<"\"" 
            << " local_entity=\"" << boundaryFaceIds->GetId(i) << "\" " 
            << " value=\""  << value << "\" " 
            << "/>" << endl;
        }
      out << "        </mesh_value_collection>" << endl;

      if (this->StoreCellMarkers)
        {
        out << "        <mesh_value_collection type=\"uint\" dim=\"3\" size=\""<< numberOfTetras << "\">" << endl;
        for (int i=0; i<numberOfTetras; i++)
          {
          const vtkIdType cellId = tetraCellIdArray->GetValue(i);
	  const vtkIdType value = boundaryDataArray->GetValue(cellId);
          out << "            <value cell_index=\"" << i << "\"" 
              << " local_entity=\"" << 0 << "\" " 
              << " value=\""  <<  value << "\" " 
              << "/>" << endl;
          }
        out << "        </mesh_value_collection>" << endl;
        }

      out << "      </domains>" << endl;
      }

    if (exteriorBoundaryFacetsFound)
      {
      vtkWarningMacro("Found boundary cells not on boundary!");
      }

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

void vtkvmtkDolfinWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkUnstructuredGridWriter::PrintSelf(os,indent);
}
