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
 * @class   vtkvmtkMeshVelocityStatistics
 * @brief   Calculates average and RMS velocity statistics.
 * @ingroup Misc
 *
 * Given several time steps of a velocity field stored as separate 3-component point data arrays on
 * the input mesh, computes the time-average and RMS (root-mean-square) velocity vectors at each
 * point. Typically used to summarize unsteady/pulsatile CFD simulation results into cycle-averaged
 * and turbulence-like statistics.
 */

#ifndef __vtkvmtkMeshVelocityStatistics_h
#define __vtkvmtkMeshVelocityStatistics_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkUnstructuredGrid.h"
#include "vtkvmtkWin32Header.h"

#include "vtkIdList.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkMeshVelocityStatistics : public vtkUnstructuredGridAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkMeshVelocityStatistics,vtkUnstructuredGridAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkMeshVelocityStatistics *New();

  ///@{
  /**
   * Set/Get the point data array indices (indices into the input's point data, as returned by
   * vtkPointData::GetArray(int), not array ids) of the per-time-step velocity arrays to average.
   * Required input; at least 2 entries are needed for RMS to be meaningful.
   */
  vtkSetObjectMacro(VelocityArrayIds,vtkIdList);
  vtkGetObjectMacro(VelocityArrayIds,vtkIdList);
  ///@}
  
  protected:
  vtkvmtkMeshVelocityStatistics();
  ~vtkvmtkMeshVelocityStatistics();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkIdList* VelocityArrayIds;

  private:
  vtkvmtkMeshVelocityStatistics(const vtkvmtkMeshVelocityStatistics&);  // Not implemented.
  void operator=(const vtkvmtkMeshVelocityStatistics&);  // Not implemented.
};

#endif
