#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceappend.py,v $
## Language:  Python
## Date:      $Date: 2020/10/22 14:18:14 $
## Version:   $Revision: 1.4 $

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
import numpy as np

from vmtk import vtkvmtk
from vmtk import pypes


class vmtkEntityExtractor(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.Surface = None
        self.EntityIds = []
        self.Invert = 0
        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.CellEntityIdsArray = None
        self.ConvertToInt = 0
        self.OutputEntityIds = []

        self.SetScriptName('vmtkentityextractor')
        self.SetScriptDoc('extract a subset of the input mesh/surface by exploiting a set of entityids')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh', 'vmtkmeshreader'],
            ['Surface','isurface','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['EntityIds','ids','int',-1,'','the list of ids to be extracted (or deleted) from the input surface'],
            ['Invert','invert','bool',1,'','if true, the given list of ids identifies the subset of the input domain to be deleted'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'',''],
            ['ConvertToInt','converttoint','bool',1,'','toggle converting the input CellEntityIdsArray into a vtkIntArray (if it is not already of this type)'],
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter'],
            ['Surface','osurface','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ['OutputEntityIds','oids','int',-1,'','the list of ids of the output surface/mesh'],
            ])


    def Threshold(self,idValue):
        from vmtk import vmtkcontribscripts
        th = vmtkcontribscripts.vmtkThreshold()
        th.Surface = self.Surface
        th.Mesh = self.Mesh
        th.ArrayName = self.CellEntityIdsArrayName
        th.CellData = 1
        th.LowThreshold = idValue
        th.HighThreshold = idValue
        th.Execute()
        return th.Surface or th.Mesh


    def SurfaceAppend(self,surface1,surface2):
        from vmtk import vmtkscripts
        if surface1 == None:
            surf = surface2
        elif surface2 == None:
            surf = surface1
        else:
            a = vmtkscripts.vmtkSurfaceAppend()
            a.Surface = surface1
            a.Surface2 = surface2
            a.Execute()
            surf = a.Surface
            tr = vmtkscripts.vmtkSurfaceTriangle()
            tr.Surface = surf
            tr.Execute()
            surf = tr.Surface
        return surf

    def Execute(self):
        from vmtk import vmtkcontribscripts

        idList = vmtkcontribscripts.vmtkEntityList()
        idList.Surface = self.Surface
        idList.Mesh = self.Mesh
        idList.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        idList.ConvertToInt = self.ConvertToInt
        idList.Execute()

        self.Surface = idList.Surface
        self.Mesh = idList.Mesh

        self.CellEntityIdsArray = idList.CellEntityIdsArray

        if self.Invert:
            self.OutputEntityIds = sorted(set(idList.EntityIds).difference(self.EntityIds))
        else:
            self.OutputEntityIds = self.EntityIds

        if self.Surface: # input is a surface
            appender = vtk.vtkAppendPolyData()
        else: # input is a mesh
            appender = vtk.vtkAppendFilter()
            appender.MergePointsOn()
        for item in self.OutputEntityIds:
            appender.AddInputData(self.Threshold(item))
        appender.Update()
        
        if self.Surface:
            cleaner = vtk.vtkCleanPolyData()
            cleaner.SetInputConnection(appender.GetOutputPort())
            cleaner.Update()
            self.Surface = cleaner.GetOutput()
        else:
            self.Mesh = appender.GetOutput()



if __name__ == '__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()

