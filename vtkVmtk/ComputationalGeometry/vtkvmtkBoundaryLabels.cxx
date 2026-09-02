/*=========================================================================

Program:   VMTK

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkBoundaryLabels.h"
#include "vtkvmtkPolyDataBoundaryExtractor.h"

#include "vtkCellArray.h"
#include "vtkDataArray.h"
#include "vtkIdList.h"
#include "vtkIdTypeArray.h"
#include "vtkMath.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"

#include <algorithm>
#include <map>
#include <vector>

vtkStandardNewMacro(vtkvmtkBoundaryLabels);

namespace
{

// The two arrays are written as vtkIdTypeArray, but a filter that knows nothing about them may
// have passed them through as something else, so read them as plain numbers and round, the way
// the boundary point ids of the extractor's own output are read everywhere in these filters.
vtkDataArray* GetSingleComponentPointArray(vtkPolyData* surface, const char* arrayName)
{
  if (!surface || !arrayName || !arrayName[0])
    {
    return nullptr;
    }
  vtkDataArray* array = surface->GetPointData()->GetArray(arrayName);
  if (!array || array->GetNumberOfComponents() != 1
      || array->GetNumberOfTuples() != surface->GetNumberOfPoints())
    {
    return nullptr;
    }
  return array;
}

vtkIdType GetValue(vtkDataArray* array, vtkIdType pointId)
{
  return static_cast<vtkIdType>(vtkMath::Round(array->GetComponent(pointId,0)));
}

}

bool vtkvmtkBoundaryLabels::HasBoundaryLabels(vtkPolyData* surface, const char* boundaryLabelsArrayName, const char* boundaryPointOrderArrayName)
{
  return GetSingleComponentPointArray(surface,boundaryLabelsArrayName) != nullptr
      && GetSingleComponentPointArray(surface,boundaryPointOrderArrayName) != nullptr;
}

bool vtkvmtkBoundaryLabels::GetBoundaries(vtkPolyData* surface, const char* boundaryLabelsArrayName, const char* boundaryPointOrderArrayName, vtkPolyData* boundaries, vtkIdList* boundaryLabels)
{
  if (!boundaries)
    {
    return false;
    }

  boundaries->Initialize();
  if (boundaryLabels)
    {
    boundaryLabels->Initialize();
    }

  vtkDataArray* labelsArray = GetSingleComponentPointArray(surface,boundaryLabelsArrayName);
  vtkDataArray* orderArray = GetSingleComponentPointArray(surface,boundaryPointOrderArrayName);
  if (!labelsArray || !orderArray)
    {
    return false;
    }

  // Gather the points of each boundary, keyed by label so that the boundaries come out in
  // ascending label order however the points happen to be numbered.
  std::map<vtkIdType,std::vector<std::pair<vtkIdType,vtkIdType> > > pointsByLabel;
  vtkIdType numberOfPoints = surface->GetNumberOfPoints();
  for (vtkIdType pointId=0; pointId<numberOfPoints; pointId++)
    {
    vtkIdType label = GetValue(labelsArray,pointId);
    if (label < 0)
      {
      continue;
      }
    vtkIdType order = GetValue(orderArray,pointId);
    if (order < 0)
      {
      // On a boundary by one array and not by the other: the two no longer agree, so neither
      // can be trusted.
      return false;
      }
    pointsByLabel[label].push_back(std::make_pair(order,pointId));
    }

  if (pointsByLabel.empty())
    {
    // A closed surface has no boundaries, and saying so is a valid answer rather than a
    // failure. The arrays are still there, so the caller has nothing to fall back to.
    vtkNew<vtkPoints> emptyPoints;
    vtkNew<vtkCellArray> emptyLines;
    vtkNew<vtkIdTypeArray> emptyScalars;
    boundaries->SetPoints(emptyPoints);
    boundaries->SetLines(emptyLines);
    boundaries->GetPointData()->SetScalars(emptyScalars);
    return true;
    }

  for (std::map<vtkIdType,std::vector<std::pair<vtkIdType,vtkIdType> > >::iterator
       boundary=pointsByLabel.begin(); boundary!=pointsByLabel.end(); ++boundary)
    {
    std::vector<std::pair<vtkIdType,vtkIdType> >& ring = boundary->second;
    // A ring of fewer than three points is not one the extractor would have emitted
    // (see vtkvmtkPolyDataBoundaryExtractor, which requires more than two).
    if (ring.size() < 3)
      {
      return false;
      }
    std::sort(ring.begin(),ring.end());
    // The order values have to be the whole run 0..n-1. A gap, a repeat, or a value off the end
    // means the ring this describes is not the ring that is there now.
    for (size_t index=0; index<ring.size(); index++)
      {
      if (ring[index].first != static_cast<vtkIdType>(index))
        {
        return false;
        }
      }
    }

  // Each point of a boundary is listed once, the way vtkvmtkPolyDataBoundaryExtractor lists
  // them, so that a boundary reconstructed from the arrays is the boundary the extractor would
  // have produced and the two ways of getting the boundaries of a surface are interchangeable.
  vtkNew<vtkPoints> newPoints;
  vtkNew<vtkCellArray> newLines;
  vtkNew<vtkIdTypeArray> newScalars;
  vtkNew<vtkIdList> newCell;

  for (std::map<vtkIdType,std::vector<std::pair<vtkIdType,vtkIdType> > >::iterator
       boundary=pointsByLabel.begin(); boundary!=pointsByLabel.end(); ++boundary)
    {
    const std::vector<std::pair<vtkIdType,vtkIdType> >& ring = boundary->second;

    newCell->Initialize();
    newCell->SetNumberOfIds(static_cast<vtkIdType>(ring.size()));
    for (size_t index=0; index<ring.size(); index++)
      {
      vtkIdType pointId = ring[index].second;
      newCell->SetId(static_cast<vtkIdType>(index),newPoints->InsertNextPoint(surface->GetPoint(pointId)));
      newScalars->InsertNextValue(pointId);
      }

    newLines->InsertNextCell(newCell);

    if (boundaryLabels)
      {
      boundaryLabels->InsertNextId(boundary->first);
      }
    }

  boundaries->SetPoints(newPoints);
  boundaries->SetLines(newLines);
  boundaries->GetPointData()->SetScalars(newScalars);

  return true;
}

bool vtkvmtkBoundaryLabels::GetOrExtractBoundaries(vtkPolyData* surface, const char* boundaryLabelsArrayName, const char* boundaryPointOrderArrayName, vtkPolyData* boundaries, vtkIdList* boundaryLabels, vtkObject* warningSource)
{
  if (!surface || !boundaries)
    {
    return false;
    }

  if (boundaryLabels)
    {
    boundaryLabels->Reset();
    }

  if (boundaryLabelsArrayName && boundaryLabelsArrayName[0]
      && boundaryPointOrderArrayName && boundaryPointOrderArrayName[0])
    {
    if (vtkvmtkBoundaryLabels::GetBoundaries(surface,boundaryLabelsArrayName,boundaryPointOrderArrayName,boundaries,boundaryLabels))
      {
      return true;
      }
    if (warningSource)
      {
      vtkWarningWithObjectMacro(warningSource,<<"The boundary label arrays are missing from the input surface or no longer describe it; "
                                              <<"extracting its boundaries instead, and naming the caps by position.");
      }
    }

  vtkNew<vtkvmtkPolyDataBoundaryExtractor> boundaryExtractor;
  boundaryExtractor->SetInputData(surface);
  boundaryExtractor->Update();
  boundaries->ShallowCopy(boundaryExtractor->GetOutput());

  return false;
}

void vtkvmtkBoundaryLabels::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
