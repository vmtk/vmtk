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
 * @class   vtkvmtkStaticTemporalInterpolatedVelocityField
 * @brief   Evaluates an interpolated velocity field from a time-tabulated, temporally
 * piecewise-linear velocity field sampled on a fixed (non-moving) mesh.
 * @ingroup Misc
 *
 * This class extends VTK's interpolated velocity field machinery (vtkInterpolatedVelocityField
 * on VTK < 9.2, vtkCompositeInterpolatedVelocityField on VTK >= 9.2) so that, in addition to
 * spatial interpolation within the dataset's cells, it also interpolates in time between a
 * discrete set of time steps whose velocity samples are stored as separate point data arrays on
 * the same static mesh (hence "static": the mesh connectivity/geometry does not change over
 * time, only the velocity values attached to it do). TimeStepsTable maps each time step index to
 * a time value; FunctionValues looks up, for a query point (x,y,z,t), which two consecutive time
 * steps bracket t (wrapping around if Periodic is on) and linearly interpolates in time between
 * the spatially-interpolated velocity at each of them. Velocity samples can be provided either
 * as a single 3-component vector array per time step (named with VectorPrefix followed by the
 * time step index) or as three separate scalar component arrays per time step (named with
 * Component0Prefix/Component1Prefix/Component2Prefix followed by the time step index), selected
 * with UseVectorComponents.
 *
 * This is the velocity field evaluator used by vtkvmtkStaticTemporalStreamTracer (and, through
 * it, the vmtkparticletracer pype script) to trace particle/streak lines through unsteady (e.g.
 * CFD) flow fields sampled at discrete time points on a fixed mesh.
 *
 * @sa
 * vtkvmtkStaticTemporalStreamTracer
 */

#ifndef __vtkvmtkStaticTemporalInterpolatedVelocityField_h
#define __vtkvmtkStaticTemporalInterpolatedVelocityField_h

#include "vtkvmtkWin32Header.h"
#include "vtkVersion.h"

#if VTK_MAJOR_VERSION > 9 || (VTK_MAJOR_VERSION == 9 && VTK_MINOR_VERSION >= 2)
// vtkInterpolatedVelocityField was deprecated by the VTK 9.2 rework of the
// interpolated velocity fields and later removed.
#include "vtkCompositeInterpolatedVelocityField.h"
#define VMTK_STIVF_SUPERCLASS vtkCompositeInterpolatedVelocityField
#else
#include "vtkInterpolatedVelocityField.h"
#define VMTK_STIVF_SUPERCLASS vtkInterpolatedVelocityField
#endif

class vtkTable;
class vtkGenericCell;

class VTK_VMTK_MISC_EXPORT vtkvmtkStaticTemporalInterpolatedVelocityField
  : public VMTK_STIVF_SUPERCLASS
{
public:
  vtkTypeMacro( vtkvmtkStaticTemporalInterpolatedVelocityField,
                      VMTK_STIVF_SUPERCLASS );

  void PrintSelf( std::ostream & os, vtkIndent indent ) override;

  /**
   * Construct a vtkvmtkStaticTemporalInterpolatedVelocityField without an initial dataset. Caching is
   * set on and LastCellId is set to -1.
   */
  static vtkvmtkStaticTemporalInterpolatedVelocityField * New();

  ///@{
  /**
   * Set/Get the table mapping time step index to time value, used to interpolate in time.
   * The table must have (at least) two columns: column 0 holds the integer time step index
   * appended to the array name prefixes (VectorPrefix, or Component0Prefix/Component1Prefix/
   * Component2Prefix) to look up each time step's velocity array(s), and column 1 holds the
   * corresponding time value, with rows in increasing time order (need not be evenly spaced).
   */
  vtkGetObjectMacro(TimeStepsTable,vtkTable);
  virtual void SetTimeStepsTable(vtkTable*);
  ///@}

  ///@{
  /**
   * Toggle whether time values are treated as periodic, i.e. wrapped modulo the span between the
   * first and last row of TimeStepsTable, so that a query time beyond the last time step maps
   * back into range (useful for cyclic flows, e.g. one cardiac cycle). When off, query times at
   * or beyond the last time step use the last time interval with no extrapolation. Default: off.
   */
  vtkSetMacro(Periodic, int);
  vtkGetMacro(Periodic, int);
  vtkBooleanMacro(Periodic, int);
  ///@}

  ///@{
  /**
   * Set/Get a uniform scale factor applied to every interpolated velocity vector/component
   * before it is combined into the output. Default: 1.0.
   */
  vtkSetMacro(VelocityScale, double);
  vtkGetMacro(VelocityScale, double);
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

  /**
   * Set the cell id cached by the last evaluation within a specified dataset.
   */
  virtual void SetLastCellId( vtkIdType c, int dataindex ) override;
  
  ///@{
  /**
   * Set the cell id cached by the last evaluation.
   */
  virtual void SetLastCellId( vtkIdType c ) override
    { this->Superclass::SetLastCellId( c ); }
  ///@}

  /**
   * Copy Periodic, VelocityScale, UseVectorComponents, VectorPrefix, Component0Prefix,
   * Component1Prefix, Component2Prefix, and a deep copy of TimeStepsTable from another
   * interpolated velocity field, provided it is (or derives from)
   * vtkvmtkStaticTemporalInterpolatedVelocityField. Used internally when this velocity field is
   * cloned, e.g. for multithreaded streamline integration.
   */
  virtual void CopyParameters( vtkAbstractInterpolatedVelocityField * from ) override;

protected:
  vtkvmtkStaticTemporalInterpolatedVelocityField();
  ~vtkvmtkStaticTemporalInterpolatedVelocityField();

  /**
   * Evaluate the velocity field f at point (x, y, z) in a specified dataset by either involving
   * vtkPointLocator, via vtkPointSet::FindCell(), in locating the next cell (for datasets of type
   * vtkPointSet) or simply invoking vtkImageData/vtkRectilinearGrid::FindCell() to fulfill the same
   * task if the point is outside the current cell.
   */
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
  int LastDataSetIndex;

#if VTK_MAJOR_VERSION > 9 || (VTK_MAJOR_VERSION == 9 && VTK_MINOR_VERSION >= 2)
  // The VTK 9.2 rework of the interpolated velocity fields removed the
  // GenCell/Cell scratch members from the base class; keep our own.
  vtkGenericCell* TemporalGenCell;
  vtkGenericCell* TemporalCell;
#endif

private:
  vtkvmtkStaticTemporalInterpolatedVelocityField
    ( const vtkvmtkStaticTemporalInterpolatedVelocityField & );  // Not implemented.
  void operator = 
    ( const vtkvmtkStaticTemporalInterpolatedVelocityField & );  // Not implemented.
};

#endif
