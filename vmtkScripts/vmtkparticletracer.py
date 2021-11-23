#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkparticletracer.py,v $
## Language:  Python
## Date:      $Date: 2013/07/09 16:10:27 $
## Version:   $Revision: 1.6 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Simone Manini
##       Orobix Srl

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys
import os

from vmtk import vtkvmtk
from vmtk import pypes


class vmtkParticleTracer(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.Source = None
        self.Traces = None
        self.FirstTimeStep = None
        self.LastTimeStep = None
        self.IntervalTimeStep = None
        self.MinSpeed = 5.0
        self.Subdivide = 0
        self.SeedTime = 0.0
        self.MaximumPropagation = 1E10
        self.InitialIntegrationStep = 0.1
        self.MinimumIntegrationStep = 0.01
        self.MaximumIntegrationStep = 10.0
        self.MaximumNumberOfSteps = 1E10
        self.VelocityScale = 1.0
        self.IntegrationDirectionBoth = 1
        self.VectorComponents = 1
        self.Periodic = 1
        self.Vorticity = 1
        self.Component0Prefix = "u_"
        self.Component1Prefix = "v_"
        self.Component2Prefix = "w_"
        self.SetScriptName('vmtkparticletracer')
        self.SetScriptDoc('Generate streamlines')

        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['Source','s','vtkPolyData',1,'','source points', 'vmtksurfacereader'],
            ['SeedTime','seedtime','float',1,'(0.0,)'],
            ['MinSpeed','minspeed','float',1,'(0.0,)','lower speed threshold'],
            ['Subdivide','subdivide','bool',1,'','Subdivide input polydata'],
            ['MaximumPropagation','maximumpropagation','int',1,'(0,)'],
            ['InitialIntegrationStep','initialintegrationstep','float',1,'(0.0,)'],
            ['MinimumIntegrationStep','minimumintegrationstep','float',1,'(0.0,)'],
            ['MaximumIntegrationStep','maximumintegrationstep','float',1,'(0.0,)'],
            ['MaximumNumberOfSteps','maximumnumberofsteps','int',1,'(0,)'],
            ['VelocityScale','velocityscale','float',1,'(0.0,)'],
            ['IntegrationDirectionBoth','integrationdirectionboth','bool',1,''],
            ['VectorComponents','vectorcomponents','bool',1,''],
            ['Periodic','periodic','bool',1,''],
            ['Vorticity','vorticity','bool',1,''],
            ['Component0Prefix','component0prefix','str',1,''],
            ['Component1Prefix','component1prefix','str',1,''],
            ['Component2Prefix','component2prefix','str',1,''],
            ['FirstTimeStep','firststep','int',1,'(0.0,)','first timestep'],
            ['LastTimeStep','laststep','int',1,'(0.0,)','last timestep'],
            ['IntervalTimeStep','interval','int',1,'(0.0,)','interval time step'],
            ])
        self.SetOutputMembers([
            ['Traces','o','vtkPolyData',1,'','the output traces','vmtksurfacewriter']
            ])

    def Execute(self):

        if (self.Mesh == None):
            self.PrintError('Error: no Mesh.')

        if (self.Source == None):
            self.PrintError('Error: no Source surface.')

        if (self.FirstTimeStep == None or self.LastTimeStep == None or self.IntervalTimeStep == None):
            timesteps = self.Mesh.GetFieldData().GetArray("timesteps")
            if (timesteps == None):
                self.PrintError('Error: no Timesteps.')
            indexList = []
            i = 0
            while i < self.Mesh.GetFieldData().GetArray("timesteps").GetNumberOfTuples():
                indexList.append(int(timesteps.GetTuple(i)[0]))
                i+=1
            firstTimeStep = indexList[0]
        else:
            indexList = list(range(self.FirstTimeStep,self.LastTimeStep+1,self.IntervalTimeStep))
            firstTimeStep = self.FirstTimeStep

        indexColumn = vtk.vtkIntArray()
        indexColumn.SetName("index")
        timeColumn = vtk.vtkDoubleArray()
        timeColumn.SetName("time")

        time = 0
        timeStepsTable = vtk.vtkTable()
        timeStepsTable.AddColumn(indexColumn)
        timeStepsTable.AddColumn(timeColumn)

        for index in indexList:
            time+=(1./(len(indexList)-1))
            indexColumn.InsertNextValue(index)
            timeColumn.InsertNextValue(time)

        u = self.Source.GetPointData().GetArray("u")
        if u:
            v = self.Source.GetPointData().GetArray("v")
            w = self.Source.GetPointData().GetArray("w")
        else:
            u = self.Source.GetPointData().GetArray("u_"+str(firstTimeStep))
            v = self.Source.GetPointData().GetArray("v_"+str(firstTimeStep))
            w = self.Source.GetPointData().GetArray("w_"+str(firstTimeStep))

        speed = vtk.vtkFloatArray()
        speed.SetNumberOfComponents(u.GetNumberOfComponents())
        speed.SetNumberOfTuples(u.GetNumberOfTuples())
        speed.SetName('speed')
        i=0
        while i<u.GetNumberOfTuples():
            speed.InsertTuple1(i, vtk.vtkMath.Norm((u.GetTuple(i)[0],v.GetTuple(i)[0],w.GetTuple(i)[0])))
            i+=1

        self.Source.GetPointData().AddArray(speed)

        if self.Subdivide:
            sd = vtk.vtkLinearSubdivisionFilter()
            sd.SetInputData(self.Source)
            sd.SetNumberOfSubdivisions(1)
            sd.Update()
            self.Source = sd.GetOutput()

        self.Source.GetPointData().SetActiveScalars('speed')
        cp = vtk.vtkClipPolyData()
        cp.SetInputData(self.Source)
        cp.GenerateClipScalarsOff()
        cp.SetValue(self.MinSpeed)
        cp.Update()
        self.Source = cp.GetOutput()

        tracer = vtkvmtk.vtkvmtkStaticTemporalStreamTracer()
        tracer.SetInputData(self.Mesh)
        tracer.SetIntegratorTypeToRungeKutta45()
        tracer.SetTimeStepsTable(timeStepsTable)
        tracer.SetSeedTime(self.SeedTime)
        tracer.SetMaximumPropagation(self.MaximumPropagation)
        tracer.SetInitialIntegrationStep(self.InitialIntegrationStep)
        tracer.SetMinimumIntegrationStep(self.MinimumIntegrationStep)
        tracer.SetMaximumIntegrationStep(self.MaximumIntegrationStep)
        tracer.SetMaximumNumberOfSteps(self.MaximumNumberOfSteps)
        if self.Vorticity:
            tracer.SetComputeVorticity(1)
        if self.IntegrationDirectionBoth:
            tracer.SetIntegrationDirectionToBoth()
        tracer.SetSourceData(self.Source)
        tracer.SetVelocityScale(self.VelocityScale)
        if self.VectorComponents:
            tracer.UseVectorComponentsOn()
        tracer.SetComponent0Prefix(self.Component0Prefix)
        tracer.SetComponent1Prefix(self.Component1Prefix)
        tracer.SetComponent2Prefix(self.Component2Prefix)
        if self.Periodic:
            tracer.PeriodicOn()
        tracer.Update()

        self.Traces = tracer.GetOutput()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
