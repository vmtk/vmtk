#!/usr/bin/env python


from __future__ import print_function, absolute_import # NEED TO STAY AS TOP IMPORT
import vtk
import sys
import math
from vmtk import vtkvmtk

def ReadPolyData(filename):
   reader = vtk.vtkXMLPolyDataReader()
   reader.SetFileName(filename)
   reader.Update()
   return reader.GetOutput()

def WritePolyData(input,filename):
   writer = vtk.vtkXMLPolyDataWriter()
   writer.SetFileName(filename)
   writer.SetInputData(input)
   writer.Write()
  
def ExtractCylindricInterpolationVoronoiDiagram(cellId,pointId,cylinderRadius,voronoi,centerlines):
   isInside = 0

   if (cellId == 0):
      cylinderTop = centerlines.GetPoint(pointId) 
      cylinderCenter = centerlines.GetPoint(pointId-interpolationHalfSize)
      cylinderBottom = centerlines.GetPoint(pointId-interpolationHalfSize*2) 
   else:
      cylinderTop = centerlines.GetPoint(pointId) 
      cylinderCenter = centerlines.GetPoint(pointId+interpolationHalfSize)
      cylinderBottom = centerlines.GetPoint(pointId+interpolationHalfSize*2) 

   interpolationDataset = vtk.vtkPolyData()
   interpolationDatasetPoints = vtk.vtkPoints()
   interpolationDatasetCellArray = vtk.vtkCellArray()
          
   maskArray = vtk.vtkIntArray()
   maskArray.SetNumberOfComponents(1)
   maskArray.SetNumberOfTuples(voronoi.GetNumberOfPoints())
   maskArray.FillComponent(0,0)

   for i in range(voronoi.GetNumberOfPoints()):
      point = voronoi.GetPoint(i)
      isInside = IsPointInsideInterpolationCylinder(point,cylinderTop,cylinderCenter,cylinderBottom,cylinderRadius)

      if (isInside == 1):
         maskArray.SetTuple1(i,1)

   numberOfInterpolationPoints = ComputeNumberOfMaskedPoints(maskArray)

   radiusArray = vtk.vtkDoubleArray()
   radiusArray.SetNumberOfComponents(1)
   radiusArray.SetNumberOfTuples(numberOfInterpolationPoints)
   radiusArray.SetName(radiusArrayName)
   radiusArray.FillComponent(0,0.0)

   count = 0
   for i in range(voronoi.GetNumberOfPoints()):
      value = maskArray.GetTuple1(i)
      if (value == 1):
         interpolationDatasetPoints.InsertNextPoint(voronoi.GetPoint(i))
         interpolationDatasetCellArray.InsertNextCell(1)
         interpolationDatasetCellArray.InsertCellPoint(count)
         radius = voronoi.GetPointData().GetArray(radiusArrayName).GetTuple1(i)
         radiusArray.SetTuple1(count,radius)
         count +=1

   interpolationDataset.SetPoints(interpolationDatasetPoints)
   interpolationDataset.SetVerts(interpolationDatasetCellArray)
   interpolationDataset.GetPointData().AddArray(radiusArray)

   return interpolationDataset

def IsPointInsideInterpolationCylinder(x,t,c,b,r):
  halfheigth = math.sqrt(vtk.vtkMath.Distance2BetweenPoints(b,t))/2.0

  xc = [0.0,0.0,0.0]
  tb = [0.0,0.0,0.0]

  xc[0] = x[0]-c[0]
  xc[1] = x[1]-c[1]
  xc[2] = x[2]-c[2]

  tb[0] = t[0]-b[0]
  tb[1] = t[1]-b[1]
  tb[2] = t[2]-b[2]

  xcnorm = vtk.vtkMath.Norm(xc)
  vtk.vtkMath.Normalize(xc)  
  vtk.vtkMath.Normalize(tb)  

  alpha = math.acos(vtk.vtkMath.Dot(xc,tb))

  parallelxc = xcnorm*math.cos(alpha) 
  perpendicularxc =  xcnorm*math.sin(alpha)

  thetamin = math.atan(r/halfheigth)
  thetamax = thetamin + (math.pi - 2*thetamin)

  inside = 0
  if (thetamin<=alpha<=thetamax):
    if (abs(perpendicularxc)<=r): inside = 1
  else:
    if (abs(parallelxc)<=halfheigth): inside = 1

  return inside   


