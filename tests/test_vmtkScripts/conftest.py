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

from __future__ import print_function
import pytest
import os
import copy
from hashlib import sha1

import vmtk.vmtkimagereader as imagereader
import vmtk.vmtkimagetonumpy as imagetonumpy
import vmtk.vmtkimagewriter as imagewriter
import vmtk.vmtkimagecompare as imagecompare

import vmtk.vmtksurfacereader as surfacereader
import vmtk.vmtksurfacetonumpy as surfacetonumpy
import vmtk.vmtksurfacecompare as surfacecompare
import vmtk.vmtksurfacewriter as surfacewriter



@pytest.fixture(scope='function')
def input_datadir():
    '''
    returns a path to the vmtk/tests/testData directory
    '''
    try:
        datadir = '@ExternalData_BINARY_ROOT@/tests/data/input'
        if not os.path.isdir(datadir): raise ValueError()
    except ValueError:
        try:
            datadir = '@ExternalData_BINARY_ROOT@'
            datadir = datadir.replace('/work/build/ExternalData', '/test_tmp/build/ExternalData/tests/data/input')
            if not os.path.isdir(datadir): raise ValueError()
        except ValueError:
            datadir = os.path.join(
                os.path.dirname(
                    os.path.dirname(
                        os.path.dirname(
                            os.path.dirname(
                                os.path.realpath(__file__))))),
                'vmtk-test-data/input')
            if not os.path.isdir(datadir): 
                raise ValueError('the vmtk-test-data repository cannot be found at the same level as vmtk. expected it to be at', datadir)
    return datadir


# **************************************************
# Image Functions
# **************************************************


@pytest.fixture(scope='function')
def aorta_image(input_datadir):
    reader = imagereader.vmtkImageReader()
    reader.InputFileName = os.path.join(input_datadir, 'aorta.mha')
    reader.Execute()
    return reader.Image


# this is a hack because pytest doesn't currently let you define functions
# with inputs as fixtures. This way we return a function which accepts the input
# and returns the sha.
@pytest.fixture()
def image_to_sha():
    def make_image_to_sha(image):
        converter = imagetonumpy.vmtkImageToNumpy()
        converter.Image = image
        converter.Execute()
        check = converter.ArrayDict['PointData']['ImageScalars'].copy(order='C')
        return sha1(check).hexdigest()
    return make_image_to_sha


@pytest.fixture()
def write_image():
    def make_write_image(image, filename):
        writer = imagewriter.vmtkImageWriter()
        writer.Image = image
        try:
            datadir = '@ExternalData_BINARY_ROOT@/tests/data/imagereference'
            if not os.path.isdir(datadir): raise ValueError()
        except ValueError:
            try:
                datadir = '@ExternalData_BINARY_ROOT@'
                datadir = datadir.replace('/work/build/ExternalData', '/test_tmp/build/ExternalData/tests/data/imagereference')
                if not os.path.isdir(datadir): raise ValueError()
            except ValueError:
                # should be something like '/Users/rick/projects/vmtk/vmtk-test-data/imagereference'
                # where script is run from '/Users/rick/projects/vmtk/vmtk/tests/test_vmtkScripts' dir
                datadir = os.path.join(
                    os.path.dirname(
                        os.path.dirname(
                            os.path.dirname(
                                os.path.dirname(
                                    os.path.realpath(__file__))))),
                    'vmtk-test-data/imagereference')
                if not os.path.isdir(datadir): 
                    raise ValueError('the vmtk-test-data repository cannot be found at the same level as vmtk. expected it to be at', datadir)
        writer.OutputFileName = os.path.join(datadir, filename)
        writer.Execute()
        return
    return make_write_image


