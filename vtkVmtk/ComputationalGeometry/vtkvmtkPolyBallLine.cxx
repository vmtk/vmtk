/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyBallLine.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkvmtkPolyBallLine.h"
#include "vtkvmtkConstants.h"
#include "vtkPointData.h"
#include "vtkPolyLine.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkPolyBallLine);

vtkvmtkPolyBallLine::vtkvmtkPolyBallLine()
{
  this->Input = NULL;
  this->InputCellIds = NULL;
  this->InputCellId = -1;
  this->PolyBallRadiusArrayName = NULL;
  this->LastPolyBallCellId = -1;
  this->LastPolyBallCellSubId = -1;
  this->LastPolyBallCellPCoord = 0.0;
  this->LastPolyBallCenter[0] = this->LastPolyBallCenter[1] = this->LastPolyBallCenter[2] = 0.0;
  this->LastPolyBallCenterRadius = 0.0;
  this->UseRadiusInformation = 1;
}

vtkvmtkPolyBallLine::~vtkvmtkPolyBallLine()
{
  if (this->Input)
    {
    this->Input->Delete();
    this->Input = NULL;
    }

  if (this->InputCellIds)
    {
    this->InputCellIds->Delete();
    this->InputCellIds = NULL;
    }

  if (this->PolyBallRadiusArrayName)
    {
    delete[] this->PolyBallRadiusArrayName;
    this->PolyBallRadiusArrayName = NULL;
    }
}

double vtkvmtkPolyBallLine::ComplexDot(double x[4], double y[4])
{
  return x[0]*y[0] + x[1]*y[1] + x[2]*y[2] - x[3]*y[3];
}

