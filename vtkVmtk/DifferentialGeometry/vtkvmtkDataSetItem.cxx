/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkDataSetItem.cxx,v $
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

#include "vtkvmtkDataSetItem.h"
#include "vtkObjectFactory.h"



vtkvmtkDataSetItem::vtkvmtkDataSetItem()
  {
  this->DataSetPointId = -1;
  this->DataSet = NULL;
  this->ReallocateOnBuild = 0;
  }

void vtkvmtkDataSetItem::DeepCopy(vtkvmtkItem* src)
  {
  this->Superclass::DeepCopy(src);

  vtkvmtkDataSetItem* dataSetItemSrc = vtkvmtkDataSetItem::SafeDownCast(src);

  if (dataSetItemSrc==NULL)
    {
    vtkErrorMacro(<<"Trying to deep copy a non-stencil item");
    }

  this->SetDataSet(dataSetItemSrc->GetDataSet());
  this->DataSetPointId = dataSetItemSrc->DataSetPointId;

  }
