/*=========================================================================
                                                                                                                                    
Program:   VMTK
Module:    $RCSfile: vtkvmtkXdaWriter.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:47 $
Version:   $Revision: 1.6 $
                                                                                                                                    
  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
                                                                                                                                    
=========================================================================*/

// #include <fstream>

#include "vtkvmtkXdaWriter.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellType.h"
#include "vtkCell.h"
#include "vtkCellData.h"
#include "vtkIdTypeArray.h"
#include "vtkObjectFactory.h"
#include "vtkvmtkConstants.h"


vtkStandardNewMacro(vtkvmtkXdaWriter);

vtkvmtkXdaWriter::vtkvmtkXdaWriter()
{
  this->BoundaryDataArrayName = NULL;
}

vtkvmtkXdaWriter::~vtkvmtkXdaWriter()
{
  if (this->BoundaryDataArrayName)
    {
    delete[] this->BoundaryDataArrayName;
    this->BoundaryDataArrayName = NULL;
    }
}

void vtkvmtkXdaWriter::WriteData()
{
  vtkUnstructuredGrid *input= vtkUnstructuredGrid::SafeDownCast(this->GetInput());

  if (!this->GetFileName())
    {
    vtkErrorMacro(<<"FileName not set.");
    return;
    }
        
  ofstream out (this->GetFileName());

  if (!out.good())
    {
    vtkErrorMacro(<<"Could not open file for writing.");
    return;
    }
  
  input->BuildLinks();

  int numberOfPoints = input->GetNumberOfPoints();
  int numberOfCells = input->GetNumberOfCells();

  vtkDataArray* boundaryDataArray = NULL;
  if (this->BoundaryDataArrayName)
    {
    boundaryDataArray = input->GetCellData()->GetArray(this->BoundaryDataArrayName);
    if (!boundaryDataArray)
      {
      vtkErrorMacro(<<"BoundaryDataArray with name specified does not exist");
      }
    }

  int totalWeight = 0;

  enum
    {
      tetraType = 0,
      hexahedronType,
      wedgeType,
      pyramidType,
      quadraticTetraType,
      quadraticHexahedronType,
      quadraticWedgeType,
      biquadraticWedgeType,
      quadraticPyramidType
    };

  const int numberOfVolumeCellTypes = 9;

  enum
    {
      tet4LibmeshType = 8,
      tet10LibmeshType,
      hex8LibmeshType,
      hex20LibmeshType,
      hex27LibmeshType,
      prism6LibmeshType,
      prism15LibmeshType,
      prism18LibmeshType,
      pyramid5LibmeshType
    };

  int elementTypeLibmeshMap[numberOfVolumeCellTypes];
  elementTypeLibmeshMap[tetraType] = tet4LibmeshType;
  elementTypeLibmeshMap[hexahedronType] = hex8LibmeshType;
  elementTypeLibmeshMap[wedgeType] = prism6LibmeshType;
  elementTypeLibmeshMap[pyramidType] = pyramid5LibmeshType;
  elementTypeLibmeshMap[quadraticTetraType] = tet10LibmeshType;
  elementTypeLibmeshMap[quadraticHexahedronType] = hex20LibmeshType;
  elementTypeLibmeshMap[quadraticWedgeType] = prism15LibmeshType;
  elementTypeLibmeshMap[biquadraticWedgeType] = prism18LibmeshType;
  elementTypeLibmeshMap[quadraticPyramidType] = -1;

  int numberOfElementsInBlock[numberOfVolumeCellTypes];
  int i;
  for (i=0; i<numberOfVolumeCellTypes; i++)
    {
    numberOfElementsInBlock[i] = 0;
    }

  int blockVtkCellTypes[numberOfVolumeCellTypes];
  blockVtkCellTypes[tetraType] = VTK_TETRA;
  blockVtkCellTypes[hexahedronType] = VTK_HEXAHEDRON;
  blockVtkCellTypes[wedgeType] = VTK_WEDGE;
  blockVtkCellTypes[pyramidType] = VTK_PYRAMID;
  blockVtkCellTypes[quadraticTetraType] = VTK_QUADRATIC_TETRA;
  blockVtkCellTypes[quadraticHexahedronType] = VTK_QUADRATIC_HEXAHEDRON;
  blockVtkCellTypes[quadraticWedgeType] = VTK_QUADRATIC_WEDGE;
#if VTK_MAJOR_VERSION > 5 || (VTK_MAJOR_VERSION == 5 && VTK_MINOR_VERSION > 0)
  blockVtkCellTypes[biquadraticWedgeType] = VTK_BIQUADRATIC_QUADRATIC_WEDGE;
#else
  blockVtkCellTypes[biquadraticWedgeType] = VTK_EMPTY_CELL;
#endif
  blockVtkCellTypes[quadraticPyramidType] = VTK_QUADRATIC_PYRAMID;

  int numberOfVolumeCells = 0;

  for (i=0; i<numberOfCells; i++)
    {
    int cellType = input->GetCellType(i);
    switch (cellType)
      {
      case VTK_TETRA:
        ++numberOfElementsInBlock[tetraType];
        break;
      case VTK_HEXAHEDRON:
        ++numberOfElementsInBlock[hexahedronType];
        break;
      case VTK_WEDGE:
        ++numberOfElementsInBlock[wedgeType];
        break;
      case VTK_PYRAMID:
        ++numberOfElementsInBlock[pyramidType];
        break;
      case VTK_QUADRATIC_TETRA:
        ++numberOfElementsInBlock[quadraticTetraType];
        break;
      case VTK_QUADRATIC_HEXAHEDRON:
        ++numberOfElementsInBlock[quadraticHexahedronType];
        break;
      case VTK_QUADRATIC_WEDGE:
        ++numberOfElementsInBlock[quadraticWedgeType];
        break;
#if VTK_MAJOR_VERSION > 5 || (VTK_MAJOR_VERSION == 5 && VTK_MINOR_VERSION > 0)
      case VTK_BIQUADRATIC_QUADRATIC_WEDGE:
        ++numberOfElementsInBlock[biquadraticWedgeType];
        break;
#else
        vtkWarningMacro(<<"Biquadratic quadratic wedge not supported by the installed VTK version. Skipping element.");
        continue;
#endif
      case VTK_QUADRATIC_PYRAMID:
        vtkWarningMacro(<<"Quadratic pyramid not currently supported in libmesh. Skipping element.");
        continue;
      default:
        continue;
      }
    
    ++numberOfVolumeCells;  
    totalWeight += input->GetCell(i)->GetNumberOfPoints();
    }

  int numberOfElementBlocks = 0;

  for (i=0; i<numberOfVolumeCellTypes; i++)
    {
    if (numberOfElementsInBlock[i] > 0)
      {
      ++numberOfElementBlocks;
      }
    }  

  int numberOfBoundaryConditions = 0;
  if (boundaryDataArray)
    {
    for (i=0; i<numberOfCells; i++)
      {
      int cellType = input->GetCellType(i);
      switch (cellType)
        {
        case VTK_TRIANGLE:
        case VTK_QUAD:
        case VTK_QUADRATIC_TRIANGLE:
        case VTK_QUADRATIC_QUAD:
          numberOfBoundaryConditions++;
          break;
        default:
          continue;
        }
      }
    }

  out << "DEAL 003:003" << endl;
//  out << "LIBM 0" << endl;
  out << numberOfVolumeCells << "\t# Num. Elements" << endl;
  out << numberOfPoints << "\t# Num. Nodes" << endl;
  out << totalWeight << "\t# Sum of Element Weights" << endl;
  
  out << numberOfBoundaryConditions << "\t# Num. Boundary Conds." << endl;

  int stringSize = 65536;
  out << stringSize << "\t# String Size (ignore)" << endl;

//  out << numberOfElementBlocks << "\t# Num. Element Blocks." << endl;
  out << numberOfElementBlocks << "\t# Num. Element Types." << endl;

  for (i=0; i<numberOfVolumeCellTypes; i++)
    {
    if (numberOfElementsInBlock[i] > 0)
      {
      out << elementTypeLibmeshMap[i] << " ";
      }
    }
  out << "\t# Element types in each block." << endl;

  for (i=0; i<numberOfVolumeCellTypes; i++)
    {
    if (numberOfElementsInBlock[i] > 0)
      {
      out << numberOfElementsInBlock[i] << " ";
      }
    }
  out << "\t# Num. of elements in each block at each refinement level." << endl;

  out << "Id String" << endl;
  out << "Title String" << endl;

  vtkIdList* volumeCellIdMap = vtkIdList::New();
  volumeCellIdMap->SetNumberOfIds(numberOfCells);
  int volumeCellCounter = 0;
  for (i=0; i<numberOfVolumeCellTypes; i++)
    {
    if (numberOfElementsInBlock[i] == 0)
      {
      continue;
      }

    vtkIdTypeArray* typeCellIds = vtkIdTypeArray::New();
    input->GetIdsOfCellsOfType(blockVtkCellTypes[i], typeCellIds);

    vtkIdList* libmeshConnectivity = vtkIdList::New();
    this->GetLibmeshConnectivity(blockVtkCellTypes[i],libmeshConnectivity);

    int numberOfTypeCells = typeCellIds->GetNumberOfTuples();

    int j;
    for (j=0; j<numberOfTypeCells; j++)
      {
      vtkIdType id = typeCellIds->GetValue(j);
      vtkIdList* cellPoints = vtkIdList::New();
      input->GetCellPoints(id,cellPoints);
      //TODO: get individual connectivity, reversed if needed by positive Jacobian in libMesh
      int numberOfCellPoints = libmeshConnectivity->GetNumberOfIds();
      for (int k=0; k<numberOfCellPoints; k++)
        {
        out << cellPoints->GetId(libmeshConnectivity->GetId(k)) << " ";
        }
//      out << j << " " << "-1"; // TODO: change -1 into parent for AMR datasets
      out << endl;

      volumeCellIdMap->SetId(id,volumeCellCounter);
      volumeCellCounter++;
      cellPoints->Delete();
      }
    
    typeCellIds->Delete();
    libmeshConnectivity->Delete();
    }

  double point[3];
  for (i=0; i<numberOfPoints; i++)
    {
    input->GetPoint(i,point);
    out << point[0] << " " << point[1] << " " << point[2] << endl;
    }

  if (boundaryDataArray)
    {
    for (i=0; i<numberOfCells; i++)
      {
      int faceCellType = input->GetCellType(i);
      switch (faceCellType)
        {
        case VTK_TRIANGLE:
        case VTK_QUAD:
        case VTK_QUADRATIC_TRIANGLE:
        case VTK_QUADRATIC_QUAD:
          break;
        default:
          continue;
        }

      vtkIdList* faceCellPoints = vtkIdList::New();
      input->GetCellPoints(i,faceCellPoints);

      vtkIdList* cellIds = vtkIdList::New();
      input->GetCellNeighbors(i,faceCellPoints,cellIds);

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

      vtkIdList* libmeshFaceOrder = vtkIdList::New();

      this->GetLibmeshFaceOrder(cellType,libmeshFaceOrder);
      vtkIdType libmeshFaceId = libmeshFaceOrder->GetId(faceId);

      libmeshFaceOrder->Delete();

      short int boundaryValue = static_cast<short int>(boundaryDataArray->GetComponent(i,0));
      out << volumeCellIdMap->GetId(cellId) << " " << libmeshFaceId << " " << boundaryValue << endl;

      faceCellPoints->Delete();
      }
    }

  volumeCellIdMap->Delete();
}

