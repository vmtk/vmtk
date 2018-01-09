/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataDiscreteElasticaFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
  Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkPolyDataDiscreteElasticaFilter - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataDiscreteElasticaFilter_h
#define __vtkvmtkPolyDataDiscreteElasticaFilter_h

#include "vtkObject.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataDiscreteElasticaFilter : public vtkPolyDataAlgorithm
{
public:

  static vtkvmtkPolyDataDiscreteElasticaFilter *New();
  vtkTypeMacro(vtkvmtkPolyDataDiscreteElasticaFilter,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE; 

protected:
  vtkvmtkPolyDataDiscreteElasticaFilter();
  ~vtkvmtkPolyDataDiscreteElasticaFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;

private:
  vtkvmtkPolyDataDiscreteElasticaFilter(const vtkvmtkPolyDataDiscreteElasticaFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataDiscreteElasticaFilter&);  // Not implemented.
};

#endif

