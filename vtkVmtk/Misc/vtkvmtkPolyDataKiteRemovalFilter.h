/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataKiteRemovalFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:48 $
Version:   $Revision: 1.2 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkPolyDataKiteRemovalFilter - removes small protrusions (kites) from a surface based on a SizeFactor parameter
// .SECTION Description
// ...

#ifndef __vtkvmtkPolyDataKiteRemovalFilter_h
#define __vtkvmtkPolyDataKiteRemovalFilter_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkPolyDataKiteRemovalFilter : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkPolyDataKiteRemovalFilter,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkPolyDataKiteRemovalFilter *New();
  
  vtkGetMacro(SizeFactor,double);
  vtkSetMacro(SizeFactor,double);

  protected:
  vtkvmtkPolyDataKiteRemovalFilter();
  ~vtkvmtkPolyDataKiteRemovalFilter();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  double SizeFactor;

  private:
  vtkvmtkPolyDataKiteRemovalFilter(const vtkvmtkPolyDataKiteRemovalFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataKiteRemovalFilter&);  // Not implemented.
};

#endif
