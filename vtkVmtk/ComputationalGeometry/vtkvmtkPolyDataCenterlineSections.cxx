/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataCenterlineSections.cxx,v $
Language:  C++
Date:      $Date: 2006/10/17 15:16:16 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

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

#include "vtkvmtkCenterlineUtilities.h"
#include "vtkvmtkPolyDataBranchUtilities.h"


vtkStandardNewMacro(vtkvmtkPolyDataCenterlineSections);

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

  vtkPoints* outputPoints = vtkPoints::New();
  vtkCellArray* outputPolys = vtkCellArray::New();

  output->SetPoints(outputPoints);
  output->SetPolys(outputPolys);

  vtkDoubleArray* centerlineSectionAreaArray = vtkDoubleArray::New();
  centerlineSectionAreaArray->SetName(this->CenterlineSectionAreaArrayName);

  vtkDoubleArray* centerlineSectionShapeArray = vtkDoubleArray::New();
  centerlineSectionShapeArray->SetName(this->CenterlineSectionShapeArrayName);

  vtkDoubleArray* centerlineSectionMinSizeArray = vtkDoubleArray::New();
  centerlineSectionMinSizeArray->SetName(this->CenterlineSectionMinSizeArrayName);

  vtkDoubleArray* centerlineSectionMaxSizeArray = vtkDoubleArray::New();
  centerlineSectionMaxSizeArray->SetName(this->CenterlineSectionMaxSizeArrayName);

  vtkIntArray* centerlineSectionClosedArray = vtkIntArray::New();
  centerlineSectionClosedArray->SetName(this->CenterlineSectionClosedArrayName);

  vtkDoubleArray* centerlineAreaArray = vtkDoubleArray::New();
  centerlineAreaArray->SetName(this->CenterlineSectionAreaArrayName);

  vtkDoubleArray* centerlineShapeArray = vtkDoubleArray::New();
  centerlineShapeArray->SetName(this->CenterlineSectionShapeArrayName);

  vtkDoubleArray* centerlineMinSizeArray = vtkDoubleArray::New();
  centerlineMinSizeArray->SetName(this->CenterlineSectionMinSizeArrayName);

  vtkDoubleArray* centerlineMaxSizeArray = vtkDoubleArray::New();
  centerlineMaxSizeArray->SetName(this->CenterlineSectionMaxSizeArrayName);

  vtkIntArray* centerlineClosedArray = vtkIntArray::New();
  centerlineClosedArray->SetName(this->CenterlineSectionClosedArrayName);

  output->GetCellData()->AddArray(centerlineSectionAreaArray);
  output->GetCellData()->AddArray(centerlineSectionMinSizeArray);
  output->GetCellData()->AddArray(centerlineSectionMaxSizeArray);
  output->GetCellData()->AddArray(centerlineSectionShapeArray);
  output->GetCellData()->AddArray(centerlineSectionClosedArray);

  this->Centerlines->GetPointData()->AddArray(centerlineAreaArray);
  this->Centerlines->GetPointData()->AddArray(centerlineMinSizeArray);
  this->Centerlines->GetPointData()->AddArray(centerlineMaxSizeArray);
  this->Centerlines->GetPointData()->AddArray(centerlineShapeArray);
  this->Centerlines->GetPointData()->AddArray(centerlineClosedArray);

  int numberOfCenterlinePoints = this->Centerlines->GetNumberOfPoints();

  centerlineAreaArray->SetNumberOfTuples(numberOfCenterlinePoints);
  centerlineMinSizeArray->SetNumberOfTuples(numberOfCenterlinePoints);
  centerlineMaxSizeArray->SetNumberOfTuples(numberOfCenterlinePoints);
  centerlineShapeArray->SetNumberOfTuples(numberOfCenterlinePoints);
  centerlineClosedArray->SetNumberOfTuples(numberOfCenterlinePoints);

  int numberOfCenterlineCells = this->Centerlines->GetNumberOfCells();
  int i;
  for (i=0; i<numberOfCenterlineCells; i++)
  {
    this->ComputeCenterlineSections(input,i,output);
  }

  outputPoints->Delete();
  outputPolys->Delete();

  centerlineSectionAreaArray->Delete();
  centerlineSectionMinSizeArray->Delete();
  centerlineSectionMaxSizeArray->Delete();
  centerlineSectionShapeArray->Delete();
  centerlineSectionClosedArray->Delete();
 
  centerlineAreaArray->Delete();
  centerlineMinSizeArray->Delete();
  centerlineMaxSizeArray->Delete();
  centerlineShapeArray->Delete();
  centerlineClosedArray->Delete();
 
  return 1;
}

