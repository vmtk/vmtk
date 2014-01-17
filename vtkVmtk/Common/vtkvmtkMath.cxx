/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkMath.cxx,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:47:30 $
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

#include "vtkvmtkMath.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkMath);

double vtkvmtkMath::Cotangent(double point0[3], double point1[3], double point2[3])
  {
  double norm0Squared, norm1Squared, dot, crossNorm, cotangent;

  norm0Squared = (point0[0]-point1[0])*(point0[0]-point1[0]) + (point0[1]-point1[1])*(point0[1]-point1[1]) + (point0[2]-point1[2])*(point0[2]-point1[2]);

  norm1Squared = (point2[0]-point1[0])*(point2[0]-point1[0]) + (point2[1]-point1[1])*(point2[1]-point1[1]) + (point2[2]-point1[2])*(point2[2]-point1[2]);

  dot = (point0[0]-point1[0])*(point2[0]-point1[0]) + (point0[1]-point1[1])*(point2[1]-point1[1]) + (point0[2]-point1[2])*(point2[2]-point1[2]);

  crossNorm = sqrt(norm0Squared * norm1Squared - dot*dot);

  cotangent = 0.0;
  
  if (crossNorm < GetTolerance(crossNorm))
    {  
    if (fabs(dot) < GetTolerance(dot))
      cotangent = 0.0;
    else
      cotangent = GetLarge(cotangent);
    }
  else if (fabs(dot) < GetTolerance(dot))
    {
    cotangent = 0.0;
    }
  else
    {
    cotangent = dot / crossNorm;
    }

  return cotangent;
  }

double vtkvmtkMath::TriangleArea(double point0[3], double point1[3], double point2[3])
  {
  double norm0Squared, norm1Squared, dot, crossNorm;

  norm0Squared = (point0[0]-point1[0])*(point0[0]-point1[0]) + (point0[1]-point1[1])*(point0[1]-point1[1]) + (point0[2]-point1[2])*(point0[2]-point1[2]);
  norm1Squared = (point2[0]-point1[0])*(point2[0]-point1[0]) + (point2[1]-point1[1])*(point2[1]-point1[1]) + (point2[2]-point1[2])*(point2[2]-point1[2]);
  dot = (point0[0]-point1[0])*(point2[0]-point1[0]) + (point0[1]-point1[1])*(point2[1]-point1[1]) + (point0[2]-point1[2])*(point2[2]-point1[2]);

  crossNorm = sqrt(norm0Squared * norm1Squared - dot*dot);

  if (crossNorm < GetTolerance(crossNorm))
    crossNorm = 0.0;

  return 0.5 * crossNorm;
  }

int vtkvmtkMath::IsAngleObtuse(double point0[3], double point1[3], double point2[3])
  {
  if ((point0[0]-point1[0])*(point2[0]-point1[0]) + (point0[1]-point1[1])*(point2[1]-point1[1]) + (point0[2]-point1[2])*(point2[2]-point1[2])<GetTolerance(point0[0]))
    return 1;

  return 0;
  }

int vtkvmtkMath::IsTriangleObtuse(double point0[3], double point1[3], double point2[3])
  {
  if (IsAngleObtuse(point0,point1,point2))
    return VTK_VMTK_OBTUSE_IN_POINT;
  else if (IsAngleObtuse(point2,point0,point1) || IsAngleObtuse(point1,point2,point0))
    return VTK_VMTK_OBTUSE_NOT_IN_POINT;
        
  return VTK_VMTK_NON_OBTUSE;
  }

double vtkvmtkMath::VoronoiSectorArea(double point0[3], double point1[3], double point2[3])
  {
  double cotAlpha, cotBeta, norm0Squared, norm1Squared;
  double sectorArea;

  cotAlpha = Cotangent(point2,point0,point1);
  cotBeta = Cotangent(point1,point2,point0);

  norm0Squared = (point0[0]-point1[0])*(point0[0]-point1[0]) + (point0[1]-point1[1])*(point0[1]-point1[1]) + (point0[2]-point1[2])*(point0[2]-point1[2]);
  norm1Squared = (point2[0]-point1[0])*(point2[0]-point1[0]) + (point2[1]-point1[1])*(point2[1]-point1[1]) + (point2[2]-point1[2])*(point2[2]-point1[2]);

  sectorArea = 1.0 / 8.0 * (norm0Squared * cotBeta + norm1Squared * cotAlpha);

  if (sectorArea < GetTolerance(sectorArea))
    sectorArea = 0.0;
        
  return sectorArea;
  }

