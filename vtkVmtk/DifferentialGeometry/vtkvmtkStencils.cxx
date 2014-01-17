/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkStencils.cxx,v $
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

#include "vtkvmtkStencils.h"

#include "vtkObjectFactory.h"
#include "vtkvmtkEmptyStencil.h"
#include "vtkvmtkPolyDataUmbrellaStencil.h"
#include "vtkvmtkPolyDataAreaWeightedUmbrellaStencil.h"
#include "vtkvmtkPolyDataFELaplaceBeltramiStencil.h"
#include "vtkvmtkPolyDataFVFELaplaceBeltramiStencil.h"
#include "vtkvmtkPolyDataGradientStencil.h"


vtkStandardNewMacro(vtkvmtkStencils);

vtkvmtkStencils::vtkvmtkStencils()
{
  this->WeightScaling = 1;
  this->NegateWeights = 1;
  this->ReallocateOnBuild = 0;
}

vtkvmtkItem* vtkvmtkStencils::InstantiateNewItem(int itemType)
  {
  vtkvmtkStencil* newStencil;

  switch(itemType)
    {
    case VTK_VMTK_EMPTY_STENCIL:
      newStencil = vtkvmtkEmptyStencil::New();
      break;
    case VTK_VMTK_UMBRELLA_STENCIL:
      newStencil = vtkvmtkPolyDataUmbrellaStencil::New();
      break;
    case VTK_VMTK_AREA_WEIGHTED_UMBRELLA_STENCIL:
      newStencil = vtkvmtkPolyDataAreaWeightedUmbrellaStencil::New();
      break;
    case VTK_VMTK_FE_LAPLACE_BELTRAMI_STENCIL:
      newStencil = vtkvmtkPolyDataFELaplaceBeltramiStencil::New();
      break;
    case VTK_VMTK_FVFE_LAPLACE_BELTRAMI_STENCIL:
      newStencil = vtkvmtkPolyDataFVFELaplaceBeltramiStencil::New();
      break;
    case VTK_VMTK_GRADIENT_STENCIL:
      newStencil = vtkvmtkPolyDataGradientStencil::New();
      break;
    default:
      vtkErrorMacro(<<"Invalid stencil type");
      return NULL;
    }
  
  newStencil->SetWeightScaling(this->WeightScaling);
  newStencil->SetNegateWeights(this->NegateWeights);
  newStencil->SetReallocateOnBuild(this->ReallocateOnBuild);

  newStencil->Register(this);
  newStencil->Delete();

  return newStencil;
  }

