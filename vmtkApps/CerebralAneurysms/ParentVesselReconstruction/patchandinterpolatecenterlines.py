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

def SaveParentArtery(centerlines):
   numberOfCells = centerlines.GetNumberOfCells()

   cell0 = centerlines.GetCell(0)
   numberOfArteryPoints = centerlines.GetNumberOfPoints()-cell0.GetNumberOfPoints()
  
   artery = vtk.vtkPolyData()
   arteryPoints = vtk.vtkPoints()
   arteryCellArray = vtk.vtkCellArray()

   radiusArray = vtk.vtkDoubleArray()
   radiusArray.SetName(radiusArrayName)
   radiusArray.SetNumberOfComponents(1)
   radiusArray.SetNumberOfTuples(numberOfArteryPoints)
   radiusArray.FillComponent(0,0.0)

   count = 0
   for i in range(1,numberOfCells): # cell0 is the one that goes to the aneurysm dome
      cell = vtk.vtkGenericCell()
      centerlines.GetCell(i,cell)
   
      arteryCellArray.InsertNextCell(cell.GetNumberOfPoints())
   
      for j in range(cell.GetNumberOfPoints()):
         arteryPoints.InsertNextPoint(cell.GetPoints().GetPoint(j))
         radiusArray.SetTuple1(count,centerlines.GetPointData().GetArray(radiusArrayName).GetTuple1(cell.GetPointId(j)))
         arteryCellArray.InsertCellPoint(count)
         count+=1
         
   artery.SetPoints(arteryPoints)
   artery.SetLines(arteryCellArray)
   artery.GetPointData().AddArray(radiusArray)
   return artery 

def FindClippingPointOnParentArtery(centerlines,parentCenterlines,toll):
   divergingPointID = -1
   divergingPoint = [0.0,0.0,0.0]
   divergingPointMISR = -1

   clippingPointID = -1
   clippingPoint = [0.0,0.0,0.0]
   
   cell0PointIds = vtk.vtkIdList()
   cell1PointIds = vtk.vtkIdList()

   centerlines.GetCellPoints(0,cell0PointIds) #this is the cl that goes through the aneurysm
   centerlines.GetCellPoints(1,cell1PointIds)
 
   for i in range(0,min(cell0PointIds.GetNumberOfIds(),cell1PointIds.GetNumberOfIds())):
      cell0Point = centerlines.GetPoint(cell0PointIds.GetId(i))
      cell1Point = centerlines.GetPoint(cell1PointIds.GetId(i))

      distanceBetweenPoints = math.sqrt(vtk.vtkMath.Distance2BetweenPoints(cell0Point,cell1Point))
      if (distanceBetweenPoints>toll):
        divergingPointID = cell1PointIds.GetId(i)
        divergingPoint = centerlines.GetPoint(cell1PointIds.GetId(i))
        divergingPointMISR = centerlines.GetPointData().GetArray(radiusArrayName).GetTuple1(cell1PointIds.GetId(i))
        break
  
   MISphere = vtk.vtkSphere()
   MISphere.SetCenter(divergingPoint)
   MISphere.SetRadius(divergingPointMISR)
   tempPoint = [0.0,0.0,0.0]

   for i in range(divergingPointID,0,-1):
      value = MISphere.EvaluateFunction(centerlines.GetPoint(i))
      if (value>=0.0):
         tempPoint = centerlines.GetPoint(i) 
         break
       
   locator = vtk.vtkPointLocator()
   locator.SetDataSet(parentCenterlines)
   locator.BuildLocator()
 
   clippingPointID = locator.FindClosestPoint(tempPoint)
   clippingPoint = parentCenterlines.GetPoint(clippingPointID)

   return clippingPoint,divergingPoint

def SaveClippingPoints(points,filename):
   pointSet = vtk.vtkPolyData()
   cellArray = vtk.vtkCellArray()

   for i in range(points.GetNumberOfPoints()):
      cellArray.InsertNextCell(1)
      cellArray.InsertCellPoint(i)
   
   pointSet.SetPoints(points)
   pointSet.SetVerts(cellArray)

   WritePolyData(pointSet,filename)   
  
