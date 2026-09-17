/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataCenterlineSections.cxx,v $
Language:  C++
Date:      $Date: 2006/10/17 15:16:16 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataCenterlineSections.h"
#include "vtkvmtkPolyDataBranchSections.h"
#include "vtkPolyData.h"
#include "vtkPolyLine.h"
#include "vtkPolygon.h"
#include "vtkTriangle.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkPlane.h"
#include "vtkCutter.h"
#include "vtkStripper.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkMath.h"
#include "vtkCleanPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkvmtkMath.h"
#include "vtkvmtkCenterlineSphereDistance.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkIdList.h"
#include "vtkNew.h"
#include "vtkVersionMacros.h"

#include <algorithm>
#include <vector>

#include "vtkvmtkCenterlineUtilities.h"
#include "vtkvmtkPolyDataBranchUtilities.h"


vtkStandardNewMacro(vtkvmtkPolyDataCenterlineSections);

namespace
{
// Selects the cells of a polygonal surface that a plane crosses. Cutting only
// these cells gives the same cut as cutting all cells, as the others contribute
// nothing to it. Crossing cells are found from the signed distance of each
// point to the plane, with a small tolerance, so that no cell that a cutter
// would cut is left out.
class CrossingCells
{
  public:
  explicit CrossingCells(vtkPolyData* surface)
    : Surface(surface),
      Tolerance(1.0e-6 * surface->GetLength())
    {
    if (surface->GetNumberOfCells() == 0 || surface->GetNumberOfCells() != surface->GetNumberOfPolys())
      {
      return;
      }
    // Point coordinates and cell connectivity are copied once, as reading them
    // through vtkPoints and vtkCellArray for every plane would take longer than
    // the cut itself
    this->Points.resize(3 * surface->GetNumberOfPoints());
    for (vtkIdType k=0; k<surface->GetNumberOfPoints(); k++)
      {
      surface->GetPoint(k,&this->Points[3*k]);
      }
    vtkIdType numberOfCellPoints;
    const vtkIdType* cellPoints;
    vtkCellArray* polys = surface->GetPolys();
    polys->InitTraversal();
    this->CellOffsets.push_back(0);
    while (polys->GetNextCell(numberOfCellPoints,cellPoints))
      {
      this->CellConnectivity.insert(this->CellConnectivity.end(),cellPoints,cellPoints + numberOfCellPoints);
      this->CellOffsets.push_back(static_cast<vtkIdType>(this->CellConnectivity.size()));
      }
    this->SignedDistances.resize(surface->GetNumberOfPoints());
    }

  // Only surfaces that consist of polygons are supported
  bool CanSelect() const
    {
    return !this->CellOffsets.empty();
    }

  // Set cells to the cells of the surface that the plane specified by origin and
  // normal crosses, with the points and point data of the surface
  void Select(const double origin[3], const double normal[3], vtkPolyData* cells)
    {
    for (size_t k=0; k<this->SignedDistances.size(); k++)
      {
      const double* point = &this->Points[3*k];
      this->SignedDistances[k] = normal[0] * (point[0] - origin[0]) + normal[1] * (point[1] - origin[1]) + normal[2] * (point[2] - origin[2]);
      }
    vtkNew<vtkCellArray> crossingPolys;
    for (size_t c=0; c+1<this->CellOffsets.size(); c++)
      {
      double minimumDistance = VTK_DOUBLE_MAX;
      double maximumDistance = -VTK_DOUBLE_MAX;
      for (vtkIdType k=this->CellOffsets[c]; k<this->CellOffsets[c+1]; k++)
        {
        minimumDistance = std::min(minimumDistance,this->SignedDistances[this->CellConnectivity[k]]);
        maximumDistance = std::max(maximumDistance,this->SignedDistances[this->CellConnectivity[k]]);
        }
      if (minimumDistance <= this->Tolerance && maximumDistance >= -this->Tolerance)
        {
        crossingPolys->InsertNextCell(this->CellOffsets[c+1] - this->CellOffsets[c],&this->CellConnectivity[this->CellOffsets[c]]);
        }
      }
    cells->Initialize();
    cells->SetPoints(this->Surface->GetPoints());
    cells->SetPolys(crossingPolys);
    cells->GetPointData()->ShallowCopy(this->Surface->GetPointData());
    }

