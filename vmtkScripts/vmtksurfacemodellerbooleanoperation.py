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


class vmtkSurfaceModellerBooleanOperation(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Surface = None
        self.Surface2 = None
        self.Operation = 'union'
        self.Image = None
        self.Spacing = -1.0
        self.CorrectSpacing = 1
        self.ImageVoxelExpansion = 0.
        self.ExpansionDirections = [1, 1, 1, 1, 1, 1]
        self.Binary = 0
        self.NegativeInside = 1

        self.SetScriptName('vmtksurfacemodellerbooleanoperation')
        self.SetScriptDoc('performing boolean operation between two surfaces after converting them to images')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['Surface2','i2','vtkPolyData',1,'','the second input surface','vmtksurfacereader'],
            ['Operation','operation','str',1,'["union","intersection","difference"]','the boolean operation to be performed'],
            ['Spacing','samplespacing','float',1,'(0.0,)','spacing of the output image (isotropic)'],
            ['CorrectSpacing','correctspacing','bool',1,'','correct spacing in order to fit exactly the bounding box of the surface'],
            ['ImageVoxelExpansion','imagevoxelexpansion','int',1,'(0.0,)','expansion (in term of number of voxels) of the resulting image bounds compared to the input surfaces combined bounding boxes'],
            ['ExpansionDirections','expansiondirections','bool',6,'','expand only in true direction of this array [-x +x -y +y -z +z]'],
            ['Binary','binary','bool',1,'','binary image as output (overwrite the signeddistance value)']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter']])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        if self.Surface2 == None:
            self.PrintError('Error: No second input surface.')

        bounds1 = np.array( self.Surface.GetBounds() )
        bounds2 = np.array( self.Surface2.GetBounds() )
        bounds = [0., 0., 0., 0., 0., 0.]
        bounds[0] = min( bounds1[0], bounds2[0] )
        bounds[1] = max( bounds1[1], bounds2[1] )
        bounds[2] = min( bounds1[2], bounds2[2] )
        bounds[3] = max( bounds1[3], bounds2[3] )
        bounds[4] = min( bounds1[4], bounds2[4] )
        bounds[5] = max( bounds1[5], bounds2[5] )
        spacing = np.array( [self.Spacing, self.Spacing, self.Spacing] )

        # correct spacing in order to fit the bounding box
        if self.CorrectSpacing:
            xvoxels = (bounds[1] - bounds[0]) / self.Spacing
            yvoxels = (bounds[3] - bounds[2]) / self.Spacing
            zvoxels = (bounds[5] - bounds[4]) / self.Spacing
            spacing[0] = self.Spacing + ( xvoxels - math.floor( xvoxels ) ) * self.Spacing / math.floor( xvoxels )
            spacing[1] = self.Spacing + ( yvoxels - math.floor( yvoxels ) ) * self.Spacing / math.floor( yvoxels )
            spacing[2] = self.Spacing + ( zvoxels - math.floor( zvoxels ) ) * self.Spacing / math.floor( zvoxels )
            self.PrintLog( '    Corrected Spacing = ' + str(spacing) )

        # expanding bounds only in ExpansionDirections using ImageVoxelExpansion values
        bounds[0] = bounds[0] - self.ImageVoxelExpansion * spacing[0] * self.ExpansionDirections[0]
        bounds[1] = bounds[1] + self.ImageVoxelExpansion * spacing[0] * self.ExpansionDirections[1]
        bounds[2] = bounds[2] - self.ImageVoxelExpansion * spacing[1] * self.ExpansionDirections[2]
        bounds[3] = bounds[3] + self.ImageVoxelExpansion * spacing[1] * self.ExpansionDirections[3]
        bounds[4] = bounds[4] - self.ImageVoxelExpansion * spacing[2] * self.ExpansionDirections[4]
        bounds[5] = bounds[5] + self.ImageVoxelExpansion * spacing[2] * self.ExpansionDirections[5]

        # creating the image
        self.Image = vtk.vtkImageData()
        self.Image.SetOrigin( bounds[0], bounds[2], bounds[4] )
        self.Image.SetSpacing( spacing )
        self.Image.SetExtent( 0, int(math.ceil((bounds[1]-bounds[0])/spacing[0])), 0, int(math.ceil((bounds[3]-bounds[2])/spacing[1])), 0, int(math.ceil((bounds[5]-bounds[4])/spacing[2])) )
        self.Image.AllocateScalars( vtk.VTK_DOUBLE, 1 )

        # fill the image with the distance with the respective surface
        implicitDistanceFilter1 = vtk.vtkImplicitPolyDataDistance()
        implicitDistanceFilter2 = vtk.vtkImplicitPolyDataDistance()
        implicitDistanceFilter1.SetInput( self.Surface )
        implicitDistanceFilter2.SetInput( self.Surface2 )

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
            distance1 = implicitDistanceFilter1.EvaluateFunction( self.Image.GetPoint(i) )
            distance2 = implicitDistanceFilter2.EvaluateFunction( self.Image.GetPoint(i) )

            if self.Operation == 'union':
                value = min( distance1, distance2 )
            elif self.Operation == 'intersection':
                value = max( distance1, distance2 )
            elif self.Operation == 'difference':
                value = max( distance1, -distance2 )

            if self.Binary:
                if value > 0:
                    value = 1
                else:
                    value = 0

            scalars.SetTuple1( i, value )

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
