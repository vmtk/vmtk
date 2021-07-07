/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkCellDimensionFilter.h,v $
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
// .NAME vtkvmtkCellDimensionFilter - Threshold all cells of a given dimension, output to an unstructured grid
// .SECTION Description
// ..

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
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkCellDimensionFilter* New();

  // Description:
  // Criterion is cells whose dimension is less or equal to lower threshold.
  void ThresholdByLower(int lower);

  // Description:
  // Criterion is cells whose dimension is greater or equal to upper threshold.
  void ThresholdByUpper(int upper);

  // Description:
  // Criterion is cells whose dimension is between lower and upper thresholds
  // (inclusive of the end values).
  void ThresholdBetween(int lower, int upper);
  
  // Description:
  // Get the Upper and Lower thresholds.
  virtual int GetUpperThreshold();
  virtual int GetLowerThreshold();

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
