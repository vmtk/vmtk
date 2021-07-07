/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkImageBoxPainter.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:47:48 $
  Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkImageBoxPainter - Create a box shaped sub-volume within a 3D image and paint (assign a value to) every sub-volume element with the same value.
// .SECTION Description
// ..

#ifndef __vtkvmtkImageBoxPainter_h
#define __vtkvmtkImageBoxPainter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkImageBoxPainter : public vtkSimpleImageToImageFilter
{
public:
  static vtkvmtkImageBoxPainter *New();
  vtkTypeMacro(vtkvmtkImageBoxPainter,vtkSimpleImageToImageFilter);

  vtkGetMacro(PaintValue,double);
  vtkSetMacro(PaintValue,double);

  vtkGetVectorMacro(BoxBounds,double,6);
  vtkSetVectorMacro(BoxBounds,double,6);

  vtkGetVectorMacro(BoxExtent,int,6);
  vtkSetVectorMacro(BoxExtent,int,6);

  vtkGetMacro(BoxDefinition,int);
  vtkSetMacro(BoxDefinition,int);
  void SetBoxDefinitionToUseExtent()
    { this->BoxDefinition = vtkvmtkImageBoxPainter::USE_EXTENT; }
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







