#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshmergetimesteps.py,v $
## Language:  Python
## Date:      $Date: 2013/07/15 12:59:27 $
## Version:   $Revision: 1.6 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.


import vtk
import sys
import os
    
import vtkvmtk
import vmtkmeshreader
import vmtkmeshvectorfromcomponents
import pypes

vmtkmeshmergetimesteps = 'vmtkMeshMergeTimesteps'

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
        self.Pressure = 0
        self.SetScriptName('vmtkmeshmergetimesteps')
        self.SetScriptDoc('merge multiple mesh files with different timesteps into one')
        
        self.SetInputMembers([
            ['InputDirectoryName','directory','str',1,''],
            ['Pattern','pattern','str',1,''],
            ['FirstTimeStep','firststep','int',1,'(0,)'],
            ['LastTimeStep','laststep','int',1,'(0,)'],
            ['IntervalTimeStep','intervalstep','int',1,'(0,)'],
            ['VelocityVector','velocity','bool',1,'','velocity vector instead of components'],
            ['Pressure','pressure','bool',1,'','pressure array']
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
        
        for root, dirs, files in os.walk(self.InputDirectoryName):
            if root == self.InputDirectoryName:
                fileList = [x for x in files if not (x.startswith('.'))]
        
        timeIndexList = range(self.FirstTimeStep,self.LastTimeStep+1,self.IntervalTimeStep)
        reader = vmtkmeshreader.vmtkMeshReader()
        if self.VelocityVector:
            vectorFromComponents = vmtkmeshvectorfromcomponents.vmtkMeshVectorFromComponents()
        
        for step in timeIndexList:
            if (self.Pattern%step).replace(' ','0') in fileList:
                fileName = (self.Pattern%step).replace(' ','0')
                timeIndex = step
                reader.InputFileName = os.path.abspath(os.path.join(self.InputDirectoryName,fileName))
                reader.Execute()
                mesh = reader.Mesh
                
                if step == self.FirstTimeStep:
                    mesh.CopyStructure(mesh)
                    self.Mesh = mesh

                if not self.Pressure:
                    self.Mesh.GetPointData().RemoveArray("p")
                else:
                    p = self.Mesh.GetPointData().GetArray("p")
                    p.SetName("p_"+str(step))
                    self.Mesh.GetPointData().AddArray(p)
                
                u = mesh.GetPointData().GetArray("u")
                u.SetName("u_"+str(step))
                self.Mesh.GetPointData().AddArray(u)

                v = mesh.GetPointData().GetArray("v")
                v.SetName("v_"+str(step))
                self.Mesh.GetPointData().AddArray(v)
                
                w = mesh.GetPointData().GetArray("w")
                w.SetName("w_"+str(step))
                self.Mesh.GetPointData().AddArray(w)
                                
                if self.VelocityVector:
                    vectorFromComponents.Mesh = self.Mesh
                    vectorFromComponents.VectorArrayName = "Velocity_"+str(step)
                    vectorFromComponents.ComponentsArrayNames = [u.GetName(),v.GetName(),w.GetName()]
                    vectorFromComponents.RemoveComponentArrays = True
                    vectorFromComponents.Execute()
        
        field = vtk.vtkFieldData()
        field.AllocateArrays(1)
        timesteps = vtk.vtkIntArray()
        timesteps.SetNumberOfTuples(3)
        timesteps.SetNumberOfComponents(1)
        timesteps.InsertTuple1(0, self.FirstTimeStep)
        timesteps.InsertTuple1(1, self.LastTimeStep)
        timesteps.InsertTuple1(2, self.IntervalTimeStep)
        timesteps.SetName("timesteps")
        field.AddArray(timesteps)
        self.Mesh.SetFieldData(field)
                 
if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()