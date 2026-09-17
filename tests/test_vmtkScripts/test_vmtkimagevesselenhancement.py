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
import vtk
import vmtk.vmtkimagevesselenhancement as vesselenhancement


#TODO: VEDM Method is currently disabled in vtkVmtk (vtkvmtkVesselEnhancingDiffusion3DImageFilter). Find a fix or remove.
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


# Voxel index bounds of the region of aorta.mha around the aorta bifurcation
# that VED runs on. Each VED iteration takes about a second on the whole image,
# so the 20 and 40 iterations below took 20 to 75 seconds per test on it.
VED_REGION = [50, 101, 100, 160, 0, 33]


def image_region(image, voi):
    '''The voxels of image within voi, with the extent starting at 0 and the origin
    moved so that they keep their positions.'''
    extract = vtk.vtkExtractVOI()
    extract.SetInputData(image)
    extract.SetVOI(*voi)
    extract.Update()
    region = vtk.vtkImageData()
    region.DeepCopy(extract.GetOutput())
    region.SetExtent(0, voi[1] - voi[0], 0, voi[3] - voi[2], 0, voi[5] - voi[4])
    region.SetOrigin([image.GetOrigin()[i] + voi[2 * i] * image.GetSpacing()[i] for i in range(3)])
    return region


@pytest.fixture(scope='module')
def aorta_image_region(aorta_image):
    return image_region(aorta_image, VED_REGION)


# VED uses an explicit anisotropic-diffusion time-stepping scheme whose stable
# time step scales inversely with the diffusion-tensor eigenvalue amplification
# (~WStrength). The previous parameters (WStrength=25, NumberOfIterations=1)
# sat right at the stability edge: they barely diffused the image (output was
# nearly identical to the input, so the test exercised almost nothing) and any
# stronger setting rang and drove voxels negative. These three sets stay well
# inside the stable regime (moderate WStrength, TimeStep=0.04) while running
# enough iterations to produce clear, artifact-free smoothing (17-24% mean
# change within the region, output stays non-negative). Mild -> strong; about
# 1-2 seconds per test on the region.
@pytest.mark.parametrize("alpha,beta,gamma,c,timestep,epsilon,wstrength,\
                         sensitivity,numiterations,numdiffusioniterations,paramid", [
    (0.5, 0.5, 5.0, 1E-6, 4E-2, 1E-2, 5.0, 5.0, 20, 0, '0'),
    (0.5, 0.5, 5.0, 1E-6, 4E-2, 1E-2, 5.0, 5.0, 40, 0, '1'),
    (0.5, 0.5, 5.0, 1E-6, 4E-2, 1E-2, 8.0, 5.0, 40, 0, '2'),
])
def test_ved_enhancement_with_varied_params(aorta_image_region, compare_images,
                                            alpha, beta, gamma, c, timestep,
                                            epsilon, wstrength, sensitivity,
                                            numiterations, numdiffusioniterations,
                                            paramid):
    name = __name__ + '_test_ved_enhancement_with_varied_params_' + paramid + '.mha'
    enhancer = vesselenhancement.vmtkImageVesselEnhancement()
    enhancer.Image = aorta_image_region
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
