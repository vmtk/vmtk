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
 * @class   vtkvmtkPolyDataCenterlineSections
 * @brief   Extract a set of vessel sections from a surface along a centerline.
 * @ingroup ComputationalGeometry
 *
 *  Unlike vtkvmtkPolyDataBranchSections/vtkvmtkPolyDataBifurcationSections (which cut one section
 *  at a fixed sphere-distance per branch/bifurcation), this class cuts one cross-section at every
 *  point of the input Centerlines, giving a dense series of sections along the whole vessel.
 *
 *  The set of vessel sections contain the profile as well as the the following information about the section:
 *  - Centerline Section Area
 *  - Centerline Section Min Size
 *  - Centerline Section Max Size
 *  - Centerline Section Shape
 *  - Centerline Section Closed
 *
 * @sa
 * vtkvmtkPolyDataBranchSections, vtkvmtkPolyDataBifurcationSections
 */

#ifndef __vtkvmtkPolyDataCenterlineSections_h
#define __vtkvmtkPolyDataCenterlineSections_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"
#include "vtkPolyData.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataCenterlineSections : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkPolyDataCenterlineSections,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkPolyDataCenterlineSections* New();

  ///@{
  /**
   * Set/Get the centerline(s) along which sections are cut, one per centerline point. Required
   * input.
   */
  vtkSetObjectMacro(Centerlines,vtkPolyData);
  vtkGetObjectMacro(Centerlines,vtkPolyData);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the cross-sectional area of each section is
   * stored.
   * Commonly named "CenterlineSectionArea".
   */
  vtkSetStringMacro(CenterlineSectionAreaArrayName);
  vtkGetStringMacro(CenterlineSectionAreaArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the minimum diameter of each section is
   * stored.
   * Commonly named "CenterlineSectionMinSize".
   */
  vtkSetStringMacro(CenterlineSectionMinSizeArrayName);
  vtkGetStringMacro(CenterlineSectionMinSizeArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the maximum diameter of each section is
   * stored.
   * Commonly named "CenterlineSectionMaxSize".
   */
  vtkSetStringMacro(CenterlineSectionMaxSizeArrayName);
  vtkGetStringMacro(CenterlineSectionMaxSizeArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the shape index of each section (the ratio
   * between its minimum and maximum diameter) is stored.
   * Commonly named "CenterlineSectionShape".
   */
  vtkSetStringMacro(CenterlineSectionShapeArrayName);
  vtkGetStringMacro(CenterlineSectionShapeArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where, for each section, whether the cutting
   * plane fully closes off the vessel lumen (1) or not (0) is stored.
   * Commonly named "CenterlineSectionClosed".
   */
  vtkSetStringMacro(CenterlineSectionClosedArrayName);
  vtkGetStringMacro(CenterlineSectionClosedArrayName);
  ///@}

  protected:
  vtkvmtkPolyDataCenterlineSections();
  ~vtkvmtkPolyDataCenterlineSections();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void ComputeCenterlineSections(vtkPolyData* input, int cellId, vtkPolyData* output);

  vtkPolyData* Centerlines;

  char* CenterlineSectionAreaArrayName;
  char* CenterlineSectionMinSizeArrayName;
  char* CenterlineSectionMaxSizeArrayName;
  char* CenterlineSectionShapeArrayName;
  char* CenterlineSectionClosedArrayName;

  private:
  vtkvmtkPolyDataCenterlineSections(const vtkvmtkPolyDataCenterlineSections&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataCenterlineSections&);  // Not implemented.
};

#endif