def ComputeNumberOfMaskedPoints(dataArray):
   numberOfPoints = 0
   for i  in range(dataArray.GetNumberOfTuples()):
      value = dataArray.GetTuple1(i)
      if (value ==1): numberOfPoints +=1
   return numberOfPoints

def VoronoiDiagramInterpolation(interpolationcellid,id0,id1,voronoiDataset0,voronoiDataset1,centerlines,direction):
    print('Interpolating cells ',id0,id1)
    cellLine = ExtractLine(interpolationcellid,centerlines)

    startPoint = clippingPoints.GetPoint(id0)
    endPoint = clippingPoints.GetPoint(id1)

    startId = cellLine.FindPoint(startPoint)
    endId = cellLine.FindPoint(endPoint)

    if (direction == 1):
       gapStartId = startId + 1
       gapEndId = endId - 1
       arrivalId = gapEndId + 1
       endSavingInterval = gapEndId + 1
       step = 1
    else:
       gapStartId = startId - 1
       gapEndId = endId + 1
       arrivalId = gapEndId - 1
       endSavingInterval = gapEndId - 1
       step = -1

    numberOfGapPoints = int(math.fabs(gapEndId - gapStartId)) + 1
    numberOfInterpolationPoints = voronoiDataset0.GetNumberOfPoints()
    numberOfCenterlinesPoints = cellLine.GetNumberOfPoints()
    numberOfAddedPoints = numberOfGapPoints*numberOfInterpolationPoints
    print('from id ',gapStartId,'to id ',gapEndId,'; number of points added ',numberOfAddedPoints)

    finalNewVoronoiPoints = vtk.vtkPoints()
    cellArray = vtk.vtkCellArray()
    finalRadiusArray = vtk.vtkDoubleArray()
    finalRadiusArray.SetNumberOfComponents(1)
    finalRadiusArray.SetNumberOfTuples(numberOfAddedPoints)
    finalRadiusArray.SetName(radiusArrayName)
    finalRadiusArray.FillComponent(0,0.0)
    count = 0

    for i in range(numberOfInterpolationPoints):
       voronoiPoint = voronoiDataset0.GetPoint(i)
       voronoiPointRadius = voronoiDataset0.GetPointData().GetArray(radiusArrayName).GetTuple1(i)

       centerlinePointLocator = vtk.vtkPointLocator()
       centerlinePointLocator.SetDataSet(cellLine)
       centerlinePointLocator.BuildLocator()

       closestPointId = centerlinePointLocator.FindClosestPoint(voronoiPoint)
       closestPoint = cellLine.GetPoint(closestPointId)
    
       voronoiVector = [0.0,0.0,0.0]
       voronoiVector[0] = voronoiPoint[0] - closestPoint[0]
       voronoiVector[1] = voronoiPoint[1] - closestPoint[1]
       voronoiVector[2] = voronoiPoint[2] - closestPoint[2]
       voronoiVectorNorm = vtk.vtkMath.Norm(voronoiVector)
    
       rotationAngle = ComputeVoronoiVectorToCenterlineAngle(closestPointId,voronoiVector,cellLine)

       PTPoints = vtk.vtkPoints()
       for j in range(closestPointId,arrivalId,step):
          localtangent = [0.0,0.0,0.0]
          localnormal = [0.0,0.0,0.0]
          newVoronoiVector = [0.0,0.0,0.0]
          newVoronoiPoint = [0.0,0.0,0.0]
           
          transform = vtk.vtkTransform()

          point0 = [0.0,0.0,0.0]
          point0 = cellLine.GetPoint(j)

          if (j<numberOfCenterlinesPoints-1):
             point1 = [0.0,0.0,0.0]
             cellLine.GetPoint(j+1,point1)   
             localtangent[0] += point1[0] - point0[0]  
             localtangent[1] += point1[1] - point0[1]  
             localtangent[2] += point1[2] - point0[2]  
          if (j>0):
             point2 = [0.0,0.0,0.0]
             cellLine.GetPoint(j-1,point2) 
             localtangent[0] += point0[0] - point2[0]
             localtangent[1] += point0[1] - point2[1]
             localtangent[2] += point0[2] - point2[2]

          localnormal = cellLine.GetPointData().GetArray(parallelTransportNormalsArrayName).GetTuple3(j)
          localnormaldot = vtk.vtkMath.Dot(localtangent,localnormal)

          localtangent[0] -= localnormaldot*localnormal[0]
          localtangent[1] -= localnormaldot*localnormal[1]
          localtangent[2] -= localnormaldot*localnormal[2]
          vtk.vtkMath.Normalize(localtangent)

          transform.RotateWXYZ(rotationAngle,localtangent)
          transform.TransformNormal(localnormal,newVoronoiVector)
          vtk.vtkMath.Normalize(newVoronoiVector)

          newVoronoiPoint[0] = point0[0] + voronoiVectorNorm*newVoronoiVector[0]
          newVoronoiPoint[1] = point0[1] + voronoiVectorNorm*newVoronoiVector[1]
          newVoronoiPoint[2] = point0[2] + voronoiVectorNorm*newVoronoiVector[2]

          PTPoints.InsertNextPoint(newVoronoiPoint)
       numberOfPTPoints = PTPoints.GetNumberOfPoints() 

       lastPTPoint = PTPoints.GetPoint(PTPoints.GetNumberOfPoints()-1)
       
       voronoiPointLocator = vtk.vtkPointLocator()
       voronoiPointLocator.SetDataSet(voronoiDataset1)
       voronoiPointLocator.BuildLocator()
     
       arrivalVoronoiPointId = voronoiPointLocator.FindClosestPoint(lastPTPoint)
       arrivalVoronoiPoint = voronoiDataset1.GetPoint(arrivalVoronoiPointId)
       arrivalVoronoiPointRadius = voronoiDataset1.GetPointData().GetArray(radiusArrayName).GetTuple1(arrivalVoronoiPointId)

       arrivalCenterlinePointLocator = vtk.vtkPointLocator()
       arrivalCenterlinePointLocator.SetDataSet(cellLine)
       arrivalCenterlinePointLocator.BuildLocator()
      
       arrivalCenterlineClosestPointId = arrivalCenterlinePointLocator.FindClosestPoint(arrivalVoronoiPoint) 
       arrivalCenterlineClosestPoint = cellLine.GetPoint(arrivalCenterlineClosestPointId)
      
       arrivalVoronoiVector = [0.0,0.0,0.0]
       arrivalVoronoiVector[0] = arrivalVoronoiPoint[0] - arrivalCenterlineClosestPoint[0]  
       arrivalVoronoiVector[1] = arrivalVoronoiPoint[1] - arrivalCenterlineClosestPoint[1]  
       arrivalVoronoiVector[2] = arrivalVoronoiPoint[2] - arrivalCenterlineClosestPoint[2]  
       arrivalVoronoiVectorNorm = vtk.vtkMath.Norm(arrivalVoronoiVector)

       radiusArray = ComputeSpline(voronoiPointRadius,arrivalVoronoiPointRadius,numberOfPTPoints)
       vectorNormArray = ComputeSpline(voronoiVectorNorm,arrivalVoronoiVectorNorm, numberOfPTPoints)

       if (direction==1):
          pointsToGap = gapStartId - closestPointId
       else:
          pointsToGap = closestPointId - gapStartId
          
       pointId = pointsToGap
       for k in range(gapStartId,endSavingInterval,step):
          ptpoint = PTPoints.GetPoint(pointsToGap)
          clpoint = cellLine.GetPoint(k)
    
          vector = [0.0,0.0,0.0]
          vector[0] = ptpoint[0] - clpoint[0]
          vector[1] = ptpoint[1] - clpoint[1]
          vector[2] = ptpoint[2] - clpoint[2]
          vtk.vtkMath.Normalize(vector)
          
          norm = vectorNormArray.GetTuple1(pointsToGap)

          newvector = [0.0,0.0,0.0]
          newvector[0] = norm*vector[0]
          newvector[1] = norm*vector[1]
          newvector[2] = norm*vector[2]
          
          newpoint = [0.0,0.0,0.0]
          newpoint[0] = clpoint[0] + newvector[0] 
          newpoint[1] = clpoint[1] + newvector[1] 
          newpoint[2] = clpoint[2] + newvector[2] 

          finalNewVoronoiPoints.InsertNextPoint(newpoint)
          cellArray.InsertNextCell(1)
          cellArray.InsertCellPoint(count)
          finalRadiusArray.SetTuple1(count,radiusArray.GetTuple1(pointsToGap))
          pointsToGap +=1
          count +=1
       
    return finalNewVoronoiPoints,finalRadiusArray

