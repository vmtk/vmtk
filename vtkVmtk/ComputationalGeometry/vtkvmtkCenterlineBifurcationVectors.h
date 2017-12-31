/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineBifurcationVectors.h,v $
Language:  C++
Date:      $Date: 2006/10/17 15:16:16 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
  // .NAME vtkvmtkCenterlineBifurcationVectors - ...
  // .SECTION Description
  // ...

#ifndef __vtkvmtkCenterlineBifurcationVectors_h
#define __vtkvmtkCenterlineBifurcationVectors_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"
#include "vtkPolyData.h"

class vtkPoints;
class vtkDoubleArray;
class vtkIntArray;
  
class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineBifurcationVectors : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkCenterlineBifurcationVectors,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE; 

  static vtkvmtkCenterlineBifurcationVectors* New();

  vtkSetStringMacro(RadiusArrayName);
  vtkGetStringMacro(RadiusArrayName);

  vtkSetStringMacro(GroupIdsArrayName);
  vtkGetStringMacro(GroupIdsArrayName);

  vtkSetStringMacro(CenterlineIdsArrayName);
  vtkGetStringMacro(CenterlineIdsArrayName);

  vtkSetStringMacro(TractIdsArrayName);
  vtkGetStringMacro(TractIdsArrayName);

  vtkSetStringMacro(BlankingArrayName);
  vtkGetStringMacro(BlankingArrayName);

  vtkSetObjectMacro(ReferenceSystems,vtkPolyData);
  vtkGetObjectMacro(ReferenceSystems,vtkPolyData);

  vtkSetStringMacro(ReferenceSystemGroupIdsArrayName);
  vtkGetStringMacro(ReferenceSystemGroupIdsArrayName);

  vtkSetStringMacro(ReferenceSystemNormalArrayName);
  vtkGetStringMacro(ReferenceSystemNormalArrayName);

  vtkSetStringMacro(ReferenceSystemUpNormalArrayName);
  vtkGetStringMacro(ReferenceSystemUpNormalArrayName);

  vtkSetStringMacro(BifurcationVectorsArrayName);
  vtkGetStringMacro(BifurcationVectorsArrayName);

  vtkSetStringMacro(InPlaneBifurcationVectorsArrayName);
  vtkGetStringMacro(InPlaneBifurcationVectorsArrayName);

  vtkSetStringMacro(OutOfPlaneBifurcationVectorsArrayName);
  vtkGetStringMacro(OutOfPlaneBifurcationVectorsArrayName);

  vtkSetStringMacro(InPlaneBifurcationVectorAnglesArrayName);
  vtkGetStringMacro(InPlaneBifurcationVectorAnglesArrayName);

  vtkSetStringMacro(OutOfPlaneBifurcationVectorAnglesArrayName);
  vtkGetStringMacro(OutOfPlaneBifurcationVectorAnglesArrayName);

  vtkSetStringMacro(BifurcationVectorsOrientationArrayName);
  vtkGetStringMacro(BifurcationVectorsOrientationArrayName);

  vtkSetStringMacro(BifurcationGroupIdsArrayName);
  vtkGetStringMacro(BifurcationGroupIdsArrayName);

  vtkSetMacro(NormalizeBifurcationVectors,int);
  vtkGetMacro(NormalizeBifurcationVectors,int);
  vtkBooleanMacro(NormalizeBifurcationVectors,int);
//BTX
  enum
    {
    VTK_VMTK_UPSTREAM_ORIENTATION = 0,
    VTK_VMTK_DOWNSTREAM_ORIENTATION
    };
//ETX
  protected:
  vtkvmtkCenterlineBifurcationVectors();
  ~vtkvmtkCenterlineBifurcationVectors();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;

  void ComputeBifurcationVectors(vtkPolyData* input, int bifurcationGroupId, vtkIdList* bifurcationVectorsGroupIds, vtkIntArray* bifurcationVectorsOrientation, vtkDoubleArray* bifurcationVectors, vtkPoints* bifurcationVectorsPoints);

  void ComputeBifurcationVectorComponents(int bifurcationGroupId, vtkDoubleArray* bifurcationVectors, vtkDoubleArray* inPlaneBifurcationVectors, vtkDoubleArray* outOfPlaneBifurcationVectors);
  
  void ComputeBifurcationVectorAngles(int bifurcationGroupId, vtkDoubleArray* bifurcationVectors, vtkDoubleArray* inPlaneBifurcationVectors, vtkDoubleArray* outOfPlaneBifurcationVectors, vtkDoubleArray* inPlaneBifurcationVectorAngles, vtkDoubleArray* outOfPlaneBifurcationVectorAngles);

  char* RadiusArrayName;
  char* GroupIdsArrayName;
  char* CenterlineIdsArrayName;
  char* TractIdsArrayName;
  char* BlankingArrayName;

  vtkPolyData* ReferenceSystems;

  char* ReferenceSystemGroupIdsArrayName;
  char* ReferenceSystemNormalArrayName;
  char* ReferenceSystemUpNormalArrayName;

  char* BifurcationVectorsArrayName;
  char* InPlaneBifurcationVectorsArrayName;
  char* OutOfPlaneBifurcationVectorsArrayName;
  char* BifurcationVectorsOrientationArrayName;

  char* InPlaneBifurcationVectorAnglesArrayName;
  char* OutOfPlaneBifurcationVectorAnglesArrayName;

  char* BifurcationGroupIdsArrayName;

  int NormalizeBifurcationVectors;
  
  private:
  vtkvmtkCenterlineBifurcationVectors(const vtkvmtkCenterlineBifurcationVectors&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineBifurcationVectors&);  // Not implemented.
};

#endif
