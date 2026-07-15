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
 * @class   vtkvmtkCenterlineBifurcationReferenceSystems
 * @brief   Generates a normal and upnormal reference system for each bifurcation point of a split centerline.
 * @ingroup ComputationalGeometry
 *
 * The "normal" vector is the normal to the bifurcation plane (which is the best fit plane in relation to bifurcation reference points). The upnormal vector is normal to the "normal" vector pointing downstream from the parent artery.
 *
 * Takes as input centerlines that have already been split into branches (see
 * vtkvmtkCenterlineSplittingAndGroupingFilter), and outputs a vtkPolyData of vertex points, one per
 * bifurcation, with the reference system arrays as point data. This is the filter behind the
 * vmtkbifurcationreferencesystems pype script; its output is consumed by
 * vtkvmtkCenterlineBifurcationVectors to characterize branch angles at each bifurcation.
 *
 * @sa
 * vtkvmtkCenterlineSplittingAndGroupingFilter, vtkvmtkCenterlineBifurcationVectors
 */

#ifndef __vtkvmtkCenterlineBifurcationReferenceSystems_h
#define __vtkvmtkCenterlineBifurcationReferenceSystems_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class vtkPoints;
class vtkDoubleArray;
class vtkIntArray;
  
class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineBifurcationReferenceSystems : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkCenterlineBifurcationReferenceSystems,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkCenterlineBifurcationReferenceSystems* New();

  ///@{
  /**
   * Set/Get the name of the point data array of the input centerlines holding the maximum inscribed
   * sphere radius at each point. Required input.
   * Commonly named "MaximumInscribedSphereRadius".
   */
  vtkSetStringMacro(RadiusArrayName);
  vtkGetStringMacro(RadiusArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array of the input centerlines holding the branch group id of
   * each cell. Required input; see vtkvmtkCenterlineSplittingAndGroupingFilter.
   * Commonly named "GroupIds".
   */
  vtkSetStringMacro(GroupIdsArrayName);
  vtkGetStringMacro(GroupIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array of the input centerlines holding, for each cell, whether
   * it is a "blanked" (redundant, overlapping) tract introduced by splitting -- blanked cells are
   * excluded when identifying the branches meeting at each bifurcation.
   * Commonly named "Blanking".
   */
  vtkSetStringMacro(BlankingArrayName);
  vtkGetStringMacro(BlankingArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the 3-component point data array of the output reference systems where the
   * bifurcation plane normal is stored for each bifurcation.
   * Commonly named "Normal".
   */
  vtkSetStringMacro(NormalArrayName);
  vtkGetStringMacro(NormalArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the 3-component point data array of the output reference systems where the
   * "upnormal" (perpendicular to Normal, pointing downstream from the parent branch) is stored for
   * each bifurcation.
   * Commonly named "UpNormal".
   */
  vtkSetStringMacro(UpNormalArrayName);
  vtkGetStringMacro(UpNormalArrayName);
  ///@}

  protected:
  vtkvmtkCenterlineBifurcationReferenceSystems();
  ~vtkvmtkCenterlineBifurcationReferenceSystems();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void ComputeGroupReferenceSystem(vtkPolyData* input, int referenceGroupId, vtkPoints* outputPoints, vtkDoubleArray* normalArray, vtkDoubleArray* upNormalArray, vtkIntArray* referenceGroupIdsArray);

  char* RadiusArrayName;
  char* GroupIdsArrayName;
  char* BlankingArrayName;

  char* NormalArrayName;
  char* UpNormalArrayName;

  private:
  vtkvmtkCenterlineBifurcationReferenceSystems(const vtkvmtkCenterlineBifurcationReferenceSystems&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineBifurcationReferenceSystems&);  // Not implemented.
};

#endif
