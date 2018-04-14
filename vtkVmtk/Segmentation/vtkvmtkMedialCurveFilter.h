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
#include "vtkImageAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class vtkImageData;
class vtkPolyData;
class itkImage;

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkMedialCurveFilter : public vtkPolyDataAlgorithm
{
  public:
    vtkTypeMacro(vtkvmtkMedialCurveFilter, vtkPolyDataAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE; 

    static vtkvmtkMedialCurveFilter *New();

    virtual void SetInputSurface(vtkPolyData *);
    vtkGetObjectMacro(InputSurface, vtkPolyData);

    virtual void SetOutputImage(vtkImageData *);
    vtkGetObjectMacro(OutputImage, vtkImageData);

    vtkGetMacro(Sigma,double);
    vtkSetMacro(Sigma,double);

    vtkGetMacro(Threshold,double);
    vtkSetMacro(Threshold,double);

    vtkGetMacro(PolyDataToImageDataSpacingX,double);
    vtkSetMacro(PolyDataToImageDataSpacingX,double);
  
    vtkGetMacro(PolyDataToImageDataSpacingY,double)
    vtkSetMacro(PolyDataToImageDataSpacingY,double)

    vtkGetMacro(PolyDataToImageDataSpacingZ,double)
    vtkSetMacro(PolyDataToImageDataSpacingZ,double)

  protected:
    vtkvmtkMedialCurveFilter();
    ~vtkvmtkMedialCurveFilter();

    virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;

    vtkPolyData *InputSurface;
    vtkImageData *OutputImage;
    vtkImageData *BinaryImage;
    vtkImageData *DistanceImage;
    double Sigma;
    double Threshold;
    double PolyDataToImageDataSpacingX;
    double PolyDataToImageDataSpacingY;
    double PolyDataToImageDataSpacingZ;

  private:
    vtkvmtkMedialCurveFilter(const vtkvmtkMedialCurveFilter&);  // Not implemented.
    void operator=(const vtkvmtkMedialCurveFilter&);  // Not implemented.
};
#endif