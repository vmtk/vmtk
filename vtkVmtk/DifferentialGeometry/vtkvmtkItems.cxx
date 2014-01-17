/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkItems.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/15 17:39:25 $
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

#include "vtkvmtkItems.h"

#include "vtkObjectFactory.h"
#include "vtkIdList.h"
#include "vtkCell.h"



vtkvmtkItems::~vtkvmtkItems()
{
  if ( this->Array != NULL )
    {
    this->ReleaseArray();

    delete [] this->Array;
    this->Array = NULL;
    }
}

void vtkvmtkItems::Allocate(vtkIdType sz, vtkIdType ext)
{
  if ( this->Array != NULL )
    {
    this->ReleaseArray();
    delete [] this->Array;
    this->Array = NULL;
    }

  this->Size = sz;
  this->Array = new vtkvmtkItem*[sz];
  this->Extend = ext;
  this->MaxId = -1;

  for (vtkIdType i=0; i<sz; i++)
    {
    this->Array[i] = InstantiateNewItem(this->ItemType);
    }
}

void vtkvmtkItems::AllocateItem(vtkIdType i, vtkIdType itemType)
  {
  this->Array[i]->Delete();
  this->Array[i] = InstantiateNewItem(itemType);
  }

void vtkvmtkItems::ReleaseArray()
  {
  for (vtkIdType i=0; i<this->Size; i++)
    {
    this->Array[i]->UnRegister(this);
    }
  }

void vtkvmtkItems::Squeeze()
  {
  this->Resize (this->MaxId+1);
  }

void vtkvmtkItems::Reset()
  {
  this->MaxId = -1;
  }

void vtkvmtkItems::Initialize()
  {
  this->Reset();
  this->Squeeze();
  }

vtkvmtkItem **vtkvmtkItems::Resize(vtkIdType sz)
{
  vtkIdType i;
  vtkvmtkItem** newArray;
  vtkIdType newSize;

  if ( sz >= this->Size )
    {
    newSize = this->Size + sz;
    }
  else
    {
    newSize = sz;
    }

  newArray = new vtkvmtkItem*[newSize];

  for (i=0; i<sz && i<this->Size; i++)
    {
    newArray[i] = this->Array[i];
    }

  for (i=this->Size; i<newSize ; i++)
    {
    newArray[i] = this->InstantiateNewItem(this->ItemType);
    }

  this->Size = newSize;
  delete [] this->Array;
  this->Array = newArray;

  return NULL;
}

void vtkvmtkItems::DeepCopy(vtkvmtkItems *src)
  {
  vtkIdType i, numItems;

  numItems = src->GetNumberOfItems();

  this->Allocate(src->Size, src->Extend);
  for (i=0; i<numItems; i++)
    {
    this->GetItem(i)->DeepCopy(src->GetItem(i));
    }

  this->MaxId = src->MaxId;
  }

void vtkvmtkItems::ShallowCopy(vtkvmtkItems *src)
  {
  if ( this->Array != NULL )
    {
    this->ReleaseArray();
    delete [] this->Array;
    this->Array = NULL;
    }

  this->Array = new vtkvmtkItem*[src->Size];

  for (vtkIdType i=0; i<src->Size; i++)
    {
    this->Array[i] = src->Array[i];
    this->Array[i]->Register(this);
    }

  this->MaxId = src->MaxId;
  this->Extend = src->Extend;
  this->Size = src->Size;
  }
