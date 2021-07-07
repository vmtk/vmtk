/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkMeshVelocityStatistics.h,v $
Language:  C++
Date:      $Date: 2006/07/27 08:28:36 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkMeshVelocityStatistics - calculates average and RMS velocity statistics
// .SECTION Description
// .

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
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkMeshVelocityStatistics *New();

  vtkSetObjectMacro(VelocityArrayIds,vtkIdList);
  vtkGetObjectMacro(VelocityArrayIds,vtkIdList);
  
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
