#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkcenterlinemerge.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.4 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.


import vtk
import sys

import pypes
import vtkvmtk

vmtkcenterlinemerge = 'vmtkCenterlineMerge'

class vmtkCenterlineMerge(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Centerlines = None

        self.RadiusArrayName = ''
        self.GroupIdsArrayName = ''
        self.BlankingArrayName = ''

        self.Length = 1.0;

        self.SetScriptName('vmtkcenterlinemerge')
        self.SetScriptDoc('merge centerline tracts belonging to the same groups')
        self.SetInputMembers([
            ['Centerlines','i','vtkPolyData',1,'','the input centerlines','vmtksurfacereader'],
      	    ['RadiusArrayName','radiusarray','str',1,'','name of the array where centerline radius is stored'],
      	    ['GroupIdsArrayName','groupidsarray','str',1,'','name of the array where centerline group ids are stored'],
      	    ['BlankingArrayName','blankingarray','str',1,'','name of the array where centerline blanking information about branches is stored'],
            ['Length','length','float',1,'','length of the resampling interval']
            ])
        self.SetOutputMembers([
            ['Centerlines','o','vtkPolyData',1,'','the output centerlines','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        mergeCenterlines = vtkvmtk.vtkvmtkMergeCenterlines()
        mergeCenterlines.SetInput(self.Centerlines)
        mergeCenterlines.SetRadiusArrayName(self.RadiusArrayName)
        mergeCenterlines.SetGroupIdsArrayName(self.GroupIdsArrayName)
        mergeCenterlines.SetBlankingArrayName(self.BlankingArrayName)
        mergeCenterlines.SetResamplingStepLength(self.Length)
        mergeCenterlines.Update()

        self.Centerlines = mergeCenterlines.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