void vtkvmtkXdaWriter::GetLibmeshConnectivity(int cellType, vtkIdList* libmeshConnectivity)
{
  libmeshConnectivity->Initialize();

  switch(cellType)
    {
    case VTK_TETRA:
      libmeshConnectivity->SetNumberOfIds(4);
      libmeshConnectivity->SetId(0,0);
      libmeshConnectivity->SetId(1,1);
      libmeshConnectivity->SetId(2,2);
      libmeshConnectivity->SetId(3,3);
      break;
    case VTK_HEXAHEDRON:
      libmeshConnectivity->SetNumberOfIds(8);
      libmeshConnectivity->SetId(0,0);
      libmeshConnectivity->SetId(1,1);
      libmeshConnectivity->SetId(2,2);
      libmeshConnectivity->SetId(3,3);
      libmeshConnectivity->SetId(4,4);
      libmeshConnectivity->SetId(5,5);
      libmeshConnectivity->SetId(6,6);
      libmeshConnectivity->SetId(7,7);
      break;
    case VTK_WEDGE:
      libmeshConnectivity->SetNumberOfIds(6);
      libmeshConnectivity->SetId(0,0);
      libmeshConnectivity->SetId(1,2);
      libmeshConnectivity->SetId(2,1);
      libmeshConnectivity->SetId(3,3);
      libmeshConnectivity->SetId(4,5);
      libmeshConnectivity->SetId(5,4);
      break;
    case VTK_PYRAMID:
      libmeshConnectivity->SetNumberOfIds(5);
      libmeshConnectivity->SetId(0,0);
      libmeshConnectivity->SetId(1,1);
      libmeshConnectivity->SetId(2,2);
      libmeshConnectivity->SetId(3,3);
      libmeshConnectivity->SetId(4,4);
      break;
    case VTK_QUADRATIC_TETRA:
      libmeshConnectivity->SetNumberOfIds(10);
      libmeshConnectivity->SetId(0,0);
      libmeshConnectivity->SetId(1,1);
      libmeshConnectivity->SetId(2,2);
      libmeshConnectivity->SetId(3,3);
      libmeshConnectivity->SetId(4,4);
      libmeshConnectivity->SetId(5,5);
      libmeshConnectivity->SetId(6,6);
      libmeshConnectivity->SetId(7,7);
      libmeshConnectivity->SetId(8,8);
      libmeshConnectivity->SetId(9,9);
      break;
    case VTK_QUADRATIC_HEXAHEDRON:
      libmeshConnectivity->SetNumberOfIds(20);
      libmeshConnectivity->SetId(0,0);
      libmeshConnectivity->SetId(1,1);
      libmeshConnectivity->SetId(2,2);
      libmeshConnectivity->SetId(3,3);
      libmeshConnectivity->SetId(4,4);
      libmeshConnectivity->SetId(5,5);
      libmeshConnectivity->SetId(6,6);
      libmeshConnectivity->SetId(7,7);
      libmeshConnectivity->SetId(8,8);
      libmeshConnectivity->SetId(9,9);
      libmeshConnectivity->SetId(10,10);
      libmeshConnectivity->SetId(11,11);
      libmeshConnectivity->SetId(12,16);
      libmeshConnectivity->SetId(13,17);
      libmeshConnectivity->SetId(14,18);
      libmeshConnectivity->SetId(15,19);
      libmeshConnectivity->SetId(16,12);
      libmeshConnectivity->SetId(17,13);
      libmeshConnectivity->SetId(18,14);
      libmeshConnectivity->SetId(19,15);
      break;
    case VTK_QUADRATIC_WEDGE:
      libmeshConnectivity->SetNumberOfIds(15);
      libmeshConnectivity->SetId(0,0);
      libmeshConnectivity->SetId(1,2);
      libmeshConnectivity->SetId(2,1);
      libmeshConnectivity->SetId(3,3);
      libmeshConnectivity->SetId(4,5);
      libmeshConnectivity->SetId(5,4);
      libmeshConnectivity->SetId(6,8);
      libmeshConnectivity->SetId(7,7);
      libmeshConnectivity->SetId(8,6);
      libmeshConnectivity->SetId(9,12);
      libmeshConnectivity->SetId(10,14);
      libmeshConnectivity->SetId(11,13);
      libmeshConnectivity->SetId(12,11);
      libmeshConnectivity->SetId(13,10);
      libmeshConnectivity->SetId(14,9);
      break;
#if VTK_MAJOR_VERSION > 5 || (VTK_MAJOR_VERSION == 5 && VTK_MINOR_VERSION > 0)
    case VTK_BIQUADRATIC_QUADRATIC_WEDGE:
      libmeshConnectivity->SetNumberOfIds(18);
      libmeshConnectivity->SetId(0,0);
      libmeshConnectivity->SetId(1,2);
      libmeshConnectivity->SetId(2,1);
      libmeshConnectivity->SetId(3,3);
      libmeshConnectivity->SetId(4,5);
      libmeshConnectivity->SetId(5,4);
      libmeshConnectivity->SetId(6,8);
      libmeshConnectivity->SetId(7,7);
      libmeshConnectivity->SetId(8,6);
      libmeshConnectivity->SetId(9,12);
      libmeshConnectivity->SetId(10,14);
      libmeshConnectivity->SetId(11,13);
      libmeshConnectivity->SetId(12,11);
      libmeshConnectivity->SetId(13,10);
      libmeshConnectivity->SetId(14,9);
      libmeshConnectivity->SetId(15,17);
      libmeshConnectivity->SetId(16,16);
      libmeshConnectivity->SetId(17,15);
      break;
#endif
    default:
      cerr<<"Element type not currently supported in libmesh. Skipping element."<<endl;
      break;
    }
}

