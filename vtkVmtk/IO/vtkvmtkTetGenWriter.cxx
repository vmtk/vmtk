/*=========================================================================
                                                                                                                                    
Program:   VMTK
Module:    vtkvmtkTetGenWriter.cxx
Language:  C++
Date:      Sat Feb 19 22:47:34 CET 2011
Version:   Revision: 1.0
                                                                                                                                    
  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
                                                                                                                                    
=========================================================================*/

#include "vtkvmtkTetGenWriter.h"
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


vtkStandardNewMacro(vtkvmtkTetGenWriter);

vtkvmtkTetGenWriter::vtkvmtkTetGenWriter()
{
  this->BoundaryDataArrayName = NULL;
}

vtkvmtkTetGenWriter::~vtkvmtkTetGenWriter()
{
  if (this->BoundaryDataArrayName)
    {
    delete[] this->BoundaryDataArrayName;
    this->BoundaryDataArrayName = NULL;
    }
}

void vtkvmtkTetGenWriter::WriteData()
{
  vtkUnstructuredGrid *input= vtkUnstructuredGrid::SafeDownCast(this->GetInput());

  if (!this->GetFileName())
    {
    vtkErrorMacro(<<"FileName not set.");
    return;
    }

  std::string nodeFileName = this->GetFileName();
  nodeFileName += ".node";

  std::string eleFileName = this->GetFileName();
  eleFileName += ".ele";

  ofstream nodeStream(nodeFileName.c_str());
  ofstream eleStream(eleFileName.c_str());
 
  if (!nodeStream.good())
    {
    vtkErrorMacro(<<"Could not open node file for writing.");
    return;
    }
 
  if (!eleStream.good())
    {
    vtkErrorMacro(<<"Could not open ele file for writing.");
    return;
    }

  input->BuildLinks();

  int numberOfPoints = input->GetNumberOfPoints();

  int i, j;

  //TODO: add attributes and boundary markers

  nodeStream << numberOfPoints << " 3 0 0" << std::endl;

  double point[3];
  for (i=0; i<numberOfPoints; i++)
    {
    input->GetPoint(i,point);
    nodeStream << i+1 << " " << point[0] << " " << point[1] << " " << point[2] << std::endl;
    }

#if 0
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
#endif

  vtkIdTypeArray* tetraCellIdArray = vtkIdTypeArray::New();
  input->GetIdsOfCellsOfType(VTK_TETRA,tetraCellIdArray);
  int numberOfTetras = tetraCellIdArray->GetNumberOfTuples();

  vtkIdTypeArray* quadraticTetraCellIdArray = vtkIdTypeArray::New();
  input->GetIdsOfCellsOfType(VTK_QUADRATIC_TETRA,quadraticTetraCellIdArray);
  int numberOfQuadraticTetras = quadraticTetraCellIdArray->GetNumberOfTuples();

  int pointsInTet = 4;
  vtkIdTypeArray* tetIdsArray = tetraCellIdArray;
  int numberOfOutputTetras = tetraCellIdArray->GetNumberOfTuples();

  if (numberOfQuadraticTetras > numberOfTetras)
    {
    pointsInTet = 10;
    tetIdsArray = quadraticTetraCellIdArray;
    numberOfOutputTetras = quadraticTetraCellIdArray->GetNumberOfTuples();
    }

  //TODO: add attributes

  eleStream << numberOfOutputTetras << " " << pointsInTet << " 0" << std::endl;

  double point0[3], point1[3], point2[3], point3[3];
  double cross[3], vector01[3], vector21[3], vector31[3];
  double dot;
  int tmp;

  int cellPointIds[10];
  for (i=0; i<numberOfOutputTetras; i++)
    {
    vtkIdType cellId = tetraCellIdArray->GetValue(i);
    vtkCell* cell = input->GetCell(cellId);
    for (j=0; j<pointsInTet; j++)
      {
      cellPointIds[j] = cell->GetPointId(j);
      }
    input->GetPoint(cellPointIds[0],point0);
    input->GetPoint(cellPointIds[1],point1);
    input->GetPoint(cellPointIds[2],point2);
    input->GetPoint(cellPointIds[3],point3);
    vector01[0] = point0[0] - point1[0];
    vector01[1] = point0[1] - point1[1];
    vector01[2] = point0[2] - point1[2];
    vector21[0] = point2[0] - point1[0];
    vector21[1] = point2[1] - point1[1];
    vector21[2] = point2[2] - point1[2];
    vector31[0] = point3[0] - point1[0];
    vector31[1] = point3[1] - point1[1];
    vector31[2] = point3[2] - point1[2];
    vtkMath::Cross(vector21,vector31,cross);
    dot = vtkMath::Dot(cross,vector01);
    if (dot < 0.0)
      {
      tmp = cellPointIds[2];
      cellPointIds[2] = cellPointIds[3];
      cellPointIds[3] = tmp;
      if (pointsInTet == 10)
        {
        tmp = cellPointIds[6];
        cellPointIds[6] = cellPointIds[7];
        cellPointIds[7] = tmp;
        tmp = cellPointIds[5];
        cellPointIds[5] = cellPointIds[8];
        cellPointIds[8] = tmp;
        }
      }
 
    eleStream << i+1 << " ";
    for (j=0; j<pointsInTet; j++)
      {
      eleStream << cellPointIds[j]+1 << " ";
      }
    eleStream << std::endl;
    }

  tetraCellIdArray->Delete();
  quadraticTetraCellIdArray->Delete();
}

void vtkvmtkTetGenWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkUnstructuredGridWriter::PrintSelf(os,indent);
}
