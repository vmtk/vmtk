#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshconnectivity.py,v $
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

vmtkmeshconnectivity = 'vmtkMeshConnectivity'

class vmtkMeshConnectivity(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Mesh = None

        self.SetScriptName('vmtkmeshconnectivity')
        self.SetScriptDoc('extract the largest connected region or the scalar-connected region from a mesh')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']
            ])

    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: No input mesh.')

        connectivityFilter = vtk.vtkConnectivityFilter()
        connectivityFilter.SetInput(self.Mesh)
        connectivityFilter.ColorRegionsOff()       
        connectivityFilter.SetExtractionModeToLargestRegion()
        connectivityFilter.Update()

        self.Mesh = connectivityFilter.GetOutput()
	
        if self.Mesh.GetSource():
            self.Mesh.GetSource().UnRegisterAllOutputs()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()

