/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineGeometry.cxx,v $
Language:  C++
Date:      $Date: 2006/07/17 09:52:56 $
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

#include "vtkvmtkCenterlineGeometry.h"
#include "vtkvmtkCenterlineSmoothing.h"
#include "vtkvmtkConstants.h"
#include "vtkPolyData.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkPolyLine.h"
#include "vtkMath.h"
#include "vtkTransform.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkCenterlineGeometry);

vtkvmtkCenterlineGeometry::vtkvmtkCenterlineGeometry()
{
  this->LengthArrayName = NULL;
  this->CurvatureArrayName = NULL;
  this->TorsionArrayName = NULL;
  this->TortuosityArrayName = NULL;
  
  this->FrenetTangentArrayName = NULL;
  this->FrenetNormalArrayName = NULL;
  this->FrenetBinormalArrayName = NULL;
  
  this->LineSmoothing = 0;
  this->OutputSmoothedLines = 0;
  this->SmoothingFactor = 0.01;
  this->NumberOfSmoothingIterations = 100;
}

vtkvmtkCenterlineGeometry::~vtkvmtkCenterlineGeometry()
{
  if (this->LengthArrayName)
    {
    delete[] this->LengthArrayName;
    this->LengthArrayName = NULL;
    }

  if (this->CurvatureArrayName)
    {
    delete[] this->CurvatureArrayName;
    this->CurvatureArrayName = NULL;
    }

  if (this->TorsionArrayName)
    {
    delete[] this->TorsionArrayName;
    this->TorsionArrayName = NULL;
    }

  if (this->TortuosityArrayName)
    {
    delete[] this->TortuosityArrayName;
    this->TortuosityArrayName = NULL;
    }

  if (this->FrenetTangentArrayName)
    {
    delete[] this->FrenetTangentArrayName;
    this->FrenetTangentArrayName = NULL;
    }
  
  if (this->FrenetNormalArrayName)
    {
    delete[] this->FrenetNormalArrayName;
    this->FrenetNormalArrayName = NULL;
    }
  
  if (this->FrenetBinormalArrayName)
    {
    delete[] this->FrenetBinormalArrayName;
    this->FrenetBinormalArrayName = NULL;
    }
}

