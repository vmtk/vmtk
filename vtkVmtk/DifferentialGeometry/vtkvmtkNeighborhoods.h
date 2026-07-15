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
 * @class   vtkvmtkNeighborhoods
 * @brief   Create an empty, polydata, polydata manifold, polydata manifold extended, or unstructured grid neighborhood from a matching vtk object.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkNeighborhoods is the concrete vtkvmtkDataSetItems collection that builds one
 * vtkvmtkNeighborhood per point of an input DataSet (set via the inherited SetDataSet). The
 * SetNeighborhoodTypeTo* convenience methods select which concrete vtkvmtkNeighborhood subclass
 * (vtkvmtkEmptyNeighborhood, vtkvmtkPolyDataNeighborhood, vtkvmtkPolyDataManifoldNeighborhood,
 * vtkvmtkPolyDataManifoldExtendedNeighborhood, or vtkvmtkUnstructuredGridNeighborhood) is
 * instantiated for each point; Build() (inherited from vtkvmtkDataSetItems) then computes every
 * point's neighborhood. This is the standard way vmtk filters (e.g. non-manifold edge detection,
 * centerline resampling) obtain, for every point of a surface or volume mesh, the ids of its
 * topological neighbors.
 *
 * @sa
 * vtkvmtkDataSetItems, vtkvmtkNeighborhood
 */

#ifndef __vtkvmtkNeighborhoods_h
#define __vtkvmtkNeighborhoods_h

#include "vtkObject.h"
#include "vtkvmtkConstants.h"
#include "vtkvmtkDataSetItems.h"
#include "vtkvmtkNeighborhood.h"
#include "vtkDataSet.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkNeighborhoods : public vtkvmtkDataSetItems 
{
public:

  static vtkvmtkNeighborhoods* New();
  vtkTypeMacro(vtkvmtkNeighborhoods,vtkvmtkDataSetItems);

  ///@{
  /**
   * Select which concrete vtkvmtkNeighborhood subclass Build()/AllocateNeighborhood() instantiate
   * for each point: an empty (no-op) neighborhood, a generic poly data neighborhood, a poly data
   * neighborhood restricted to a 2-manifold surface, an extended (larger-ring) manifold
   * neighborhood, or an unstructured grid neighborhood. Equivalent to calling SetItemType() with
   * the matching VTK_VMTK_*_NEIGHBORHOOD constant.
   */
  void SetNieghborhoodTypeToEmptyNeighborhood()
    {this->SetItemType(VTK_VMTK_EMPTY_NEIGHBORHOOD);};
  void SetNeighborhoodTypeToPolyDataNeighborhood()
    {this->SetItemType(VTK_VMTK_POLYDATA_NEIGHBORHOOD);};
  void SetNeighborhoodTypeToPolyDataManifoldNeighborhood()
    {this->SetItemType(VTK_VMTK_POLYDATA_MANIFOLD_NEIGHBORHOOD);};
  void SetNeighborhoodTypeToPolyDataManifoldExtendedNeighborhood()
    {this->SetItemType(VTK_VMTK_POLYDATA_MANIFOLD_EXTENDED_NEIGHBORHOOD);};
  void SetNeighborhoodTypeToUnstructuredGridNeighborhood()
    {this->SetItemType(VTK_VMTK_UNSTRUCTUREDGRID_NEIGHBORHOOD);};
  ///@}

  /**
   * Get the neighborhood item built for the point with the given id. Valid only after Build() has
   * been called.
   */
  vtkvmtkNeighborhood* GetNeighborhood(vtkIdType ptId) {return (vtkvmtkNeighborhood*)this->Array[ptId];};

  ///@{
  /**
   * Set/get the number of neighborhoods (i.e. points) in the collection.
   */
  void SetNumberOfNeighborhoods(vtkIdType numberOfNeighborhoods) { this->SetNumberOfItems(numberOfNeighborhoods);};
  vtkIdType GetNumberOfNeighborhoods() {return this->GetNumberOfItems();};
  ///@}

  /**
   * Replace the neighborhood at index i with a newly instantiated neighborhood of the given
   * neighborhoodType (one of the VTK_VMTK_*_NEIGHBORHOOD constants), overriding the collection-wide
   * type set via the SetNeighborhoodTypeTo* methods for that single point.
   */
  void AllocateNeighborhood(vtkIdType i, vtkIdType neighborhoodType) {this->AllocateItem(i,neighborhoodType);};

protected:
  vtkvmtkNeighborhoods() {};
  ~vtkvmtkNeighborhoods() {};

  virtual vtkvmtkItem* InstantiateNewItem(int itemType) override;

private:
  vtkvmtkNeighborhoods(const vtkvmtkNeighborhoods&);  // Not implemented.
  void operator=(const vtkvmtkNeighborhoods&);  // Not implemented.
};

#endif

