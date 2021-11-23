## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceloopextraction.py,v $
## Language:  Python
## Date:      $Date: 2014/10/24 16:35:13 $
## Version:   $Revision: 1.10 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Elena Faggiano (elena.faggiano@gmail.com)
##       Politecnico di Milano

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys
from vmtk import vmtkrenderer
from vmtk import pypes


class vmtkSurfaceLoopExtraction(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Opacity = 1.0

        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.ContourWidget = None
        self.Interpolator = None

        #Output members
        self.Loop = None

        self.SetScriptName('vmtksurfaceloopextraction')
        self.SetScriptDoc('interactively define loops on a surface')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer'],
            ['Opacity','opacity','float',1,'(0.0,1.0)','object opacity in the scene'],
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ['Loop','oloop','vtkPolyData',1,'','the output loop','vmtksurfacewriter'],
            ])

    def AnswerValidator(self,text):
        if not text:
            return 0
        if not text.split():
            return 0
        if int(text) != 1 and int(text) != 2 :
            return 0
        return 1

    def AnswerRegionValidator(self,text):
        if not text:
            return 0
        if not text.split():
            return 0
        if text != "y" and text != "n":
            return 0
        return 1

    def LoopCallback(self, obj):
        rep = vtk.vtkOrientedGlyphContourRepresentation.SafeDownCast(self.ContourWidget.GetRepresentation())
        polycontour=vtk.vtkPolyData()
        polycontour=rep.GetContourRepresentationAsPolyData()
        self.Loop = polycontour
        self.InputInfo("loop generated. Press q to quit \n")

    def DeleteContourCallback(self, obj):
        self.ContourWidget.Initialize()

    def InteractCallback(self, obj):
        if self.ContourWidget.GetEnabled() == 1:
            self.ContourWidget.SetEnabled(0)
        else:
            self.ContourWidget.SetEnabled(1)

    def Display(self):
        self.vmtkRenderer.Render()

    def Execute(self):
        from vmtk import vmtkscripts
        if self.Surface == None:
            self.PrintError('Error: no Surface.')

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self)

        triangleFilter = vtk.vtkTriangleFilter()
        triangleFilter.SetInputData(self.Surface)
        triangleFilter.Update()

        self.Surface = triangleFilter.GetOutput()

        self.tagviewer = vmtkscripts.vmtkSurfaceViewer()
        self.tagviewer.Surface = self.Surface
        self.tagviewer.vmtkRenderer = self.vmtkRenderer
        self.tagviewer.Representation = 'edges'
        self.tagviewer.Opacity = self.Opacity
        self.tagviewer.Execute()

        self.ContourWidget = vtk.vtkContourWidget()
        self.ContourWidget.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)

        rep = vtk.vtkOrientedGlyphContourRepresentation.SafeDownCast(self.ContourWidget.GetRepresentation())
        rep.GetLinesProperty().SetColor(1, 0, 0)
        rep.GetLinesProperty().SetLineWidth(4.0)

        pointPlacer = vtk.vtkPolygonalSurfacePointPlacer()
        pointPlacer.AddProp(self.tagviewer.Actor)
        pointPlacer.GetPolys().AddItem(self.Surface)
        rep.SetPointPlacer(pointPlacer)

        self.Interpolator = vtk.vtkPolygonalSurfaceContourLineInterpolator()
        self.Interpolator.GetPolys().AddItem(self.Surface)
        rep.SetLineInterpolator(self.Interpolator)

        self.InputInfo("Building loop ...\n")

        self.vmtkRenderer.AddKeyBinding('space','Generate loop',self.LoopCallback)
        self.vmtkRenderer.AddKeyBinding('d','Delete contour',self.DeleteContourCallback)
        self.vmtkRenderer.AddKeyBinding('i','Start/stop contour drawing',self.InteractCallback)

        self.Display()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


if __name__ == '__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
