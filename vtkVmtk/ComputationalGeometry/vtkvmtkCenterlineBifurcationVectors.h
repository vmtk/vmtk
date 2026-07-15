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
 * @class   vtkvmtkCenterlineBifurcationVectors
 * @brief   Compute vectors which represent the orientation of branches involved within a bifurcation from a split centerline and bifurcation reference system.
 * @ingroup ComputationalGeometry
 *
 *  For each branch of a bifurcation we compute bifurcation vectors, bifurcation vector components, and bifurcation vector angles. These represent both the in-plane and out-of-plane orientation of the bifurcation branches (which is why we need the bifurcation reference system). This allows us to later compute factors which characterize the bifurcation such as: relationship between the angle of upstream vs downstream vessels entering/leaving the bifurcation, how broad is the bifurcation, how tight is the bifurcation.
 *
 *  This class Computes vectors which are stored as results for the following data arrays:
 *  - BifurcationVectors
 *  - InPlaneBifurcationVectors
 *  - OutOfPlaneBifurcationVectors
 *  - InPlaneBifurcationVectorAngles (the angle between the InPlaneBifurcationVectors and the bifurcation UpNormal, in radians, from -pi to pi, zero for a UpNormal oriented vector, positive in the clockwise direction with respect to the bifurcation Normal)
 *  - OutOfPlaneBifurcationVectorAngles (the angle between the BifurcationVectors and the bifurcation plane, in radians, positive if the OutOfPLaneBifurcationVector is directed as the bifurcation Normal)
 *  - BifurcationVectorsOrientation (flag accounting for the role played by the branch in the bifurcation, 0 for upstream, 1 for downstream the bifurcation)
 *  - GroupIds (the groupId of the branch described by the vector)
 *  - BifurcationGroupIds (the groupId of the bifurcation)
 *
 * Takes as input split, grouped centerlines and the corresponding bifurcation reference systems
 * (see vtkvmtkCenterlineBifurcationReferenceSystems); this is the filter behind the
 * vmtkbifurcationvectors pype script.
 *
 * @sa
 * vtkvmtkCenterlineBifurcationReferenceSystems, vtkvmtkCenterlineSplittingAndGroupingFilter
 */

#ifndef __vtkvmtkCenterlineBifurcationVectors_h
#define __vtkvmtkCenterlineBifurcationVectors_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"
#include "vtkPolyData.h"

class vtkPoints;
class vtkDoubleArray;
class vtkIntArray;
  
