/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: vtkvmtkCocoaRenderWindowInteractor.h,v $

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkvmtkCocoaRenderWindowInteractor - Cocoa OpenGL rendering window
//
// .SECTION Description
// vtkvmtkCocoaRenderWindowInteractor is a concrete implementation of the abstract
// class vtkCocoaRenderWindowInteractor. It is only available on Mac OS X 10.3
// and later.
// To use this class, build VTK with VTK_USE_COCOA turned ON.
// This class can be used by 32 and 64 bit processes, and either in
// garbage collected or reference counted modes.
// vtkvmtkCocoaRenderWindowInteractor uses Objective-C++, and the OpenGL and
// Cocoa APIs. This class's default behaviour is to create an NSWindow and
// a vtkCocoaGLView which are used together to draw all vtk stuff into.
// If you already have an NSWindow and vtkCocoaGLView and you want this
// class to use them you must call both SetRootWindow() and SetWindowId(),
// respectively, early on (before WindowInitialize() is executed).
//
// .SECTION See Also
// vtkCocoaRenderWindowInteractor vtkCocoaGLView

// .SECTION Warning
// This header must be in C++ only because it is included by .cxx files.
// That means no Objective-C may be used. That's why some instance variables 
// are void* instead of what they really should be.

#ifndef __vtkvmtkCocoaRenderWindowInteractor_h
#define __vtkvmtkCocoaRenderWindowInteractor_h

#include "vtkCocoaRenderWindowInteractor.h"

class VTK_RENDERING_EXPORT vtkvmtkCocoaRenderWindowInteractor : public vtkCocoaRenderWindowInteractor
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
