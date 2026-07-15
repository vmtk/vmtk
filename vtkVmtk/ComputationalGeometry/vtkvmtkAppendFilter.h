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
 * @class   vtkvmtkAppendFilter
 * @brief   Merges points, point data, and cell data.
 * @ingroup ComputationalGeometry
 *
 * The RequestData method implementation is heavily based on the same method in vtkAppendFilter.
 * vtkAppendFilter is covered by the following (VTK) copyright notice.
 */

#ifndef __vtkvmtkAppendFilter_h
#define __vtkvmtkAppendFilter_h

#include "vtkAppendFilter.h"
#include "vtkvmtkWin32Header.h"

class vtkDataSetCollection;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkAppendFilter : public vtkAppendFilter
{
public:
  static vtkvmtkAppendFilter *New();

  vtkTypeMacro(vtkvmtkAppendFilter,vtkAppendFilter);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Toggle merging of duplicate (coincident) points across the appended inputs, in addition to the
   * point/point-data/cell-data merging already performed by the base vtkAppendFilter. Default: on.
   */
  vtkSetMacro(MergeDuplicatePoints,int);
  vtkGetMacro(MergeDuplicatePoints,int);
  vtkBooleanMacro(MergeDuplicatePoints,int);
  ///@}

protected:
  vtkvmtkAppendFilter();
  ~vtkvmtkAppendFilter();

  // Usual data generation method
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  int MergeDuplicatePoints;

private:
  vtkvmtkAppendFilter(const vtkvmtkAppendFilter&);  // Not implemented.
  void operator=(const vtkvmtkAppendFilter&);  // Not implemented.
};

#endif
