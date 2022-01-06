/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkActiveTubeFilter.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

  Portions of this code are covered under the ITK copyright.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "vtkvmtkActiveTubeFilter.h"

#include "vtkvmtkCardinalSpline.h"

#include "vtkvmtkConstants.h"
#include "vtkMath.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkImageGradient.h"
#include "vtkDoubleArray.h"

#include "vtkPolyLine.h"
#include "vtkVoxel.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkActiveTubeFilter);

vtkvmtkActiveTubeFilter::vtkvmtkActiveTubeFilter()
{
  this->RadiusArrayName = NULL;

  this->PotentialImage = NULL;
  this->PotentialGradientImage = NULL;
  this->NumberOfIterations = VTK_VMTK_LARGE_INTEGER;
  this->NumberOfAngularEvaluations = 16;
  this->Convergence = 1E-1;
  this->PotentialWeight = 1.0;
  this->StiffnessWeight = 1.0;
  this->CFLCoefficient = 0.1;

  this->PotentialMaxNorm = 0.0;
  this->MinimumRadius = 0.0;
  this->FixedEndpointCoordinates = 0;
  this->FixedEndpointRadius = 0;

  this->SplineResamplingWhileIterating = 1;

  this->NegativeNormWarnings = 0;
}

vtkvmtkActiveTubeFilter::~vtkvmtkActiveTubeFilter()
{
  if (this->RadiusArrayName)
    {
    delete[] this->RadiusArrayName;
    this->RadiusArrayName = NULL;
    }

  if (this->PotentialImage)
    {
    this->PotentialImage->Delete();
    this->PotentialImage = NULL;
    }

  if (this->PotentialGradientImage)
    {
    this->PotentialGradientImage->Delete();
    this->PotentialGradientImage = NULL;
    }
}

vtkCxxSetObjectMacro(vtkvmtkActiveTubeFilter,PotentialImage,vtkImageData);

void vtkvmtkActiveTubeFilter::EvaluateForce(double point[3], double force[3], bool normalize)
{
  int ijk[3];
  double pcoords[3];
  double weights[8];
  int inBounds;
  int i;

  force[0] = force[1] = force[2] = 0.0;

  inBounds = this->PotentialGradientImage->ComputeStructuredCoordinates(point,ijk,pcoords);

  if (!inBounds)
    {
    //vtkWarningMacro("Point out of extent");
    return;
    }

  vtkCell* cell = this->PotentialGradientImage->GetCell(this->PotentialGradientImage->ComputeCellId(ijk));

  if (cell->GetCellType() == VTK_VOXEL)
  {
    vtkVoxel::InterpolationFunctions(pcoords,weights);
  }
  else
  {
    vtkErrorMacro("Non voxel cell found in PotentialImage");
  }

  int numberOfCellPoints = cell->GetNumberOfPoints();

  vtkDataArray* gradientVectorsArray = this->PotentialGradientImage->GetPointData()->GetScalars();

  for (i=0; i<numberOfCellPoints; i++)
  {
    double vectorValue[3];
    gradientVectorsArray->GetTuple(cell->GetPointId(i),vectorValue);
    force[0] += weights[i] * vectorValue[0];
    force[1] += weights[i] * vectorValue[1];
    force[2] += weights[i] * vectorValue[2];
  }

  if (normalize && this->PotentialMaxNorm > VTK_VMTK_DOUBLE_TOL)
  {
    force[0] /= this->PotentialMaxNorm;
    force[1] /= this->PotentialMaxNorm;
    force[2] /= this->PotentialMaxNorm;
  }

  force[0] *= -1.0;
  force[1] *= -1.0;
  force[2] *= -1.0;
}

double vtkvmtkActiveTubeFilter::EvaluatePotential(double point[3])
{
  int ijk[3];
  double pcoords[3];
  double weights[8];
  int inBounds;
  int i;

  double potential = 0.0;

  inBounds = this->PotentialImage->ComputeStructuredCoordinates(point,ijk,pcoords);

  if (!inBounds)
    {
    //vtkWarningMacro("Point out of extent");
    return 0.0;
    }

  vtkCell* cell = this->PotentialImage->GetCell(this->PotentialImage->ComputeCellId(ijk));

  if (cell->GetCellType() == VTK_VOXEL)
  {
    vtkVoxel::InterpolationFunctions(pcoords,weights);
  }
  else
  {
    vtkErrorMacro("Non voxel cell found in PotentialImage");
    return 0.0;
  }

  int numberOfCellPoints = cell->GetNumberOfPoints();

  vtkDataArray* potentialArray = this->PotentialImage->GetPointData()->GetScalars();

  for (i=0; i<numberOfCellPoints; i++)
  {
    double value = potentialArray->GetTuple1(cell->GetPointId(i));
    potential += weights[i] * value;
  }

  return potential;
}

