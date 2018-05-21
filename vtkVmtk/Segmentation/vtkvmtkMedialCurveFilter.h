/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkMedialCurveFilter.h,v $
Language:  C++
Date:      $Date: 2018/04/09 16:48:25 $
Version:   $Revision: 1.4 $

  Copyright (c) Richard Izzo, Luca Antiga. All rights reserved.
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
#include "vtkImageData.h"
#include "vtkvmtkWin32Header.h"

class vtkImageData;
class itkImage;

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkMedialCurveFilter : public vtkSimpleImageToImageFilter
{
  public:

    static vtkvmtkMedialCurveFilter *New();
    vtkTypeMacro(vtkvmtkMedialCurveFilter, vtkSimpleImageToImageFilter);
    // void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE; 

    vtkGetMacro(Sigma,double);
    vtkSetMacro(Sigma,double);

    vtkGetMacro(Threshold,double);
    vtkSetMacro(Threshold,double);

  protected:
    vtkvmtkMedialCurveFilter();
    ~vtkvmtkMedialCurveFilter();

    virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) VTK_OVERRIDE;

    double Sigma;
    double Threshold;

  private:
    vtkvmtkMedialCurveFilter(const vtkvmtkMedialCurveFilter&);  // Not implemented.
    void operator=(const vtkvmtkMedialCurveFilter&);  // Not implemented.
};
#endif