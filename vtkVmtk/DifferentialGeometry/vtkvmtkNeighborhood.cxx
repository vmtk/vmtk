/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkNeighborhood.cxx,v $
  Language:  C++
  Date:      $Date: 2005/03/04 11:07:29 $
  Version:   $Revision: 1.2 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkNeighborhood.h"
#include "vtkObjectFactory.h"



vtkvmtkNeighborhood::vtkvmtkNeighborhood()
  {
  this->NPoints = 0;
  this->PointIds = NULL;
  this->IsBoundary = false;
  }

vtkvmtkNeighborhood::~vtkvmtkNeighborhood()
  {
  if (this->PointIds != NULL)
    {
    delete [] this->PointIds;
    this->PointIds = NULL;
    }
  }

void vtkvmtkNeighborhood::ResizePointList(vtkIdType ptId, int size)
  {
  int newSize;
  vtkIdType *pointIds;
  
  newSize = this->NPoints + size;
  pointIds = new vtkIdType[newSize];
  memcpy(pointIds, this->PointIds,this->NPoints*sizeof(vtkIdType));
  delete [] this->PointIds;
  this->PointIds = NULL;

  this->PointIds = pointIds;

  this->NPoints = newSize;
  }

void vtkvmtkNeighborhood::DeepCopy(vtkvmtkItem *src)
  {
  this->Superclass::DeepCopy(src);

  vtkvmtkNeighborhood* neighborhoodSrc = vtkvmtkNeighborhood::SafeDownCast(src);

  if (neighborhoodSrc==NULL)
    {
    vtkErrorMacro(<<"Trying to deep copy a non-stencil item");
    }

  this->NPoints = neighborhoodSrc->NPoints;

  if (this->PointIds != NULL)
    {
    delete [] this->PointIds;
    this->PointIds = NULL;
    }

  if (neighborhoodSrc->NPoints>0)
    {
    this->PointIds = new vtkIdType[neighborhoodSrc->NPoints];
    memcpy(this->PointIds, neighborhoodSrc->PointIds, this->NPoints * sizeof(vtkIdType));
    }

  this->IsBoundary = neighborhoodSrc->IsBoundary;
  }

