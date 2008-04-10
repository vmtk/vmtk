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
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyBallModeller : public vtkImageAlgorithm 
{
  public:
  vtkTypeRevisionMacro(vtkvmtkPolyBallModeller,vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkvmtkPolyBallModeller *New();
  
  // Description:
  // Specify i-j-k dimensions on which to sample polyball function.
  vtkGetVectorMacro(SampleDimensions,int,3);
  vtkSetVectorMacro(SampleDimensions,int,3);
  
  // Description:
  // Specify influence distance of each input point. 
  vtkSetMacro(MaximumDistance,double);
  vtkGetMacro(MaximumDistance,double);

  // Description:
  // Specify the position in space to perform the sampling.
  vtkSetVectorMacro(ModelBounds,double,6);
  vtkGetVectorMacro(ModelBounds,double,6);

  vtkSetStringMacro(RadiusArrayName);
  vtkGetStringMacro(RadiusArrayName);

  protected:
  vtkvmtkPolyBallModeller();
  ~vtkvmtkPolyBallModeller();

  int FillInputPortInformation(int, vtkInformation *info);
  virtual int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  int SampleDimensions[3];
  double MaximumDistance;
  double ModelBounds[6];

  char* RadiusArrayName;

  private:
  vtkvmtkPolyBallModeller(const vtkvmtkPolyBallModeller&);  // Not implemented.
  void operator=(const vtkvmtkPolyBallModeller&);  // Not implemented.
};

#endif


