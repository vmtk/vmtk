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
        self.ThresholdOnFirst = 1
        self.ThresholdOnSecond = 1

        self.RemeshOnlyBuffer = 0
        self.Buffer = None
        self.BufferTags = [4, 5]

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
            ['Eps','threshold','float',1,'(0.0,)','the distance threshold from the intersection of the two input surfaces, where to cut them in order to create a regular connecting triangulation'],
            ['ThresholdOnFirst','thresholdonfirst','bool',1,'','toggle applying threshold on the first input surface'],
            ['ThresholdOnSecond','thresholdonsecond','bool',1,'','toggle applying threshold on the second input surface'],
            ['RemeshOnlyBuffer','remeshonlybuffer','bool',1,'','toggle remeshing only a buffer zone near the intersection of the two input surfaces'],
            ['Buffer','buffer','float',1,'(0.0,)','the buffer width, measured from the intersection lines of the two input surfaces (if None, it is set equal to 3*Eps)'],
            ['BufferTags','buffertags','int',2,'','entity ids of the two buffer zones on the two surfaces'],
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

        if not self.ThresholdOnFirst and not self.ThresholdOnSecond:
            self.PrintError('Error: at least one of ThresholdOnFirst/Second should be true.')

        if self.ThresholdOnFirst:
            [self.Surface,self.Rings] = clipWithArray(self.Surface,'Distance',self.Eps,False)

        if self.ThresholdOnSecond:
            [self.Surface2,self.Rings2] = clipWithArray(self.Surface2,'Distance',self.Eps,False)

        # 4. initialize CellEntityIdsArray
        def initEntityIdsArray(surface,entityId):
            if surface.GetCellData().GetArray(self.CellEntityIdsArrayName) == None:
                    array = vtk.vtkIntArray()
                    array.SetName(self.CellEntityIdsArrayName)
                    array.SetNumberOfComponents(1)
                    array.SetNumberOfTuples(surface.GetNumberOfCells())
                    array.FillComponent(0,entityId)
                    surface.GetCellData().AddArray(array)
            return

        initEntityIdsArray(self.Surface,self.ConnectingId-1)
        initEntityIdsArray(self.Surface2,self.ConnectingId+1)

        # 5. remeshing
        def tagBuffer(surface,arrayName,value,tag):
            st = vmtkcontribscripts.vmtkSurfaceTagger()
            st.Surface = surface
            st.Method = 'array'
            st.ArrayName = arrayName
            st.Value = value
            st.InsideTag = tag
            st.CellEntityIdsArrayName = self.CellEntityIdsArrayName
            st.PrintTags = 1
            st.Execute()
            tagsToExclude = st.Tags
            tagsToExclude.remove(tag)
            return list(map(int,tagsToExclude))

        tagsToExclude = []
        tagsToExclude2 = []
        allTagsToExclude = []
        if not self.SkipRemeshing and self.RemeshOnlyBuffer:
            if self.Buffer == None:
                self.Buffer = 3.0*self.Eps
            tagsToExclude = tagBuffer(self.Surface,'Distance',self.Buffer,self.BufferTags[0])
            tagsToExclude2 = tagBuffer(self.Surface2,'Distance',self.Buffer,self.BufferTags[1])
            allTagsToExclude = list(set().union(tagsToExclude,tagsToExclude2))
            self.PrintLog('\nRemeshOnlyBuffer - tags excluded from remeshing:')
            self.PrintLog('\tfirst input surface:  '+str(tagsToExclude))
            self.PrintLog('\tsecond input surface: '+str(tagsToExclude2))
            self.PrintLog('\toutput surface:       '+str(allTagsToExclude)+'\n')

        def surfaceRemeshing(surface,iters,excludeIds):
            sr = vmtkscripts.vmtkSurfaceRemeshing()
            sr.Surface = surface
            sr.TargetEdgeLength = self.EdgeLength
            sr.ElementSizeMode = 'edgelength'
            sr.CellEntityIdsArrayName = self.CellEntityIdsArrayName
            sr.NumberOfIterations = iters
            sr.ExcludeEntityIds = excludeIds
            sr.CleanOutput = 1
            sr.Execute()
            return sr.Surface

        if not self.SkipRemeshing:
            self.Surface = surfaceRemeshing(self.Surface,5,tagsToExclude)
            self.Surface2 = surfaceRemeshing(self.Surface2,5,tagsToExclude2)


        # 6. connecting
        conn = vmtkcontribscripts.vmtkSurfaceConnector()
        conn.Surface = self.Surface
        conn.Surface2 = self.Surface2
        conn.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        conn.IdValue = self.ConnectingId
        conn.Execute()

        self.Surface = conn.OutputSurface

        # 7. remeshing the connection
        if not self.SkipRemeshing:
            self.Surface = surfaceRemeshing(self.Surface,10,allTagsToExclude)



if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