int vtkvmtkCenterlineGeometry::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (!this->LengthArrayName)
    {
    vtkErrorMacro(<<"LengthArrayName not specified");
    return 1;
    }

  if (!this->CurvatureArrayName)
    {
    vtkErrorMacro(<<"CurvatureArrayName not specified");
    return 1;
    }

  if (!this->TorsionArrayName)
    {
    vtkErrorMacro(<<"TorsionArrayName not specified");
    return 1;
    }

  if (!this->TortuosityArrayName)
    {
    vtkErrorMacro(<<"TorsionArrayName not specified");
    return 1;
    }

  if (!this->FrenetTangentArrayName)
    {
    vtkErrorMacro(<<"FrenetTangentArrayName not specified");
    return 1;
    }

  if (!this->FrenetNormalArrayName)
    {
    vtkErrorMacro(<<"FrenetNormalArrayName not specified");
    return 1;
    }
  
  if (!this->FrenetBinormalArrayName)
    {
    vtkErrorMacro(<<"FrenetBinormalArrayName not specified");
    return 1;
    }

  output->DeepCopy(input);

  int numberOfInputPoints = input->GetNumberOfPoints();
  int numberOfInputCells = input->GetNumberOfCells();

  vtkDoubleArray* lengthArray = vtkDoubleArray::New();
  lengthArray->SetName(this->LengthArrayName);
  lengthArray->SetNumberOfComponents(1);
  lengthArray->SetNumberOfTuples(numberOfInputCells);
  lengthArray->FillComponent(0,0.0);

  output->GetCellData()->AddArray(lengthArray);

  vtkDoubleArray* curvatureArray = vtkDoubleArray::New();
  curvatureArray->SetName(this->CurvatureArrayName);
  curvatureArray->SetNumberOfComponents(1);
  curvatureArray->SetNumberOfTuples(numberOfInputPoints);
  curvatureArray->FillComponent(0,0.0);

  output->GetPointData()->AddArray(curvatureArray);

  vtkDoubleArray* torsionArray = vtkDoubleArray::New();
  torsionArray->SetName(this->TorsionArrayName);
  torsionArray->SetNumberOfComponents(1);
  torsionArray->SetNumberOfTuples(numberOfInputPoints);
  torsionArray->FillComponent(0,0.0);

  output->GetPointData()->AddArray(torsionArray);

  vtkDoubleArray* tortuosityArray = vtkDoubleArray::New();
  tortuosityArray->SetName(this->TortuosityArrayName);
  tortuosityArray->SetNumberOfComponents(1);
  tortuosityArray->SetNumberOfTuples(numberOfInputCells);
  tortuosityArray->FillComponent(0,0.0);

  output->GetCellData()->AddArray(tortuosityArray);

  vtkDoubleArray* frenetTangentArray = vtkDoubleArray::New();
  frenetTangentArray->SetName(this->FrenetTangentArrayName);
  frenetTangentArray->SetNumberOfComponents(3);
  frenetTangentArray->SetNumberOfTuples(numberOfInputPoints);
  frenetTangentArray->FillComponent(0,0.0);
  frenetTangentArray->FillComponent(1,0.0);
  frenetTangentArray->FillComponent(2,0.0);

  output->GetPointData()->AddArray(frenetTangentArray);

  vtkDoubleArray* frenetNormalArray = vtkDoubleArray::New();
  frenetNormalArray->SetName(this->FrenetNormalArrayName);
  frenetNormalArray->SetNumberOfComponents(3);
  frenetNormalArray->SetNumberOfTuples(numberOfInputPoints);
  frenetNormalArray->FillComponent(0,0.0);
  frenetNormalArray->FillComponent(1,0.0);
  frenetNormalArray->FillComponent(2,0.0);

  output->GetPointData()->AddArray(frenetNormalArray);

  vtkDoubleArray* frenetBinormalArray = vtkDoubleArray::New();
  frenetBinormalArray->SetName(this->FrenetBinormalArrayName);
  frenetBinormalArray->SetNumberOfComponents(3);
  frenetBinormalArray->SetNumberOfTuples(numberOfInputPoints);
  frenetBinormalArray->FillComponent(0,0.0);
  frenetBinormalArray->FillComponent(1,0.0);
  frenetBinormalArray->FillComponent(2,0.0);

  output->GetPointData()->AddArray(frenetBinormalArray);
 
  for (int i=0; i<input->GetNumberOfCells(); i++)
    {
    vtkCell* line = input->GetCell(i);
    if (line->GetCellType() != VTK_LINE && line->GetCellType() != VTK_POLY_LINE)
      {
      continue;
      }

    vtkDoubleArray* lineCurvatureArray = vtkDoubleArray::New();
    vtkDoubleArray* lineTorsionArray = vtkDoubleArray::New();
    vtkDoubleArray* lineTangentArray = vtkDoubleArray::New();
    vtkDoubleArray* lineNormalArray = vtkDoubleArray::New();
    vtkDoubleArray* lineBinormalArray = vtkDoubleArray::New();
 
    vtkPoints* linePoints = vtkPoints::New();
    linePoints->DeepCopy(line->GetPoints());
    
    if (this->LineSmoothing)
      {
      vtkPoints* smoothLinePoints = vtkPoints::New();
      vtkvmtkCenterlineSmoothing::SmoothLine(linePoints,smoothLinePoints,this->NumberOfSmoothingIterations,this->SmoothingFactor);
      linePoints->DeepCopy(smoothLinePoints);
      smoothLinePoints->Delete();
      }
   
    this->ComputeLineCurvature(linePoints,lineCurvatureArray);
    this->ComputeLineTorsion(linePoints,lineTorsionArray);
    this->ComputeLineFrenetReferenceSystem(linePoints,lineTangentArray,lineNormalArray,lineBinormalArray);
    
    int numberOfLinePoints = linePoints->GetNumberOfPoints();

    double length = 0.0;
    double point0[3], point1[3];
    for (int j=0; j<numberOfLinePoints; j++)
      {
      vtkIdType pointId = line->GetPointId(j);
      if (this->OutputSmoothedLines)
        {
        output->GetPoints()->SetPoint(pointId,linePoints->GetPoint(j));
        }
      if (j>0)
        {
        linePoints->GetPoint(j-1,point0);
        linePoints->GetPoint(j,point1);
        length += sqrt(vtkMath::Distance2BetweenPoints(point0,point1));
        }
      curvatureArray->SetComponent(pointId,0,lineCurvatureArray->GetComponent(j,0));
      torsionArray->SetComponent(pointId,0,lineTorsionArray->GetComponent(j,0));
      double tuple[3];
      lineTangentArray->GetTuple(j,tuple);
      frenetTangentArray->SetTuple(pointId,tuple);
      lineNormalArray->GetTuple(j,tuple);
      frenetNormalArray->SetTuple(pointId,tuple);
      lineBinormalArray->GetTuple(j,tuple);
      frenetBinormalArray->SetTuple(pointId,tuple);
      }

    linePoints->GetPoint(0,point0);
    linePoints->GetPoint(numberOfLinePoints-1,point1);
    double tortuosity = length / sqrt(vtkMath::Distance2BetweenPoints(point0,point1)) - 1.0;

    lengthArray->SetComponent(i,0,length);
    tortuosityArray->SetComponent(i,0,tortuosity);

    lineCurvatureArray->Delete();
    lineTorsionArray->Delete();
    linePoints->Delete();
    lineTangentArray->Delete();
    lineNormalArray->Delete();
    lineBinormalArray->Delete();
    }

  lengthArray->Delete();
  curvatureArray->Delete();
  torsionArray->Delete();
  tortuosityArray->Delete();
  frenetTangentArray->Delete();
  frenetNormalArray->Delete();
  frenetBinormalArray->Delete();

  return 1;
}

