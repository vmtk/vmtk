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
 * @class   vtkvmtkImagePlaneWidget
 * @brief   Generates an interactive widget used to browse through image data.
 * @ingroup Rendering
 *
 * vtkvmtkImagePlaneWidget specializes vtkImagePlaneWidget, overriding its
 * event-processing callback so that image slices can be stepped through
 * (and the widget's cursor-driven controls modified) using Shift/Control
 * qualified interactions in addition to the base class behavior. It is
 * used by vmtk's interactive rendering windows to let the user scroll
 * through an image plane while a surface or centerline is displayed.
 */

#ifndef __vtkvmtkImagePlaneWidget_h
#define __vtkvmtkImagePlaneWidget_h

#include "vtkImagePlaneWidget.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_RENDERING_EXPORT vtkvmtkImagePlaneWidget : public vtkImagePlaneWidget
{
public:
  static vtkvmtkImagePlaneWidget *New();

  vtkTypeMacro(vtkvmtkImagePlaneWidget,vtkImagePlaneWidget);
  //void PrintSelf(std::ostream& os, vtkIndent indent);

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
