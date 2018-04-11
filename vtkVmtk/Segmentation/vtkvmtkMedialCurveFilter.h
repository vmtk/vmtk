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

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class vtkImageData;

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkMedialCurveFilter : public vtkPolyDataAlgorithm
{
  public:
    vtkTypeMacro(vtkvmtkMedialCurveFilter, vtkPolyDataAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE; 

    static vtkvmtkMedialCurveFilter *New();

    virtual void SetInputImage(vtkImageData *);
    vtkGetObjectMacro(InputImage, vtkImageData);

    virtual void SetOutputImage(vtkImageData *);
    vtkGetObjectMacro(OutputImage, vtkImageData);
  
    vtkGetMacro(Sigma,double);
    vtkSetMacro(Sigma,double);

    vtkGetMacro(Threshold,double);
    vtkSetMacro(Threshold,double);

  protected:
    vtkvmtkMedialCurveFilter();
    ~vtkvmtkMedialCurveFilter();

    template< class TImage >
    void ExecuteCalculation();

    vtkImageData *InputImage;
    vtkImageData *OutputImage;
    double Sigma;
    double Threshold;

  private:
    vtkvmtkMedialCurveFilter(const vtkvmtkMedialCurveFilter&);  // Not implemented.
    void operator=(const vtkvmtkMedialCurveFilter&);  // Not implemented.
};
#endif