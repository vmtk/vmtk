#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacemodeller2.py,v $
## Language:  Python
## Date:      $Date: 2014/10/24 16:35:13 $
## Version:   $Revision: 1.10 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

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
import math

from vmtk import pypes


class vmtkSurfaceModeller(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Surface = None
        self.Image = None
        self.Spacing = -1.0
        self.ImageVoxelExpansion = 0.
        self.NegativeInside = 1

        self.SetScriptName('vmtksurfacemodeller')
        self.SetScriptDoc('converts a surface to an image containing the signed distance transform from the surface points')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['Spacing','samplespacing','float',1,'(0.0,)','spacing of the output image (isotropic)'],
            ['ImageVoxelExpansion','imagevoxelexpansion','int',1,'(0.0,)','expansion (in term of number of voxels) of the resulting image bounds compared to the input surface bounding box'],
            ['NegativeInside','negativeinside','bool',1,'','toggle sign of distance transform negative inside the surface']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter']])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        #1) create an images with size of the surface bounding box++
        bounds = np.array( self.Surface.GetBounds() )
        spacing = np.array( [self.Spacing, self.Spacing, self.Spacing] )
        boundExpansion = self.ImageVoxelExpansion * self.Spacing

        bounds[0] = bounds[0] - boundExpansion
        bounds[2] = bounds[2] - boundExpansion
        bounds[4] = bounds[4] - boundExpansion

        bounds[1] = bounds[1] + boundExpansion
        bounds[3] = bounds[3] + boundExpansion
        bounds[5] = bounds[5] + boundExpansion

        self.Image = vtk.vtkImageData()
        self.Image.SetOrigin( bounds[0], bounds[2], bounds[4] )
        self.Image.SetSpacing( spacing )
        self.Image.SetExtent( 0, int(math.ceil((bounds[1]-bounds[0])/spacing[0])), 0, int(math.ceil((bounds[3]-bounds[2])/spacing[1])), 0, int(math.ceil((bounds[5]-bounds[4])/spacing[2])) )
        self.Image.AllocateScalars( vtk.VTK_DOUBLE, 1 )

        #2) fill at each point the distance with the respective surface
        implicitDistanceFilter = vtk.vtkImplicitPolyDataDistance()
        implicitDistanceFilter.SetInput( self.Surface )

        scalars = self.Image.GetPointData().GetScalars()

        N = self.Image.GetNumberOfPoints()

        point = N / 100
        increment = N / 20

        self.PrintLog("    Computing signed distance in the output image:")

        toolbar_width = 40

        # setup toolbar
        sys.stdout.write( "    [%s]" % (" " * toolbar_width) )
        sys.stdout.flush()
        sys.stdout.write( "\b" * ( toolbar_width + 1 ) ) # return to start of line, after '['

        for i in range(N):
            scalars.SetTuple1( i, implicitDistanceFilter.EvaluateFunction( self.Image.GetPoint(i) ) )
            if ( i % ( int( N / toolbar_width ) ) == 0 ):
                sys.stdout.write("-")
                sys.stdout.flush()

        sys.stdout.write("]\n")

        if self.NegativeInside:
            negate = vtk.vtkImageMathematics()
            negate.SetInputData( self.Image )
            negate.SetConstantK( -1.0 )
            negate.SetOperationToMultiplyByK()
            negate.Update()
            self.Image = negate.GetOutput()



if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