@pytest.fixture()
def compare_images():
    def make_compare_images(image, reference_file, tolerance=0.1):
        reader = imagereader.vmtkImageReader()
        try:
            datadir = '@ExternalData_BINARY_ROOT@/tests/data/imagereference'
            if not os.path.isdir(datadir): raise ValueError()
        except ValueError:
            try:
                datadir = '@ExternalData_BINARY_ROOT@'
                datadir = datadir.replace('/work/build/ExternalData', '/test_tmp/build/ExternalData/tests/data/imagereference')
                if not os.path.isdir(datadir): raise ValueError()
            except ValueError:
                datadir = os.path.join(
                    os.path.dirname(
                        os.path.dirname(
                            os.path.dirname(
                                os.path.dirname(
                                    os.path.realpath(__file__))))),
                    'vmtk-test-data/imagereference')
                if not os.path.isdir(datadir): 
                    raise ValueError('the vmtk-test-data repository cannot be found at the same level as vmtk. expected it to be at', datadir)
        reader.InputFileName = os.path.join(datadir, reference_file)
        reader.Execute()

        comp = imagecompare.vmtkImageCompare()
        comp.Image = image
        comp.ReferenceImage = reader.Image
        comp.Method = 'subtraction'
        comp.Tolerance = tolerance
        comp.Execute()

        return comp.Result
    return make_compare_images


# **************************************************
# Surface Functions
# **************************************************


@pytest.fixture(scope='function')
def aorta_surface(input_datadir):
    reader = surfacereader.vmtkSurfaceReader()
    reader.InputFileName = os.path.join(input_datadir, 'aorta-surface.vtp')
    reader.Execute()
    return reader.Surface


@pytest.fixture(scope='function')
def aorta_surface2(input_datadir):
    reader = surfacereader.vmtkSurfaceReader()
    reader.InputFileName = os.path.join(input_datadir, 'aorta-surface-segment-2.vtp')
    reader.Execute()
    return reader.Surface

@pytest.fixture(scope='function')
def aorta_surface_openends(input_datadir):
    reader = surfacereader.vmtkSurfaceReader()
    reader.InputFileName = os.path.join(input_datadir, 'aorta-surface-open-ends.stl')
    reader.Execute()
    return reader.Surface


@pytest.fixture()
def poly_to_np():
    def make_poly_to_np(surface):
        converter = surfacetonumpy.vmtkSurfaceToNumpy()
        converter.Surface = surface
        converter.Execute()
        return converter.ArrayDict
    return make_poly_to_np


@pytest.fixture()
def write_surface():
    def make_write_surface(surface, filename):
        writer = surfacewriter.vmtkSurfaceWriter()
        writer.Surface = surface
        try:
            datadir = '@ExternalData_BINARY_ROOT@/tests/data/surfacereference'
            if not os.path.isdir(datadir): raise ValueError()
        except ValueError:
            try:
                datadir = '@ExternalData_BINARY_ROOT@'
                datadir = datadir.replace('/work/build/ExternalData', '/test_tmp/build/ExternalData/tests/data/surfacereference')
                if not os.path.isdir(datadir): raise ValueError()
            except ValueError:
                datadir = os.path.join(
                    os.path.dirname(
                        os.path.dirname(
                            os.path.dirname(
                                os.path.dirname(
                                    os.path.realpath(__file__))))),
                    'vmtk-test-data/surfacereference')
                if not os.path.isdir(datadir): 
                    raise ValueError('the vmtk-test-data repository cannot be found at the same level as vmtk. expected it to be at', datadir)
        writer.OutputFileName = os.path.join(datadir, filename)
        writer.Execute()
        return
    return make_write_surface


@pytest.fixture()
def compare_surfaces():
    def make_compare_surface(surface, reference_file, tolerance=0.001, method='distance', arrayname=''):
        reader = surfacereader.vmtkSurfaceReader()
        try:
            datadir = '@ExternalData_BINARY_ROOT@/tests/data/surfacereference'
            if not os.path.isdir(datadir): raise ValueError()
        except ValueError:
            try:
                datadir = '@ExternalData_BINARY_ROOT@'
                datadir = datadir.replace('/work/build/ExternalData', '/test_tmp/build/ExternalData/tests/data/surfacereference')
                if not os.path.isdir(datadir): raise ValueError()
            except ValueError:
                datadir = os.path.join(
                    os.path.dirname(
                        os.path.dirname(
                            os.path.dirname(
                                os.path.dirname(
                                    os.path.realpath(__file__))))),
                    'vmtk-test-data/surfacereference')
                if not os.path.isdir(datadir): 
                    raise ValueError('the vmtk-test-data repository cannot be found at the same level as vmtk. expected it to be at', datadir)
        reader.InputFileName = os.path.join(datadir, reference_file)
        reader.Execute()

        comp = surfacecompare.vmtkSurfaceCompare()
        comp.Surface = surface
        comp.ReferenceSurface = reader.Surface
        comp.Method = method
        comp.ArrayName = arrayname
        comp.Tolerance = tolerance
        comp.Execute()

        return comp.Result
    return make_compare_surface


