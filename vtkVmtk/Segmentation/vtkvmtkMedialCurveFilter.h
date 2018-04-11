/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkMedialCurveFilter.h,v $
Language:  C++
Date:      $Date: 2018/04/09 16:48:25 $
Version:   $Revision: 1.4 $

  Copyright (c) Richard Izzo, Luca Antiga. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

  Portions of this code are covered under the ITK copyright.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
  Note: this class was contributed by 
      Richard Izzo
      https://github.com/rlizzo

=========================================================================*/

// .NAME vtkvmtkMedialCurveFilter.h - Wrapper class around
// .SECTION Description
// vtkvmtkMedialCurveFilter.h


#ifndef __vtkvmtkMedialCurveFilter_h
#define __vtkvmtkMedialCurveFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkMedialCurveFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkMedialCurveFilter *New();
  vtkTypeMacro(vtkvmtkMedialCurveFilter, vtkSimpleImageToImageFilter);

  vtkGetMacro(Sigma,double);
  vtkSetMacro(Sigma,double);

  vtkGetMacro(Threshold,double);
  vtkSetMacro(Threshold,double);

protected:
  vtkvmtkMedialCurveFilter() {
    	this->Sigma = 0.5;
	    this->Threshold = 0.0;
  };
  ~vtkvmtkMedialCurveFilter() {};

  template< class TImage >
  void SimpleExecute(vtkImageData* input, vtkImageData* output);

private:
  vtkvmtkMedialCurveFilter(const vtkvmtkMedialCurveFilter&);  // Not implemented.
  void operator=(const vtkvmtkMedialCurveFilter&);  // Not implemented.

  double Sigma;
  double Threshold;
  
};

#endif