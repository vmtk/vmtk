/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkSteepestDescentLineTracer.cxx,v $
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

#include "vtkvmtkSteepestDescentLineTracer.h"
#include "vtkFloatArray.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkCellArray.h"
#include "vtkLine.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkvmtkConstants.h"

vtkCxxRevisionMacro(vtkvmtkSteepestDescentLineTracer, "$Revision: 1.4 $");
vtkStandardNewMacro(vtkvmtkSteepestDescentLineTracer);

vtkvmtkSteepestDescentLineTracer::vtkvmtkSteepestDescentLineTracer()
{
  this->Seeds = NULL;
  this->Targets = NULL;
  this->StopOnTargets = 0;
  this->DataArrayName = NULL;
  this->EdgeArrayName = NULL;
  this->EdgePCoordArrayName = NULL;

  this->MergePaths = 0;
  this->MergeTolerance = VTK_VMTK_DOUBLE_TOL;

  this->LineDataArray = NULL;

  this->HitTargets = vtkIdList::New();
}

vtkvmtkSteepestDescentLineTracer::~vtkvmtkSteepestDescentLineTracer()
{
  this->HitTargets->Delete();

  if (this->Seeds)
    {
    this->Seeds->Delete();
    this->Seeds = NULL;
    }

  if (this->Targets)
    {
    this->Targets->Delete();
    this->Targets = NULL;
    }

  if (this->EdgePCoordArrayName)
    {
    delete [] this->EdgePCoordArrayName;
    this->EdgePCoordArrayName = NULL;
    }

  if (this->EdgeArrayName)
    {
    delete [] this->EdgeArrayName;
    this->EdgeArrayName = NULL;
    }

  if (this->DescentArrayName)
    {
    delete[] this->DescentArrayName;
    this->DescentArrayName = NULL;
    }

  if (this->DataArrayName)
    {
    delete[] this->DataArrayName;
    this->DataArrayName = NULL;
    }

}

