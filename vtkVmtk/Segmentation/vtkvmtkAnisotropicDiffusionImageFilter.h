/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkAnisotropicDiffusionImageFilter.h,v $
Language:  C++
Date:      $Date: 2018/03/05 16:48:25 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga. All rights reserved.
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
      Kurt Sansom
      https://github.com/kayarre

=========================================================================*/
// .NAME vtkvmtkAnisotropicDiffusionImageFilter - Wrapper class around itk::GradientAnisotropicDiffusionImageFilter
// .SECTION Description
// vtkvmtkAnisotropicDiffusionImageFilter


#ifndef __vtkvmtkAnisotropicDiffusionImageFilter_h
#define __vtkvmtkAnisotropicDiffusionImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkAnisotropicDiffusionImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkAnisotropicDiffusionImageFilter *New();
  vtkTypeMacro(vtkvmtkAnisotropicDiffusionImageFilter, vtkSimpleImageToImageFilter);

  vtkGetMacro(NumberOfIterations,int);
  vtkSetMacro(NumberOfIterations,int);

  vtkGetMacro(TimeStep,double);
  vtkSetMacro(TimeStep,double);

  vtkGetMacro(Conductance,double);
  vtkSetMacro(Conductance,double);


protected:

  vtkvmtkAnisotropicDiffusionImageFilter();
  ~vtkvmtkAnisotropicDiffusionImageFilter() {};

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  template< class TImage >
  int FilterImage( vtkImageData*, vtkImageData*, typename TImage::Pointer );
  
  template< int VDimension >
  int FilterScalarImage( vtkImageData* , vtkImageData* , int );
  
  vtkvmtkAnisotropicDiffusionImageFilter(const vtkvmtkAnisotropicDiffusionImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkAnisotropicDiffusionImageFilter&);  // Not implemented.

  int NumberOfIterations;
  double TimeStep;
  double Conductance;
  
};

#endif