double vtkvmtkMath::TriangleGradient(double point0[3], double point1[3], double point2[3], double scalar0, double scalar1, double scalar2, double gradient[3])
{
  double vector0[3], vector1[3], vector2[3];
  double gradientNormalVector[3], gradientDirection[3], gradientNorm;
  double triangleNormal[3];
  double area;

  area = TriangleArea(point0,point1,point2);

  if (area<GetTolerance(area))
    {
      gradientNormalVector[0] = 0.0;
      gradientNormalVector[1] = 0.0;
      gradientNormalVector[2] = 0.0;
      if ((fabs(scalar0-scalar1)>GetTolerance(scalar0))||(fabs(scalar1-scalar2)>GetTolerance(scalar1))||(fabs(scalar2-scalar0)>GetTolerance(scalar2)))
        return GetLarge(scalar0);
      else
        return 0.0;
    }

  vector0[0] = point2[0] - point1[0];
  vector0[1] = point2[1] - point1[1];
  vector0[2] = point2[2] - point1[2];
  vector1[0] = point0[0] - point2[0];
  vector1[1] = point0[1] - point2[1];
  vector1[2] = point0[2] - point2[2];
  vector2[0] = point1[0] - point0[0];
  vector2[1] = point1[1] - point0[1];
  vector2[2] = point1[2] - point0[2];

  vtkMath::Cross(vector0,vector1,triangleNormal);
  vtkMath::Normalize(triangleNormal);

  gradientNormalVector[0] = vector0[0]*scalar0 + vector1[0]*scalar1 + vector2[0]*scalar2;
  gradientNormalVector[1] = vector0[1]*scalar0 + vector1[1]*scalar1 + vector2[1]*scalar2;
  gradientNormalVector[2] = vector0[2]*scalar0 + vector1[2]*scalar1 + vector2[2]*scalar2;
  gradientNormalVector[0] /= 2.0*area;
  gradientNormalVector[1] /= 2.0*area;
  gradientNormalVector[2] /= 2.0*area;
  
  gradientNorm = vtkMath::Norm(gradientNormalVector);

  vtkMath::Cross(triangleNormal,gradientNormalVector,gradientDirection);
  vtkMath::Normalize(gradientDirection);
  
  gradient[0] = gradientNorm * gradientDirection[0];
  gradient[1] = gradientNorm * gradientDirection[1];
  gradient[2] = gradientNorm * gradientDirection[2];

  return gradientNorm;
}

void vtkvmtkMath::TwoSphereIntersection(double center0[3], double radius0, double center1[3], double radius1, double origin[3], double normal[3])
{
  double distance, displacement, squaredRadiusDifference, displacementRatio;

  normal[0] = center1[0] - center0[0];
  normal[1] = center1[1] - center0[1];
  normal[2] = center1[2] - center0[2];

  distance = vtkMath::Normalize(normal);

  if (distance<GetTolerance(distance))
    {
    origin[0] = center1[0];
    origin[1] = center1[1];
    origin[2] = center1[2];
    normal[0] = normal[1] = normal[2] = 0.0;
    return;
    }

  squaredRadiusDifference = radius0*radius0 - radius1*radius1;
  displacementRatio = squaredRadiusDifference/distance;
  
  if (distance*distance - fabs(squaredRadiusDifference) > GetTolerance(distance))
    {
    displacement = 0.5 * (displacementRatio + distance);
    }
  else 
    {
      if (squaredRadiusDifference > GetTolerance(distance))
        { 
        displacement = 0.5 * (displacementRatio - distance);
        }
      else
        {
        displacement = 0.5 * (displacementRatio + distance);
        }
    }
  
  origin[0] = center0[0] + displacement * normal[0];
  origin[1] = center0[1] + displacement * normal[1];
  origin[2] = center0[2] + displacement * normal[2];
}

double vtkvmtkMath::AngleBetweenNormals(double normal0[3], double normal1[3])
{
  double sum[3], difference[3];
  double sumNorm, differenceNorm;

  sum[0] = normal0[0] + normal1[0];
  sum[1] = normal0[1] + normal1[1];
  sum[2] = normal0[2] + normal1[2];
  sumNorm = vtkMath::Norm(sum);

  difference[0] = normal0[0] - normal1[0];
  difference[1] = normal0[1] - normal1[1];
  difference[2] = normal0[2] - normal1[2];
  differenceNorm = vtkMath::Norm(difference);

  return 2.0 * atan2(differenceNorm,sumNorm);
}

double vtkvmtkMath::EvaluateSphereFunction(double center[3], double radius, double point[3])
{
  return (center[0]-point[0]) * (center[0]-point[0]) + (center[1]-point[1]) * (center[1]-point[1]) + (center[2]-point[2]) * (center[2]-point[2]) - radius * radius;
}


