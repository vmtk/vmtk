/*=========================================================================

Program:   VMTK

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
/**
 * @class   vtkvmtkMedialCurveFilter
 * @brief   Performs flux-driven automatic centerline extraction from a surface.
 * @ingroup Segmentation
 *
 * Please see http://www.insight-journal.org/browse/publication/165 for source implementation.
 *
 * Implementation of the flux driven automatic centerline extraction algorithm proposed by Bouix et al. in 2004. This is based on a skeletonisation algorithm initially proposed by Siddiqi et al. in 2002, using properties of an average outward flux measure to distinguish skeletal points from non-skeletal ones. This information is combined with a topology preserving thinning procedure to obtain the final result. This implementation combines this skeletonisation algorithm with other techniques as described in the paper of Bouix et al. to produce an ITK filter that generates as output the skeleton, as a binary object represented in an image, of the input surface, represented as a distance transform image. In this work is described this medial curve extraction procedure following the ITK philosophy.
 */

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
    // void PrintSelf(std::ostream& os, vtkIndent indent) override;

    ///@{
    /**
     * Set/get the standard deviation (in physical units) of the Gaussian kernel used to smooth the
     * input distance map before computing its gradient and average outward flux. A light amount of
     * smoothing improves the quality of the flux estimate driving the skeletonization. Default: 0.5.
     */
    vtkGetMacro(Sigma,double);
    vtkSetMacro(Sigma,double);
    ///@}

    ///@{
    /**
     * Set/get the average outward flux threshold below which a point is classified as a skeletal
     * (medial curve) point during the topology-preserving thinning procedure. Default: 0.0.
     */
    vtkGetMacro(Threshold,double);
    vtkSetMacro(Threshold,double);
    ///@}

  protected:
    vtkvmtkMedialCurveFilter();
    ~vtkvmtkMedialCurveFilter();

    virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

    double Sigma;
    double Threshold;

  private:
    vtkvmtkMedialCurveFilter(const vtkvmtkMedialCurveFilter&);  // Not implemented.
    void operator=(const vtkvmtkMedialCurveFilter&);  // Not implemented.
};
#endif