void vtkvmtkActiveTubeFilter::EvolveCellSpline(vtkPolyData* lines, vtkIdType cellId)
{
  //TODO: whip strategy - from start to end of spline, create displacement field simulating
  //inertia (i.e. force on a point dependent on displacement on previous point)

  vtkDataArray* radiusArray = lines->GetPointData()->GetArray(this->RadiusArrayName);

  vtkPolyLine* polyLine = vtkPolyLine::SafeDownCast(lines->GetCell(cellId));

  if (!polyLine)
    {
    return;
    }

  vtkPoints* polyLinePoints = polyLine->GetPoints();

  vtkvmtkCardinalSpline* xSpline = vtkvmtkCardinalSpline::New();
  vtkvmtkCardinalSpline* ySpline = vtkvmtkCardinalSpline::New();
  vtkvmtkCardinalSpline* zSpline = vtkvmtkCardinalSpline::New();
  vtkvmtkCardinalSpline* rSpline = vtkvmtkCardinalSpline::New();

  int numberOfPoints = polyLine->GetNumberOfPoints();
  int numberOfSubIds = numberOfPoints - 1;

  double cellLength = 0.0;
  double point0[3], point1[3];
  int i;
  for (i=0; i<numberOfSubIds; i++)
    {
    polyLinePoints->GetPoint(i,point0);
    polyLinePoints->GetPoint(i+1,point1);
    cellLength += sqrt(vtkMath::Distance2BetweenPoints(point0,point1));
    }

  xSpline->SetParametricRange(0.0,cellLength);
  ySpline->SetParametricRange(0.0,cellLength);
  zSpline->SetParametricRange(0.0,cellLength);
  rSpline->SetParametricRange(0.0,cellLength);

  vtkDoubleArray* parametricCoordinates = vtkDoubleArray::New();
  parametricCoordinates->SetNumberOfTuples(numberOfPoints);

  double currentLength = 0.0; 
  polyLinePoints->GetPoint(0,point0);
  double radius = radiusArray->GetTuple1(polyLine->GetPointId(0));
  double t = 0.0;
  xSpline->AddPoint(0.0,point0[0]);
  ySpline->AddPoint(0.0,point0[1]);
  zSpline->AddPoint(0.0,point0[2]);
  rSpline->AddPoint(0.0,radius);
  parametricCoordinates->SetValue(0,0.0);
  for (i=1; i<numberOfPoints; i++)
    {
    polyLinePoints->GetPoint(i-1,point0);
    polyLinePoints->GetPoint(i,point1);
    radius = radiusArray->GetTuple1(polyLine->GetPointId(i));
    currentLength += sqrt(vtkMath::Distance2BetweenPoints(point0,point1));
    t = currentLength;
    xSpline->AddPoint(t,point1[0]);
    ySpline->AddPoint(t,point1[1]);
    zSpline->AddPoint(t,point1[2]);
    rSpline->AddPoint(t,radius);
    parametricCoordinates->SetValue(i,t);
    }

  if (this->SplineResamplingWhileIterating)
    {
    double point[3];
    for (i=0; i<numberOfPoints; i++)
      {
      t = (double)i / (numberOfPoints-1) * cellLength;
      point[0] = xSpline->Evaluate(t);
      point[1] = ySpline->Evaluate(t);
      point[2] = zSpline->Evaluate(t);
      radius = rSpline->Evaluate(t);
      polyLinePoints->SetPoint(i,point);
      radiusArray->SetTuple1(i,radius);
      parametricCoordinates->SetValue(i,t);
      }
    }
 
  vtkDoubleArray* probedForces = vtkDoubleArray::New();
  probedForces->SetNumberOfComponents(3);
  probedForces->SetNumberOfTuples(this->NumberOfAngularEvaluations);

  vtkDoubleArray* tubeNormals = vtkDoubleArray::New();
  tubeNormals->SetNumberOfComponents(3);
  tubeNormals->SetNumberOfTuples(this->NumberOfAngularEvaluations);
  
  vtkDoubleArray* dxArray = vtkDoubleArray::New();
  dxArray->SetNumberOfTuples(numberOfPoints);
  dxArray->FillComponent(0,0.0);
  vtkDoubleArray* dyArray = vtkDoubleArray::New();
  dyArray->SetNumberOfTuples(numberOfPoints);
  dyArray->FillComponent(0,0.0);
  vtkDoubleArray* dzArray = vtkDoubleArray::New();
  dzArray->SetNumberOfTuples(numberOfPoints);
  dzArray->FillComponent(0,0.0);
  vtkDoubleArray* drArray = vtkDoubleArray::New();
  drArray->SetNumberOfTuples(numberOfPoints);
  drArray->FillComponent(0,0.0);

  //TODO: choose numberOfLongitudinalEvaluations with a strategy 
  //      (fixed number, based on length, adaptive - higher curve or radius derivatives, more points)
  int numberOfLongitudinalEvaluations = numberOfPoints * 3 / 2;
 
  for (i=0; i<numberOfLongitudinalEvaluations; i++)
    {
    t = (double)i / (numberOfLongitudinalEvaluations-1) * cellLength;
    double x = xSpline->Evaluate(t);
    double y = ySpline->Evaluate(t);
    double z = zSpline->Evaluate(t);
    double r = rSpline->Evaluate(t);
    double xp = xSpline->EvaluateDerivative(t);
    double yp = ySpline->EvaluateDerivative(t);
    double zp = zSpline->EvaluateDerivative(t);
    double rp = rSpline->EvaluateDerivative(t);
    double xpp = xSpline->EvaluateSecondDerivative(t);
    double ypp = ySpline->EvaluateSecondDerivative(t);
    double zpp = zSpline->EvaluateSecondDerivative(t);
    double rpp = rSpline->EvaluateSecondDerivative(t);
  
    double tangent[3], normal[3];
 
    tangent[0] = xp; 
    tangent[1] = yp; 
    tangent[2] = zp; 

    double tubeNormSquared = xp * xp + yp * yp + zp * zp - rp * rp;

    //if (tubeNormSquared < 0.0)
    if (tubeNormSquared <= 0.0)
      {
      if (this->NegativeNormWarnings)
        {
        vtkWarningMacro("Negative tubeNormSquared. Skipping.");
        }
      continue;
      }
    
    double tubeNorm = sqrt(tubeNormSquared);

    double isotropicForce = 0.0;
    double anisotropicForce[3];
    anisotropicForce[0] = anisotropicForce[1] = anisotropicForce[2] = 0.0;

    double tubeNormal[3], probePoint[3], probedForce[3];
    double theta;
    int j;
    for (j=0; j<this->NumberOfAngularEvaluations; j++)
      {
      theta = j * 2.0 * vtkMath::Pi() / this->NumberOfAngularEvaluations;

      vtkMath::Perpendiculars(tangent,normal,NULL,theta);

      tubeNormal[0] = - tangent[0] * rp + normal[0] * tubeNorm;
      tubeNormal[1] = - tangent[1] * rp + normal[1] * tubeNorm;
      tubeNormal[2] = - tangent[2] * rp + normal[2] * tubeNorm;
      //optional, but better be on the safe side
      vtkMath::Normalize(tubeNormal);

      probePoint[0] = x + tubeNormal[0] * r;
      probePoint[1] = y + tubeNormal[1] * r;
      probePoint[2] = z + tubeNormal[2] * r;

      //double probedPotential = this->EvaluatePotential(probePoint);

      this->EvaluateForce(probePoint,probedForce,false);

      probedForces->SetTuple(j,probedForce);
      tubeNormals->SetTuple(j,tubeNormal);

      isotropicForce += vtkMath::Dot(probedForce,tubeNormal);
      }

    isotropicForce /= this->NumberOfAngularEvaluations;

    for (j=0; j<this->NumberOfAngularEvaluations; j++)
      {
      probedForces->GetTuple(j,probedForce);
      tubeNormals->GetTuple(j,tubeNormal);
      anisotropicForce[0] += probedForce[0] - tubeNormal[0] * isotropicForce;
      anisotropicForce[1] += probedForce[1] - tubeNormal[1] * isotropicForce;
      anisotropicForce[2] += probedForce[2] - tubeNormal[2] * isotropicForce;
      }

    anisotropicForce[0] /= this->NumberOfAngularEvaluations;
    anisotropicForce[1] /= this->NumberOfAngularEvaluations;
    anisotropicForce[2] /= this->NumberOfAngularEvaluations;

    //TODO: define influence (based on parametric distance? Or also consider derivatives?)
    //      implement Gaussian RBF?
    for (j=0; j<numberOfPoints; j++)
      {
      double parametricCoordinate = parametricCoordinates->GetValue(j);
      double influence = 0.1 * cellLength;
      if (fabs(parametricCoordinate-t) > influence)
        {
        continue;
        }
      double weight = (influence - fabs(parametricCoordinate-t)) / influence; 
      double dx, dy, dz, dr;
      dx = dxArray->GetValue(j);
      dy = dyArray->GetValue(j);
      dz = dzArray->GetValue(j);
      dr = drArray->GetValue(j);
      dx += anisotropicForce[0] * weight * this->PotentialWeight;
      dy += anisotropicForce[1] * weight * this->PotentialWeight;
      dz += anisotropicForce[2] * weight * this->PotentialWeight;
      dr += isotropicForce * weight * this->PotentialWeight;
      dx += xpp * weight * this->StiffnessWeight;
      dy += ypp * weight * this->StiffnessWeight;
      dz += zpp * weight * this->StiffnessWeight;
      dr += rpp * weight * this->StiffnessWeight;
      dxArray->SetValue(j,dx); 
      dyArray->SetValue(j,dy); 
      dzArray->SetValue(j,dz); 
      drArray->SetValue(j,dr); 
      }
    }

  double spacing[3];
#if 0
  if (this->PotientialImage)
    {
    this->PotentialImage->GetSpacing(spacing);
    }
  else
#endif
    {
    this->PotentialGradientImage->GetSpacing(spacing);
    }

  double minSpacing = VTK_VMTK_LARGE_DOUBLE;
  for (i=0; i<3; i++)
    {
    minSpacing = spacing[i] < minSpacing ? spacing[i] : minSpacing;
    }

  double maxChange = 0.0;
  for (i=0; i<numberOfPoints; i++)
    {
    double dx = dxArray->GetValue(i);
    double dy = dyArray->GetValue(i);
    double dz = dzArray->GetValue(i);
    double dr = drArray->GetValue(i);
    double change = sqrt(dx*dx + dy*dy + dz*dz) + dr;
    maxChange = change > maxChange ? change : maxChange;
    }
 
  double timeStep;
  if (maxChange > 0.0)
    {
    timeStep = minSpacing / maxChange * this->CFLCoefficient;
    }
  else
    {
    timeStep = 0.0;
    }

  vtkIdType pointId;
  double point[3];
  for (i=0; i<numberOfPoints; i++)
    {
    pointId = polyLine->GetPointId(i);
    polyLinePoints->GetPoint(i,point);
    radius = radiusArray->GetTuple1(pointId);
    point[0] += dxArray->GetValue(i) * timeStep;
    point[1] += dyArray->GetValue(i) * timeStep;
    point[2] += dzArray->GetValue(i) * timeStep;
    radius += drArray->GetValue(i) * timeStep;
    if (radius < this->MinimumRadius)
      {
      radius = this->MinimumRadius;
      }
    if (!(this->FixedEndpointCoordinates && (i==0 || i==numberOfPoints-1)))
      {
      lines->GetPoints()->SetPoint(pointId,point);
      }
    if (!(this->FixedEndpointRadius && (i==0 || i==numberOfPoints-1)))
      {
      radiusArray->SetTuple1(pointId,radius);
      }
    }

  //TODO: longitudinal evolution

  xSpline->Delete();
  ySpline->Delete();
  zSpline->Delete();
  rSpline->Delete();
  parametricCoordinates->Delete();
  probedForces->Delete();
  tubeNormals->Delete();
  dxArray->Delete();
  dyArray->Delete();
  dzArray->Delete();
  drArray->Delete();
}

