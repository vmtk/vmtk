/*=========================================================================

  Program:   Visualization Toolkit

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
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
 * @class   vtkvmtkStaticTemporalStreamTracer
 * @brief   Trace particle paths (streaklines) through an unsteady velocity field sampled at
 * discrete time steps on a fixed mesh.
 * @ingroup Misc
 *
 * vtkvmtkStaticTemporalStreamTracer specializes vtkStreamTracer to integrate trajectories
 * through a time-varying, but spatially static (non-deforming), velocity field: the input mesh
 * does not move or change connectivity over time, but carries one velocity sample per time step,
 * stored as separate point data arrays (see TimeStepsTable, VectorPrefix, Component0Prefix,
 * Component1Prefix, Component2Prefix, UseVectorComponents). Internally it installs a
 * vtkvmtkStaticTemporalInterpolatedVelocityField as its interpolator prototype, which linearly
 * interpolates in time between the two bracketing time steps (optionally wrapping around
 * cyclically, see Periodic) in addition to the usual spatial interpolation within cells. Each
 * seed point starts its integration at a time given either by a uniform SeedTime or, if
 * SeedTimesArrayName is found on the seed source, by a per-seed start time array -- so, unlike a
 * classic streamline tracer, trajectories seeded at the same location but different times
 * generally trace different paths (streaklines) through the unsteady field. This is the tracer
 * driving the vmtkparticletracer pype script, typically used to visualize/analyze particle
 * transport in unsteady (e.g. pulsatile CFD) blood flow simulations.
 *
 * @sa
 * vtkvmtkStaticTemporalInterpolatedVelocityField
 */

#ifndef __vtkvmtkStaticTemporalStreamTracer_h
#define __vtkvmtkStaticTemporalStreamTracer_h

#include "vtkStreamTracer.h"
#include "vtkvmtkWin32Header.h"

class vtkTable;

class VTK_VMTK_MISC_EXPORT vtkvmtkStaticTemporalStreamTracer : public vtkStreamTracer
{
public:
  vtkTypeMacro(vtkvmtkStaticTemporalStreamTracer,vtkStreamTracer);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkStaticTemporalStreamTracer *New();

  ///@{
  /**
   * Set/Get the time value at which every seed point starts its integration, used when
   * SeedTimesArrayName does not name an existing point data array on the seed source. Default: 0.0.
   */
  vtkSetMacro(SeedTime, double);
  vtkGetMacro(SeedTime, double);
  ///@}

  ///@{
  /**
   * Set/Get the name of a point data array, on the seed source, holding a per-seed integration
   * start time. When present, it overrides the uniform SeedTime for every corresponding seed
   * point, allowing seeds released at different times to be traced in a single pass. Default:
   * NULL (use SeedTime for all seeds).
   */
  vtkSetStringMacro(SeedTimesArrayName);
  vtkGetStringMacro(SeedTimesArrayName);
  ///@}

  ///@{
  /**
   * Toggle whether time values are treated as periodic, i.e. wrapped modulo the span between the
   * first and last row of TimeStepsTable, so that integration continues cyclically past the last
   * time step (useful for cyclic flows, e.g. one cardiac cycle). Default: off.
   */
  vtkSetMacro(Periodic, int);
  vtkGetMacro(Periodic, int);
  vtkBooleanMacro(Periodic, int);
  ///@}

  ///@{
  /**
   * Set/Get a uniform scale factor applied to every interpolated velocity vector/component
   * before integration. Default: 1.0.
   */
  vtkSetMacro(VelocityScale, double);
  vtkGetMacro(VelocityScale, double);
  ///@}

  ///@{
  /**
   * Set/Get the table mapping time step index to time value, used to interpolate the velocity
   * field in time. The table must have (at least) two columns: column 0 holds the integer time
   * step index appended to the array name prefixes (VectorPrefix, or Component0Prefix/
   * Component1Prefix/Component2Prefix) to look up each time step's velocity array(s), and column
   * 1 holds the corresponding time value, with rows in increasing time order.
   */
  vtkGetObjectMacro(TimeStepsTable,vtkTable);
  virtual void SetTimeStepsTable(vtkTable*);
  ///@}

  ///@{
  /**
   * Toggle whether each time step's velocity is read from three separate scalar point data
   * arrays (named with Component0Prefix/Component1Prefix/Component2Prefix followed by the time
   * step index) rather than from a single 3-component vector point data array (named with
   * VectorPrefix followed by the time step index). Default: off (use vector arrays).
   */
  vtkSetMacro(UseVectorComponents, int);
  vtkGetMacro(UseVectorComponents, int);
  vtkBooleanMacro(UseVectorComponents, int);
  ///@}

  ///@{
  /**
   * Set/Get the prefix used, together with the time step index taken from TimeStepsTable, to
   * build the name of the 3-component point data vector array holding each time step's velocity
   * samples. Only used when UseVectorComponents is off.
   */
  vtkSetStringMacro(VectorPrefix);
  vtkGetStringMacro(VectorPrefix);
  ///@}

  ///@{
  /**
   * Set/Get the prefix used, together with the time step index taken from TimeStepsTable, to
   * build the name of the scalar point data array holding the x-component of each time step's
   * velocity samples. Only used when UseVectorComponents is on.
   */
  vtkSetStringMacro(Component0Prefix);
  vtkGetStringMacro(Component0Prefix);
  ///@}

  ///@{
  /**
   * Set/Get the prefix used, together with the time step index taken from TimeStepsTable, to
   * build the name of the scalar point data array holding the y-component of each time step's
   * velocity samples. Only used when UseVectorComponents is on.
   */
  vtkSetStringMacro(Component1Prefix);
  vtkGetStringMacro(Component1Prefix);
  ///@}

  ///@{
  /**
   * Set/Get the prefix used, together with the time step index taken from TimeStepsTable, to
   * build the name of the scalar point data array holding the z-component of each time step's
   * velocity samples. Only used when UseVectorComponents is on.
   */
  vtkSetStringMacro(Component2Prefix);
  vtkGetStringMacro(Component2Prefix);
  ///@}

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


