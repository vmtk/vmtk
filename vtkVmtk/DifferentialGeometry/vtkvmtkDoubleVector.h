/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkDoubleVector.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
  Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkDoubleVector - Base class for vectors utilized in finite-element and other PDE evaluations.
// .SECTION Description
// ..

#ifndef __vtkvmtkDoubleVector_h
#define __vtkvmtkDoubleVector_h

#include "vtkObject.h"
#include "vtkDataArray.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#define VTK_VMTK_L2_NORM 0
#define VTK_VMTK_LINF_NORM 1

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkDoubleVector : public vtkObject
{
public:

  static vtkvmtkDoubleVector* New();
  vtkTypeMacro(vtkvmtkDoubleVector,vtkObject);

  vtkSetMacro(NormType,int);
  vtkGetMacro(NormType,int);
  void SetNormTypeToL2()
    {this->SetNormType(VTK_VMTK_L2_NORM);};  
  void SetNormTypeToLInf()
    {this->SetNormType(VTK_VMTK_LINF_NORM);};  

  vtkGetMacro(NumberOfElements,vtkIdType);
  vtkGetMacro(NumberOfElementsPerVariable,vtkIdType);
  vtkGetMacro(NumberOfVariables,vtkIdType);

  void Allocate(vtkIdType numberOfElementsPerVariable, vtkIdType numberOfVariables=1);
  void Fill(double value);
  void Assign(vtkvmtkDoubleVector *src);
  void Assign(vtkIdType numberOfElements, const double *array);

  double GetElement(vtkIdType i) {return this->Array[i];};
  void SetElement(vtkIdType i, double value) {this->Array[i] = value;};
  void AddElement(vtkIdType i, double value) {this->Array[i] += value;};

  //bool GetLocked(vtkIdType i) {return this->Locked[i];}
  //void SetLocked(vtkIdType i, bool locked) {this->Locked[i] = locked;}
  //void UnlockAll();

  const double* GetArray() {return this->Array;};
  void CopyIntoArrayComponent(vtkDataArray *array, int component);
  void CopyVariableIntoArrayComponent(vtkDataArray *array, int variable, int component);

  double ComputeNorm();
  void Add(vtkvmtkDoubleVector* vectorToAdd);
  void Subtract(vtkvmtkDoubleVector* vectorToSubtract);
  void MultiplyBy(double scalar);
  double Dot(vtkvmtkDoubleVector* vectorToDotWith);

  void DeepCopy(vtkvmtkDoubleVector *src);

protected:
  vtkvmtkDoubleVector();
  ~vtkvmtkDoubleVector();

  vtkIdType NumberOfElements;
  vtkIdType NumberOfElementsPerVariable;
  vtkIdType NumberOfVariables;

  int NormType;

  double* Array;
  //bool* Locked;

private:
  vtkvmtkDoubleVector(const vtkvmtkDoubleVector&);  // Not implemented.
  void operator=(const vtkvmtkDoubleVector&);  // Not implemented.
};

#endif

