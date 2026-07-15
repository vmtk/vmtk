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
 * @class   vtkvmtkSurfaceProjectCellArray
 * @brief   Projects a cell array from a reference surface.
 * @ingroup Contrib
 *
 * For each cell, the cell value chosen is the one of the cell on the reference surface which has the smallest minimum distance to the cell vertices. If the distance of any vertices in the cell to the reference surface is higher than DistanceTolerance, the cell array values are set to DefaultValue for each component.
 */

#ifndef __vtkvmtkSurfaceProjectCellArray_h
#define __vtkvmtkSurfaceProjectCellArray_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkvmtkWin32Header.h"

class vtkPolyData;

class VTK_VMTK_CONTRIB_EXPORT vtkvmtkSurfaceProjectCellArray : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkSurfaceProjectCellArray,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkSurfaceProjectCellArray *New();

  ///@{
  /**
   * Set/Get the reference surface to compute distance from.
   */
  vtkSetObjectMacro(ReferenceSurface,vtkPolyData);
  vtkGetObjectMacro(ReferenceSurface,vtkPolyData);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell array holding the projected values.
   * Commonly named "CellEntityIds".
   */
  vtkSetStringMacro(ProjectedArrayName);
  vtkGetStringMacro(ProjectedArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the maximum admissible distance between a cell of the input
   * surface and the closest cell of the reference surface. Cells farther
   * than this tolerance from the reference surface are assigned DefaultValue.
   */
  vtkSetMacro(DistanceTolerance, double);
  vtkGetMacro(DistanceTolerance, double);
  ///@}

  ///@{
  /**
   * Set/Get the value assigned to cells whose distance to the reference
   * surface exceeds DistanceTolerance.
   */
  vtkSetMacro(DefaultValue, double);
  vtkGetMacro(DefaultValue, double);
  ///@}

  protected:
  vtkvmtkSurfaceProjectCellArray();
  ~vtkvmtkSurfaceProjectCellArray();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkPolyData *ReferenceSurface;
  
  char *ProjectedArrayName;
  
  double DistanceTolerance;
  double DefaultValue;

  private:
  vtkvmtkSurfaceProjectCellArray(const vtkvmtkSurfaceProjectCellArray&);  // Not implemented.
  void operator=(const vtkvmtkSurfaceProjectCellArray&);  // Not implemented.
};

#endif