  private:
  vtkPolyData* Surface;
  const double Tolerance;
  std::vector<double> Points;
  std::vector<vtkIdType> CellOffsets;
  std::vector<vtkIdType> CellConnectivity;
  std::vector<double> SignedDistances;
};
}

vtkvmtkPolyDataCenterlineSections::vtkvmtkPolyDataCenterlineSections()
{
  this->Centerlines = NULL;

  this->CenterlineSectionAreaArrayName = NULL;
  this->CenterlineSectionMinSizeArrayName = NULL;
  this->CenterlineSectionMaxSizeArrayName = NULL;
  this->CenterlineSectionShapeArrayName = NULL;
  this->CenterlineSectionClosedArrayName = NULL;
}

vtkvmtkPolyDataCenterlineSections::~vtkvmtkPolyDataCenterlineSections()
{
  if (this->Centerlines)
    {
    this->Centerlines->Delete();
    this->Centerlines = NULL;
    }

  if (this->CenterlineSectionAreaArrayName)
    {
    delete[] this->CenterlineSectionAreaArrayName;
    this->CenterlineSectionAreaArrayName = NULL;
    }

  if (this->CenterlineSectionMinSizeArrayName)
    {
    delete[] this->CenterlineSectionMinSizeArrayName;
    this->CenterlineSectionMinSizeArrayName = NULL;
    }

  if (this->CenterlineSectionMaxSizeArrayName)
    {
    delete[] this->CenterlineSectionMaxSizeArrayName;
    this->CenterlineSectionMaxSizeArrayName = NULL;
    }

  if (this->CenterlineSectionShapeArrayName)
    {
    delete[] this->CenterlineSectionShapeArrayName;
    this->CenterlineSectionShapeArrayName = NULL;
    }

  if (this->CenterlineSectionClosedArrayName)
    {
    delete[] this->CenterlineSectionClosedArrayName;
    this->CenterlineSectionClosedArrayName = NULL;
    }
}

int vtkvmtkPolyDataCenterlineSections::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (!this->Centerlines)
    {
    vtkErrorMacro(<<"Centerlines not set");
    return 1;
    }

  if (!this->CenterlineSectionAreaArrayName)
    {
    vtkErrorMacro(<<"CenterlineSectionAreaArrayName not specified");
    return 1;
    }

  if (!CenterlineSectionMinSizeArrayName)
    {
    vtkErrorMacro(<<"CenterlineSectionMinSizeArrayName not specified");
    return 1;
    }

  if (!CenterlineSectionMaxSizeArrayName)
    {
    vtkErrorMacro(<<"CenterlineSectionMaxSizeArrayName not specified");
    return 1;
    }

  if (!CenterlineSectionShapeArrayName)
    {
    vtkErrorMacro(<<"CenterlineSectionShapeArrayName not specified");
    return 1;
    }

  if (!CenterlineSectionClosedArrayName)
    {
    vtkErrorMacro(<<"CenterlineSectionClosedArrayName not specified");
    return 1;
    }

  this->SetProgressText("Computing centerline sections");
  this->UpdateProgress(0.0);
  this->ComputeCenterlineSections(input,output);
  this->UpdateProgress(1.0);

  return 1;
}

bool vtkvmtkPolyDataCenterlineSections::AbortRequested()
{
#if VTK_MAJOR_VERSION > 9 || (VTK_MAJOR_VERSION == 9 && VTK_MINOR_VERSION >= 4)
  return this->CheckAbort();
#else
  return this->GetAbortExecute() != 0;
#endif
}

