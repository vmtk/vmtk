## Program: VMTK
## Language:  Python
## Date:      January 10, 2018
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga, All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this code was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo

import pytest
import vmtk.vmtkimageinitialization as imageinitialization

#TODO: How do we test interaction?

def test_threshold_initialization_output_level_sets(aorta_image, compare_images):
    name = __name__ + '_test_threshold_initialization_output_level_sets.mha'
    initializer = imageinitialization.vmtkImageInitialization()
    initializer.Image = aorta_image
    initializer.Interactive = 0
    initializer.Method = 'threshold'
    initializer.LowerThreshold = 100
    initializer.UpperThreshold = 1000
    initializer.Execute()

    assert compare_images(initializer.InitialLevelSets, name) == True


def test_threshold_initialization_isosurface_value_is_zero(aorta_image):
    initializer = imageinitialization.vmtkImageInitialization()
    initializer.Image = aorta_image
    initializer.Interactive = 0
    initializer.Method = 'threshold'
    initializer.LowerThreshold = 100
    initializer.UpperThreshold = 1000
    initializer.Execute()

    assert initializer.IsoSurfaceValue == 0


def test_threshold_initialization_no_output_surface(aorta_image):
    initializer = imageinitialization.vmtkImageInitialization()
    initializer.Image = aorta_image
    initializer.Interactive = 0
    initializer.Method = 'threshold'
    initializer.LowerThreshold = 100
    initializer.UpperThreshold = 1000
    initializer.Execute()

    assert initializer.Surface is None


def test_isosurface_initialization_output_level_sets(aorta_image, compare_images):
    name = __name__ + '_test_isosurface_initialization_output_level_sets.mha'
    initializer = imageinitialization.vmtkImageInitialization()
    initializer.Image = aorta_image
    initializer.Interactive = 0
    initializer.Method = 'isosurface'
    initializer.IsoSurfaceValue = 500
    initializer.Execute()

    assert compare_images(initializer.InitialLevelSets, name) == True


def test_isosurface_initialization_output_isosurface_value(aorta_image):
    initializer = imageinitialization.vmtkImageInitialization()
    initializer.Image = aorta_image
    initializer.Interactive = 0
    initializer.Method = 'isosurface'
    initializer.IsoSurfaceValue = 500

    assert initializer.IsoSurfaceValue == 500

    initializer.Execute()

    # after execution the output isosurface value is reset to zero
    assert initializer.IsoSurfaceValue == 0


def test_fastmarching_initialization_output_level_sets(aorta_image, compare_images,
                                                       fast_marching_source_points,
                                                       fast_marching_target_points):
    name = __name__ + '_test_fastmarching_initialization_output_level_sets.mha'
    initializer = imageinitialization.vmtkImageInitialization()
    initializer.Image = aorta_image
    initializer.Interactive = 0
    initializer.Method = 'fastmarching'
    initializer.LowerThreshold = 700
    initializer.SourcePoints = fast_marching_source_points
    initializer.TargetPoints = fast_marching_target_points
    initializer.Execute()

    assert compare_images(initializer.InitialLevelSets, name) == True


def test_collidingfronts_initialization_output_level_sets(aorta_image, compare_images,
                                                          colliding_fronts_source_points,
                                                          colliding_fronts_target_points):
    name = __name__ + '_test_collidingfronts_initialization_output_level_sets.mha'
    initializer = imageinitialization.vmtkImageInitialization()
    initializer.Image = aorta_image
    initializer.Interactive = 0
    initializer.Method = 'collidingfronts'
    initializer.LowerThreshold = 700
    initializer.SourcePoints = colliding_fronts_source_points
    initializer.TargetPoints = colliding_fronts_target_points
    initializer.Execute()

    assert compare_images(initializer.InitialLevelSets, name) == True
