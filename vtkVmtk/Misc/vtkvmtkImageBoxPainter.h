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
 * @class   vtkvmtkImageBoxPainter
 * @brief   Create a box shaped sub-volume within a 3D image and paint (assign a value to) every sub-volume element with the same value.
 * @ingroup Misc
 *
 * This filter copies the input image and overwrites every voxel inside an axis-aligned box
 * sub-region with PaintValue, leaving voxels outside the box unchanged. The box can be specified
 * either as a voxel index range (BoxExtent) or as physical bounds (BoxBounds, converted to the
 * nearest enclosed extent using the image spacing); which one is used is selected with
 * BoxDefinition / SetBoxDefinitionToUseExtent / SetBoxDefinitionToUseBounds.
 *
 * This is the filter behind the vmtkimagevoipainter pype script, typically used to blank out or
 * flag a rectangular region of interest in an image (e.g. to mask out part of a scan before
 * further processing).
 */

#ifndef __vtkvmtkImageBoxPainter_h
#define __vtkvmtkImageBoxPainter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkImageBoxPainter : public vtkSimpleImageToImageFilter
{
public:
  static vtkvmtkImageBoxPainter *New();
  vtkTypeMacro(vtkvmtkImageBoxPainter,vtkSimpleImageToImageFilter);

  ///@{
  /**
   * Set/Get the scalar value assigned to every voxel inside the box region. Default: 0.0.
   */
  vtkGetMacro(PaintValue,double);
  vtkSetMacro(PaintValue,double);
  ///@}

  ///@{
  /**
   * Set/Get the box region in physical (world) coordinates, as (xmin,xmax,ymin,ymax,zmin,zmax).
   * Only used when BoxDefinition is USE_BOUNDS (see SetBoxDefinitionToUseBounds); converted
   * internally to the nearest enclosed voxel extent using the input image spacing.
   */
  vtkGetVectorMacro(BoxBounds,double,6);
  vtkSetVectorMacro(BoxBounds,double,6);
  ///@}

  ///@{
  /**
   * Set/Get the box region in voxel index coordinates, as
   * (imin,imax,jmin,jmax,kmin,kmax). Used directly when BoxDefinition is USE_EXTENT (see
   * SetBoxDefinitionToUseExtent, the default); also holds the result of converting BoxBounds when
   * BoxDefinition is USE_BOUNDS.
   */
  vtkGetVectorMacro(BoxExtent,int,6);
  vtkSetVectorMacro(BoxExtent,int,6);
  ///@}

  ///@{
  /**
   * Set/Get whether the painted box region is specified through BoxExtent (USE_EXTENT, the
   * default) or through BoxBounds (USE_BOUNDS). Prefer the SetBoxDefinitionToUseExtent /
   * SetBoxDefinitionToUseBounds convenience methods over setting this enum value directly.
   */
  vtkGetMacro(BoxDefinition,int);
  vtkSetMacro(BoxDefinition,int);
  ///@}

  /**
   * Specify the painted box region through BoxExtent (voxel index range). See BoxDefinition.
   */
  void SetBoxDefinitionToUseExtent()
    { this->BoxDefinition = vtkvmtkImageBoxPainter::USE_EXTENT; }
  /**
   * Specify the painted box region through BoxBounds (physical coordinate range). See
   * BoxDefinition.
   */
  void SetBoxDefinitionToUseBounds()
    { this->BoxDefinition = vtkvmtkImageBoxPainter::USE_BOUNDS; }

protected:

  vtkvmtkImageBoxPainter();
  ~vtkvmtkImageBoxPainter() {};

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

  double PaintValue;
  double BoxBounds[6];
  int BoxExtent[6];
  
  int BoxDefinition;
  //BTX
  enum
    {
      USE_BOUNDS,
      USE_EXTENT
    };
  //ETX

private:
  vtkvmtkImageBoxPainter(const vtkvmtkImageBoxPainter&);  // Not implemented.
  void operator=(const vtkvmtkImageBoxPainter&);  // Not implemented.
};

#endif







