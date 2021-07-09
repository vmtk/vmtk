/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkStencil.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
  Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkStencil - Apply a weighting matrix to a single neighborhood of a data set.
// .SECTION Description
// ..

#ifndef __vtkvmtkStencil_h
#define __vtkvmtkStencil_h

#include "vtkObject.h"
#include "vtkvmtkNeighborhood.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkStencil : public vtkvmtkNeighborhood 
{
public:

  vtkTypeMacro(vtkvmtkStencil,vtkvmtkNeighborhood);

  vtkSetMacro(NumberOfComponents,vtkIdType);
  vtkGetMacro(NumberOfComponents,vtkIdType);

  double GetWeight(vtkIdType i) {return this->Weights[i];};
  void SetWeight(vtkIdType i, double weight) {this->Weights[i] = weight;};

  double GetWeight(vtkIdType i, vtkIdType component) {return this->Weights[this->NumberOfComponents * i + component];};
  void SetWeight(vtkIdType i, vtkIdType component, double weight) {this->Weights[this->NumberOfComponents * i + component] = weight;};

  vtkIdType GetNumberOfWeights() {return this->NumberOfComponents * this->NPoints;};

  void ScaleWeights(double factor);

  double GetCenterWeight() {return this->CenterWeight[0];};
  void SetCenterWeight(double weight) {this->CenterWeight[0] = weight;};

  const double* GetCenterWeightTuple() {return this->CenterWeight;};
  
  double GetCenterWeight(vtkIdType component) {return this->CenterWeight[component];};
  void SetCenterWeight(vtkIdType component, double weightComponent) {this->CenterWeight[component] = weightComponent;};

//  vtkSetMacro(CenterWeight,double);
//  vtkGetMacro(CenterWeight,double);

  // Description:
  // Build the stencil.
  virtual void Build() override = 0;

  // Description:
  // Standard DeepCopy method.
  virtual void DeepCopy(vtkvmtkItem *src) override;

  vtkSetMacro(WeightScaling,int)
  vtkGetMacro(WeightScaling,int)
  vtkBooleanMacro(WeightScaling,int)

  vtkSetMacro(NegateWeights,int)
  vtkGetMacro(NegateWeights,int)
  vtkBooleanMacro(NegateWeights,int)

protected:
  vtkvmtkStencil();
  ~vtkvmtkStencil();

  void ResizePointList(vtkIdType ptId, int size);

  void ChangeWeightSign();

  vtkIdType NumberOfComponents;
  double* Weights;
  double* CenterWeight;

  int WeightScaling;

  int NegateWeights;
  
private:
  vtkvmtkStencil(const vtkvmtkStencil&);  // Not implemented.
  void operator=(const vtkvmtkStencil&);  // Not implemented.
};

#endif