void vtkvmtkSteepestDescentLineTracer::Backtrace(vtkPolyData* input, vtkIdType seedId)
{
  bool done;
  double startingPoint[3], endingPoint[3], currentPoint[3], currentScalar;
  double currentS;
  double currentRadius;
  vtkIdType currentEdge[2], steepestDescentEdge[2], previousEdge[2], previousEdge2[2];
  double steepestDescentS, steepestDescent;
  double previousS, previousS2;
  double previousPoint[3];
  double directionFactor;
  vtkIdList* neighborCells;
  vtkIdType pointId, j, targetId;
  vtkIdList* lineIds;

  vtkPoints* newPoints;
  vtkDataArray* newScalars;
  vtkCellArray* newLines;

  newPoints = this->GetOutput()->GetPoints();
  newScalars = this->GetOutput()->GetPointData()->GetArray(this->DataArrayName);
  newLines = this->GetOutput()->GetLines();

  neighborCells = vtkIdList::New();
  lineIds = vtkIdList::New();

  directionFactor = 0.0;
  if (this->Direction==VTK_VMTK_DOWNWARD)
    {
    directionFactor = 1.0;
    }
  else if (this->Direction==VTK_VMTK_UPWARD)
    {
    directionFactor = - 1.0;
    }

  done = false;

  input->GetPoint(seedId,startingPoint);

  currentPoint[0] = startingPoint[0];
  currentPoint[1] = startingPoint[1];
  currentPoint[2] = startingPoint[2];

  pointId = newPoints->InsertNextPoint(startingPoint);          

  currentScalar = this->DescentArray->GetTuple1(seedId);
  currentRadius = this->LineDataArray->GetTuple1(seedId);
  lineIds->InsertNextId(pointId);       

  currentEdge[0] = seedId;
  currentEdge[1] = seedId;
  currentS = 0.0;

  previousS = 0.0;
  previousS2 = 0.0;

  newScalars->InsertComponent(pointId,0,currentRadius);
  this->Edges->InsertComponent(pointId,0,currentEdge[0]);
  this->Edges->InsertComponent(pointId,1,currentEdge[1]);
  this->EdgeParCoords->InsertValue(pointId,currentS);

  std::size_t numIterations = 0;

  previousEdge[0] = currentEdge[0];
  previousEdge[1] = currentEdge[1];

  previousEdge2[0] = -1;
  previousEdge2[1] = -1;

  while (!done)
    {
    if ((this->StopOnTargets) && ((this->Targets->IsId(currentEdge[0]) != -1) || (this->Targets->IsId(currentEdge[1]) != -1)))
      {
      if (this->Targets->IsId(currentEdge[0]) != -1)
        {
        targetId = currentEdge[0];
        }
      else
        {
        targetId = currentEdge[1];
        }

      input->GetPoint(targetId,endingPoint);
      if (vtkMath::Distance2BetweenPoints(currentPoint,endingPoint) > VTK_VMTK_DOUBLE_TOL)
        {
        pointId = newPoints->InsertNextPoint(endingPoint);
        currentScalar = this->DescentArray->GetTuple1(targetId);
        currentRadius = this->LineDataArray->GetTuple1(targetId);
        lineIds->InsertNextId(pointId);
      
        currentEdge[0] = targetId;
        currentEdge[1] = targetId;
        currentS = 0.0;
      
        newScalars->InsertTuple1(pointId,currentRadius);
        this->Edges->InsertComponent(pointId,0,currentEdge[0]);
        this->Edges->InsertComponent(pointId,1,currentEdge[1]);
        this->EdgeParCoords->InsertValue(pointId,currentS);
        }

      this->HitTargets->InsertNextId(targetId);

      done = true;
      break;
      }
    if (done)
      {
      break;
      }

    input->GetCellEdgeNeighbors(-1,currentEdge[0],currentEdge[1],neighborCells);

    steepestDescent = this->GetSteepestDescent(input,currentEdge,currentS,steepestDescentEdge,steepestDescentS);

    if (steepestDescentEdge[0] == -1 || steepestDescentEdge[1] == -1)
      {
      vtkWarningMacro(<<"Can't find a steepest descent edge. Target not reached.");
      done = true;
      break;
      }

    if (directionFactor*steepestDescent <  VTK_VMTK_DOUBLE_TOL)
      {
      if (!this->StopOnTargets)
      //if (!this->StopOnTargets || (previousEdge[0] == currentEdge[0] && previousEdge[1] == currentEdge[1]))
        {
        vtkWarningMacro(<<"Target not reached.");
        done = true; // these two lines were outside the if (!this->StopOnTarget), but that may lead to unnecessary failure.
        break;       // Need of better detection of stall.
        }
      }

    currentEdge[0] = steepestDescentEdge[0];
    currentEdge[1] = steepestDescentEdge[1];
    currentS = steepestDescentS;

    if (numIterations > 0 &&
			(
				(currentEdge[0] == previousEdge2[0] && currentEdge[1] == previousEdge2[1] && fabs(previousS2 - currentS) < VTK_VMTK_DOUBLE_TOL) ||
				(currentEdge[0] == previousEdge2[1] && currentEdge[1] == previousEdge2[0]) && fabs(1.0 - previousS2 - currentS) < VTK_VMTK_DOUBLE_TOL
			)
		)
      {
      vtkWarningMacro(<<"Degenerate descent detected. Target not reached.");
      done = true;
      break;
      }

    previousPoint[0] = currentPoint[0];
    previousPoint[1] = currentPoint[1]; 
    previousPoint[2] = currentPoint[2];
                
    currentPoint[0] = input->GetPoint(currentEdge[0])[0] * (1.0 - currentS) + input->GetPoint(currentEdge[1])[0] * currentS;
    currentPoint[1] = input->GetPoint(currentEdge[0])[1] * (1.0 - currentS) + input->GetPoint(currentEdge[1])[1] * currentS;
    currentPoint[2] = input->GetPoint(currentEdge[0])[2] * (1.0 - currentS) + input->GetPoint(currentEdge[1])[2] * currentS;
                
    currentScalar = this->DescentArray->GetTuple1(currentEdge[0]) * (1.0 - currentS) + this->DescentArray->GetTuple1(currentEdge[1]) * currentS;
    currentRadius = this->LineDataArray->GetTuple1(currentEdge[0]) * (1.0 - currentS) + this->LineDataArray->GetTuple1(currentEdge[1]) * currentS;

    if (this->MergePaths)
      {
      for (j=newPoints->GetNumberOfPoints()-1; j>=0; j--)
        {
        if (((this->Edges->GetComponent(j,0)==currentEdge[0])&&(this->Edges->GetComponent(j,1)==currentEdge[1]))||
            ((this->Edges->GetComponent(j,0)==currentEdge[1])&&(this->Edges->GetComponent(j,1)==currentEdge[0])))
          {
          double newPoint[3];
          newPoints->GetPoint(j,newPoint);
          if (sqrt(vtkMath::Distance2BetweenPoints(currentPoint,newPoint)) <= this->MergeTolerance)
            {
            pointId = j;
            lineIds->InsertNextId(pointId);
            done = true;
            break;
            }
          }
        }
      if (done)
        break;
      }
    
    pointId = newPoints->InsertNextPoint(currentPoint);
    lineIds->InsertNextId(pointId);
    
    newScalars->InsertTuple1(pointId,currentRadius);
    this->Edges->InsertComponent(pointId,0,currentEdge[0]);
    this->Edges->InsertComponent(pointId,1,currentEdge[1]);
    this->EdgeParCoords->InsertValue(pointId,currentS);

    previousEdge2[0] = previousEdge[0];
    previousEdge2[1] = previousEdge[1];
    previousS2 = previousS;

    previousEdge[0] = currentEdge[0];
    previousEdge[1] = currentEdge[1];
    previousS = currentS;

	++numIterations;
    }

  newLines->InsertNextCell(lineIds);

  neighborCells->Delete();
  lineIds->Delete();
}