def CreateParentArteryPatches(parentCenterlines,clipPoints):
   numberOfDaughterPatches = parentCenterlines.GetNumberOfCells()

   patchedCenterlines = vtk.vtkPolyData()
   patchedCenterlinesPoints = vtk.vtkPoints()
   patchedCenterlinesCellArray = vtk.vtkCellArray()
   patchedRadiusArray = vtk.vtkDoubleArray()

   clipIds,numberOfPatchedCenterlinesPoints = ExtractPatchesIds(parentCenterlines,clipPoints)
   print('Clipping Point Ids ', clipIds)

   radiusArray = vtk.vtkDoubleArray()
   radiusArray.SetNumberOfComponents(1)
   radiusArray.SetName(radiusArrayName)
   radiusArray.SetNumberOfTuples(numberOfPatchedCenterlinesPoints) 
   radiusArray.FillComponent(0,0.0)

   numberOfCommonPatch = clipIds[0]+1
   patchedCenterlinesCellArray.InsertNextCell(numberOfCommonPatch)

   count = 0
   for i in range(0,numberOfCommonPatch):
      patchedCenterlinesPoints.InsertNextPoint(parentCenterlines.GetPoint(i))
      patchedCenterlinesCellArray.InsertCellPoint(i)
      radiusArray.SetTuple1(i,parentCenterlines.GetPointData().GetArray(radiusArrayName).GetTuple1(i))
      count+=1 
 
   for j in range(numberOfDaughterPatches):
      cell = vtk.vtkGenericCell()
      parentCenterlines.GetCell(j,cell)

      numberOfCellPoints = cell.GetNumberOfPoints()
      startId = clipIds[j+1]
      patchNumberOfPoints = numberOfCellPoints-startId
      patchedCenterlinesCellArray.InsertNextCell(patchNumberOfPoints)

      for i in range(startId, cell.GetNumberOfPoints()):
         point = cell.GetPoints().GetPoint(i)
         patchedCenterlinesPoints.InsertNextPoint(point)
         patchedCenterlinesCellArray.InsertCellPoint(count)
         radiusArray.SetTuple1(count,parentCenterlines.GetPointData().GetArray(radiusArrayName).GetTuple1(cell.GetPointId(i)))
         count+=1

   patchedCenterlines.SetPoints(patchedCenterlinesPoints)
   patchedCenterlines.SetLines(patchedCenterlinesCellArray)
   patchedCenterlines.GetPointData().AddArray(radiusArray)

   return patchedCenterlines

def ExtractPatchesIds(parentCl,clipPts):
   clipIds = []
   numberOfPoints = 0

   if (clipPts.GetNumberOfPoints() == 2):
      upstreamPoint = clipPts.GetPoint(0)
      downstreamPoint = clipPts.GetPoint(1)

      for j in range(parentCl.GetNumberOfCells()):
         cellLine = ExtractSingleLine(parentCl,j)

         locator = vtk.vtkPointLocator()
         locator.SetDataSet(cellLine)
         locator.BuildLocator()

         upstreamId = locator.FindClosestPoint(upstreamPoint)
         downstreamId = locator.FindClosestPoint(downstreamPoint)

         if j==0:
            clipIds.append(upstreamId)
            clipIds.append(downstreamId)
            numberOfPoints += upstreamId+1
            numberOfPoints += cellLine.GetNumberOfPoints()-downstreamId
         else:
            clipIds.append(downstreamId)
            numberOfPoints += cellLine.GetNumberOfPoints()-downstreamId

   if (clipPts.GetNumberOfPoints() == 3):
      commonPoint = clipPts.GetPoint(0)
      dau1Point = clipPts.GetPoint(1)
      dau2Point = clipPts.GetPoint(2)
      for j in range(parentCl.GetNumberOfCells()):
         cellLine = ExtractSingleLine(parentCl,j)

         locator = vtk.vtkPointLocator()
         locator.SetDataSet(cellLine)
         locator.BuildLocator()

         if j==0:
            upstreamId = locator.FindClosestPoint(commonPoint)
            downstreamId = locator.FindClosestPoint(dau1Point)
            clipIds.append(upstreamId)
            clipIds.append(downstreamId)
            numberOfPoints += upstreamId+1
            numberOfPoints += cellLine.GetNumberOfPoints()-downstreamId
         else:
            downstreamId = locator.FindClosestPoint(dau2Point)
            clipIds.append(downstreamId)
            numberOfPoints += cellLine.GetNumberOfPoints()-downstreamId

   return clipIds,numberOfPoints

def ExtractSingleLine(centerlines,id):
   cell = vtk.vtkGenericCell()
   centerlines.GetCell(id,cell) 

   line = vtk.vtkPolyData()
   points = vtk.vtkPoints()
   cellArray = vtk.vtkCellArray()
   cellArray.InsertNextCell(cell.GetNumberOfPoints())

   radiusArray = vtk.vtkDoubleArray()
   radiusArray.SetName(radiusArrayName)
   radiusArray.SetNumberOfComponents(1)
   radiusArray.SetNumberOfTuples(cell.GetNumberOfPoints())
   radiusArray.FillComponent(0,0.0)

   for i in range(cell.GetNumberOfPoints()):
      point = [0.0,0.0,0.0]
      point = cell.GetPoints().GetPoint(i)

      points.InsertNextPoint(point)
      cellArray.InsertCellPoint(i)
      radius = centerlines.GetPointData().GetArray(radiusArrayName).GetTuple1(cell.GetPointId(i))
      radiusArray.SetTuple1(i,radius)

   line.SetPoints(points)   
   line.SetLines(cellArray)
   line.GetPointData().AddArray(radiusArray)

   return line 

