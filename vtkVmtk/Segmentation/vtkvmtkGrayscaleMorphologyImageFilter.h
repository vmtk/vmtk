/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkGrayscaleMorphologyImageFilter.h,v $
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

// .NAME vtkvmtkGrayscaleMorphologyImageFilter - Wrapper class around itk::GrayscaleMorphologyImageFilter
// .SECTION Description
// vtkvmtkGrayscaleMorphologyImageFilter


#ifndef __vtkvmtkGrayscaleMorphologyImageFilter_h
#define __vtkvmtkGrayscaleMorphologyImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkGrayscaleMorphologyImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkGrayscaleMorphologyImageFilter *New();
  vtkTypeMacro(vtkvmtkGrayscaleMorphologyImageFilter, vtkSimpleImageToImageFilter);

  vtkGetVectorMacro(BallRadius,int,3);
  vtkSetVectorMacro(BallRadius,int,3);

  vtkSetMacro(Operation,int);
  vtkGetMacro(Operation,int);
  void SetOperationToDilate()
  { this->SetOperation(DILATE); }
  void SetOperationToErode()
  { this->SetOperation(ERODE); }
  void SetOperationToClose()
  { this->SetOperation(CLOSE); }
  void SetOperationToOpen()
  { this->SetOperation(OPEN); }

  enum
  {
    DILATE,
    ERODE,
    CLOSE,
    OPEN
  };

protected:
  vtkvmtkGrayscaleMorphologyImageFilter();
  ~vtkvmtkGrayscaleMorphologyImageFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;
  virtual int RequestInformation(vtkInformation * vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector) override;

  int BallRadius[3];
  int Operation;

private:
  vtkvmtkGrayscaleMorphologyImageFilter(const vtkvmtkGrayscaleMorphologyImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkGrayscaleMorphologyImageFilter&);  // Not implemented.
};

#endif




