/*=========================================================================

Program:   VMTK 
Module:    vtkvmtkITKVersion
Language:  C++
Date:      $Date: 2021/12/16 $
Version:   $Revision: 1.5 $

  Copyright (c) Kurt Sansom, Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __vtkvmtkITKVersion_h
#define __vtkvmtkITKVersion_h

#include "vtkvmtkITK.h"
#include "vtkCommonCoreModule.h" // For export macro
#include "vtkObject.h"
#include "itkVersion.h" // For export macro


class VTK_VMTK_ITK_EXPORT vtkvmtkITKVersion : public vtkObject
{
public:
  static vtkvmtkITKVersion* New();
  vtkTypeMacro(vtkvmtkITKVersion, vtkObject);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  /**
   * Return the version of itk this object is a part of.
   * A variety of methods are included. GetITKSourceVersion returns a string
   * with an identifier which timestamps a particular source tree.
   */
  static const char* GetITKVersion() { return itk::Version::GetITKVersion(); }
  static int GetITKMajorVersion() { return itk::Version::GetITKMajorVersion(); }
  static int GetITKMinorVersion() { return itk::Version::GetITKMinorVersion(); }
  static int GetITKBuildVersion() { return itk::Version::GetITKBuildVersion(); }
  static const char* GetITKSourceVersion() { return itk::Version::GetITKSourceVersion(); }

protected:
  vtkvmtkITKVersion() = default; // ensure constructor/destructor protected
  ~vtkvmtkITKVersion() override = default;

private:
  vtkvmtkITKVersion(const vtkvmtkITKVersion&) = delete;
  void operator=(const vtkvmtkITKVersion&) = delete;
};

#endif
