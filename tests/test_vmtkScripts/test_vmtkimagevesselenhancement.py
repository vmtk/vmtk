## Program: VMTK
## Language:  Python
## Date:      January 10, 2018
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga, All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this code was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo

import pytest
import vmtk.vmtkimagevesselenhancement as vesselenhancement


#TODO: VEDM Method is currently diabled in vtkVmtk (vtkvmtkVesselEnhancingDiffusion3DImageFilter). Find a fix or remove.
@pytest.mark.parametrize("enhance_method,paramid", [
    ('frangi', '0'),
    ('sato', '1'),
    ('ved', '2'),
])
def test_enhancement_methods_with_default_params(aorta_image, compare_images,
                                                 enhance_method,
                                                 paramid):
    name = __name__ + '_test_enhancement_methods_with_default_params_' + paramid + '.mha'
    enhancer = vesselenhancement.vmtkImageVesselEnhancement()
    enhancer.Image = aorta_image
    enhancer.Method = enhance_method
    enhancer.Execute()

    assert compare_images(enhancer.Image, name) == True


@pytest.mark.parametrize("scaled,alpha,beta,gamma,paramid", [
    (1, 0.5, 0.5, 5.0, '0'),
    (1, 1.5, 0.5, 5.0, '1'),
    (1, 0.5, 1.5, 5.0, '2'),
    (1, 0.5, 0.5, 8.0, '3'),
    (0, 1.5, 0.5, 5.0, '4'),
    (0, 0.5, 1.5, 5.0, '5'),
    (0, 0.5, 0.5, 8.0, '6'),
])
def test_frangi_enhancement_with_varied_params(aorta_image, compare_images,
                                               scaled, alpha, beta, gamma,
                                               paramid):
    name = __name__ + '_test_frangi_enhancement_with_varied_params_' + paramid + '.mha'
    enhancer = vesselenhancement.vmtkImageVesselEnhancement()
    enhancer.Image = aorta_image
    enhancer.Method = 'frangi'
    enhancer.ScaledVesselness = scaled
    enhancer.Alpha = alpha
    enhancer.Beta = beta
    enhancer.Gamma = gamma
    enhancer.Execute()

    assert compare_images(enhancer.Image, name) == True


@pytest.mark.parametrize("alpha1,alpha2,paramid", [
    (0.5, 4.5, '0'),
    (1.5, 2.0, '1'),
    (1.5, 4.5, '2'),
])
def test_sato_enhancement_with_varied_params(aorta_image, compare_images,
                                             alpha1, alpha2, paramid):
    name = __name__ + '_test_sato_enhancement_with_varied_params_' + paramid + '.mha'
    enhancer = vesselenhancement.vmtkImageVesselEnhancement()
    enhancer.Image = aorta_image
    enhancer.Method = 'sato'
    enhancer.Alpha1 = alpha1
    enhancer.Alpha2 = alpha2
    enhancer.Execute()

    assert compare_images(enhancer.Image, name) == True


@pytest.mark.skip(reason='failing on linux for unknown reason')
@pytest.mark.parametrize("alpha,beta,gamma,c,timestep,epsilon,wstrength,\
                         sensitivity,numiterations,numdiffusioniterations,paramid", [
    (1.5, 0.5, 5.0, 1E-6, 1E-2, 1E-2, 25.0, 5.0, 1, 0, '0'),
    (0.5, 1.5, 5.0, 1E-6, 1E-2, 1E-2, 25.0, 5.0, 1, 0, '1'),
    (0.5, 0.5, 8.0, 1E-6, 1E-2, 1E-2, 25.0, 5.0, 1, 0, '2'),
    (0.5, 0.5, 5.0, 2E-6, 1E-2, 1E-2, 25.0, 5.0, 1, 0, '3'),
    (0.5, 0.5, 5.0, 1E-6, 2E-2, 1E-2, 25.0, 5.0, 1, 0, '4'),
    (0.5, 0.5, 5.0, 1E-6, 1E-2, 2E-2, 25.0, 5.0, 1, 0, '5'),
    (0.5, 0.5, 5.0, 1E-6, 1E-2, 1E-2, 30.0, 5.0, 1, 0, '6'),
    (0.5, 0.5, 5.0, 1E-6, 1E-2, 1E-2, 25.0, 8.0, 1, 0, '7'),
    (0.5, 0.5, 5.0, 1E-6, 1E-2, 1E-2, 25.0, 5.0, 3, 0, '8'),
    (0.5, 0.5, 5.0, 1E-6, 1E-2, 1E-2, 25.0, 5.0, 1, 1, '9'),
])
def test_ved_enhancement_with_varied_params(aorta_image, compare_images,
                                            alpha, beta, gamma, c, timestep,
                                            epsilon, wstrength, sensitivity,
                                            numiterations, numdiffusioniterations,
                                            paramid):
    name = __name__ + '_test_ved_enhancement_with_varied_params_' + paramid + '.mha'
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

    assert compare_images(enhancer.Image, name, tolerance=1.0) == True
