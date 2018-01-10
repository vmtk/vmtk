import pytest
import vmtk.vmtkimagebinarize as binarize

def test_binarize_default_parameters(aorta_image, image_to_sha):
    binarizer = binarize.vmtkImageBinarize()
    binarizer.Image = aorta_image
    binarizer.Execute()

    assert image_to_sha(binarizer.Image) == '3494222d77bf1a4b9be4f709436c6825a29ac181'


def test_binarize_simple_threshold(aorta_image, image_to_sha):
    binarizer = binarize.vmtkImageBinarize()
    binarizer.Image = aorta_image
    binarizer.Threshold = 500
    binarizer.Execute()

    assert image_to_sha(binarizer.Image) == 'b2950d439179f1637b3297df81e3adfcfd878d07'


def test_binarize_modify_lower_label(aorta_image, image_to_sha):
    binarizer = binarize.vmtkImageBinarize()
    binarizer.Image = aorta_image
    binarizer.Threshold = 500
    binarizer.LowerLabel = -1
    binarizer.Execute()

    assert image_to_sha(binarizer.Image) == '553eaffc0a6d617b52bb4a3a9b9bbcc61236424b'


def test_binarize_modify_upper_label(aorta_image, image_to_sha):
    binarizer = binarize.vmtkImageBinarize()
    binarizer.Image = aorta_image
    binarizer.Threshold = 500
    binarizer.UpperLabel = 7
    binarizer.Execute()

    assert image_to_sha(binarizer.Image) == 'd76f020b72fab68adcde0f1b5d269c2b731c3c21'


def test_binarize_modify_upper_and_lower_label(aorta_image, image_to_sha):
    binarizer = binarize.vmtkImageBinarize()
    binarizer.Image = aorta_image
    binarizer.Threshold = 500
    binarizer.UpperLabel = 7
    binarizer.LowerLabel = 3
    binarizer.Execute()

    assert image_to_sha(binarizer.Image) == 'c250c060b43e33f566ccd24d667b6714793cdf7a'