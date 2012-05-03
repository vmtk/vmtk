/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkvmtkWin32Header.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkWin32Header - manage Windows system differences
// .SECTION Description
// The vtkWin32Header captures some system differences between Unix and
// Windows operating systems. 

#ifndef __vtkvmtkWIN32Header_h
#define __vtkvmtkWIN32Header_h

//#ifndef __VTK_SYSTEM_INCLUDES__INSIDE
//Do_not_include_vtkWin32Header_directly__vtkSystemIncludes_includes_it;
//#endif

#include <vtkvmtkConfigure.h>

//
// Windows specific stuff------------------------------------------
#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)

// define strict header for windows
#ifndef STRICT
#define STRICT
#endif

#ifdef VTK_USE_ANSI_STDLIB
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#endif

// Never include the windows header here when building VTK itself.
#if defined(VTK_IN_VTK)
# undef VTK_INCLUDE_WINDOWS_H
#endif

#if defined(_WIN32)
  // Include the windows header here only if requested by user code.
# if defined(VTK_INCLUDE_WINDOWS_H)
#  include <windows.h>
   // Define types from the windows header file.
   typedef DWORD vtkWindowsDWORD;
   typedef PVOID vtkWindowsPVOID;
   typedef LPVOID vtkWindowsLPVOID;
   typedef HANDLE vtkWindowsHANDLE;
   typedef LPTHREAD_START_ROUTINE vtkWindowsLPTHREAD_START_ROUTINE;
# else
   // Define types from the windows header file.
   typedef unsigned long vtkWindowsDWORD;
   typedef void* vtkWindowsPVOID;
   typedef vtkWindowsPVOID vtkWindowsLPVOID;
   typedef vtkWindowsPVOID vtkWindowsHANDLE;
   typedef vtkWindowsDWORD (__stdcall *vtkWindowsLPTHREAD_START_ROUTINE)(vtkWindowsLPVOID);
# endif
  // Enable workaround for windows header name mangling.
  // See VTK/Utilities/Upgrading/README.WindowsMangling.txt for details.
# define VTK_WORKAROUND_WINDOWS_MANGLE
#endif

#if defined(_MSC_VER)
  // Enable MSVC compiler warning messages that are useful but off by default.
# pragma warning ( default : 4263 ) /* no override, call convention differs */
  // Disable MSVC compiler warning messages that often occur in valid code.
# if !defined(VTK_DISPLAY_WIN32_WARNINGS)
#  pragma warning ( disable : 4097 ) /* typedef is synonym for class */
#  pragma warning ( disable : 4127 ) /* conditional expression is constant */
#  pragma warning ( disable : 4244 ) /* possible loss in conversion */
#  pragma warning ( disable : 4251 ) /* missing DLL-interface */
#  pragma warning ( disable : 4305 ) /* truncation from type1 to type2 */
#  pragma warning ( disable : 4309 ) /* truncation of constant value */
#  pragma warning ( disable : 4514 ) /* unreferenced inline function */
#  pragma warning ( disable : 4706 ) /* assignment in conditional expression */
#  pragma warning ( disable : 4710 ) /* function not inlined */
#  pragma warning ( disable : 4786 ) /* identifier truncated in debug info */
# endif
#endif

// MSVC 6.0 in release mode will warn about code it produces with its
// optimizer.  Disable the warnings specifically for this
// configuration.  Real warnings will be revealed by a debug build or
// by other compilers.
#if defined(_MSC_VER) && (_MSC_VER < 1300) && defined(NDEBUG)
# pragma warning ( disable : 4701 ) /* Variable may be used uninitialized.  */
# pragma warning ( disable : 4702 ) /* Unreachable code.  */
#endif

#if defined(WIN32) && defined(VTK_VMTK_BUILD_SHARED_LIBS)
// #define VTK_EXPORT __declspec( dllexport )

 #if defined(vtkvmtkCommon_EXPORTS)
  #define VTK_VMTK_COMMON_EXPORT __declspec( dllexport ) 
 #else
  #define VTK_VMTK_COMMON_EXPORT __declspec( dllimport ) 
 #endif

 #if defined(vtkvmtkComputationalGeometry_EXPORTS)
  #define VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT __declspec( dllexport ) 
 #else
  #define VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT __declspec( dllimport ) 
 #endif

 #if defined(vtkvmtkDifferentialGeometry_EXPORTS)
  #define VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT __declspec( dllexport ) 
 #else
  #define VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT __declspec( dllimport ) 
 #endif

 #if defined(vtkvmtkIO_EXPORTS)
  #define VTK_VMTK_IO_EXPORT __declspec( dllexport ) 
 #else
  #define VTK_VMTK_IO_EXPORT __declspec( dllimport ) 
 #endif

 #if defined(vtkvmtkMisc_EXPORTS)
  #define VTK_VMTK_MISC_EXPORT __declspec( dllexport ) 
 #else
  #define VTK_VMTK_MISC_EXPORT __declspec( dllimport ) 
 #endif

// #if defined(vtkvmtkITK_EXPORTS)
  #define VTK_VMTK_ITK_EXPORT __declspec( dllexport ) 
// #else
//  #define VTK_ITK_EXPORT __declspec( dllimport ) 
// #endif

 #if defined(vtkvmtkSegmentation_EXPORTS)
  #define VTK_VMTK_SEGMENTATION_EXPORT __declspec( dllexport ) 
 #else
  #define VTK_VMTK_SEGMENTATION_EXPORT __declspec( dllimport ) 
 #endif

 #if defined(vtkvmtkContrib_EXPORTS)
  #define VTK_VMTK_CONTRIB_EXPORT __declspec( dllexport ) 
 #else
  #define VTK_VMTK_CONTRIB_EXPORT __declspec( dllimport ) 
 #endif

 #if defined(vtkvmtkRendering_EXPORTS)
  #define VTK_VMTK_RENDERING_EXPORT __declspec( dllexport ) 
 #else
  #define VTK_VMTK_RENDERING_EXPORT __declspec( dllimport ) 
 #endif

#else
 #define VTK_VMTK_COMMON_EXPORT
 #define VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT
 #define VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT
 #define VTK_VMTK_IO_EXPORT
 #define VTK_VMTK_MISC_EXPORT
 #define VTK_VMTK_ITK_EXPORT
 #define VTK_VMTK_SEGMENTATION_EXPORT
 #define VTK_VMTK_CONTRIB_EXPORT
 #define VTK_VMTK_RENDERING_EXPORT
#endif

// this is exclusively for the tcl Init functions
#if defined(WIN32)
// #define VTK_TK_EXPORT __declspec( dllexport )
#else
// #define VTK_TK_EXPORT
#endif

#endif