def ExtractLine(cellid,centerlines):
   cell = vtk.vtkGenericCell()
   centerlines.GetCell(cellid,cell)

   numberOfPoints = cell.GetNumberOfPoints()

   line = vtk.vtkPolyData()
   cellArray = vtk.vtkCellArray()
   cellArray.InsertNextCell(numberOfPoints)

   linePoints = cell.GetPoints()

   ptnArray = vtk.vtkDoubleArray()
   ptnArray.SetNumberOfComponents(3)
   ptnArray.SetNumberOfTuples(numberOfPoints)
   ptnArray.SetName(parallelTransportNormalsArrayName)
   ptnArray.FillComponent(0,0.0)
   ptnArray.FillComponent(1,0.0)
   ptnArray.FillComponent(2,0.0)

   for i in range(numberOfPoints):
      cellArray.InsertCellPoint(i)   
      normal = centerlines.GetPointData().GetArray(parallelTransportNormalsArrayName).GetTuple3(cell.GetPointId(i))
      ptnArray.SetTuple3(i,normal[0],normal[1],normal[2])
       
   line.SetPoints(linePoints)
   line.SetLines(cellArray) 
   line.GetPointData().AddArray(ptnArray)
   return line

def ComputeVoronoiVectorToCenterlineAngle(pointId,vector,centerline):
    point0 = [0.0,0.0,0.0]
    point1 = [0.0,0.0,0.0]
    point2 = [0.0,0.0,0.0]
    centerline.GetPoint(pointId,point0)
    centerline.GetPoint(pointId+1,point1)
    centerline.GetPoint(pointId-1,point2)

    tangent = [0.0,0.0,0.0]
    tangent[0] += point1[0]-point0[0]
    tangent[1] += point1[1]-point0[1]
    tangent[2] += point1[2]-point0[2]
    tangent[0] += point0[0]-point2[0]
    tangent[1] += point0[1]-point2[1]
    tangent[2] += point0[2]-point2[2]

    ptnnormal = centerline.GetPointData().GetArray(parallelTransportNormalsArrayName).GetTuple3(pointId)

    alpha = ComputeAngleBetweenVectors(ptnnormal,tangent,vector)
    return alpha

