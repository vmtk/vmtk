/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkvmtkStaticTemporalInterpolatedVelocityField.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*=========================================================================

Program:   VMTK
Language:  C++

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkStaticTemporalInterpolatedVelocityField - A concrete class for obtaining the interpolated velocity values at a point.


#ifndef __vtkvmtkStaticTemporalInterpolatedVelocityField_h
#define __vtkvmtkStaticTemporalInterpolatedVelocityField_h

#include "vtkvmtkWin32Header.h"
#include "vtkVersion.h"

#if (VTK_MAJOR_VERSION <= 5)
#include "vtkAbstractInterpolatedVelocityField.h"
#else
#include "vtkInterpolatedVelocityField.h"
#endif

class vtkTable;
#if (VTK_MAJOR_VERSION > 5)
class vtkAbstractInterpolatedVelocityFieldDataSetsType;
#endif

class VTK_VMTK_MISC_EXPORT vtkvmtkStaticTemporalInterpolatedVelocityField
#if (VTK_MAJOR_VERSION <= 5)
  : public vtkAbstractInterpolatedVelocityField
#else
  : public vtkInterpolatedVelocityField
#endif
{
public:
#if (VTK_MAJOR_VERSION <= 5)
  vtkTypeMacro( vtkvmtkStaticTemporalInterpolatedVelocityField,
                        vtkAbstractInterpolatedVelocityField );
#else
  vtkTypeMacro( vtkvmtkStaticTemporalInterpolatedVelocityField,
                      vtkInterpolatedVelocityField );

#endif
  void PrintSelf( ostream & os, vtkIndent indent ) override;

  // Description:
  // Construct a vtkvmtkStaticTemporalInterpolatedVelocityField without an initial dataset.
  // Caching is set on and LastCellId is set to -1.
  static vtkvmtkStaticTemporalInterpolatedVelocityField * New();

#if (VTK_MAJOR_VERSION <= 5)
  // Description:
  // Add a dataset used for the implicit function evaluation. If more than
  // one dataset is added, the evaluation point is searched in all until a 
  // match is found. THIS FUNCTION DOES NOT CHANGE THE REFERENCE COUNT OF 
  // DATASET FOR THREAD SAFETY REASONS.
  virtual void AddDataSet( vtkDataSet * dataset );
#endif
  
  vtkGetObjectMacro(TimeStepsTable,vtkTable);
  virtual void SetTimeStepsTable(vtkTable*);

  vtkSetMacro(Periodic, int);
  vtkGetMacro(Periodic, int);
  vtkBooleanMacro(Periodic, int);

  vtkSetMacro(VelocityScale, double);
  vtkGetMacro(VelocityScale, double);

  vtkSetMacro(UseVectorComponents, int);
  vtkGetMacro(UseVectorComponents, int);
  vtkBooleanMacro(UseVectorComponents, int);

  vtkSetStringMacro(VectorPrefix);
  vtkGetStringMacro(VectorPrefix);

  vtkSetStringMacro(Component0Prefix);
  vtkGetStringMacro(Component0Prefix);

  vtkSetStringMacro(Component1Prefix);
  vtkGetStringMacro(Component1Prefix);

  vtkSetStringMacro(Component2Prefix);
  vtkGetStringMacro(Component2Prefix);

#if (VTK_MAJOR_VERSION <= 5)
  // Description:
  // Evaluate the velocity field f at point (x, y, z, t).
  virtual int FunctionValues( double * x, double * f ) override;
#endif
  
  // Description:
  // Set the cell id cached by the last evaluation within a specified dataset.
  virtual void SetLastCellId( vtkIdType c, int dataindex ) override;
  
  // Description:
  // Set the cell id cached by the last evaluation.
  virtual void SetLastCellId( vtkIdType c ) override
    { this->Superclass::SetLastCellId( c ); }

  virtual void CopyParameters( vtkAbstractInterpolatedVelocityField * from ) override;

protected:
  vtkvmtkStaticTemporalInterpolatedVelocityField();
  ~vtkvmtkStaticTemporalInterpolatedVelocityField();

  // Description:
  // Evaluate the velocity field f at point (x, y, z) in a specified dataset
  // by either involving vtkPointLocator, via vtkPointSet::FindCell(), in
  // locating the next cell (for datasets of type vtkPointSet) or simply
  // invoking vtkImageData/vtkRectilinearGrid::FindCell() to fulfill the same
  // task if the point is outside the current cell.
  virtual int FunctionValues( vtkDataSet * ds, double * x, double * f ) override;

  void FindTimeRowId(double time, int& prevRowId, int& nextRowId, double& p);

  void BuildArrayName(char* prefix, int index, char* name);

  vtkTable* TimeStepsTable;

  int Periodic;

  double VelocityScale;

  int UseVectorComponents;

  char* VectorPrefix;
  char* Component0Prefix;
  char* Component1Prefix;
  char* Component2Prefix;
#if (VTK_MAJOR_VERSION > 5)
  int LastDataSetIndex;
#endif

private:
  vtkvmtkStaticTemporalInterpolatedVelocityField
    ( const vtkvmtkStaticTemporalInterpolatedVelocityField & );  // Not implemented.
  void operator = 
    ( const vtkvmtkStaticTemporalInterpolatedVelocityField & );  // Not implemented.
};

#endif
