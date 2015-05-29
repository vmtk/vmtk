/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyBallLine2.cxx,v $
  Language:  C++
  Date:      $$
  Version:   $$

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
     
  Note: this class was contributed by 
	Tangui Morvan
	Kalkulo AS
	Simula Research Laboratory

=========================================================================*/
#include "vtkvmtkPolyBallLine2.h"
#include "vtkvmtkConstants.h"
#include "vtkPointData.h"
#include "vtkPolyLine.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkPolyBallLine2);

vtkvmtkPolyBallLine2::vtkvmtkPolyBallLine2()
{
  this->Input = NULL;
  this->InputCellIds = NULL;
  this->InputCellId = -1;
  this->PolyBallRadiusArrayName = NULL;
  this->UseRadiusInformation = 1;
  this->LocalInput = NULL;
  this->TriangulatedInput = NULL;
  this->Triangulator = vtkTriangleFilter::New();
  this->Triangulator->PassLinesOn();
  this->Locator = vtkCellLocator::New();
  this->ShouldBuildLocator = true;
  this->LocatorMTime = 0;
}

vtkvmtkPolyBallLine2::~vtkvmtkPolyBallLine2()
{
  if (this->LocalInput && (this->LocalInput != this->Input))
    {
    this->LocalInput->Delete();
    this->LocalInput = NULL;
    }
    
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
    
  if (this->Triangulator)
    {
    this->Triangulator->Delete();
    this->Triangulator = NULL;
    }
  
  if (this->Locator)
    {
    this->Locator->Delete();
    this->Locator = NULL;
    }
}

void vtkvmtkPolyBallLine2::SetInput(vtkPolyData *inp)
{
  if (this->LocalInput == this->Input)
    {
    this->LocalInput = NULL;
    }
  //The locator needs to be rebuilt
  this->ShouldBuildLocator = true;
  vtkSetObjectBodyMacro(Input,vtkPolyData,inp);
}

void vtkvmtkPolyBallLine2::SetInputCellIds(vtkIdList *cellIds)
{
  //The locator needs to be rebuilt
  this->ShouldBuildLocator = true;
  vtkSetObjectBodyMacro(InputCellIds,vtkIdList,cellIds);
}

void vtkvmtkPolyBallLine2::SetInputCellId(vtkIdType cellId)
{
  vtkDebugMacro(<< this->GetClassName() << " (" << this << "): setting InputCellId to " << cellId);
  if (this->InputCellId != cellId) 
    {
    //The locator needs to be rebuilt
    this->ShouldBuildLocator = true;
    this->InputCellId = cellId;
    this->Modified();
    }
} 
double vtkvmtkPolyBallLine2::ComplexDot(double x[4], double y[4])
{
  return x[0]*y[0] + x[1]*y[1] + x[2]*y[2] - x[3]*y[3];
}

double vtkvmtkPolyBallLine2::EvaluateFunction(double x[3])
{ 
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
    }
    
  this->ShouldBuildLocator = this->ShouldBuildLocator || (this->LocatorMTime != this->Input->GetMTime());

  //Rebuild the locator if needed
  if (this->ShouldBuildLocator)
    {
    this->BuildLocator();
    }
    
  //Find the cell closest to x
  double dist2;
  double closPt[3];
  vtkIdType closCellId;
  int subId;
  this->Locator->FindClosestPoint(x,closPt,closCellId,subId,dist2);
  
  double value;
  
  if (this->UseRadiusInformation)
    {
    vtkDataArray *polyballRadiusArray =  this->TriangulatedInput->GetPointData()->GetArray(this->PolyBallRadiusArrayName);

    if (polyballRadiusArray==NULL)
      {
      vtkErrorMacro(<<"PolyBallRadiusArray with name specified does not exist!");
      return 0.0;
      }
    
    vtkIdType cellType = this->TriangulatedInput->GetCellType(closCellId);
    
    if (cellType != VTK_LINE)
      {
      vtkErrorMacro(<<"Error wrong cell type in polyline!");
      return 0.0;
      }
    
    vtkIdList *linePointsIds = vtkIdList::New();
    this->TriangulatedInput->GetCellPoints(closCellId, linePointsIds);
    double *point0;
    double *point1;
    double vector0[3], vector1[3];
    double radius0, radius1;
    point0 = this->TriangulatedInput->GetPoint(linePointsIds->GetId(0));
    point1 = this->TriangulatedInput->GetPoint(linePointsIds->GetId(1));
    
    for (int i=0;i<3;i++)
    {
      vector0[i] = point1[i] - point0[i];
      vector1[i] = closPt[i] - point0[i];
    }
    
    double num = vtkMath::Dot(vector0,vector1);
    double den = vtkMath::Dot(vector0,vector0);
    
    double t=0;
    
    //t should be between 0 and 1 since we took the closest point
    if (den!=0.)
      {
      t = num/den;
      }
    
    radius0 = polyballRadiusArray->GetComponent(linePointsIds->GetId(0),0);
    radius1 = polyballRadiusArray->GetComponent(linePointsIds->GetId(1),0);
    
    double radius = radius0 + t*(radius1 - radius0);
    
    value = sqrt(dist2) - radius;
    
    linePointsIds->Delete();
    }
  else
    {
    value = sqrt(dist2);
    }

  return value;
}

void vtkvmtkPolyBallLine2::EvaluateGradient(double x[3], double n[3])
{
  vtkWarningMacro("Poly ball gradient computation not yet implemented!");
  // TODO
}

void vtkvmtkPolyBallLine2::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}

void vtkvmtkPolyBallLine2::BuildLocator()
{
   if (!this->Input)
    {
    vtkErrorMacro(<<"No Input specified!");
    return;
    }

  if (this->Input->GetNumberOfPoints()==0)
    {
    vtkWarningMacro(<<"Empty Input specified!");
    return;
    }
    
  this->LocatorMTime = this->Input->GetMTime();
  
  if (this->LocalInput && (this->LocalInput != this->Input))
    {
    this->LocalInput->Delete();
    }
  
  //If only part of the cells are used, copy these cells to LocalInput
  if (this->InputCellIds || this->InputCellId)
    {
    this->LocalInput = vtkPolyData::New();
    this->LocalInput->DeepCopy(this->Input);
      
    if (this->InputCellIds)
      {
      //Remove the cells whose id is not the list
      for (int i=0; i<this->LocalInput->GetNumberOfCells(); i++)
        {
        if (!this->InputCellIds->IsId(i))
          {
          this->LocalInput->DeleteCell(i);
          }
        }
      }
      
    else if (this->InputCellId != -1)
      {
      //Remove the cells whose is not InputCellId
      for (int i=0; i<this->LocalInput->GetNumberOfCells(); i++)
        {
        if (i!=this->InputCellId)
          {
          this->LocalInput->DeleteCell(i);
          }
        }
      }
    }
  else
    {
    this->LocalInput = this->Input;
    }
    
  //Triangulate the input for improved performance
#if (VTK_MAJOR_VERSION <= 5)
  this->Triangulator->SetInput(this->LocalInput);
#else
  this->Triangulator->SetInputData(this->LocalInput);
#endif
  this->Triangulator->Update();
  this->TriangulatedInput = this->Triangulator->GetOutput();
  
  //Build the locator
  this->Locator->SetDataSet(this->TriangulatedInput);
  this->Locator->BuildLocator();
  
  this->ShouldBuildLocator = false;
}
