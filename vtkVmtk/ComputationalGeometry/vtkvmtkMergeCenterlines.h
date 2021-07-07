/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkMergeCenterlines.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkMergeCenterlines - Combine multiple centerlines which lie within the same branch of a split and grouped centerline.
// .SECTION Description
// If multiple centerlines exist within the same branch, each centerlines point locations are averaged (euclidian interpolation) to result in a branch with only one centerline.

#ifndef __vtkvmtkMergeCenterlines_h
#define __vtkvmtkMergeCenterlines_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkMergeCenterlines : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkMergeCenterlines,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

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

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

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
