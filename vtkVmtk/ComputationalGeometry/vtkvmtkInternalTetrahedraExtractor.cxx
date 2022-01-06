/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkInternalTetrahedraExtractor.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkInternalTetrahedraExtractor.h"
#include "vtkPointData.h"
#include "vtkCellArray.h"
#include "vtkUnstructuredGrid.h"
#include "vtkIntArray.h"
#include "vtkTetra.h"
#include "vtkTriangle.h"
#include "vtkMath.h"
#include "vtkvmtkConstants.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkInternalTetrahedraExtractor);

vtkvmtkInternalTetrahedraExtractor::vtkvmtkInternalTetrahedraExtractor()
{
  this->UseCaps = 0;
  this->CapCenterIds = NULL;
  this->OutwardNormalsArrayName = NULL;
  this->Tolerance = VTK_VMTK_DOUBLE_TOL;
  this->RemoveSubresolutionTetrahedra = 0;
  this->SubresolutionFactor = 1.0;
  this->Surface = NULL;
}

vtkvmtkInternalTetrahedraExtractor::~vtkvmtkInternalTetrahedraExtractor()
{
  if (this->CapCenterIds)
    {
    this->CapCenterIds->Delete();
    this->CapCenterIds = NULL;
    }
  if (this->OutwardNormalsArrayName)
    {
    delete [] this->OutwardNormalsArrayName;
    this->OutwardNormalsArrayName = NULL;
    }
  if (this->Surface)
    {
    this->Surface->Delete();
    this->Surface = NULL;
    }
}