int vtkvmtkSteepestDescentLineTracer::RequestData(
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

  vtkIdType i;
  vtkPoints* newPoints;
  vtkCellArray* newLines;
  vtkDoubleArray* newScalars;

  if (!this->DescentArrayName)
    {
    vtkErrorMacro("Descent array name not specified.");
    return 1;
    }

  if (!input->GetPointData()->GetArray(this->DescentArrayName))
    {
    vtkErrorMacro(<< "Descent array with name specified does not exist!");
    return 1;
    }

  if (!this->Seeds)
    {
    vtkErrorMacro("No Seeds set");
    return 1;
    }

  if (!this->Seeds->GetNumberOfIds())
    {
    vtkErrorMacro("Empty Seeds list");
    return 1;
    }

  for (i=0; i<this->Seeds->GetNumberOfIds(); i++)
    {
    if ((this->Seeds->GetId(i)<0) || (this->Seeds->GetId(i)>input->GetNumberOfPoints()))
      {
      vtkErrorMacro("Seed id invalid or exceeds input number of points.");
      return 1;
      }
    }

  if (this->StopOnTargets)
    {
    for (i=0; i<this->Targets->GetNumberOfIds(); i++)
      {
      if ((this->Targets->GetId(i)<0) || (this->Targets->GetId(i) > input->GetNumberOfPoints()))
        {
        vtkErrorMacro("Invalid target id.");
        return 1;
        }
      }
    }

  this->DescentArray = input->GetPointData()->GetArray(this->DescentArrayName);

  if (this->DataArrayName)
    {
    this->LineDataArray = input->GetPointData()->GetArray(this->DataArrayName);
    if (!this->LineDataArray)
      {
      vtkErrorMacro(<< "Line data array with name specified does not exist!");
      return 1;
      }
    }
  else
    {
    this->SetDataArrayName("Data Array");
    this->LineDataArray = this->DescentArray;
    }

  this->Edges = vtkIntArray::New();
  this->EdgeParCoords = vtkDoubleArray::New();
  this->CellIdsArray = vtkIntArray::New();
  this->PCoordsArray = vtkDoubleArray::New();

  this->Edges->SetNumberOfComponents(2);
  this->CellIdsArray->SetNumberOfComponents(2);

  if (this->EdgeArrayName)
    {
    this->Edges->SetName(this->EdgeArrayName);
    }
  else
    {
    this->Edges->SetName("Edges");
    }

  if (this->EdgePCoordArrayName)
    {
    this->EdgeParCoords->SetName(this->EdgePCoordArrayName);
    }
  else
    {
    this->EdgeParCoords->SetName("EdgePCoords");
    }

  newPoints = vtkPoints::New();
  newLines = vtkCellArray::New();
  newScalars = vtkDoubleArray::New();
  newScalars->SetName(this->DataArrayName);

  if (this->MergeTolerance < VTK_VMTK_DOUBLE_TOL)
    {
    this->MergeTolerance = VTK_VMTK_DOUBLE_TOL;
    }

  output->SetPoints(newPoints);
  output->SetLines(newLines);
  output->GetPointData()->AddArray(newScalars);

  newPoints->Delete();
  newLines->Delete();
  newScalars->Delete();

  input->BuildCells();
  input->BuildLinks();

  this->HitTargets->Initialize();

  for (i=0; i<this->Seeds->GetNumberOfIds(); i++)
    {
    this->Backtrace(input,this->Seeds->GetId(i));
    }

  output->GetPointData()->AddArray(this->Edges);
  output->GetPointData()->AddArray(this->EdgeParCoords);

  this->Edges->Delete();
  this->EdgeParCoords->Delete();
  this->CellIdsArray->Delete();
  this->PCoordsArray->Delete();

  return 1;
}

void vtkvmtkSteepestDescentLineTracer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