void vtkvmtkCenterlineGeometry::ComputeLineFrenetReferenceSystem(vtkPoints* linePoints, vtkDoubleArray* lineTangentArray, vtkDoubleArray* lineNormalArray, vtkDoubleArray* lineBinormalArray)
{
  int numberOfPoints = linePoints->GetNumberOfPoints();

  lineTangentArray->SetNumberOfComponents(3);
  lineTangentArray->SetNumberOfTuples(numberOfPoints);
  lineTangentArray->FillComponent(0,0.0);
  lineTangentArray->FillComponent(1,0.0);
  lineTangentArray->FillComponent(2,0.0);

  lineNormalArray->SetNumberOfComponents(3);
  lineNormalArray->SetNumberOfTuples(numberOfPoints);
  lineNormalArray->FillComponent(0,0.0);
  lineNormalArray->FillComponent(1,0.0);
  lineNormalArray->FillComponent(2,0.0);

  lineBinormalArray->SetNumberOfComponents(3);
  lineBinormalArray->SetNumberOfTuples(numberOfPoints);
  lineBinormalArray->FillComponent(0,0.0);
  lineBinormalArray->FillComponent(1,0.0);
  lineBinormalArray->FillComponent(2,0.0);

  double point0[3], point1[3], point2[3];
  double vector0[3], vector1[3];

  for (int j=1; j<numberOfPoints-1; j++)
    {
    linePoints->GetPoint(j-1,point0);
    linePoints->GetPoint(j,point1);
    linePoints->GetPoint(j+1,point2);

    vector0[0] = point1[0] - point0[0];
    vector0[1] = point1[1] - point0[1];
    vector0[2] = point1[2] - point0[2];

    vector1[0] = point2[0] - point1[0];
    vector1[1] = point2[1] - point1[1];
    vector1[2] = point2[2] - point1[2];

    double norm0 = vtkMath::Norm(vector0);
    double norm1 = vtkMath::Norm(vector1);

    if (norm0 < VTK_VMTK_DOUBLE_TOL || norm1 < VTK_VMTK_DOUBLE_TOL)
      {
      continue;
      }

    double xp[3];
    xp[0] = point2[0] - point0[0];
    xp[1] = point2[1] - point0[1];
    xp[2] = point2[2] - point0[2];
    xp[0] /= norm0 + norm1;
    xp[1] /= norm0 + norm1;
    xp[2] /= norm0 + norm1;

    double xpp[3];
    xpp[0] = (point2[0] - point1[0]) / norm1 - (point1[0] - point0[0]) / norm0;
    xpp[1] = (point2[1] - point1[1]) / norm1 - (point1[1] - point0[1]) / norm0;
    xpp[2] = (point2[2] - point1[2]) / norm1 - (point1[2] - point0[2]) / norm0;
    xpp[0] /= (norm0 + norm1) / 2.0;
    xpp[1] /= (norm0 + norm1) / 2.0;
    xpp[2] /= (norm0 + norm1) / 2.0;
      
    double xpxppcross[3];
    vtkMath::Cross(xp,xpp,xpxppcross);

    double tangent[3], normal[3], binormal[3];
    
    tangent[0] = xp[0];
    tangent[1] = xp[1];
    tangent[2] = xp[2];
    vtkMath::Normalize(tangent);
    
    binormal[0] = xpxppcross[0];
    binormal[1] = xpxppcross[1];
    binormal[2] = xpxppcross[2];
    vtkMath::Normalize(binormal);

    vtkMath::Cross(binormal,tangent,normal);
    vtkMath::Normalize(normal);
    
    lineTangentArray->SetTuple(j,tangent); 
    lineNormalArray->SetTuple(j,normal); 
    lineBinormalArray->SetTuple(j,binormal); 
    }
  
  double tuple[3];
  lineTangentArray->GetTuple(1,tuple); 
  lineTangentArray->SetTuple(0,tuple); 
  lineNormalArray->GetTuple(1,tuple); 
  lineNormalArray->SetTuple(0,tuple); 
  lineBinormalArray->GetTuple(1,tuple); 
  lineBinormalArray->SetTuple(0,tuple); 
  lineTangentArray->GetTuple(numberOfPoints-2,tuple); 
  lineTangentArray->SetTuple(numberOfPoints-1,tuple); 
  lineNormalArray->GetTuple(numberOfPoints-2,tuple); 
  lineNormalArray->SetTuple(numberOfPoints-1,tuple); 
  lineBinormalArray->GetTuple(numberOfPoints-2,tuple); 
  lineBinormalArray->SetTuple(numberOfPoints-1,tuple); 
}

