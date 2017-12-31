/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataKiteRemovalFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:48 $
Version:   $Revision: 1.2 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
  // .NAME vtkvmtkPolyDataKiteRemovalFilter - Add caps to boundaries.
  // .SECTION Description
  // This class closes the boundaries of a surface with a cap.

#ifndef __vtkvmtkPolyDataKiteRemovalFilter_h
#define __vtkvmtkPolyDataKiteRemovalFilter_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkPolyDataKiteRemovalFilter : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkPolyDataKiteRemovalFilter,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE; 

  static vtkvmtkPolyDataKiteRemovalFilter *New();
  
  vtkGetMacro(SizeFactor,double);
  vtkSetMacro(SizeFactor,double);

  protected:
  vtkvmtkPolyDataKiteRemovalFilter();
  ~vtkvmtkPolyDataKiteRemovalFilter();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;

  double SizeFactor;

  private:
  vtkvmtkPolyDataKiteRemovalFilter(const vtkvmtkPolyDataKiteRemovalFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataKiteRemovalFilter&);  // Not implemented.
};

#endif