def ComputeAngleBetweenVectors(normal,tangent,vector):
    #compute the tangent component orthogonal to normal 
    otangent = [0.0,0.0,0.0]
    normalDot = vtk.vtkMath.Dot(tangent,normal)
    otangent[0] = tangent[0] - normalDot*normal[0]
    otangent[1] = tangent[1] - normalDot*normal[1]
    otangent[2] = tangent[2] - normalDot*normal[2]
    vtk.vtkMath.Normalize(otangent)

    #compute the vector component orthogonal to otangent, i.e. parallel to normal
    vtk.vtkMath.Normalize(vector)
    ovector = [0.0,0.0,0.0]
    vectorDot = vtk.vtkMath.Dot(vector,otangent)
    ovector[0] = vector[0] - vectorDot*otangent[0]
    ovector[1] = vector[1] - vectorDot*otangent[1]
    ovector[2] = vector[2] - vectorDot*otangent[2]
    vtk.vtkMath.Normalize(ovector)
 
    theta = vtkvmtk.vtkvmtkMath.AngleBetweenNormals(normal,ovector)
    theta = vtk.vtkMath.DegreesFromRadians(theta)
   
    cross = [0.0,0.0,0.0]
    vtk.vtkMath.Cross(ovector,normal,cross)
    tangentDot = vtk.vtkMath.Dot(otangent,cross)
    if (tangentDot<0.0): theta = -1.0*theta
    angle = -theta
    return angle 

