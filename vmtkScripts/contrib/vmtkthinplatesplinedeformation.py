#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkthinplatesplinedeformation.py,v $
## Language:  Python
## Date:      $$
## Version:   $$

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by 
##       Tangui Morvan
##       Kalkulo AS
##       Simula Research Laboratory

## This class deforms a surface using a thin plate spline transform
## The thin plate spline transform is specified with a set of source and destination landmarks


import vtk
import sys

import vtkvmtk
import vtkvmtkcontrib
import vmtkrenderer
import pypes

vmtkthinplatesplinedeformation = 'vmtkThinPlateSplineDeformation'


class vmtkThinPlateSplineDeformation(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Surface = None
        self.DeformedSurface = None
        self.SourcePoints = vtk.vtkPoints()
        self.TargetPoints = vtk.vtkPoints()
        self.DisplacementNorms = vtk.vtkDoubleArray()
        self.Displacements = vtk.vtkDoubleArray()
        self.Displacements.SetNumberOfComponents(3)
        self.SourceSpheres = vtk.vtkPolyData()
        self.TargetSpheres = vtk.vtkPolyData()
        self.SourceSpheres.SetPoints(self.SourcePoints)
        self.TargetSpheres.SetPoints(self.TargetPoints)
        self.SourceSpheres.GetPointData().SetScalars(self.DisplacementNorms)
        self.SourceSpheres.GetPointData().SetVectors(self.Displacements)
        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.DisplayDeformed = False
        self.SurfaceMapper = None
        self.Opacity = 1.
        self.SourceSpheresActor = None
        self.TargetSpheresActor = None

        self.SetScriptName('vmtkthinplatesplinedeformation')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['Opacity','opacity','float',1,'(0.0,1.0)','object opacities in the scene'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['DeformedSurface','o','vtkPolyData',1,'','','vmtksurfacewriter']
            ])
        
        
    def DeformSurface(self):
        #interpolate and sample the displacement norms over the surface
        rbf = vtkvmtkcontrib.vtkvmtkRBFInterpolation2()
        rbf.SetSource(self.SourceSpheres)
        rbf.SetRBFTypeToBiharmonic()
        rbf.ComputeCoefficients()
        sampler = vtkvmtkcontrib.vtkvmtkPolyDataSampleFunction()
        sampler.SetInput(self.Surface)
        sampler.SetImplicitFunction(rbf)
        sampler.SetSampleArrayName("DisplacementNorms")
        sampler.Update()
                
        sampArray = sampler.GetOutput().GetPointData().GetArray("DisplacementNorms")
                
        ##Clamp the negative values to 0 and the positive values to one in a weight array
        calculator = vtk.vtkArrayCalculator()
        calculator.SetInput(sampler.GetOutput())
        calculator.AddScalarArrayName("DisplacementNorms")
        calculator.SetFunction("if( DisplacementNorms > 0 , iHat, jHat)")
        calculator.SetResultArrayName("Weights")
        calculator.SetResultArrayType(vtk.VTK_FLOAT)
        calculator.Update()
        
        #Create the transform
        thinPlateSplineTransform = vtk.vtkThinPlateSplineTransform()
        thinPlateSplineTransform.SetBasisToR()
        thinPlateSplineTransform.SetSourceLandmarks(self.SourcePoints)
        thinPlateSplineTransform.SetTargetLandmarks(self.TargetPoints)
        
        transform = vtk.vtkTransform()
        transform.Identity()        
        transform2 = vtk.vtkTransform()
        transform2.Identity()
        
        #Apply weighted transform
        transformFilter = vtk.vtkWeightedTransformFilter()
        transformFilter.SetInput(calculator.GetOutput())
        transformFilter.SetNumberOfTransforms(3)
        transformFilter.SetWeightArray("Weights")
        transformFilter.SetTransform(thinPlateSplineTransform,0)
        transformFilter.SetTransform(transform,1)
        transformFilter.SetTransform(transform2,2)
        transformFilter.Update()

        normalsFilter = vtk.vtkPolyDataNormals()
        normalsFilter.SetInput(transformFilter.GetOutput())
        normalsFilter.Update()
        
        self.DeformedSurface = normalsFilter.GetOutput()

    
    def InitializeSpheres(self):
        self.SourcePoints.Reset()
        self.TargetPoints.Reset()
        self.Displacements.Reset()
        self.DisplacementNorms.Reset()
        self.CurrentSourcePointId = -1
        self.CurrentTargetPointId = -1
        self.SphereWidget.Off()
        
    def AddDisplacement(self,source,target):
      displacement = [0, 0, 0]
      vtk.vtkMath.Subtract(target,source,displacement)
      self.SourcePoints.InsertNextPoint(source)
      self.Displacements.InsertNextTuple3(displacement[0],displacement[1],displacement[2])
      self.DisplacementNorms.InsertNextValue(vtk.vtkMath.Norm(displacement))
      self.SourceSpheres.Modified()
      self.TargetPoints.InsertNextPoint(target)
      self.TargetSpheres.Modified()
      
    def SetDisplacement(self,ind, source,target):
      if (ind<0) or (ind>=self.SourcePoints.GetNumberOfPoints()):
          return;
      displacement = [0, 0, 0]
      vtk.vtkMath.Subtract(target,source,displacement)
      self.SourcePoints.SetPoint(ind,source)
      self.Displacements.SetTuple(ind,displacement)
      self.DisplacementNorms.SetValue(ind,vtk.vtkMath.Norm(displacement))
      self.TargetPoints.SetPoint(ind,target)
      self.SourceSpheres.Modified()
      self.TargetSpheres.Modified()
        
    def PlaceSphere(self):
        if self.CurrentSourcePointId != -1:
            self.SphereWidget.SetCenter(self.SourceSpheres.GetPoint(self.CurrentSourcePointId))
            self.SphereWidget.On()
        elif self.CurrentTargetPointId != -1:
            self.SphereWidget.SetCenter(self.TargetSpheres.GetPoint(self.CurrentTargetPointId))
            self.SphereWidget.On()
        else:
            self.SphereWidget.Off()
        self.vmtkRenderer.RenderWindow.Render()
        
    def SelectSource(self, pointId):
        self.CurrentTargetPointId = -1
        self.CurrentSourcePointId = pointId
        self.PlaceSphere()
        
    def SelectTarget(self, pointId):
        self.CurrentTargetPointId = pointId
        self.CurrentSourcePointId = -1
        self.PlaceSphere()
        
    def SphereCallback(self,widget,event_string):
        if self.CurrentSourcePointId != -1:
            self.SetDisplacement(self.CurrentSourcePointId,self.SphereWidget.GetCenter(),self.TargetPoints.GetPoint(self.CurrentSourcePointId))
        elif self.CurrentTargetPointId != -1:
            self.SetDisplacement(self.CurrentTargetPointId,self.SourcePoints.GetPoint(self.CurrentTargetPointId),self.SphereWidget.GetCenter())
          
    def ResetCallback(self,obj):
        if self.CurrentSourcePointId != -1:
            self.SetDisplacement(self.CurrentSourcePointId,self.SourcePoints.GetPoint(self.CurrentSourcePointId),self.SourcePoints.GetPoint(self.CurrentSourcePointId))
        elif self.CurrentTargetPointId != -1:
            self.SetDisplacement(self.CurrentTargetPointId,self.SourcePoints.GetPoint(self.CurrentTargetPointId),self.SourcePoints.GetPoint(self.CurrentTargetPointId))
        self.PlaceSphere()
        self.vmtkRenderer.RenderWindow.Render()
          
    def DeleteCallback(self,obj):
        delId = -1
        sourceValid = False
        if self.CurrentSourcePointId != -1:
            delId = self.CurrentSourcePointId
            sourceValid = True
        elif self.CurrentTargetPointId != -1:
            delId = self.CurrentTargetPointId
        else:
            return
        savedSources = vtk.vtkPoints()
        savedSources.DeepCopy(self.SourcePoints)
        savedTargets = vtk.vtkPoints()
        savedTargets.DeepCopy(self.TargetPoints)
        self.InitializeSpheres()
        
        for i in range(savedSources.GetNumberOfPoints()):
            if i != delId:
                self.AddDisplacement(savedSources.GetPoint(i),savedTargets.GetPoint(i))
            
        nPoints = self.SourcePoints.GetNumberOfPoints()
            
        if nPoints == 0:
            self.SelectSource(-1)
        elif sourceValid:
            self.SelectSource((delId-1)%nPoints)
        else:
            self.SelectTarget((delId-1)%nPoints)
        
        self.SourceSpheres.Modified()
        self.TargetSpheres.Modified()
        
        self.vmtkRenderer.RenderWindow.Render()
            
                
          
    def UndoCallback(self,obj):
        self.InitializeSpheres()
        self.SourceSpheres.Modified()
        self.TargetSpheres.Modified()
        self.vmtkRenderer.RenderWindow.Render()
        

    def PickCallback(self,obj):
        picker = vtk.vtkCellPicker()
        picker.SetTolerance(1E-4 * self.Surface.GetLength())
        eventPosition = self.vmtkRenderer.RenderWindowInteractor.GetEventPosition()
        #eventPosition = obj.GetEventPosition()
        self.SphereWidget.Off()
        result = picker.Pick(float(eventPosition[0]),float(eventPosition[1]),0.0,self.vmtkRenderer.Renderer)
        self.SphereWidget.On()
        if result == 0:
            return
        pickPosition = picker.GetPickPosition()
        self.AddDisplacement(pickPosition,pickPosition)
        self.SelectTarget(self.TargetPoints.GetNumberOfPoints() - 1)
        pickedCellPointIds = self.Surface.GetCell(picker.GetCellId()).GetPointIds()
        self.vmtkRenderer.RenderWindow.Render()
        
    def SelectCallback(self,obj):
        picker = vtk.vtkCellPicker()
        picker.SetTolerance(1E-4 * self.Surface.GetLength())
        eventPosition = self.vmtkRenderer.RenderWindowInteractor.GetEventPosition()
        #eventPosition = obj.GetEventPosition()
        self.SourcePointActor.PickableOn()
        self.TargetPointActor.PickableOn()
        self.SphereWidget.Off()
        result = picker.Pick(float(eventPosition[0]),float(eventPosition[1]),0.0,self.vmtkRenderer.Renderer)
        self.SphereWidget.On()
        self.SourcePointActor.PickableOff()
        self.TargetPointActor.PickableOff()
        if result == 0:
            return
        pickDataSet = picker.GetDataSet()
        pickPointId = picker.GetPointId()
        if pickDataSet == self.SourceGlyphs.GetOutput():
            inputIds = self.SourceGlyphs.GetOutput().GetPointData().GetArray("InputPointIds")
            self.SelectSource(inputIds.GetValue(pickPointId))
        elif pickDataSet == self.TargetGlyphs.GetOutput():
            inputIds = self.TargetGlyphs.GetOutput().GetPointData().GetArray("InputPointIds")
            self.SelectTarget(inputIds.GetValue(pickPointId))
        self.vmtkRenderer.RenderWindow.Render()

      
    def NextCallback(self,obj):
        if self.CurrentSourcePointId != -1:
            self.SelectTarget(self.CurrentSourcePointId)
        elif self.CurrentTargetPointId != -1:
            self.SelectSource((self.CurrentTargetPointId + 1) % self.SourcePoints.GetNumberOfPoints())

    def PreviousCallback(self,obj):
        if self.CurrentSourcePointId != -1:
            self.SelectTarget((self.CurrentSourcePointId - 1) % self.TargetPoints.GetNumberOfPoints())
        elif self.CurrentTargetPointId != -1:
            self.SelectSource(self.CurrentTargetPointId)

    def ShowDeformedCallback(self,obj):
        if self.SurfaceMapper.GetInput() == self.Surface:
            self.DeformSurface()
            self.SurfaceMapper.SetInput(self.DeformedSurface)
        else:
            self.SurfaceMapper.SetInput(self.Surface)
        self.vmtkRenderer.RenderWindow.Render()
        
    def PlaceCylinder(self):
        sourcePoint = self.DisplacementGlyphs.GetPoint()
        displacementNorm = self.DisplacementNorms.GetValue(self.DisplacementGlyphs.GetPointId())
        displacement = self.Displacements.GetTuple3(self.DisplacementGlyphs.GetPointId())
        self.CylinderSource.SetHeight(displacementNorm)
        
        self.CylinderTransform.Identity()
        
        
        self.CylinderTransform.Translate(displacement[0]/2.,displacement[1]/2.,displacement[2]/2.)
        self.CylinderTransform.Translate(sourcePoint[0],sourcePoint[1],sourcePoint[2])
        
        if ((displacement[0]==0) and (displacement[2]==0)):
            if (displacement[1]<0):
                self.CylinderTransform.RotateWXYZ(180.0,0,1,0)
        else:
            v = [(displacement[0])/2., (displacement[1]+displacementNorm)/2., displacement[2]/2.]
            self.CylinderTransform.RotateWXYZ(180.0,v[0],v[1],v[2])
        
        

    def Execute(self):
        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        if not self.vmtkRenderer:
          self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
          self.vmtkRenderer.Initialize()
          self.OwnRenderer = 1
          
        self.vmtkRenderer.RegisterScript(self) 
	
        self.DeformedSurface = self.Surface
        
        
        sphereSource = vtk.vtkSphereSource()
        sphereSource.SetRadius(1)
        
        self.SourceGlyphs = vtk.vtkGlyph3D()
        self.SourceGlyphs.SetInput(self.SourceSpheres)
        self.SourceGlyphs.SetSource(sphereSource.GetOutput())
        self.SourceGlyphs.SetScaleModeToDataScalingOff()
        self.SourceGlyphs.SetScaleFactor(self.Surface.GetLength()*0.01)
        self.SourceGlyphs.GeneratePointIdsOn()
        self.SourceGlyphMapper = vtk.vtkPolyDataMapper()
        self.SourceGlyphMapper.SetInput(self.SourceGlyphs.GetOutput())
        self.SourceGlyphMapper.ScalarVisibilityOff()
        self.SourcePointActor = vtk.vtkActor()
        self.SourcePointActor.SetMapper(self.SourceGlyphMapper)
        self.SourcePointActor.GetProperty().SetColor(1.0,0.0,0.0)
        self.SourcePointActor.GetProperty().SetOpacity(self.Opacity)
        self.SourcePointActor.PickableOff()
        self.vmtkRenderer.Renderer.AddActor(self.SourcePointActor)
        
        self.TargetGlyphs = vtk.vtkGlyph3D()
        self.TargetGlyphs.SetInput(self.TargetSpheres)
        self.TargetGlyphs.SetSource(sphereSource.GetOutput())
        self.TargetGlyphs.SetScaleModeToDataScalingOff()
        self.TargetGlyphs.SetScaleFactor(self.Surface.GetLength()*0.01)
        self.TargetGlyphs.GeneratePointIdsOn()
        self.TargetGlyphMapper = vtk.vtkPolyDataMapper()
        self.TargetGlyphMapper.SetInput(self.TargetGlyphs.GetOutput())
        self.TargetGlyphMapper.ScalarVisibilityOff()
        self.TargetPointActor = vtk.vtkActor()
        self.TargetPointActor.SetMapper(self.TargetGlyphMapper)
        self.TargetPointActor.GetProperty().SetColor(0.0,0.0,1.0)
        self.TargetPointActor.GetProperty().SetOpacity(self.Opacity)
        self.TargetPointActor.PickableOff()
        self.vmtkRenderer.Renderer.AddActor(self.TargetPointActor)
        
        self.CylinderSource = vtk.vtkCylinderSource()
        self.CylinderSource.SetCenter(0,0,0)
        self.CylinderSource.SetRadius(self.Surface.GetLength()*0.0025)
        self.CylinderSource.SetHeight(0)
        
        self.CylinderTransform = vtk.vtkTransform()
        self.CylinderTransform.Identity()
        
        self.CylinderTransformFilter = vtk.vtkTransformPolyDataFilter()
        self.CylinderTransformFilter.SetTransform(self.CylinderTransform)
        self.CylinderTransformFilter.SetInputConnection(self.CylinderSource.GetOutputPort())
        
        self.DisplacementGlyphs = vtk.vtkProgrammableGlyphFilter()
        self.DisplacementGlyphs.SetInput(self.SourceSpheres)
        self.DisplacementGlyphs.SetSource(self.CylinderTransformFilter.GetOutput())
        self.DisplacementGlyphs.SetGlyphMethod(self.PlaceCylinder)
        self.DisplacementGlyphsMapper = vtk.vtkPolyDataMapper()
        self.DisplacementGlyphsMapper.SetInput(self.DisplacementGlyphs.GetOutput())
        self.DisplacementGlyphsMapper.ScalarVisibilityOff()
        self.DisplacementGlyphsActor = vtk.vtkActor()
        self.DisplacementGlyphsActor.SetMapper(self.DisplacementGlyphsMapper)
        self.DisplacementGlyphsActor.GetProperty().SetColor(0.0,1.0,0.0)
        self.DisplacementGlyphsActor.GetProperty().SetOpacity(self.Opacity)
        self.DisplacementGlyphsActor.PickableOff()
        self.vmtkRenderer.Renderer.AddActor(self.DisplacementGlyphsActor)
	
        #self.vmtkRenderer.RenderWindowInteractor.AddObserver("KeyPressEvent", self.KeyPressed)
        self.vmtkRenderer.AddKeyBinding('u','Undo.',self.UndoCallback)
        self.vmtkRenderer.AddKeyBinding('space','Add displacement.',self.PickCallback)
        self.vmtkRenderer.AddKeyBinding('s','Select source/target.',self.SelectCallback)
        self.vmtkRenderer.AddKeyBinding('n','Skip to next source/target.',self.NextCallback)
        self.vmtkRenderer.AddKeyBinding('v','Skip to previous source/target.',self.PreviousCallback)
        self.vmtkRenderer.AddKeyBinding('x','Delete source/target.',self.DeleteCallback)
        self.vmtkRenderer.AddKeyBinding('r','Reset target to source.',self.ResetCallback)
        self.vmtkRenderer.AddKeyBinding('d','Show deformed surface.',self.ShowDeformedCallback)
        
        self.SurfaceMapper = vtk.vtkPolyDataMapper()
        self.SurfaceMapper.SetInput(self.Surface)
        self.SurfaceMapper.SetScalarVisibility(0)
        surfaceActor = vtk.vtkActor()
        surfaceActor.SetMapper(self.SurfaceMapper)
        surfaceActor.GetProperty().SetOpacity(self.Opacity)
        self.vmtkRenderer.Renderer.AddActor(surfaceActor)
                
        self.SphereWidget = vtk.vtkSphereWidget()
        self.SphereWidget.SetRadius(self.Surface.GetLength()*0.01)
        self.SphereWidget.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
        self.SphereWidget.AddObserver("InteractionEvent", self.SphereCallback)

        self.InputInfo('Please position the mouse and press space to add source and target points, \'u\' to undo\n')

        self.InitializeSpheres()
        self.vmtkRenderer.Render()
 
        if self.Surface.GetSource():
            self.Surface.GetSource().UnRegisterAllOutputs()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()

if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
