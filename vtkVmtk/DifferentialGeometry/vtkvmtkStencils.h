/*=========================================================================

  Program:   VMTK

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/**
 * @class   vtkvmtkStencils
 * @brief   Create an empty, umbrella, area weighted umbrella, finite-element LP, or finite-volume finite-element stencil for a particular data set.
 * @ingroup DifferentialGeometry
 *
 * A vtkvmtkItems collection specialized to hold one vtkvmtkStencil per point of a data set (see
 * vtkvmtkDataSetItems::SetDataSet), instantiating the concrete stencil subclass requested by
 * SetItemType (or the SetStencilTypeTo* convenience methods) for each point. WeightScaling and
 * NegateWeights are propagated to every stencil instantiated afterwards, mirroring the
 * like-named flags on vtkvmtkStencil itself.
 *
 * @sa vtkvmtkStencil, vtkvmtkDataSetItems
 */

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

  /**
   * Convenience method: subsequent AllocateStencil/AllocateItem calls create vtkvmtkEmptyStencil
   * instances.
   */
  void SetStencilTypeToEmptyStencil()
    {this->SetItemType(VTK_VMTK_EMPTY_STENCIL);};
  /**
   * Convenience method: subsequent AllocateStencil/AllocateItem calls create
   * vtkvmtkPolyDataUmbrellaStencil instances (uniform Laplacian weights).
   */
  void SetStencilTypeToUmbrellaStencil()
    {this->SetItemType(VTK_VMTK_UMBRELLA_STENCIL);};
  /**
   * Convenience method: subsequent AllocateStencil/AllocateItem calls create
   * vtkvmtkPolyDataAreaWeightedUmbrellaStencil instances.
   */
  void SetStencilTypeToAreaWeightedUmbrellaStencil()
    {this->SetItemType(VTK_VMTK_AREA_WEIGHTED_UMBRELLA_STENCIL);};
  /**
   * Convenience method: subsequent AllocateStencil/AllocateItem calls create
   * vtkvmtkPolyDataFELaplaceBeltramiStencil instances.
   */
  void SetStencilTypeToFELaplaceBeltramiStencil()
    {this->SetItemType(VTK_VMTK_FE_LAPLACE_BELTRAMI_STENCIL);};
  /**
   * Convenience method: subsequent AllocateStencil/AllocateItem calls create
   * vtkvmtkPolyDataFVFELaplaceBeltramiStencil instances.
   */
  void SetStencilTypeToFVFELaplaceBeltramiStencil()
    {this->SetItemType(VTK_VMTK_FVFE_LAPLACE_BELTRAMI_STENCIL);};
  /**
   * Convenience method: subsequent AllocateStencil/AllocateItem calls create
   * vtkvmtkPolyDataGradientStencil instances.
   */
  void SetStencilTypeToGradientStencil()
    {this->SetItemType(VTK_VMTK_GRADIENT_STENCIL);};

  /**
   * Get a stencil given a point id.
   */
  vtkvmtkStencil* GetStencil(vtkIdType ptId) {return (vtkvmtkStencil*)this->Array[ptId];};

  ///@{
  /**
   * Set/Get the number of stencils held by this collection (one per data set point).
   */
  void SetNumberOfStencils(vtkIdType numberOfStencils) { this->SetNumberOfItems(numberOfStencils);};
  vtkIdType GetNumberOfStencils() {return this->GetNumberOfItems();};
  ///@}

  /**
   * Instantiate the stencil for point i as the given stencilType (one of the VTK_VMTK_*_STENCIL
   * constants, or use the SetStencilTypeTo* convenience methods beforehand and call
   * AllocateItem(i, GetItemType()) via the base class instead).
   */
  void AllocateStencil(vtkIdType i, vtkIdType stencilType) {this->AllocateItem(i,stencilType);};

  ///@{
  /**
   * Toggle whether ScaleWeights() rescales stencil weights, propagated to every stencil
   * instantiated by this collection afterwards. See vtkvmtkStencil::WeightScaling. Default: on.
   */
  vtkSetMacro(WeightScaling,int)
  vtkGetMacro(WeightScaling,int)
  vtkBooleanMacro(WeightScaling,int)
  ///@}

  ///@{
  /**
   * Toggle whether stencil weights are sign-negated after being computed, propagated to every
   * stencil instantiated by this collection afterwards. See vtkvmtkStencil::NegateWeights.
   * Default: on.
   */
  vtkSetMacro(NegateWeights,int)
  vtkGetMacro(NegateWeights,int)
  vtkBooleanMacro(NegateWeights,int)
  ///@}

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

