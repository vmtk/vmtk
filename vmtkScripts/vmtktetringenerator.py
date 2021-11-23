#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtktetringenerator.py,v $
## Language:  Python
## Date:      $Date: 2006/07/27 08:27:40 $
## Version:   $Revision: 1.14 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import sys
import math
import vtk
from vmtk import vtkvmtk

from vmtk import pypes


class SectionProperties:

    def __init__(self):

        self.Radius = 0.0
        self.Normal = [1.0, 0.0, 0.0]
        self.Origin = [0.0, 0.0, 0.0]

        self.NormalizationTransform = None

        self.FlipOutwardNormal = 0

        self.Mesh = None
        self.SectionBoundaryPointIds = None

    def ComputeSectionOrigin(self):
        self.Origin = [0.0, 0.0, 0.0]
        numberOfIds = self.SectionBoundaryPointIds.GetNumberOfIds()
        weightSum = 0.0
        for i in range(numberOfIds):
            point = [0.0,0.0,0.0]
            self.NormalizationTransform.TransformPoint(self.Mesh.GetPoint(self.SectionBoundaryPointIds.GetId(i)),point)
            weight = pow(vtk.vtkMath.Distance2BetweenPoints(self.Mesh.GetPoint(self.SectionBoundaryPointIds.GetId(i)),self.Mesh.GetPoint(self.SectionBoundaryPointIds.GetId((i+1)%numberOfIds))),0.5) + pow(vtk.vtkMath.Distance2BetweenPoints(self.Mesh.GetPoint(self.SectionBoundaryPointIds.GetId(i)),self.Mesh.GetPoint(self.SectionBoundaryPointIds.GetId((i+numberOfIds-1)%numberOfIds))),0.5)
            weightSum += weight
            for j in range(3):
                self.Origin[j] += weight * point[j]
        for j in range(3):
            self.Origin[j] /= weightSum

    def ComputeSectionRadius(self):
        self.Radius = 0.0
        numberOfIds = self.SectionBoundaryPointIds.GetNumberOfIds()
        weightSum = 0.0
        for i in range(numberOfIds):
            point = [0.0,0.0,0.0]
            self.NormalizationTransform.TransformPoint(self.Mesh.GetPoint(self.SectionBoundaryPointIds.GetId(i)),point)
            weight = pow(vtk.vtkMath.Distance2BetweenPoints(self.Mesh.GetPoint(self.SectionBoundaryPointIds.GetId(i)),self.Mesh.GetPoint(self.SectionBoundaryPointIds.GetId((i+1)%numberOfIds))),0.5) + pow(vtk.vtkMath.Distance2BetweenPoints(self.Mesh.GetPoint(self.SectionBoundaryPointIds.GetId(i)),self.Mesh.GetPoint(self.SectionBoundaryPointIds.GetId((i+numberOfIds-1)%numberOfIds))),0.5)
            weightSum += weight
            self.Radius += weight * pow(vtk.vtkMath.Distance2BetweenPoints(self.Origin,point),0.5)
        self.Radius /= weightSum

    def ComputeSectionNormal(self):
        self.Normal = [0.0, 0.0, 0.0]
        numberOfIds = self.SectionBoundaryPointIds.GetNumberOfIds()
        point1 = [0.0,0.0,0.0]
        self.NormalizationTransform.TransformPoint(self.Mesh.GetPoint(self.SectionBoundaryPointIds.GetId(0)),point1)
        vector1 = [point1[0] - self.Origin[0], point1[1] - self.Origin[1], point1[2] - self.Origin[2]]
        vtk.vtkMath.Normalize(vector1)
        vector2 = None
        minAbsDot = 1.0
        for i in range(1,numberOfIds):
            currentPoint = [0.0,0.0,0.0]
            self.NormalizationTransform.TransformPoint(self.Mesh.GetPoint(self.SectionBoundaryPointIds.GetId(i)),currentPoint)

            currentVector = [currentPoint[0] - self.Origin[0], currentPoint[1] - self.Origin[1], currentPoint[2] - self.Origin[2]]
            vtk.vtkMath.Normalize(currentVector)
            absDot = abs(vtk.vtkMath.Dot(vector1,currentVector))
            if (absDot<minAbsDot):
                minAbsDot = absDot
                vector2 = currentVector
        self.Normal = [0.0,0.0,0.0]
        vtk.vtkMath.Cross(vector1,vector2,self.Normal)
        vtk.vtkMath.Normalize(self.Normal)

    def FlipNormal(self):
        for j in range(3):
            if (self.Normal[j] != 0.0):
                self.Normal[j] *= -1.0

    def OrientNormalOutward(self):
        id = self.SectionBoundaryPointIds.GetId(0)
        point = [0.0,0.0,0.0]
        self.NormalizationTransform.TransformPoint(self.Mesh.GetPoint(id),point)
        cellIds = vtk.vtkIdList()
        self.Mesh.GetPointCells(id,cellIds)
        cell = self.Mesh.GetCell(cellIds.GetId(0))
        maxAbsDot = 0.0
        farVector = None
        for i in range(cell.GetNumberOfPoints()):
            currentPoint = [0.0,0.0,0.0]
            self.NormalizationTransform.TransformPoint(cell.GetPoints().GetPoint(i),currentPoint)

            currentVector = [point[0] - currentPoint[0], point[1] - currentPoint[1], point[2] - currentPoint[2]]
            absDot = abs(vtk.vtkMath.Dot(self.Normal,currentVector))
            if (absDot>maxAbsDot):
                maxAbsDot = absDot
                farVector = currentVector
        dot = vtk.vtkMath.Dot(self.Normal,farVector)
        if (dot < 0.0):
            self.FlipNormal()

    def Execute(self):

        if self.Mesh == None:
            return

        if self.SectionBoundaryPointIds == None:
            return

        self.ComputeSectionOrigin()
        self.ComputeSectionRadius()
        self.ComputeSectionNormal()
        self.OrientNormalOutward()

        if (self.FlipOutwardNormal == 1):
            self.FlipNormal()


