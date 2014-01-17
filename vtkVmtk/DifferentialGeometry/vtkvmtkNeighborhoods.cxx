/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkNeighborhoods.cxx,v $
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

#include "vtkvmtkNeighborhoods.h"

#include "vtkObjectFactory.h"
#include "vtkIdList.h"
#include "vtkCell.h"
#include "vtkvmtkEmptyNeighborhood.h"
#include "vtkvmtkPolyDataNeighborhood.h"
#include "vtkvmtkPolyDataManifoldNeighborhood.h"
#include "vtkvmtkPolyDataManifoldExtendedNeighborhood.h"
#include "vtkvmtkUnstructuredGridNeighborhood.h"


vtkStandardNewMacro(vtkvmtkNeighborhoods);

vtkvmtkItem* vtkvmtkNeighborhoods::InstantiateNewItem(int itemType)
  {
  vtkvmtkNeighborhood* newItem;

  switch(itemType)
    {
    case VTK_VMTK_EMPTY_NEIGHBORHOOD:
      newItem = vtkvmtkEmptyNeighborhood::New();
      break;
    case VTK_VMTK_POLYDATA_NEIGHBORHOOD:
      newItem = vtkvmtkPolyDataNeighborhood::New();
      break;
    case VTK_VMTK_POLYDATA_MANIFOLD_NEIGHBORHOOD:
      newItem = vtkvmtkPolyDataManifoldNeighborhood::New();
      break;
    case VTK_VMTK_POLYDATA_MANIFOLD_EXTENDED_NEIGHBORHOOD:
      newItem = vtkvmtkPolyDataManifoldExtendedNeighborhood::New();
      break;
    case VTK_VMTK_UNSTRUCTUREDGRID_NEIGHBORHOOD:
      newItem = vtkvmtkUnstructuredGridNeighborhood::New();
      break;
    default:
      vtkErrorMacro(<<"Invalid neighborhood type");
      return NULL;
    }

  newItem->SetReallocateOnBuild(this->ReallocateOnBuild);

  newItem->Register(this);
  newItem->Delete();

  return newItem;
  }
