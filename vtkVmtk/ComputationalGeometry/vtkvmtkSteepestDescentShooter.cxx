/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkSteepestDescentShooter.cxx,v $
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

#include "vtkvmtkSteepestDescentShooter.h"
#include "vtkPolyLine.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkLine.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkvmtkConstants.h"


vtkStandardNewMacro(vtkvmtkSteepestDescentShooter);

vtkvmtkSteepestDescentShooter::vtkvmtkSteepestDescentShooter()
{
  this->Seeds = NULL;
  this->Target = NULL;

  this->EdgeArrayName = NULL;

  this->TargetVectorsArrayName = NULL;
  this->TargetCellIdsArrayName = NULL;
  this->TargetPCoordsArrayName = NULL;

  this->TargetVectors = vtkDoubleArray::New();
  this->TargetCellIds = vtkIntArray::New();
  this->TargetPCoords = vtkDoubleArray::New();

  this->TargetArray = vtkIntArray::New();

}

vtkvmtkSteepestDescentShooter::~vtkvmtkSteepestDescentShooter()
{
  this->TargetVectors->Delete();
  this->TargetCellIds->Delete();
  this->TargetPCoords->Delete();

  this->TargetArray->Delete();

  if (this->Seeds)
    {
    this->Seeds->Delete();
    this->Seeds = NULL;
    }

  if (this->Target)
    {
    this->Target->Delete();
    this->Target = NULL;
    }
}

void vtkvmtkSteepestDescentShooter::FindNearestPolyLinePoint(double* currentPoint, double* lineClosestPoint, int &lineCellId, int &lineCellSubId, double &linePCoord)
{
  vtkIdType i;
  int subId;
  double closestPoint[3];
  double parCoords[3];
  double dist2, minDist2;
  double* weights;
  vtkPolyLine* polyLine;
        
  minDist2 = VTK_VMTK_LARGE_DOUBLE;
  for (i=0; i<this->Target->GetNumberOfCells(); i++)
    {
    polyLine = vtkPolyLine::SafeDownCast(this->Target->GetCell(i));
    if (!polyLine)
      {
      continue;
      }
    weights = new double[polyLine->GetNumberOfPoints()];
    polyLine->EvaluatePosition(currentPoint,closestPoint,subId,parCoords,dist2,weights);

    if (dist2 - minDist2 < -VTK_VMTK_DOUBLE_TOL)
      {
      minDist2 = dist2;
      lineCellId = i;
      lineCellSubId = subId;
      linePCoord = parCoords[0];
      lineClosestPoint[0] = closestPoint[0];
      lineClosestPoint[1] = closestPoint[1];
      lineClosestPoint[2] = closestPoint[2];
      }
    
    delete weights;
    }
}

void vtkvmtkSteepestDescentShooter::Backtrace(vtkPolyData* input, vtkIdType seedId)
{
  bool done;
  double seedPoint[3];
  double currentPoint[3], currentScalar;
  double currentS;
  vtkIdType currentEdge[2], steepestDescentEdge[2];
  double steepestDescentS;
  double previousPoint[3], lineClosestPoint[3];
  double vector[3];
  int lineCellId, lineCellSubId;
  double linePCoord;

  done = false;

  input->GetPoint(seedId,seedPoint);
  currentPoint[0] = seedPoint[0];
  currentPoint[1] = seedPoint[1];
  currentPoint[2] = seedPoint[2];

  previousPoint[0] = seedPoint[0];
  previousPoint[1] = seedPoint[1];
  previousPoint[2] = seedPoint[2];
        
  currentScalar = this->DescentArray->GetTuple1(seedId);

  currentEdge[0] = seedId;
  currentEdge[1] = seedId;
  currentS = 0.0;

  while (!done)
    {
    if (((this->TargetArray->GetValue(currentEdge[0])!=-1) && (this->TargetArray->GetValue(currentEdge[1])!=-1)))
      {
      this->FindNearestPolyLinePoint(currentPoint, lineClosestPoint, lineCellId, lineCellSubId, linePCoord);
      vector[0] = lineClosestPoint[0] - seedPoint[0];
      vector[1] = lineClosestPoint[1] - seedPoint[1];
      vector[2] = lineClosestPoint[2] - seedPoint[2];
      this->TargetVectors->SetTuple(seedId,vector);
      this->TargetCellIds->SetComponent(seedId,0,lineCellId);
      this->TargetCellIds->SetComponent(seedId,1,lineCellSubId);
      this->TargetPCoords->SetTuple1(seedId,linePCoord);

      done = true;
      break;
      }

    if (currentScalar==0.0)
      {
      // THIS MEANS THERE'S SOMETHING WRONG SOMEWHERE!

      done = true;
      break;
      }

    this->GetSteepestDescent(input,currentEdge,currentS,steepestDescentEdge,steepestDescentS);

    currentEdge[0] = steepestDescentEdge[0];            
    currentEdge[1] = steepestDescentEdge[1];
    currentS = steepestDescentS;

    previousPoint[0] = currentPoint[0];
    previousPoint[1] = currentPoint[1]; 
    previousPoint[2] = currentPoint[2];
                
    currentPoint[0] = input->GetPoint(currentEdge[0])[0] * (1.0 - currentS) + input->GetPoint(currentEdge[1])[0] * currentS;
    currentPoint[1] = input->GetPoint(currentEdge[0])[1] * (1.0 - currentS) + input->GetPoint(currentEdge[1])[1] * currentS;
    currentPoint[2] = input->GetPoint(currentEdge[0])[2] * (1.0 - currentS) + input->GetPoint(currentEdge[1])[2] * currentS;
                
    currentScalar = this->DescentArray->GetTuple1(currentEdge[0]) * (1.0 - currentS) + this->DescentArray->GetTuple1(currentEdge[1]) * currentS;
    }

}

