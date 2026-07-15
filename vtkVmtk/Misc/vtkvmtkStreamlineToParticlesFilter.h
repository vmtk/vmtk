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
 * @class   vtkvmtkStreamlineToParticlesFilter
 * @brief   Resample time-resolved streamlines into discrete Lagrangian particle positions at
 * successive time steps, simulating repeated particle injections.
 * @ingroup Misc
 *
 * Given input streamlines (polylines) carrying a per-point integration-time array
 * (IntegrationTimeArrayName), simulates NumberOfInjections separate particle injections evenly
 * spaced between InjectionStart and InjectionEnd (in the same time units as the streamline
 * integration time), each releasing NumberOfParticlesPerInjection particles that are then advected
 * along their streamlines up to TracingEnd, sampled at a fixed time increment DeltaT. The output is
 * a point cloud of particle positions at each sampled time, useful for visualizing pulsatile/
 * unsteady flow as a particle animation rather than static streamlines.
 */

#ifndef __vtkvmtkStreamlineToParticlesFilter_h
#define __vtkvmtkStreamlineToParticlesFilter_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkStreamlineToParticlesFilter : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkStreamlineToParticlesFilter,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkStreamlineToParticlesFilter *New();
 
  ///@{
  /**
   * Set/Get the number of separate particle injections simulated, evenly spaced between
   * InjectionStart and InjectionEnd. Default: 10.
   */
  vtkSetMacro(NumberOfInjections,int);
  vtkGetMacro(NumberOfInjections,int);
  ///@}

  ///@{
  /**
   * Set/Get the number of particles released per injection, subsampled from the input streamlines
   * (one particle roughly every GetNumberOfCells()/NumberOfParticlesPerInjection streamlines).
   * Default: 10.
   */
  vtkSetMacro(NumberOfParticlesPerInjection,int);
  vtkGetMacro(NumberOfParticlesPerInjection,int);
  ///@}

  ///@{
  /**
   * Set/Get the integration time (in the same units as IntegrationTimeArrayName) of the first
   * injection. Default: 0.0.
   */
  vtkSetMacro(InjectionStart,double);
  vtkGetMacro(InjectionStart,double);
  ///@}

  ///@{
  /**
   * Set/Get the integration time of the last injection; the NumberOfInjections injections are
   * spaced evenly between InjectionStart and InjectionEnd. Default: 1.0.
   */
  vtkSetMacro(InjectionEnd,double);
  vtkGetMacro(InjectionEnd,double);
  ///@}

  ///@{
  /**
   * Set/Get the integration time at which particle tracing stops for every injection. Default: 1.0.
   */
  vtkSetMacro(TracingEnd,double);
  vtkGetMacro(TracingEnd,double);
  ///@}

  ///@{
  /**
   * Set/Get the time increment at which each particle's position is sampled along its streamline.
   * Default: 1E-2.
   */
  vtkSetMacro(DeltaT,double);
  vtkGetMacro(DeltaT,double);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of the input streamlines holding the cumulative
   * integration time at each point. Required input.
   */
  vtkSetStringMacro(IntegrationTimeArrayName);
  vtkGetStringMacro(IntegrationTimeArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output point data array where each sampled particle position's absolute
   * time (injection offset plus elapsed integration time) is stored.
   */
  vtkSetStringMacro(TimeArrayName);
  vtkGetStringMacro(TimeArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the 3-component point data array of the input streamlines holding the
   * velocity field, copied onto each sampled particle position.
   */
  vtkSetStringMacro(VelocityArrayName);
  vtkGetStringMacro(VelocityArrayName);
  ///@}
 
  protected:
  vtkvmtkStreamlineToParticlesFilter();
  ~vtkvmtkStreamlineToParticlesFilter();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  int NumberOfInjections;
  int NumberOfParticlesPerInjection;

  double InjectionStart;
  double InjectionEnd;
  double TracingEnd;
  double DeltaT;

  char* IntegrationTimeArrayName;
  char* TimeArrayName;
  char* VelocityArrayName;

  private:
  vtkvmtkStreamlineToParticlesFilter(const vtkvmtkStreamlineToParticlesFilter&);  // Not implemented.
  void operator=(const vtkvmtkStreamlineToParticlesFilter&);  // Not implemented.
};

#endif