def InterpolatePatchCenterlines(patchCenterlines,parentCenterlines):
   additionalPoint = [-1.0,-1.0,-1.0]
   additionalPointIds = [] 

   if (useAdditionalInterpolationPoint == 1):
      additionalPoint = divergingPoints.GetPoint(0)
      line1 = ExtractSingleLine(parentCenterlines,0) 
      line2 = ExtractSingleLine(parentCenterlines,1) 
      additionalPointIds.append(line1.FindPoint(additionalPoint))
      additionalPointIds.append(line2.FindPoint(additionalPoint))
   else:
      for i in range(parentCenterlines.GetNumberOfCells()):	   
          additionalPoint = clippingPoints.GetPoint(0)
          line1 = ExtractSingleLine(parentCenterlines,0)
          additionalPointIds.append(line1.FindPoint(additionalPoint)) 
  
   interpolatedLines = vtk.vtkPolyData()
   interpolatedPoints = vtk.vtkPoints()
   interpolatedCellArray = vtk.vtkCellArray()

   pointsInserted = 0
   interpolatedCellArray.Initialize()

   for i in range(parentCenterlines.GetNumberOfCells()):
      startingCell = vtk.vtkGenericCell()
      endingCell = vtk.vtkGenericCell()

      numberOfInterpolationPoints = parentCenterlines.GetCell(i).GetNumberOfPoints()

      patchCenterlines.GetCell(0,startingCell)
      patchCenterlines.GetCell(i+1,endingCell)
     
      splinePoints = InterpolateTwoCells(startingCell,endingCell,numberOfInterpolationPoints,additionalPointIds[i],additionalPoint)

      interpolatedCellArray.InsertNextCell(splinePoints.GetNumberOfPoints())
      for j in range(splinePoints.GetNumberOfPoints()):
         interpolatedPoints.InsertNextPoint(splinePoints.GetPoint(j))
         interpolatedCellArray.InsertCellPoint(pointsInserted + j)
      pointsInserted += splinePoints.GetNumberOfPoints()

   interpolatedLines.SetPoints(interpolatedPoints)
   interpolatedLines.SetLines(interpolatedCellArray)
 
   attributeFilter = vtkvmtk.vtkvmtkCenterlineAttributesFilter()
   attributeFilter.SetInputData(interpolatedLines)
   attributeFilter.SetAbscissasArrayName(abscissasArrayName)
   attributeFilter.SetParallelTransportNormalsArrayName(parallelTransportNormalsArrayName)
   attributeFilter.Update()

   attributeInterpolatedLines = attributeFilter.GetOutput()
  
   return attributeInterpolatedLines
 
def InterpolateTwoCells(startCell,endCell,numberOfSplinePoints,additionalPointId,additionalPoint):
   points = vtk.vtkPoints()
   xspline = vtk.vtkCardinalSpline()
   yspline = vtk.vtkCardinalSpline()
   zspline = vtk.vtkCardinalSpline()
   
   numberOfStartCellPoints = startCell.GetNumberOfPoints()
   numberOfEndCellPoints = endCell.GetNumberOfPoints()
   endCellFirstId = numberOfSplinePoints - numberOfEndCellPoints
   numberOfClippedCenterlinePoints = numberOfStartCellPoints + numberOfEndCellPoints

   for i in range(numberOfStartCellPoints):
      point = startCell.GetPoints().GetPoint(i)
      xspline.AddPoint(float(i),point[0])
      yspline.AddPoint(float(i),point[1])
      zspline.AddPoint(float(i),point[2])

   if (useAdditionalInterpolationPoint == 1):
      xspline.AddPoint(float(additionalPointId),additionalPoint[0])
      yspline.AddPoint(float(additionalPointId),additionalPoint[1])
      zspline.AddPoint(float(additionalPointId),additionalPoint[2])

   for i in range(numberOfEndCellPoints):
      point = endCell.GetPoints().GetPoint(i)
      index = endCellFirstId+i
      xspline.AddPoint(float(endCellFirstId + i),point[0])
      yspline.AddPoint(float(endCellFirstId + i),point[1])
      zspline.AddPoint(float(endCellFirstId + i),point[2])
   xspline.Compute()
   yspline.Compute()  
   zspline.Compute()
   
   points.SetNumberOfPoints(numberOfSplinePoints)
   for i in range(numberOfSplinePoints):
      points.SetPoint(i,xspline.Evaluate(float(i)),yspline.Evaluate(float(i)),zspline.Evaluate(float(i)))
   return points


