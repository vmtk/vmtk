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
import vmtk.vmtkimageotsuthresholds as otsu
import vtk


@pytest.mark.skipif(vtk.vtkVersion.GetVTKMajorVersion() > 8, reason="requires vtk 8 or lower")
@pytest.mark.parametrize("thresholds,paramid", [
    (1, '0'),
    (2, '1'),
    (3, '2'),
])
def test_otsu_image_multiple_thresholds_vtk8(aorta_image, compare_images, thresholds, paramid):
    name = __name__ + '_test_otsu_image_multiple_thresholds_' + paramid + '.mha'
    otsuer = otsu.vmtkImageOtsuThresholds()
    otsuer.Image = aorta_image
    otsuer.NumberOfThresholds = thresholds
    otsuer.Execute()

    assert compare_images(otsuer.Image, name) == True


@pytest.mark.skipif(vtk.vtkVersion.GetVTKMajorVersion() > 8, reason="requires vtk 8 or lower")
def test_otsu_image_256_histogram_bins_vtk8(aorta_image, compare_images):
    name = __name__ + '_test_otsu_image_256_histogram_bins.mha'
    otsuer = otsu.vmtkImageOtsuThresholds()
    otsuer.Image = aorta_image
    otsuer.NumberOfHistogramBins = 256
    otsuer.Execute()

    assert compare_images(otsuer.Image, name) == True


@pytest.mark.skipif(vtk.vtkVersion.GetVTKMajorVersion() < 9, reason="requires vtk 9 or higher")
@pytest.mark.parametrize("thresholds,paramid", [
    (1, '0'),
    (2, '1'),
    (3, '2'),
])
def test_otsu_image_multiple_thresholds_vtk9(aorta_image, compare_images, thresholds, paramid):
    name = __name__ + '_test_otsu_image_multiple_thresholds_' + paramid + '_vtk9.mha'
    otsuer = otsu.vmtkImageOtsuThresholds()
    otsuer.Image = aorta_image
    otsuer.NumberOfThresholds = thresholds
    otsuer.Execute()

    assert compare_images(otsuer.Image, name) == True


@pytest.mark.skipif(vtk.vtkVersion.GetVTKMajorVersion() < 9, reason="requires vtk 9 or higher")
def test_otsu_image_256_histogram_bins_vtk9(aorta_image, compare_images):
    name = __name__ + '_test_otsu_image_256_histogram_bins_vtk9.mha'
    otsuer = otsu.vmtkImageOtsuThresholds()
    otsuer.Image = aorta_image
    otsuer.NumberOfHistogramBins = 256
    otsuer.Execute()

    assert compare_images(otsuer.Image, name) == True