void vtkvmtkPolyDataCenterlineSections::ComputeCenterlineSections(vtkPolyData* input, vtkPolyData* output)
{
  vtkNew<vtkPoints> outputPoints;
  vtkNew<vtkCellArray> outputPolys;

  output->SetPoints(outputPoints);
  output->SetPolys(outputPolys);
  vtkPoints* centerlineSectionPoints = outputPoints;
  vtkCellArray* centerlineSectionPolys = outputPolys;

  int numberOfCenterlinePoints = this->Centerlines->GetNumberOfPoints();

  // Sections are stored both on the output, one tuple per section, and on
  // the centerline points. Centerline points that get no section keep 0.
  vtkNew<vtkDoubleArray> centerlineSectionAreaArray;
  centerlineSectionAreaArray->SetName(this->CenterlineSectionAreaArrayName);
  vtkNew<vtkDoubleArray> centerlineSectionMinSizeArray;
  centerlineSectionMinSizeArray->SetName(this->CenterlineSectionMinSizeArrayName);
  vtkNew<vtkDoubleArray> centerlineSectionMaxSizeArray;
  centerlineSectionMaxSizeArray->SetName(this->CenterlineSectionMaxSizeArrayName);
  vtkNew<vtkDoubleArray> centerlineSectionShapeArray;
  centerlineSectionShapeArray->SetName(this->CenterlineSectionShapeArrayName);
  vtkNew<vtkIntArray> centerlineSectionClosedArray;
  centerlineSectionClosedArray->SetName(this->CenterlineSectionClosedArrayName);

  output->GetCellData()->AddArray(centerlineSectionAreaArray);
  output->GetCellData()->AddArray(centerlineSectionMinSizeArray);
  output->GetCellData()->AddArray(centerlineSectionMaxSizeArray);
  output->GetCellData()->AddArray(centerlineSectionShapeArray);
  output->GetCellData()->AddArray(centerlineSectionClosedArray);

  vtkNew<vtkDoubleArray> centerlineAreaArray;
  centerlineAreaArray->SetName(this->CenterlineSectionAreaArrayName);
  vtkNew<vtkDoubleArray> centerlineMinSizeArray;
  centerlineMinSizeArray->SetName(this->CenterlineSectionMinSizeArrayName);
  vtkNew<vtkDoubleArray> centerlineMaxSizeArray;
  centerlineMaxSizeArray->SetName(this->CenterlineSectionMaxSizeArrayName);
  vtkNew<vtkDoubleArray> centerlineShapeArray;
  centerlineShapeArray->SetName(this->CenterlineSectionShapeArrayName);
  vtkNew<vtkIntArray> centerlineClosedArray;
  centerlineClosedArray->SetName(this->CenterlineSectionClosedArrayName);

  vtkDataArray* centerlineArrays[] = {centerlineAreaArray, centerlineMinSizeArray, centerlineMaxSizeArray, centerlineShapeArray, centerlineClosedArray};
  for (vtkDataArray* centerlineArray : centerlineArrays)
    {
    centerlineArray->SetNumberOfTuples(numberOfCenterlinePoints);
    centerlineArray->Fill(0.0);
    this->Centerlines->GetPointData()->AddArray(centerlineArray);
    }

  // Points are visited in the order of the centerline cells and of the points
  // in each cell. A point shared by several cells gets a single section.
  std::vector<bool> visited(this->Centerlines->GetNumberOfPoints(),false);
  // Progress is reported, and an abort checked for, about every 1% of points
  const vtkIdType progressInterval = std::max<vtkIdType>(numberOfCenterlinePoints / 100, 1);
  vtkIdType numberOfVisitedPoints = 0;

  // The cutter visits every cell of its input, which dominates the time spent
  // on a section, so only the cells that the plane crosses are cut
  CrossingCells crossingCells(input);
  vtkNew<vtkPolyData> sectionInput;

  vtkNew<vtkIdList> cellPointIds;
  for (vtkIdType cellId=0; cellId<this->Centerlines->GetNumberOfCells(); cellId++)
    {
    this->Centerlines->GetCellPoints(cellId,cellPointIds);
    for (vtkIdType i=0; i<cellPointIds->GetNumberOfIds(); i++)
      {
      vtkIdType pointId = cellPointIds->GetId(i);
      if (visited[pointId])
        {
        continue;
        }
      visited[pointId] = true;

      if (numberOfVisitedPoints % progressInterval == 0)
        {
        this->UpdateProgress(static_cast<double>(numberOfVisitedPoints) / numberOfCenterlinePoints);
        if (this->AbortRequested())
          {
          return;
          }
        }
      numberOfVisitedPoints++;

      double origin[3], normal[3];
      if (!this->ComputeSectionPlane(cellId,i,origin,normal))
        {
        continue;
        }

      vtkNew<vtkPolyData> section;
      bool closed = false;
      if (crossingCells.CanSelect())
        {
        crossingCells.Select(origin,normal,sectionInput);
        this->ExtractSection(sectionInput,origin,normal,section,closed);
        }
      else
        {
        this->ExtractSection(input,origin,normal,section,closed);
        }

      section->BuildCells();
      if (section->GetNumberOfCells() == 0)
        {
        // The plane does not cut the surface, for example past an open end
        continue;
        }

      vtkPoints* sectionCellPoints = section->GetCell(0)->GetPoints();
      int numberOfSectionCellPoints = sectionCellPoints->GetNumberOfPoints();
      vtkIdType sectionId = centerlineSectionPolys->InsertNextCell(numberOfSectionCellPoints);
      for (int k=0; k<numberOfSectionCellPoints; k++)
        {
        vtkIdType sectionPointId = centerlineSectionPoints->InsertNextPoint(sectionCellPoints->GetPoint(k));
        centerlineSectionPolys->InsertCellPoint(sectionPointId);
        }

      double area = vtkvmtkPolyDataBranchSections::ComputeBranchSectionArea(section);
      double sizeRange[2];
      double shape = vtkvmtkPolyDataBranchSections::ComputeBranchSectionShape(section,origin,sizeRange);

      centerlineSectionAreaArray->InsertNextValue(area);
      centerlineSectionMinSizeArray->InsertNextValue(sizeRange[0]);
      centerlineSectionMaxSizeArray->InsertNextValue(sizeRange[1]);
      centerlineSectionShapeArray->InsertNextValue(shape);
      centerlineSectionClosedArray->InsertNextValue(closed);

      centerlineAreaArray->SetValue(pointId,area);
      centerlineMinSizeArray->SetValue(pointId,sizeRange[0]);
      centerlineMaxSizeArray->SetValue(pointId,sizeRange[1]);
      centerlineShapeArray->SetValue(pointId,shape);
      centerlineClosedArray->SetValue(pointId,closed);

      this->ProcessSection(input,pointId,sectionId,section,origin,normal,closed);
      }
    }
}