int vtkvmtkInternalTetrahedraExtractor::RequestData(
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkUnstructuredGrid *input = vtkUnstructuredGrid::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // Declare
  double circumcenter[3];
  double p0[3], p1[3], p2[3], p3[3];
  double v0[3], v1[3], v2[3], v3[3], n0[3], n1[3], n2[3], n3[3];
  double dot0, dot1, dot2, dot3;
  bool boundaryTetra;
  bool allDotPositive, allDotMinusOnePositive;
  vtkIdType i, j;
  vtkCellArray* newTetras;
  vtkIdList* newCellTypes;
  vtkIntArray* keepCell;
  vtkDataArray* outwardPointNormals;
  vtkTetra* tetra;

  if (!this->OutwardNormalsArrayName)
    {
    vtkErrorMacro(<< "No normals array name specified!");
    return 1;
    }

  outwardPointNormals = input->GetPointData()->GetArray(this->OutwardNormalsArrayName);

  if (!outwardPointNormals)
    {
    vtkErrorMacro(<< "Array with name specified does not exist!");
    return 1;
    }

  if (outwardPointNormals->GetNumberOfComponents()!=3)
    {
    vtkErrorMacro(<< "Normals have NumberOfComponents != 3!");
    return 1;
    }

  if (outwardPointNormals->GetNumberOfTuples()!=input->GetNumberOfPoints())
    {
    vtkErrorMacro(<< "Number of normals does not match input number of points !");
    return 1;
    }

  if ((this->UseCaps)&&(!this->CapCenterIds))
    {
    vtkErrorMacro(<< "UseCapsOn but no CapCenterIds specified !");
    return 1;
    }

  if (this->RemoveSubresolutionTetrahedra && !this->Surface)
    {
    vtkErrorMacro(<< "RemoveSubresolutionTetrahedraOn but no Surface specified !");
    return 1;
    }

  if (this->RemoveSubresolutionTetrahedra && this->Surface->GetNumberOfPoints() != input->GetNumberOfPoints())
    {
    vtkErrorMacro(<< "NumberOfPoints of Surface is different than that of input Delaunay tessellation, the Delaunay tessellation doesn't look like it has been produced from the Surface !");
    return 1;
    }

  // Allocate
  newTetras = vtkCellArray::New();
  newCellTypes = vtkIdList::New();
  keepCell = vtkIntArray::New();
  keepCell->SetNumberOfTuples(input->GetNumberOfCells());

  // Execute 

  //skeleton: dual of inner delaunay tets (Attali, Sk0)(not necessarily internal) or inner voronoi elements (Sk2)(not necessarily homotpic).
  //actual choice: Sk2. 

  double tolerance = this->Tolerance;

  for (i=0; i<input->GetNumberOfCells(); i++)
    {
    tetra = vtkTetra::SafeDownCast(input->GetCell(i));

    if (!tetra)
      {
      continue;
      }

    boundaryTetra = false;
    if (this->UseCaps)
      {
      for (j=0; j<tetra->GetNumberOfPoints(); j++)
        {
        if (this->CapCenterIds->IsId(tetra->GetPointId(j))!=-1)
          {
          boundaryTetra = true;
          }
        }
      } 

    tetra->GetPoints()->GetPoint(0,p0);
    tetra->GetPoints()->GetPoint(1,p1);
    tetra->GetPoints()->GetPoint(2,p2);
    tetra->GetPoints()->GetPoint(3,p3);
    vtkTetra::Circumsphere(p0,p1,p2,p3,circumcenter);

    for (j=0; j<3; j++)
      {
      v0[j] = p0[j] - circumcenter[j];
      v1[j] = p1[j] - circumcenter[j];
      v2[j] = p2[j] - circumcenter[j];
      v3[j] = p3[j] - circumcenter[j];
      }
    
    outwardPointNormals->GetTuple(tetra->GetPointId(0),n0);
    outwardPointNormals->GetTuple(tetra->GetPointId(1),n1);
    outwardPointNormals->GetTuple(tetra->GetPointId(2),n2);
    outwardPointNormals->GetTuple(tetra->GetPointId(3),n3);

    dot0 = vtkMath::Dot(v0,n0);
    dot1 = vtkMath::Dot(v1,n1);
    dot2 = vtkMath::Dot(v2,n2);
    dot3 = vtkMath::Dot(v3,n3);

    allDotPositive = false;
    allDotMinusOnePositive = false;

    if ((dot0>tolerance)&&(dot1>tolerance)&&(dot2>tolerance)&&(dot3>tolerance))
      {
      allDotPositive = true;
      }
    else if (((dot0>tolerance)&&(dot1>tolerance)&&(dot2>tolerance))||
             ((dot0>tolerance)&&(dot1>tolerance)&&(dot3>tolerance))||
             ((dot0>tolerance)&&(dot2>tolerance)&&(dot3>tolerance))||
             ((dot1>tolerance)&&(dot2>tolerance)&&(dot3>tolerance)))
      {
      allDotMinusOnePositive = true;
      }

    keepCell->SetValue(i,0);

    if (allDotPositive)
      {
      keepCell->SetValue(i,1);
      }
    else if (boundaryTetra)
      {
      if (allDotMinusOnePositive)
        {
        keepCell->SetValue(i,1);
        }
      }
    }
    
  if (this->RemoveSubresolutionTetrahedra)
    {
    double pt0[3], pt1[3], pt2[3];
    this->Surface->BuildLinks();

    vtkIdList* cellNeighbors = vtkIdList::New();
    for (i=0; i<input->GetNumberOfCells(); i++)
      {
      if (keepCell->GetValue(i) == 0)
        {
        continue;
        }

      tetra = vtkTetra::SafeDownCast(input->GetCell(i));
      if (!tetra)
        {
        continue;
        }
      bool onNewBoundary = false;
      for (j=0; j<tetra->GetNumberOfFaces(); j++)
        {
        cellNeighbors->Initialize();
        input->GetCellNeighbors(i,tetra->GetFace(j)->GetPointIds(),cellNeighbors);
        if (cellNeighbors->GetNumberOfIds() == 0 || keepCell->GetValue(cellNeighbors->GetId(0)) == 0)
          {
          onNewBoundary = true;
          }
        }
      if (!onNewBoundary)
        {
        continue;
        }

      tetra->GetPoints()->GetPoint(0,p0);
      tetra->GetPoints()->GetPoint(1,p1);
      tetra->GetPoints()->GetPoint(2,p2);
      tetra->GetPoints()->GetPoint(3,p3);
      double circumradius = vtkTetra::Circumsphere(p0,p1,p2,p3,circumcenter);

      for (j=0; j<tetra->GetNumberOfPoints(); j++)
        {
        vtkIdType ncells;
        vtkIdType* cells;
        this->Surface->GetPointCells(j,ncells,cells);
        double minEdgeLength = VTK_VMTK_LARGE_DOUBLE;
        int k;
        for (k=0; k<ncells; k++)
          {
          vtkTriangle* triangle = vtkTriangle::SafeDownCast(this->Surface->GetCell(cells[k]));
          if (!triangle)
            {
            continue;
            }
          triangle->GetPoints()->GetPoint(0,pt0);
          triangle->GetPoints()->GetPoint(1,pt1);
          triangle->GetPoints()->GetPoint(2,pt2);
          double triangleArea = triangle->TriangleArea(pt0,pt1,pt2);
          double edgeLength = sqrt(2.0 * triangleArea);
          if (edgeLength < minEdgeLength)
              {
              minEdgeLength = edgeLength;
              }
          }
        // TODO: set factor as a member variable.
        if (circumradius < this->SubresolutionFactor * minEdgeLength)
          {
          keepCell->SetValue(i,0);
          }
        }
      }
    cellNeighbors->Delete();
    }
 
  for (i=0; i<input->GetNumberOfCells(); i++)
    {
    if (keepCell->GetValue(i))
      {
      newCellTypes->InsertNextId(VTK_TETRA);
      newTetras->InsertNextCell(input->GetCell(i));
      }
    }

  int* newCellTypesInt = new int[newCellTypes->GetNumberOfIds()];
  for (i=0; i<newCellTypes->GetNumberOfIds(); i++)
    {
    newCellTypesInt[i] = newCellTypes->GetId(i);
    }

  output->SetPoints(input->GetPoints());
  output->GetPointData()->PassData(input->GetPointData());
  output->SetCells(newCellTypesInt,newTetras);

  // Destroy
  newTetras->Delete();
  newCellTypes->Delete();
  keepCell->Delete();

  delete[] newCellTypesInt;

  return 1;
}

void vtkvmtkInternalTetrahedraExtractor::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