def ComputeSpline(startValue,endValue,numberOfPoints):
   averageValue = (startValue + endValue)/2.0
   averageId = int(numberOfPoints/2) 

   splineArray = vtk.vtkDoubleArray()
   splineArray.SetNumberOfComponents(1)
   splineArray.SetNumberOfTuples(numberOfPoints)
   splineArray.FillComponent(0,0.0)

   spline = vtk.vtkCardinalSpline()
   spline.AddPoint(float(0),startValue)
   spline.AddPoint(float(averageId),averageValue)
   spline.AddPoint(float(numberOfPoints),endValue)
   spline.Compute()   

   for i in range(numberOfPoints):
      splineArray.SetTuple1(i,spline.Evaluate(float(i)))

   return splineArray 

def InsertNewVoronoiPoints(oldDataset,newPoints,newArray):
   numberOfDatasetPoints = oldDataset.GetNumberOfPoints()
   numberOfNewPoints = newPoints.GetNumberOfPoints()
   numberOfNewVoronoiPoints = numberOfDatasetPoints + numberOfNewPoints

   newDataset = vtk.vtkPolyData()
   radiusArray = vtk.vtkDoubleArray()
   radiusArray.SetNumberOfComponents(1)
   radiusArray.SetNumberOfTuples(numberOfNewVoronoiPoints)
   radiusArray.SetName(radiusArrayName) 
   radiusArray.FillComponent(0,0.0)
   cellArray = vtk.vtkCellArray()  
   points = vtk.vtkPoints()

   for i in range(numberOfDatasetPoints):
      point = [0.0,0.0,0.0]
      oldDataset.GetPoint(i,point)
      points.InsertNextPoint(point)
      cellArray.InsertNextCell(1)
      cellArray.InsertCellPoint(i)

      value = oldDataset.GetPointData().GetArray(radiusArrayName).GetTuple1(i)
      radiusArray.SetTuple1(i,value)

   for i in range(numberOfNewPoints):
      point = [0.0,0.0,0.0]
      newPoints.GetPoint(i,point)
      points.InsertNextPoint(point)
      cellArray.InsertNextCell(1)
      cellArray.InsertCellPoint(numberOfDatasetPoints+i)

      value = newArray.GetTuple1(i)
      radiusArray.SetTuple1(numberOfDatasetPoints+i,value)

   newDataset.SetPoints(points)
   newDataset.SetVerts(cellArray)
   newDataset.GetPointData().AddArray(radiusArray)
   return newDataset


# -----------------------------------------------------------------------------------------


## Program:	clipvoronoidiagram.py	
## Language:	Python
## Date:	2012/02/27
## Version:	1.0
## Application: Cerebral Aneurysms - Parent Vessel Reconstruction 
## Author:	Marina Piccinelli

## Description:	Given the parallel transport normal system defined on the interpolated 
##		centerlines computes the trajectory of Voronoi Diagram points from one end
##		to the other of the empty area in the clipped Voronoi Diagram. Saves a new
##		complete Voronoi Diagram and recalculates the model surface by merging of 
##		maximal inscribed spheres. Recommended use of smoothed clipped Voronoi
##		Diagram.


# -----------------------------------------------------------------------------------------


#SOME COMMON VMTK DATA ARRAY NAMES
radiusArrayName = 'MaximumInscribedSphereRadius'
parallelTransportNormalsArrayName = 'ParallelTransportNormals'

#OPTIONS TO SET
interpolationHalfSize = 3
polyBallImageSize = [90,90,90]   #size of the image for the evaluation of the polyball function

#inputfilenames:
print('USAGE:')
print('      ./paralleltransportvoronoidiagram.py inputfilesDirectory caseID smoothVoronoi')
print('')

inputfiledirectory = sys.argv[1]
ID = sys.argv[2]
smoothedVoronoi = int(sys.argv[3])

print('Inputfiles Directory	', inputfiledirectory)
print('case ID			', ID)
print('')

interpolatedCenterlinesFilename = inputfiledirectory + '/' + ID + '/' + ID + '_interpolatedcl.vtp'
patchCenterlinesFilename        = inputfiledirectory + '/' + ID + '/' + ID + '_patchcl.vtp'
if smoothedVoronoi == 1:
   clippedVoronoiFilename       = inputfiledirectory + '/' + ID + '/' + ID + '_smoothclippedvoronoi.vtp'
else:
   clippedVoronoiFilename       = inputfiledirectory + '/' + ID + '/' + ID + '_clippedvoronoi.vtp'