# -----------------------------------------------------------------------------------------


## Program:	patchandinterpolatecenterlines.py	
## Language:	Python
## Date:	2012/02/27
## Version:	1.0
## Application: Cerebral Aneurysms - Parent Vessel Reconstruction 
## Author:	Marina Piccinelli

## Description: Given centerlines traveling along parent vessels and to the aneurysm
##		dome, identifies clipping points on centerlines delimiting aneurysm,
##		eliminates the portion of centerlines within clipping points and 
##		interpolates with cubic splines the empty regions. It is possible to 
##		directly provide clipping points and parent vasculature centerlines.	


# -----------------------------------------------------------------------------------------


#SOME COMMON VMTK DATA ARRAY NAMES
radiusArrayName = 'MaximumInscribedSphereRadius'
abscissasArrayName = 'Abscissas'
parallelTransportNormalsArrayName = 'ParallelTransportNormals'

#OPTIONS TO SET 
useAdditionalInterpolationPoint = 0      # automatically set to 1 for terminal aneurysms  
setClippingPoints = 0                    # set to 1 if clipping points will be provided not computed from the centerlines
divergingRatioToSpacingTolerance = 2.0   # commonly 2.0; the DivergingTolerange for the identification of diverging points on each couple of centerlines is computed as (centerlineSpacing / DivergingRatioToSpacingTolerance)

#inputfilename:
print('USAGE:')
print('      ./patchandinterpolatecenterlines.py inputfilesDirectory caseID aneurysmType')
print('')

inputfiledirectory = sys.argv[1]
ID = sys.argv[2]
aneurysmType = sys.argv[3]

print('Inputfiles Directory	', inputfiledirectory)
print('case ID			', ID)
print('Aneurysm Type		', aneurysmType)
print('')

# outputfilenames:
divergingPointsFilename         = inputfiledirectory + '/' + ID + '/' + ID + '_divergingpoints.vtp'
patchParentArteryFilename       = inputfiledirectory + '/' + ID + '/' + ID + '_patchcl.vtp'
interpolatedCenterlinesFilename = inputfiledirectory + '/' + ID + '/' + ID + '_interpolatedcl.vtp'

if (aneurysmType == 'lateral'):

   if (setClippingPoints == 0):
      forwardCenterlinesFilename  = inputfiledirectory + '/' + ID + '/'+ ID + '_forwardcl.vtp'
      backwardCenterlinesFilename = inputfiledirectory + '/' + ID + '/'+ ID + '_backwardcl.vtp'
      clippingPointsFilename      = inputfiledirectory + '/' + ID + '/' + ID + '_clippingpoints.vtp'

      forwardCenterlines      = ReadPolyData(forwardCenterlinesFilename)
      backwardCenterlines     = ReadPolyData(backwardCenterlinesFilename)
      parentArteryCenterlines = SaveParentArtery(forwardCenterlines)

      centerlineSpacing = math.sqrt(vtk.vtkMath.Distance2BetweenPoints(parentArteryCenterlines.GetPoint(10),parentArteryCenterlines.GetPoint(11)))
      divergingTolerance = centerlineSpacing/divergingRatioToSpacingTolerance

      upstreamClippingPoint   = [0.0,0.0,0.0]
      downstreamClippingPoint = [0.0,0.0,0.0]
      clippingPoints = vtk.vtkPoints()
      divergingPoints = vtk.vtkPoints()
      
      print('Computing Clipping Points')
      upstreamClippingPoint,upstreamDivergingPoint = FindClippingPointOnParentArtery(forwardCenterlines,parentArteryCenterlines,divergingTolerance)
      clippingPoints.InsertNextPoint(upstreamClippingPoint)
      divergingPoints.InsertNextPoint(upstreamDivergingPoint)

      downstreamClippingPoint,downstreamDivergingPoint = FindClippingPointOnParentArtery(backwardCenterlines,parentArteryCenterlines,divergingTolerance)
   
      for i in range(parentArteryCenterlines.GetNumberOfCells()): 
         clippingPoints.InsertNextPoint(downstreamClippingPoint)
         divergingPoints.InsertNextPoint(downstreamDivergingPoint)
   
      SaveClippingPoints(clippingPoints,clippingPointsFilename)
      SaveClippingPoints(divergingPoints,divergingPointsFilename)

   elif (setClippingPoints == 1):
      print('Clipping Points and Parent Vessel Centerlines provided')
      parentArteryFileName   = inputfiledirectory + '/' + ID + '/' + ID + '_parentvessel.vtp'
      clippingPointsFileName = inputfiledirectory + '/' + ID + '/' + ID + '_clippingpoints.vtp'

      clippingPoints          = ReadPolyData(clippingPointsFileName)
      parentArteryCenterlines = ReadPolyData(parentArteryFileName)

   print('Creating Patched Centerlines')
   patchParentArteryCenterlines = CreateParentArteryPatches(parentArteryCenterlines,clippingPoints)
   WritePolyData(patchParentArteryCenterlines,patchParentArteryFilename)

   print('Interpolating Patched Centerlines')
   interpolatedCenterlines = InterpolatePatchCenterlines(patchParentArteryCenterlines,parentArteryCenterlines)

   WritePolyData(interpolatedCenterlines,interpolatedCenterlinesFilename)

