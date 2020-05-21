#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacedistance.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.6 $

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


class vmtkSurfaceFeatureEdges(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Surface = None

        self.BoundaryEdges = 1
        self.FeatureEdges = 1
        self.FeatureAngle = 30
        self.NonManifoldEdges = 0
        self.Coloring = 0

        self.SetScriptName('vmtksurfacefeatureedges')
        self.SetScriptDoc('extract feature edges from a surface')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['BoundaryEdges','boundaryedges','bool',1,'',''],
            ['FeatureEdges','featureedges','bool',1,'',''],
            ['FeatureAngle','featureangle','float',1,'(0,)',''],
            ['NonManifoldEdges','nonmanifoldedges','bool',1,'',''],
            ['Coloring','coloring','bool',1,'',''],
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output feature edges','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No Surface.')

        extractor = vtk.vtkFeatureEdges()
        
        extractor.SetInputData(self.Surface)

        extractor.SetBoundaryEdges(self.BoundaryEdges)

        extractor.SetFeatureEdges(self.FeatureEdges)
        extractor.SetFeatureAngle(self.FeatureAngle)

        extractor.SetNonManifoldEdges(self.NonManifoldEdges)

       	extractor.SetColoring(self.Coloring)

        extractor.CreateDefaultLocator()

        extractor.Update()

        self.Surface = extractor.GetOutput()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
