#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceviewer.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.10 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by 
##       Elena Faggiano (elena.faggiano@gmail.com)
##       Politecnico di Milano

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

import os
import errno

from vmtk import pypes


vmtksurfaceharmonicsections = 'vmtkSurfaceHarmonicSections'

class vmtkSurfaceHarmonicSections(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.NumberOfContourPoints = 30
        self.NumberOfSections = 6
        self.DirName = 'Sections'
        self.NumberOfSubdivisions = 0
        # Output objects
        self.SurfaceTracer = None
        self.SurfaceHarmonic = None
        self.SurfaceSectionsPoints = None
        self.ComputeSections = 1

        self.SetScriptName('vmtksurfaceharmonicsections')
        self.SetScriptDoc('')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['NumberOfContourPoints','points','int',1,'(0,)','number of points for each section'],
            ['NumberOfSections','sections','int',1,'(0,)','number of sections'],
            ['NumberOfSubdivisions','subdivisions','int',1,'(0,)','number of subdivisions, if zero no subdivisions occurs'],
            ['DirName','dirname','str',1],
            ['ComputeSections','computesections','bool',1,'','toggle computing harmonic sections (if false, only the harmonic solution on the surface is computed'],
            ['BoundaryConditions','bcs','float',-1,'','list of bcs, one for each boundary, ordered as boundary id (only if computesections is false)']
            ])
        self.SetOutputMembers([
            ['SurfaceHarmonic','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ['SurfaceTracer','o2','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ['SurfaceSectionsPoints','o3','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ])


    def WriteTXTFileForEachSection(self,path,polydata,i):
        #Scrivo in una cartella apposita le coordinate dei punti della sezione i-esima
        #Mi sposto nella cartella apposita
        os.chdir(path+self.DirName)
        #Creo il file con il nome giusto (a seconda dell'indice della sezione che sto considerando)
        with open('Section_'+str(i),'w') as fh:
            for i in range(polydata.GetNumberOfPoints()-1):
                pt = polydata.GetPoint(i)
                fh.write('{} {} {}\n'.format(pt[0],pt[1],pt[2]))

        fh.close()

        os.chdir(path)

        return


    def Execute(self): 
        from vmtk import vmtkscripts
        from vmtk import vtkvmtk

        if (self.Surface == None):
            self.PrintError('Error: no Surface.') 

        surface = vtk.vtkPolyData()
        surface = self.Surface

        print ("\nsurface subdivision")

        subdivision = vmtkscripts.vmtkSurfaceSubdivision()
        subdivision.Surface = surface
        subdivision.NumberOfSubdivisions = self.NumberOfSubdivisions
        print ("number of subdivisions: ", subdivision.NumberOfSubdivisions)
        subdivision.Execute()
        surface_out = vtk.vtkPolyData()
        surface_out = subdivision.Surface

        #Boundaries extraction
        boundaryExtractor = vtkvmtk.vtkvmtkPolyDataBoundaryExtractor()
        boundaryExtractor.SetInputData(surface_out)
        boundaryExtractor.Update()

        boundaries = vtk.vtkPolyData()
        boundaries = boundaryExtractor.GetOutput()

        boundaryIds = vtk.vtkIdList()
        SourceIdList = vtk.vtkIdList()
        TargetIdList = vtk.vtkIdList()

        temperature = vtk.vtkDoubleArray()
        temperature.SetNumberOfComponents(1)
        
        #create on a new surface the Array boundaries (= 1 on boundary nodes, =0 otherwise
        Boundary = vtk.vtkDoubleArray()
        Boundary.SetName("boundaries")
        Boundary.SetNumberOfComponents(1)
        Boundary.SetNumberOfTuples(surface_out.GetNumberOfPoints())
        surface_out.GetPointData().AddArray(Boundary)

        for i in range(surface_out.GetNumberOfPoints()):
            Boundary.SetTuple1(i,0.0)
        
        Lines = vtk.vtkCellArray()
        Points = vtk.vtkPoints()
        
        numOfCells = boundaries.GetNumberOfCells()
        print ("number of boundaries", numOfCells)

        if self.ComputeSections:
            if numOfCells != 2:
                self.PrintError("Error: this script works only with two boundaries")
            else:
                print ("number of points boundary 1 =", boundaries.GetCell(0).GetNumberOfPoints() )
                print (surface_out.GetPoint(int(boundaries.GetPointData().GetScalars().GetComponent(boundaries.GetCell(0).GetPointId(0),0))) )
                Lines.InsertNextCell(boundaries.GetCell(0).GetNumberOfPoints())
                for j in range(boundaries.GetCell(0).GetNumberOfPoints()):
                    idb1 = int(boundaries.GetPointData().GetScalars().GetComponent(boundaries.GetCell(0).GetPointId(j),0))
                    boundaryIds.InsertNextId(idb1)
                    Boundary.SetTuple1(idb1,5)
                    temperature.InsertNextTuple1(1.0)
                    Lines.InsertCellPoint(j)
                    Points.InsertNextPoint(surface_out.GetPoint(idb1))

                print ("number of points boundary 2 =", boundaries.GetCell(1).GetNumberOfPoints() )
                print (surface_out.GetPoint(int(boundaries.GetPointData().GetScalars().GetComponent(boundaries.GetCell(1).GetPointId(0),0))) )
                for k in range(boundaries.GetCell(1).GetNumberOfPoints()):
                    idb2 = int(boundaries.GetPointData().GetScalars().GetComponent(boundaries.GetCell(1).GetPointId(k),0))
                    boundaryIds.InsertNextId(idb2)
                    Boundary.SetTuple1(idb2,2)
                    temperature.InsertNextTuple1(0.0)
        else:

            bc = [0.0] * numOfCells;

            if len(self.BoundaryConditions) > numOfCells:
                self.PrintError("Error: too many boundary conditions given")

            for i, item in enumerate(self.BoundaryConditions):
                bc[i] = item

            for i in range(numOfCells):
                currentCell = boundaries.GetCell(i)
                for j in range(currentCell.GetNumberOfPoints()):
                    idb = int(boundaries.GetPointData().GetScalars().GetComponent(currentCell.GetPointId(j),0))
                    boundaryIds.InsertNextId(idb)
                    Boundary.SetTuple1(idb,1) # 1 on boundary, 0 otherwise
                    temperature.InsertNextTuple1(bc[i])


        print ("\nboundaries identified and temperature assigned\n")

        #perform harmonic mapping using temperature as boundary condition
        harmonicMappingFilter = vtkvmtk.vtkvmtkPolyDataHarmonicMappingFilter()
        harmonicMappingFilter.SetInputData(surface_out)
        harmonicMappingFilter.SetHarmonicMappingArrayName("HarmonicMappedTemperature")
        harmonicMappingFilter.SetBoundaryPointIds(boundaryIds)
        harmonicMappingFilter.SetBoundaryValues(temperature)
        harmonicMappingFilter.SetAssemblyModeToFiniteElements()
        harmonicMappingFilter.Update()

        self.SurfaceHarmonic = harmonicMappingFilter.GetOutput()

        print ("\ntemperature diffused\n")

        if self.ComputeSections:

            #Creo una cartella apposita per mettere i txt con tutti i punti per ogni sezione
            path = os.getcwd()
            path = path + '/'
            print ("path = ", path)

            dirname = self.DirName
            try:
                os.mkdir(dirname)
            except OSError as exc:
                if exc.errno != errno.EEXIST:
                    raise exc
                pass

            #Boundary 1 extraction
            Lines.InsertCellPoint(0)
            Polygon = vtk.vtkPolyData()
            Polygon.SetPoints(Points)
            Polygon.SetLines(Lines)

            stripper = vtk.vtkStripper()
            stripper.SetInputData(Polygon)
            stripper.Update()
            
            #Spline interpolation of the boundary 1 with a number of points specified by the user
            splineFilter = vtk.vtkSplineFilter()
            splineFilter.SetInputData(stripper.GetOutput())
            splineFilter.SetSubdivideToSpecified()
            splineFilter.SetNumberOfSubdivisions(self.NumberOfContourPoints)
            splineFilter.Update()

            #Spline location on the surface
            point_locator = vtk.vtkPointLocator()
            point_locator.SetDataSet(surface_out)
            point_locator.AutomaticOn()
            point_locator.BuildLocator()
            pointsp = [0.0,0.0,0.0]
            print ("number of spline points:", splineFilter.GetOutput().GetNumberOfPoints() )
            for j in range(splineFilter.GetOutput().GetNumberOfPoints()):
                splineFilter.GetOutput().GetPoint(j,pointsp)
                ptIdsp = point_locator.FindClosestPoint(pointsp)
                SourceIdList.InsertNextId(ptIdsp)


            #Perform steepest gradient descent from boundaries with temperature 1 downward to temperature zero, along diffused temperature values
            #The source are points sampled bby the spline
            LineTracer = vtkvmtk.vtkvmtkSteepestDescentLineTracer()
            LineTracer.SetInputData(self.SurfaceHarmonic)
            LineTracer.SetDescentArrayName("HarmonicMappedTemperature")
            LineTracer.SetDataArrayName("HarmonicMappedTemperature")
            LineTracer.SetDirectionToDownward()
            LineTracer.SetSeeds(SourceIdList)
            LineTracer.MergePathsOff()
            LineTracer.StopOnTargetsOff()
            #LineTracer.SetTargets(TargetIdList)
            LineTracer.Update()
            print ("\nsteepest gradient descent performed\n")


            tracer = vtk.vtkPolyData()
            tracer = LineTracer.GetOutput()

            tracearray=vtk.vtkIntArray()
            tracearray.SetName("traceid")
            tracearray.SetNumberOfComponents(1)
            tracearray.SetNumberOfTuples(tracer.GetNumberOfPoints())
            tracer.GetPointData().AddArray(tracearray)
            cellIds = vtk.vtkIdList()
            for j in range(tracer.GetNumberOfPoints()):
                tracer.GetPointCells(j,cellIds)
                tracearray.SetTuple1(j,int(cellIds.GetId(0)))

            tracer.GetPointData().SetActiveScalars("HarmonicMappedTemperature")

            print ("\ncontouring now\n")

            #extraction of first contour (near to 0 temperature)
            contours = vtk.vtkContourFilter()
            contours.SetInputData(tracer)
            contours.SetValue(0,0.0001)
            contours.Update()
            self.WriteTXTFileForEachSection(path,contours.GetOutput(),0)

            appendFilter = vtk.vtkAppendPolyData()
            appendFilter.AddInputData(contours.GetOutput())
            appendFilter.Update()

            nsections = self.NumberOfSections
            nsectionsm = nsections-1.
            pointindex = 0
            for i in range (1,nsections-1):
                contours = vtk.vtkContourFilter()
                contours.SetInputData(tracer)
                contours.SetValue(0,float(i)/float(nsectionsm))
                contours.Update()
                outcontours = vtk.vtkPolyData()
                outcontours = contours.GetOutput()
                self.WriteTXTFileForEachSection(path,outcontours,i)

                appendFilter.AddInputData(outcontours)
                appendFilter.Update()

            #extraction of last contour (near to 1 temperature)
            contours = vtk.vtkContourFilter()
            contours.SetInputData(tracer)
            contours.SetValue(0,1.0-0.0001)
            contours.Update()
            self.WriteTXTFileForEachSection(path,contours.GetOutput(),nsections-1)
            appendFilter.AddInputData(contours.GetOutput())
            appendFilter.Update()

            print ("\nSections generated")
            print ("total number of points", appendFilter.GetOutput().GetNumberOfCells() )
        
            self.SurfaceTracer = tracer
            self.SurfaceSectionsPoints = appendFilter.GetOutput()

if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
