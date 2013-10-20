/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   vtkITK
  Module:    $HeadURL: http://www.na-mic.org/svn/Slicer3/trunk/Libs/vtkITK/vtkITKWin32Header.h $
  Date:      $Date: 2006-12-21 13:21:52 +0100 (Thu, 21 Dec 2006) $
  Version:   $Revision: 1900 $

==========================================================================*/

// .NAME vtkvmtkITKWin32Header - manage Windows system differences
// .SECTION Description
// The vtkITKWin32Header captures some system differences between Unix
// and Windows operating systems. 

#ifndef __vtkvmtkITKWin32Header_h
#define __vtkvmtkITKWin32Header_h

#include <vtkvmtkITKConfigure.h>

#if defined(WIN32) && !defined(VTK_VMTK_ITK_STATIC)
#if defined(vtkvmtkITK_EXPORTS)
#define VTK_VMTK_ITK_EXPORT __declspec( dllexport ) 
#else
#define VTK_VMTK_ITK_EXPORT __declspec( dllimport ) 
#endif
#else
#define VTK_VMTK_ITK_EXPORT
#endif

#endif
