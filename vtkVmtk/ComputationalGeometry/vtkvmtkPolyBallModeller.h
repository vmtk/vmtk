/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyBallModeller.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// .NAME vtkvmtkPolyBallModeller - sample poly ball onto structured points 
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyBallModeller_h
#define __vtkvmtkPolyBallModeller_h

#include "vtkImageAlgorithm.h"
#include "vtkImageData.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyBallModeller : public vtkImageAlgorithm 
{
  public:
  vtkTypeMacro(vtkvmtkPolyBallModeller,vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  static vtkvmtkPolyBallModeller *New();
  
  // Description:
  // Specify i-j-k dimensions on which to sample polyball function.
  vtkGetVectorMacro(SampleDimensions,int,3);
  vtkSetVectorMacro(SampleDimensions,int,3);
  
  // Description:
  // Specify the position in space to perform the sampling.
  vtkSetVectorMacro(ModelBounds,double,6);
  vtkGetVectorMacro(ModelBounds,double,6);

  vtkSetObjectMacro(ReferenceImage,vtkImageData);
  vtkGetObjectMacro(ReferenceImage,vtkImageData);
 
  vtkSetStringMacro(RadiusArrayName);
  vtkGetStringMacro(RadiusArrayName);

  vtkSetMacro(UsePolyBallLine,int);
  vtkGetMacro(UsePolyBallLine,int);
  vtkBooleanMacro(UsePolyBallLine,int);

  vtkSetMacro(NegateFunction,int);
  vtkGetMacro(NegateFunction,int);
  vtkBooleanMacro(NegateFunction,int);


  protected:
  vtkvmtkPolyBallModeller();
  ~vtkvmtkPolyBallModeller();

  int FillInputPortInformation(int, vtkInformation *info) VTK_OVERRIDE;
  virtual int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;

  int SampleDimensions[3];
  double ModelBounds[6];

  char* RadiusArrayName;

  int UsePolyBallLine;

  int NegateFunction;

  vtkImageData* ReferenceImage;

  private:
  vtkvmtkPolyBallModeller(const vtkvmtkPolyBallModeller&);  // Not implemented.
  void operator=(const vtkvmtkPolyBallModeller&);  // Not implemented.
};

#endif


