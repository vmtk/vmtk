/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkCocoaRenderWindowInteractor.h,v $
  Language:  C++
  Date:      $Date: 2010/05/05 16:46:43 $
  Version:   $Revision: 1.0 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkCocoaRenderWindowInteractor - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkCocoaRenderWindowInteractor_h
#define __vtkvmtkCocoaRenderWindowInteractor_h

#include "vtkCocoaRenderWindowInteractor.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMMON_EXPORT vtkvmtkCocoaRenderWindowInteractor : public vtkCocoaRenderWindowInteractor
{
public:
  static vtkvmtkCocoaRenderWindowInteractor *New();
  vtkTypeRevisionMacro(vtkvmtkCocoaRenderWindowInteractor,vtkCocoaRenderWindowInteractor);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void TerminateApp();
  virtual void Close();

  vtkSetMacro(CloseWindowOnTerminateApp,int);
  vtkGetMacro(CloseWindowOnTerminateApp,int);
  vtkBooleanMacro(CloseWindowOnTerminateApp,int);

protected:
  vtkvmtkCocoaRenderWindowInteractor();
  ~vtkvmtkCocoaRenderWindowInteractor();

  int CloseWindowOnTerminateApp;

private:
  vtkvmtkCocoaRenderWindowInteractor(const vtkvmtkCocoaRenderWindowInteractor&);  // Not implemented.
  void operator=(const vtkvmtkCocoaRenderWindowInteractor&);  // Not implemented.
};

#endif
