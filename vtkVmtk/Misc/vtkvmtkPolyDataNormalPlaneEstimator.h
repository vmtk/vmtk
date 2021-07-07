/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataNormalPlaneEstimator.h,v $
Language:  C++
Date:      $Date: 2015/12/01 12:26:27 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkPolyDataNormalPlaneEstimator - calculate the normal plane alone a centerline path
// .SECTION Description
// .

#ifndef __vtkvmtkPolyDataNormalPlaneEstimator_h
#define __vtkvmtkPolyDataNormalPlaneEstimator_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkPolyDataNormalPlaneEstimator : public vtkPolyDataAlgorithm
{
  public:
  vtkTypeMacro(vtkvmtkPolyDataNormalPlaneEstimator,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkvmtkPolyDataNormalPlaneEstimator *New();

  vtkSetMacro(OriginPointId,vtkIdType);
  vtkGetMacro(OriginPointId,vtkIdType);

  vtkGetVectorMacro(Origin,double,3);
  vtkGetVectorMacro(Normal,double,3);

  vtkSetMacro(UseConnectivity,int);
  vtkGetMacro(UseConnectivity,int);
  vtkBooleanMacro(UseConnectivity,int);

  vtkSetMacro(MinimumNeighborhoodSize,int);
  vtkGetMacro(MinimumNeighborhoodSize,int);
 
  protected:
  vtkvmtkPolyDataNormalPlaneEstimator();
  ~vtkvmtkPolyDataNormalPlaneEstimator();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkIdType OriginPointId;

  double Origin[3];
  double Normal[3];

  int UseConnectivity;
  int MinimumNeighborhoodSize;

  private:
  vtkvmtkPolyDataNormalPlaneEstimator(const vtkvmtkPolyDataNormalPlaneEstimator&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataNormalPlaneEstimator&);  // Not implemented.
};

#endif
