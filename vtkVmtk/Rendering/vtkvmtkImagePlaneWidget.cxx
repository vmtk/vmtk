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
#include "vtkvmtkImagePlaneWidget.h"
#include "vtkObjectFactory.h"

#include "vtkRenderWindowInteractor.h"
#include "vtkCallbackCommand.h"
#include "vtkCommand.h"

vtkStandardNewMacro(vtkvmtkImagePlaneWidget);

vtkvmtkImagePlaneWidget::vtkvmtkImagePlaneWidget()
{
  this->EventCallbackCommand->SetCallback(vtkvmtkImagePlaneWidget::ProcessEvents);
  this->LastDownShift = 0;
  this->LastDownControl = 0;
}

void vtkvmtkImagePlaneWidget::ProcessEvents(vtkObject* vtkNotUsed(object),
    unsigned long event,
    void* clientdata,
    void* vtkNotUsed(calldata))
{
  vtkvmtkImagePlaneWidget* self =
    reinterpret_cast<vtkvmtkImagePlaneWidget *>( clientdata );

  self->LastButtonPressed = vtkvmtkImagePlaneWidget::VTK_NO_BUTTON;

  switch ( event )
  {
    case vtkCommand::LeftButtonPressEvent:
      self->LastButtonPressed = vtkvmtkImagePlaneWidget::VTK_LEFT_BUTTON;
      self->LastDownShift = self->Interactor->GetShiftKey();
      self->LastDownControl = self->Interactor->GetControlKey();
      if (self->LastDownShift && self->LastDownControl)
      {
        self->Interactor->SetControlKey(0);
        self->Interactor->SetShiftKey(0);
        self->OnRightButtonDown();
        self->Interactor->SetControlKey(1);
        self->Interactor->SetShiftKey(1);
      }
      else if (self->LastDownShift)
      {
        self->Interactor->SetShiftKey(0);
        self->OnMiddleButtonDown();
        self->Interactor->SetShiftKey(1);
      }
      else
      {
        self->OnLeftButtonDown();
      }
      break;
    case vtkCommand::LeftButtonReleaseEvent:
      self->LastButtonPressed = vtkvmtkImagePlaneWidget::VTK_LEFT_BUTTON;
      if (self->LastDownShift)
      {
        self->OnMiddleButtonUp();
      }
      else if (self->LastDownShift && self->LastDownControl)
      {
        self->OnRightButtonUp();
      }
      else
      {
        self->OnLeftButtonUp();
      }
      break;
    case vtkCommand::MiddleButtonPressEvent:
      self->LastButtonPressed = vtkvmtkImagePlaneWidget::VTK_MIDDLE_BUTTON;
      self->OnMiddleButtonDown();
      break;
    case vtkCommand::MiddleButtonReleaseEvent:
      self->LastButtonPressed = vtkvmtkImagePlaneWidget::VTK_MIDDLE_BUTTON;
      self->OnMiddleButtonUp();
      break;
    case vtkCommand::RightButtonPressEvent:
      self->LastButtonPressed = vtkvmtkImagePlaneWidget::VTK_RIGHT_BUTTON;
      self->OnRightButtonDown();
      break;
    case vtkCommand::RightButtonReleaseEvent:
      self->LastButtonPressed = vtkvmtkImagePlaneWidget::VTK_RIGHT_BUTTON;
      self->OnRightButtonUp();
      break;
    case vtkCommand::MouseMoveEvent:
      self->OnMouseMove();
      break;
    case vtkCommand::CharEvent:
      self->OnChar();
      break;
  }
}

