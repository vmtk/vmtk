/*=========================================================================

Program:   VMTK 
Module:    vtkvmtkImagePlaneWidget
Language:  C++
Date:      $Date: 2006/07/17 09:52:56 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
  // .NAME vtkvmtkImagePlaneWidget - ...
  // .SECTION Description
  // ...

#ifndef __vtkvmtkImagePlaneWidget_h
#define __vtkvmtkImagePlaneWidget_h

#include "vtkImagePlaneWidget.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_RENDERING_EXPORT vtkvmtkImagePlaneWidget : public vtkImagePlaneWidget
{
public:
  static vtkvmtkImagePlaneWidget *New();

  vtkTypeMacro(vtkvmtkImagePlaneWidget,vtkImagePlaneWidget);
  //void PrintSelf(ostream& os, vtkIndent indent);

protected:

  vtkvmtkImagePlaneWidget();
  ~vtkvmtkImagePlaneWidget() {};

  static void ProcessEvents(vtkObject* object, unsigned long event, void* clientdata, void* calldata);

  int LastDownShift;
  int LastDownControl;

private:
  vtkvmtkImagePlaneWidget(const vtkvmtkImagePlaneWidget&);  //Not implemented
  void operator=(const vtkvmtkImagePlaneWidget&);  //Not implemented
};

#endif
