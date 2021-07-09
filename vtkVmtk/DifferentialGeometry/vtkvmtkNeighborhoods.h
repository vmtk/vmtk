/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkNeighborhoods.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
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
// .NAME vtkvmtkNeighborhoods - Create an empty, polydata, polydata manifold, polydata manifold extended, or unstructured grid neighborhood from a a matching vtk object.
// .SECTION Description
// ..

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

  // Description:
  // Get a stencil given a point id.
  vtkvmtkNeighborhood* GetNeighborhood(vtkIdType ptId) {return (vtkvmtkNeighborhood*)this->Array[ptId];};

  void SetNumberOfNeighborhoods(vtkIdType numberOfNeighborhoods) { this->SetNumberOfItems(numberOfNeighborhoods);};
  vtkIdType GetNumberOfNeighborhoods() {return this->GetNumberOfItems();};

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

