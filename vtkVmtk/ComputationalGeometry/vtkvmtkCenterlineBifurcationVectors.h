/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineBifurcationVectors.h,v $
Language:  C++
Date:      $Date: 2006/10/17 15:16:16 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkCenterlineBifurcationVectors - Compute vectors which represent the orientation of branches involved within a bifurcation from a split centerline and bifurcation reference system.
// .SECTION Description
//  For each branch of a bifurcation we compute bifurcation vectors, bifurcation vector components, and bifurcation vector angles. These represent both the in-plane and out-of-plane orientation of the bifurcation branches (which is why we need the bifurcation reference system). This allows us to later compute factors which characterize the bifurcation such as: relationship between the angle of upstream vs downstream vessels entering/leaving the bifurcation, how broad is the bifurcation, how tight is the bifurcation. 
//
//  This class Computes vectors which are stored as results for the following data arrays:
//  - BifurcationVectors
//  - InPlaneBifurcationVectors
//  - OutOfPlaneBifurcationVectors
//  - InPlaneBifurcationVectorAngles (the angle between the InPlaneBifurcationVectors and the bifurcation UpNormal, in radians, from -pi to pi, zero for a UpNormal oriented vector, positive in the clockwise direction with respect to the bifurcation Normal)
//  - OutOfPlaneBifurcationVectorAngles (the angle between the BifurcationVectors and the bifurcation plane, in radians, positive if the OutOfPLaneBifurcationVector is directed as the bifurcation Normal)
//  - BifurcationVectorsOrientation (flag accounting for the role played by the branch in the bifurcation, 0 for upstream, 1 for downstream the bifurcation)
//  - GroupIds (the groupId of the branch described by the vector)
//  - BifurcationGroupIds (the groupId of the bifurcation)

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
  void PrintSelf(ostream& os, vtkIndent indent) override; 

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

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

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
