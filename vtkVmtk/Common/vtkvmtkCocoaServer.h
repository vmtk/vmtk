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
// .NAME vtkvmtkCocoaServer - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkCocoaServer_h
#define __vtkvmtkCocoaServer_h

#import <Cocoa/Cocoa.h>
#import "vtkCocoaRenderWindow.h"

@interface vtkCocoaServer : NSObject
{
  vtkCocoaRenderWindow* renWin;
}

+ (id)cocoaServerWithRenderWindow:(vtkCocoaRenderWindow *)inRenderWindow;

- (void)start;
- (void)stop;

@end

@interface vtkCocoaServer(vtkvmtkCocoaServer)

- (void)start;
- (void)stop;

@end

#endif 
