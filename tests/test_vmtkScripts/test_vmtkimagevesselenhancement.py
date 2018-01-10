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
import vmtk.vmtkimagevesselenhancement as vesselenhancement


#TODO: VEDM Method is currently diabled in vtkVmtk (vtkvmtkVesselEnhancingDiffusion3DImageFilter). Find a fix or remove.
@pytest.mark.parametrize("enhance_method,expected_sha", [
    ('frangi', '8103abde74c72a498c68250e1fd85b81415f4e73'),
    ('sato', '41cc6d5a56d2d68ffce0e9d05ae192f86f1e147c'),
    ('ved', '08c6d50899ecfbe5ad42d9c17703b54f2b4fe428'),
])
def test_enhancement_methods_with_default_params(aorta_image, image_to_sha,
                                                 enhance_method,
                                                 expected_sha):
    enhancer = vesselenhancement.vmtkImageVesselEnhancement()
    enhancer.Image = aorta_image
    enhancer.Method = enhance_method
    enhancer.Execute()

    assert image_to_sha(enhancer.Image) == expected_sha


@pytest.mark.parametrize("scaled,alpha,beta,gamma,expected_sha", [
    (1, 0.5, 0.5, 5.0, 'c62d6261246849195d6aab891afbb2bcdc2d04c7'),
    (1, 1.5, 0.5, 5.0, '479c47fe8502beffa18b9063551ad6e9ec60ad8a'),
    (1, 0.5, 1.5, 5.0, '70490613e197572e5a507d6002c3ae13acbbfae2'),
    (1, 0.5, 0.5, 8.0, 'e436de963fb7dd24a9695eb1b3290cb82e5f5649'),
    (0, 1.5, 0.5, 5.0, '2b33f3d7ed7e83c7cedf8f0298f2f5f1d0f17e42'),
    (0, 0.5, 1.5, 5.0, 'a14afd2b72a2c1c7d11ff55e51c1e1b337e05d86'),
    (0, 0.5, 0.5, 8.0, 'd29a418fc59bf7a880fada2ca262567f67138012'),
])
def test_frangi_enhancement_with_varied_params(aorta_image, image_to_sha,
                                               scaled, alpha, beta, gamma,
                                               expected_sha):
        enhancer = vesselenhancement.vmtkImageVesselEnhancement()
        enhancer.Image = aorta_image
        enhancer.Method = 'frangi'
        enhancer.ScaledVesselness = scaled
        enhancer.Alpha = alpha
        enhancer.Beta = beta
        enhancer.Gamma = gamma
        enhancer.Execute()

        assert image_to_sha(enhancer.Image) == expected_sha


@pytest.mark.parametrize("alpha1,alpha2,expected_sha", [
    (0.5, 4.5, 'f52baecd512dd1bff56b0d868ffeeb2e75230c1d'),
    (1.5, 2.0, '5add639b61224bcfe39ca423774ef28565f0d560'),
    (1.5, 4.5, '25094e53c07ff611577b869609cfe86f369cfa5f'),
])
def test_sato_enhancement_with_varied_params(aorta_image, image_to_sha,
                                             alpha1, alpha2, expected_sha):
        enhancer = vesselenhancement.vmtkImageVesselEnhancement()
        enhancer.Image = aorta_image
        enhancer.Method = 'sato'
        enhancer.Alpha1 = alpha1
        enhancer.Alpha2 = alpha2
        enhancer.Execute()

        assert image_to_sha(enhancer.Image) == expected_sha


@pytest.mark.parametrize("alpha,beta,gamma,c,timestep,epsilon,wstrength,\
                         sensitivity,numiterations,numdiffusioniterations,expected_sha", [
    (1.5, 0.5, 5.0, 1E-6, 1E-2, 1E-2, 25.0, 5.0, 1, 0, '2255cff824da8faecc2cbf78f84add7f143eae48'),
    (0.5, 1.5, 5.0, 1E-6, 1E-2, 1E-2, 25.0, 5.0, 1, 0, '96e467a327a942c45b7b8300f1bbf6cb4a0d2f7c'),
    (0.5, 0.5, 8.0, 1E-6, 1E-2, 1E-2, 25.0, 5.0, 1, 0, 'c055d6275616711ecfffe79b30e018ce6ba0b022'),
    (0.5, 0.5, 5.0, 2E-6, 1E-2, 1E-2, 25.0, 5.0, 1, 0, 'fe9e633c9c8c51d4df5900279bebced78ec2b920'),
    (0.5, 0.5, 5.0, 1E-6, 2E-2, 1E-2, 25.0, 5.0, 1, 0, '4cb30d1cf6fd0318f3796db7c932a5bb5cb0b2a8'),
    (0.5, 0.5, 5.0, 1E-6, 1E-2, 2E-2, 25.0, 5.0, 1, 0, '1c03fe0e0194f29901d07cb4616c0cb0815607c3'),
    (0.5, 0.5, 5.0, 1E-6, 1E-2, 1E-2, 30.0, 5.0, 1, 0, '3314ee4cd89f5da75a98b0698ebe2e721a1ff940'),
    (0.5, 0.5, 5.0, 1E-6, 1E-2, 1E-2, 25.0, 8.0, 1, 0, '0000eafde7ad2437c70823d7b1e0f5941fa33ade'),
    (0.5, 0.5, 5.0, 1E-6, 1E-2, 1E-2, 25.0, 5.0, 3, 0, '8ba93466c690cf64a5205ff60fa4f0490f5cc240'),
    (0.5, 0.5, 5.0, 1E-6, 1E-2, 1E-2, 25.0, 5.0, 1, 1, 'fe9e633c9c8c51d4df5900279bebced78ec2b920'),
])
def test_ved_enhancement_with_varied_params(aorta_image, image_to_sha,
                                            alpha, beta, gamma, c, timestep,
                                            epsilon, wstrength, sensitivity,
                                            numiterations, numdiffusioniterations,
                                            expected_sha):
        enhancer = vesselenhancement.vmtkImageVesselEnhancement()
        enhancer.Image = aorta_image
        enhancer.Method = 'ved'
        enhancer.Alpha = alpha
        enhancer.Beta = beta
        enhancer.Gamma = gamma
        enhancer.C = c
        enhancer.TimeStep = timestep
        enhancer.Epsilon = epsilon
        enhancer.WStrength = wstrength
        enhancer.Sensitivity = sensitivity
        enhancer.NumberOfIterations = numiterations
        enhancer.NumberOfDiffusionSubIterations = numdiffusioniterations
        enhancer.Execute()

        assert image_to_sha(enhancer.Image) == expected_sha