/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkMergeCenterlines.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkMergeCenterlines.h"
#include "../Common/vtkvmtkConstants.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkPolyLine.h"
#include "vtkIdList.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkvmtkMergeCenterlines, "$Revision: 1.4 $");
vtkStandardNewMacro(vtkvmtkMergeCenterlines);

vtkvmtkMergeCenterlines::vtkvmtkMergeCenterlines()
{
  this->CenterlinesToMerge = NULL;
}

vtkvmtkMergeCenterlines::~vtkvmtkMergeCenterlines()
{
  if (this->CenterlinesToMerge)
    {
    this->CenterlinesToMerge->Delete();
    this->CenterlinesToMerge = NULL;
    }
}

int vtkvmtkMergeCenterlines::RequestData(
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

  vtkIdType j, k;
  vtkIdType jointId0, jointId1;
  vtkIdType jointPointId0, jointPointId1;
  vtkIdType jointCellId0, jointCellId1;
  vtkIdType minDistCellId, minDistId;
  jointPointId0 = jointPointId1 = -1;
  jointCellId0 = jointCellId1 = -1;
  minDistCellId = minDistId = -1;
  vtkIdType lastId;
  double point1[3], closestPoint[3];
  double dist2, minDist2;
  vtkIdType subId;
  double* weights;
  double parCoords[3];
  vtkPolyLine *polyLine1, *polyLine;
  vtkIdList* newLine;
  vtkPoints* newPoints;
  vtkCellArray* newCellArray;
  vtkPointData* newPointData = this->GetOutput()->GetPointData();
  vtkCellData* newCellData = this->GetOutput()->GetCellData();

  if (!this->CenterlinesToMerge)
    {
    vtkErrorMacro(<< "No centerline to merge specified!");
    return 1;
    }

  newLine = vtkIdList::New();

  newPoints = vtkPoints::New();
  newCellArray = vtkCellArray::New();

  newPoints->DeepCopy(input->GetPoints());
  newCellArray->DeepCopy(input->GetLines());

  input->BuildCells();
  this->CenterlinesToMerge->BuildCells();

  // TODO: GENERALIZE TO NCELLS

  polyLine1 = (vtkPolyLine*)this->CenterlinesToMerge->GetCell(0);
  
  jointId0 = -1;
  jointId1 = -1;
  for (j=0; j<polyLine1->GetNumberOfPoints(); j++)
    {
    minDist2 = VTK_VMTK_LARGE_DOUBLE;
    polyLine1->GetPoints()->GetPoint(j,point1);
    for (k=0; k<input->GetNumberOfCells(); k++)
      {
      polyLine = (vtkPolyLine*)input->GetCell(k);
      weights = new double[polyLine->GetNumberOfPoints()];
      polyLine->EvaluatePosition(point1,closestPoint,subId,parCoords,dist2,weights);
      
      if (dist2 - minDist2 < -VTK_VMTK_DOUBLE_TOL)
        {
        minDist2 = dist2;
        minDistCellId = k;
        if (parCoords[0]-0.5 < VTK_VMTK_DOUBLE_TOL)
          {
          minDistId = polyLine->GetPointId(subId);
          }
        else
          {
          minDistId = polyLine->GetPointId(subId+1);
          }
        }
      }
    if (minDist2 < VTK_VMTK_DOUBLE_TOL)
      {
      if ((jointId0==-1)||(jointId1!=-1))
       {
        jointId0 = j;
        jointPointId0 = minDistId;
        jointCellId0 = minDistCellId;
        jointId1 = -1;
        jointCellId1 = -1;
        }
      else if (jointId1==-1)
        {
        if (j==jointId0+1)
          {
          jointId0 = j;
          jointPointId0 = minDistId;
          jointCellId0 = minDistCellId;
          }
        else
          {
          jointId1 = j;
          jointPointId1 = minDistId;
          jointCellId1 = minDistCellId;
          }
        }
      }
    else
      {
      if (j==jointId1+1)
        {
        jointId0 = jointId1;
        jointPointId0 = jointPointId1;
        jointCellId0 = jointCellId1;
        jointId1 = -1;
        jointCellId1 = -1;
        }
      }

    if (((jointCellId0!=-1)&&(jointCellId1!=-1))&&(jointCellId0!=jointCellId1))
      {
      break;
      }
    }

  vtkDataArray* dataArray;
  for (k=0; k<input->GetPointData()->GetNumberOfArrays(); k++)
    {
    dataArray = input->GetPointData()->GetArray(k)->NewInstance();
    dataArray->DeepCopy(input->GetPointData()->GetArray(k));
    dataArray->SetName(input->GetPointData()->GetArray(k)->GetName());
    newPointData->AddArray(dataArray);
    dataArray->Delete();
    }
  for (k=0; k<input->GetCellData()->GetNumberOfArrays(); k++)
    {
    dataArray = input->GetCellData()->GetArray(k)->NewInstance();
    dataArray->DeepCopy(input->GetCellData()->GetArray(k));
    dataArray->SetName(input->GetCellData()->GetArray(k)->GetName());
    newPointData->AddArray(dataArray);
    dataArray->Delete();
    }

  newLine->InsertNextId(jointPointId0);
  for (j=jointId0+1; j<jointId1; j++)
    {
    polyLine1->GetPoints()->GetPoint(j,point1);
    lastId = newPoints->InsertNextPoint(point1);
    for (k=0; k<newPointData->GetNumberOfArrays(); k++)
      {
      newPointData->GetArray(k)->InsertNextTuple(this->CenterlinesToMerge->GetPointData()->GetArray(k)->GetTuple(polyLine1->GetPointId(j)));
      }
    for (k=0; k<newCellData->GetNumberOfArrays(); k++)
      {
      newCellData->GetArray(k)->InsertNextTuple(this->CenterlinesToMerge->GetCellData()->GetArray(k)->GetTuple(polyLine1->GetPointId(j)));
      }
    newLine->InsertNextId(lastId);
    }
  newLine->InsertNextId(jointPointId1);

  newCellArray->InsertNextCell(newLine);

  newLine->Delete();

  output->SetPoints(newPoints);
  output->SetLines(newCellArray);

  newPoints->Delete();
  newCellArray->Delete();
  
  return 1;
}

void vtkvmtkMergeCenterlines::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
