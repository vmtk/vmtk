#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceconnectivity.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.8 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.


import vtk
import sys

import pypes

vmtksurfaceconnectivity = 'vmtkSurfaceConnectivity'

class vmtkSurfaceConnectivity(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Surface = None

        self.GroupId = -1
        self.GroupIdsArrayName = ''

        self.CleanOutput = 0

        self.SetScriptName('vmtksurfaceconnectivity')
        self.SetScriptDoc('extract the largest connected region or the scalar-connected region from a surface')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['CleanOutput','cleanoutput','bool',1,'','clean the unused points in the output'],
            ['GroupIdsArrayName','groupidsarray','str',1,'','name of the array containing the connectivity scalar'],
            ['GroupId','groupid','int',1,'','value of the connectivity scalar']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        if (self.GroupId != -1) & (self.GroupIdsArrayName!=''):
            self.Surface.GetPointData().SetActiveScalars(self.GroupIdsArrayName)

        connectivityFilter = vtk.vtkPolyDataConnectivityFilter()
        connectivityFilter.SetInput(self.Surface)
        connectivityFilter.ColorRegionsOff()       
        connectivityFilter.SetExtractionModeToLargestRegion()
        if self.GroupId != -1:
            connectivityFilter.ScalarConnectivityOn()
            scalarRange = [self.GroupId,self .GroupId]
            connectivityFilter.SetScalarRange(scalarRange)
        connectivityFilter.Update()

        self.Surface = connectivityFilter.GetOutput()
	
      	if self.CleanOutput == 1:
      	    cleaner = vtk.vtkCleanPolyData()
      	    cleaner.SetInput(connectivityFilter.GetOutput())
      	    cleaner.Update()

            self.Surface = cleaner.GetOutput()

        if self.Surface.GetSource():
            self.Surface.GetSource().UnRegisterAllOutputs()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
