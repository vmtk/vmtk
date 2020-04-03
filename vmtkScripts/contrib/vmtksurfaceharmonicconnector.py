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
##       Marco Fedele (marco.fedele@polimi.it)
##       Politecnico di Milano

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys


from vmtk import pypes


vmtksurfaceharmoniconnector = 'vmtkSurfaceHarmonicConnector'

class vmtkSurfaceHarmonicConnector(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.ReferenceSurface = None

        self.Ids = set()
        self.DeformableIds = []

        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.CellEntityIdsArray = None

        self.Remeshing = True
        self.RemeshingEdgeLength = 1.0
        self.RemeshingIterations = 10

        self.SkipConnection = False

        self.CleanOutput = 1




        self.SetScriptName('vmtksurfaceharmonicconnector')
        self.SetScriptDoc('extend an input vector harmonically on a surface')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['ReferenceSurface','r','vtkPolyData',1,'','the reference surface with which you want to connect','vmtksurfacereader'],
            ['DeformableIds','deformableids','int',-1,'','input surface entity ids to deform onto the reference surface'],
            ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1, '','name of the array where entity ids have been stored'],
            ['Remeshing','remeshing','bool',1,'','toggle remeshing the deformed part of the final surface'],
            ['RemeshingEdgeLength','remeshingedgelength','float',1,'(0.0,)'],
            ['RemeshingIterations','remeshingiterations','int',1,'(0,)'],
            ['SkipConnection','skipconnection','bool',1,'','deform input surface onto the reference one without connecting to it'],
            ['CleanOutput','cleanoutput','bool',1,'','toggle cleaning the unused points']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def SurfaceThreshold(self,surface,low,high):
        th = vmtkcontribscripts.vmtkThreshold()
        th.Surface = surface
        th.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        th.LowThreshold = low
        th.HighThreshold = high
        th.Execute()
        surf = th.Surface
        return surf

    def SurfaceAppend(self,surface1,surface2):
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

    def SurfaceProjection(self,isurface,rsurface):
        proj = vmtkscripts.vmtkSurfaceProjection()
        proj.Surface = isurface
        proj.ReferenceSurface = rsurface
        proj.Execute()
        return proj.Surface


    def Execute(self):
        from vmtk import vtkvmtk
        from vmtk import vmtkscripts
        from vmtk import vmtkcontribscripts

        if self.Surface == None:
            self.PrintError('Error: no Surface.')

        if self.ReferenceSurface == None:
            self.PrintError('Error: no ReferenceSurface.')

        self.CellEntityIdsArray = self.Surface.GetCellData().GetArray(self.CellEntityIdsArrayName)

        for i in range(self.Surface.GetNumberOfCells()):
            self.Ids.add(self.CellEntityIdsArray.GetComponent(i,0))
        self.Ids = sorted(self.Ids)
        self.PrintLog('Tags of the input surface: '+str(self.Ids))

        for item in self.DeformableIds:
            if item not in self.Ids:
                self.PrintError('Error: entity id '+str(item)+ ' not defined on the input Surface.')






if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