int vtkvmtkSteepestDescentShooter::RequestData(
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

  vtkIdType i, j, k, h, l, m;
  vtkIdType poleId;
  vtkIdType npts, *pts, targetNpts, *targetPts, *cells;
  unsigned short ncells;

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
    vtkErrorMacro(<< "No seeds specified!");
    return 1;
    }

  if (!this->Target)
    {
    vtkErrorMacro(<< "No target specified!");
    return 1;
    }

  if (!this->EdgeArrayName)
    {
    vtkErrorMacro(<< "No edge array name specified!");
    return 1;
    }

  if (!this->Target->GetPointData()->GetArray(this->EdgeArrayName))
    {
    vtkErrorMacro(<< "Edge array with name specified does not exist in target!");
    return 1;
    }

  if (this->TargetVectorsArrayName)
    {
    this->TargetVectors->SetName(TargetVectorsArrayName);
    }
  else
    {
    this->TargetVectors->SetName("TargetVectors");
    }

  if (this->TargetCellIdsArrayName)
    {
    this->TargetCellIds->SetName(TargetCellIdsArrayName);
    }
  else
    {
    this->TargetCellIds->SetName("TargetCellIds");
    }

  if (this->TargetPCoordsArrayName)
    {
    this->TargetPCoords->SetName(TargetPCoordsArrayName);
    }
  else
    {
    this->TargetPCoords->SetName("TargetPCoords");
    }

  this->DescentArray = input->GetPointData()->GetArray(this->DescentArrayName);
  this->EdgeArray = this->Target->GetPointData()->GetArray(this->EdgeArrayName);

  this->TargetVectors->SetNumberOfComponents(3);
  this->TargetCellIds->SetNumberOfComponents(2);

  this->TargetVectors->SetNumberOfTuples(input->GetNumberOfPoints());
  this->TargetCellIds->SetNumberOfTuples(input->GetNumberOfPoints());
  this->TargetPCoords->SetNumberOfTuples(input->GetNumberOfPoints());

  this->TargetVectors->FillComponent(0,0.0);
  this->TargetVectors->FillComponent(1,0.0);
  this->TargetVectors->FillComponent(2,0.0);
  this->TargetCellIds->FillComponent(0,0.0);
  this->TargetCellIds->FillComponent(1,0.0);
  this->TargetPCoords->FillComponent(0,0.0);

  input->BuildCells();
  input->BuildLinks();

  this->TargetArray->SetNumberOfTuples(input->GetNumberOfPoints());
  this->TargetArray->FillComponent(0,-1);

  this->Target->BuildCells();

  for (h=this->Target->GetNumberOfCells()-1; h>=0; h--)
    {
    this->Target->GetCellPoints(h,targetNpts,targetPts);
    for (l=0; l<targetNpts; l++)
      {
      for (m=0; m<2; m++)
        {
        input->GetPointCells(static_cast<vtkIdType>(this->EdgeArray->GetComponent(targetPts[l],m)),ncells,cells);
        for (j=0; j<ncells; j++)
          {
          input->GetCellPoints(cells[j],npts,pts);
          for (k=0; k<npts; k++)
            {
            this->TargetArray->SetValue(pts[k],h);
            }
          }
        }
      }
    }

  for (i=0; i<this->Seeds->GetNumberOfIds(); i++)
    {
    poleId = this->Seeds->GetId(i);
    this->Backtrace(input,poleId);
    }

  output->CopyStructure(input);
  output->GetPointData()->PassData(input->GetPointData());
  output->GetCellData()->PassData(input->GetCellData());

  output->GetPointData()->AddArray(this->TargetVectors);
  output->GetPointData()->AddArray(this->TargetCellIds);
  output->GetPointData()->AddArray(this->TargetPCoords);

  return 1;
}

void vtkvmtkSteepestDescentShooter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
