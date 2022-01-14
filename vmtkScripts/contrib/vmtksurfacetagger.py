#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceclipper.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.9 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Marco Fedele (marco.fedele@polimi.it)
##       Politecnico di Milano

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys
import math

from vmtk import vmtkrenderer
from vmtk import pypes


class vmtkSurfaceTagger(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Method = 'cliparray'
        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.CellEntityIdsArray = None
        self.ArrayName = None
        self.Array = None
        self.Value = None
        self.Range = None
        self.InsideTag = 2
        self.OutsideTag = 1
        self.OverwriteOutsideTag = 0
        self.InsideOut = 0
        self.TagsToModify = None
        self.ConnectivityOffset = 1
        self.TagSmallestRegion = 1
        self.CleanOutput = 1
        self.PrintTags = 1
        self.Tags = None
        self.HarmonicRadius = 1.0
        self.HarmonicGenerateTag = 0
        self.HarmonicCleaningFixPoints = 0

        self.SetScriptName('vmtksurfacetagger')
        self.SetScriptDoc('tag a surface exploiting an array defined on it')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['Method','method','str',1,'["cliparray","array","harmonic","connectivity","constant","drawing"]','tagging method (cliparray: exploit an array to clip the surface at a certain value tagging the two parts, it creates skew triangles that need a successive remeshing; array: the same of cliparray, but without clipping the original triangles, thus creating a zig-zag tag; harmonic: move harmonically the original points of the input surface toward the array value in order to be able to obtain a precise tag also with a successive call of the array method without the need of remeshing; connectivity: given an already tagged surface, tag disconnected part of each input tag; constant: assign a constant tag to the input surface; drawing: interactive drawing a region)'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'','name of the array where the tags are stored'],
            ['ArrayName','array','str',1,'','name of the array with which to define the boundary between tags'],
            ['Value','value','float',1,'','scalar value of the array identifying the boundary between tags'],
            ['Range','range','float',2,'','range scalar values of the array identifying the region for the new tag (alternative to value, only array method)'],
            ['InsideTag','inside','int',1,'','tag of the inside region (i.e. where the Array is lower than Value; used also in case of "constant" method)'],
            ['HarmonicRadius','harmonicradius','float',1,'','buffer zone radius for the harmonic method beyond which the points are not moved'],
            ['HarmonicGenerateTag','harmonicgeneratetag','float',1,'','toggle tagging with the array method after the harmonic movement, it is suggested not to tag directly the surface, but to recompute the array on the warped surface and to use the array method on the recomputed array'],
            ['HarmonicCleaningFixPoints','harmoniccleanfixpoints','bool',1,'','toggle if the cleaning harmonic method has to fix the points or to leave them free'],
            ['OverwriteOutsideTag','overwriteoutside','bool',1,'','overwrite outside value also when the CellEntityIdsArray already exists in the input surface'],
            ['OutsideTag','outside','int',1,'','tag of the outside region (i.e. where the Array is greater than Value)'],
            ['InsideOut','insideout','bool',1,'','toggle switching inside and outside tags ("cliparray" and "array" methods, only when specifying value and not range)'],
            ['TagsToModify','tagstomodify','int',-1,'','if set, new tag is created only in this subset of existing tags ("cliparray" only)'],
            ['ConnectivityOffset','offset','int',1,'','offset added to the entityidsarray of each disconnected parts of each input tag ("connectivity" only)'],
            ['TagSmallestRegion','tagsmallestregion','bool',1,'','toggle tagging the smallest or the largest region (drawing only)'],
            ['CleanOutput','cleanoutput','bool',1,'','toggle cleaning the unused points'],
            ['PrintTags','printtags','bool',1,'','toggle printing the set of tags']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ['CellEntityIdsArray','oentityidsarray','vtkIntArray',1,'','the output entity ids array']
            ])

    def CleanSurface(self):
        cleaner = vtk.vtkCleanPolyData()
        cleaner.SetInputData(self.Surface)
        cleaner.Update()
        self.Surface = cleaner.GetOutput()
        self.CellEntityIdsArray = self.Surface.GetCellData().GetArray(self.CellEntityIdsArrayName)

    def ClipArrayTagger(self,onlyRing=False):

        from vmtk import vmtkscripts

        # clip the surface according to the Array
        clipper = vmtkscripts.vmtkSurfaceClipper()
        clipper.Surface = self.Surface
        clipper.Interactive = False
        clipper.InsideOut = 1-self.InsideOut # inside means regions where the Array is lower than Value
        clipper.CleanOutput = self.CleanOutput
        clipper.ClipArrayName = self.ArrayName
        clipper.ClipValue = self.Value

        clipper.Execute()

        if onlyRing:
            return clipper.CutLines
        else:
            insideSurface = clipper.Surface
            outsideSurface = clipper.ClippedSurface

            # change values of the inside tags
            insideCellEntityIdsArray = insideSurface.GetCellData().GetArray( self.CellEntityIdsArrayName )
            outsideCellEntityIdsArray = outsideSurface.GetCellData().GetArray( self.CellEntityIdsArrayName )

            if self.TagsToModify!=None:
                for i in range(insideCellEntityIdsArray.GetNumberOfTuples()):
                    if insideCellEntityIdsArray.GetValue(i) in self.TagsToModify:
                        insideCellEntityIdsArray.SetValue(i,self.InsideTag)
            else:
                insideCellEntityIdsArray.FillComponent(0,self.InsideTag)

            # merge the inside and the outside surfaces
            mergeSurface = vtk.vtkAppendPolyData()
            mergeSurface.AddInputData(insideSurface)
            mergeSurface.AddInputData(outsideSurface)
            mergeSurface.Update()

            self.Surface = mergeSurface.GetOutput()
            self.CellEntityIdsArray = self.Surface.GetCellData().GetArray(self.CellEntityIdsArrayName)

    def ArrayTagger(self,surface=None,arrayName=None,insideTag=None,rangeValues=[]):
        if surface == None:
            surface = self.Surface
        if arrayName == None:
            arrayName = self.ArrayName
        if insideTag == None:
            insideTag = self.InsideTag
        if rangeValues == []:
            rangeValues = self.Range
        pointsToCells = vtk.vtkPointDataToCellData()
        pointsToCells.SetInputData(surface)
        pointsToCells.PassPointDataOn()
        pointsToCells.Update()
        surface = pointsToCells.GetPolyDataOutput()
        cellEntityIdsArray = surface.GetCellData().GetArray(self.CellEntityIdsArrayName)
        cellArray = surface.GetCellData().GetArray(arrayName)
        for i in range(surface.GetNumberOfCells()):
            if cellArray.GetValue(i) > rangeValues[0] and cellArray.GetValue(i) < rangeValues[1]:
                cellEntityIdsArray.SetValue(i,insideTag)
        return surface

    def CleanPreciseRingDistance(self,ring):
        from vmtk import vmtkscripts

        def nextPointId(ring,cellId,currentPointId):
            idList = vtk.vtkIdList()
            ring.GetCellPoints(cellId,idList)
            if idList.GetId(0) == currentPointId:
                return idList.GetId(1)
            else:
                return idList.GetId(0)

        def nextCellId(ring,pointId,currentCellId):
            idList = vtk.vtkIdList()
            ring.GetPointCells(pointId,idList)
            if idList.GetId(0) == currentCellId:
                return idList.GetId(1)
            else:
                return idList.GetId(0)

        def checkThreeConsecutivePointsOnATriangle(lastThreeCellIdLists):
            for item in lastThreeCellIdLists[2]:
                if item in lastThreeCellIdLists[1]:
                    if item in lastThreeCellIdLists[0]:
                        return True
            return False

        nP = ring.GetNumberOfPoints()
        nC = ring.GetNumberOfCells()
        # print ("points and cells: ", nP, ", ", nC)

        pointLocator = vtk.vtkPointLocator()
        pointLocator.SetDataSet(self.Surface)
        pointLocator.BuildLocator()

        lastThreePointsIds = []
        lastThreeCellIdLists = []
        distanceCleaned = [0, 0, 0, 0]
        cleanRingPointsIds = set()

        currentCellId = 0
        pointIdList = vtk.vtkIdList()
        cellIdList = vtk.vtkIdList()

        ring.GetCellPoints(currentCellId,pointIdList)
        currentRingPointId = pointIdList.GetId(0)

        for i in range(nP):
            lastThreePointsIds.append(currentRingPointId)

            currentSurfPointId = pointLocator.FindClosestPoint(ring.GetPoint(currentRingPointId))
            self.Surface.GetPointCells(currentSurfPointId,cellIdList)
            cellIds=[]
            for k in range(cellIdList.GetNumberOfIds()):
                cellIds.append(cellIdList.GetId(k))
            lastThreeCellIdLists.append(cellIds)

            currentCellId = nextCellId(ring,currentRingPointId,currentCellId)
            currentRingPointId = nextPointId(ring,currentCellId,currentRingPointId)

            if i > 1:
                # print("last three points: ",lastThreePointsIds)
                # print("last three cell id Lists: ",lastThreeCellIdLists)
                answer = checkThreeConsecutivePointsOnATriangle(lastThreeCellIdLists)
                # print("answer: ", answer)
                if answer:
                    if distanceCleaned[1] == 0:
                        distanceCleaned[2] = 1
                        cleanRingPointsIds.add(lastThreePointsIds[1])
                    else:
                        distanceCleaned[1] = 1
                        cleanRingPointsIds.add(lastThreePointsIds[0])

                # print("distance cleaned: ", distanceCleaned)
                # print("")
                lastThreePointsIds.pop(0)
                lastThreeCellIdLists.pop(0)
                distanceCleaned.append(0)
                distanceCleaned.pop(0)

        cleanRingPointsIds = sorted(cleanRingPointsIds)
        print(cleanRingPointsIds)

        if self.HarmonicCleaningFixPoints:
            outputRing = ring
            distanceArray = outputRing.GetPointData().GetArray('PreciseRingDistance')
        else:
            outputRing = vtk.vtkPolyData()
            distanceArray = vtk.vtkDoubleArray()

        if self.HarmonicCleaningFixPoints:
            for pointId in cleanRingPointsIds:
                distanceArray.SetComponent(pointId,0,0.0)
                distanceArray.SetComponent(pointId,1,0.0)
                distanceArray.SetComponent(pointId,2,0.0)
        else:
            points = vtk.vtkPoints()
            lines = vtk.vtkCellArray()
            countCleanedPoints = 0
            ko = 1
            firstInsertedId = 0
            while ko:
                if firstInsertedId not in cleanRingPointsIds:
                    ko = 0
                else:
                    firstInsertedId = firstInsertedId + 1
            lastInsertedId = firstInsertedId

            for i in range(lastInsertedId+1,nP):
                # currentSurfPointId = pointLocator.FindClosestPoint(ring.GetPoint(i))
                if i not in cleanRingPointsIds:
                    # print ('IDS: ring = ',i,'; surface = ',currentSurfPointId)
                    points.InsertNextPoint(ring.GetPoint(i))
                    line = vtk.vtkLine()
                    line.GetPointIds().SetId(0,lastInsertedId)
                    line.GetPointIds().SetId(1,i)
                    lines.InsertNextCell(line)
                    lastInsertedId = i
                else:
                    countCleanedPoints = countCleanedPoints + 1
                    # print ('IDS: ring = ',i,'; surface = ',currentSurfPointId)
                    print ('IDS: ring = ',i)

            # cloase the loop
            line = vtk.vtkLine()
            line.GetPointIds().SetId(0,lastInsertedId)
            line.GetPointIds().SetId(1,firstInsertedId)
            lines.InsertNextCell(line)

            print('\ncleaned points: ',countCleanedPoints,'/',nP,'\n')

            outputRing.SetPoints(points)
            outputRing.SetLines(lines)
            surfaceProjection = vmtkscripts.vmtkSurfaceProjection()
            surfaceProjection.Surface = outputRing
            surfaceProjection.ReferenceSurface = ring
            surfaceProjection.Execute()
            outputRing = surfaceProjection.Surface

        # FIRST AND LAST POINTS NOT YET CHECKED

        return outputRing

    def HarmonicTagger(self):
        from vmtk import vmtkscripts
        from vmtk import vmtkcontribscripts
        from vmtk import vtkvmtk

        def zigZagRingExtractor(surface,arrayname,tag,rangevalues):
            surf = vtk.vtkPolyData()
            surf.DeepCopy(surface)
            surf = self.ArrayTagger(surf,arrayname,tag,rangevalues)
            th = vmtkcontribscripts.vmtkThreshold()
            th.Surface = surf
            th.ArrayName = self.CellEntityIdsArrayName
            th.CellData = True
            th.LowThreshold = tag
            th.HighThreshold = tag
            th.Execute()
            surf = th.Surface

            # boundaryExtractor = vtkvmtk.vtkvmtkPolyDataBoundaryExtractor()
            # boundaryExtractor.SetInputData(surf)
            # boundaryExtractor.Update()
            # zigZagRing = boundaryExtractor.GetOutput()

            featureEdges = vtk.vtkFeatureEdges()
            featureEdges.SetInputData(surf)
            featureEdges.BoundaryEdgesOn()
            featureEdges.FeatureEdgesOff()
            featureEdges.NonManifoldEdgesOff()
            featureEdges.ManifoldEdgesOff()
            featureEdges.ColoringOff()
            featureEdges.CreateDefaultLocator()
            featureEdges.Update()
            zigZagRing = featureEdges.GetOutput()
            return zigZagRing

        tags = set()
        for i in range(self.Surface.GetNumberOfCells()):
            tags.add(self.CellEntityIdsArray.GetComponent(i,0))
        tags = sorted(tags)

        # use clip-array method only to extract the ring
        preciseRing = self.ClipArrayTagger(True)

        if self.HarmonicGenerateTag:
            self.ArrayTagger()

        zigZagRing = zigZagRingExtractor(self.Surface,self.ArrayName,12345,[-math.inf, self.Value])

        surfaceDistance = vmtkscripts.vmtkSurfaceDistance()
        surfaceDistance.Surface = zigZagRing
        surfaceDistance.ReferenceSurface = preciseRing
        surfaceDistance.DistanceVectorsArrayName = 'PreciseRingDistance'
        surfaceDistance.Execute()
        zigZagRing = surfaceDistance.Surface

        passArray = vtk.vtkPassArrays()
        passArray.SetInputData(zigZagRing)
        passArray.AddPointDataArray('PreciseRingDistance')
        passArray.Update()
        zigZagRing = passArray.GetOutput()

        zigZagRing = self.CleanPreciseRingDistance(zigZagRing)

        writer = vtk.vtkXMLPolyDataWriter()
        writer.SetInputData(zigZagRing)
        writer.SetFileName('zigZagRing.vtp')
        writer.SetDataModeToBinary()
        writer.Write()

        surfaceDistance2 = vmtkscripts.vmtkSurfaceDistance()
        surfaceDistance2.Surface = self.Surface
        surfaceDistance2.ReferenceSurface = zigZagRing
        surfaceDistance2.DistanceArrayName = 'ZigZagRingDistance'
        surfaceDistance2.Execute()
        self.Surface = surfaceDistance2.Surface
        print('OK!')

        homogeneousBoundaries = zigZagRingExtractor(self.Surface,'ZigZagRingDistance',2435,[-math.inf,self.HarmonicRadius])

        pointLocator = vtk.vtkPointLocator()
        pointLocator.SetDataSet(self.Surface)
        pointLocator.BuildLocator()

        for k in range(3):
            print("Harmonic extension of component ",k)
            boundaryIds = vtk.vtkIdList()
            temperature = vtk.vtkDoubleArray()
            temperature.SetNumberOfComponents(1)

            for i in range(homogeneousBoundaries.GetNumberOfPoints()):
                idb = pointLocator.FindClosestPoint(homogeneousBoundaries.GetPoint(i))
                boundaryIds.InsertNextId(idb)
                temperature.InsertNextTuple1(0.0)

            warpArray = zigZagRing.GetPointData().GetArray('PreciseRingDistance')
            for i in range(zigZagRing.GetNumberOfPoints()):
                idb = pointLocator.FindClosestPoint(zigZagRing.GetPoint(i))
                boundaryIds.InsertNextId(idb)
                #temperature.InsertNextTuple1(1.0)
                temperature.InsertNextTuple1(warpArray.GetComponent(i,k))

            # perform harmonic mapping using temperature as boundary condition
            harmonicMappingFilter = vtkvmtk.vtkvmtkPolyDataHarmonicMappingFilter()
            harmonicMappingFilter.SetInputData(self.Surface)
            harmonicMappingFilter.SetHarmonicMappingArrayName('WarpVector'+str(k))
            harmonicMappingFilter.SetBoundaryPointIds(boundaryIds)
            harmonicMappingFilter.SetBoundaryValues(temperature)
            harmonicMappingFilter.SetAssemblyModeToFiniteElements()
            harmonicMappingFilter.Update()

            self.Surface = harmonicMappingFilter.GetOutput()

        warpVector = vtk.vtkDoubleArray()
        warpVector.SetNumberOfComponents(3)
        warpVector.SetNumberOfTuples(self.Surface.GetNumberOfPoints())
        warpVector.SetName('WarpVector')
        warpVectorX = self.Surface.GetPointData().GetArray('WarpVector0')
        warpVectorY = self.Surface.GetPointData().GetArray('WarpVector1')
        warpVectorZ = self.Surface.GetPointData().GetArray('WarpVector2')
        for i in range(self.Surface.GetNumberOfPoints()):
            warpVector.SetComponent(i,0,warpVectorX.GetComponent(i,0))
            warpVector.SetComponent(i,1,warpVectorY.GetComponent(i,0))
            warpVector.SetComponent(i,2,warpVectorZ.GetComponent(i,0))

        self.Surface.GetPointData().AddArray(warpVector)

        warper = vtk.vtkWarpVector()
        warper.SetInputData(self.Surface)
        warper.SetInputArrayToProcess(0,0,0,0,'WarpVector')
        warper.SetScaleFactor(1.)
        warper.Update()

        self.Surface = warper.GetOutput()

    def ConnectivityTagger(self):

        self.CleanSurface()

        tags = set()
        for i in range(self.Surface.GetNumberOfCells()):
            tags.add(self.CellEntityIdsArray.GetComponent(i,0))
        tags = sorted(tags)
        if self.PrintTags:
            self.PrintLog('Initial tags: '+str(tags))

        surface = []
        mergeTags = vtk.vtkAppendPolyData()

        for k, item in enumerate(tags):

            th = vtk.vtkThreshold()
            th.SetInputData(self.Surface)
            th.SetInputArrayToProcess(0, 0, 0, 1, self.CellEntityIdsArrayName)
            th.ThresholdBetween(item-0.001,item+0.001)
            th.Update()
            gf = vtk.vtkGeometryFilter()
            gf.SetInputConnection(th.GetOutputPort())
            gf.Update()
            surface.append(gf.GetOutput())

            connectivityFilter = vtk.vtkConnectivityFilter()
            connectivityFilter.SetInputData(surface[k])
            connectivityFilter.SetExtractionModeToAllRegions()
            connectivityFilter.ColorRegionsOn()
            connectivityFilter.Update()
            surface[k] = connectivityFilter.GetOutput()
            cellEntityIdsArray = surface[k].GetCellData().GetArray(self.CellEntityIdsArrayName)

            regionIdArray = surface[k].GetCellData().GetArray('RegionId')

            for i in range(surface[k].GetNumberOfCells()):
                tag = cellEntityIdsArray.GetComponent(i,0) +regionIdArray.GetComponent(i,0)*self.ConnectivityOffset
                cellEntityIdsArray.SetComponent(i,0,tag)

            mergeTags.AddInputData(surface[k])

        mergeTags.Update()
        self.Surface = mergeTags.GetOutput()
        self.CellEntityIdsArray = self.Surface.GetCellData().GetArray(self.CellEntityIdsArrayName)

    def DrawingTagger(self):
        from vmtk import vmtkscripts

        drawer = vmtkscripts.vmtkSurfaceRegionDrawing()
        drawer.Surface = self.Surface
        drawer.InsideValue = self.InsideTag
        drawer.OutsideValue = self.OutsideTag
        drawer.OverwriteOutsideValue = self.OverwriteOutsideTag
        drawer.ArrayName = self.CellEntityIdsArrayName
        drawer.TagSmallestRegion = self.TagSmallestRegion
        drawer.CellData = 1
        drawer.ComputeDisance = 0
        drawer.Execute()
        self.Surface = drawer.Surface

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: no Surface.')

        self.CellEntityIdsArray = self.Surface.GetCellData().GetArray(self.CellEntityIdsArrayName)

        # initialize the CellEntityIdsArray with OutsideTag in some cases
        if self.CellEntityIdsArray == None or (self.OverwriteOutsideTag and self.Method != "connectivity"):
            self.CellEntityIdsArray = vtk.vtkIntArray()
            self.CellEntityIdsArray.SetName(self.CellEntityIdsArrayName)
            self.CellEntityIdsArray.SetNumberOfComponents(1)
            self.CellEntityIdsArray.SetNumberOfTuples(self.Surface.GetNumberOfCells())
            self.Surface.GetCellData().AddArray(self.CellEntityIdsArray)
            self.CellEntityIdsArray.FillComponent(0,self.OutsideTag)

        if self.Method in ['array','harmonic']: # to be extended also to other method ['cliparray','array','harmonic']:
            if self.Value == None and self.Range == None:
                self.PrintError("This method need the definition of a value or a range")
            elif self.Range == None:
                if self.InsideOut:
                    self.Range = [self.Value, math.inf]
                else:
                    self.Range = [-math.inf, self.Value]
                # print("range: ",self.Range)

        if self.Method == 'cliparray':
            self.ClipArrayTagger()
        elif self.Method == 'array':
            self.ArrayTagger()
        elif self.Method == 'harmonic':
            self.HarmonicTagger()
        elif self.Method == 'connectivity':
            self.ConnectivityTagger()
        elif self.Method == 'constant':
            self.CellEntityIdsArray.FillComponent(0,self.InsideTag)
        elif self.Method == 'drawing':
            self.DrawingTagger()
        else:
            self.PrintError("Method unknown (available: cliparray, array, connectivity, constant, drawing)")

        if self.CleanOutput:
            self.CleanSurface()

        if self.PrintTags:
            self.CellEntityIdsArray = self.Surface.GetCellData().GetArray(self.CellEntityIdsArrayName)
            self.Tags = set()
            for i in range(self.Surface.GetNumberOfCells()):
                self.Tags.add(self.CellEntityIdsArray.GetComponent(i,0))
            self.Tags = sorted(self.Tags)
            self.PrintLog('Tags of the output surface: '+str(self.Tags))

        # useless, already triangulated
        # if self.Triangulate:
        #     triangleFilter = vtk.vtkTriangleFilter()
        #     triangleFilter.SetInputData(self.Surface)
        #     triangleFilter.Update()
        #     self.Surface = triangleFilter.GetOutput()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
