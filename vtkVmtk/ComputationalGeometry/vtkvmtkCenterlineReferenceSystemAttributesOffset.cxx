/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineReferenceSystemAttributesOffset.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.8 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkCenterlineReferenceSystemAttributesOffset.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkIdList.h"
#include "vtkCell.h"
#include "vtkDoubleArray.h"
#include "vtkTransform.h"
#include "vtkvmtkPolyBallLine.h"
#include "vtkvmtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkCenterlineUtilities.h"
#include "vtkvmtkReferenceSystemUtilities.h"


vtkStandardNewMacro(vtkvmtkCenterlineReferenceSystemAttributesOffset);

vtkvmtkCenterlineReferenceSystemAttributesOffset::vtkvmtkCenterlineReferenceSystemAttributesOffset()
{
  this->OffsetAbscissasArrayName = NULL;
  this->OffsetNormalsArrayName = NULL;

  this->AbscissasArrayName = NULL;
  this->NormalsArrayName = NULL;
  this->GroupIdsArrayName = NULL;
  this->CenterlineIdsArrayName = NULL;

  this->ReferenceSystems = NULL;

  this->ReferenceSystemsNormalArrayName = NULL;
  this->ReferenceSystemsGroupIdsArrayName = NULL;

  this->ReferenceGroupId = 0;
}

vtkvmtkCenterlineReferenceSystemAttributesOffset::~vtkvmtkCenterlineReferenceSystemAttributesOffset()
{
  if (this->OffsetAbscissasArrayName)
    {
    delete[] this->OffsetAbscissasArrayName;
    this->OffsetAbscissasArrayName = NULL;
    }

  if (this->OffsetNormalsArrayName)
    {
    delete[] this->OffsetNormalsArrayName;
    this->OffsetNormalsArrayName = NULL;
    }

  if (this->AbscissasArrayName)
    {
    delete[] this->AbscissasArrayName;
    this->AbscissasArrayName = NULL;
    }

  if (this->NormalsArrayName)
    {
    delete[] this->NormalsArrayName;
    this->NormalsArrayName = NULL;
    }

  if (this->GroupIdsArrayName)
    {
    delete[] this->GroupIdsArrayName;
    this->GroupIdsArrayName = NULL;
    }

  if (this->CenterlineIdsArrayName)
    {
    delete[] this->CenterlineIdsArrayName;
    this->CenterlineIdsArrayName = NULL;
    }

  if (this->NormalsArrayName)
    {
    delete[] this->NormalsArrayName;
    this->NormalsArrayName = NULL;
    }

  if (this->ReferenceSystems)
    {
    this->ReferenceSystems->Delete();
    this->ReferenceSystems = NULL;
    }

  if (this->ReferenceSystemsNormalArrayName)
    {
    delete[] this->ReferenceSystemsNormalArrayName;
    this->ReferenceSystemsNormalArrayName = NULL;
    }

  if (this->ReferenceSystemsGroupIdsArrayName)
    {
    delete[] this->ReferenceSystemsGroupIdsArrayName;
    this->ReferenceSystemsGroupIdsArrayName = NULL;
    }
}

