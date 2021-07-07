/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkBoundaryLayerGenerator.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:48 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

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
class vtkUnsignedCharArray;
class vtkDataArray;

class VTK_VMTK_MISC_EXPORT vtkvmtkBoundaryLayerGenerator : public vtkUnstructuredGridAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkBoundaryLayerGenerator,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

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

  vtkGetMacro(IncludeSidewallCells,int);
  vtkSetMacro(IncludeSidewallCells,int);
  vtkBooleanMacro(IncludeSidewallCells,int);

  vtkGetMacro(NegateWarpVectors,int);
  vtkSetMacro(NegateWarpVectors,int);
  vtkBooleanMacro(NegateWarpVectors,int);

  vtkGetMacro(LayerThickness,double);
  vtkSetMacro(LayerThickness,double);

  vtkGetMacro(LayerThicknessRatio,double);
  vtkSetMacro(LayerThicknessRatio,double);

  vtkGetMacro(MaximumLayerThickness,double);
  vtkSetMacro(MaximumLayerThickness,double);

  vtkGetMacro(NumberOfSubLayers,int);
  vtkSetMacro(NumberOfSubLayers,int);

  vtkGetMacro(SubLayerRatio,double);
  vtkSetMacro(SubLayerRatio,double);

  vtkGetMacro(NumberOfSubsteps,int);
  vtkSetMacro(NumberOfSubsteps,int);

  vtkGetMacro(Relaxation,double);
  vtkSetMacro(Relaxation,double);

  vtkGetMacro(LocalCorrectionFactor,double);
  vtkSetMacro(LocalCorrectionFactor,double);

  vtkSetStringMacro(CellEntityIdsArrayName);
  vtkGetStringMacro(CellEntityIdsArrayName);

  vtkGetMacro(InnerSurfaceCellEntityId,int);
  vtkSetMacro(InnerSurfaceCellEntityId,int);

  vtkGetMacro(OuterSurfaceCellEntityId,int);
  vtkSetMacro(OuterSurfaceCellEntityId,int);

  vtkGetMacro(SidewallCellEntityId,int);
  vtkSetMacro(SidewallCellEntityId,int);

  vtkGetMacro(VolumeCellEntityId,int);
  vtkSetMacro(VolumeCellEntityId,int);

  vtkGetObjectMacro(InnerSurface,vtkUnstructuredGrid);

  protected:
  vtkvmtkBoundaryLayerGenerator();
  ~vtkvmtkBoundaryLayerGenerator();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void BuildWarpVectors(vtkUnstructuredGrid* input);
  void IncrementalWarpPoints(vtkUnstructuredGrid* input, vtkPoints* basePoints, vtkPoints* warpedPoints, int substep, int numberOfSubsteps, double relaxation);
  void IncrementalWarpVectors(vtkUnstructuredGrid* input, int numberOfSubsteps, double relaxation);
  int CheckTangle(vtkUnstructuredGrid* input, vtkUnsignedCharArray* checkArray);
  void LocalUntangle(vtkUnstructuredGrid* input, vtkUnsignedCharArray* checkArray, double alpha); 
  void WarpPoints(vtkPoints* inputPoints, vtkPoints* warpedPoints, int subLayerId, bool quadratic);
  void UnwrapSublayers(vtkUnstructuredGrid* input, vtkPoints* outputPoints);

  vtkDataArray* WarpVectorsArray;
  vtkDataArray* LayerThicknessArray;

  int UseWarpVectorMagnitudeAsThickness;
  int ConstantThickness;

  char* WarpVectorsArrayName;
  char* LayerThicknessArrayName;

  double LayerThickness;
  double LayerThicknessRatio;
  double MaximumLayerThickness;

  int NumberOfSubLayers;
  int NumberOfSubsteps;
  double SubLayerRatio;

  int IncludeSurfaceCells;
  int IncludeSidewallCells;
  int NegateWarpVectors;

  vtkUnstructuredGrid* InnerSurface;

  char* CellEntityIdsArrayName;
  int InnerSurfaceCellEntityId;
  int OuterSurfaceCellEntityId;
  int SidewallCellEntityId;
  int VolumeCellEntityId;

  double Relaxation;
  double LocalCorrectionFactor;

  private:
  vtkvmtkBoundaryLayerGenerator(const vtkvmtkBoundaryLayerGenerator&);  // Not implemented.
  void operator=(const vtkvmtkBoundaryLayerGenerator&);  // Not implemented.
};

#endif
