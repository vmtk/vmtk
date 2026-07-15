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

  Note: this class was contributed by 
	Tangui Morvan
	Kalkulo AS
	Simula Research Laboratory
=========================================================================*/
/**
 * @class   vtkvmtkBoundaryLayerGenerator2
 * @brief   Provides an extended version of vtkvmtkBoundaryLayerGenerator.
 * @ingroup Contrib
 *
 * This version allows finer control on the surfaces to be included. If the original surface has open profiles, a surface can be extracted from them as well. The points on the open profiles may be specified with an array (OpenProfilesIdsArrayName). A value of -1 in this array specifies a point on the interior of the surface. Cell entity ids can also be output for the volume and surface elements. Surface elements are numbered in increasing numbers, using the openProfilesIds if specified.
 *
 * @sa vtkvmtkBoundaryLayerGenerator
 */

#ifndef __vtkvmtkBoundaryLayerGenerator2_h
#define __vtkvmtkBoundaryLayerGenerator2_h

#include "vtkvmtkBoundaryLayerGenerator.h"
#include "vtkvmtkWin32Header.h"

class vtkPoints;
class vtkDataArray;
class vtkOrderedTriangulator;

class VTK_VMTK_CONTRIB_EXPORT vtkvmtkBoundaryLayerGenerator2 : public vtkvmtkBoundaryLayerGenerator
{
  public: 
  vtkTypeMacro(vtkvmtkBoundaryLayerGenerator2,vtkvmtkBoundaryLayerGenerator);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkBoundaryLayerGenerator2 *New();

  // vtkGetStringMacro(CellEntityIdsArrayName);
  // vtkSetStringMacro(CellEntityIdsArrayName);

  ///@{
  /**
   * Set/get the name of the per-point array that identifies which points
   * of the input surface lie on an open profile. A value of -1 marks a
   * point on the interior of the surface; any other value groups points
   * belonging to the same open profile.
   */
  vtkGetStringMacro(OpenProfilesIdsArrayName);
  vtkSetStringMacro(OpenProfilesIdsArrayName);
  ///@}

  ///@{
  /**
   * Include/exclude the extruded open-profile surface cells (the caps
   * generated on the boundary-layer extrusion of the open profiles) in
   * the output unstructured grid.
   */
  vtkGetMacro(IncludeExtrudedOpenProfilesCells,int);
  vtkSetMacro(IncludeExtrudedOpenProfilesCells,int);
  vtkBooleanMacro(IncludeExtrudedOpenProfilesCells,int);
  ///@}

  ///@{
  /**
   * Include/exclude the extruded outer surface cells (the outermost shell
   * of the boundary layer) in the output unstructured grid.
   */
  vtkGetMacro(IncludeExtrudedSurfaceCells,int);
  vtkSetMacro(IncludeExtrudedSurfaceCells,int);
  vtkBooleanMacro(IncludeExtrudedSurfaceCells,int);
  ///@}

  ///@{
  /**
   * Include/exclude the original (non-extruded) input surface cells in
   * the output unstructured grid.
   */
  vtkGetMacro(IncludeOriginalSurfaceCells,int);
  vtkSetMacro(IncludeOriginalSurfaceCells,int);
  vtkBooleanMacro(IncludeOriginalSurfaceCells,int);
  ///@}

  ///@{
  /**
   * Id assigned to the volumetric layer in the CellEntityIds array
   */
  vtkGetMacro(LayerEntityId,int);
  vtkSetMacro(LayerEntityId,int);
  ///@}

  ///@{
  /**
   * Entity Id of the first surface element of the layer.
   */
  vtkGetMacro(SurfaceEntityId,int);
  vtkSetMacro(SurfaceEntityId,int);
  ///@}
  
  ///@{
  /**
   * Entity Id of the first open profile of the layer
   */
  vtkGetMacro(OpenProfilesEntityId,int);
  vtkSetMacro(OpenProfilesEntityId,int);
  ///@}

  protected:
  vtkvmtkBoundaryLayerGenerator2();
  ~vtkvmtkBoundaryLayerGenerator2();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  // char *CellEntityIdsArrayName;
 
 //A per-vertex array indicating which vertices are situated on the open profiles of the surface.
 //Elements with value -1 are assumed to be on the inner surface
  char *OpenProfilesIdsArrayName;
  
  int IncludeExtrudedOpenProfilesCells;
  int IncludeExtrudedSurfaceCells;
  int IncludeOriginalSurfaceCells;

  int LayerEntityId;
  int SurfaceEntityId;
  int OpenProfilesEntityId;

  // Used to triangulate 3D cells
  vtkOrderedTriangulator *Triangulator;

  private:
  vtkvmtkBoundaryLayerGenerator2(const vtkvmtkBoundaryLayerGenerator2&);  // Not implemented.
  void operator=(const vtkvmtkBoundaryLayerGenerator2&);  // Not implemented.
};

#endif
