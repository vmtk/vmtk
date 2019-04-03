#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceclipper.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.9 $

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

from vmtk import vmtkrenderer
from vmtk import pypes

class vmtkSurfaceEdgeLengthArray(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.CellEntityIdsArray = None
        self.InputArrayName = 'Thickness'
        self.OutputArrayName = 'EdgeLength'
        self.InputArray = None
        self.OutputArray = None
        self.Method = 'default'
        self.MinSize = 0.0
        self.MaxSize = 1E16
        self.Alpha = 1.0
        self.Beta = 1.0
        self.Constant = 1.0
        self.Overwrite = 1
        self.Interactive = 0
        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.Representation = 'surface'

        self.TagsSet = set()


        self.SetScriptName('vmtksurfaceedgelengtharray')
        self.SetScriptDoc('given a scalar array f, compute the edge length array as max( minsize, min( alpha*f^beta, maxsize ) )')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['InputArrayName','inputarray','str',1,'','name of the input array f'],
            ['OutputArrayName','outputarray','str',1,'','name of the array where the edge length is stored'],
            ['Method','method','str',1,'','method: "default" defines the edge-length array as the aforementioned function of the input array; "constant" defines a constant edge length array (useful if combined with interactive)'],
            ['MinSize','minsize','float',1,'(0.0,)','minimum edge length'],
            ['MaxSize','maxsize','float',1,'(0.0,)','maximum edge length'],
            ['Alpha','alpha','float',1,'(0.0,)','multiplicative factor to the input array'],
            ['Beta','beta','float',1,'(0.0,)','exponent to the input array'],
            ['Constant','constant','float',1,'(0.0,)','constant value for the edge-length array ("constant" method only)'],
            ['Interactive','interactive','bool',1,'','interactively selection of some entity ids to assign specific parameters in the associated regions'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'','name of the array where the tags are stored'],
            ['Overwrite','overwrite','bool',1,'','toggle overwriting an already existing output array on the input surface'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ['CellEntityIdsArray','oentityidsarray','vtkIntArray',1,'','the output entity ids array']
            ])




    def SetSurfaceRepresentation(self, representation):
        if representation == 'surface':
            self.Actor.GetProperty().SetRepresentationToSurface()
            self.Actor.GetProperty().EdgeVisibilityOff()
        elif representation == 'edges':
            self.Actor.GetProperty().SetRepresentationToSurface()
            self.Actor.GetProperty().EdgeVisibilityOn()
        elif representation == 'wireframe':
            self.Actor.GetProperty().SetRepresentationToWireframe()
            self.Actor.GetProperty().EdgeVisibilityOff()
        self.Representation = representation


    def RepresentationCallback(self, obj):
        if not self.Actor:
            return
        if self.Representation == 'surface':
            representation = 'edges'
        elif self.Representation == 'edges':
            representation = 'wireframe'
        elif self.Representation == 'wireframe':
            representation = 'surface'
        self.SetSurfaceRepresentation(representation)
        self.vmtkRenderer.RenderWindow.Render()


    def ViewSurface(self,polydata,color):
        mapper = vtk.vtkPolyDataMapper()
        mapper.SetInputData(polydata)
        mapper.ScalarVisibilityOff()
        self.Actor = vtk.vtkActor()
        self.Actor.SetMapper(mapper)
        self.Actor.GetProperty().SetColor(color)
        self.SetSurfaceRepresentation(self.Representation)
        self.vmtkRenderer.Renderer.AddActor(self.Actor)
        self.vmtkRenderer.AddKeyBinding('w','Change surface representation.',self.RepresentationCallback)
        self.vmtkRenderer.Render()


    def LabelValidator(self,text):
        import string
        if not text:
            return 0
        if text == 'q':
            return 1
        if not text.split():
            return 0
        for char in text:
            if char not in string.digits + " ":
                return 0
        return 1

    def ParamValidator(self,text):
        import string
        if not text:
            return 1
        if not text.split():
            return 0
        for char in text:
            if char not in string.digits + "." + " ":
                return 0
        return 1

    def StringValidator(self,text):
        if not text:
            return 1
        if len(text.split())>1:
            return 0
        return 1

    def YesNoValidator(self,text):
        if text in ['n','y']:
            return 1
        return 0


    def InteractiveEdgeLengthArray(self):
        from vmtk import vmtkscripts
        
        self.CellEntityIdsArray = self.Surface.GetCellData().GetArray(self.CellEntityIdsArrayName)

        if self.CellEntityIdsArray == None:
            self.PrintError('Error: No CellEntityIds Array called '+self.CellEntityIdsArrayName+' defined on the input Surface, interactive mode cannot work')

        for i in range(self.Surface.GetNumberOfCells()):
            self.TagsSet.add(self.CellEntityIdsArray.GetComponent(i,0))
        self.TagsSet = sorted(self.TagsSet)
        self.PrintLog('Tags of the input surface: '+str(self.TagsSet))

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1
        self.vmtkRenderer.RegisterScript(self)

        surfaceViewer = vmtkscripts.vmtkSurfaceViewer()
        surfaceViewer.Surface = self.Surface
        surfaceViewer.vmtkRenderer = self.vmtkRenderer
        surfaceViewer.DisplayCellData = 1
        surfaceViewer.Representation = self.Representation
        surfaceViewer.Legend = 1
        surfaceViewer.LegendTitle = 'EntityIds'
        surfaceViewer.DisplayTag = 1
        surfaceViewer.RegionTagArrayName = self.CellEntityIdsArrayName
        surfaceViewer.Execute()

        while True:

            # input an ids list
            ok = False
            while not ok :
                queryString = "Please input a list of ids or \'q\' to quit: "
                labelString = self.InputText(queryString,self.LabelValidator)
                if labelString == 'q':
                    return
                labels = [int(label) for label in labelString.split()]
                print(labels)
                idsSelection = set(labels)
                if idsSelection.issubset(self.TagsSet):
                    ok = True

            # input the method to generate the EdgeLength Array
            ok = False
            while not ok :
                queryString = 'Please input the method; available "default"=1 or "constant"=2\n(type return to accept current value):\n\nMethod('+self.Method+'): '
                string = self.InputText(queryString,self.StringValidator)
                print(string)
                ok = True
                if string == '1' or string == 'default':
                    self.Method = 'default'
                elif string == '2' or string == 'constant':
                    self.Method = 'constant'
                elif string != '':
                    ok = False

            # input the parameters for the default method
            if self.Method == 'default':

                ok = False
                while not ok :
                    queryString = 'Please input the name of the array to process\n(type return to accept current value):\n\nInputArrayName('+self.InputArrayName+'): '
                    string = self.InputText(queryString,self.StringValidator)
                    print(string)
                    if string == '':
                        ok = True
                    else:
                        array = self.Surface.GetPointData().GetArray(string)
                        if array:
                            self.InputArrayName = string
                            self.InputArray = array
                            ok = True

                ok = False
                while not ok :
                    queryString = 'Please input the parameters of the method\n(type return to accept current value):\n\nAlpha('+str(self.Alpha)+'), Beta('+str(self.Beta)+'), MinSize('+str(self.MinSize)+'), MaxSize('+str(self.MaxSize)+'): '
                    paramString = self.InputText(queryString,self.ParamValidator)
                    if paramString == '':
                        ok = True
                    else:
                        params = [float(param) for param in paramString.split()]
                        print(params)
                        if len(params)<=4:
                            ok = True
                            self.Alpha = params[0]
                            if len(params)>1:
                                self.Beta = params[1]
                                if len(params)>2:
                                    self.MinSize = params[2]
                                    if len(params)>3:
                                        self.MaxSize = params[3]

            # input the parameters for the constant method
            elif self.Method == 'constant':
                ok = False
                while not ok :
                    queryString = 'Please input the parameters of the method\n(type return to accept current value):\n\nConstant('+str(self.Constant)+'): '
                    paramString = self.InputText(queryString,self.ParamValidator)
                    if paramString == '':
                        ok = True
                    else:
                        params = [float(param) for param in paramString.split()]
                        print(params)
                        if len(params)==1:
                            ok = True
                            self.Constant = params[0]

            # define edge-length on the selected ids
            newELFilter = vtk.vtkAppendPolyData()
            oldELFilter = vtk.vtkAppendPolyData()
            tags = []
            for i,item in enumerate(self.TagsSet):
                th = vtk.vtkThreshold()
                th.SetInputData(self.Surface)
                th.SetInputArrayToProcess(0,0,0,1,self.CellEntityIdsArrayName)
                th.ThresholdBetween(item-0.5,item+0.5)
                th.Update()
                gf = vtk.vtkGeometryFilter()
                gf.SetInputConnection(th.GetOutputPort())
                gf.Update()
                tags.append(gf.GetOutput())
                if item in idsSelection:
                    newELFilter.AddInputData(tags[i])
                else:
                    oldELFilter.AddInputData(tags[i])
            newELFilter.Update()
            oldELFilter.Update()
            newELSurface = newELFilter.GetOutput()
            oldELSurface = oldELFilter.GetOutput()
            newELSurface = self.CleanSurface(newELSurface)
            oldELSurface = self.CleanSurface(oldELSurface)

            self.EdgeLengthArrayWriter(newELSurface)
            finalAppend = vtk.vtkAppendPolyData()
            finalAppend.AddInputData(newELSurface)
            finalAppend.AddInputData(oldELSurface)
            finalAppend.Update()
            self.Surface = finalAppend.GetOutput()
            self.Surface = self.CleanSurface(self.Surface)



    def CleanSurface(self,surface):
        cleaner = vtk.vtkCleanPolyData()
        cleaner.SetInputData(surface)
        cleaner.Update()
        triangleFilter = vtk.vtkTriangleFilter()
        triangleFilter.SetInputData(cleaner.GetOutput())
        triangleFilter.Update()
        surface = triangleFilter.GetOutput()
        return surface



    def EdgeLengthArrayWriter(self,surface):
        outputArray = surface.GetPointData().GetArray(self.OutputArrayName)
        if self.Method == 'default':
            inputArray = surface.GetPointData().GetArray(self.InputArrayName)
            for i in range(surface.GetNumberOfPoints()):
                inputValue = inputArray.GetComponent(i,0)
                outputValue = max( self.MinSize, min( pow(self.Alpha*inputValue,self.Beta), self.MaxSize ) )
                outputArray.SetComponent(i,0,outputValue)
        elif self.Method == 'constant':
            outputArray.FillComponent(0,self.Constant)
        else:
            self.PrintError("Method unknown (available: default, constant)")


    def Execute(self):
        if self.Surface == None:
            self.PrintError('Error: No Surface.')

        self.OutputArray = self.Surface.GetPointData().GetArray(self.OutputArrayName)

        if self.OutputArray == None or self.Overwrite:
            self.OutputArray = vtk.vtkDoubleArray()
            self.OutputArray.SetName(self.OutputArrayName)
            self.OutputArray.SetNumberOfComponents(1)
            self.OutputArray.SetNumberOfTuples(self.Surface.GetNumberOfPoints())
            self.Surface.GetPointData().AddArray(self.OutputArray)

        if self.Method == 'default' and self.Overwrite:
            self.InputArray = self.Surface.GetPointData().GetArray(self.InputArrayName)
            if self.InputArray == None:
                self.PrintError('Error: No PointData Array called '+self.InputArrayName+' defined on the input Surface')

        self.EdgeLengthArrayWriter(self.Surface)

        if self.Interactive:
            self.InteractiveEdgeLengthArray()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()