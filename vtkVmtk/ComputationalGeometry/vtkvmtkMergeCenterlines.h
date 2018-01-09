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
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkMergeCenterlines : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkMergeCenterlines,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE; 

  static vtkvmtkMergeCenterlines *New();
  
  vtkSetStringMacro(RadiusArrayName);
  vtkGetStringMacro(RadiusArrayName);

  vtkSetStringMacro(GroupIdsArrayName);
  vtkGetStringMacro(GroupIdsArrayName);

  vtkSetStringMacro(CenterlineIdsArrayName);
  vtkGetStringMacro(CenterlineIdsArrayName);

  vtkSetStringMacro(TractIdsArrayName);
  vtkGetStringMacro(TractIdsArrayName);

  vtkSetStringMacro(BlankingArrayName);
  vtkGetStringMacro(BlankingArrayName);

  vtkSetMacro(ResamplingStepLength,double);
  vtkGetMacro(ResamplingStepLength,double);

  vtkSetMacro(MergeBlanked,int);
  vtkGetMacro(MergeBlanked,int);
  vtkBooleanMacro(MergeBlanked,int);

  protected:
  vtkvmtkMergeCenterlines();
  ~vtkvmtkMergeCenterlines();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;

  char* RadiusArrayName;
  char* GroupIdsArrayName;
  char* CenterlineIdsArrayName;
  char* TractIdsArrayName;
  char* BlankingArrayName;

  double ResamplingStepLength;
  int MergeBlanked;

  private:
  vtkvmtkMergeCenterlines(const vtkvmtkMergeCenterlines&);  // Not implemented.
  void operator=(const vtkvmtkMergeCenterlines&);  // Not implemented.
};

#endif
