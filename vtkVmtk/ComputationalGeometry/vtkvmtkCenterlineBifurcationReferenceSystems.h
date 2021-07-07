/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineBifurcationReferenceSystems.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkCenterlineBifurcationReferenceSystems - Generates a normal and upnormal reference system for each bifurcation point of a split centerline. 
// .SECTION Description
// The "normal" vector is the normal to the bifurcation plane (which is the best fit plane in relation to bifurcation reference points). The upnormal vector is normal to the "normal" vector pointing downstream from the parent artery. 

#ifndef __vtkvmtkCenterlineBifurcationReferenceSystems_h
#define __vtkvmtkCenterlineBifurcationReferenceSystems_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class vtkPoints;
class vtkDoubleArray;
class vtkIntArray;
  
class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineBifurcationReferenceSystems : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkCenterlineBifurcationReferenceSystems,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkCenterlineBifurcationReferenceSystems* New();

  vtkSetStringMacro(RadiusArrayName);
  vtkGetStringMacro(RadiusArrayName);

  vtkSetStringMacro(GroupIdsArrayName);
  vtkGetStringMacro(GroupIdsArrayName);

  vtkSetStringMacro(BlankingArrayName);
  vtkGetStringMacro(BlankingArrayName);

  vtkSetStringMacro(NormalArrayName);
  vtkGetStringMacro(NormalArrayName);

  vtkSetStringMacro(UpNormalArrayName);
  vtkGetStringMacro(UpNormalArrayName);

  protected:
  vtkvmtkCenterlineBifurcationReferenceSystems();
  ~vtkvmtkCenterlineBifurcationReferenceSystems();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void ComputeGroupReferenceSystem(vtkPolyData* input, int referenceGroupId, vtkPoints* outputPoints, vtkDoubleArray* normalArray, vtkDoubleArray* upNormalArray, vtkIntArray* referenceGroupIdsArray);

  char* RadiusArrayName;
  char* GroupIdsArrayName;
  char* BlankingArrayName;

  char* NormalArrayName;
  char* UpNormalArrayName;

  private:
  vtkvmtkCenterlineBifurcationReferenceSystems(const vtkvmtkCenterlineBifurcationReferenceSystems&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineBifurcationReferenceSystems&);  // Not implemented.
};

#endif
