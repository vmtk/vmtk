#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkcenterlineinterpolation.py,v $
## Language:  Python
## Date:      $Date: 2006/07/17 09:52:56 $
## Version:   $Revision: 1.1 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes


class vmtkCenterlineInterpolation(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Centerlines = None
        self.MaskArrayName = None
        self.Threshold = 0.0
        self.InsideOut = 0

        self.SetScriptName('vmtkcenterlineinterpolation')
        self.SetScriptDoc('smooth centerlines with a moving average filter')
        self.SetInputMembers([
            ['Centerlines','i','vtkPolyData',1,'','the input centerlines','vmtksurfacereader'],
            ['MaskArrayName','array','str',1,'','the array used for determining what portion of the centerline has to be reinterpolated is stored'],
            ['Threshold','threshold','float',1,'','value of the mask array below which (above which, in case insideout is 1) the centerline is reinterpolated'],
            ['InsideOut','insideout','bool',1,'','toggle interpolating below (0) or above (1) the threshold value']
            ])
        self.SetOutputMembers([
            ['Centerlines','o','vtkPolyData',1,'','the output centerlines','vmtksurfacewriter']
            ])

    def IsMasked(self, value):
        return (not self.InsideOut and value < self.Threshold) or (self.InsideOut and value > self.Threshold)

    def Execute(self):

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        maskArray = self.Centerlines.GetPointData().GetArray(self.MaskArrayName)

        numberOfArrays = self.Centerlines.GetPointData().GetNumberOfArrays()

        for i in range(self.Centerlines.GetNumberOfCells()):

            cell = self.Centerlines.GetCell(i)

            xSpline = vtk.vtkCardinalSpline()
            ySpline = vtk.vtkCardinalSpline()
            zSpline = vtk.vtkCardinalSpline()

            aSplines = []
            for j in range(numberOfArrays):
                aSplines.append(vtk.vtkCardinalSpline())

            abscissaArray = vtk.vtkFloatArray()

            abscissa = 0.0
            previousPoint = None
            for j in range(cell.GetNumberOfPoints()):

                pointId = cell.GetPointId(j)
                point = self.Centerlines.GetPoint(pointId)

                if previousPoint:
                    abscissa += vtk.vtkMath.Distance2BetweenPoints(point,previousPoint)**0.5

                abscissaArray.InsertNextValue(abscissa)

                if not self.IsMasked(maskArray.GetTuple1(pointId)):

                    xSpline.AddPoint(abscissa,point[0])
                    ySpline.AddPoint(abscissa,point[1])
                    zSpline.AddPoint(abscissa,point[2])

                    for k in range(numberOfArrays):
                        array = self.Centerlines.GetPointData().GetArray(k)
                        if array.GetNumberOfComponents() != 1:
                            continue
                        value = array.GetTuple1(pointId)
                        aSplines[k].AddPoint(abscissa,value)

                previousPoint = point

            for j in range(cell.GetNumberOfPoints()):

                pointId = cell.GetPointId(j)

                if not self.IsMasked(maskArray.GetTuple1(pointId)):
                    continue

                abscissa = abscissaArray.GetValue(pointId)

                point = [xSpline.Evaluate(abscissa), ySpline.Evaluate(abscissa), zSpline.Evaluate(abscissa)]
                self.Centerlines.SetPoint(pointId,point)

                for k in range(numberOfArrays):
                    array = self.Centerlines.GetPointData().GetArray(k)
                    if array.GetNumberOfComponents() != 1:
                        continue
                    value = aSplines[k].Evaluate(abscissa)
                    array.SetTuple1(pointId,value)


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
