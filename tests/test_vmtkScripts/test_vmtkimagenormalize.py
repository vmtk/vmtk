import pytest
import vmtk.vmtkimagenormalize as norm

def test_normalize_image(aorta_image, image_to_sha):
    normer = norm.vmtkImageNormalize()
    normer.Image = aorta_image
    normer.Execute()

    assert image_to_sha(normer.Image) == '0e25a160968487bf9dc9a7217fe9fdb43a96d6f9'