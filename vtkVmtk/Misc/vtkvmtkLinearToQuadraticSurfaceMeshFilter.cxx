/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkLinearToQuadraticSurfaceMeshFilter.cxx,v $
Language:  C++
Date:      $Date: 2005/03/31 15:49:05 $
Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkLinearToQuadraticSurfaceMeshFilter.h"
#include "vtkGeometryFilter.h"
#include "vtkInterpolatingSubdivisionFilter.h"
#include "vtkLinearSubdivisionFilter.h"
#include "vtkButterflySubdivisionFilter.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkLinearToQuadraticSurfaceMeshFilter);

vtkvmtkLinearToQuadraticSurfaceMeshFilter::vtkvmtkLinearToQuadraticSurfaceMeshFilter()
{
}

vtkvmtkLinearToQuadraticSurfaceMeshFilter::~vtkvmtkLinearToQuadraticSurfaceMeshFilter()
{
}

int vtkvmtkLinearToQuadraticSurfaceMeshFilter::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkUnstructuredGrid *input = vtkUnstructuredGrid::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkIdType nqpts, *qpts, npts0, *pts0, npts1, *pts1;
  vtkIdType numberOfInputCells;
  vtkPolyData* subdividedSurface;
  vtkCellArray* quadraticTriangles;
  vtkIdList* quadraticTriangleCellTypes;

  vtkGeometryFilter* geometryFilter;
  vtkInterpolatingSubdivisionFilter* subdivisionFilter;

  geometryFilter = vtkGeometryFilter::New();
#if (VTK_MAJOR_VERSION <= 5)
  geometryFilter->SetInput(input);
#else
  geometryFilter->SetInputData(input);
#endif
  geometryFilter->MergingOff();

  switch (this->SubdivisionMethod)
    {
    case LINEAR_SUBDIVISION:
      subdivisionFilter = vtkLinearSubdivisionFilter::New();
      break;
    case BUTTERFLY_SUBDIVISION:
      subdivisionFilter = vtkButterflySubdivisionFilter::New();
      break;
    default:
      vtkErrorMacro(<<"Unsupported subdivision method.");
      return 1;
    }

#if (VTK_MAJOR_VERSION <= 5)
  subdivisionFilter->SetInput(geometryFilter->GetOutput());
#else
  subdivisionFilter->SetInputConnection(geometryFilter->GetOutputPort());
#endif
  subdivisionFilter->SetNumberOfSubdivisions(1);
  subdivisionFilter->Update();
  
  subdividedSurface = subdivisionFilter->GetOutput();

  output->SetPoints(subdividedSurface->GetPoints());
  output->GetPointData()->PassData(subdividedSurface->GetPointData());
  output->GetCellData()->PassData(subdividedSurface->GetCellData());

  //  input->BuildCells();
  subdividedSurface->BuildCells();

  numberOfInputCells = input->GetNumberOfCells();  

  quadraticTriangles = vtkCellArray::New();
  quadraticTriangleCellTypes = vtkIdList::New();

  nqpts = 6;
  qpts = new vtkIdType[nqpts];

  // strongly based on the way output cells are built in vtkInterpolatingSubdivisionFilter !!!
  for (int i=0; i<numberOfInputCells; i++)
    {
    input->GetCellPoints(i,npts0,pts0);
    subdividedSurface->GetCellPoints(4*i+3,npts1,pts1);

    qpts[0] = pts0[0];
    qpts[1] = pts0[1];
    qpts[2] = pts0[2];
    qpts[3] = pts1[0];
    qpts[4] = pts1[1];
    qpts[5] = pts1[2];

    // old PolyDataToPolyData version
//     qpts[0] = pts0[0];
//     qpts[1] = pts1[0];
//     qpts[2] = pts0[1];
//     qpts[3] = pts1[1];
//     qpts[4] = pts0[2];
//     qpts[5] = pts1[2];

    quadraticTriangles->InsertNextCell(nqpts,qpts);
    quadraticTriangleCellTypes->InsertNextId(VTK_QUADRATIC_TRIANGLE);
    }

  delete[] qpts;

  int* quadraticTriangleCellTypesInt = new int[quadraticTriangleCellTypes->GetNumberOfIds()];
  for (int i=0; i<quadraticTriangleCellTypes->GetNumberOfIds(); i++)
    {
    quadraticTriangleCellTypesInt[i] = quadraticTriangleCellTypes->GetId(i);
    }                                                                                                             

  output->SetCells(quadraticTriangleCellTypesInt,quadraticTriangles);

  delete[] quadraticTriangleCellTypesInt;

  geometryFilter->Delete();
  subdivisionFilter->Delete();
  quadraticTriangles->Delete();
  quadraticTriangleCellTypes->Delete();

  return 1;
}

void vtkvmtkLinearToQuadraticSurfaceMeshFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
