/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkCocoaGLView.mm,v $
  Language:  C++
  Date:      $Date: 2010/01/08 16:46:43 $
  Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#import "vtkvmtkCocoaGLView.h"
#import "vtkCocoaRenderWindow.h"
#import "vtkCocoaRenderWindowInteractor.h"
#import "vtkCommand.h"

@implementation vtkCocoaGLView(vtkvmtkCocoaGLView)

- (void)mouseDown:(NSEvent *)theEvent {

  vtkCocoaRenderWindowInteractor *interactor = [self getInteractor];
  if (!interactor) {
    return;
  }

  vtkCocoaRenderWindow* renWin = vtkCocoaRenderWindow::SafeDownCast([self getVTKRenderWindow]);
  if (!renWin) {
    return;
  }
 
  double factor = renWin->GetScaleFactor();

  BOOL keepOn = YES;

  NSPoint mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];

  int shiftDown = ([theEvent modifierFlags] & NSShiftKeyMask);
  int controlDown = ([theEvent modifierFlags] & NSControlKeyMask);

  int commandDown = ([theEvent modifierFlags] & NSCommandKeyMask);
  int optionDown = ([theEvent modifierFlags] & NSAlternateKeyMask);

  interactor->SetEventInformation((int)round(mouseLoc.x * factor), (int)round(mouseLoc.y * factor), controlDown, shiftDown);

  if (commandDown) {
    interactor->InvokeEvent(vtkCommand::RightButtonPressEvent,NULL);
  }
  else if (optionDown) {
    interactor->InvokeEvent(vtkCommand::MiddleButtonPressEvent,NULL);
  }
  else {
    interactor->InvokeEvent(vtkCommand::LeftButtonPressEvent,NULL);
  }
  [[self nextResponder] mouseDown:theEvent];

  NSDate* infinity = [NSDate distantFuture];
  do {
    theEvent =
      [NSApp nextEventMatchingMask: NSLeftMouseUpMask | NSLeftMouseDraggedMask
      untilDate: infinity
      inMode: NSEventTrackingRunLoopMode
      dequeue: YES];
    if (theEvent) {
      mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];

      interactor->SetEventInformation(
        (int)round(mouseLoc.x * factor), (int)round(mouseLoc.y * factor), controlDown, shiftDown);

      switch ([theEvent type]) {
        case NSLeftMouseDragged:
          interactor->InvokeEvent(vtkCommand::MouseMoveEvent, NULL);
          break;
        case NSLeftMouseUp:
          if (commandDown) {
            interactor->InvokeEvent(vtkCommand::RightButtonReleaseEvent, NULL);
          }
          else if (optionDown) {
            interactor->InvokeEvent(vtkCommand::MiddleButtonReleaseEvent, NULL);
          }
          else {
            interactor->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, NULL);
          }
          keepOn = NO;
        default:
        break;
      }
    }
    else {
      keepOn = NO;
    }
  }
  while (keepOn);
}

@end
