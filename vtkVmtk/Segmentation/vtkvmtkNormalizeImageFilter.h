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
 * @class   vtkvmtkNormalizeImageFilter
 * @brief   Wraps itk::NormalizeImageFilter.
 * @ingroup Segmentation
 *
 * vtkvmtkNormalizeImageFilter rescales the intensities of the input image so that the resulting
 * image has zero mean and unit variance, using the image's statistical mean and standard deviation
 * (as opposed to a fixed intensity range as in min/max rescaling). It is the filter behind the
 * vmtkimagenormalize pype script, used to bring images from different scanners/protocols to a
 * comparable intensity scale before further processing (e.g. before feature extraction or level
 * set segmentation). Like the other single-purpose ITK wrappers in this module, it is a thin
 * vtkSimpleImageToImageFilter: SimpleExecute() converts the VTK input to a float itk::Image, runs
 * itk::NormalizeImageFilter, and converts the result back to vtkImageData. Has no configurable
 * parameters beyond the input image itself.
 */

#ifndef __vtkvmtkNormalizeImageFilter_h
#define __vtkvmtkNormalizeImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class vtkFloatArray;

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkNormalizeImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkNormalizeImageFilter *New();
  vtkTypeMacro(vtkvmtkNormalizeImageFilter, vtkSimpleImageToImageFilter);

protected:
  vtkvmtkNormalizeImageFilter();
  ~vtkvmtkNormalizeImageFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkNormalizeImageFilter(const vtkvmtkNormalizeImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkNormalizeImageFilter&);  // Not implemented.
};

#endif

