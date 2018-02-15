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
import vmtk.vmtklevelsetsegmentation as levelsetsegmentation


@pytest.fixture(scope='module')
def initial_level_sets(input_datadir):
    import vmtk.vmtkimagereader as reader
    import os
    read = reader.vmtkImageReader()
    read.InputFileName = os.path.join(input_datadir, 'aorta-fastmarching-initial-levelset.mha')
    read.Execute()

    return read.Image


@pytest.fixture(scope='module')
def feature_image(input_datadir):
    import vmtk.vmtkimagereader as reader
    import os
    read = reader.vmtkImageReader()
    read.InputFileName = os.path.join(input_datadir, 'aorta-feature-image.mha')
    read.Execute()

    return read.Image


@pytest.mark.parametrize("level_sets_type,paramid", [
    ("geodesic", '0'),
    ("curves", '1'),
    ("laplacian", '2'),
])
def test_level_sets(aorta_image, initial_level_sets, feature_image,
                    level_sets_type, paramid, compare_images):
    name = __name__ + '_test_level_sets_' + paramid + '.mha'
    ls = levelsetsegmentation.vmtkLevelSetSegmentation()
    ls.Image = aorta_image
    ls.InitialLevelSets = initial_level_sets
    ls.FeatureImage = feature_image
    ls.LevelSetsType = level_sets_type
    ls.NumberOfIterations = 10
    ls.Execute()

    assert compare_images(ls.LevelSets, name) == True


@pytest.mark.parametrize('iterations,propogation,curvature,advection,paramid,', [
    (20, 0.0, 0.0, 1.0, '0'),
    (10, 2.0, 0.0, 1.0, '1'),
    (10, 0.0, 4.0, 1.0, '2'),
    (10, 0.0, 0.0, 3.0, '3'),
])
def test_geodesic_level_set_parameters(aorta_image, initial_level_sets, feature_image,
                                       iterations, propogation, curvature, advection,
                                       paramid, compare_images):
    name = __name__ + '_test_geodesic_level_set_parameters_' + paramid + '.mha'
    ls = levelsetsegmentation.vmtkLevelSetSegmentation()
    ls.Image = aorta_image
    ls.InitialLevelSets = initial_level_sets
    ls.FeatureImage = feature_image
    ls.LevelSetsType = 'geodesic'
    ls.NumberOfIterations = iterations
    ls.PropagationScaling = propogation
    ls.CurvatureScaling = curvature
    ls.AdvectionScaling = advection
    ls.Execute()

    assert compare_images(ls.LevelSets, name) == True