class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineBifurcationVectors : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkCenterlineBifurcationVectors,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkCenterlineBifurcationVectors* New();

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
   * Set/Get the name of the cell data array of the input centerlines holding the id of the original,
   * unsplit centerline that each cell belongs to.
   * Commonly named "CenterlineIds".
   */
  vtkSetStringMacro(CenterlineIdsArrayName);
  vtkGetStringMacro(CenterlineIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array of the input centerlines holding the tract id (position
   * of a cell along its original centerline) of each cell.
   * Commonly named "TractIds".
   */
  vtkSetStringMacro(TractIdsArrayName);
  vtkGetStringMacro(TractIdsArrayName);
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
   * Set/Get the bifurcation reference systems (one vertex point per bifurcation, with normal/
   * upnormal point data arrays) computed by vtkvmtkCenterlineBifurcationReferenceSystems. Required
   * input.
   */
  vtkSetObjectMacro(ReferenceSystems,vtkPolyData);
  vtkGetObjectMacro(ReferenceSystems,vtkPolyData);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of ReferenceSystems holding the group id that each
   * reference system (bifurcation) refers to.
   * Commonly named "GroupIds".
   */
  vtkSetStringMacro(ReferenceSystemGroupIdsArrayName);
  vtkGetStringMacro(ReferenceSystemGroupIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of ReferenceSystems holding the bifurcation plane
   * normal vector.
   * Commonly named "Normal".
   */
  vtkSetStringMacro(ReferenceSystemNormalArrayName);
  vtkGetStringMacro(ReferenceSystemNormalArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of ReferenceSystems holding the bifurcation "upnormal"
   * vector (perpendicular to the normal, pointing downstream from the parent branch).
   * Commonly named "UpNormal".
   */
  vtkSetStringMacro(ReferenceSystemUpNormalArrayName);
  vtkGetStringMacro(ReferenceSystemUpNormalArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the 3-component output point data array where the full bifurcation vector
   * (from the bifurcation origin to the branch's reference point) is stored for each branch.
   * Commonly named "BifurcationVectors".
   */
  vtkSetStringMacro(BifurcationVectorsArrayName);
  vtkGetStringMacro(BifurcationVectorsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the 3-component output point data array where the in-plane component of
   * each bifurcation vector (its projection onto the bifurcation plane) is stored.
   * Commonly named "InPlaneBifurcationVectors".
   */
  vtkSetStringMacro(InPlaneBifurcationVectorsArrayName);
  vtkGetStringMacro(InPlaneBifurcationVectorsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the 3-component output point data array where the out-of-plane component of
   * each bifurcation vector (its projection onto the bifurcation normal) is stored.
   * Commonly named "OutOfPlaneBifurcationVectors".
   */
  vtkSetStringMacro(OutOfPlaneBifurcationVectorsArrayName);
  vtkGetStringMacro(OutOfPlaneBifurcationVectorsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output point data array where the angle (in radians, in [-pi,pi]) between
   * each InPlaneBifurcationVectors entry and the bifurcation UpNormal is stored: zero for a vector
   * aligned with UpNormal, positive in the clockwise direction with respect to the bifurcation Normal.
   * Commonly named "InPlaneBifurcationVectorAngles".
   */
  vtkSetStringMacro(InPlaneBifurcationVectorAnglesArrayName);
  vtkGetStringMacro(InPlaneBifurcationVectorAnglesArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output point data array where the angle (in radians) between each
   * BifurcationVectors entry and the bifurcation plane is stored: positive if the out-of-plane
   * component is directed as the bifurcation Normal.
   * Commonly named "OutOfPlaneBifurcationVectorAngles".
   */
  vtkSetStringMacro(OutOfPlaneBifurcationVectorAnglesArrayName);
  vtkGetStringMacro(OutOfPlaneBifurcationVectorAnglesArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output point data array where the role played by each branch in its
   * bifurcation is stored: VTK_VMTK_UPSTREAM_ORIENTATION (0) for the parent branch, or
   * VTK_VMTK_DOWNSTREAM_ORIENTATION (1) for a daughter branch.
   * Commonly named "BifurcationVectorsOrientation".
   */
  vtkSetStringMacro(BifurcationVectorsOrientationArrayName);
  vtkGetStringMacro(BifurcationVectorsOrientationArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output point data array where the group id of the bifurcation each
   * vector belongs to is stored.
   * Commonly named "BifurcationGroupIds".
   */
  vtkSetStringMacro(BifurcationGroupIdsArrayName);
  vtkGetStringMacro(BifurcationGroupIdsArrayName);
  ///@}

  ///@{
  /**
   * Toggle normalizing BifurcationVectors (and its in-plane/out-of-plane components) to unit length
   * before computing angles and storing the output arrays. Default: off.
   */
  vtkSetMacro(NormalizeBifurcationVectors,int);
  vtkGetMacro(NormalizeBifurcationVectors,int);
  vtkBooleanMacro(NormalizeBifurcationVectors,int);
  ///@}
//BTX
  /**
   * Values stored in the BifurcationVectorsOrientationArrayName output array, flagging whether a
   * branch is the parent (upstream) or a daughter (downstream) branch of its bifurcation.
   */
  enum
    {
    VTK_VMTK_UPSTREAM_ORIENTATION = 0,
    VTK_VMTK_DOWNSTREAM_ORIENTATION
    };
//ETX
  protected:
  vtkvmtkCenterlineBifurcationVectors();
  ~vtkvmtkCenterlineBifurcationVectors();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void ComputeBifurcationVectors(vtkPolyData* input, int bifurcationGroupId, vtkIdList* bifurcationVectorsGroupIds, vtkIntArray* bifurcationVectorsOrientation, vtkDoubleArray* bifurcationVectors, vtkPoints* bifurcationVectorsPoints);

  void ComputeBifurcationVectorComponents(int bifurcationGroupId, vtkDoubleArray* bifurcationVectors, vtkDoubleArray* inPlaneBifurcationVectors, vtkDoubleArray* outOfPlaneBifurcationVectors);
  
  void ComputeBifurcationVectorAngles(int bifurcationGroupId, vtkDoubleArray* bifurcationVectors, vtkDoubleArray* inPlaneBifurcationVectors, vtkDoubleArray* outOfPlaneBifurcationVectors, vtkDoubleArray* inPlaneBifurcationVectorAngles, vtkDoubleArray* outOfPlaneBifurcationVectorAngles);

  char* RadiusArrayName;
  char* GroupIdsArrayName;
  char* CenterlineIdsArrayName;
  char* TractIdsArrayName;
  char* BlankingArrayName;

  vtkPolyData* ReferenceSystems;

  char* ReferenceSystemGroupIdsArrayName;
  char* ReferenceSystemNormalArrayName;
  char* ReferenceSystemUpNormalArrayName;

  char* BifurcationVectorsArrayName;
  char* InPlaneBifurcationVectorsArrayName;
  char* OutOfPlaneBifurcationVectorsArrayName;
  char* BifurcationVectorsOrientationArrayName;

  char* InPlaneBifurcationVectorAnglesArrayName;
  char* OutOfPlaneBifurcationVectorAnglesArrayName;

  char* BifurcationGroupIdsArrayName;

  int NormalizeBifurcationVectors;
  
  private:
  vtkvmtkCenterlineBifurcationVectors(const vtkvmtkCenterlineBifurcationVectors&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineBifurcationVectors&);  // Not implemented.
};

#endif
