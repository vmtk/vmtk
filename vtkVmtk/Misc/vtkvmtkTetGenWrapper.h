/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkTetGenWrapper.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:48 $
Version:   $Revision: 1.7 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
  // .NAME vtkvmtkTetGenWrapper - Converts linear elements to quadratic.
  // .SECTION Description
  // ...

#ifndef __vtkvmtkTetGenWrapper_h
#define __vtkvmtkTetGenWrapper_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkPointSet.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkTetGenWrapper : public vtkUnstructuredGridAlgorithm
{
  public: 
  static vtkvmtkTetGenWrapper *New();
  vtkTypeRevisionMacro(vtkvmtkTetGenWrapper,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent); 

  vtkSetMacro(PLC,int);
  vtkGetMacro(PLC,int);
  vtkBooleanMacro(PLC,int);
  
  vtkSetMacro(Refine,int);
  vtkGetMacro(Refine,int);
  vtkBooleanMacro(Refine,int);

  vtkSetMacro(NoBoundarySplit,int);
  vtkGetMacro(NoBoundarySplit,int);
  vtkBooleanMacro(NoBoundarySplit,int);

  vtkSetMacro(Quality,int);
  vtkGetMacro(Quality,int);
  vtkBooleanMacro(Quality,int);
  
  vtkSetMacro(MinRatio,double);
  vtkGetMacro(MinRatio,double);
  
  vtkSetMacro(VarVolume,int);
  vtkGetMacro(VarVolume,int);
  vtkBooleanMacro(VarVolume,int);

  vtkSetMacro(FixedVolume,int);
  vtkGetMacro(FixedVolume,int);
  vtkBooleanMacro(FixedVolume,int);

  vtkSetMacro(MaxVolume,double);
  vtkGetMacro(MaxVolume,double);

  vtkSetMacro(RemoveSliver,int);
  vtkGetMacro(RemoveSliver,int);
  vtkBooleanMacro(RemoveSliver,int);

  vtkSetMacro(MaxDihedral,double);
  vtkGetMacro(MaxDihedral,double);

  vtkSetMacro(InsertAddPoints,int);
  vtkGetMacro(InsertAddPoints,int);
  vtkBooleanMacro(InsertAddPoints,int);

  vtkSetMacro(RegionAttrib,int);
  vtkGetMacro(RegionAttrib,int);
  vtkBooleanMacro(RegionAttrib,int);

  vtkSetMacro(Epsilon,double);
  vtkGetMacro(Epsilon,double);

  vtkSetMacro(NoMerge,int);
  vtkGetMacro(NoMerge,int);
  vtkBooleanMacro(NoMerge,int);
  
  vtkSetMacro(DetectInter,int);
  vtkGetMacro(DetectInter,int);
  vtkBooleanMacro(DetectInter,int);

  vtkSetMacro(CheckClosure,int);
  vtkGetMacro(CheckClosure,int);
  vtkBooleanMacro(CheckClosure,int);

  vtkSetMacro(Order,int);
  vtkGetMacro(Order,int);

  vtkSetMacro(DoCheck,int);
  vtkGetMacro(DoCheck,int);
  vtkBooleanMacro(DoCheck,int);

  vtkSetMacro(Verbose,int);
  vtkGetMacro(Verbose,int);
  vtkBooleanMacro(Verbose,int);

  vtkSetStringMacro(PointMarkerArrayName);
  vtkGetStringMacro(PointMarkerArrayName);

  vtkSetStringMacro(FacetMarkerArrayName);
  vtkGetStringMacro(FacetMarkerArrayName);

  vtkSetStringMacro(TetrahedronVolumeArrayName);
  vtkGetStringMacro(TetrahedronVolumeArrayName);

  vtkSetObjectMacro(AdditionalPoints,vtkPointSet);
  vtkGetObjectMacro(AdditionalPoints,vtkPointSet);

  vtkSetMacro(OutputSurfaceElements,int);
  vtkGetMacro(OutputSurfaceElements,int);
  vtkBooleanMacro(OutputSurfaceElements,int);

  vtkSetMacro(OutputVolumeElements,int);
  vtkGetMacro(OutputVolumeElements,int);
  vtkBooleanMacro(OutputVolumeElements,int);

  vtkSetMacro(GenerateEntityArrays,int);
  vtkGetMacro(GenerateEntityArrays,int);
  vtkBooleanMacro(GenerateEntityArrays,int);

  vtkSetStringMacro(EntityArrayNamePrefix);
  vtkGetStringMacro(EntityArrayNamePrefix);

  protected:
  vtkvmtkTetGenWrapper();
  ~vtkvmtkTetGenWrapper();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  int PLC;
  int Refine;
  int NoBoundarySplit;
  int Quality;
  double MinRatio;
  int VarVolume;
  int FixedVolume;
  double MaxVolume;
  int RemoveSliver;
  double MaxDihedral;
  int InsertAddPoints;
  int RegionAttrib;
  double Epsilon;
  int NoMerge;
  int DetectInter;
  int CheckClosure;
  int Order;
  int DoCheck;
  int Verbose;

  vtkPointSet* AdditionalPoints;

  char* PointMarkerArrayName;
  char* FacetMarkerArrayName;
  char* TetrahedronVolumeArrayName;

  int OutputSurfaceElements;
  int OutputVolumeElements;

  int GenerateEntityArrays;

  char* EntityArrayNamePrefix;

  private:
  vtkvmtkTetGenWrapper(const vtkvmtkTetGenWrapper&);  // Not implemented.
  void operator=(const vtkvmtkTetGenWrapper&);  // Not implemented.
};

#endif
