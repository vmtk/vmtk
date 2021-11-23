#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkbranchmetrics.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:48:31 $
## Version:   $Revision: 1.4 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
from vmtk import vtkvmtk
import sys

from vmtk import pypes


class vmtkBranchMetrics(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Centerlines = None

        self.ComputeAbscissaMetric = 1
        self.ComputeAngularMetric = 1

        self.AbscissasArrayName = 'Abscissas'
        self.NormalsArrayName = 'ParallelTransportNormals'
        self.GroupIdsArrayName = 'GroupIds'
        self.CenterlineIdsArrayName = 'CenterlineIds'
        self.TractIdsArrayName = 'TractIds'
        self.RadiusArrayName = 'MaximumInscribedSphereRadius'
        self.BlankingArrayName = 'Blanking'

        self.AngularMetricArrayName = 'AngularMetric'
        self.AbscissaMetricArrayName = 'AbscissaMetric'

        self.SetScriptName('vmtkbranchmetrics')
        self.SetScriptDoc('Takes a centerline and input surface, already split into branches (with centerline attributes calculated) \
                           and calculates the surface angular and abscissa metric.')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','','vmtksurfacereader'],
            ['Centerlines','centerlines','vtkPolyData',1,'','','vmtksurfacereader'],
            ['ComputeAbscissaMetric','abscissametric','bool',1],
            ['ComputeAngularMetric','angularmetric','bool',1],
            ['AbscissasArrayName','abscissasarray','str',1],
            ['NormalsArrayName','normalsarray','str',1],
            ['GroupIdsArrayName','groupidsarray','str',1],
            ['CenterlineIdsArrayName','centerlineidsarray','str',1],
            ['TractIdsArrayName','tractidsarray','str',1],
            ['RadiusArrayName','radiusarray','str',1],
            ['BlankingArrayName','blankingarray','str',1],
            ['AngularMetricArrayName','angularmetricarray','str',1],
            ['AbscissaMetricArrayName','abscissametricarray','str',1]
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','','vmtksurfacewriter'],
            ['AngularMetricArrayName','angularmetricarray','str',1],
            ['AbscissaMetricArrayName','abscissametricarray','str',1]
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        if self.ComputeAngularMetric == 1:
            self.PrintLog('Computing angular metric')
            angularMetricFilter = vtkvmtk.vtkvmtkPolyDataCenterlineAngularMetricFilter()
            angularMetricFilter.SetInputData(self.Surface)
            angularMetricFilter.SetMetricArrayName(self.AngularMetricArrayName)
            angularMetricFilter.SetGroupIdsArrayName(self.GroupIdsArrayName)
            angularMetricFilter.SetCenterlines(self.Centerlines)
            angularMetricFilter.SetRadiusArrayName(self.RadiusArrayName)
            angularMetricFilter.SetCenterlineNormalsArrayName(self.NormalsArrayName)
            angularMetricFilter.SetCenterlineGroupIdsArrayName(self.GroupIdsArrayName)
            angularMetricFilter.SetCenterlineTractIdsArrayName(self.TractIdsArrayName)
            angularMetricFilter.UseRadiusInformationOff()
            angularMetricFilter.IncludeBifurcationsOff()
            angularMetricFilter.SetBlankingArrayName(self.BlankingArrayName)
            angularMetricFilter.SetCenterlineIdsArrayName(self.CenterlineIdsArrayName)
            angularMetricFilter.Update()
            self.Surface = angularMetricFilter.GetOutput()

        if self.ComputeAbscissaMetric == 1:
            self.PrintLog('Computing abscissa metric')
            abscissaMetricFilter = vtkvmtk.vtkvmtkPolyDataCenterlineAbscissaMetricFilter()
            abscissaMetricFilter.SetInputData(self.Surface)
            abscissaMetricFilter.SetMetricArrayName(self.AbscissaMetricArrayName)
            abscissaMetricFilter.SetGroupIdsArrayName(self.GroupIdsArrayName)
            abscissaMetricFilter.SetCenterlines(self.Centerlines)
            abscissaMetricFilter.SetRadiusArrayName(self.RadiusArrayName)
            abscissaMetricFilter.SetAbscissasArrayName(self.AbscissasArrayName)
            abscissaMetricFilter.SetCenterlineGroupIdsArrayName(self.GroupIdsArrayName)
            abscissaMetricFilter.SetCenterlineTractIdsArrayName(self.TractIdsArrayName)
            abscissaMetricFilter.UseRadiusInformationOff()
            abscissaMetricFilter.IncludeBifurcationsOn()
            abscissaMetricFilter.SetBlankingArrayName(self.BlankingArrayName)
            abscissaMetricFilter.SetCenterlineIdsArrayName(self.CenterlineIdsArrayName)
            abscissaMetricFilter.Update()
            self.Surface = abscissaMetricFilter.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
