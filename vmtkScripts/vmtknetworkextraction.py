#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtknetworkextraction.py,v $
## Language:  Python
## Date:      $Date: 2006/07/17 09:53:14 $
## Version:   $Revision: 1.8 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
from vmtk import vtkvmtk
import vtk
import sys

from vmtk import pypes


class vmtkNetworkExtraction(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Network = None
        self.GraphLayout = None

        self.RadiusArrayName = 'Radius'
        self.TopologyArrayName = 'Topology'
        self.MarksArrayName = 'Marks'

        self.AdvancementRatio = 1.05

        self.SetScriptName('vmtknetworkextraction')
        self.SetScriptDoc('extract a network of approximated centerlines from a surface, the surface must have at least an opening')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['AdvancementRatio','advancementratio','float',1,'(1.0,)','the ratio between the sphere step and the local maximum radius'],
            ['RadiusArrayName','radiusarray','str',1],
            ['TopologyArrayName','topologyarray','str',1],
            ['MarksArrayName','marksarray','str',1]
            ])
        self.SetOutputMembers([
            ['Network','o','vtkPolyData',1,'','the output network','vmtknetworkwriter'],
            ['GraphLayout','ograph','vtkPolyData',1,'','the output graph layout','vmtknetworkwriter']
            ])

    def PrintProgress(self,obj,event):
        self.OutputProgress(obj.GetProgress(),10)

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        networkExtraction = vtkvmtk.vtkvmtkPolyDataNetworkExtraction()
        networkExtraction.SetInputData(self.Surface)
        networkExtraction.SetAdvancementRatio(self.AdvancementRatio)
        networkExtraction.SetRadiusArrayName(self.RadiusArrayName)
        networkExtraction.SetTopologyArrayName(self.TopologyArrayName)
        networkExtraction.SetMarksArrayName(self.MarksArrayName)
        networkExtraction.AddObserver("ProgressEvent", self.PrintProgress)
        networkExtraction.Update()

        self.EndProgress()

        self.Network = networkExtraction.GetOutput()
        self.GraphLayout = networkExtraction.GetGraphLayout()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