double vtkvmtkCenterlineGeometry::ComputeLineCurvature(vtkPoints* linePoints, vtkDoubleArray* curvatureArray)
{
  int numberOfPoints = linePoints->GetNumberOfPoints();

  curvatureArray->SetNumberOfComponents(1);
  curvatureArray->SetNumberOfTuples(numberOfPoints);
  curvatureArray->FillComponent(0,0.0);

  double point0[3], point1[3], point2[3];
  double vector0[3], vector1[3];

  double averageCurvature = 0.0;
  double weightSum = 0.0;

  for (int j=1; j<numberOfPoints-1; j++)
    {
    linePoints->GetPoint(j-1,point0);
    linePoints->GetPoint(j,point1);
    linePoints->GetPoint(j+1,point2);

    vector0[0] = point1[0] - point0[0];
    vector0[1] = point1[1] - point0[1];
    vector0[2] = point1[2] - point0[2];

    vector1[0] = point2[0] - point1[0];
    vector1[1] = point2[1] - point1[1];
    vector1[2] = point2[2] - point1[2];

    double norm0 = vtkMath::Norm(vector0);
    double norm1 = vtkMath::Norm(vector1);

    if (norm0 < VTK_VMTK_DOUBLE_TOL || norm1 < VTK_VMTK_DOUBLE_TOL)
      {
      continue;
      }

    double xp[3];
    xp[0] = point2[0] - point0[0];
    xp[1] = point2[1] - point0[1];
    xp[2] = point2[2] - point0[2];
    xp[0] /= norm0 + norm1;
    xp[1] /= norm0 + norm1;
    xp[2] /= norm0 + norm1;

    double xpp[3];
    xpp[0] = (point2[0] - point1[0]) / norm1 - (point1[0] - point0[0]) / norm0;
    xpp[1] = (point2[1] - point1[1]) / norm1 - (point1[1] - point0[1]) / norm0;
    xpp[2] = (point2[2] - point1[2]) / norm1 - (point1[2] - point0[2]) / norm0;
    xpp[0] /= (norm0 + norm1) / 2.0;
    xpp[1] /= (norm0 + norm1) / 2.0;
    xpp[2] /= (norm0 + norm1) / 2.0;
      
    double xpxppcross[3];
    vtkMath::Cross(xp,xpp,xpxppcross);

    double curvature = vtkMath::Norm(xpxppcross) / pow(vtkMath::Norm(xp),3.0);
    curvatureArray->SetComponent(j,0,curvature);
    double weight = (norm0 + norm1) / 2.0;
    averageCurvature += curvature * weight;

    weightSum += weight;
    }

  if (weightSum>0.0)
    {
    averageCurvature /= weightSum;
    }

  return averageCurvature;
}

