#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkdistancetocenterlines.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:48:31 $
## Version:   $Revision: 1.5 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.


import vtk
import vtkvmtk
import sys

import pypes

vmtkdistancetocenterlines = 'vmtkDistanceToCenterlines'

class vmtkDistanceToCenterlines(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Surface = None
        self.Centerlines = None
        self.UseRadiusInformation = 0
        self.DistanceToCenterlinesArrayName = 'DistanceToCenterlines'
        self.CenterlineRadiusArrayName = ''
        
        self.SetScriptName('vmtkdistancetocenterlines')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','vmtksurfacereader'],
            ['Centerlines','centerlines','vtkPolyData',1,'','vmtksurfacereader'],
            ['UseRadiusInformation','useradius','int',1],
            ['DistanceToCenterlinesArrayName','distancetocenterlinesarray','str',1],
            ['CenterlineRadiusArrayName','radiusarray','str',1]
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        distanceToCenterlinesFilter = vtkvmtk.vtkvmtkPolyDataDistanceToCenterlines()
        distanceToCenterlinesFilter.SetInput(self.Surface)
        distanceToCenterlinesFilter.SetCenterlines(self.Centerlines)
        distanceToCenterlinesFilter.SetUseRadiusInformation(self.UseRadiusInformation)
        distanceToCenterlinesFilter.SetDistanceToCenterlinesArrayName(self.DistanceToCenterlinesArrayName)
        distanceToCenterlinesFilter.SetCenterlineRadiusArrayName(self.CenterlineRadiusArrayName)
        distanceToCenterlinesFilter.Update()

        self.Surface = distanceToCenterlinesFilter.GetOutput()

        if self.Surface.GetSource():
            self.Surface.GetSource().UnRegisterAllOutputs()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
