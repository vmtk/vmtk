/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkMergeCenterlines.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
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
  // .NAME vtkvmtkMergeCenterlines - Merge centerlines.
  // .SECTION Description
  // ...

#ifndef __vtkvmtkMergeCenterlines_h
#define __vtkvmtkMergeCenterlines_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

  // TODO: this class needs to be rewritten or removed

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkMergeCenterlines : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeRevisionMacro(vtkvmtkMergeCenterlines,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent); 

  static vtkvmtkMergeCenterlines *New();
  
  vtkSetObjectMacro(CenterlinesToMerge,vtkPolyData);
  vtkGetObjectMacro(CenterlinesToMerge,vtkPolyData);

  protected:
  vtkvmtkMergeCenterlines();
  ~vtkvmtkMergeCenterlines();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  vtkPolyData* CenterlinesToMerge;

  private:
  vtkvmtkMergeCenterlines(const vtkvmtkMergeCenterlines&);  // Not implemented.
  void operator=(const vtkvmtkMergeCenterlines&);  // Not implemented.
};

#endif
