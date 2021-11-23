#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkbifurcationvectors.py,v $
## Language:  Python
## Date:      $Date: 2006/10/17 15:16:16 $
## Version:   $Revision: 1.1 $

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


class vmtkBifurcationVectors(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Centerlines = None
        self.ReferenceSystems = None
        self.BifurcationVectors = None

        self.RadiusArrayName = 'MaximumInscribedSphereRadius'
        self.GroupIdsArrayName = 'GroupIds'
        self.CenterlineIdsArrayName = 'CenterlineIds'
        self.TractIdsArrayName = 'TractIds'
        self.BlankingArrayName = 'Blanking'
        self.ReferenceSystemsNormalArrayName = 'Normal'
        self.ReferenceSystemsUpNormalArrayName = 'UpNormal'

        self.BifurcationVectorsArrayName = 'BifurcationVectors'
        self.InPlaneBifurcationVectorsArrayName = 'InPlaneBifurcationVectors'
        self.OutOfPlaneBifurcationVectorsArrayName = 'OutOfPlaneBifurcationVectors'
        self.InPlaneBifurcationVectorAnglesArrayName = 'InPlaneBifurcationVectorAngles'
        self.OutOfPlaneBifurcationVectorAnglesArrayName = 'OutOfPlaneBifurcationVectorAngles'

        self.BifurcationVectorsOrientationArrayName = 'BifurcationVectorsOrientation'
        self.BifurcationGroupIdsArrayName = 'BifurcationGroupIds'
        self.NormalizeBifurcationVectors = 0

        self.SetScriptName('vmtkbifurcationvectors')
        self.SetScriptDoc('calculate in plane and out of place binfurcation vectors for a centerline')
        self.SetInputMembers([
            ['Centerlines','i','vtkPolyData',1,'','the input split centerlines','vmtksurfacereader'],
            ['ReferenceSystems','referencesystems','vtkPolyData',1,'','reference systems relative to the split centerlines','vmtksurfacereader'],
            ['RadiusArrayName','radiusarray','str',1,'','name of the array where centerline radius values are stored'],
            ['GroupIdsArrayName','groupidsarray','str',1,'','name of the array where centerline group ids are stored'],
            ['CenterlineIdsArrayName','centerlineidsarray','str',1,'','name of the array where centerline ids are stored'],
            ['TractIdsArrayName','tractidsarray','str',1,'','name of the array where centerline tract ids are stored'],
            ['BlankingArrayName','blankingarray','str',1,'','name of the array where blanking information about branches is stored'],
            ['ReferenceSystemsNormalArrayName','normalarray','str',1,'','name of the array where reference system normal vectors are stored'],
            ['ReferenceSystemsUpNormalArrayName','upnormalarray','str',1,'','name of the array where reference system upnormal vectors are stored'],
            ['BifurcationVectorsArrayName','vectorsarray','str',1,''],
            ['InPlaneBifurcationVectorsArrayName','inplanevectorsarray','str',1,''],
            ['OutOfPlaneBifurcationVectorsArrayName','outofplanevectorsarray','str',1,''],
            ['InPlaneBifurcationVectorAnglesArrayName','inplaneanglesarray','str',1,''],
            ['OutOfPlaneBifurcationVectorAnglesArrayName','outofplaneanglesarray','str',1,''],
            ['BifurcationVectorsOrientationArrayName','orientationarray','str',1,''],
            ['BifurcationGroupIdsArrayName','bifurcationgroupidsarray','str',1,''],
            ['NormalizeBifurcationVectors','normalizevectors','bool',1,'']
            ])
        self.SetOutputMembers([
            ['BifurcationVectors','o','vtkPolyData',1,'','the output data','vmtksurfacewriter'],
            ['BifurcationVectorsArrayName','vectorsarray','str',1,''],
            ['InPlaneBifurcationVectorsArrayName','inplanevectorsarray','str',1,''],
            ['OutOfPlaneBifurcationVectorsArrayName','outofplanevectorsarray','str',1,''],
            ['InPlaneBifurcationVectorAnglesArrayName','inplaneanglesarray','str',1,''],
            ['OutOfPlaneBifurcationVectorAnglesArrayName','outofplaneanglesarray','str',1,''],
            ['BifurcationVectorsOrientationArrayName','orientationarray','str',1,''],
            ['BifurcationGroupIdsArrayName','bifurcationgroupidsarray','str',1,'']
            ])

    def Execute(self):

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        if self.ReferenceSystems == None:
            self.PrintError('Error: No input reference systems.')

        centerlineBifurcationVectors = vtkvmtk.vtkvmtkCenterlineBifurcationVectors()
        centerlineBifurcationVectors.SetInputData(self.Centerlines)
        centerlineBifurcationVectors.SetReferenceSystems(self.ReferenceSystems)
        centerlineBifurcationVectors.SetRadiusArrayName(self.RadiusArrayName)
        centerlineBifurcationVectors.SetGroupIdsArrayName(self.GroupIdsArrayName)
        centerlineBifurcationVectors.SetCenterlineIdsArrayName(self.CenterlineIdsArrayName)
        centerlineBifurcationVectors.SetTractIdsArrayName(self.TractIdsArrayName)
        centerlineBifurcationVectors.SetBlankingArrayName(self.BlankingArrayName)
        centerlineBifurcationVectors.SetReferenceSystemGroupIdsArrayName(self.GroupIdsArrayName)
        centerlineBifurcationVectors.SetReferenceSystemNormalArrayName(self.ReferenceSystemsNormalArrayName)
        centerlineBifurcationVectors.SetReferenceSystemUpNormalArrayName(self.ReferenceSystemsUpNormalArrayName)
        centerlineBifurcationVectors.SetBifurcationVectorsArrayName(self.BifurcationVectorsArrayName)
        centerlineBifurcationVectors.SetInPlaneBifurcationVectorsArrayName(self.InPlaneBifurcationVectorsArrayName)
        centerlineBifurcationVectors.SetOutOfPlaneBifurcationVectorsArrayName(self.OutOfPlaneBifurcationVectorsArrayName)
        centerlineBifurcationVectors.SetInPlaneBifurcationVectorAnglesArrayName(self.InPlaneBifurcationVectorAnglesArrayName)
        centerlineBifurcationVectors.SetOutOfPlaneBifurcationVectorAnglesArrayName(self.OutOfPlaneBifurcationVectorAnglesArrayName)
        centerlineBifurcationVectors.SetBifurcationVectorsOrientationArrayName(self.BifurcationVectorsOrientationArrayName)
        centerlineBifurcationVectors.SetBifurcationGroupIdsArrayName(self.BifurcationGroupIdsArrayName)
        centerlineBifurcationVectors.SetNormalizeBifurcationVectors(self.NormalizeBifurcationVectors)
        centerlineBifurcationVectors.Update()

        self.BifurcationVectors = centerlineBifurcationVectors.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
