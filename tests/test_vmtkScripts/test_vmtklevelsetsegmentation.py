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


@pytest.fixture(scope='function')
def initial_level_sets(test_data):
    import vmtk.vmtkimagereader as reader
    import os
    read = reader.vmtkImageReader()
    read.InputFileName = os.path.join(test_data, 'aorta-fastmarching-initial-levelset.mha')
    read.Execute()

    return read.Image


@pytest.fixture(scope='function')
def feature_image(test_data):
    import vmtk.vmtkimagereader as reader
    import os
    read = reader.vmtkImageReader()
    read.InputFileName = os.path.join(test_data, 'aorta-feature-image.mha')
    read.Execute()

    return read.Image


@pytest.mark.parametrize("level_sets_type,expected_hash", [
    ("geodesic", 'd59c250b798684ed9f0b414001e7b29503529b23'),
    ("curves", '8e3a2d00c1e79b706f6ca92f886d13fb58522211'),
    ("laplacian", '88561c10dcfaf6c64ae1509094c443aabc9c6e5d'),
])
def test_level_sets(aorta_image, initial_level_sets, feature_image,
                    level_sets_type, expected_hash, image_to_sha):
    ls = levelsetsegmentation.vmtkLevelSetSegmentation()
    ls.Image = aorta_image
    ls.InitialLevelSets = initial_level_sets
    ls.FeatureImage = feature_image
    ls.LevelSetsType = level_sets_type
    ls.NumberOfIterations = 10
    ls.Execute()

    assert image_to_sha(ls.LevelSets) == expected_hash


@pytest.mark.parametrize('iterations,propogation,curvature,advection,expected_hash,', [
    (20, 0.0, 0.0, 1.0, '83904b7c0a395af6c0fabdd3c172a7fcca518259'),
    (10, 2.0, 0.0, 1.0, '647aa580aa8f465b02e0a39c620d6b356c4d322d'),
    (10, 0.0, 4.0, 1.0, '93d118c14f6202e3183e52d48477635127ed3cca'),
    (10, 0.0, 0.0, 3.0, '6fb5f1088d0a371baab82e4bec6f3f0795d10df0'),
])
def test_geodesic_level_set_parameters(aorta_image, initial_level_sets, feature_image,
                                       iterations, propogation, curvature, advection,
                                       expected_hash, image_to_sha):
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

    assert image_to_sha(ls.LevelSets) == expected_hash