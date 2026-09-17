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

  /**
   * Cut a section of input at each centerline point and store it in output, with its properties
   * both on output and on the centerline point. The section arrays are created here, so the array
   * names must be set; cell data arrays already on output are kept. Points are visited in the order
   * of the centerline cells and of the points in each cell; a point shared by several cells is
   * visited once. A point gets no section if ComputeSectionPlane returns false for it or the plane
   * does not cut input. Progress is reported from 0 to 1 over the points. If an abort is requested,
   * it returns before all points are visited.
   */
  void ComputeCenterlineSections(vtkPolyData* input, vtkPolyData* output);

  /**
   * Compute the plane of the section at a centerline point, given as the index cellPointIndex of
   * the point within centerline cell cellId. Return false to cut no section at that point.
   * The default places the plane at the point, perpendicular to the average of the directions from
   * the previous and to the next distinct point of the cell, and returns false only if the cell has
   * no point distinct from it.
   * Subclasses can override it, for example to use tangents computed in advance.
   */
  virtual bool ComputeSectionPlane(vtkIdType cellId, vtkIdType cellPointIndex, double origin[3], double normal[3]);

  /**
   * Cut input with the plane specified by origin and normal. The default calls
   * vtkvmtkPolyDataBranchSections::ExtractCylinderSection. Subclasses can override it, for example
   * to keep attributes of input on the section.
   * To save time, ComputeCenterlineSections calls it with only the cells of input that the plane
   * crosses, sharing the points and point data of input, so overrides must depend only on the cut
   * of the cells they are given.
   */
  virtual void ExtractSection(vtkPolyData* input, double origin[3], double normal[3], vtkPolyData* section, bool & closed);

  /**
   * Called for each section after it has been added to the output as cell sectionId and its
   * properties have been stored. pointId is the centerline point the section was cut at. The default
   * does nothing. Subclasses can override it to compute and store additional section properties.
   */
  virtual void ProcessSection(vtkPolyData* input, vtkIdType pointId, vtkIdType sectionId, vtkPolyData* section, double origin[3], double normal[3], bool closed);

  /**
   * Return true if aborting the execution was requested, using vtkAlgorithm::CheckAbort where
   * available (VTK 9.4 and later), so that the output is also flagged as aborted.
   */
  bool AbortRequested();

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
