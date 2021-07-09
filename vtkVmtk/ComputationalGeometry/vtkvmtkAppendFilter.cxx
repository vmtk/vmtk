/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkAppendFilter.cxx,v $
Language:  C++
Date:      $Date: 2005/03/31 15:49:05 $
Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkAppendFilter.h"

#include "vtkCell.h"
#include "vtkCellData.h"
#include "vtkDataSetAttributes.h"
#include "vtkDataSetCollection.h"
#include "vtkExecutive.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkUnstructuredGrid.h"

#include "vtkMath.h"
#include "vtkMergePoints.h"


vtkStandardNewMacro(vtkvmtkAppendFilter);

vtkvmtkAppendFilter::vtkvmtkAppendFilter()
{
  this->MergeDuplicatePoints = 1;
}

vtkvmtkAppendFilter::~vtkvmtkAppendFilter()
{
}

int vtkvmtkAppendFilter::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the output info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the ouptut
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkIdType numPts, numCells;
  int   tenth, count, abort=0;
  float decimal;
  vtkPoints *newPts;
  vtkPointData *pd;
  vtkCellData *cd;
  vtkIdList *ptIds, *newPtIds;
  int i, idx;
  vtkDataSet *ds;
  vtkIdType ptId, cellId, newCellId;
  vtkPointData *outputPD = output->GetPointData();
  vtkCellData *outputCD = output->GetCellData();

  vtkDebugMacro(<<"Appending data together");

  double bounds[6], inputBounds[6];
  vtkMath::UninitializeBounds(bounds);

  // Loop over all data sets, checking to see what data is common to 
  // all inputs. Note that data is common if 1) it is the same attribute 
  // type (scalar, vector, etc.), 2) it is the same native type (int, 
  // float, etc.), and 3) if a data array in a field, if it has the same name.
  count   = 0;
  decimal = 0.0;

  numPts = 0;
  numCells = 0;

  int numInputs = inputVector[0]->GetNumberOfInformationObjects();
  vtkDataSetAttributes::FieldList ptList(numInputs);
  vtkDataSetAttributes::FieldList cellList(numInputs);
  int firstPD=1;
  int firstCD=1;
  vtkInformation *inInfo = 0;
  for (idx = 0; idx < numInputs; ++idx)
    {
    inInfo = inputVector[0]->GetInformationObject(idx);
    ds = 0;
    if (inInfo)
      {
      ds = vtkDataSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
      }
    if (ds != NULL)
      {
      if ( ds->GetNumberOfPoints() <= 0 && ds->GetNumberOfCells() <= 0 )
        {
        continue; //no input, just skip
        }

      numPts += ds->GetNumberOfPoints();
      numCells += ds->GetNumberOfCells();

      ds->GetBounds(inputBounds);
      if (vtkMath::AreBoundsInitialized(bounds))
        {
        int j;
        for (j=0; j<6; j+=2)
          {
          if (inputBounds[j] < bounds[j])
            {
            bounds[j] = inputBounds[j];
            }
          if (inputBounds[j+1] > bounds[j+1])
            {
            bounds[j+1] = inputBounds[j+1];
            }
          }
        }
      else
        {
        int j;
        for (j=0; j<6; j+=2)
          {
          bounds[j] = inputBounds[j];
          bounds[j+1] = inputBounds[j+1];
          }
        }

      pd = ds->GetPointData();
      if ( firstPD )
        {
        ptList.InitializeFieldList(pd);
        firstPD = 0;
        }
      else
        {
        ptList.IntersectFieldList(pd);
        }
      
      cd = ds->GetCellData();
      if ( firstCD )
        {
        cellList.InitializeFieldList(cd);
        firstCD = 0;
        }
      else
        {
        cellList.IntersectFieldList(cd);
        }
      }//if non-empty dataset
    }//for all inputs

  if ( numPts < 1)
    {
    vtkDebugMacro(<<"No data to append!");
    return 1;
    }
  
  // Now can allocate memory
  output->Allocate(numCells); //allocate storage for geometry/topology
//  outputPD->CopyGlobalIdsOn();
  outputPD->CopyAllocate(ptList,numPts);
//  outputCD->CopyGlobalIdsOn();
  outputCD->CopyAllocate(cellList,numCells);

  newPts = vtkPoints::New();
  ptIds = vtkIdList::New(); ptIds->Allocate(VTK_CELL_SIZE);
  newPtIds = vtkIdList::New(); newPtIds->Allocate(VTK_CELL_SIZE);
  
  vtkMergePoints* locator = vtkMergePoints::New();
  if (this->MergeDuplicatePoints)
    {
    locator->InitPointInsertion(newPts,bounds);
    }

  vtkIdList* ptIdMap = vtkIdList::New();

  vtkIdType newPtId;
  double point[3];

  // Append each input dataset together
  //
  tenth = (numPts + numCells)/10 + 1;
  int inputCount = 0; // Since empty inputs are not in the list.
  for (idx = 0; idx < numInputs && !abort; ++idx)
    {
    inInfo = inputVector[0]->GetInformationObject(idx);
    ds = 0;
    if (inInfo)
      {
      ds = vtkDataSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
      }
    if ( ds != NULL &&
         (ds->GetNumberOfPoints() > 0 || ds->GetNumberOfCells() > 0) )
      {
      numPts = ds->GetNumberOfPoints();
      numCells = ds->GetNumberOfCells();
      pd = ds->GetPointData();

      ptIdMap->Initialize();
      ptIdMap->SetNumberOfIds(numPts);
      
      // copy points and point data
      for (ptId=0; ptId < numPts && !abort; ptId++)
        {
        ds->GetPoint(ptId,point);
        if (this->MergeDuplicatePoints)
          {
          if (locator->InsertUniquePoint(point,newPtId))
            {
            outputPD->CopyData(ptList,pd,inputCount,ptId,newPtId);
            }
          }
        else
          {
          newPtId = newPts->InsertNextPoint(point);
          outputPD->CopyData(ptList,pd,inputCount,ptId,newPtId);
          }
        ptIdMap->SetId(ptId,newPtId);
        
        // Update progress
        count++;
        if (!(count % tenth)) 
          {
          decimal += 0.1;
          this->UpdateProgress(decimal);
          abort = this->GetAbortExecute();
          }
        }
      
      cd = ds->GetCellData();
      // copy cell and cell data
      for (cellId=0; cellId<numCells && !abort; cellId++)
        {
        ds->GetCellPoints(cellId, ptIds);
        newPtIds->Reset();
        for (i=0; i<ptIds->GetNumberOfIds(); i++)
          {
          newPtIds->InsertId(i,ptIdMap->GetId(ptIds->GetId(i)));
          }
        newCellId = output->InsertNextCell(ds->GetCellType(cellId),newPtIds);
        outputCD->CopyData(cellList,cd,inputCount,cellId,newCellId);
        
        // Update progress
        count++;
        if (!(count % tenth)) 
          {
          decimal += 0.1;
          this->UpdateProgress(decimal);
          abort = this->GetAbortExecute();
          }
        }
      ++inputCount;
      }
    }
  
  // Update ourselves and release memory
  //
  output->SetPoints(newPts);
  newPts->Delete();
  ptIds->Delete();
  newPtIds->Delete();
  locator->Delete();

  return 1;
}

void vtkvmtkAppendFilter::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
