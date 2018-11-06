/*=========================================================================
                                                                                                                                    
Program:   VMTK
Module:    $RCSfile: vtkvmtkXdaReader.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:47 $
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

// #include <fstream>

#include "vtkvmtkXdaReader.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellType.h"
#include "vtkCell.h"
#include "vtkIdTypeArray.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkvmtkConstants.h"


vtkStandardNewMacro(vtkvmtkXdaReader);

vtkvmtkXdaReader::vtkvmtkXdaReader()
{
  this->BoundaryDataArrayName = NULL;
}

vtkvmtkXdaReader::~vtkvmtkXdaReader()
{
  if (this->BoundaryDataArrayName)
    {
    delete[] this->BoundaryDataArrayName;
    this->BoundaryDataArrayName = NULL;
    }
}

int vtkvmtkXdaReader::RequestData(
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  if (outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER()) > 0)
    {
    return 1;
    }

  if (!this->GetFileName())
    {
    vtkErrorMacro(<<"FileName not set.");
    return 1;
    }
  
  return 1;
}

void vtkvmtkXdaReader::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkUnstructuredGridReader::PrintSelf(os,indent);
}
