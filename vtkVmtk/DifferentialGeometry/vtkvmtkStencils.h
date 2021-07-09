/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkStencils.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
  Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkStencils - Create an empty, umbrella, area weighted umbrella, finite-element LP, or finite-volume finite-element stencil for a particular data set.
// .SECTION Description
// ..

#ifndef __vtkvmtkStencils_h
#define __vtkvmtkStencils_h

#include "vtkObject.h"
#include "vtkvmtkConstants.h"
#include "vtkvmtkDataSetItems.h"
#include "vtkvmtkStencil.h"
#include "vtkDataSet.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkStencils : public vtkvmtkDataSetItems 
{
public:

  static vtkvmtkStencils* New();
  vtkTypeMacro(vtkvmtkStencils,vtkvmtkItems);

  void SetStencilTypeToEmptyStencil()
    {this->SetItemType(VTK_VMTK_EMPTY_STENCIL);};  
  void SetStencilTypeToUmbrellaStencil()
    {this->SetItemType(VTK_VMTK_UMBRELLA_STENCIL);};  
  void SetStencilTypeToAreaWeightedUmbrellaStencil()
    {this->SetItemType(VTK_VMTK_AREA_WEIGHTED_UMBRELLA_STENCIL);};  
  void SetStencilTypeToFELaplaceBeltramiStencil()
    {this->SetItemType(VTK_VMTK_FE_LAPLACE_BELTRAMI_STENCIL);};  
  void SetStencilTypeToFVFELaplaceBeltramiStencil()
    {this->SetItemType(VTK_VMTK_FVFE_LAPLACE_BELTRAMI_STENCIL);};
  void SetStencilTypeToGradientStencil()
    {this->SetItemType(VTK_VMTK_GRADIENT_STENCIL);};  

  // Description:
  // Get a stencil given a point id.
  vtkvmtkStencil* GetStencil(vtkIdType ptId) {return (vtkvmtkStencil*)this->Array[ptId];};

  void SetNumberOfStencils(vtkIdType numberOfStencils) { this->SetNumberOfItems(numberOfStencils);};
  vtkIdType GetNumberOfStencils() {return this->GetNumberOfItems();};

  void AllocateStencil(vtkIdType i, vtkIdType stencilType) {this->AllocateItem(i,stencilType);};

  vtkSetMacro(WeightScaling,int)
  vtkGetMacro(WeightScaling,int)
  vtkBooleanMacro(WeightScaling,int)

  vtkSetMacro(NegateWeights,int)
  vtkGetMacro(NegateWeights,int)
  vtkBooleanMacro(NegateWeights,int)

protected:
  vtkvmtkStencils();
  ~vtkvmtkStencils() {};

  virtual vtkvmtkItem* InstantiateNewItem(int itemType) override;

  int WeightScaling;

  int NegateWeights;
  
private:
  vtkvmtkStencils(const vtkvmtkStencils&);  // Not implemented.
  void operator=(const vtkvmtkStencils&);  // Not implemented.
};

#endif