bool vtkvmtkPolyDataCenterlineSections::ComputeSectionPlane(vtkIdType cellId, vtkIdType cellPointIndex, double origin[3], double normal[3])
{
  vtkNew<vtkIdList> cellPointIds;
  this->Centerlines->GetCellPoints(cellId,cellPointIds);
  const vtkIdType numberOfCellPoints = cellPointIds->GetNumberOfIds();

  this->Centerlines->GetPoint(cellPointIds->GetId(cellPointIndex),origin);

  // Average the directions from the previous and to the next point. Points
  // that coincide with the section point, such as a duplicated end point, have
  // no direction, so the nearest distinct point on each side is used instead.
  normal[0] = normal[1] = normal[2] = 0.0;
  double neighborPoint[3], direction[3];
  for (vtkIdType step : {-1, 1})
    {
    for (vtkIdType j=cellPointIndex+step; j>=0 && j<numberOfCellPoints; j+=step)
      {
      this->Centerlines->GetPoint(cellPointIds->GetId(j),neighborPoint);
      vtkMath::Subtract(neighborPoint,origin,direction);
      double distance = vtkMath::Normalize(direction);
      if (distance > 0.0)
        {
        normal[0] += step * direction[0];
        normal[1] += step * direction[1];
        normal[2] += step * direction[2];
        break;
        }
      }
    }

  return vtkMath::Normalize(normal) > 0.0;
}

void vtkvmtkPolyDataCenterlineSections::ExtractSection(vtkPolyData* input, double origin[3], double normal[3], vtkPolyData* section, bool & closed)
{
  vtkvmtkPolyDataBranchSections::ExtractCylinderSection(input,origin,normal,section,closed);
}

void vtkvmtkPolyDataCenterlineSections::ProcessSection(vtkPolyData* vtkNotUsed(input), vtkIdType vtkNotUsed(pointId), vtkIdType vtkNotUsed(sectionId), vtkPolyData* vtkNotUsed(section), double* vtkNotUsed(origin), double* vtkNotUsed(normal), bool vtkNotUsed(closed))
{
}

void vtkvmtkPolyDataCenterlineSections::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