if (aneurysmType == 'terminal'):

   useAdditionalInterpolationPoint = 1

   if (setClippingPoints == 0):
      parentArteryCenterlinesFilename = inputfiledirectory + '/' + ID + '/' + ID + '_parentvessel.vtp'
      daughter1CenterlinesFilename    = inputfiledirectory + '/' + ID + '/' + ID + '_dau1cl.vtp'
      daughter2CenterlinesFilename    = inputfiledirectory + '/' + ID + '/' + ID + '_dau2cl.vtp'
      clippingPointsFilename          = inputfiledirectory + '/' + ID + '/' + ID + '_clippingpoints.vtp'

      parentArteryCenterlines = ReadPolyData(parentArteryCenterlinesFilename)
      daughter1Centerlines    = ReadPolyData(daughter1CenterlinesFilename)
      daughter2Centerlines    = ReadPolyData(daughter2CenterlinesFilename)

      centerlineSpacing = math.sqrt(vtk.vtkMath.Distance2BetweenPoints(parentArteryCenterlines.GetPoint(10),parentArteryCenterlines.GetPoint(11)))
      divergingTolerance = centerlineSpacing/divergingRatioToSpacingTolerance

      commonClippingPoint = [0.0,0.0,0.0]
      dau1ClippingPoint = [0.0,0.0,0.0]
      dau2ClippingPoint = [0.0,0.0,0.0]
      clippingPoints = vtk.vtkPoints()
      divergingPoints = vtk.vtkPoints()

      commonClippingPoint,commonDivergingPoint = FindClippingPointOnParentArtery(parentArteryCenterlines,parentArteryCenterlines,divergingTolerance)
      clippingPoints.InsertNextPoint(commonClippingPoint)
      divergingPoints.InsertNextPoint(commonDivergingPoint)

      dau1ClippingPoint,dau1DivergingPoint = FindClippingPointOnParentArtery(daughter1Centerlines,parentArteryCenterlines,divergingTolerance)
      clippingPoints.InsertNextPoint(dau1ClippingPoint)
      divergingPoints.InsertNextPoint(dau1DivergingPoint)

      dau2ClippingPoint,dau2DivergingPoint = FindClippingPointOnParentArtery(daughter2Centerlines,parentArteryCenterlines,divergingTolerance)
      clippingPoints.InsertNextPoint(dau2ClippingPoint)
      divergingPoints.InsertNextPoint(dau2DivergingPoint)
  
      SaveClippingPoints(clippingPoints,clippingPointsFilename)
      SaveClippingPoints(divergingPoints,divergingPointsFilename)

   elif (setClippingPoints == 1):
      print('Clipping Points and Parent Vessel Centerlines provided')
      parentArteryFileName   = inputfiledirectory + '/' + ID + '/' + ID + '_parentvessel.vtp'
      clippingPointsFileName = inputfiledirectory + '/' + ID + '/' + ID + '_clippingpoints.vtp'

      clippingPoints          = ReadPolyData(clippingPointsFileName)
      parentArteryCenterlines = ReadPolyData(parentArteryFileName)

   print('Creating Patched Centerlines')
   patchParentArteryCenterlines = CreateParentArteryPatches(parentArteryCenterlines,clippingPoints)
   WritePolyData(patchParentArteryCenterlines,patchParentArteryFilename)

   print('Interpolating Patched Centerlines')
   interpolatedCenterlines = InterpolatePatchCenterlines(patchParentArteryCenterlines,parentArteryCenterlines)

   WritePolyData(interpolatedCenterlines,interpolatedCenterlinesFilename)


