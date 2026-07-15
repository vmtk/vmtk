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
 * @class   vtkvmtkDanielssonDistanceMapImageFilter
 * @brief   Wraps itk::DanielssonDistanceMapImageFilter.
 * @ingroup Segmentation
 *
 * vtkvmtkDanielssonDistanceMapImageFilter computes, for every pixel of the (internally float-cast,
 * assumed 3D) input image, the Euclidean distance to the nearest non-zero pixel, using ITK's
 * Danielsson two-pass algorithm. Non-zero input pixels are treated as the boundary/seed set (the
 * distance transform is computed away from them); with InputIsBinary on, all non-zero input pixels
 * are treated as foreground with unit weight rather than being interpreted as distance-contributing
 * intensity values.
 *
 * @sa vtkvmtkFastMarchingUpwindGradientImageFilter
 */

#ifndef __vtkvmtkDanielssonDistanceMapImageFilter_h
#define __vtkvmtkDanielssonDistanceMapImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkDanielssonDistanceMapImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkDanielssonDistanceMapImageFilter *New();
  vtkTypeMacro(vtkvmtkDanielssonDistanceMapImageFilter, vtkSimpleImageToImageFilter);

  ///@{
  /**
   * Toggle output of squared Euclidean distances instead of Euclidean distances. Default: off.
   */
  vtkGetMacro(SquaredDistance,int);
  vtkSetMacro(SquaredDistance,int);
  vtkBooleanMacro(SquaredDistance,int);
  ///@}

  ///@{
  /**
   * Toggle treating the input as a binary mask, i.e. every non-zero pixel is foreground with equal
   * weight, rather than using pixel intensity values directly. Default: off.
   */
  vtkGetMacro(InputIsBinary,int);
  vtkSetMacro(InputIsBinary,int);
  vtkBooleanMacro(InputIsBinary,int);
  ///@}

protected:

  vtkvmtkDanielssonDistanceMapImageFilter();
  ~vtkvmtkDanielssonDistanceMapImageFilter() {};

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkDanielssonDistanceMapImageFilter(const vtkvmtkDanielssonDistanceMapImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkDanielssonDistanceMapImageFilter&);  // Not implemented.

  int SquaredDistance;
  int InputIsBinary;
};

#endif




