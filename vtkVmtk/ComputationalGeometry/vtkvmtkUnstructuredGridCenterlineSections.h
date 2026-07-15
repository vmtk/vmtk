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
 * @class   vtkvmtkUnstructuredGridCenterlineSections
 * @brief   Obtain sections of an unstructured grid mesh that lie along a centerline path.
 * @ingroup ComputationalGeometry
 *
 * Obtain mesh sections (along a centerline) which pass through a dense unstructured grid mesh. This is useful because you can use this data to visualize velocity vectors (or other solutions) along many mesh sections intersecting the centerline.
 *
 * This is the filter behind the vmtkcenterlinemeshsections pype script, typically used to sample a
 * CFD solution mesh at a dense series of cross-sections along a centerline for post-processing
 * (e.g. visualizing velocity profiles). Despite deriving from vtkPolyDataAlgorithm, it accepts an
 * unstructured grid as input (see FillInputPortInformation).
 *
 * @sa
 * vtkvmtkPolyDataCenterlineSections
 */

#ifndef __vtkvmtkUnstructuredGridCenterlineSections_h
#define __vtkvmtkUnstructuredGridCenterlineSections_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"
#include "vtkPolyData.h"

class vtkUnstructuredGrid;
class vtkTransform;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkUnstructuredGridCenterlineSections : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkUnstructuredGridCenterlineSections,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkUnstructuredGridCenterlineSections* New();

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
   * Set/Get an optional poly data used as a template shape for each section outline instead of the
   * mesh's own intersection polygon; used when UseSectionSource is on.
   */
  vtkSetObjectMacro(SectionSource,vtkPolyData);
  vtkGetObjectMacro(SectionSource,vtkPolyData);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of Centerlines holding the section "up" direction used
   * to orient each cutting plane (together with SectionNormalsArrayName).
   */
  vtkSetStringMacro(SectionUpNormalsArrayName);
  vtkGetStringMacro(SectionUpNormalsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of Centerlines holding the cutting plane normal at
   * each centerline point.
   * Commonly named "SectionNormals".
   */
  vtkSetStringMacro(SectionNormalsArrayName);
  vtkGetStringMacro(SectionNormalsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of an additional 3-component point data array of Centerlines to be carried
   * through and transformed onto each section (e.g. a secondary reference direction), alongside
   * SectionNormalsArrayName/SectionUpNormalsArrayName.
   */
  vtkSetStringMacro(AdditionalNormalsArrayName);
  vtkGetStringMacro(AdditionalNormalsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of an additional scalar point data array of Centerlines to be carried through
   * onto each section (e.g. used as a per-section scaling factor when SourceScaling is on).
   */
  vtkSetStringMacro(AdditionalScalarsArrayName);
  vtkGetStringMacro(AdditionalScalarsArrayName);
  ///@}

  ///@{
  /**
   * Toggle transforming each section into a canonical local frame (aligning its normal/up-normal to
   * fixed axes) rather than leaving it in the original mesh coordinate system. Default: off.
   */
  vtkSetMacro(TransformSections,int);
  vtkGetMacro(TransformSections,int);
  vtkBooleanMacro(TransformSections,int);
  ///@}

  ///@{
  /**
   * Toggle using SectionSource as a template shape, transformed to each centerline point's local
   * frame, instead of intersecting the mesh with the cutting plane. Default: off.
   */
  vtkSetMacro(UseSectionSource,int);
  vtkGetMacro(UseSectionSource,int);
  vtkBooleanMacro(UseSectionSource,int);
  ///@}

  ///@{
  /**
   * Toggle scaling SectionSource at each centerline point by the value of
   * AdditionalScalarsArrayName (e.g. the local vessel radius), when UseSectionSource is on. Default:
   * off.
   */
  vtkSetMacro(SourceScaling,int);
  vtkGetMacro(SourceScaling,int);
  vtkBooleanMacro(SourceScaling,int);
  ///@}

  ///@{
  /**
   * Set/Get a constant (x,y,z) offset applied to the origin of every section. Default: (0,0,0).
   */
  vtkSetVectorMacro(OriginOffset,double,3);
  vtkGetVectorMacro(OriginOffset,double,3);
  ///@}

  ///@{
  /**
   * Set/Get the name of a 3-component point data array of the input mesh (e.g. a velocity field) to
   * be resampled onto each output section.
   */
  vtkSetStringMacro(VectorsArrayName);
  vtkGetStringMacro(VectorsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the id of the centerline point each section
   * was cut at is stored.
   * Commonly named "SectionIds".
   */
  vtkSetStringMacro(SectionIdsArrayName);
  vtkGetStringMacro(SectionIdsArrayName);
  ///@}

  /**
   * Get a poly data of vertex points, one per output section, located at each section's origin --
   * useful for visualizing where along the centerline each section was taken.
   */
  vtkGetObjectMacro(SectionPointsPolyData,vtkPolyData);

  protected:
  vtkvmtkUnstructuredGridCenterlineSections();
  ~vtkvmtkUnstructuredGridCenterlineSections();  

  int FillInputPortInformation(int, vtkInformation *info) override;

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  double ComputeAngle(double vector0[3], double vector1[3]);
  void CreateTransform(vtkTransform* transform, double currentOrigin[3], double currentNormal[3], double currentUpNormal[3], double targetOrigin[3], double targetNormal[3], double targetUpNormal[3]);

  vtkPolyData* Centerlines;
  vtkPolyData* SectionSource;
  vtkPolyData* SectionPointsPolyData;

  char* SectionUpNormalsArrayName;
  char* SectionNormalsArrayName;
  char* AdditionalNormalsArrayName;
  char* AdditionalScalarsArrayName;
  char* SectionIdsArrayName;

  char* VectorsArrayName;

  int TransformSections;

  int UseSectionSource;
  int SourceScaling;

  double OriginOffset[3];

  private:
  vtkvmtkUnstructuredGridCenterlineSections(const vtkvmtkUnstructuredGridCenterlineSections&);  // Not implemented.
  void operator=(const vtkvmtkUnstructuredGridCenterlineSections&);  // Not implemented.
};

#endif
