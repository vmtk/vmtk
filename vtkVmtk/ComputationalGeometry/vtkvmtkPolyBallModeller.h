/*=========================================================================

Program:   VMTK

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/**
 * @class   vtkvmtkPolyBallModeller
 * @brief   Create an image where a polyball or polyball line is evaluated as a function.
 * @ingroup ComputationalGeometry
 *
 * This creates an image which might look similar to a level set (0 at surface boundaries negative inside, positive outside), but it much more powerful. It is a finite approximation of the entire implicit sphere function solution within the bounds of the image volume.
 *
 * Rasterizes the vtkvmtkPolyBallLine (or, if UsePolyBallLine is off, vtkvmtkPolyBall) tube function
 * of the input centerlines into a regular image, sampled on a SampleDimensions grid over
 * ModelBounds. This is the filter behind the vmtkcenterlinemodeller pype script; the resulting
 * image is typically thresholded at zero to reconstruct a tubular surface, or used as a smooth
 * distance-like field for other level-set operations.
 *
 * @sa
 * vtkvmtkPolyBallLine, vtkvmtkPolyBall
 */

#ifndef __vtkvmtkPolyBallModeller_h
#define __vtkvmtkPolyBallModeller_h

#include "vtkImageAlgorithm.h"
#include "vtkImageData.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyBallModeller : public vtkImageAlgorithm 
{
  public:
  vtkTypeMacro(vtkvmtkPolyBallModeller,vtkImageAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkPolyBallModeller *New();
  
  ///@{
  /**
   * Specify i-j-k dimensions on which to sample polyball function.
   */
  vtkGetVectorMacro(SampleDimensions,int,3);
  vtkSetVectorMacro(SampleDimensions,int,3);
  ///@}
  
  ///@{
  /**
   * Specify the position in space to perform the sampling.
   */
  vtkSetVectorMacro(ModelBounds,double,6);
  vtkGetVectorMacro(ModelBounds,double,6);
  ///@}

  ///@{
  /**
   * Set/Get an optional reference image: when set, its extent/spacing/origin are used in place of
   * SampleDimensions/ModelBounds to define the output image geometry (e.g. to match an existing
   * volume).
   */
  vtkSetObjectMacro(ReferenceImage,vtkImageData);
  vtkGetObjectMacro(ReferenceImage,vtkImageData);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of the input centerlines holding the sphere radius at
   * each point. Required input.
   * Commonly named "MaximumInscribedSphereRadius".
   */
  vtkSetStringMacro(RadiusArrayName);
  vtkGetStringMacro(RadiusArrayName);
  ///@}

  ///@{
  /**
   * Toggle whether the tube function is evaluated with vtkvmtkPolyBallLine (continuous, linearly
   * interpolated tubular envelope; default: on) or vtkvmtkPolyBall (discrete union of spheres, one
   * per input point, when off).
   */
  vtkSetMacro(UsePolyBallLine,int);
  vtkGetMacro(UsePolyBallLine,int);
  vtkBooleanMacro(UsePolyBallLine,int);
  ///@}

  ///@{
  /**
   * Toggle negating the sampled function values (multiplying by -1) before writing them to the
   * output image. Default: off.
   */
  vtkSetMacro(NegateFunction,int);
  vtkGetMacro(NegateFunction,int);
  vtkBooleanMacro(NegateFunction,int);
  ///@}


  protected:
  vtkvmtkPolyBallModeller();
  ~vtkvmtkPolyBallModeller();

  int FillInputPortInformation(int, vtkInformation *info) override;
  virtual int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  int SampleDimensions[3];
  double ModelBounds[6];

  char* RadiusArrayName;

  int UsePolyBallLine;

  int NegateFunction;

  vtkImageData* ReferenceImage;

  private:
  vtkvmtkPolyBallModeller(const vtkvmtkPolyBallModeller&);  // Not implemented.
  void operator=(const vtkvmtkPolyBallModeller&);  // Not implemented.
};

#endif


