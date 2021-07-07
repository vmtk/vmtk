/*=========================================================================

Program:   VMTK 
Module:    vtkvmtkInteractorStyleTrackballCamera
Language:  C++
Date:      $Date: 2006/07/17 09:52:56 $
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
// .NAME vtkvmtkInteractorStyleTrackballCamera - use the trackball camera style for 3d user ineteraction
// .SECTION Description
// ...

#ifndef __vtkvmtkInteractorStyleTrackballCamera_h
#define __vtkvmtkInteractorStyleTrackballCamera_h

#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_RENDERING_EXPORT vtkvmtkInteractorStyleTrackballCamera : public vtkInteractorStyleTrackballCamera
{
public:
  static vtkvmtkInteractorStyleTrackballCamera *New();

  vtkTypeMacro(vtkvmtkInteractorStyleTrackballCamera,vtkInteractorStyleTrackballCamera);
  //void PrintSelf(ostream& os, vtkIndent indent);

protected:

  vtkvmtkInteractorStyleTrackballCamera() {};
  ~vtkvmtkInteractorStyleTrackballCamera() {};

  virtual void OnChar() override;

private:
  vtkvmtkInteractorStyleTrackballCamera(const vtkvmtkInteractorStyleTrackballCamera&);  //Not implemented
  void operator=(const vtkvmtkInteractorStyleTrackballCamera&);  //Not implemented
};

#endif
