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
 * @class   vtkvmtkCellDimensionFilter
 * @brief   Threshold all cells of a given dimension, output to an unstructured grid.
 * @ingroup Contrib
 *
 * vtkvmtkCellDimensionFilter selects the cells of the input data set whose
 * topological dimension (0 for vertices, 1 for lines, 2 for polygons, 3 for
 * solids) satisfies a threshold criterion set with ThresholdByLower(),
 * ThresholdByUpper(), or ThresholdBetween(), and outputs them as an
 * unstructured grid. It is a thin wrapper around vtkThreshold applied to
 * a cell-dimension array.
 */

#ifndef __vtkvmtkCellDimensionFilter_h
#define __vtkvmtkCellDimensionFilter_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class vtkUnstructuredGrid;
class vtkThreshold;


class VTK_VMTK_CONTRIB_EXPORT vtkvmtkCellDimensionFilter : public vtkUnstructuredGridAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkCellDimensionFilter,vtkUnstructuredGridAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkCellDimensionFilter* New();

  /**
   * Criterion is cells whose dimension is less or equal to lower threshold.
   */
  void ThresholdByLower(int lower);

  /**
   * Criterion is cells whose dimension is greater or equal to upper threshold.
   */
  void ThresholdByUpper(int upper);

  /**
   * Criterion is cells whose dimension is between lower and upper thresholds (inclusive of the end
   * values).
   */
  void ThresholdBetween(int lower, int upper);
  
  ///@{
  /**
   * Get the Upper and Lower thresholds.
   */
  virtual int GetUpperThreshold();
  virtual int GetLowerThreshold();
  ///@}

  protected:
  vtkvmtkCellDimensionFilter();
  ~vtkvmtkCellDimensionFilter();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  virtual int FillInputPortInformation(int port, vtkInformation *info) override;

  vtkThreshold *Threshold;
  
  private:
  vtkvmtkCellDimensionFilter(const vtkvmtkCellDimensionFilter&);  // Not implemented.
  void operator=(const vtkvmtkCellDimensionFilter&);  // Not implemented.
};

#endif
