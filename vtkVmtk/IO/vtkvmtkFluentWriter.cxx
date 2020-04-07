/*=========================================================================
                                                                                                                                    
Program:   VMTK
Module:    $RCSfile: vtkvmtkFluentWriter.cxx,v $
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

#include "vtkvmtkFluentWriter.h"
#include "vtkUnstructuredGrid.h"
#include "vtkTetra.h"
#include "vtkCellType.h"
#include "vtkCell.h"
#include "vtkCellData.h"
#include "vtkIntArray.h"
#include "vtkIdTypeArray.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkvmtkConstants.h"


vtkStandardNewMacro(vtkvmtkFluentWriter);

vtkvmtkFluentWriter::vtkvmtkFluentWriter()
{
  this->BoundaryDataArrayName = NULL;
}

vtkvmtkFluentWriter::~vtkvmtkFluentWriter()
{
  if (this->BoundaryDataArrayName)
    {
    delete[] this->BoundaryDataArrayName;
    this->BoundaryDataArrayName = NULL;
    }
}

void vtkvmtkFluentWriter::ConvertFaceToLeftHanded(vtkUnstructuredGrid* input, vtkIdType tetraCellId, vtkIdType& id0, vtkIdType& id1, vtkIdType& id2)
{
  vtkTetra* tetra = vtkTetra::SafeDownCast(input->GetCell(tetraCellId));
  vtkIdList* cellPointIds = tetra->GetPointIds();
  vtkIdType id3 = -1;
  vtkIdType tmpId = -1;
  int k;
  for (k=0; k<4; k++)
    {
    tmpId = cellPointIds->GetId(k);
    if (tmpId != id0 && tmpId != id1 && tmpId != id2)
      {
      id3 = tmpId;
      break;
      }
    }

  bool reverse = false;
  double point0[3], point1[3], point2[3], point3[3];
  double vector0[3], vector1[3], vector2[3];
  double cross[3];
  input->GetPoint(id0,point0);
  input->GetPoint(id1,point1);
  input->GetPoint(id2,point2);
  input->GetPoint(id3,point3);
  vector0[0] = point1[0] - point0[0];
  vector0[1] = point1[1] - point0[1];
  vector0[2] = point1[2] - point0[2];
  vector1[0] = point2[0] - point0[0];
  vector1[1] = point2[1] - point0[1];
  vector1[2] = point2[2] - point0[2];
  vector2[0] = point3[0] - point0[0];
  vector2[1] = point3[1] - point0[1];
  vector2[2] = point3[2] - point0[2];
  vtkMath::Cross(vector0,vector1,cross);
  if (vtkMath::Dot(cross,vector2) < 0.0)
    {
    reverse = true;
    }
  if (reverse)
    {
    tmpId = id2;
    id2 = id1;
    id1 = tmpId;
    }
}

void vtkvmtkFluentWriter::WriteData()
{
  vtkUnstructuredGrid *input= vtkUnstructuredGrid::SafeDownCast(this->GetInput());

  int numberOfCells = input->GetNumberOfCells();
  for (int i=0; i<numberOfCells; i++)
    {
    int cellType = input->GetCellType(i);
    if (cellType != VTK_TRIANGLE && cellType != VTK_TETRA) 
      {
      vtkErrorMacro(<<"Only triangles and linear tetrahedra supported.");
      }
    }

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

  vtkIntArray* boundaryDataArray = vtkIntArray::New();
  if (this->BoundaryDataArrayName)
    {
    if (input->GetCellData()->GetArray(this->BoundaryDataArrayName))
      {
      boundaryDataArray->DeepCopy(input->GetCellData()->GetArray(this->BoundaryDataArrayName));
      }
    else
      {
      vtkErrorMacro(<<"BoundaryDataArray with name specified does not exist");
      boundaryDataArray->Delete();
      return;
      }
    }
  else
    {
    boundaryDataArray->SetNumberOfValues(numberOfCells);
    boundaryDataArray->FillComponent(0,0.0);
    }

  vtkIdTypeArray* tetraCellIdArray = vtkIdTypeArray::New();
  input->GetIdsOfCellsOfType(VTK_TETRA,tetraCellIdArray);
  int numberOfTetras = tetraCellIdArray->GetNumberOfTuples();

  vtkIdList* tetraCellIdMap = vtkIdList::New();
  tetraCellIdMap->SetNumberOfIds(numberOfCells);
  
  int i;

  for (i=0; i<numberOfCells; i++)
    {
    tetraCellIdMap->SetId(i,-1);
    }

  for (i=0; i<numberOfTetras; i++)
    {
    tetraCellIdMap->SetId(tetraCellIdArray->GetValue(i),i);
    }

  vtkIdTypeArray* triangleCellIdArray = vtkIdTypeArray::New();
  input->GetIdsOfCellsOfType(VTK_TRIANGLE,triangleCellIdArray);
  int numberOfTriangles = triangleCellIdArray->GetNumberOfTuples();

//  out << "(0 \"Fluent file generated by the Vascular Modeling Toolkit - www.vmtk.org\" )" << endl;
  out << "(0 \"GAMBIT to Fluent File\")" << endl;
  out << "(0 \"Dimension:\")" << endl;
  out << "(2 3)" << endl;
  out << endl;

  char str[200];

  sprintf(str,"(10 (0 1 %x 1 3))",numberOfPoints);
  out << str << endl;
  sprintf(str,"(10 (1 1 %x 1 3)(",numberOfPoints);
  out << str << endl;

  out.precision(6);

  double point[3];
  for (i=0; i<numberOfPoints; i++)
    {
    input->GetPoint(i,point);
    sprintf(str,"  %17.10e  %17.10e  %17.10e",point[0],point[1],point[2]);
    out << str << endl;
    }
  out << " ))" << endl << endl;

  out << "(0 \"Faces:\")" << endl;

  int numberOfInteriorFaces = 2*numberOfTetras - numberOfTriangles/2;

  sprintf(str,"(13 (0 1 %x 0))",numberOfInteriorFaces+numberOfTriangles);
  out << str << endl; 

  int faceOffset = 1;

  vtkIdList* boundaryDataNumberOfTriangles = vtkIdList::New();
  int boundaryDataRange = static_cast<int>(boundaryDataArray->GetRange()[1]);
  boundaryDataNumberOfTriangles->SetNumberOfIds(boundaryDataRange+1);
  for (i=0; i<boundaryDataRange+1; i++)
    {
    boundaryDataNumberOfTriangles->SetId(i,0);
    }
  int boundaryDataValue, value;
  for (i=0; i<numberOfTriangles; i++)
    {
    vtkIdType triangleCellId = triangleCellIdArray->GetValue(i);
    boundaryDataValue = boundaryDataArray->GetValue(triangleCellId);
    value = boundaryDataNumberOfTriangles->GetId(boundaryDataValue);
    boundaryDataNumberOfTriangles->SetId(boundaryDataValue,value+1);
    }

  vtkIdList* neighborCellIds = vtkIdList::New();
  const int entityOffset = 3;
  int entityId = entityOffset;
  int n;
  for (n=0; n<boundaryDataRange+1; n++)
    {
    int numberOfBoundaryTriangles = boundaryDataNumberOfTriangles->GetId(n);
    if (numberOfBoundaryTriangles == 0)
      {
      continue;
      }
    //sprintf(str,"(13 (%x %x %x %x 0)(",entityId,faceOffset,faceOffset+numberOfBoundaryTriangles-1,entityId);
    sprintf(str,"(13 (%x %x %x 3 0)(",entityId,faceOffset,faceOffset+numberOfBoundaryTriangles-1);
    entityId++;
    out << str << endl;
    for (i=0; i<numberOfTriangles; i++)
      {
      vtkIdType triangleCellId = triangleCellIdArray->GetValue(i);
      if (boundaryDataArray->GetValue(triangleCellId) != n)
        {
        continue;
        }
      vtkIdList* cellPointIds = input->GetCell(triangleCellId)->GetPointIds();
      vtkIdType id0 = cellPointIds->GetId(0);
      vtkIdType id1 = cellPointIds->GetId(1);
      vtkIdType id2 = cellPointIds->GetId(2);
      input->GetCellNeighbors(triangleCellId,cellPointIds,neighborCellIds);
      vtkIdType tetraCellId = neighborCellIds->GetId(0);
      this->ConvertFaceToLeftHanded(input,tetraCellId,id0,id1,id2);
      sprintf(str," 3 %x %x %x %x 0",(int)id0+1,(int)id1+1,(int)id2+1,(int)tetraCellId+1);
      out << str << endl;
      }
    out << "))" << endl << endl;
    faceOffset += numberOfBoundaryTriangles;
    }

  sprintf(str,"(13 (%x %x %x 2 0)(",(int)entityId,faceOffset,faceOffset+numberOfInteriorFaces-1);
  out << str << endl;

//TODO: loop over tets and write interior faces.
//one space, #points on the face, pid1, pid2, pid3, tetraid1, tetraid2
  for (i=0; i<numberOfTetras; i++)
    {
    vtkIdType tetraCellId = tetraCellIdArray->GetValue(i);
    vtkTetra* tetra = vtkTetra::SafeDownCast(input->GetCell(tetraCellId));
    int j;
    for (j=0; j<4; j++)
      {
      vtkCell* face = tetra->GetFace(j);
      vtkIdList* facePointIds = face->GetPointIds();
      vtkIdType id0 = facePointIds->GetId(0);
      vtkIdType id1 = facePointIds->GetId(1);
      vtkIdType id2 = facePointIds->GetId(2);
      input->GetCellNeighbors(tetraCellId,facePointIds,neighborCellIds);
      if (neighborCellIds->GetNumberOfIds() != 1)
        {
        continue;
        }
      if (input->GetCellType(neighborCellIds->GetId(0)) != VTK_TETRA)
        {
        continue;
        }
      if (neighborCellIds->GetId(0) < tetraCellId)
        {
        continue;
        }
      this->ConvertFaceToLeftHanded(input,tetraCellId,id0,id1,id2);
      sprintf(str," 3 %x %x %x %x %x",(int)id0+1,(int)id1+1,(int)id2+1,(int)tetraCellIdMap->GetId(tetraCellId)+1,(int)tetraCellIdMap->GetId(neighborCellIds->GetId(0))+1);
      out << str << endl;
      }
    }
  out << "))" << endl << endl;
  faceOffset += numberOfInteriorFaces;

  neighborCellIds->Delete();

  out << "(0 \"Cells:\")" << endl;
  sprintf(str,"(12 (0 1 %x 0))",numberOfTetras);
  out << str << endl;
  sprintf(str,"(12 (2 1 %x 1 2))",numberOfTetras);
  out << str << endl << endl;

  out << "(0 \"Zones:\")" << endl;
  out << "(45 (2 fluid blood)())" << endl;
  int numberOfBoundaryTriangles = 0;
  entityId = entityOffset;
  for (n=0; n<boundaryDataRange+1; n++)
    {
    numberOfBoundaryTriangles = boundaryDataNumberOfTriangles->GetId(n);
    if (numberOfBoundaryTriangles == 0)
      {
      continue;
      }
    sprintf(str,"(45 (%x wall surface%d)())",entityId,entityId);
    entityId++;
    out << str << endl;
    }
  sprintf(str,"(45 (%x interior default-interior)())",entityId);
  out << str << endl;

  boundaryDataNumberOfTriangles->Delete();
  triangleCellIdArray->Delete();
  tetraCellIdArray->Delete();
  tetraCellIdMap->Delete();
}

void vtkvmtkFluentWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkUnstructuredGridWriter::PrintSelf(os,indent);
}
