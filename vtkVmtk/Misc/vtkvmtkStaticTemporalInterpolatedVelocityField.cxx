/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkvmtkStaticTemporalInterpolatedVelocityField.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkvmtkStaticTemporalInterpolatedVelocityField.h"

#include "vtkTable.h"
#include "vtkDataSet.h"
#include "vtkGenericCell.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkvmtkStaticTemporalInterpolatedVelocityField); 
vtkCxxSetObjectMacro(vtkvmtkStaticTemporalInterpolatedVelocityField, TimeStepsTable, vtkTable);

vtkvmtkStaticTemporalInterpolatedVelocityField::vtkvmtkStaticTemporalInterpolatedVelocityField()
{
  this->Periodic = 0;
  this->VelocityScale = 1.0;
  this->TimeStepsTable = NULL;
  this->UseVectorComponents = 0;
  this->VectorPrefix = NULL;
  this->Component0Prefix = NULL;
  this->Component1Prefix = NULL;
  this->Component2Prefix = NULL;
}

vtkvmtkStaticTemporalInterpolatedVelocityField::~vtkvmtkStaticTemporalInterpolatedVelocityField()
{
  if (this->VectorPrefix) {
    delete[] this->VectorPrefix;
    this->VectorPrefix = NULL;
  }

  if (this->Component0Prefix) {
    delete[] this->Component0Prefix;
    this->Component0Prefix = NULL;
  }

  if (this->Component1Prefix) {
    delete[] this->Component1Prefix;
    this->Component1Prefix = NULL;
  }

  if (this->Component2Prefix) {
    delete[] this->Component2Prefix;
    this->Component2Prefix = NULL;
  }

  this->SetTimeStepsTable(NULL);
}

void vtkvmtkStaticTemporalInterpolatedVelocityField::SetLastCellId( vtkIdType c, int dataindex )
{
  this->LastCellId  = c; 
  this->LastDataSet = ( *this->DataSets )[dataindex];
  
  // if the dataset changes, then the cached cell is invalidated
  // we might as well prefetch the cached cell either way
  if ( this->LastCellId != -1 )
  {
    this->LastDataSet->GetCell( this->LastCellId, this->GenCell );
  } 
  
  this->LastDataSetIndex = dataindex;
}

void vtkvmtkStaticTemporalInterpolatedVelocityField::FindTimeRowId(double time, int& prevRowId, int& nextRowId, double& p)
{
  //cout<<"Time "<<time<<endl;
  prevRowId = 0;
  nextRowId = 0;
  p = 0.0;
  if (this->TimeStepsTable == NULL)
    {
    return;
    }
  int numberOfRows = this->TimeStepsTable->GetNumberOfRows();
  if (this->TimeStepsTable->GetNumberOfColumns() < 2 || numberOfRows == 1)
    {
    return;
    }
  int previousRowIndex = this->TimeStepsTable->GetValue(0,0).ToInt();
  double previousRowTime = this->TimeStepsTable->GetValue(0,1).ToDouble();
  if (numberOfRows < 2 || previousRowTime == time)
    {
    return;
    }

  double shiftedTime = time;

  double period = this->TimeStepsTable->GetValue(numberOfRows-1,1).ToDouble() - previousRowTime;

  if (this->Periodic)
    {
    double ratio = (time - previousRowTime) / period;
    shiftedTime = (ratio - floor(ratio)) * period + previousRowTime;
    }

  bool found = false;
  int pRowId = 0;
  for (int i=1; i<numberOfRows; i++)
    {
    int rowId = i;
    //if (i == numberOfRows)
    //  {
    //  rowId = 0;
    //  }
    int rowIndex = this->TimeStepsTable->GetValue(rowId,0).ToInt();
    double rowTime = this->TimeStepsTable->GetValue(rowId,1).ToDouble();
    if (shiftedTime > previousRowTime && shiftedTime <= rowTime)
      {
      prevRowId = pRowId;
      nextRowId = rowId;
      p = (shiftedTime - previousRowTime) / (rowTime - previousRowTime);
      found = true;
      break;
      }
    pRowId = rowId;
    previousRowIndex = rowIndex;
    previousRowTime = rowTime;
    }

  //cout<<"Index "<<prevRowId<<" "<<nextRowId<<endl;
  if (!found)
    {
    //cout<<"Shifted time "<<shiftedTime<<endl;
    prevRowId = numberOfRows - 2;
    nextRowId = numberOfRows - 2;
    p = 1.0;
    }
}

