/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   vtkvmtkITK
  Module:    $HeadURL: http://www.na-mic.org/svn/Slicer3/trunk/Libs/vtkITK/vtkITK.h $
  Date:      $Date: 2006-12-21 13:21:52 +0100 (Thu, 21 Dec 2006) $
  Version:   $Revision: 1900 $

==========================================================================*/
//
// use an ifdef on SLICER_VTK5 to flag code that won't
// compile on vtk4.4 and before
//
//#if ( (VTK_MAJOR_VERSION >= 5) || ( VTK_MAJOR_VERSION == 4 && VTK_MINOR_VERSION >= 5 ) )
//#define SLICER_VTK5
//#endif

#include "vtkvmtkWin32Header.h"