clippingPointsFilename          = inputfiledirectory + '/' + ID + '/' + ID + '_clippingpoints.vtp'

#outputfilenames:
completeVoronoiFilename         = inputfiledirectory + '/' + ID + '/' + ID + '_completevoronoi.vtp'
surfaceFilename                 = inputfiledirectory + '/' + ID + '/' + ID + '_reconstructedmodel.vtp'

interpolatedCenterlines = ReadPolyData(interpolatedCenterlinesFilename)
patchCenterlines = ReadPolyData(patchCenterlinesFilename)
clippedVoronoi = ReadPolyData(clippedVoronoiFilename)
clippingPoints = ReadPolyData(clippingPointsFilename)

completeVoronoiDiagram = vtk.vtkPolyData()
completeVoronoiDiagram.DeepCopy(clippedVoronoi)

numberOfInterpolatedCenterlinesCells = interpolatedCenterlines.GetNumberOfCells()

for j in range(1,numberOfInterpolatedCenterlinesCells+1):
   interpolationCellId = j-1
   startId = 0
   endId = j

   startCell = vtk.vtkGenericCell()
   patchCenterlines.GetCell(startId,startCell)

   startCellPointId = startCell.GetPointId(startCell.GetNumberOfPoints()-1)
   startCellPoint = patchCenterlines.GetPoint(startCellPointId)
   startCellPointRadius = patchCenterlines.GetPointData().GetArray(radiusArrayName).GetTuple1(startCellPointId)
   startCellPointHalfRadius = startCellPointRadius/7.0

   startInterpolationDataset = ExtractCylindricInterpolationVoronoiDiagram(startId,startCellPointId,startCellPointRadius,clippedVoronoi,patchCenterlines)
   startHalfInterpolationDataset = ExtractCylindricInterpolationVoronoiDiagram(startId,startCellPointId,startCellPointHalfRadius,clippedVoronoi,patchCenterlines)

   endCell = vtk.vtkGenericCell()
   patchCenterlines.GetCell(endId,endCell)

   endCellPointId = endCell.GetPointId(0)
   endCellPoint = patchCenterlines.GetPoint(endCellPointId)
   endCellPointRadius = patchCenterlines.GetPointData().GetArray(radiusArrayName).GetTuple1(endCellPointId)
   endCellPointHalfRadius = endCellPointRadius/7.0

   endInterpolationDataset = ExtractCylindricInterpolationVoronoiDiagram(endId,endCellPointId,endCellPointRadius,clippedVoronoi,patchCenterlines)
   endHalfInterpolationDataset = ExtractCylindricInterpolationVoronoiDiagram(endId,endCellPointId,endCellPointHalfRadius,clippedVoronoi,patchCenterlines)

   newVoronoiPoints, newVoronoiPointsMISR = VoronoiDiagramInterpolation(interpolationCellId,startId,endId,startInterpolationDataset,endHalfInterpolationDataset,interpolatedCenterlines,1)
   completeVoronoiDiagram = InsertNewVoronoiPoints(completeVoronoiDiagram,newVoronoiPoints,newVoronoiPointsMISR)

   newVoronoiPoints, newVoronoiPointsMISR = VoronoiDiagramInterpolation(interpolationCellId,endId,startId,endInterpolationDataset,startHalfInterpolationDataset,interpolatedCenterlines,0)
   completeVoronoiDiagram = InsertNewVoronoiPoints(completeVoronoiDiagram,newVoronoiPoints,newVoronoiPointsMISR)

WritePolyData(completeVoronoiDiagram,completeVoronoiFilename)

print('Reconstructing Surface from Voronoi Diagram')
modeller = vtkvmtk.vtkvmtkPolyBallModeller()
modeller.SetInputData(completeVoronoiDiagram)
modeller.SetRadiusArrayName(radiusArrayName)
modeller.UsePolyBallLineOff()
modeller.SetSampleDimensions(polyBallImageSize)
modeller.Update()

marchingCube = vtk.vtkMarchingCubes()
marchingCube.SetInputData(modeller.GetOutput())
marchingCube.SetValue(0,0.0)
marchingCube.Update()  
envelope = marchingCube.GetOutput()
WritePolyData(envelope,surfaceFilename)