class vmtkTetrInGenerator(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.OutputFileName = ''
        self.Mesh = None
        self.NormalizationTransform = None

        self.NormalizationRadius = 1.0

        self.NormalizationEntity = ''
        self.InletEntities = []
        self.ReverseInletEntities = []
        self.OutletEntity = ''
        self.WallEntity = ''
        self.HistoryEntity = ''

        self.UseCellDefinedEntities = 1
        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.NormalizationEntityId = -1
        self.InletEntityIds = []
        self.OutletEntityId = -1
        self.WallEntityId = -1
        self.HistoryEntityId = -1

        self.ReverseInlets = []

        self.TimeStepsOnly = 0

        self.StartTime = 0.0
        self.EndTime = 1.0
        self.NumberOfTimeSteps = 0
        self.NumberOfDumps = 0
        self.DumpFlag = '11000'

        self.WriteWNodeSection = 1
        self.WriteWElemSection = 1

##             ['NormalizationEntity','normalizationentity','str',1],
##             ['InletEntities','inletentities','str',-1],
##             ['OutletEntity','outletentity','str',1],
##             ['WallEntity','wallentity','str',1],
##             ['HistoryEntity','historyentity','str',1],
##             ['UseCellDefinedEntities','celldefinedentities','int',1],

        self.SetScriptName('vmtktetringenerator')
        self.SetScriptDoc('generate input files for the newtetr CFD solver')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['OutputFileName','ofile','str',1,'','output file name'],
            ['TimeStepsOnly','timestepsonly','bool',1,'','only generate the $time section of the tetr.in file'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'','name of the array where entity ids relative to cells are stored'],
            ['NormalizationEntityId','normalizationid','int',1,'','id of the entity relative to which the mesh has to be normalized'],
            ['NormalizationRadius','normalizationradius','float',1,'(0.0,)','explicit value of the radius relative to which the mesh has to be normalized (to be used when -normalizationid is not used)'],
            ['InletEntityIds','inletids','int',-1,'','id of inlet entities'],
            ['OutletEntityId','outletid','int',1,'','id of the outlet entity'],
            ['WallEntityId','wallid','int',1,'','id of the wall entity'],
            ['HistoryEntityId','historyid','int',1,'','id of the entity whose history has to be stored'],
            ['ReverseInlets','reverseinlets','bool',-1,'','toggle reversal of the nth inlet entity normal'],
            ['StartTime','starttime','float',1,'','simulation normalized start time'],
            ['EndTime','endtime','float',1,'','simulation normalized end time'],
            ['NumberOfTimeSteps','timesteps','int',1,'(0,)','number of time steps between start and end time'],
            ['NumberOfDumps','dumps','int',1,'(0,)','number of solution dumps between start and end time'],
            ['DumpFlag','dumpflag','str',1,'','flag identifying solution dump (e.g. 11000 = tetr.pres and tetr.vel)'],
            ['WriteWNodeSection','wnodesection','bool',1,'','append $wnode section to .in file for subsequent wall shear stress computation'],
            ['WriteWElemSection','welemsection','bool',1,'','append $welem section to .in file for subsequent wall shear stress computation']
            ])
        self.SetOutputMembers([])

    def ComputeBarycenter(self,pointIds):
        barycenter = [0.0, 0.0, 0.0]
        numberOfIds = pointIds.GetNumberOfIds()
        for i in range(numberOfIds):
            point = self.Mesh.GetPoint(pointIds.GetId(i))
            for j in range(3):
                barycenter[j] += point[j]
        for j in range(3):
            barycenter[j] /= numberOfIds
        return barycenter

    def ComputeAngle(self,origin,point0,point1):
        vector0 = [
            point0[0] - origin[0],
            point0[1] - origin[1],
            point0[2] - origin[2]]
        vector1 = [
            point1[0] - origin[0],
            point1[1] - origin[1],
            point1[2] - origin[2]]
        vtk.vtkMath.Normalize(vector0)
        vtk.vtkMath.Normalize(vector1)
        sum = [
            vector0[0] + vector1[0],
            vector0[1] + vector1[1],
            vector0[2] + vector1[2]]
        difference = [
            vector0[0] - vector1[0],
            vector0[1] - vector1[1],
            vector0[2] - vector1[2]]
        sumNorm = vtk.vtkMath.Norm(sum)
        differenceNorm = vtk.vtkMath.Norm(difference)
        angle = 2.0 * math.atan2(differenceNorm,sumNorm) + math.pi
        return angle

    def SortBoundaryPoints(self,sectionBoundaryPointIds):
        barycenter = self.ComputeBarycenter(sectionBoundaryPointIds)
        numberOfIds = sectionBoundaryPointIds.GetNumberOfIds()
        for i in range(0,numberOfIds-1):
            minAngleId = -1
            minAngle = 2.0 * math.pi
            point0 = self.Mesh.GetPoint(sectionBoundaryPointIds.GetId(i))
            for j in range(i+1,numberOfIds):
                point1 = self.Mesh.GetPoint(sectionBoundaryPointIds.GetId(j))
                angle = self.ComputeAngle(barycenter,point0,point1)
                if angle < minAngle:
                    minAngleId = j
                    minAngle = angle
            tempId = sectionBoundaryPointIds.GetId(i+1)
            sectionBoundaryPointIds.SetId(i+1,sectionBoundaryPointIds.GetId(minAngleId))
            sectionBoundaryPointIds.SetId(minAngleId,tempId)

    def GetSectionBoundaryPointIds(self,sectionEntity):

        if (self.WallEntity == ''):
            self.PrintLog('Cannot compute section properties if WallEntity is undefined.')
            return None

        sectionEntityArray = self.Mesh.GetPointData().GetArray(sectionEntity)
        wallEntityArray = self.Mesh.GetPointData().GetArray(self.WallEntity)

        sectionBoundaryPointIds = vtk.vtkIdList()

        for i in range(self.Mesh.GetNumberOfPoints()):
            if (sectionEntityArray.GetComponent(i,0) != 0) & (wallEntityArray.GetComponent(i,0) != 0):
                sectionBoundaryPointIds.InsertNextId(i)

        self.SortBoundaryPoints(sectionBoundaryPointIds)

        return sectionBoundaryPointIds

    def GenerateTetrInFile(self):

        self.PrintLog('Generating Tetr .in file.')

        f=open(self.OutputFileName, 'w')

        line = '$title' + '\n'
        f.write(line)
        line = self.OutputFileName + '\n'
        f.write(line)
        line = '\n'
        f.write(line)

        line = '$compress' + '\n'
        f.write(line)
        line = 'gzip -f' + '\n'
        f.write(line)
        line = '\n'
        f.write(line)

        self.NormalizationTransform.Identity()

        if (self.NormalizationEntity != ''):
            sectionBoundaryPointIds = self.GetSectionBoundaryPointIds(self.NormalizationEntity)
            sectionProperties = SectionProperties()
            sectionProperties.Mesh = self.Mesh
            sectionProperties.NormalizationTransform = self.NormalizationTransform
            sectionProperties.SectionBoundaryPointIds = sectionBoundaryPointIds
            sectionProperties.Execute()
            self.NormalizationRadius = sectionProperties.Radius

        if (self.NormalizationRadius != 0.0) & (self.NormalizationRadius != 1.0):
            self.NormalizationTransform.Scale(1.0/self.NormalizationRadius,1.0/self.NormalizationRadius,1.0/self.NormalizationRadius)

        line = '$radius' + '\n'
        f.write(line)
        line = str(self.NormalizationRadius) + '\n'
        f.write(line)
        line = '\n'
        f.write(line)

        line = '$viscosity' + '\n'
        f.write(line)
        line = str(0.035) + '\n'
        f.write(line)
        line = '\n'
        f.write(line)

        line = '$density' + '\n'
        f.write(line)
        line = str(1.06) + '\n'
        f.write(line)
        line = '\n'
        f.write(line)

        line = '$red' + '\n'
        f.write(line)
        line = str(0) + '\n'
        f.write(line)
        line = '\n'
        f.write(line)

        line = '$alpha' + '\n'
        f.write(line)
        line = str(0) + '\n'
        f.write(line)
        line = '\n'
        f.write(line)

        line = '$nsolve' + '\n'
        f.write(line)
        line = '22' + '\n'
        f.write(line)
        line = '\n'
        f.write(line)

        line = '$Uzawa_PC' + '\n'
        f.write(line)
        line = '1' + '\n'
        f.write(line)
        line = '\n'
        f.write(line)

        line = '$node' + '\n'
        f.write(line)
        line = str(self.Mesh.GetNumberOfPoints()) + '\n'
        f.write(line)
        for i in range(self.Mesh.GetNumberOfPoints()):
            point = [0.0,0.0,0.0]
            self.NormalizationTransform.TransformPoint(self.Mesh.GetPoint(i),point)
            line = str(i+1) + ' ' + str(point[0]) + ' ' + str(point[1]) + ' ' + str(point[2]) + '\n'
            f.write(line)

        line = '\n'
        f.write(line)

        line = '$elem' + '\n'
        f.write(line)
        quadratidTetraCellType = 24
        quadraticTetraCellIds = vtk.vtkIdTypeArray()
        self.Mesh.GetIdsOfCellsOfType(quadratidTetraCellType,quadraticTetraCellIds)
        line = str(quadraticTetraCellIds.GetNumberOfTuples()) + '\n'
        f.write(line)
        for i in range(quadraticTetraCellIds.GetNumberOfTuples()):
            line = str(i+1) + ' '
            cell = self.Mesh.GetCell(quadraticTetraCellIds.GetValue(i))
            line += str(cell.GetPointId(0)+1) + ' '
            line += str(cell.GetPointId(4)+1) + ' '
            line += str(cell.GetPointId(1)+1) + ' '
            line += str(cell.GetPointId(5)+1) + ' '
            line += str(cell.GetPointId(2)+1) + ' '
            line += str(cell.GetPointId(6)+1) + ' '
            line += str(cell.GetPointId(7)+1) + ' '
            line += str(cell.GetPointId(8)+1) + ' '
            line += str(cell.GetPointId(9)+1) + ' '
            line += str(cell.GetPointId(3)+1) + ' '
            line += '\n'
            f.write(line)

