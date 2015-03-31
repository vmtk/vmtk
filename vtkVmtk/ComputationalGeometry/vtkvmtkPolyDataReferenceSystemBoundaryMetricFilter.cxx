/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
  Version:   $Revision: 1.7 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkvmtkPolyBallLine.h"
#include "vtkPolyLine.h"
#include "vtkCellArray.h"
#include "vtkObjectFactory.h"
#include "vtkvmtkPolyDataBoundaryExtractor.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkVersion.h"

#include "vtkvmtkPolyBallLine.h"

#include "vtkvmtkPolyDataBranchUtilities.h"
#include "vtkvmtkCenterlineUtilities.h"
#include "vtkvmtkReferenceSystemUtilities.h"


vtkStandardNewMacro(vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter);

vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter::vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter() 
{
  this->BoundaryMetricArrayName = NULL;

  this->GroupIdsArrayName = NULL;

  this->Centerlines = NULL;
  this->CenterlineAbscissasArrayName = NULL;
  this->CenterlineRadiusArrayName = NULL;
  this->CenterlineGroupIdsArrayName = NULL;
  this->CenterlineIdsArrayName = NULL;
  this->CenterlineTractIdsArrayName = NULL;

  this->ReferenceSystems = NULL;
  this->ReferenceSystemGroupIdsArrayName = NULL;
}

vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter::~vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter()
{
  if (this->BoundaryMetricArrayName)
    {
    delete[] this->BoundaryMetricArrayName;
    this->BoundaryMetricArrayName = NULL;
    }

  if (this->GroupIdsArrayName)
    {
    delete[] this->GroupIdsArrayName;
    this->GroupIdsArrayName = NULL;
    }

  if (this->Centerlines)
    {
    this->Centerlines->Delete();
    this->Centerlines = NULL;
    }

  if (this->CenterlineAbscissasArrayName)
    {
    delete[] this->CenterlineAbscissasArrayName;
    this->CenterlineAbscissasArrayName = NULL;
    }

  if (this->CenterlineRadiusArrayName)
    {
    delete[] this->CenterlineRadiusArrayName;
    this->CenterlineRadiusArrayName = NULL;
    }

  if (this->CenterlineGroupIdsArrayName)
    {
    delete[] this->CenterlineGroupIdsArrayName;
    this->CenterlineGroupIdsArrayName = NULL;
    }

  if (this->CenterlineIdsArrayName)
    {
    delete[] this->CenterlineIdsArrayName;
    this->CenterlineIdsArrayName = NULL;
    }

  if (this->CenterlineTractIdsArrayName)
    {
    delete[] this->CenterlineTractIdsArrayName;
    this->CenterlineTractIdsArrayName = NULL;
    }

  if (this->ReferenceSystems)
    {
    this->ReferenceSystems->Delete();
    this->ReferenceSystems = NULL;
    }

  if (this->ReferenceSystemGroupIdsArrayName)
    {
    delete[] this->ReferenceSystemGroupIdsArrayName;
    this->ReferenceSystemGroupIdsArrayName = NULL;
    }
}

int vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter::RequestData(
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

  if (!this->BoundaryMetricArrayName)
    {
    vtkErrorMacro(<<"BoundaryMetricArrayName not set.");
    return 1;
    }

  if (!this->GroupIdsArrayName)
    {
    vtkErrorMacro(<<"GroupIdsArrayName not set.");
    return 1;
    }

  vtkDataArray* groupIdsArray = input->GetPointData()->GetArray(this->GroupIdsArrayName);

  if (!groupIdsArray)
    {
    vtkErrorMacro(<<"GroupIdsArray with name specified does not exist.");
    return 1;
    }

  if (!this->Centerlines)
    {
    vtkErrorMacro(<<"Centerlines not set");
    return 1;
    }

  if (!this->CenterlineAbscissasArrayName)
    {
    vtkErrorMacro(<<"CenterlineAbscissasArrayName not set.");
    return 1;
    }

  vtkDataArray* centerlineAbscissasArray = this->Centerlines->GetPointData()->GetArray(this->CenterlineAbscissasArrayName);

  if (!centerlineAbscissasArray)
    {
    vtkErrorMacro(<<"CenterlineAbscissasArray with name specified does not exist.");
    return 1;
    }

  if (!this->CenterlineRadiusArrayName)
    {
    vtkErrorMacro(<<"CenterlineRadiusArrayName not set.");
    return 1;
    }

  vtkDataArray* centerlineRadiusArray = this->Centerlines->GetPointData()->GetArray(this->CenterlineRadiusArrayName);

  if (!centerlineRadiusArray)
    {
    vtkErrorMacro(<<"CenterlineRadiusArray with name specified does not exist.");
    return 1;
    }

  if (!this->CenterlineGroupIdsArrayName)
    {
    vtkErrorMacro(<<"CenterlineGroupIdsArrayName not set.");
    return 1;
    }

  vtkDataArray* centerlineGroupIdsArray = this->Centerlines->GetCellData()->GetArray(this->CenterlineGroupIdsArrayName);

  if (!centerlineGroupIdsArray)
    {
    vtkErrorMacro(<<"CenterlineGroupIdsArrayName with name specified does not exist.");
    return 1;
    }

  if (!this->CenterlineIdsArrayName)
    {
    vtkErrorMacro(<<"CenterlineIdsArrayName not set.");
    return 1;
    }

  vtkDataArray* centerlineIdsArray = this->Centerlines->GetCellData()->GetArray(this->CenterlineIdsArrayName);

  if (!centerlineIdsArray)
    {
    vtkErrorMacro(<<"CenterlineIdsArrayName with name specified does not exist.");
    return 1;
    }

  if (!this->CenterlineTractIdsArrayName)
    {
    vtkErrorMacro(<<"CenterlineTractIdsArrayName not set.");
    return 1;
    }

  vtkDataArray* centerlineTractIdsArray = this->Centerlines->GetCellData()->GetArray(this->CenterlineTractIdsArrayName);

  if (!centerlineTractIdsArray)
    {
    vtkErrorMacro(<<"CenterlineTractIdsArrayName with name specified does not exist.");
    return 1;
    }

  if (!this->ReferenceSystems)
    {
    vtkErrorMacro(<<"ReferenceSystems not set");
    return 1;
    }

  if (!this->ReferenceSystemGroupIdsArrayName)
    {
    vtkErrorMacro(<<"ReferenceSystemGroupIdsArrayName not set.");
    return 1;
    }

  vtkDataArray* referenceSystemGroupIdsArray = this->ReferenceSystems->GetPointData()->GetArray(this->ReferenceSystemGroupIdsArrayName);

  if (!referenceSystemGroupIdsArray)
    {
    vtkErrorMacro(<<"ReferenceSystemGroupIdsArrayName with name specified does not exist.");
    return 1;
    }

  int numberOfInputPoints = input->GetNumberOfPoints();

  output->DeepCopy(input);

  vtkDoubleArray* boundaryMetricArray = vtkDoubleArray::New();
  boundaryMetricArray->SetName(this->BoundaryMetricArrayName);
  boundaryMetricArray->SetNumberOfComponents(1);
  boundaryMetricArray->SetNumberOfTuples(numberOfInputPoints);
  boundaryMetricArray->FillComponent(0,0.0);
  
  output->GetPointData()->AddArray(boundaryMetricArray);

  // for each group, find boundaries, find centerline group, see if it's adjacent to a bifurcation, get correspondent reference system origin, evaluate mean abscissa and put value on right boundary; if not adjacent to bifucation, just put extremal centerline abscissa.

  vtkIdList* groupIds = vtkIdList::New();
  vtkvmtkPolyDataBranchUtilities::GetGroupsIdList(input,this->GroupIdsArrayName,groupIds);
  int i;
  for (i=0; i<groupIds->GetNumberOfIds(); i++)
    {
    vtkIdType groupId = groupIds->GetId(i);
    vtkPolyData* cylinder = vtkPolyData::New();
    vtkvmtkPolyDataBranchUtilities::ExtractGroup(input,this->GroupIdsArrayName,groupId,false,cylinder);
            
    vtkvmtkPolyDataBoundaryExtractor* boundaryExtractor = vtkvmtkPolyDataBoundaryExtractor::New();   
#if (VTK_MAJOR_VERSION <= 5)
    boundaryExtractor->SetInput(cylinder);
#else
    boundaryExtractor->SetInputData(cylinder);
#endif
    boundaryExtractor->Update();

    int numberOfBoundaries = boundaryExtractor->GetOutput()->GetNumberOfCells();

    if (numberOfBoundaries != 2)
      {
      vtkErrorMacro(<<"Branch is not topologically a cylinder");
      return 1;
      }

    vtkIdList* centerlineGroupCellIds = vtkIdList::New();
    vtkvmtkCenterlineUtilities::GetGroupCellIds(this->Centerlines,this->CenterlineGroupIdsArrayName,groupId,centerlineGroupCellIds);

    vtkvmtkPolyBallLine* tube = vtkvmtkPolyBallLine::New();
    tube->SetInput(this->Centerlines);
    tube->SetInputCellIds(centerlineGroupCellIds);
    tube->UseRadiusInformationOn();
    tube->SetPolyBallRadiusArrayName(this->CenterlineRadiusArrayName);

    double boundaryAbscissa[2];

    int j;
    for (j=0; j<numberOfBoundaries; j++)
      {
      vtkPolyLine* boundary = vtkPolyLine::SafeDownCast(boundaryExtractor->GetOutput()->GetCell(j));

      if (boundary->GetNumberOfPoints() == 0)
        {
        vtkErrorMacro(<<"Zero-length boundary found");
        return 1;
        }

      tube->EvaluateFunction(boundary->GetPoints()->GetPoint(0));
      
      vtkIdType centerlineCellId = tube->GetLastPolyBallCellId();
      vtkIdType centerlineSubId = tube->GetLastPolyBallCellSubId();
      double centerlinePCoord = tube->GetLastPolyBallCellPCoord();
      double abscissa = 0.0;
      vtkvmtkCenterlineUtilities::InterpolateTuple1(this->Centerlines,this->CenterlineAbscissasArrayName,centerlineCellId,centerlineSubId,centerlinePCoord,abscissa);

      boundaryAbscissa[j] = abscissa;
      }    

    int boundaryOrderedIds[2];
    boundaryOrderedIds[0] = 0;
    boundaryOrderedIds[1] = 1;
    if (boundaryAbscissa[0] > boundaryAbscissa[1])
      {
      boundaryOrderedIds[0] = 1;
      boundaryOrderedIds[1] = 0;
      }

  // see if centerline group is adjacent to a bifurcation, get correspondent reference system origin, evaluate mean abscissa and put value on boundary; if not adjacent to bifucation, just put extremal centerline abscissa.

    vtkIdList* upStreamGroupIds = vtkIdList::New();
    vtkIdList* downStreamGroupIds = vtkIdList::New();
    
    vtkvmtkCenterlineUtilities::FindAdjacentCenterlineGroupIds(this->Centerlines,this->CenterlineGroupIdsArrayName,this->CenterlineIdsArrayName,this->CenterlineTractIdsArrayName,groupId,upStreamGroupIds,downStreamGroupIds);
    
    if ((upStreamGroupIds->GetNumberOfIds() > 1) || (downStreamGroupIds->GetNumberOfIds() > 1))
      {
      vtkErrorMacro(<<"More than one adjacent group found for current group. Not topologically a cylinder.");
      return 1;
      }

    vtkIdType referenceSystemGroupIds[2];
    referenceSystemGroupIds[0] = -1;
    referenceSystemGroupIds[1] = -1;
    vtkIdType referenceSystemPointIds[2];
    referenceSystemPointIds[0] = -1;
    referenceSystemPointIds[1] = -1;

    if (upStreamGroupIds->GetNumberOfIds() > 0)
      {
      referenceSystemGroupIds[0] = upStreamGroupIds->GetId(0);
      }

    if (downStreamGroupIds->GetNumberOfIds() > 0)
      {
      referenceSystemGroupIds[1] = downStreamGroupIds->GetId(0);
      }

    int n;
    for (n=0; n<2; n++)
      {
      if (referenceSystemGroupIds[n] == -1)
        {
        continue;
        }
     referenceSystemPointIds[n] = vtkvmtkReferenceSystemUtilities::GetReferenceSystemPointId(this->ReferenceSystems,this->ReferenceSystemGroupIdsArrayName,referenceSystemGroupIds[n]);
      if (referenceSystemPointIds[n] == -1)
        {
        vtkErrorMacro(<<"Centerline group ids and reference system group ids are inconsistent.");
        return 1;
        }
      }

    // now, we have the origins and the corresponding boundaries. Compute average origin abscissa and put it in on the right boundary.
    // if no boundary, put minimal abscissa in group

    double referenceSystemAbscissas[2];
    referenceSystemAbscissas[0] = 0.0;
    referenceSystemAbscissas[1] = 0.0;

    for (n=0; n<2; n++)
      {
      if (referenceSystemGroupIds[n] != -1)
        {
//         Compute average origin abscissa and store it in referenceSystemAbscissas[n]. TODO: this one could be stored in ReferenceSystems as Abscissas array

        tube->UseRadiusInformationOff();

        double weightSum = 0.0;

        vtkIdList* centerlineGroupAllCellIds = vtkIdList::New();
        vtkvmtkCenterlineUtilities::GetGroupUniqueCellIds(this->Centerlines,this->CenterlineGroupIdsArrayName,referenceSystemGroupIds[n],centerlineGroupAllCellIds);
        for (j=0; j<centerlineGroupAllCellIds->GetNumberOfIds(); j++)
          {
          centerlineGroupCellIds->Initialize();
          centerlineGroupCellIds->InsertNextId(centerlineGroupAllCellIds->GetId(j));
          
          tube->SetInputCellIds(centerlineGroupCellIds);
          tube->EvaluateFunction(this->ReferenceSystems->GetPoint(referenceSystemPointIds[n]));
          
          vtkIdType centerlineCellId = tube->GetLastPolyBallCellId();
          vtkIdType centerlineSubId = tube->GetLastPolyBallCellSubId();
          double centerlinePCoord = tube->GetLastPolyBallCellPCoord();

          double radius = 0.0;
          vtkvmtkCenterlineUtilities::InterpolateTuple1(this->Centerlines,this->CenterlineRadiusArrayName,centerlineCellId,centerlineSubId,centerlinePCoord,radius);
          double weight = radius * radius;
          double abscissa = 0.0;
          vtkvmtkCenterlineUtilities::InterpolateTuple1(this->Centerlines,this->CenterlineAbscissasArrayName,centerlineCellId,centerlineSubId,centerlinePCoord,abscissa);

          referenceSystemAbscissas[n] += weight * abscissa;
          weightSum += weight;
          }
        referenceSystemAbscissas[n] /= weightSum;
        }
      else
        {
//         Compute extremal abscissa in group and store it in referenceSystemAbscissas[n]
        if (n==0)
          {
          referenceSystemAbscissas[n] = VTK_DOUBLE_MAX;

          vtkIdList* centerlineGroupCellIds = vtkIdList::New();
          vtkvmtkCenterlineUtilities::GetGroupCellIds(this->Centerlines,this->CenterlineGroupIdsArrayName,groupId,centerlineGroupCellIds);
          for (j=0; j<centerlineGroupCellIds->GetNumberOfIds(); j++)
            {
            vtkIdType centerlineCellId = centerlineGroupCellIds->GetId(j);
            double abscissa = centerlineAbscissasArray->GetComponent(this->Centerlines->GetCell(centerlineCellId)->GetPointId(0),0);
            if (abscissa < referenceSystemAbscissas[n])
              {
              referenceSystemAbscissas[n] = abscissa;
              }
            }
            centerlineGroupCellIds->Delete();
          }
        else if (n==1)
          {
          referenceSystemAbscissas[n] = VTK_DOUBLE_MIN;
 
          vtkIdList* centerlineGroupCellIds = vtkIdList::New();
          vtkvmtkCenterlineUtilities::GetGroupCellIds(this->Centerlines,this->CenterlineGroupIdsArrayName,groupId,centerlineGroupCellIds);
          for (j=0; j<centerlineGroupCellIds->GetNumberOfIds(); j++)
            {
            vtkIdType centerlineCellId = centerlineGroupCellIds->GetId(j);
        
            int numberOfCellPoints = this->Centerlines->GetCell(centerlineCellId)->GetNumberOfPoints();
            double abscissa = centerlineAbscissasArray->GetComponent(this->Centerlines->GetCell(centerlineCellId)->GetPointId(numberOfCellPoints-1),0);
            if (abscissa > referenceSystemAbscissas[n])
              {
              referenceSystemAbscissas[n] = abscissa;
              }
            }
          }
        }
      }

//     put abscissas in the right place

    for (n=0; n<2; n++)
      {
      vtkCell* boundary = boundaryExtractor->GetOutput()->GetCell(boundaryOrderedIds[n]);
      vtkDataArray* boundaryPointIds = boundaryExtractor->GetOutput()->GetPointData()->GetScalars();
      int numberOfBoundaryPoints = boundary->GetNumberOfPoints();
      for (j=0; j<numberOfBoundaryPoints; j++)
        {
        vtkIdType boundaryPointId = static_cast<int>(boundaryPointIds->GetComponent(boundary->GetPointId(j),0));
        boundaryMetricArray->SetComponent(boundaryPointId,0,referenceSystemAbscissas[n]);
        }
      }

    tube->Delete();
    centerlineGroupCellIds->Delete();
    boundaryExtractor->Delete();
    cylinder->Delete();
    upStreamGroupIds->Delete();
    downStreamGroupIds->Delete();
    }

  groupIds->Delete();
  boundaryMetricArray->Delete();

  return 1;
}

