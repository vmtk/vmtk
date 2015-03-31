/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataCylinderHarmonicMappingFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2005/03/31 15:07:53 $
  Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataCylinderHarmonicMappingFilter.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkCell.h"
#include "vtkvmtkPolyDataBoundaryExtractor.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkPolyDataCylinderHarmonicMappingFilter);

vtkvmtkPolyDataCylinderHarmonicMappingFilter::vtkvmtkPolyDataCylinderHarmonicMappingFilter() 
{
}

vtkvmtkPolyDataCylinderHarmonicMappingFilter::~vtkvmtkPolyDataCylinderHarmonicMappingFilter()
{
}

int vtkvmtkPolyDataCylinderHarmonicMappingFilter::RequestData(
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);

  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkvmtkPolyDataBoundaryExtractor* boundaryExtractor = vtkvmtkPolyDataBoundaryExtractor::New();
#if (VTK_MAJOR_VERSION <= 5)
  boundaryExtractor->SetInput(input);
#else
  boundaryExtractor->SetInputData(input);
#endif
  boundaryExtractor->Update();

  vtkPolyData* boundaries = boundaryExtractor->GetOutput();
  
  int numberOfBoundaries = boundaries->GetNumberOfCells();

  if (numberOfBoundaries != 2)
    {
    vtkWarningMacro(<<"Input poly data is not topologically a cylinder.");
    boundaryExtractor->Delete();
    this->GetOutput()->DeepCopy(input);
    return 1;
    }

  if (this->BoundaryPointIds)
    {
    this->BoundaryPointIds->Delete();
    }

  if (this->BoundaryValues)
    {
    this->BoundaryValues->Delete();
    }

  this->BoundaryPointIds = vtkIdList::New();
  this->BoundaryValues = vtkDoubleArray::New();

  for (int i=0; i<numberOfBoundaries; i++)
    {
    vtkCell* boundaryCell = boundaries->GetCell(i);

    for (int j=0; j<boundaryCell->GetNumberOfPoints(); j++)
      {
      this->BoundaryPointIds->InsertNextId(static_cast<vtkIdType>(boundaries->GetPointData()->GetScalars()->GetTuple1(boundaryCell->GetPointId(j))));
      this->BoundaryValues->InsertNextTuple1(static_cast<double>(i));
      }
    }

  Superclass::RequestData(request,inputVector,outputVector);

  this->BoundaryPointIds->Delete();
  this->BoundaryPointIds = NULL;

  this->BoundaryValues->Delete();
  this->BoundaryValues = NULL;

  return 1;
}