#        inletEntities = self.InletEntities
#        reversedInlets = self.ReverseInlets

#        inletEntitiesReversed = zip(inletEntities,reversedInlets)

#        for inletEntityReversed in inletEntitiesReversed:
#            inletEntity = inletEntityReversed[0]
#            reversedInlet = inletEntityReversed[1]
#            if (inletEntity == ''):
#                continue
        for inletEntity in self.InletEntities:
            reversedInlet = inletEntity in self.ReverseInletEntities
            line = '\n'
            f.write(line)
            line = '$binlet' + ' (' + inletEntity + ') ' + '\n'
            f.write(line)
            entityArray = self.Mesh.GetPointData().GetArray(inletEntity)
            numberOfEntityNodes = 0
            for i in range(entityArray.GetNumberOfTuples()):
                if (entityArray.GetComponent(i,0) != 1.0):
                    continue
                numberOfEntityNodes += 1
            line = str(numberOfEntityNodes) + '\n'
            f.write(line)
            for i in range(entityArray.GetNumberOfTuples()):
                if (entityArray.GetComponent(i,0) != 1.0):
                    continue
                line = str(i+1) + '\n'
                f.write(line)

            sectionBoundaryPointIds = self.GetSectionBoundaryPointIds(inletEntity)
            sectionProperties = SectionProperties()
            sectionProperties.Mesh = self.Mesh
            sectionProperties.NormalizationTransform = self.NormalizationTransform
            sectionProperties.SectionBoundaryPointIds = sectionBoundaryPointIds
            if not reversedInlet:
                sectionProperties.FlipOutwardNormal = 1
            else:
                sectionProperties.FlipOutwardNormal = 0
            sectionProperties.Execute()
            line = str(sectionProperties.Radius) + '\n'
            f.write(line)
            line = str(sectionProperties.Origin[0]) + ' ' + str(sectionProperties.Origin[1]) + ' ' + str(sectionProperties.Origin[2]) + '\n'
            f.write(line)
            line = str(sectionProperties.Normal[0]) + ' ' + str(sectionProperties.Normal[1]) + ' ' + str(sectionProperties.Normal[2]) + '\n'
            f.write(line)
            #TODO: for every inlet insert fourier coefficients given points of waveform (spline-interpolate points beforehands to get them equispaced).

        if (self.OutletEntity != ''):
            line = '\n'
            f.write(line)
            line = '$boutlet' + ' (' + self.OutletEntity + ') ' + '\n'
            f.write(line)
            entityArray = self.Mesh.GetPointData().GetArray(self.OutletEntity)
            numberOfEntityNodes = 0
            for i in range(entityArray.GetNumberOfTuples()):
                if (entityArray.GetComponent(i,0) != 1.0):
                    continue
                numberOfEntityNodes += 1
            line = str(numberOfEntityNodes) + '\n'
            f.write(line)
            for i in range(entityArray.GetNumberOfTuples()):
                if (entityArray.GetComponent(i,0) != 1.0):
                    continue
                line = str(i+1) + '\n'
                f.write(line)
            sectionBoundaryPointIds = self.GetSectionBoundaryPointIds(self.OutletEntity)
            sectionProperties = SectionProperties()
            sectionProperties.Mesh = self.Mesh
            sectionProperties.NormalizationTransform = self.NormalizationTransform
            sectionProperties.SectionBoundaryPointIds = sectionBoundaryPointIds
            sectionProperties.FlipOutwardNormal = 0
            sectionProperties.Execute()
            line = str(sectionProperties.Radius) + '\n'
            f.write(line)
            line = str(sectionProperties.Origin[0]) + ' ' + str(sectionProperties.Origin[1]) + ' ' + str(sectionProperties.Origin[2]) + '\n'
            f.write(line)
            line = str(sectionProperties.Normal[0]) + ' ' + str(sectionProperties.Normal[1]) + ' ' + str(sectionProperties.Normal[2]) + '\n'
            f.write(line)

        if (self.WallEntity != ''):
            line = '\n'
            f.write(line)
            line = '$bwall' + '\n'
            f.write(line)
            entityArray = self.Mesh.GetPointData().GetArray(self.WallEntity)
            numberOfEntityNodes = 0
            for i in range(entityArray.GetNumberOfTuples()):
                if (entityArray.GetComponent(i,0) != 1.0):
                    continue
                numberOfEntityNodes += 1
            line = str(numberOfEntityNodes) + '\n'
            f.write(line)
            for i in range(entityArray.GetNumberOfTuples()):
                if (entityArray.GetComponent(i,0) != 1.0):
                    continue
                line = str(i+1) + '\n'
                f.write(line)

            wallPointIdsMap = vtk.vtkIdList()
            if (self.WriteWNodeSection == 1):
                line = '\n'
                f.write(line)
                line = '$wnode' + '\n'
                f.write(line)
                line = str(numberOfEntityNodes) + '\n'
                f.write(line)
                count = 0
                wallPointIdsMap.SetNumberOfIds(entityArray.GetNumberOfTuples())
                extractSurface = vtk.vtkGeometryFilter()
                extractSurface.SetInputData(self.Mesh)
                extractSurface.Update()
                normalsFilter = vtk.vtkPolyDataNormals()
                normalsFilter.SetInputConnection(extractSurface.GetOutputPort())
                normalsFilter.AutoOrientNormalsOn()
                normalsFilter.ConsistencyOn()
                normalsFilter.Update()
                normalsSurface = normalsFilter.GetOutput()
                locator = vtk.vtkMergePoints()
                locator.SetDataSet(normalsSurface)
                locator.BuildLocator()
                for i in range(entityArray.GetNumberOfTuples()):
                    if (entityArray.GetComponent(i,0) != 1.0):
                        wallPointIdsMap.SetId(i,-1)
                        continue
                    wallPointIdsMap.SetId(i,count)
                    point = self.Mesh.GetPoint(i)
                    surfacePointId = locator.FindClosestPoint(point)
                    normal = normalsSurface.GetPointData().GetNormals().GetTuple3(surfacePointId)
                    line = str(count+1) + ' ' + str(i+1) + ' ' + str(normal[0]) + ' ' + str(normal[1]) + ' ' + str(normal[2]) + '\n'
                    f.write(line)
                    count += 1

            if (self.WriteWElemSection == 1):
                line = '\n'
                f.write(line)
                line = '$welem' + '\n'
                f.write(line)
                dataArray = vtk.vtkIntArray()
                dataArray.SetNumberOfComponents(8)
                for i in range(self.Mesh.GetNumberOfCells()):
                    qtetra = self.Mesh.GetCell(i)
                    for j in range(qtetra.GetNumberOfFaces()):
                        face = qtetra.GetFace(j)
                        isBoundaryFace = 1
                        for k in range(face.GetNumberOfPoints()):
                            if (entityArray.GetComponent(face.GetPointId(k),0) != 1.0):
                                isBoundaryFace = 0
                                break
                        if (isBoundaryFace == 0):
                            continue
                        dataArray.InsertNextValue(i)
                        dataArray.InsertNextValue(j)
                        for k in range(face.GetNumberOfPoints()):
                            dataArray.InsertNextValue(face.GetPointId(k))
                line = str(dataArray.GetNumberOfTuples()) + '\n'
                f.write(line)
                for i in range(dataArray.GetNumberOfTuples()):
                    line = str(i+1) + ' '
                    line += str(int(dataArray.GetComponent(i,0))+1) + ' '
                    faceId = int(dataArray.GetComponent(i,1))
                    newTetrFaceIdsMap = [2, 4, 3, 1]
                    line += str(newTetrFaceIdsMap[faceId]) + ' '
                    for j in range(2,dataArray.GetNumberOfComponents()):
                        line += str(wallPointIdsMap.GetId(int(dataArray.GetComponent(i,j)))+1) + ' '
                    line += '\n'
                    f.write(line)

        if (self.HistoryEntity != ''):
            line = '\n'
            f.write(line)
            line = '$history' + '\n'
            f.write(line)
            entityArray = self.Mesh.GetPointData().GetArray(self.HistoryEntity)
            numberOfEntityNodes = 0
            for i in range(entityArray.GetNumberOfTuples()):
                if (entityArray.GetComponent(i,0) != 1.0):
                    continue
                numberOfEntityNodes += 1
            line = str(numberOfEntityNodes) + '\n'
            f.write(line)
            for i in range(entityArray.GetNumberOfTuples()):
                if (entityArray.GetComponent(i,0) != 1.0):
                    continue
                line = str(i+1) + '\n'
                f.write(line)

        self.WriteTimeSteps(f)

    def GenerateTimeStepsFile(self):

        self.PrintLog('Generating $time section.')
        f=open(self.OutputFileName, 'w')
        self.WriteTimeSteps(f)

    def WriteTimeSteps(self,f):

        if (self.NumberOfTimeSteps > 0):
            line = '\n'
            f.write(line)
            line = '$time' + '\n'
            f.write(line)
            line = str(self.NumberOfTimeSteps) + '\n'
            f.write(line)
            line = str(self.StartTime) + '\n'
            f.write(line)
            timeStep = (self.EndTime - self.StartTime) / float(self.NumberOfTimeSteps)
            for i in range(1,self.NumberOfTimeSteps+1):
                time = self.StartTime + float(i) * timeStep
                dumpFlag = '0000'
                if (i % (self.NumberOfTimeSteps / self.NumberOfDumps) == 0):
                    dumpFlag = self.DumpFlag
                nsub = 0
                line = str(time) + ' ' + dumpFlag + ' ' + str(nsub) + '\n'
                f.write(line)

    def BuildPointEntityArray(self,entityId,pointEntityArray):

        cellEntityArray = self.Mesh.GetCellData().GetArray(self.CellEntityIdsArrayName)
        pointEntityArray.SetNumberOfTuples(self.Mesh.GetNumberOfPoints())
        pointEntityArray.FillComponent(0,0.0)

        for i in range(self.Mesh.GetNumberOfCells()):
            cellEntityId = int(cellEntityArray.GetComponent(i,0))
            if cellEntityId != entityId:
                continue
            cell = self.Mesh.GetCell(i)
            for j in range(cell.GetNumberOfPoints()):
                pointEntityArray.SetComponent(cell.GetPointId(j),0,1)

    def Execute(self):

        if (self.OutputFileName == ''):
            self.PrintError('Error: no OutputFileName.')

        if self.TimeStepsOnly:
            self.GenerateTimeStepsFile()
            return

        if self.Mesh == None:
            self.PrintError('Error: no Mesh.')

        self.NormalizationTransform = vtk.vtkTransform()

        if self.UseCellDefinedEntities == 1:

            entityIds = []
            entityNames = []

            if self.NormalizationEntityId != -1:
                entityIds.append(self.NormalizationEntityId)
                entityNames.append('NormalizationEntity')
                self.NormalizationEntity = 'NormalizationEntity'
            for inletEntityId in self.InletEntityIds:
                entityIds.append(inletEntityId)
                entityNames.append('InletEntity'+str(inletEntityId))
                self.InletEntities.append('InletEntity'+str(inletEntityId))
            for reverseInletEntityId in self.ReverseInlets:
                self.ReverseInletEntities.append('InletEntity'+str(reverseInletEntityId))
            if self.OutletEntityId != -1:
                entityIds.append(self.OutletEntityId)
                entityNames.append('OutletEntity')
                self.OutletEntity = 'OutletEntity'
            if self.WallEntityId != -1:
                entityIds.append(self.WallEntityId)
                entityNames.append('WallEntity')
                self.WallEntity = 'WallEntity'
            if self.HistoryEntityId != -1:
                entityIds.append(self.HistoryEntityId)
                entityNames.append('HistoryEntity')
                self.HistoryEntity = 'HistoryEntity'

            for i in range(len(entityIds)):
                entityId = entityIds[i]
                entityName = entityNames[i]
                pointEntityArray = vtk.vtkIntArray()
                pointEntityArray.SetName(entityName)
                self.BuildPointEntityArray(entityId,pointEntityArray)
                self.Mesh.GetPointData().AddArray(pointEntityArray)

        self.GenerateTetrInFile()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