# *************************************************
# Centerlines
# *************************************************

@pytest.fixture(scope='function')
def aorta_centerline(input_datadir):
    reader = surfacereader.vmtkSurfaceReader()
    reader.InputFileName = os.path.join(input_datadir, 'aorta-centerline.vtp')
    reader.Execute()
    return reader.Surface

@pytest.fixture(scope='function')
def aorta_centerline_branches(input_datadir):
    reader = surfacereader.vmtkSurfaceReader()
    reader.InputFileName = os.path.join(input_datadir, 'aorta-centerline-branches.vtp')
    reader.Execute()
    return reader.Surface

@pytest.fixture()
def write_centerline():
    def make_write_centerline(centerline, filename):
        writer = surfacewriter.vmtkSurfaceWriter()
        writer.Surface = centerline
        try:
            datadir = '@ExternalData_BINARY_ROOT@/tests/data/centerlinereference'
            if not os.path.isdir(datadir): raise ValueError()
        except ValueError:
            try:
                datadir = '@ExternalData_BINARY_ROOT@'
                datadir = datadir.replace('/work/build/ExternalData', '/test_tmp/build/ExternalData/tests/data/centerlinereference')
                if not os.path.isdir(datadir): raise ValueError()
            except ValueError:
                datadir = os.path.join(
                    os.path.dirname(
                        os.path.dirname(
                            os.path.dirname(
                                os.path.dirname(
                                    os.path.realpath(__file__))))),
                    'vmtk-test-data/centerlinereference')
                if not os.path.isdir(datadir): 
                    raise ValueError('the vmtk-test-data repository cannot be found at the same level as vmtk. expected it to be at', datadir)
        writer.OutputFileName = os.path.join(datadir, filename)
        writer.Execute()
        return
    return make_write_centerline


@pytest.fixture()
def compare_centerlines():
    def make_compare_centerline(centerline, reference_file, tolerance=0.001, method='distance', arrayname=''):
        reader = surfacereader.vmtkSurfaceReader()
        try:
            datadir = '@ExternalData_BINARY_ROOT@/tests/data/centerlinereference'
            if not os.path.isdir(datadir): raise ValueError()
        except ValueError:
            try:
                datadir = '@ExternalData_BINARY_ROOT@'
                datadir = datadir.replace('/work/build/ExternalData', '/test_tmp/build/ExternalData/tests/data/centerlinereference')
                if not os.path.isdir(datadir): raise ValueError()
            except ValueError:
                datadir = os.path.join(
                    os.path.dirname(
                        os.path.dirname(
                            os.path.dirname(
                                os.path.dirname(
                                    os.path.realpath(__file__))))),
                    'vmtk-test-data/centerlinereference')
                if not os.path.isdir(datadir): 
                    raise ValueError('the vmtk-test-data repository cannot be found at the same level as vmtk. expected it to be at', datadir)
        reader.InputFileName = os.path.join(datadir, reference_file)
        reader.Execute()

        comp = surfacecompare.vmtkSurfaceCompare()
        comp.Surface = centerline
        comp.ReferenceSurface = reader.Surface
        comp.Method = method
        comp.ArrayName = arrayname
        comp.Tolerance = tolerance
        comp.Execute()

        return comp.Result
    return make_compare_centerline


# **************************************************
# Misc Functions
# **************************************************


