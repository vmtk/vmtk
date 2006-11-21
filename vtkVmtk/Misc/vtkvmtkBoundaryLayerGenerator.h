/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkBoundaryLayerGenerator.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:48 $
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
  // .NAME vtkvmtkBoundaryLayerGenerator - Generates boundary layers of prismatic elements by warping a surface mesh.
  // .SECTION Description
  // ...

#ifndef __vtkvmtkBoundaryLayerGenerator_h
#define __vtkvmtkBoundaryLayerGenerator_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class vtkPoints;
class vtkDataArray;

class VTK_VMTK_MISC_EXPORT vtkvmtkBoundaryLayerGenerator : public vtkUnstructuredGridAlgorithm
{
  public: 
  vtkTypeRevisionMacro(vtkvmtkBoundaryLayerGenerator,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent); 

  static vtkvmtkBoundaryLayerGenerator *New();
  
  vtkGetStringMacro(WarpVectorsArrayName);
  vtkSetStringMacro(WarpVectorsArrayName);

  vtkGetStringMacro(LayerThicknessArrayName);
  vtkSetStringMacro(LayerThicknessArrayName);

  vtkGetMacro(UseWarpVectorMagnitudeAsThickness,int);
  vtkSetMacro(UseWarpVectorMagnitudeAsThickness,int);
  vtkBooleanMacro(UseWarpVectorMagnitudeAsThickness,int);

  vtkGetMacro(ConstantThickness,int);
  vtkSetMacro(ConstantThickness,int);
  vtkBooleanMacro(ConstantThickness,int);

  vtkGetMacro(IncludeSurfaceCells,int);
  vtkSetMacro(IncludeSurfaceCells,int);
  vtkBooleanMacro(IncludeSurfaceCells,int);

  vtkGetMacro(LayerThickness,double);
  vtkSetMacro(LayerThickness,double);

  vtkGetMacro(LayerThicknessRatio,double);
  vtkSetMacro(LayerThicknessRatio,double);

  vtkGetMacro(MaxvmtkmLayerThickness,double);
  vtkSetMacro(MaxvmtkmLayerThickness,double);

  vtkGetMacro(NumberOfSubLayers,int);
  vtkSetMacro(NumberOfSubLayers,int);

  vtkGetMacro(SubLayerRatio,double);
  vtkSetMacro(SubLayerRatio,double);

  protected:
  vtkvmtkBoundaryLayerGenerator();
  ~vtkvmtkBoundaryLayerGenerator();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  void WarpPoints(vtkPoints* inputPoints, vtkPoints* warpedPoints, vtkIdType subLayerId, bool quadratic);

  vtkDataArray* WarpVectorsArray;
  vtkDataArray* LayerThicknessArray;

  int UseWarpVectorMagnitudeAsThickness;
  int ConstantThickness;

  char* WarpVectorsArrayName;
  char* LayerThicknessArrayName;

  double LayerThickness;
  double LayerThicknessRatio;
  double MaxvmtkmLayerThickness;

  int NumberOfSubLayers;
  double SubLayerRatio;

  int IncludeSurfaceCells;

  private:
  vtkvmtkBoundaryLayerGenerator(const vtkvmtkBoundaryLayerGenerator&);  // Not implemented.
  void operator=(const vtkvmtkBoundaryLayerGenerator&);  // Not implemented.
};

#endif
