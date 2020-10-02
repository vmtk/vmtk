#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceappend.py,v $
## Language:  Python
## Date:      $Date: 2029/10/02 14:28:19 $
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

from vmtk import vmtkrenderer
from vmtk import vtkvmtk
from vmtk import pypes


class vmtkSurfaceBooleanConnector(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)


        self.Surface = None
        self.Surface2 = None

        self.Rings = None
        self.Rings2 = None

        self.Operation = 'union'


        self.Eps = 1.0
        self.SkipRemeshing = 0
        self.EdgeLength = 1.0

        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.ConnectingId = 2
        # self.OverwriteIds = 1
        self.CleanOutput = 1


        self.SetScriptName('vmtksurfacebooleanconnector')
        self.SetScriptDoc('perform boolean operations between surfaces automatically remeshing the connection region')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the first input surface','vmtksurfacereader'],
            ['Surface2','i2','vtkPolyData',1,'','the second input surface','vmtksurfacereader'],
            ['Operation','operation','str',1,'["union","intersection","difference"]','the boolean operation to be performed'],
            ['Eps','threshold','float',1,'(0.0,)','the distance threshold where to cut the two input surfaces to create a regular connecting triangulation'],
            ['SkipRemeshing','skipremeshing','bool',1,'','toggle skipping the remeshing (if true, the connection is performed between boundaries made of irregular triangles)'],
            ['EdgeLength','edgelength','float',1,'(0.0,)','the constant edgelength for the remeshing'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'',''],
            ['ConnectingId','connectingid','int',1,'','entity id value in the connecting surface'],
            #['OverwriteIds','overwriteids','bool',1,'','toggle overwriting the ids of the input surfaces; if true, (ConnectingId-1) and (ConnectingId+1) is assigned to Surface and Surface2, respectively'],
            ['CleanOutput','cleanoutput','bool',1,'','clean the unused points in the output and triangulate it'],
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ])



    def Execute(self):
        from vmtk import vmtkscripts
        from vmtk import vmtkcontribscripts

        if self.Surface == None or self.Surface == None:
            self.PrintError('Error: No input surfaces.')

        # 1. compute signed distances
        def implicitDistance(surface,rsurface):
            sd = vmtkscripts.vmtkSurfaceImplicitDistance()
            sd.Surface = surface
            sd.ReferenceSurface = rsurface
            sd.Execute()
            return sd.Surface

        self.Surface = implicitDistance(self.Surface,self.Surface2)
        self.Surface2 = implicitDistance(self.Surface2,self.Surface) 

        # 2. clip both surfaces at the zero level,
        #    keeping the positive or negative part depending on the
        #    boolean operation to be performed
        def clipWithArray(surface,array,value,insideout):
            sc = vmtkscripts.vmtkSurfaceClipper()
            sc.Interactive = False
            sc.CleanOutput = True
            sc.Surface = surface
            sc.ClipArrayName = array
            sc.ClipValue = value
            sc.InsideOut = insideout
            sc.Execute()
            return [sc.Surface,sc.CutLines]

        insideout = True if (self.Operation == 'intersection') else False
        [self.Surface,self.Rings] = clipWithArray(self.Surface,'ImplicitDistance',0.0,insideout)

        insideout = False if (self.Operation == 'union') else True
        [self.Surface2,self.Rings2] = clipWithArray(self.Surface2,'ImplicitDistance',0.0,insideout)

        # 3. compute distances from CutLines and clip at the threshold
        def unsignedDistance(surface,rings):
            d = vmtkscripts.vmtkSurfaceDistance()
            d.DistanceArrayName = 'Distance'
            d.Surface = surface
            d.ReferenceSurface = rings
            d.Execute()
            return d.Surface

        self.Surface = unsignedDistance(self.Surface,self.Rings)
        self.Surface2 = unsignedDistance(self.Surface2,self.Rings2)

        [self.Surface,self.Rings] = clipWithArray(self.Surface,'Distance',self.Eps,False)
        [self.Surface2,self.Rings2] = clipWithArray(self.Surface2,'Distance',self.Eps,False)

        # 4. remeshing


        # 5. connecting


        # 6. remeshing the connection
        conn = vmtkcontribscripts.vmtkSurfaceConnector()
        conn.Surface = self.Surface
        conn.Surface2 = self.Surface2
        conn.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        conn.IdValue = self.ConnectingId
        conn.Execute()

        self.Surface = conn.OutputSurface




if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
