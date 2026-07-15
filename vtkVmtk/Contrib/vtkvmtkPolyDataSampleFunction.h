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

  Note: this class was contributed by 
	Tangui Morvan
	Kalkulo AS
	Simula Research Laboratory
=========================================================================*/
/**
 * @class   vtkvmtkPolyDataSampleFunction
 * @brief   Sample an implicit function at the points of a surface.
 * @ingroup Contrib
 *
 * vtkvmtkPolyDataSampleFunction evaluates ImplicitFunction at every point
 * of the input poly data and writes the resulting scalar values into a
 * point data array named SampleArrayName, e.g. to visualize or threshold
 * an implicit function (such as a vtkvmtkPolyBallLine) directly on a
 * surface rather than on a volumetric sampling grid.
 */

#ifndef __vtkvmtkPolyDataSampleFunction_h
#define __vtkvmtkPolyDataSampleFunction_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

#include "vtkPolyData.h"
#include "vtkImplicitFunction.h"

class VTK_VMTK_CONTRIB_EXPORT vtkvmtkPolyDataSampleFunction : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataSampleFunction* New();
  vtkTypeMacro(vtkvmtkPolyDataSampleFunction,vtkPolyDataAlgorithm);

  ///@{
  /**
   * Specify the implicit function to use to generate data.
   */
  virtual void SetImplicitFunction(vtkImplicitFunction*);
  vtkGetObjectMacro(ImplicitFunction,vtkImplicitFunction);
  ///@}

  ///@{
  /**
   * Set/get the name of the output point data array holding the sampled
   * function values.
   * Commonly named "ResolutionArray".
   */
  vtkSetStringMacro(SampleArrayName);
  vtkGetStringMacro(SampleArrayName);
  ///@}

protected:
  vtkvmtkPolyDataSampleFunction();
  ~vtkvmtkPolyDataSampleFunction();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* SampleArrayName;

  vtkImplicitFunction *ImplicitFunction;


private:
  vtkvmtkPolyDataSampleFunction(const vtkvmtkPolyDataSampleFunction&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataSampleFunction&);  // Not implemented.
};

#endif

