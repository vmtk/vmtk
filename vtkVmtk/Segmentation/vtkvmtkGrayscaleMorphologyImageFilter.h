/*=========================================================================

Program:   VMTK

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

/**
 * @class   vtkvmtkGrayscaleMorphologyImageFilter
 * @brief   Wraps itk::GrayscaleMorphologyImageFilter.
 * @ingroup Segmentation
 *
 * Applies a grayscale morphological operation (dilate, erode, or their compositions close/open)
 * to the input image using a ball-shaped structuring element of radius BallRadius (in voxels, one
 * value per axis). Used e.g. to clean up or fill small gaps in a segmented/thresholded image.
 */

#ifndef __vtkvmtkGrayscaleMorphologyImageFilter_h
#define __vtkvmtkGrayscaleMorphologyImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkGrayscaleMorphologyImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkGrayscaleMorphologyImageFilter *New();
  vtkTypeMacro(vtkvmtkGrayscaleMorphologyImageFilter, vtkSimpleImageToImageFilter);

  ///@{
  /**
   * Set/Get the radius, in voxels (one value per axis), of the ball-shaped structuring element used
   * for the morphological operation. Default: (1,1,1).
   */
  vtkGetVectorMacro(BallRadius,int,3);
  vtkSetVectorMacro(BallRadius,int,3);
  ///@}

  ///@{
  /**
   * Set/Get the morphological operation to apply: DILATE, ERODE (default), CLOSE (dilate then
   * erode, fills small gaps/holes), or OPEN (erode then dilate, removes small protrusions). See also
   * SetOperationToDilate / SetOperationToErode / SetOperationToClose / SetOperationToOpen.
   */
  vtkSetMacro(Operation,int);
  vtkGetMacro(Operation,int);
  ///@}
  /**
   * Convenience method: set Operation to DILATE.
   */
  void SetOperationToDilate()
  { this->SetOperation(DILATE); }
  /**
   * Convenience method: set Operation to ERODE (default).
   */
  void SetOperationToErode()
  { this->SetOperation(ERODE); }
  /**
   * Convenience method: set Operation to CLOSE (dilate then erode).
   */
  void SetOperationToClose()
  { this->SetOperation(CLOSE); }
  /**
   * Convenience method: set Operation to OPEN (erode then dilate).
   */
  void SetOperationToOpen()
  { this->SetOperation(OPEN); }

  /**
   * Values for Operation.
   */
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




