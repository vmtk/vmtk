/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineBifurcationReferenceSystems.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.8 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkCenterlineBifurcationReferenceSystems.h"
#include "vtkPolyData.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkPolyLine.h"
#include "vtkPoints.h"
#include "vtkTriangle.h"
#include "vtkvmtkConstants.h"
#include "vtkvmtkMath.h"
#include "vtkMath.h"
#include "vtkCellArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkCenterlineUtilities.h"


vtkStandardNewMacro(vtkvmtkCenterlineBifurcationReferenceSystems);

vtkvmtkCenterlineBifurcationReferenceSystems::vtkvmtkCenterlineBifurcationReferenceSystems()
{
  this->RadiusArrayName = NULL;
  this->GroupIdsArrayName = NULL;
  this->BlankingArrayName = NULL;

  this->NormalArrayName = NULL;
  this->UpNormalArrayName = NULL;
}

vtkvmtkCenterlineBifurcationReferenceSystems::~vtkvmtkCenterlineBifurcationReferenceSystems()
{
  if (this->RadiusArrayName)
    {
    delete[] this->RadiusArrayName;
    this->RadiusArrayName = NULL;
    }

  if (this->GroupIdsArrayName)
    {
    delete[] this->GroupIdsArrayName;
    this->GroupIdsArrayName = NULL;
    }

  if (this->BlankingArrayName)
    {
    delete[] this->BlankingArrayName;
    this->BlankingArrayName = NULL;
    }

  if (this->NormalArrayName)
    {
    delete[] this->NormalArrayName;
    this->NormalArrayName = NULL;
    }

  if (this->UpNormalArrayName)
    {
    delete[] this->UpNormalArrayName;
    this->UpNormalArrayName = NULL;
    }
}

int vtkvmtkCenterlineBifurcationReferenceSystems::RequestData(
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

  vtkDataArray* radiusArray;
  vtkDataArray* groupIdsArray;
  vtkDataArray* blankingArray;

  if (!this->RadiusArrayName)
    {
    vtkErrorMacro(<<"RadiusArrayName not specified");
    return 1;
    }

  radiusArray = input->GetPointData()->GetArray(this->RadiusArrayName);

  if (!radiusArray)
    {
    vtkErrorMacro(<<"RadiusArray with name specified does not exist");
    return 1;
    }

  if (!this->GroupIdsArrayName)
    {
    vtkErrorMacro(<<"GroupIdsArrayName not specified");
    return 1;
    }

  groupIdsArray = input->GetCellData()->GetArray(this->GroupIdsArrayName);

  if (!input->GetCellData()->GetArray(this->GroupIdsArrayName))
    {
    vtkErrorMacro(<<"GroupIdsArray with name specified does not exist");
    return 1;
    }

  if (!this->BlankingArrayName)
    {
    vtkErrorMacro(<<"BlankingArrayName not specified");
    return 1;
    }

  blankingArray = input->GetCellData()->GetArray(this->BlankingArrayName);

  if (!input->GetCellData()->GetArray(this->BlankingArrayName))
    {
    vtkErrorMacro(<<"BlankingArray with name specified does not exist");
    return 1;
    }

  if (!this->NormalArrayName)
    {
    vtkErrorMacro(<<"NormalArrayName not specified");
    return 1;
    }

  if (!this->UpNormalArrayName)
    {
    vtkErrorMacro(<<"UpNormalArrayName not specified");
    return 1;
    }

  vtkPoints* outputPoints = vtkPoints::New();

  vtkDoubleArray* normalArray = vtkDoubleArray::New();
  normalArray->SetName(this->NormalArrayName);
  normalArray->SetNumberOfComponents(3);

  vtkDoubleArray* upNormalArray = vtkDoubleArray::New();
  upNormalArray->SetName(this->UpNormalArrayName);
  upNormalArray->SetNumberOfComponents(3);

  vtkIntArray* referenceGroupIdsArray = vtkIntArray::New();
  referenceGroupIdsArray->SetName(this->GroupIdsArrayName);
  vtkIdList* blankedGroupIds = vtkIdList::New();
  vtkvmtkCenterlineUtilities::GetBlankedGroupsIdList(input,this->GroupIdsArrayName,this->BlankingArrayName,blankedGroupIds);
  int i;
  for (i=0; i<blankedGroupIds->GetNumberOfIds(); i++)
    {
    vtkIdType groupId = blankedGroupIds->GetId(i);
    this->ComputeGroupReferenceSystem(input,groupId,outputPoints,normalArray,upNormalArray,referenceGroupIdsArray);
    }
  blankedGroupIds->Delete();
  
  vtkCellArray* outputVertices = vtkCellArray::New();

  int numberOfOutputPoints = outputPoints->GetNumberOfPoints();
  for (i=0; i<numberOfOutputPoints; i++)
    {
    outputVertices->InsertNextCell(1);
    outputVertices->InsertCellPoint(i);
    }

  output->SetPoints(outputPoints);
  output->SetVerts(outputVertices);
  output->GetPointData()->AddArray(normalArray);
  output->GetPointData()->AddArray(upNormalArray);
  output->GetPointData()->AddArray(referenceGroupIdsArray);

  outputPoints->Delete();
  outputVertices->Delete();
  normalArray->Delete();
  upNormalArray->Delete();
  referenceGroupIdsArray->Delete();

  return 1;
}

