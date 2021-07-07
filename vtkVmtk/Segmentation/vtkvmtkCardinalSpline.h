/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCardinalSpline.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.2 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

  Portions of this code are covered under the ITK copyright.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// .NAME vtkvmtkCardinalSpline - Implementation of vtkCardinalSpline containing methods to calculate the spline derivative, second derivatives, at derivative values.
// .SECTION Description
// ..


#ifndef __vtkvmtkCardinalSpline_h
#define __vtkvmtkCardinalSpline_h

#include "vtkCardinalSpline.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkCardinalSpline : public vtkCardinalSpline
{
public:
  static vtkvmtkCardinalSpline *New();

  vtkTypeMacro(vtkvmtkCardinalSpline,vtkCardinalSpline);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  virtual double EvaluateDerivative(double t);

  virtual double EvaluateSecondDerivative(double t);

  virtual void EvaluateValueAndDerivatives(double t, double valueAndDerivatives[3]);

protected:
  vtkvmtkCardinalSpline();
  ~vtkvmtkCardinalSpline() {}

private:
  vtkvmtkCardinalSpline(const vtkvmtkCardinalSpline&);  // Not implemented.
  void operator=(const vtkvmtkCardinalSpline&);  // Not implemented.
};

#endif

