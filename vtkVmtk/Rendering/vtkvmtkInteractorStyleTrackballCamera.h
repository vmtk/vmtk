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
 * @class   vtkvmtkInteractorStyleTrackballCamera
 * @brief   Use the trackball camera style for 3D user interaction.
 * @ingroup Rendering
 *
 * vtkvmtkInteractorStyleTrackballCamera specializes
 * vtkInteractorStyleTrackballCamera, overriding OnChar() with a no-op so
 * that none of VTK's default keyboard-character shortcuts (e.g. toggling
 * wireframe mode, resetting the camera, picking) fire. vmtk's rendering
 * windows use this to reserve the keyboard exclusively for their own
 * key bindings while still keeping trackball-style mouse camera control.
 */

#ifndef __vtkvmtkInteractorStyleTrackballCamera_h
#define __vtkvmtkInteractorStyleTrackballCamera_h

#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_RENDERING_EXPORT vtkvmtkInteractorStyleTrackballCamera : public vtkInteractorStyleTrackballCamera
{
public:
  static vtkvmtkInteractorStyleTrackballCamera *New();

  vtkTypeMacro(vtkvmtkInteractorStyleTrackballCamera,vtkInteractorStyleTrackballCamera);
  //void PrintSelf(std::ostream& os, vtkIndent indent);

protected:

  vtkvmtkInteractorStyleTrackballCamera() {};
  ~vtkvmtkInteractorStyleTrackballCamera() {};

  virtual void OnChar() override;

private:
  vtkvmtkInteractorStyleTrackballCamera(const vtkvmtkInteractorStyleTrackballCamera&);  //Not implemented
  void operator=(const vtkvmtkInteractorStyleTrackballCamera&);  //Not implemented
};

#endif
