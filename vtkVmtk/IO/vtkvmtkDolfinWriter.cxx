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
#include <algorithm>

#include "vtkvmtkDolfinWriter.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellType.h"
#include "vtkCell.h"
#include "vtkCellData.h"
#include "vtkIdTypeArray.h"
#include "vtkObjectFactory.h"
#include "vtkvmtkConstants.h"


vtkStandardNewMacro(vtkvmtkDolfinWriter);

vtkvmtkDolfinWriter::vtkvmtkDolfinWriter()
{
  this->BoundaryDataArrayName = NULL; // TODO: Rename to CellEntityIdsArrayName
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
  // Open output file
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

  // Get and prepare input mesh
  vtkUnstructuredGrid *input = vtkUnstructuredGrid::SafeDownCast(this->GetInput());
  input->BuildLinks();
  const int numberOfPoints = input->GetNumberOfPoints();
  const int numberOfCells = input->GetNumberOfCells();

  // Create copy of cell entity ids array
  vtkIdTypeArray* cellEntityIds = NULL;
  if (this->BoundaryDataArrayName)
    {
    vtkDataArray * array = input->GetCellData()->GetArray(this->BoundaryDataArrayName);
    if (array)
      {
      cellEntityIds = vtkIdTypeArray::New();
      cellEntityIds->DeepCopy(array);
      }
    else
      {
      vtkErrorMacro(<<"Array with specified BoundaryDataArrayName does not exist");
      }
    }

  // Build an array with the vtk cell ids of all tetrahedras
  vtkIdTypeArray* tetraCellIdArray = vtkIdTypeArray::New();
  input->GetIdsOfCellsOfType(VTK_TETRA, tetraCellIdArray);
  const int numberOfTetras = tetraCellIdArray->GetNumberOfTuples();
  const int numberOfTetraPoints = 4; // Points in a tetrahedron(!)

  // Build the inverted array with mapping from contiguous tetrahedron numbering to vtk cell numbering
  vtkIdList* volumeCellIdMap = vtkIdList::New();
  volumeCellIdMap->SetNumberOfIds(numberOfCells);
  for (int i=0; i<numberOfTetras; i++)
    {
    vtkIdType tetraCellId = tetraCellIdArray->GetValue(i);
    volumeCellIdMap->SetId(tetraCellId,i);
    }

  // Write out dolfin mesh header
  out << "<?xml version=\"1.0\"?>" << endl;
  out << "<dolfin xmlns:dolfin=\"http://www.fenicsproject.org\">" << endl;
  out << "  <mesh celltype=\"tetrahedron\" dim=\"3\">" << endl;

  // Write out all vertices
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

  // Write out all cells
  out << "    <cells size=\"" << numberOfTetras << "\">" << endl;
  for (int i=0; i<numberOfTetras; i++)
    {
    vtkIdType tetraCellId = tetraCellIdArray->GetValue(i);
    vtkIdList* cellPointIds = input->GetCell(tetraCellId)->GetPointIds();

    // Sort point ids in cell, the way dolfin likes it (this works only for simplices!)
    vtkIdType dolfinCellPointIds[numberOfTetraPoints];
    for (int k=0; k<numberOfTetraPoints; k++)
      {
      dolfinCellPointIds[k] = cellPointIds->GetId(k);
      }
    std::sort(dolfinCellPointIds, dolfinCellPointIds+numberOfTetraPoints);

    // Write out vertex ids for a single tetrahedron
    out << "      <tetrahedron index=\"" << i << "\" "; 
    for (int k=0; k<numberOfTetraPoints; k++)
      {
      out << "v" << k << "=\"" << dolfinCellPointIds[k] << "\" ";
      }
    out << "/>" << endl;
    }
  out << "    </cells>" << endl;

  // Build and write subdomains if available
  if (cellEntityIds)
    {
    vtkIdTypeArray* triangleCellIdArray = vtkIdTypeArray::New();
    input->GetIdsOfCellsOfType(VTK_TRIANGLE,triangleCellIdArray);
    const int numberOfTriangles = triangleCellIdArray->GetNumberOfTuples();

    vtkIdList* triangleToTetrahedron = vtkIdList::New();
    triangleToTetrahedron->SetNumberOfIds(numberOfTriangles);

    vtkIdList* triangleToLocalFacetId = vtkIdList::New();
    triangleToLocalFacetId->SetNumberOfIds(numberOfTriangles);

    int interiorFacetsFound = 0;
    int exteriorFacetsFound = 0;
    for (int i=0; i<numberOfTriangles; i++)
      {
      const vtkIdType triangleCellId = triangleCellIdArray->GetValue(i);

      vtkIdList* faceCellPoints = vtkIdList::New();
      input->GetCellPoints(triangleCellId,faceCellPoints);

      vtkIdList* cellIds = vtkIdList::New();
      input->GetCellNeighbors(triangleCellId,faceCellPoints,cellIds);

      if (cellIds->GetNumberOfIds() != 1)
        {
        interiorFacetsFound++;
        }
      else
        {
        exteriorFacetsFound++;
        }

      // Get neighbor cell to facet, pick the one with smallest index if two (interior facet)
      vtkIdType cellId = cellIds->GetId(0);
      if (cellIds->GetNumberOfIds() == 2  &&  cellIds->GetId(1) < cellId)
        {
        cellId = cellIds->GetId(1);
        }
      vtkCell* cell = input->GetCell(cellId);
      int cellType = cell->GetCellType();
      vtkIdList* cellPointIds = cell->GetPointIds();

      // Check that all neighbor cells are tets
      if (cellType != VTK_TETRA)
        {
        vtkErrorMacro(<<"Volume cell adjacent to triangle is not tetrahedron (volume cell id: "<<cellId <<") and it is unsupported by Dolfin. Skipping face.");
        faceCellPoints->Delete();
        cellIds->Delete();
        continue;
        }

      // Sort point ids in cell, the way dolfin likes it (this works only for simplices!)
      vtkIdType dolfinCellPointIds[numberOfTetraPoints];
      for (int k=0; k<numberOfTetraPoints; k++)
        {
        dolfinCellPointIds[k] = cellPointIds->GetId(k);
        }
      std::sort(dolfinCellPointIds, dolfinCellPointIds+numberOfTetraPoints);

      // Find local facet id in dolfin numbering, opposite of point in cell that is not part of facet
      vtkIdType dolfinFaceId = -1;
      for (int k=0; k<numberOfTetraPoints; k++)
        {
        bool found = false;
        const int numberOfTrianglePoints = 3;
        for (int j=0; j<numberOfTrianglePoints; j++)
          {
            if (dolfinCellPointIds[k] == faceCellPoints->GetId(j))
              {
              found = true;
              break;
              }
          }
        if (!found)
          {
          dolfinFaceId = k;
          break;
          }
        }

      // Store tetrahedron number for vtk triangle i
      triangleToTetrahedron->SetId(i, volumeCellIdMap->GetId(cellId));
      // Store local dolfin facet number for vtk triangle i
      triangleToLocalFacetId->SetId(i, dolfinFaceId);

      // Cleanup
      faceCellPoints->Delete();
      cellIds->Delete();
      }

    // Start subdomains section in file
    if (numberOfTriangles || this->StoreCellMarkers)
      {
      out << "    <domains>" << endl;
      }

    // Write facet subdomains
    if (numberOfTriangles)
      {
      out << "      <mesh_value_collection type=\"uint\" dim=\"2\" size=\""<< numberOfTriangles<< "\">" << endl;
      for (int i=0; i<numberOfTriangles; i++)
        {
        const vtkIdType triangleCellId = triangleCellIdArray->GetValue(i);
        const vtkIdType tetrahedronCellId = triangleToTetrahedron->GetId(i);
        const vtkIdType value = cellEntityIds->GetValue(triangleCellId) + this->BoundaryDataIdOffset;
        const vtkIdType localEntity = triangleToLocalFacetId->GetId(i);

        out << "        <value cell_index=\"" << tetrahedronCellId <<"\"" 
            << " local_entity=\"" << localEntity << "\""
            << " value=\""  << value << "\""
            << " />" << endl;
        }
      out << "      </mesh_value_collection>" << endl;
      }

    // Write cell subdomains
    if (this->StoreCellMarkers)
      {
      out << "      <mesh_value_collection type=\"uint\" dim=\"3\" size=\""<< numberOfTetras << "\">" << endl;
      for (int i=0; i<numberOfTetras; i++)
        {
        const vtkIdType cellId = tetraCellIdArray->GetValue(i);
        const vtkIdType value = cellEntityIds->GetValue(cellId);
        out << "        <value cell_index=\"" << i << "\"" 
            << " local_entity=\"" << 0 << "\"" 
            << " value=\""  <<  value << "\"" 
            << " />" << endl;
        }
      out << "      </mesh_value_collection>" << endl;
      }

    // End subdomains section in file
    if (numberOfTriangles || this->StoreCellMarkers)
      {
      out << "    </domains>" << endl;
      }

    if (interiorFacetsFound)
      {
      vtkWarningMacro("Found boundary cells not on boundary!");
      }

    triangleCellIdArray->Delete();
    triangleToTetrahedron->Delete();
    triangleToLocalFacetId->Delete();
  }

  out << "  </mesh>" << endl;
  out << "</dolfin>" << endl;

  tetraCellIdArray->Delete();
  volumeCellIdMap->Delete();

  if (cellEntityIds)
    {
    cellEntityIds->Delete();
    }
}

void vtkvmtkDolfinWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkUnstructuredGridWriter::PrintSelf(os,indent);
}