void vtkvmtkCenterlineBifurcationReferenceSystems::ComputeGroupReferenceSystem(vtkPolyData* input, int referenceGroupId, vtkPoints* outputPoints, vtkDoubleArray* normalArray, vtkDoubleArray* upNormalArray, vtkIntArray* referenceGroupIdsArray)
{
  vtkDataArray* radiusArray;
  vtkDataArray* groupIdsArray;
  vtkDataArray* blankingArray;

  radiusArray = input->GetPointData()->GetArray(this->RadiusArrayName);
  groupIdsArray = input->GetCellData()->GetArray(this->GroupIdsArrayName);
  blankingArray = input->GetCellData()->GetArray(this->BlankingArrayName);

  vtkIdType point0Id, point1Id;

  double point0[3];
  double point1[3];

  double radius0;
  double radius1;

  vtkPoints* bifurcationPoints = vtkPoints::New();
  vtkDoubleArray* bifurcationRadii = vtkDoubleArray::New();

  vtkIdList* groupCellIds = vtkIdList::New();
//  vtkvmtkCenterlineUtilities::GetGroupCellIds(input,this->GroupIdsArrayName,referenceGroupId,groupCellIds);
  vtkvmtkCenterlineUtilities::GetGroupUniqueCellIds(input,this->GroupIdsArrayName,referenceGroupId,groupCellIds);
  int i;
  for (i=0; i<groupCellIds->GetNumberOfIds(); i++)
    {
    vtkIdType cellId = groupCellIds->GetId(i);
    vtkCell* polyLine = input->GetCell(cellId);

    if (polyLine->GetCellType() != VTK_LINE && polyLine->GetCellType() != VTK_POLY_LINE)
      {
      continue;
      }

    point0Id = polyLine->GetPointId(0);
    input->GetPoint(point0Id,point0);
    point1Id = polyLine->GetPointId(polyLine->GetNumberOfPoints()-1);
    input->GetPoint(point1Id,point1);

    bool skip = false;

    for (int j=0; j<bifurcationPoints->GetNumberOfPoints(); j+=2)
      {
      if (vtkMath::Distance2BetweenPoints(point0,bifurcationPoints->GetPoint(j)) < VTK_VMTK_DOUBLE_TOL &&
          vtkMath::Distance2BetweenPoints(point1,bifurcationPoints->GetPoint(j+1)) < VTK_VMTK_DOUBLE_TOL)
        {
        skip = true;
        break;
        }
      }

    if (skip)
      {
      continue;
      }

    radius0 = radiusArray->GetComponent(point0Id,0);
    bifurcationPoints->InsertNextPoint(point0);
    bifurcationRadii->InsertNextTuple1(radius0);

    radius1 = radiusArray->GetComponent(point1Id,0);
    bifurcationPoints->InsertNextPoint(point1);
    bifurcationRadii->InsertNextTuple1(radius1);
    }
  groupCellIds->Delete();

  int numberOfBifurcationPoints = bifurcationPoints->GetNumberOfPoints();

  double bifurcationOrigin[3];
  double bifurcationNormal[3];
  double bifurcationUpNormal[3];

  double point[3];
  double radius;
  double weight;
  double weightSum;

  bifurcationOrigin[0] = bifurcationOrigin[1] = bifurcationOrigin[2] = 0.0;
  weightSum = 0.0;

  for (i=0; i<numberOfBifurcationPoints; i++)
    {
    bifurcationPoints->GetPoint(i,point);
    radius = bifurcationRadii->GetComponent(i,0);

    weight = radius*radius;

    bifurcationOrigin[0] += weight * point[0];
    bifurcationOrigin[1] += weight * point[1];
    bifurcationOrigin[2] += weight * point[2];

    weightSum += weight;
    }

  bifurcationOrigin[0] /= weightSum;
  bifurcationOrigin[1] /= weightSum;
  bifurcationOrigin[2] /= weightSum;

  vtkDoubleArray* bifurcationNormals = vtkDoubleArray::New();
  bifurcationNormals->SetNumberOfComponents(3);

  vtkPoints* vertexPoints = vtkPoints::New();

  for (i=0; i<numberOfBifurcationPoints; i+=2)
    {
    for (int j=i+2; j<numberOfBifurcationPoints; j+=2)
      {
      if (j==i)
        {
        continue;
        }

      vertexPoints->Initialize();

      bifurcationPoints->GetPoint(i,point0);
      bifurcationPoints->GetPoint(j,point1);
      if (vtkMath::Distance2BetweenPoints(point0,point1)<VTK_VMTK_DOUBLE_TOL)
        {
        vertexPoints->InsertPoint(0,bifurcationPoints->GetPoint(i));
        vertexPoints->InsertPoint(1,bifurcationPoints->GetPoint(i+1));
        vertexPoints->InsertPoint(2,bifurcationPoints->GetPoint(j+1));
        }
      else
        {
        vertexPoints->InsertPoint(0,bifurcationPoints->GetPoint(i));
        vertexPoints->InsertPoint(1,bifurcationPoints->GetPoint(i+1));
        vertexPoints->InsertPoint(2,bifurcationPoints->GetPoint(j+1));
        vertexPoints->InsertPoint(3,bifurcationPoints->GetPoint(j));
        }

      int numberOfPolygonPoints = vertexPoints->GetNumberOfPoints();
      double vertexPoint0[3], vertexPoint1[3], vertexPoint2[3];
      double vertexNormal[3];
      double cotangent0, cotangent1;

      double polygonNormal[3];
      polygonNormal[0] = polygonNormal[1] = polygonNormal[2] = 0.0;

      for (int n=0; n<numberOfPolygonPoints; n++)
        {        
        vtkIdType vertexId0 = (n-1+numberOfPolygonPoints) % numberOfPolygonPoints;
        vtkIdType vertexId1 = n;
        vtkIdType vertexId2 = (n+1) % numberOfPolygonPoints;
        vertexPoints->GetPoint(vertexId0,vertexPoint0);
        vertexPoints->GetPoint(vertexId1,vertexPoint1);
        vertexPoints->GetPoint(vertexId2,vertexPoint2);
        
        vtkTriangle::ComputeNormal(vertexPoint0,vertexPoint1,vertexPoint2,vertexNormal);

        cotangent0 = vtkvmtkMath::Cotangent(vertexPoint0,vertexPoint1,bifurcationOrigin);
        cotangent1 = vtkvmtkMath::Cotangent(bifurcationOrigin,vertexPoint1,vertexPoint2);

        weight = (cotangent0 + cotangent1) / vtkMath::Distance2BetweenPoints(bifurcationOrigin,vertexPoint1);

        for (int k=0; k<3; k++)
          {
          polygonNormal[k] += weight * vertexNormal[k];
          }
        }

      vtkMath::Normalize(polygonNormal);

      bifurcationNormals->InsertNextTuple(polygonNormal);
      }    
    }

  if (bifurcationNormals->GetNumberOfTuples() == 0)
    {
    if (bifurcationPoints->GetNumberOfPoints() > 1)
      {
      bifurcationPoints->GetPoint(1,point1);
      bifurcationPoints->GetPoint(0,point0);
      double lineUpNormal[3];
      lineUpNormal[0] = point1[0] - point0[0];
      lineUpNormal[1] = point1[1] - point0[1];
      lineUpNormal[2] = point1[2] - point0[2];
      vtkMath::Normalize(lineUpNormal);

      double lineNormal[3], dummy[3];
      vtkMath::Perpendiculars(lineUpNormal,lineNormal,dummy,0.0);

      outputPoints->InsertNextPoint(bifurcationOrigin);
      normalArray->InsertNextTuple(lineNormal);
      upNormalArray->InsertNextTuple(lineUpNormal);
      referenceGroupIdsArray->InsertNextTuple1(referenceGroupId);
      }

    vertexPoints->Delete();
    bifurcationPoints->Delete();
    bifurcationRadii->Delete();
    bifurcationNormals->Delete();

    return;
    }

  int numberOfNormals = bifurcationNormals->GetNumberOfTuples();

  double positiveNormal[3], orientedNormal[3];
  bifurcationNormals->GetTuple(0,positiveNormal);

  for (i=1; i<numberOfNormals; i++)
    {
    bifurcationNormals->GetTuple(i,orientedNormal);

    if (vtkMath::Dot(positiveNormal,orientedNormal) < 0.0)
      {
      orientedNormal[0] *= -1.0;
      orientedNormal[1] *= -1.0;
      orientedNormal[2] *= -1.0;
      bifurcationNormals->SetTuple(i,orientedNormal);
      }
    }
 
  bifurcationNormal[0] = bifurcationNormal[1] = bifurcationNormal[2] = 0.0;

  for (i=0; i<numberOfNormals; i++)
    {
    bifurcationNormals->GetTuple(i,orientedNormal);
    for (int k=0; k<3; k++)
      {
      bifurcationNormal[k] += orientedNormal[k];
      }
    }

  vtkMath::Normalize(bifurcationNormal);

  bifurcationUpNormal[0] = bifurcationUpNormal[1] = bifurcationUpNormal[2] = 0.0;
  double upVector[3], projectedUpVector[3];

  weightSum = 0.0;
  for (i=0; i<numberOfBifurcationPoints; i+=2)
    {
    bifurcationPoints->GetPoint(i+1,point1);
    bifurcationPoints->GetPoint(i,point0);
    radius0 = bifurcationRadii->GetTuple1(i);
    radius1 = bifurcationRadii->GetTuple1(i+1);
    //weight upVectors with squared sphere radii
    //weight = radius0 * radius0 + radius1 * radius1;
    //weight upVectors with squared sphere radii of downstream branch
    weight = radius1 * radius1;
    int k;
    for (k=0; k<3; k++)
      {
      upVector[k] = point1[k] - point0[k];
      }
    double dot = vtkMath::Dot(upVector,bifurcationNormal);
    for (k=0; k<3; k++)
      {
      projectedUpVector[k] = upVector[k] - dot * bifurcationNormal[k];
      }
    vtkMath::Normalize(projectedUpVector);
    for (k=0; k<3; k++)
      {
      bifurcationUpNormal[k] += projectedUpVector[k] * weight;
      }
    weightSum += weight;
    }

  vtkMath::Normalize(bifurcationUpNormal);
  
  outputPoints->InsertNextPoint(bifurcationOrigin);
  normalArray->InsertNextTuple(bifurcationNormal);
  upNormalArray->InsertNextTuple(bifurcationUpNormal);
  referenceGroupIdsArray->InsertNextTuple1(referenceGroupId);

  vertexPoints->Delete();
  bifurcationPoints->Delete();
  bifurcationRadii->Delete();
  bifurcationNormals->Delete();
}

void vtkvmtkCenterlineBifurcationReferenceSystems::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