void vtkvmtkPolyDataCenterlineSections::ComputeCenterlineSections(vtkPolyData* input, int cellId, vtkPolyData* output)
{
  vtkPoints* centerlineSectionPoints = output->GetPoints();
  vtkCellArray* centerlineSectionPolys = output->GetPolys();

  vtkDoubleArray* centerlineSectionAreaArray = vtkDoubleArray::SafeDownCast(output->GetCellData()->GetArray(this->CenterlineSectionAreaArrayName));
  vtkDoubleArray* centerlineSectionMinSizeArray = vtkDoubleArray::SafeDownCast(output->GetCellData()->GetArray(this->CenterlineSectionMinSizeArrayName));
  vtkDoubleArray* centerlineSectionMaxSizeArray = vtkDoubleArray::SafeDownCast(output->GetCellData()->GetArray(this->CenterlineSectionMaxSizeArrayName));
  vtkDoubleArray* centerlineSectionShapeArray = vtkDoubleArray::SafeDownCast(output->GetCellData()->GetArray(this->CenterlineSectionShapeArrayName));
  vtkIntArray* centerlineSectionClosedArray = vtkIntArray::SafeDownCast(output->GetCellData()->GetArray(this->CenterlineSectionClosedArrayName));

  vtkDoubleArray* centerlineAreaArray = vtkDoubleArray::SafeDownCast(this->Centerlines->GetPointData()->GetArray(this->CenterlineSectionAreaArrayName));
  vtkDoubleArray* centerlineMinSizeArray = vtkDoubleArray::SafeDownCast(this->Centerlines->GetPointData()->GetArray(this->CenterlineSectionMinSizeArrayName));
  vtkDoubleArray* centerlineMaxSizeArray = vtkDoubleArray::SafeDownCast(this->Centerlines->GetPointData()->GetArray(this->CenterlineSectionMaxSizeArrayName));
  vtkDoubleArray* centerlineShapeArray = vtkDoubleArray::SafeDownCast(this->Centerlines->GetPointData()->GetArray(this->CenterlineSectionShapeArrayName));
  vtkIntArray* centerlineClosedArray = vtkIntArray::SafeDownCast(this->Centerlines->GetPointData()->GetArray(this->CenterlineSectionClosedArrayName));

  vtkCell* centerlineCell = this->Centerlines->GetCell(cellId);

  vtkPoints* centerlineCellPoints = centerlineCell->GetPoints();

  int numberOfCellPoints = centerlineCellPoints->GetNumberOfPoints();

  int i;

  for (i=0; i<numberOfCellPoints; i++)
  {
    double point[3];
    centerlineCellPoints->GetPoint(i,point);

    double tangent[3];
    tangent[0] = tangent[1] = tangent[2] = 0.0;

    double weightSum = 0.0;
    if (i>0)
    {
      double point0[3], point1[3];
      centerlineCellPoints->GetPoint(i-1,point0);
      centerlineCellPoints->GetPoint(i,point1);
      double distance = sqrt(vtkMath::Distance2BetweenPoints(point0,point1));
      tangent[0] += (point1[0] - point0[0]) / distance;
      tangent[1] += (point1[1] - point0[1]) / distance;
      tangent[2] += (point1[2] - point0[2]) / distance;
      weightSum += 1.0;
    }

    if (i<numberOfCellPoints-1)
    {
      double point0[3], point1[3];
      centerlineCellPoints->GetPoint(i,point0);
      centerlineCellPoints->GetPoint(i+1,point1);
      double distance = sqrt(vtkMath::Distance2BetweenPoints(point0,point1));
      tangent[0] += (point1[0] - point0[0]) / distance;
      tangent[1] += (point1[1] - point0[1]) / distance;
      tangent[2] += (point1[2] - point0[2]) / distance;
      weightSum += 1.0;
    }

    tangent[0] /= weightSum;
    tangent[1] /= weightSum;
    tangent[2] /= weightSum;

    vtkMath::Normalize(tangent);

    //now cut branch with plane and get section. Compute section properties and store them.

    vtkPolyData* section = vtkPolyData::New();
    bool closed = false;
    vtkvmtkPolyDataBranchSections::ExtractCylinderSection(input,point,tangent,section,closed);

    section->BuildCells();
    vtkPoints* sectionCellPoints = section->GetCell(0)->GetPoints();
    int numberOfSectionCellPoints = sectionCellPoints->GetNumberOfPoints();
    centerlineSectionPolys->InsertNextCell(numberOfSectionCellPoints);
    int k;
    for (k=0; k<numberOfSectionCellPoints; k++)
    {
      vtkIdType branchPointId = centerlineSectionPoints->InsertNextPoint(sectionCellPoints->GetPoint(k));
      centerlineSectionPolys->InsertCellPoint(branchPointId);
    }
    
    double area = vtkvmtkPolyDataBranchSections::ComputeBranchSectionArea(section);
    double sizeRange[2];
    double shape = vtkvmtkPolyDataBranchSections::ComputeBranchSectionShape(section,point,sizeRange);

    centerlineSectionAreaArray->InsertNextValue(area);
    centerlineSectionMinSizeArray->InsertNextValue(sizeRange[0]);
    centerlineSectionMaxSizeArray->InsertNextValue(sizeRange[1]);
    centerlineSectionShapeArray->InsertNextValue(shape);
    centerlineSectionClosedArray->InsertNextValue(closed);

    vtkIdType pointId = centerlineCell->GetPointId(i);
    centerlineAreaArray->InsertValue(pointId,area);
    centerlineMinSizeArray->InsertValue(pointId,sizeRange[0]);
    centerlineMaxSizeArray->InsertValue(pointId,sizeRange[1]);
    centerlineShapeArray->InsertValue(pointId,shape);
    centerlineClosedArray->InsertValue(pointId,closed);

    section->Delete();
  }  
}

void vtkvmtkPolyDataCenterlineSections::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