void vtkvmtkStaticTemporalInterpolatedVelocityField::BuildArrayName(char* prefix, int index, char* name)
{
  sprintf(name,"%s%d",prefix,index);
}

int vtkvmtkStaticTemporalInterpolatedVelocityField::FunctionValues( vtkDataSet * dataset, double * x, double * f )
{
  int i, j, subId , numPts, id;
  vtkDataArray * vectorsPrev = NULL;
  vtkDataArray * vectorsNext = NULL;
  vtkDataArray * component0Prev = NULL;
  vtkDataArray * component0Next = NULL;
  vtkDataArray * component1Prev = NULL;
  vtkDataArray * component1Next = NULL;
  vtkDataArray * component2Prev = NULL;
  vtkDataArray * component2Next = NULL;
  double vecPrev[3], vecNext[3];
  double dist2;
  int ret;
  char arrayName[1024];
  
  f[0] = f[1] = f[2] = 0.0;

  // See if a dataset has been specified and if there are input vectors
  if ( !dataset )
    {
    vtkErrorMacro( << "Can't evaluate dataset!" );
    return 0;
    }

  double time = x[3];

  int prevRowId, nextRowId;
  double timeP;
  this->FindTimeRowId(time,prevRowId,nextRowId,timeP);

  int prevTimeIndex = this->TimeStepsTable->GetValue(prevRowId,0).ToInt();
  int nextTimeIndex = this->TimeStepsTable->GetValue(nextRowId,0).ToInt();

  if (this->UseVectorComponents)
    {
    this->BuildArrayName(this->Component0Prefix,prevTimeIndex,arrayName);
    component0Prev = dataset->GetPointData()->GetArray(arrayName);
    this->BuildArrayName(this->Component1Prefix,prevTimeIndex,arrayName);
    component1Prev = dataset->GetPointData()->GetArray(arrayName);
    this->BuildArrayName(this->Component2Prefix,prevTimeIndex,arrayName);
    component2Prev = dataset->GetPointData()->GetArray(arrayName);

    if (!component0Prev || !component1Prev || !component2Prev)
      {
      vtkErrorMacro(<<"Component array not found for index "<<prevTimeIndex);
      return 0;
      } 

    this->BuildArrayName(this->Component0Prefix,nextTimeIndex,arrayName);
    component0Next = dataset->GetPointData()->GetArray(arrayName);
    this->BuildArrayName(this->Component1Prefix,nextTimeIndex,arrayName);
    component1Next = dataset->GetPointData()->GetArray(arrayName);
    this->BuildArrayName(this->Component2Prefix,nextTimeIndex,arrayName);
    component2Next = dataset->GetPointData()->GetArray(arrayName);

    if (timeP > 0.0 && (!component0Next || !component1Next || !component2Next))
      {
      vtkErrorMacro(<<"Component array not found for index "<<nextTimeIndex);
      return 0;
      } 
    }
  else
    {
    this->BuildArrayName(this->VectorPrefix,prevTimeIndex,arrayName);
    vectorsPrev = dataset->GetPointData()->GetArray(arrayName);

    if (timeP > 0.0 && !vectorsPrev)
      {
      vtkErrorMacro(<<"Vector array not found for index "<<prevTimeIndex);
      return 0;
      } 

    this->BuildArrayName(this->VectorPrefix,nextTimeIndex,arrayName);
    vectorsNext = dataset->GetPointData()->GetArray(arrayName);

    if (!vectorsNext)
      {
      vtkErrorMacro(<<"Vector array not found for index "<<nextTimeIndex);
      return 0;
      } 
    }

  double tol2 = dataset->GetLength() * 
                vtkAbstractInterpolatedVelocityField::TOLERANCE_SCALE;

  int found = 0;

  if ( this->Caching )
    {
    // See if the point is in the cached cell
    if ( this->LastCellId == -1 || 
         !(  ret = this->GenCell->EvaluatePosition
                   ( x, 0, subId, this->LastPCoords, dist2, this->Weights)
          )
        || ret == -1
       )
      {
      // if not, find and get it
      if ( this->LastCellId != - 1 )
        {
        this->CacheMiss ++;

        dataset->GetCell( this->LastCellId, this->Cell );
        
        this->LastCellId = 
          dataset->FindCell( x, this->Cell, this->GenCell, this->LastCellId, 
                             tol2, subId, this->LastPCoords, this->Weights );
                             
        if ( this->LastCellId != -1 )
          {
          dataset->GetCell( this->LastCellId, this->GenCell );
          found = 1;
          }
        }
      }
    else
      {
      this->CacheHit ++;
      found = 1;
      }
    }

  if ( !found )
    {
    // if the cell is not found, do a global search (ignore initial
    // cell if there is one)
    this->LastCellId = 
      dataset->FindCell( x, 0, this->GenCell, -1, tol2, 
                         subId, this->LastPCoords, this->Weights );
                         
    if ( this->LastCellId != -1 )
      {
      dataset->GetCell( this->LastCellId, this->GenCell );
      }
    else
      {
      return  0;
      }
    }
                                
  // if the cell is valid
  if ( this->LastCellId >= 0 )
    {
    numPts = this->GenCell->GetNumberOfPoints();
    
    // interpolate the vectors
    for ( j = 0; j < numPts; j ++ )
      {
      id = this->GenCell->PointIds->GetId( j );
      if (this->UseVectorComponents)
        {
        vecPrev[0] = this->VelocityScale * component0Prev->GetTuple1(id);
        vecPrev[1] = this->VelocityScale * component1Prev->GetTuple1(id);
        vecPrev[2] = this->VelocityScale * component2Prev->GetTuple1(id);

        if (timeP > 0.0)
          {
          vecNext[0] = this->VelocityScale * component0Next->GetTuple1(id);
          vecNext[1] = this->VelocityScale * component1Next->GetTuple1(id);
          vecNext[2] = this->VelocityScale * component2Next->GetTuple1(id);
          }
        }
      else
        {
        vectorsPrev->GetTuple( id, vecPrev );
        for ( i = 0; i < 3; i ++ )
          {
          vecPrev[i] *= this->VelocityScale;
          }
        if (timeP > 0.0)
          {
          vectorsNext->GetTuple( id, vecNext );
          for ( i = 0; i < 3; i ++ )
            {
            vecNext[i] *= this->VelocityScale;
            }
          }
        }
      if (timeP > 0.0)
        {
        for ( i = 0; i < 3; i ++ )
          {
          f[i] +=  (timeP * vecNext[i] + (1.0 - timeP) * vecPrev[i]) * this->Weights[j];
          }
        }
      else
        {
        for ( i = 0; i < 3; i ++ )
          {
          f[i] +=  vecPrev[i] * this->Weights[j];
          }
        }
      }
      
    if ( this->NormalizeVector == true )
      {
      vtkMath::Normalize( f );
      }  
    }
  // if not, return false
  else
    {
    return  0;
    }

  return  1;
}

void vtkvmtkStaticTemporalInterpolatedVelocityField::CopyParameters( vtkAbstractInterpolatedVelocityField * from )
{
  if (from->IsA("vtkvmtkStaticTemporalInterpolatedVelocityField"))
    {
    vtkvmtkStaticTemporalInterpolatedVelocityField* fromCast = vtkvmtkStaticTemporalInterpolatedVelocityField::SafeDownCast(from);
    vtkTable* timeStepsTable = vtkTable::New();
    timeStepsTable->DeepCopy(fromCast->GetTimeStepsTable());
    this->SetTimeStepsTable(timeStepsTable);
    this->SetPeriodic(fromCast->GetPeriodic());
    this->SetVelocityScale(fromCast->GetVelocityScale());
    this->SetUseVectorComponents(fromCast->UseVectorComponents);
    this->SetVectorPrefix(fromCast->VectorPrefix);
    this->SetComponent0Prefix(fromCast->Component0Prefix);
    this->SetComponent1Prefix(fromCast->Component1Prefix);
    this->SetComponent2Prefix(fromCast->Component2Prefix);
    timeStepsTable->Delete();
    }
}

void vtkvmtkStaticTemporalInterpolatedVelocityField::PrintSelf( std::ostream & os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}