@pytest.fixture(scope='function')
def vmtk_scripts():
    allscripts =  [
        'vmtk.vmtkactivetubes',
        'vmtk.vmtkbifurcationprofiles',
        'vmtk.vmtkbifurcationreferencesystems',
        'vmtk.vmtkbifurcationsections',
        'vmtk.vmtkbifurcationvectors',
        'vmtk.vmtkboundarylayer',
        'vmtk.vmtkboundaryreferencesystems',
        'vmtk.vmtkbranchclipper',
        'vmtk.vmtkbranchextractor',
        'vmtk.vmtkbranchgeometry',
        'vmtk.vmtkbranchmapping',
        'vmtk.vmtkbranchmetrics',
        'vmtk.vmtkbranchpatching',
        'vmtk.vmtkbranchsections',
        'vmtk.vmtkcenterlineattributes',
        'vmtk.vmtkcenterlinegeometry',
        'vmtk.vmtkcenterlineinterpolation',
        'vmtk.vmtkcenterlinelabeler',
        'vmtk.vmtkcenterlinemerge',
        'vmtk.vmtkcenterlinemodeller',
        'vmtk.vmtkcenterlineoffsetattributes',
        'vmtk.vmtkcenterlineresampling',
        'vmtk.vmtkcenterlines',
        'vmtk.vmtkcenterlinestonumpy',
        'vmtk.vmtkcenterlinesections',
        'vmtk.vmtkcenterlinesmoothing',
        'vmtk.vmtkcenterlineviewer',
        'vmtk.vmtkdelaunayvoronoi',
        'vmtk.vmtkdistancetocenterlines',
        'vmtk.vmtkendpointextractor',
        'vmtk.vmtkflowextensions',
        'vmtk.vmtkicpregistration',
        'vmtk.vmtkimagebinarize',
        'vmtk.vmtkimagecast',
        'vmtk.vmtkimagecompose',
        'vmtk.vmtkimagecurvedmpr',
        'vmtk.vmtkimagefeaturecorrection',
        'vmtk.vmtkimagefeatures',
        'vmtk.vmtkimageinitialization',
        'vmtk.vmtkimagemipviewer',
        'vmtk.vmtkimagemorphology',
        'vmtk.vmtkimagenormalize',
        'vmtk.vmtkimageobjectenhancement',
        'vmtk.vmtkimageotsuthresholds',
        'vmtk.vmtkimagereader',
        'vmtk.vmtkimagereslice',
        'vmtk.vmtkimageseeder',
        'vmtk.vmtkimageshiftscale',
        'vmtk.vmtkimagesmoothing',
        'vmtk.vmtkimagetonumpy',
        'vmtk.vmtkimageviewer',
        'vmtk.vmtkimagevesselenhancement',
        'vmtk.vmtkimagevoipainter',
        'vmtk.vmtkimagevoiselector',
        'vmtk.vmtkimagewriter',
        'vmtk.vmtklevelsetsegmentation',
        'vmtk.vmtklineartoquadratic',
        'vmtk.vmtklineresampling',
        'vmtk.vmtklocalgeometry',
        'vmtk.vmtkmarchingcubes',
        'vmtk.vmtkmesharrayoperation',
        'vmtk.vmtkmeshboundaryinspector',
        'vmtk.vmtkmeshbranchclipper',
        'vmtk.vmtkmeshclipper',
        'vmtk.vmtkmeshconnectivity',
        'vmtk.vmtkmeshcutter',
        'vmtk.vmtkmeshdatareader',
        'vmtk.vmtkmeshextractpointdata',
        'vmtk.vmtkmeshlambda2',
        'vmtk.vmtkmeshlinearize',
        'vmtk.vmtkmeshgenerator',
        'vmtk.vmtkmeshmergetimesteps',
        'vmtk.vmtkmeshpolyballevaluation',
        'vmtk.vmtkmeshprojection',
        'vmtk.vmtkmeshreader',
        'vmtk.vmtkmeshscaling',
        'vmtk.vmtkmeshtetrahedralize',
        'vmtk.vmtkmeshtosurface',
        'vmtk.vmtkmeshtransform',
        'vmtk.vmtkmeshtransformtoras',
        'vmtk.vmtkmeshvectorfromcomponents',
        'vmtk.vmtkmeshviewer',
        'vmtk.vmtkmeshvolume',
        'vmtk.vmtkmeshvorticityhelicity',
        'vmtk.vmtkmeshwallshearrate',
        'vmtk.vmtkmeshwriter',
        'vmtk.vmtknetworkeditor',
        'vmtk.vmtknetworkextraction',
        'vmtk.vmtknetworkwriter',
        'vmtk.vmtknumpyreader',
        'vmtk.vmtknumpytocenterlines',
        'vmtk.vmtknumpytoimage',
        'vmtk.vmtknumpytosurface',
        'vmtk.vmtknumpywriter',
        'vmtk.vmtkparticletracer',
        'vmtk.vmtkpathlineanimator',
        'vmtk.vmtkpointsplitextractor',
        'vmtk.vmtkpointtransform',
        'vmtk.vmtkpolyballmodeller',
        'vmtk.vmtkpotentialfit',
        'vmtk.vmtkpythonscript',
        'vmtk.vmtkrenderer',
        'vmtk.vmtkrendertoimage',
        'vmtk.vmtkrbfinterpolation',
        'vmtk.vmtksurfaceappend',
        'vmtk.vmtksurfacearraysmoothing',
        'vmtk.vmtksurfacearrayoperation',
        'vmtk.vmtksurfacebooleanoperation',
        'vmtk.vmtksurfacecapper',
        'vmtk.vmtksurfacecelldatatopointdata',
        'vmtk.vmtksurfacecenterlineprojection',
        'vmtk.vmtksurfaceclipper',
        'vmtk.vmtksurfacecliploop',
        'vmtk.vmtksurfaceconnectivity',
        'vmtk.vmtksurfacecurvature',
        'vmtk.vmtksurfacedecimation',
        'vmtk.vmtksurfacedistance',
        'vmtk.vmtksurfaceendclipper',
        'vmtk.vmtksurfacekiteremoval',
        'vmtk.vmtksurfaceloopextraction',
        'vmtk.vmtksurfacemassproperties',
        'vmtk.vmtksurfacemodeller',
        'vmtk.vmtksurfacenormals',
        'vmtk.vmtksurfacepointdatatocelldata',
        'vmtk.vmtksurfacepolyballevaluation',
        'vmtk.vmtksurfaceprojection',
        'vmtk.vmtksurfacereader',
        'vmtk.vmtksurfacereferencesystemtransform',
        'vmtk.vmtksurfaceregiondrawing',
        'vmtk.vmtksurfaceremeshing',
        'vmtk.vmtksurfacescaling',
        'vmtk.vmtksurfacesmoothing',
        'vmtk.vmtksurfacesubdivision',
        'vmtk.vmtksurfacetonumpy',
        'vmtk.vmtksurfacetransform',
        'vmtk.vmtksurfacetransforminteractive',
        'vmtk.vmtksurfacetransformtoras',
        'vmtk.vmtksurfacetriangle',
        'vmtk.vmtksurfacetomesh',
        'vmtk.vmtksurfaceviewer',
        'vmtk.vmtksurfacewriter',
        'vmtk.vmtksurfmesh',
        'vmtk.vmtktetgen',
        'vmtk.vmtktetringenerator',
        'vmtk.vmtkboundarylayer2',
        'vmtk.vmtkcenterlinestonumpy',
        'vmtk.vmtkdijkstradistancetopoints',
        'vmtk.vmtkdistancetospheres',
        'vmtk.vmtkentityrenumber',
        'vmtk.vmtkgeodesicsurfaceresolution',
        'vmtk.vmtkimagetonumpy',
        'vmtk.vmtkmeshaddexternallayer',
        'vmtk.vmtkmeshclipcenterlines',
        'vmtk.vmtkmeshmerge',
        'vmtk.vmtkmeshtetrahedralize2',
        'vmtk.vmtkmeshviewer2',
        'vmtk.vmtkmeshwriter2',
        'vmtk.vmtknumpyreader',
        'vmtk.vmtknumpytocenterlines',
        'vmtk.vmtknumpytoimage',
        'vmtk.vmtknumpytosurface',
        'vmtk.vmtknumpywriter',
        'vmtk.vmtksurfaceextractinnercylinder',
        'vmtk.vmtksurfaceresolution',
        'vmtk.vmtksurfacetonumpy',
        'vmtk.vmtksurfacewriter2',
        'vmtk.vmtkthreshold' ]
    return allscripts