double vtkvmtkPolyBallLine::EvaluateFunction(double x[3])
{
  vtkIdType i, k;
  vtkIdType npts, *pts;
  double polyballFunctionValue, minPolyBallFunctionValue;
  double point0[3], point1[3];
  double radius0, radius1;
  double vector0[4], vector1[4], closestPoint[4];
  double t;
  double num, den;
  vtkDataArray *polyballRadiusArray = NULL;

  if (!this->Input)
    {
    vtkErrorMacro(<<"No Input specified!");
    return 0.0;
    }

  if (this->Input->GetNumberOfPoints()==0)
    {
    vtkWarningMacro(<<"Empty Input specified!");
    return 0.0;
    }

  if (this->UseRadiusInformation)
    {
    if (!this->PolyBallRadiusArrayName)
      {
      vtkErrorMacro(<<"No PolyBallRadiusArrayName specified!");
      return 0.0;
      }

    polyballRadiusArray = this->Input->GetPointData()->GetArray(this->PolyBallRadiusArrayName);

    if (polyballRadiusArray==NULL)
      {
      vtkErrorMacro(<<"PolyBallRadiusArray with name specified does not exist!");
      return 0.0;
      }
    }

  if (this->Input->GetLines()==NULL)
    {
    vtkWarningMacro(<<"No lines in Input dataset.");
    return 0.0;
    }

  this->Input->BuildCells();
#if (VTK_MAJOR_VERSION <= 5)
  this->Input->Update();
#endif

  minPolyBallFunctionValue = VTK_VMTK_LARGE_DOUBLE;

  closestPoint[0] = closestPoint[1] = closestPoint[2] = closestPoint[2] = 0.0;

  this->LastPolyBallCellId = -1;
  this->LastPolyBallCellSubId = -1;
  this->LastPolyBallCellPCoord = 0.0;
  this->LastPolyBallCenter[0] = this->LastPolyBallCenter[1] = this->LastPolyBallCenter[2] = 0.0;
  this->LastPolyBallCenterRadius = 0.0;

  vtkIdList* cellIds = vtkIdList::New();

  if (this->InputCellIds)
    {
    cellIds->DeepCopy(this->InputCellIds);
    }
  else if (this->InputCellId != -1)
    {
    cellIds->InsertNextId(this->InputCellId);
    }
  else
    {
    cellIds->SetNumberOfIds(this->Input->GetNumberOfCells());
    for (k=0; k<this->Input->GetNumberOfCells(); k++)
      {
      cellIds->SetId(k,k);
      }
    }

  for (k=0; k<cellIds->GetNumberOfIds(); k++)
    {
    vtkIdType cellId = cellIds->GetId(k);

    if (this->Input->GetCellType(cellId)!=VTK_LINE && this->Input->GetCellType(cellId)!=VTK_POLY_LINE)
      {
      continue;
      }

    this->Input->GetCellPoints(cellId,npts,pts);
    
    for (i=0; i<npts-1; i++)
      {
      this->Input->GetPoint(pts[i],point0);
      this->Input->GetPoint(pts[i+1],point1);
      if (this->UseRadiusInformation)
        {
        radius0 = polyballRadiusArray->GetComponent(pts[i],0);
        radius1 = polyballRadiusArray->GetComponent(pts[i+1],0);
        }
      else
        {
        radius0 = 0.0;
        radius1 = 0.0;
        }
      vector0[0] = point1[0] - point0[0];
      vector0[1] = point1[1] - point0[1];
      vector0[2] = point1[2] - point0[2];
      vector0[3] = radius1 - radius0;
      vector1[0] = x[0] - point0[0];
      vector1[1] = x[1] - point0[1];
      vector1[2] = x[2] - point0[2];
      vector1[3] = 0.0 - radius0;

//       cout<<x[0]<<" "<<x[1]<<" "<<x[2]<<" "<<point0[0]<<" "<<point0[1]<<" "<<point0[2]<<" "<<point1[0]<<" "<<point1[1]<<" "<<point1[2]<<" "<<endl;

      num = this->ComplexDot(vector0,vector1);
      den = this->ComplexDot(vector0,vector0);
      
      if (fabs(den)<VTK_VMTK_DOUBLE_TOL)
        {
        continue;
        }

      t = num / den;

      if (t<VTK_VMTK_DOUBLE_TOL)
        {
        t = 0.0;
        closestPoint[0] = point0[0];
        closestPoint[1] = point0[1];
        closestPoint[2] = point0[2];
        closestPoint[3] = radius0;
        }
      else if (1.0-t<VTK_VMTK_DOUBLE_TOL)
        {
        t = 1.0;
        closestPoint[0] = point1[0];
        closestPoint[1] = point1[1];
        closestPoint[2] = point1[2];
        closestPoint[3] = radius1;
        }
      else
        {
        closestPoint[0] = point0[0] + t * vector0[0];
        closestPoint[1] = point0[1] + t * vector0[1];
        closestPoint[2] = point0[2] + t * vector0[2];
        closestPoint[3] = radius0 + t * vector0[3];
        }

      polyballFunctionValue = (x[0]-closestPoint[0])*(x[0]-closestPoint[0]) + (x[1]-closestPoint[1])*(x[1]-closestPoint[1]) + (x[2]-closestPoint[2])*(x[2]-closestPoint[2]) - closestPoint[3]*closestPoint[3];

      if (polyballFunctionValue<minPolyBallFunctionValue)
        {
        minPolyBallFunctionValue = polyballFunctionValue;
        this->LastPolyBallCellId = cellId;
        this->LastPolyBallCellSubId = i;
        this->LastPolyBallCellPCoord = t;
        this->LastPolyBallCenter[0] = closestPoint[0];
        this->LastPolyBallCenter[1] = closestPoint[1];
        this->LastPolyBallCenter[2] = closestPoint[2];
        this->LastPolyBallCenterRadius = closestPoint[3];
        }
      }
    }
  
  cellIds->Delete();

  return minPolyBallFunctionValue;
}

void vtkvmtkPolyBallLine::EvaluateGradient(double x[3], double n[3])
{
  vtkWarningMacro("Poly ball gradient computation not yet implemented!");
  // TODO
}

void vtkvmtkPolyBallLine::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}
