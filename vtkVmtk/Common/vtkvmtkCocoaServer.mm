/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkCocoaServer.h,v $
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

#import "vtkvmtkCocoaServer.h"
#import "vtkCocoaRenderWindow.h"

@implementation vtkCocoaServer(vtkvmtkCocoaServer)

- (void)start
{
  NSWindow *win = nil;
  if (renWin != NULL)
    {
    win = reinterpret_cast<NSWindow *>(renWin->GetRootWindow());
    if (win != nil)
      {
      NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];
      [nc addObserver:self selector:@selector(windowWillClose:)
                               name:@"NSWindowWillCloseNotification"
                             object:win];
      }
    }

  [NSApp activateIgnoringOtherApps:YES];
  [NSApp run];
}

- (void)stop {
  NSWindow  *win = nil;
  if (renWin != NULL)
    {
    win = reinterpret_cast<NSWindow *>(renWin->GetRootWindow());
    }
 
  [NSApp stop:NSApp];
}

@end
