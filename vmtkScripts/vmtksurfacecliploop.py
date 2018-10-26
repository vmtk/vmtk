## Program:   VMTK
## Module:    $RCSfile: vmtksurfacecliploop.py,v $
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
from __future__ import print_function # NEED TO STAY AS TOP IMPORT
import vtk
import sys
from vmtk import pypes


class vmtkSurfaceClipLoop(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Loop = None

        #Output members

        self.SetScriptName('vmtksurfacecliploop')
        self.SetScriptDoc('Clip input surface using a loop')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['Loop','i2','vtkPolyData',1,'','the input loop','vmtksurfacereader'],
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ])

    def Execute(self):

        from vmtk import vmtkscripts
        if self.Surface == None:
            self.PrintError('Error: no Surface.')

        triangleFilter = vtk.vtkTriangleFilter()
        triangleFilter.SetInputData(self.Surface)
        triangleFilter.Update()

        self.Surface = triangleFilter.GetOutput()

        if self.Loop == None:
            self.PrintError('Error: no Loop.')

        select = vtk.vtkImplicitSelectionLoop()
        select.SetLoop(self.Loop.GetPoints())
        normal = [0.0,0.0,0.0]
        centroid = [0.0,0.0,0.0]
        vtk.vtkPolygon().ComputeNormal(self.Loop.GetPoints(),normal)

        #compute centroid and check normals
        p = [0.0,0.0,0.0]
        for i in range (self.Loop.GetNumberOfPoints()):
            p = self.Loop.GetPoint(i)
            centroid[0] += p[0]
            centroid[1] += p[1]
            centroid[2] += p[2]
        centroid[0] = centroid[0] / self.Loop.GetNumberOfPoints()
        centroid[1] = centroid[1] / self.Loop.GetNumberOfPoints()
        centroid[2] = centroid[2] / self.Loop.GetNumberOfPoints()
        print("loop centroid", centroid)

        locator = vtk.vtkPointLocator()
        locator.SetDataSet(self.Surface)
        locator.AutomaticOn()
        locator.BuildLocator()
        idsurface = locator.FindClosestPoint(centroid)

        if (self.Surface.GetPointData().GetNormals() == None):
            normalsFilter = vmtkscripts.vmtkSurfaceNormals()
            normalsFilter.Surface = self.Surface
            normalsFilter.NormalsArrayName = 'Normals'
            normalsFilter.Execute()
            self.Surface = normalsFilter.Surface
        normalsurface = [0.0,0.0,0.0]
        self.Surface.GetPointData().GetNormals().GetTuple(idsurface,normalsurface)
        print("loop normal: ", normal)
        print("surface normal inside the loop: ", normalsurface)
        check = vtk.vtkMath.Dot(normalsurface,normal)
        if check < 0:
            normal[0] = - normal[0]
            normal[1] = - normal[1]
            normal[2] = - normal[2]

        #compute plane
        proj = float(vtk.vtkMath.Dot(self.Loop.GetPoint(0),normal))
        point = [0.0,0.0,0.0]
        self.Loop.GetPoint(0,point)
        for i in range (self.Loop.GetNumberOfPoints()):
            tmp = vtk.vtkMath.Dot(self.Loop.GetPoint(i),normal)
            if tmp < proj:
                proj = tmp
                self.Loop.GetPoint(i,point)
        origin = [0.0,0.0,0.0]
        origin[0] = point[0] #- normal[0]
        origin[1] = point[1] #- normal[1]
        origin[2] = point[2] #- normal[2]
        plane=vtk.vtkPlane()
        plane.SetNormal(normal[0],normal[1],normal[2])
        plane.SetOrigin(origin[0],origin[1],origin[2])

        #set bool
        Bool = vtk.vtkImplicitBoolean()
        Bool.SetOperationTypeToDifference()
        Bool.AddFunction(select)
        Bool.AddFunction(plane)

        clipper=vtk.vtkClipPolyData()
        clipper.SetInputData(self.Surface)
        clipper.SetClipFunction(Bool)
        clipper.GenerateClippedOutputOn()
        clipper.InsideOutOff()
        clipper.Update()

        self.Surface = clipper.GetOutput()


if __name__ == '__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
