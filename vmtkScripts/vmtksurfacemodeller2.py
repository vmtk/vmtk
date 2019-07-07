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


class vmtkSurfaceModeller2(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Surface = None
        self.Image = None
        self.Spacing = None
        self.CorrectSpacing = 1
        self.ImageVoxelExpansion = 0.
        self.ExpansionDirections = [1, 1, 1, 1, 1, 1]
        self.Binary = 0
        self.DistanceThreshold = None
        self.NegativeInside = 1
        self.Bounds = []

        self.SetScriptName('vmtksurfacemodeller2')
        self.SetScriptDoc('converts a surface to an image containing the signed distance transform from the surface points')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['Spacing','samplespacing','float',3,'(0.0,)','spacing of the output image'],
            ['CorrectSpacing','correctspacing','bool',1,'','correct spacing in order to fit exactly the bounding box of the surface'],
            ['Bounds','bounds','float',6,'','the output image bounding box (if not set, the input surface bounding box is used)'],
            ['ImageVoxelExpansion','imagevoxelexpansion','int',1,'(0.0,)','expansion (in term of number of voxels) of the resulting image bounds compared to the input surface bounding box (only if bounds are not directly passed as input)'],
            ['ExpansionDirections','expansiondirections','bool',6,'','expand only in true direction of this array [-x +x -y +y -z +z]'],
            ['Binary','binary','bool',1,'','binary image as output (overwrite the signeddistance value)'],
            ['DistanceThreshold','distancethreshold','float',1,'(0.0,)','if set, point more distant than this threshold are taken constant'],
            ['NegativeInside','negativeinside','bool',1,'','toggle sign of distance transform negative inside the surface']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter'],
            ['Spacing','samplespacing','float',3,'(0.0,)','spacing of the output image'],
            ['Bounds','bounds','float',6,'(0.0,)','the output image bounding box (if not set, the input surface bounding box is used)']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        boundsAsInput = False
        if self.Bounds == []:
            self.Bounds = np.array( self.Surface.GetBounds() )
        else:
            boundsAsInput = True

        self.Spacing = np.array( [self.Spacing[0], self.Spacing[1], self.Spacing[2]] )

        # correct spacing in order to fit the bounding box
        if self.CorrectSpacing:
            xvoxels = (self.Bounds[1] - self.Bounds[0]) / self.Spacing[0]
            yvoxels = (self.Bounds[3] - self.Bounds[2]) / self.Spacing[1]
            zvoxels = (self.Bounds[5] - self.Bounds[4]) / self.Spacing[2]
            self.Spacing[0] = self.Spacing[0] + ( xvoxels - math.floor( xvoxels ) ) * self.Spacing[0] / math.floor( xvoxels )
            self.Spacing[1] = self.Spacing[1] + ( yvoxels - math.floor( yvoxels ) ) * self.Spacing[1] / math.floor( yvoxels )
            self.Spacing[2] = self.Spacing[2] + ( zvoxels - math.floor( zvoxels ) ) * self.Spacing[2] / math.floor( zvoxels )
            self.PrintLog( '    Corrected Spacing = ' + str(self.Spacing) )

        if not boundsAsInput:
            # expanding self.Bounds only in ExpansionDirections using ImageVoxelExpansion values
            self.Bounds[0] = self.Bounds[0] - self.ImageVoxelExpansion * self.Spacing[0] * self.ExpansionDirections[0]
            self.Bounds[1] = self.Bounds[1] + self.ImageVoxelExpansion * self.Spacing[0] * self.ExpansionDirections[1]
            self.Bounds[2] = self.Bounds[2] - self.ImageVoxelExpansion * self.Spacing[1] * self.ExpansionDirections[2]
            self.Bounds[3] = self.Bounds[3] + self.ImageVoxelExpansion * self.Spacing[1] * self.ExpansionDirections[3]
            self.Bounds[4] = self.Bounds[4] - self.ImageVoxelExpansion * self.Spacing[2] * self.ExpansionDirections[4]
            self.Bounds[5] = self.Bounds[5] + self.ImageVoxelExpansion * self.Spacing[2] * self.ExpansionDirections[5]



        # creating the image
        self.Image = vtk.vtkImageData()
        self.Image.SetOrigin( self.Bounds[0], self.Bounds[2], self.Bounds[4] )
        self.Image.SetSpacing( self.Spacing )
        self.Image.SetExtent( 0, int(math.ceil((self.Bounds[1]-self.Bounds[0])/self.Spacing[0])), 0, int(math.ceil((self.Bounds[3]-self.Bounds[2])/self.Spacing[1])), 0, int(math.ceil((self.Bounds[5]-self.Bounds[4])/self.Spacing[2])) )
        self.Image.AllocateScalars( vtk.VTK_DOUBLE, 1 )

        # fill the image with the distance with the respective surface
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
            value = implicitDistanceFilter.EvaluateFunction( self.Image.GetPoint(i) )
            if self.Binary:
                if value > 0:
                    value = 1
                else:
                    value = 0
            if self.DistanceThreshold:
                if np.absolute(value) > self.DistanceThreshold:
                    value = np.sign(value) * self.DistanceThreshold
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
