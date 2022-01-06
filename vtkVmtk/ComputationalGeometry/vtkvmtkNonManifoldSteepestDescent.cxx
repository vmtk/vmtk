/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkNonManifoldSteepestDescent.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkNonManifoldSteepestDescent.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkIdList.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkvmtkConstants.h"


vtkStandardNewMacro(vtkvmtkNonManifoldSteepestDescent);

vtkvmtkNonManifoldSteepestDescent::vtkvmtkNonManifoldSteepestDescent()
{
  this->DescentArrayName = NULL;
  this->DescentArray = NULL;
  this->NumberOfEdgeSubdivisions = 250;
  this->Direction = VTK_VMTK_DOWNWARD;
}

vtkvmtkNonManifoldSteepestDescent::~vtkvmtkNonManifoldSteepestDescent()
{
}

double vtkvmtkNonManifoldSteepestDescent::GetSteepestDescentInCell(vtkPolyData* input, vtkIdType cellId, vtkIdType* edge, double s, vtkIdType* steepestDescentEdge, double &steepestDescentS, double &steepestDescentLength)
{
  double currentPoint[3], currentScalar;
  double descent, steepestDescent;
  double descentLength;
  double point0[3], point1[3], point[3];
  double scalar0, scalar1, scalar;
  double currentS;
  double directionFactor = 0.0;
  vtkIdType npts;
  const vtkIdType *pts;
  vtkIdType i, j;

  if (!this->DescentArray)
    {
    vtkErrorMacro("DescentArray not set.");
    }

  if (this->Direction==VTK_VMTK_DOWNWARD)
    {
    directionFactor = 1.0;
    }
  else if (this->Direction==VTK_VMTK_UPWARD)
    {
    directionFactor = - 1.0;
    }

  double edgePoint0[3], edgePoint1[3];
  input->GetPoint(edge[0],edgePoint0);
  input->GetPoint(edge[1],edgePoint1);

  currentPoint[0] = edgePoint0[0] * (1.0 - s) +  edgePoint1[0] * s;
  currentPoint[1] = edgePoint0[1] * (1.0 - s) +  edgePoint1[1] * s;
  currentPoint[2] = edgePoint0[2] * (1.0 - s) +  edgePoint1[2] * s;
  currentScalar = this->DescentArray->GetTuple1(edge[0]) * (1.0 - s) +  this->DescentArray->GetTuple1(edge[1]) * s;

  steepestDescent = - VTK_VMTK_LARGE_DOUBLE * directionFactor;
  steepestDescentLength = VTK_VMTK_LARGE_DOUBLE;

  input->GetCellPoints(cellId,npts,pts);
        
  for (i=0; i<npts; i++)
    {
    input->GetPoint(pts[i],point0);
    input->GetPoint(pts[(i+1)%npts],point1);
    scalar0 = this->DescentArray->GetTuple1(pts[i]);
    scalar1 = this->DescentArray->GetTuple1(pts[(i+1)%npts]);

    if (edge[0]==edge[1])
      {
      if (pts[i]==edge[0])
        {
        continue;
        }
      }
    else
      {
      if (((pts[i]==edge[0]) && (pts[(i+1)%npts]==edge[1]))||((pts[i]==edge[1]) && (pts[(i+1)%npts]==edge[0])))
        {
        point[0] = point0[0];
        point[1] = point0[1];
        point[2] = point0[2];
        scalar = scalar0;
        currentS = 0.0;
        descentLength = sqrt(vtkMath::Distance2BetweenPoints(point,currentPoint));
        if (descentLength > VTK_VMTK_DOUBLE_TOL)
          {
          descent = - (scalar - currentScalar) / descentLength;
          }
        else 
          {
          descent = 0.0;
          }
        if (directionFactor*(descent - steepestDescent) > VTK_VMTK_DOUBLE_TOL)
          {
          steepestDescent = descent;
          steepestDescentLength = descentLength;
          steepestDescentEdge[0] = pts[i];
          steepestDescentEdge[1] = pts[i];
          steepestDescentS = currentS;
          }
        continue;
        }
      }
                
    currentS = 0.0;
    for (j=0; j<this->NumberOfEdgeSubdivisions; j++)
      {
      point[0] = point0[0] * (1.0 - currentS) + point1[0] * currentS;
      point[1] = point0[1] * (1.0 - currentS) + point1[1] * currentS;
      point[2] = point0[2] * (1.0 - currentS) + point1[2] * currentS;
      scalar = scalar0 * (1.0 - currentS) + scalar1 * currentS;
      descentLength = sqrt(vtkMath::Distance2BetweenPoints(point,currentPoint));
      if (descentLength > VTK_VMTK_DOUBLE_TOL)
        {
        descent = - (scalar - currentScalar) / descentLength;
        }
      else
        {
        descent = 0.0;
        }
      if (directionFactor * (descent - steepestDescent) > VTK_VMTK_DOUBLE_TOL)
        {
        steepestDescent = descent;
        steepestDescentLength = descentLength;
        if (fabs(currentS)<VTK_VMTK_DOUBLE_TOL)
          {
          steepestDescentEdge[0] = pts[i];
          steepestDescentEdge[1] = pts[i];
          }
        else
          {
          steepestDescentEdge[0] = pts[i];
          steepestDescentEdge[1] = pts[(i+1)%npts];                             
          }
        steepestDescentS = currentS;
        }
                        
      currentS += 1.0 / (double)this->NumberOfEdgeSubdivisions;
      }
    }

  return steepestDescent;
}

