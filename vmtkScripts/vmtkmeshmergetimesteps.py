#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshmergetimesteps.py,v $
## Language:  Python
## Date:      $Date: 2013/07/15 12:59:27 $
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
from vmtk import vmtkmeshreader
from vmtk import vmtkmeshvectorfromcomponents
from vmtk import pypes


class vmtkMeshMergeTimesteps(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.InputDirectoryName = None
        self.Pattern = None
        self.FirstTimeStep = None
        self.LastTimeStep = None
        self.IntervalTimeStep = 1
        self.VelocityVector = 0
        self.VelocityVectorArrayName = None
        self.Pressure = 0
        self.PressureArrayName = 'p'
        self.Wsr = 0
        self.VelocityComponentsArrayNames = 'u v w'
        self.WsrComponentsArrayNames = 'taux tauy tauz'
        self.WsrVector = 0
        self.SetScriptName('vmtkmeshmergetimesteps')
        self.SetScriptDoc('merge multiple mesh files with different timesteps into one')

        self.SetInputMembers([
            ['InputDirectoryName','directory','str',1,''],
            ['Pattern','pattern','str',1,''],
            ['FirstTimeStep','firststep','int',1,'(0,)'],
            ['LastTimeStep','laststep','int',1,'(0,)'],
            ['IntervalTimeStep','intervalstep','int',1,'(0,)'],
            ['VelocityComponentsArrayNames','components','str',-1,'',''],
            ['VelocityVector','velocityvector','bool',1,'','velocity vector instead of components'],
            ['VelocityVectorArrayName','vector','str',1,'','velocity vector name'],
            ['Pressure','pressure','bool',1,'','pressure array'],
            ['PressureArrayName','pressurearrayname','str',1,'','name of the pressure array'],
            ['Wsr','wallshearrate','bool',1,'','wallshearrate array'],
            ['WsrComponentsArrayNames','components','str',-1,'',''],
            ['WsrVector','wsrvector','bool',1,'','wallshearrate vector instead of components'],
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']
            ])

    def Execute(self):

        if (self.InputDirectoryName == None):
            self.PrintError('Error: no directory.')

        if (self.Pattern == None):
            self.PrintError('Error: no pattern.')

        if (self.FirstTimeStep == None):
            self.PrintError('Error: no first timestep.')

        if (self.LastTimeStep == None):
            self.PrintError('Error: no last timestep.')

        if (self.VelocityComponentsArrayNames == None):
            self.PrintError('Error: no VelocityComponentsArrayNames.')

        for root, dirs, files in os.walk(self.InputDirectoryName):
            if root == self.InputDirectoryName:
                fileList = [x for x in files if not (x.startswith('.'))]

        timeIndexList = list(range(self.FirstTimeStep,self.LastTimeStep+1,self.IntervalTimeStep))
        reader = vmtkmeshreader.vmtkMeshReader()
        #if self.VelocityVector or self.WsrVector:
        if self.WsrVector:
            vectorFromComponents = vmtkmeshvectorfromcomponents.vmtkMeshVectorFromComponents()

        u_name = self.VelocityComponentsArrayNames.split(' ')[0]
        v_name = self.VelocityComponentsArrayNames.split(' ')[1]
        w_name = self.VelocityComponentsArrayNames.split(' ')[2]

        if self.Wsr:
            taux_name = self.WsrComponentsArrayNames.split(' ')[0]
            tauy_name = self.WsrComponentsArrayNames.split(' ')[1]
            tauz_name = self.WsrComponentsArrayNames.split(' ')[2]

        field = vtk.vtkFieldData()
        field.AllocateArrays(1)
        timesteps = vtk.vtkIntArray()
        timesteps.SetNumberOfComponents(1)
        timesteps.SetName("timesteps")
        i = 0
        for step in timeIndexList:
            if (self.Pattern%step).replace(' ','0') in fileList:
                timesteps.InsertTuple1(i, step)
                i+=1

                fileName = (self.Pattern%step).replace(' ','0')
                timeIndex = step
                reader.InputFileName = os.path.abspath(os.path.join(self.InputDirectoryName,fileName))
                reader.Execute()
                mesh = reader.Mesh

                if step == self.FirstTimeStep:
                    mesh.CopyStructure(mesh)
                    self.Mesh = mesh

                if self.Pressure:
                    p = mesh.GetPointData().GetArray(self.PressureArrayName)
                    p.SetName(self.PressureArrayName+str(step))
                    self.Mesh.GetPointData().AddArray(p)

                if self.Wsr:
                    taux = mesh.GetPointData().GetArray(taux_name)
                    taux.SetName(taux_name+str(step))
                    self.Mesh.GetPointData().AddArray(taux)

                    tauy = mesh.GetPointData().GetArray(tauy_name)
                    tauy.SetName(tauy_name+str(step))
                    self.Mesh.GetPointData().AddArray(tauy)

                    tauz = mesh.GetPointData().GetArray(tauz_name)
                    tauz.SetName(tauz_name+str(step))
                    self.Mesh.GetPointData().AddArray(tauz)

                if self.VelocityVector:

                  j = 0

                  u_component = vtk.vtkDoubleArray()
                  u_component.SetNumberOfComponents(1)

                  v_component = vtk.vtkDoubleArray()
                  v_component.SetNumberOfComponents(1)

                  w_component = vtk.vtkDoubleArray()
                  w_component.SetNumberOfComponents(1)

                  while j < mesh.GetPointData().GetArray(self.VelocityVectorArrayName).GetNumberOfTuples():

                    u_val = mesh.GetPointData().GetArray(self.VelocityVectorArrayName).GetComponent(j,0)
                    v_val = mesh.GetPointData().GetArray(self.VelocityVectorArrayName).GetComponent(j,1)
                    w_val = mesh.GetPointData().GetArray(self.VelocityVectorArrayName).GetComponent(j,2)

                    u_component.InsertTuple1(j, u_val)
                    v_component.InsertTuple1(j, v_val)
                    w_component.InsertTuple1(j, w_val)

                    j+=1

                  u_component.SetName(u_name+"_"+str(step))
                  self.Mesh.GetPointData().AddArray(u_component)

                  v_component.SetName(v_name+"_"+str(step))
                  self.Mesh.GetPointData().AddArray(v_component)

                  w_component.SetName(w_name+"_"+str(step))
                  self.Mesh.GetPointData().AddArray(w_component)

                else:

                  u = mesh.GetPointData().GetArray(u_name)
                  u.SetName(u_name+"_"+str(step))
                  self.Mesh.GetPointData().AddArray(u)

                  v = mesh.GetPointData().GetArray(v_name)
                  v.SetName(v_name+"_"+str(step))
                  self.Mesh.GetPointData().AddArray(v)

                  w = mesh.GetPointData().GetArray(w_name)
                  w.SetName(w_name+"_"+str(step))
                  self.Mesh.GetPointData().AddArray(w)

                if self.WsrVector:
                    vectorFromComponents.Mesh = self.Mesh
                    vectorFromComponents.VectorArrayName = "Wsr_"+str(step)
                    vectorFromComponents.ComponentsArrayNames = [taux.GetName(),tauy.GetName(),tauz.GetName()]
                    vectorFromComponents.RemoveComponentArrays = True
                    vectorFromComponents.Execute()

        field.AddArray(timesteps)
        self.Mesh.SetFieldData(field)


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
