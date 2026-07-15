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
 * @class   vtkvmtkCenterlineSplittingAndGroupingFilter
 * @brief   Multipurpose filter used to bundle and blank split tracts in all "branch extractor" like methods.
 * @ingroup ComputationalGeometry
 *
 * In order to extract branches of a bifurcation, centerlines are split into multiple tracts. This filter is responsible for groups tracts together into logical bundles (aka "branches"), and describing which tract is blanked at an n-furcation and which are not.
 *
 * This is an abstract base class: it implements the common tract-grouping/blanking/merging logic,
 * but defers the actual choice of *where* to split each centerline to subclasses, which must
 * implement ComputeCenterlineSplitting. Concrete subclasses include
 * vtkvmtkCenterlineBranchExtractor (splits at bifurcations),
 * vtkvmtkCenterlineEndpointExtractor (splits at free ends), and
 * vtkvmtkCenterlineSplitExtractor (splits at an arbitrary point).
 *
 * @sa
 * vtkvmtkCenterlineBranchExtractor, vtkvmtkCenterlineEndpointExtractor, vtkvmtkCenterlineSplitExtractor
 */

#ifndef __vtkvmtkCenterlineSplittingAndGroupingFilter_h
#define __vtkvmtkCenterlineSplittingAndGroupingFilter_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"
  
class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineSplittingAndGroupingFilter : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkCenterlineSplittingAndGroupingFilter,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

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
   * Set/Get the name of the output cell data array where the branch group id computed for each split
   * tract is stored.
   * Commonly named "GroupIds".
   */
  vtkSetStringMacro(GroupIdsArrayName);
  vtkGetStringMacro(GroupIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the id of the original, unsplit centerline
   * that each tract belongs to is stored.
   * Commonly named "CenterlineIds".
   */
  vtkSetStringMacro(CenterlineIdsArrayName);
  vtkGetStringMacro(CenterlineIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where, for each tract, whether it is "blanked"
   * (a redundant, overlapping tract introduced by splitting, e.g. within a bifurcation region) is
   * stored.
   * Commonly named "Blanking".
   */
  vtkSetStringMacro(BlankingArrayName);
  vtkGetStringMacro(BlankingArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the tract id (position of a tract along its
   * original centerline) is stored.
   * Commonly named "TractIds".
   */
  vtkSetStringMacro(TractIdsArrayName);
  vtkGetStringMacro(TractIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the strategy used by GroupTracts to decide which tracts belong to the same branch:
   * FIRSTPOINT and LASTPOINT group tracts sharing a coincident first/last endpoint, while
   * POINTINTUBE (default) groups tracts whose points lie within each other's tube envelope. See also
   * SetGroupingModeToFirstPoint / SetGroupingModeToLastPoint / SetGroupingModeToPointInTube.
   */
  vtkSetMacro(GroupingMode,int);
  vtkGetMacro(GroupingMode,int);
  ///@}

  /**
   * Convenience method: set GroupingMode to group tracts sharing a coincident first endpoint.
   */
  void SetGroupingModeToFirstPoint()
  { this->SetGroupingMode(FIRSTPOINT); }

  /**
   * Convenience method: set GroupingMode to group tracts sharing a coincident last endpoint.
   */
  void SetGroupingModeToLastPoint()
  { this->SetGroupingMode(LASTPOINT); }

  /**
   * Convenience method: set GroupingMode to group tracts whose points lie within each other's tube
   * envelope (default).
   */
  void SetGroupingModeToPointInTube()
  { this->SetGroupingMode(POINTINTUBE); }

//BTX
  /**
   * Values for GroupingMode: strategy used to decide which split tracts belong to the same branch.
   */
  enum {
    FIRSTPOINT,
    LASTPOINT,
    POINTINTUBE
  };
//ETX

  protected:
  vtkvmtkCenterlineSplittingAndGroupingFilter();
  ~vtkvmtkCenterlineSplittingAndGroupingFilter();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  virtual void ComputeCenterlineSplitting(vtkPolyData* input, vtkIdType cellId) = 0;

  virtual void GroupTracts(vtkPolyData* input, vtkPolyData* centerlineTracts);
  
  void CoincidentExtremePointGroupTracts(vtkPolyData* input, vtkPolyData* centerlineTracts, bool first = true);
  void PointInTubeGroupTracts(vtkPolyData* input, vtkPolyData* centerlineTracts);

  virtual void MergeTracts(vtkPolyData* centerlineTracts);

  void SplitCenterline(vtkPolyData* input, vtkIdType cellId, int numberOfSplittingPoints, const vtkIdType* subIds, const double* pcoords, const int* tractBlanking, vtkPolyData* splitCenterline);

  void MakeGroupIdsAdjacent(vtkPolyData* centerlineTracts);
  void MakeTractIdsAdjacent(vtkPolyData* centerlineTracts);

  char* RadiusArrayName;
  char* GroupIdsArrayName;
  char* CenterlineIdsArrayName;
  char* BlankingArrayName;
  char* TractIdsArrayName;

  int NumberOfSplittingPoints;
  vtkIdType* SubIds;
  double* PCoords;
  int* TractBlanking;
  int GroupingMode;

  private:
  vtkvmtkCenterlineSplittingAndGroupingFilter(const vtkvmtkCenterlineSplittingAndGroupingFilter&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineSplittingAndGroupingFilter&);  // Not implemented.
};

#endif