int vtkvmtkCenterlineReferenceSystemAttributesOffset::RequestData(
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

  if (!this->OffsetAbscissasArrayName)
    {
    vtkErrorMacro(<<"OffsetAbscissasArrayName not specified");
    return 1;
    }

  if (!this->OffsetNormalsArrayName)
    {
    vtkErrorMacro(<<"OffsetNormalsArrayName not specified");
    return 1;
    }

  if (!this->AbscissasArrayName)
    {
    vtkErrorMacro(<<"AbscissasArrayName not specified");
    return 1;
    }

  vtkDataArray* abscissasArray = input->GetPointData()->GetArray(this->AbscissasArrayName);

  if (!abscissasArray)
    {
    vtkErrorMacro(<<"AbscissasArray with name specified does not exist");
    return 1;
    }

  if (!this->NormalsArrayName)
    {
    vtkErrorMacro(<<"NormalsArrayName not specified");
    return 1;
    }

  vtkDataArray* normalsArray = input->GetPointData()->GetArray(this->NormalsArrayName);

  if (!normalsArray)
    {
    vtkErrorMacro(<<"NormalsArray with name specified does not exist");
    return 1;
    }

  if (!this->GroupIdsArrayName)
    {
    vtkErrorMacro(<<"GroupIdsArrayName not specified");
    return 1;
    }

  vtkDataArray* groupIdsArray = input->GetCellData()->GetArray(this->GroupIdsArrayName);

  if (!groupIdsArray)
    {
    vtkErrorMacro(<<"GroupIdsArray with name specified does not exist");
    return 1;
    }

  if (!this->CenterlineIdsArrayName)
    {
    vtkErrorMacro(<<"CenterlineIdsArrayName not specified");
    return 1;
    }

  vtkDataArray* centerlineIdsArray = input->GetCellData()->GetArray(this->CenterlineIdsArrayName);

  if (!centerlineIdsArray)
    {
    vtkErrorMacro(<<"CenterlineIdsArray with name specified does not exist");
    return 1;
    }

  if (!this->ReferenceSystems)
    {
    vtkErrorMacro(<<"ReferenceSystems not specified");
    return 1;
    }

  if (this->ReferenceSystems->GetNumberOfPoints() == 0)
    {
    vtkErrorMacro(<<"ReferenceSystems empty");
    return 1;
    }
  
  if (!this->ReferenceSystemsNormalArrayName)
    {
    vtkErrorMacro(<<"ReferenceSystemsNormalArrayName not specified");
    return 1;
    }

  vtkDataArray* referenceSystemsNormalArray = this->ReferenceSystems->GetPointData()->GetArray(this->ReferenceSystemsNormalArrayName);

  if (!referenceSystemsNormalArray)
    {
    vtkErrorMacro(<<"ReferenceSystemNormalsArray with name specified does not exist");
    return 1;
    }

  if (!this->ReferenceSystemsGroupIdsArrayName)
    {
    vtkErrorMacro(<<"ReferenceSystemsGroupIdsArrayName not specified");
    return 1;
    }

  vtkDataArray* referenceSystemsGroupIdsArray = this->ReferenceSystems->GetPointData()->GetArray(this->ReferenceSystemsGroupIdsArrayName);

  if (!referenceSystemsGroupIdsArray)
    {
    vtkErrorMacro(<<"ReferenceSystemsGroupIdsArray with name specified does not exist");
    return 1;
    }

  output->DeepCopy(input);

  vtkDoubleArray* offsetAbscissasArray = vtkDoubleArray::New();
  offsetAbscissasArray->SetName(this->OffsetAbscissasArrayName);
  offsetAbscissasArray->SetNumberOfComponents(1);
  offsetAbscissasArray->SetNumberOfTuples(input->GetNumberOfPoints());

  vtkDoubleArray* offsetNormalsArray = vtkDoubleArray::New();
  offsetNormalsArray->SetName(this->OffsetNormalsArrayName);
  offsetNormalsArray->SetNumberOfComponents(3);
  offsetNormalsArray->SetNumberOfTuples(input->GetNumberOfPoints());

  // For each branch in the reference group, compute nearest point. Get abscissa and normal at that point.
  // Compute abscissa offset and normal offset rotation. 
  // Apply abscissa offset and normal offset rotation to each branch in the centerline.
 
  vtkvmtkPolyBallLine* tube = vtkvmtkPolyBallLine::New();
  tube->SetInput(input);
  tube->UseRadiusInformationOff();

  vtkIdList* tubeInputCellIds = vtkIdList::New();

  double referenceSystemOrigin[3], referenceSystemNormal[3];

  if (this->ReferenceGroupId == -1)
    {
    this->ReferenceGroupId = static_cast<int>(referenceSystemsGroupIdsArray->GetComponent(0,0));
    }
  
  int referenceSystemPointId = vtkvmtkReferenceSystemUtilities::GetReferenceSystemPointId(this->ReferenceSystems,this->ReferenceSystemsGroupIdsArrayName,this->ReferenceGroupId);

  if (referenceSystemPointId == -1)
    {
    vtkErrorMacro(<<"Invalid ReferenceGroupId");
    return 1;
    }

  this->ReferenceSystems->GetPoint(referenceSystemPointId,referenceSystemOrigin);
  referenceSystemsNormalArray->GetTuple(referenceSystemPointId,referenceSystemNormal);

  vtkIdList* groupCellIds = vtkIdList::New();
  vtkvmtkCenterlineUtilities::GetGroupCellIds(input,this->GroupIdsArrayName,this->ReferenceGroupId,groupCellIds);
  for (int i=0; i<groupCellIds->GetNumberOfIds(); i++)
    {
    vtkIdType currentCellId = groupCellIds->GetId(i);
 
    vtkIdType centerlineId = static_cast<int>(centerlineIdsArray->GetComponent(currentCellId,0));
    tubeInputCellIds->Initialize();
    tubeInputCellIds->InsertNextId(currentCellId);
    tube->SetInputCellIds(tubeInputCellIds);
        
    tube->EvaluateFunction(referenceSystemOrigin);
    
    vtkIdType cellId = tube->GetLastPolyBallCellId();
    vtkIdType subId = tube->GetLastPolyBallCellSubId();
    double pcoord = tube->GetLastPolyBallCellPCoord();

    double abscissa = 0.0;
    vtkvmtkCenterlineUtilities::InterpolateTuple1(input,this->AbscissasArrayName,cellId,subId,pcoord,abscissa);
    double abscissaOffset = - abscissa;
    
    double normal[3];
    vtkvmtkCenterlineUtilities::InterpolateTuple3(input,this->NormalsArrayName,cellId,subId,pcoord,normal);
    vtkMath::Normalize(normal);
    
    double point0[3], point1[3], tangent[3];
    input->GetCell(cellId)->GetPoints()->GetPoint(subId,point0);
    input->GetCell(cellId)->GetPoints()->GetPoint(subId+1,point1);
    tangent[0] = point1[0] - point0[0];
    tangent[1] = point1[1] - point0[1];
    tangent[2] = point1[2] - point0[2];
    double normalDot = vtkMath::Dot(tangent,normal);
    tangent[0] -= normalDot * normal[0];
    tangent[1] -= normalDot * normal[1];
    tangent[2] -= normalDot * normal[2];
    vtkMath::Normalize(tangent);
    
    double dot = vtkMath::Dot(tangent,referenceSystemNormal);
    double projectedReferenceSystemNormal[3];
    projectedReferenceSystemNormal[0] = referenceSystemNormal[0] - dot * tangent[0];
    projectedReferenceSystemNormal[1] = referenceSystemNormal[1] - dot * tangent[1];
    projectedReferenceSystemNormal[2] = referenceSystemNormal[2] - dot * tangent[2];
    vtkMath::Normalize(projectedReferenceSystemNormal);

    double angle = vtkvmtkMath::AngleBetweenNormals(normal,projectedReferenceSystemNormal);
    
    double cross[3];
    vtkMath::Cross(projectedReferenceSystemNormal,normal,cross);
    double tangentDot = vtkMath::Dot(tangent,cross);

    if (tangentDot < 0.0)
      {
      angle *= -1.0;
      }
    
    double angleOffset = -angle;

    vtkIdList* centerlineCellIds = vtkIdList::New();
    vtkvmtkCenterlineUtilities::GetCenterlineCellIds(input,this->CenterlineIdsArrayName,centerlineId,centerlineCellIds);
    for (int j=0; j<centerlineCellIds->GetNumberOfIds(); j++)
    {
      vtkCell* branchCell = input->GetCell(centerlineCellIds->GetId(j));
      int numberOfBranchPoints = branchCell->GetNumberOfPoints();
      for (int k=0; k<numberOfBranchPoints; k++)
        {
        vtkIdType branchPointId = branchCell->GetPointId(k);
        double branchPointAbscissa = abscissasArray->GetComponent(branchPointId,0);
        double offsetBranchPointAbscissa = branchPointAbscissa + abscissaOffset;
        offsetAbscissasArray->SetComponent(branchPointId,0,offsetBranchPointAbscissa);

        double branchPointNormal[3];
        normalsArray->GetTuple(branchPointId,branchPointNormal);
        double branchPoint[3];
        branchCell->GetPoints()->GetPoint(k,branchPoint);
        double branchTangent[3];
        branchTangent[0] = branchTangent[1] = branchTangent[2] = 0.0;
        if (k > 0)
          {
          double branchPoint0[3];
          branchCell->GetPoints()->GetPoint(k-1,branchPoint0);
          branchTangent[0] += branchPoint[0] - branchPoint0[0];
          branchTangent[1] += branchPoint[1] - branchPoint0[1];
          branchTangent[2] += branchPoint[2] - branchPoint0[2];
          }
        if (k < numberOfBranchPoints-1)
          {
          double branchPoint1[3];
          branchCell->GetPoints()->GetPoint(k+1,branchPoint1);
          branchTangent[0] += branchPoint1[0] - branchPoint[0];
          branchTangent[1] += branchPoint1[1] - branchPoint[1];
          branchTangent[2] += branchPoint1[2] - branchPoint[2];
          }
        double branchNormalDot = vtkMath::Dot(branchTangent,branchPointNormal);
        branchTangent[0] -= branchNormalDot * branchPointNormal[0];
        branchTangent[1] -= branchNormalDot * branchPointNormal[1];
        branchTangent[2] -= branchNormalDot * branchPointNormal[2];
        vtkMath::Normalize(branchTangent);

        vtkTransform* transform = vtkTransform::New();
        transform->RotateWXYZ(angleOffset/vtkMath::Pi()*180.0,branchTangent);

        double offsetBranchPointNormal[3];
        transform->TransformNormal(branchPointNormal,offsetBranchPointNormal);
        offsetNormalsArray->SetTuple(branchPointId,offsetBranchPointNormal);

        transform->Delete();
        }
      }
    centerlineCellIds->Delete();
    }

  groupCellIds->Delete();
  
  output->GetPointData()->AddArray(offsetAbscissasArray);
  output->GetPointData()->AddArray(offsetNormalsArray);

  offsetAbscissasArray->Delete();
  offsetNormalsArray->Delete();
  tube->Delete();
  tubeInputCellIds->Delete();

  return 1;
}

void vtkvmtkCenterlineReferenceSystemAttributesOffset::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