double vtkvmtkNonManifoldSteepestDescent::GetSteepestDescent(vtkPolyData* input, vtkIdType* edge, double s, vtkIdType* steepestDescentEdge, double &steepestDescentS)
{
  double descent, steepestDescent;
  double descentLength, steepestDescentLength;
  vtkIdType descentEdge[2];
  double descentS;
  vtkIdList *neighborCells;
  vtkIdType i, cellId;
  double directionFactor = 0.0;

  neighborCells = vtkIdList::New();
  
  input->GetCellEdgeNeighbors(-1,edge[0],edge[1],neighborCells);

  if (this->Direction==VTK_VMTK_DOWNWARD)
    {
    directionFactor = 1.0;
    }
  else if (this->Direction==VTK_VMTK_UPWARD)
    {
    directionFactor = - 1.0;
    }

  steepestDescent = - VTK_VMTK_LARGE_DOUBLE * directionFactor;
  steepestDescentLength = VTK_VMTK_LARGE_DOUBLE;
  steepestDescentEdge[0] = -1;
  steepestDescentEdge[1] = -1;
  for (i=0; i<neighborCells->GetNumberOfIds(); i++)
    {
    cellId = neighborCells->GetId(i);
    descent = this->GetSteepestDescentInCell(input,cellId,edge,s,descentEdge,descentS,descentLength);
    if (directionFactor*(descent - steepestDescent) > VTK_VMTK_DOUBLE_TOL)
      {
      steepestDescent = descent;
      steepestDescentLength = descentLength;
      steepestDescentEdge[0] = descentEdge[0];
      steepestDescentEdge[1] = descentEdge[1];
      steepestDescentS = descentS;
      }
    }

  neighborCells->Delete();

  return steepestDescent;
}

int vtkvmtkNonManifoldSteepestDescent::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
//  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
//  vtkPolyData *output = vtkPolyData::SafeDownCast(
//    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (!this->DescentArrayName)
    {
    vtkErrorMacro("No descent array name specified!");
    return 1;
    }

  if (!input->GetPointData()->GetArray(this->DescentArrayName))
    {
    vtkErrorMacro("Descent array with specified name does not exist!");
    return 1;
    }

  this->DescentArray = input->GetPointData()->GetArray(this->DescentArrayName);

  return 1;
}

void vtkvmtkNonManifoldSteepestDescent::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