double vtkvmtkCenterlineGeometry::ComputeLineTorsion(vtkPoints* linePoints, vtkDoubleArray* torsionArray)
{
  int numberOfPoints = linePoints->GetNumberOfPoints();

  torsionArray->SetNumberOfComponents(1);
  torsionArray->SetNumberOfTuples(numberOfPoints);
  torsionArray->FillComponent(0,0.0);

  double point0[3], point1[3], point2[3];
  double vector0[3], vector1[3];

  double averageTorsion = 0.0;
  double weightSum = 0.0;
    
  double* xps = new double[3*numberOfPoints];
  double* xpps = new double[3*numberOfPoints];

  int j;
  for (j=0; j<numberOfPoints; j++)
    {
    xps[3*j+0] = xps[3*j+1] = xps[3*j+2] = 0.0;
    xpps[3*j+0] = xpps[3*j+1] = xpps[3*j+2] = 0.0;
    }

  for (j=1; j<numberOfPoints-1; j++)
    {
    linePoints->GetPoint(j-1,point0);
    linePoints->GetPoint(j,point1);
    linePoints->GetPoint(j+1,point2);

    vector0[0] = point1[0] - point0[0];
    vector0[1] = point1[1] - point0[1];
    vector0[2] = point1[2] - point0[2];

    vector1[0] = point2[0] - point1[0];
    vector1[1] = point2[1] - point1[1];
    vector1[2] = point2[2] - point1[2];

    double norm0 = vtkMath::Norm(vector0);
    double norm1 = vtkMath::Norm(vector1);

    if (norm0 < VTK_VMTK_DOUBLE_TOL || norm1 < VTK_VMTK_DOUBLE_TOL)
      {
      continue;
      }

    xps[3*j+0] = point2[0] - point0[0];
    xps[3*j+1] = point2[1] - point0[1];
    xps[3*j+2] = point2[2] - point0[2];
    xps[3*j+0] /= norm0 + norm1;
    xps[3*j+1] /= norm0 + norm1;
    xps[3*j+2] /= norm0 + norm1;

    xpps[3*j+0] = (point2[0] - point1[0]) / norm1 - (point1[0] - point0[0]) / norm0;
    xpps[3*j+1] = (point2[1] - point1[1]) / norm1 - (point1[1] - point0[1]) / norm0;
    xpps[3*j+2] = (point2[2] - point1[2]) / norm1 - (point1[2] - point0[2]) / norm0;
    xpps[3*j+0] /= (norm0 + norm1) / 2.0;
    xpps[3*j+1] /= (norm0 + norm1) / 2.0;
    xpps[3*j+2] /= (norm0 + norm1) / 2.0;
    }

  for (j=2; j<numberOfPoints-2; j++)
    {
    linePoints->GetPoint(j-1,point0);
    linePoints->GetPoint(j,point1);
    linePoints->GetPoint(j+1,point2);

    vector0[0] = point1[0] - point0[0];
    vector0[1] = point1[1] - point0[1];
    vector0[2] = point1[2] - point0[2];

    vector1[0] = point2[0] - point1[0];
    vector1[1] = point2[1] - point1[1];
    vector1[2] = point2[2] - point1[2];

    double norm0 = vtkMath::Norm(vector0);
    double norm1 = vtkMath::Norm(vector1);

    if (norm0 < VTK_VMTK_DOUBLE_TOL || norm1 < VTK_VMTK_DOUBLE_TOL)
      {
      continue;
      }

    double xp[3];
    xp[0] = xps[3*j+0];
    xp[1] = xps[3*j+1];
    xp[2] = xps[3*j+2];

    double xpp[3];
    xpp[0] = xpps[3*j+0];
    xpp[1] = xpps[3*j+1];
    xpp[2] = xpps[3*j+2];

    double xpxppcross[3];
    vtkMath::Cross(xp,xpp,xpxppcross);

    double xppp[3];
    xppp[0] = xpps[3*(j+1)+0] - xpps[3*(j-1)+0];
    xppp[1] = xpps[3*(j+1)+1] - xpps[3*(j-1)+1];
    xppp[2] = xpps[3*(j+1)+2] - xpps[3*(j-1)+2];

    xppp[0] /= norm0 + norm1;
    xppp[1] /= norm0 + norm1;
    xppp[2] /= norm0 + norm1;

    double torsion = vtkMath::Dot(xpxppcross,xppp) / pow(vtkMath::Norm(xpxppcross),2.0);
    torsionArray->SetComponent(j,0,torsion);
    double weight = (norm0 + norm1) / 2.0;
    averageTorsion += torsion * weight;

    weightSum += weight;
    }

  delete[] xps;
  delete[] xpps;

  if (weightSum>0.0)
    {
    averageTorsion /= weightSum;
    }

  return averageTorsion;
}

void vtkvmtkCenterlineGeometry::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
