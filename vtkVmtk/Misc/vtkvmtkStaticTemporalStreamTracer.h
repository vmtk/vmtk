/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkvmtkStaticTemporalStreamTracer.h

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

// .NAME vtkvmtkStaticTemporalStreamTracer - Streamline generator
// .SECTION Description
// vtkvmtkStaticTemporalStreamTracer is a filter that integrates a vector field to generate streamlines. The integration is performed using a specified integrator, by default Runge-Kutta2.

#ifndef __vtkvmtkStaticTemporalStreamTracer_h
#define __vtkvmtkStaticTemporalStreamTracer_h

#include "vtkStreamTracer.h"
#include "vtkvmtkWin32Header.h"

class vtkTable;

class VTK_VMTK_MISC_EXPORT vtkvmtkStaticTemporalStreamTracer : public vtkStreamTracer
{
public:
  vtkTypeMacro(vtkvmtkStaticTemporalStreamTracer,vtkStreamTracer);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkvmtkStaticTemporalStreamTracer *New();

  vtkSetMacro(SeedTime, double);
  vtkGetMacro(SeedTime, double);

  vtkSetStringMacro(SeedTimesArrayName);
  vtkGetStringMacro(SeedTimesArrayName);

  vtkSetMacro(Periodic, int);
  vtkGetMacro(Periodic, int);
  vtkBooleanMacro(Periodic, int);

  vtkSetMacro(VelocityScale, double);
  vtkGetMacro(VelocityScale, double);

  vtkGetObjectMacro(TimeStepsTable,vtkTable);
  virtual void SetTimeStepsTable(vtkTable*);

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

protected:

  vtkvmtkStaticTemporalStreamTracer();
  ~vtkvmtkStaticTemporalStreamTracer();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void InitializeDefaultInterpolatorPrototype();

  int CheckInputs(vtkAbstractInterpolatedVelocityField*& func, int* maxCellSize);

  void InitializeSeeds(vtkDataArray*& seeds,
                       vtkIdList*& seedIds,
                       vtkDoubleArray*& startTimes,
                       vtkIntArray*& integrationDirections,
                       vtkDataSet *source);

  void Integrate(vtkDataSet *input,
                 vtkPolyData* output,
                 vtkDataArray* seedSource,
                 vtkIdList* seedIds,
                 vtkDoubleArray* startTimes,
                 vtkIntArray* integrationDirections,
                 double lastPoint[3],
                 vtkAbstractInterpolatedVelocityField* func,
                 int maxCellSize,
                 double& propagation,
                 vtkIdType& numSteps);
 
  double SeedTime;
  char* SeedTimesArrayName;
  int Periodic;

  int UseVectorComponents;

  char* VectorPrefix;
  char* Component0Prefix;
  char* Component1Prefix;
  char* Component2Prefix;

  vtkTable* TimeStepsTable;

  double VelocityScale;

private:
  vtkvmtkStaticTemporalStreamTracer(const vtkvmtkStaticTemporalStreamTracer&);  // Not implemented.
  void operator=(const vtkvmtkStaticTemporalStreamTracer&);  // Not implemented.
};


#endif


