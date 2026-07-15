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
 * @class   vtkvmtkUnstructuredGridVorticityFilter
 * @brief   Calculates vorticity from velocity in a mesh.
 * @ingroup DifferentialGeometry
 *
 * Computes the vorticity (curl) of the 3-component point data velocity array named
 * VelocityArrayName using a finite-element assembly (vtkvmtkUnstructuredGridFEVorticityAssembler,
 * one component at a time, Gauss quadrature order QuadratureOrder, solved to
 * ConvergenceTolerance), and stores it in the point data array named VorticityArrayName. If
 * ComputeHelicityFactor is on, also computes the normalized helicity (the cosine of the angle
 * between the velocity and vorticity vectors at each point, in [-1,1]) into
 * HelicityFactorArrayName -- a common hemodynamic metric for identifying helical flow patterns.
 *
 * @sa vtkvmtkUnstructuredGridFEVorticityAssembler
 */

#ifndef __vtkvmtkUnstructuredGridVorticityFilter_h
#define __vtkvmtkUnstructuredGridVorticityFilter_h

#include "vtkvmtkWin32Header.h"
#include "vtkUnstructuredGridAlgorithm.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkUnstructuredGridVorticityFilter : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkvmtkUnstructuredGridVorticityFilter* New();
  vtkTypeMacro(vtkvmtkUnstructuredGridVorticityFilter,vtkUnstructuredGridAlgorithm);

  ///@{
  /**
   * Set/Get the name of the 3-component point data array holding the velocity field. Required
   * input.
   */
  vtkSetStringMacro(VelocityArrayName);
  vtkGetStringMacro(VelocityArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output 3-component point data array where the computed vorticity is
   * stored.
   * Commonly named "Vorticity".
   */
  vtkSetStringMacro(VorticityArrayName);
  vtkGetStringMacro(VorticityArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output point data array where the normalized helicity is stored, when
   * ComputeHelicityFactor is on.
   * Commonly named "Helicity".
   */
  vtkSetStringMacro(HelicityFactorArrayName);
  vtkGetStringMacro(HelicityFactorArrayName);
  ///@}

  ///@{
  /**
   * Toggle computing the normalized helicity (cosine of the angle between velocity and vorticity)
   * into HelicityFactorArrayName, in addition to the vorticity field itself. Default: off.
   */
  vtkSetMacro(ComputeHelicityFactor,int);
  vtkGetMacro(ComputeHelicityFactor,int);
  vtkBooleanMacro(ComputeHelicityFactor,int);
  ///@}

  ///@{
  /**
   * Set/Get the convergence tolerance of the iterative linear solver used to solve the
   * finite-element vorticity assembly for each component.
   */
  vtkSetMacro(ConvergenceTolerance,double);
  vtkGetMacro(ConvergenceTolerance,double);
  ///@}

  ///@{
  /**
   * Set/Get the order of the Gauss quadrature rule used to integrate the finite-element matrices in
   * vtkvmtkUnstructuredGridFEVorticityAssembler.
   */
  vtkSetMacro(QuadratureOrder,int);
  vtkGetMacro(QuadratureOrder,int);
  ///@}

protected:
  vtkvmtkUnstructuredGridVorticityFilter();
  ~vtkvmtkUnstructuredGridVorticityFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* VelocityArrayName;
  char* VorticityArrayName;
  char* HelicityFactorArrayName;
  int ComputeHelicityFactor;
  double ConvergenceTolerance;
  int QuadratureOrder;

private:
  vtkvmtkUnstructuredGridVorticityFilter(const vtkvmtkUnstructuredGridVorticityFilter&);  // Not implemented.
  void operator=(const vtkvmtkUnstructuredGridVorticityFilter&);  // Not implemented.
};

#endif