int vtkvmtkActiveTubeFilter::RequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  output->DeepCopy(input);

  if (!this->RadiusArrayName)
    {
    vtkErrorMacro("Error: RadiusArrayName not specified.");
    return 0;
    }

  vtkDataArray* radiusArray = output->GetPointData()->GetArray(this->RadiusArrayName);

  if (!radiusArray)
    {
    vtkErrorMacro("Error: RadiusArray with name specified does not exist.");
    return 0;
    }

  vtkImageGradient* gradientFilter = vtkImageGradient::New();
  gradientFilter->SetInputData(this->PotentialImage);
  gradientFilter->SetDimensionality(3);
  gradientFilter->Update();

  if (this->PotentialGradientImage)
    {
    this->PotentialGradientImage->Delete();
    this->PotentialGradientImage = NULL;
    }

  this->PotentialGradientImage = vtkImageData::New();
  this->PotentialGradientImage->DeepCopy(gradientFilter->GetOutput());

  this->PotentialMaxNorm = this->PotentialGradientImage->GetPointData()->GetScalars()->GetMaxNorm();

  int numberOfCells = output->GetNumberOfCells();  

  int i;
  for (i=0; i<this->NumberOfIterations; i++)
    {
    int c;
    for (c=0; c<numberOfCells; c++)
      {
      this->EvolveCellSpline(output,c);
      }
    }

  gradientFilter->Delete();

  return 1;
}

void vtkvmtkActiveTubeFilter::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

