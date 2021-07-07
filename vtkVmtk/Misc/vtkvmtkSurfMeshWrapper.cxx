/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkSurfMeshWrapper.cxx,v $
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

#include "vtkvmtkSurfMeshWrapper.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "SurfaceMesher.h"


vtkStandardNewMacro(vtkvmtkSurfMeshWrapper);

vtkvmtkSurfMeshWrapper::vtkvmtkSurfMeshWrapper()
{
  this->NodeSpacing = 0.1;
}

vtkvmtkSurfMeshWrapper::~vtkvmtkSurfMeshWrapper()
{
}

class Remesh : public SurfaceMesher::RemeshSurface {
public:

  Remesh() 
    { this->NodeSpacing = 0.1; }
  Remesh(real nodeSpacing) 
    { this->NodeSpacing = nodeSpacing; }

  real NodeSpace(real *x) 
    {
    return this->NodeSpacing;
    }

private:
  
  real NodeSpacing;
};

int vtkvmtkSurfMeshWrapper::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  Remesh *nb = new Remesh(this->NodeSpacing);

  int numberOfInputPoints = input->GetNumberOfPoints();
  nb->SetNumberOfPoints(numberOfInputPoints);

  double point[3];
  real xpos[3];
  int i;
  for (i=0; i<numberOfInputPoints; i++)
    {
    input->GetPoint(i,point);
    xpos[0] = static_cast<real>(point[0]);
    xpos[1] = static_cast<real>(point[1]);
    xpos[2] = static_cast<real>(point[2]);
    nb->AddPoint(i,xpos[0],xpos[1],xpos[2]);
    }

  input->BuildCells();
  int numberOfInputCells = input->GetNumberOfCells();
  vtkIdType npts, *pts;
  for (i=0; i<numberOfInputCells; i++)
    {
    if (input->GetCellType(i) != VTK_TRIANGLE)
      {
      continue;
      }
    input->GetCellPoints(i,npts,pts);
    nb->AddTriangle(pts[0],pts[1],pts[2]);
    }

  try 
    {
    nb->Run2D();
    } 
  catch(Error *e) 
    {
    e->Print(std::cerr);
    }

  nb->IterateOnOutput(false);

  vtkPoints* outputPoints = vtkPoints::New();
  vtkCellArray* outputCellArray = vtkCellArray::New();

  while (nb->NextNode(xpos)!=-1) 
    {
    point[0] = static_cast<double>(xpos[0]);
    point[1] = static_cast<double>(xpos[1]);
    point[2] = static_cast<double>(xpos[2]);
    outputPoints->InsertNextPoint(point);
    }

  int icorn[3];
  while (nb->NextCell(icorn)) 
    {
    outputCellArray->InsertNextCell(3,icorn);
    }
  
  output->SetPoints(outputPoints);
  output->SetPolys(outputCellArray);

  outputPoints->Delete();
  outputCellArray->Delete();

  delete nb;
  
  return 1;
}

void vtkvmtkSurfMeshWrapper::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
