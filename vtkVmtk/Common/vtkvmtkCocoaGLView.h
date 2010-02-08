/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkCocoaGLView.h,v $
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
// .NAME vtkvmtkCocoaGLView - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkCocoaGLView_h
#define __vtkvmtkCocoaGLView_h

#import <Cocoa/Cocoa.h>
#import "vtkCocoaGLView.h"

@interface vtkCocoaGLView(vtkvmtkCocoaGLView)

- (void)mouseDown:(NSEvent *)theEvent;

@end

#endif 
