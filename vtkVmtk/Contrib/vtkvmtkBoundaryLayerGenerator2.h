/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkBoundaryLayerGenerator2.h,v $
  Language:  C++
  Date:      $$
  Version:   $$

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
// .NAME vtkvmtkBoundaryLayerGenerator2 - Extended version of vtkvmtkBoundaryLayerGenerator 
// .SECTION Description
// This version allows finer control on the surfaces to be included. If the original surface has open profiles, a surface can be extracted from them as well. The points on the open profiles may be specified with an array (OpenProfilesIdsArrayName). A value of -1 in this array specifies a point on the interior of the surface. Cell entity ids can also be output for the volume and surface elements. Surface elements are numbered in increasing numbers, using the openProfilesIds if specified.

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
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkBoundaryLayerGenerator2 *New();

  // vtkGetStringMacro(CellEntityIdsArrayName);
  // vtkSetStringMacro(CellEntityIdsArrayName);
  
  vtkGetStringMacro(OpenProfilesIdsArrayName);
  vtkSetStringMacro(OpenProfilesIdsArrayName);

  //Description:
  //Include/Exclude the different surfaces into the resulting unstructures grud
  //If IncludeSurfaceCells is set to 1 all of the surfaces are included
  vtkGetMacro(IncludeExtrudedOpenProfilesCells,int);
  vtkSetMacro(IncludeExtrudedOpenProfilesCells,int);
  vtkBooleanMacro(IncludeExtrudedOpenProfilesCells,int);
  
  vtkGetMacro(IncludeExtrudedSurfaceCells,int);
  vtkSetMacro(IncludeExtrudedSurfaceCells,int);
  vtkBooleanMacro(IncludeExtrudedSurfaceCells,int);
  
  vtkGetMacro(IncludeOriginalSurfaceCells,int);
  vtkSetMacro(IncludeOriginalSurfaceCells,int);
  vtkBooleanMacro(IncludeOriginalSurfaceCells,int);

  //Description:
  //Id assigned to the volumetric layer in the CellEntityIds array
  vtkGetMacro(LayerEntityId,int);
  vtkSetMacro(LayerEntityId,int);

  //Description:
  //Entity Id of the first surface element of the layer.
  vtkGetMacro(SurfaceEntityId,int);
  vtkSetMacro(SurfaceEntityId,int);
  
  //Description
  //Entity Id of the first open profile of the layer
  vtkGetMacro(OpenProfilesEntityId,int);
  vtkSetMacro(OpenProfilesEntityId,int);

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
