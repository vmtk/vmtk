/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkDataSetItems.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/15 17:39:25 $
  Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkDataSetItems.h"

#include "vtkObjectFactory.h"
#include "vtkvmtkDataSetItem.h"



void vtkvmtkDataSetItems::Build()
  {
  vtkIdType numPts;
  vtkIdType pointId;
  vtkvmtkDataSetItem *dataSetItem;

  if (this->DataSet==NULL)
    {
    vtkErrorMacro(<<"No DataSet specified.");
    return;
    }

  numPts = this->DataSet->GetNumberOfPoints();

  if (!this->Array || (this->Array && this->ReallocateOnBuild))
    {
    this->Allocate(numPts);
    this->MaxId = numPts - 1;
    }

  for (pointId=0; pointId<numPts; pointId++)
    {
    dataSetItem = vtkvmtkDataSetItem::SafeDownCast(this->Array[pointId]);
    if (dataSetItem==NULL)
      {
      vtkErrorMacro(<<"Cannot build non-vtkvmtkDataSetItem item.");
      return;
      }
      
    dataSetItem->SetDataSet(this->DataSet);
    dataSetItem->SetDataSetPointId(pointId);
    dataSetItem->Build();
    }
  }

