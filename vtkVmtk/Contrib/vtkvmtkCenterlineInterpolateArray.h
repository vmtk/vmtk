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
 * @class   vtkvmtkCenterlineInterpolateArray
 * @brief   Interpolate a point-based array from a set of provided values.
 * @ingroup Contrib
 *
 * Given a sparse set of values (Values) attached to a subset of the input
 * poly data's points (ValuesIds), vtkvmtkCenterlineInterpolateArray produces
 * a dense point-data array (InterpolatedArrayName) covering all points of
 * every polyline cell (e.g. a set of centerlines). Along each polyline, the
 * value at points lying between two provided points is obtained by linear
 * interpolation with respect to arc length (abscissa); points before the
 * first provided value or after the last one are set to that nearest
 * provided value (constant extrapolation). Points on cells that contain no
 * provided value at all, or that are not polylines, are set to
 * DefaultValue.
 */

#ifndef __vtkvmtkCenterlineInterpolateArray_h
#define __vtkvmtkCenterlineInterpolateArray_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#include "vtkDataArray.h"
#include "vtkIdList.h"

class VTK_VMTK_CONTRIB_EXPORT vtkvmtkCenterlineInterpolateArray : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkCenterlineInterpolateArray* New();
  vtkTypeMacro(vtkvmtkCenterlineInterpolateArray,vtkPolyDataAlgorithm);

  ///@{
  /**
   * Default value to fill in when no data is available
   */
  vtkSetMacro(DefaultValue,double);
  vtkGetMacro(DefaultValue,double);
  ///@}

  ///@{
  /**
   * Set/Get the values from which to interpolate
   */
  vtkSetObjectMacro(Values,vtkDataArray);
  vtkGetObjectMacro(Values,vtkDataArray);
  ///@}

  ///@{
  /**
   * Set/Get the point ids corresponding to the provided values
   */
  vtkSetObjectMacro(ValuesIds,vtkIdList);
  vtkGetObjectMacro(ValuesIds,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/Get the name of the resulting array
   * Commonly named "InterpolatedRadius" or "ResolutionArray", depending on context.
   */
  vtkSetStringMacro(InterpolatedArrayName);
  vtkGetStringMacro(InterpolatedArrayName);
  ///@}
  

protected:
  vtkvmtkCenterlineInterpolateArray();
  ~vtkvmtkCenterlineInterpolateArray();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
  
  char *InterpolatedArrayName;

  double DefaultValue;
  vtkDataArray *Values;
  vtkIdList *ValuesIds;

private:
  vtkvmtkCenterlineInterpolateArray(const vtkvmtkCenterlineInterpolateArray&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineInterpolateArray&);  // Not implemented.
};

#endif