void vtkvmtkXdaWriter::GetLibmeshFaceOrder(int cellType, vtkIdList* libmeshFaceOrder)
{
  libmeshFaceOrder->Initialize();

  switch(cellType)
    {
    case VTK_TETRA:
      libmeshFaceOrder->SetNumberOfIds(4);
      libmeshFaceOrder->SetId(0,1);
      libmeshFaceOrder->SetId(1,2);
      libmeshFaceOrder->SetId(2,3);
      libmeshFaceOrder->SetId(3,0);
      break;
    case VTK_HEXAHEDRON:
      libmeshFaceOrder->SetNumberOfIds(6);
      libmeshFaceOrder->SetId(0,4);
      libmeshFaceOrder->SetId(1,2);
      libmeshFaceOrder->SetId(2,1);
      libmeshFaceOrder->SetId(3,3);
      libmeshFaceOrder->SetId(4,0);
      libmeshFaceOrder->SetId(5,5);
      break;
    case VTK_WEDGE:
      libmeshFaceOrder->SetNumberOfIds(5);
      libmeshFaceOrder->SetId(0,0);
      libmeshFaceOrder->SetId(1,4);
      libmeshFaceOrder->SetId(2,3);
      libmeshFaceOrder->SetId(3,2);
      libmeshFaceOrder->SetId(4,1);
      break;
    case VTK_PYRAMID:
      libmeshFaceOrder->SetNumberOfIds(5);
      libmeshFaceOrder->SetId(0,4);
      libmeshFaceOrder->SetId(1,0);
      libmeshFaceOrder->SetId(2,1);
      libmeshFaceOrder->SetId(3,2);
      libmeshFaceOrder->SetId(4,3);
      break;
    case VTK_QUADRATIC_TETRA:
      libmeshFaceOrder->SetNumberOfIds(4);
      libmeshFaceOrder->SetId(0,1);
      libmeshFaceOrder->SetId(1,2);
      libmeshFaceOrder->SetId(2,3);
      libmeshFaceOrder->SetId(3,0);
      break;
    case VTK_QUADRATIC_HEXAHEDRON:
      libmeshFaceOrder->SetNumberOfIds(6);
      libmeshFaceOrder->SetId(0,4);
      libmeshFaceOrder->SetId(1,2);
      libmeshFaceOrder->SetId(2,1);
      libmeshFaceOrder->SetId(3,3);
      libmeshFaceOrder->SetId(4,0);
      libmeshFaceOrder->SetId(5,5);
      break;
    case VTK_QUADRATIC_WEDGE:
#if VTK_MAJOR_VERSION > 5 || (VTK_MAJOR_VERSION == 5 && VTK_MINOR_VERSION > 0)
    case VTK_BIQUADRATIC_QUADRATIC_WEDGE:
#endif
      libmeshFaceOrder->SetNumberOfIds(5);
      libmeshFaceOrder->SetId(0,0);
      libmeshFaceOrder->SetId(1,4);
      libmeshFaceOrder->SetId(2,3);
      libmeshFaceOrder->SetId(3,2);
      libmeshFaceOrder->SetId(4,1);
      break;
    default:
      cerr<<"Element type not currently supported in libmesh. Skipping element."<<endl;
      break;
    }
}

void vtkvmtkXdaWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkUnstructuredGridWriter::PrintSelf(os,indent);
}
