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
import os
import vmtk.vmtkimagereader as imagereader
import vmtk.vmtkimagetonumpy as imagetonumpy
from hashlib import sha1
import copy


@pytest.fixture(scope='function')
def test_data():
    '''
    returns a path to the vmtk/tests/testData directory
    '''
    cwd = os.path.dirname(os.path.abspath(__file__))
    datadir = os.path.join(os.path.dirname(cwd), 'testData')
    return datadir


@pytest.fixture(scope='function')
def aorta_image(test_data):
    reader = imagereader.vmtkImageReader()
    reader.InputFileName = os.path.join(test_data, 'aorta.mha')
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
def fast_marching_source_points():
    # this data was pulled from the aorta_image data set manually.
    return [71, 213, 16]


@pytest.fixture()
def fast_marching_target_points():
    # this data was pulled from the aorta_image data set manually.
    return [58, 78, 22,
            96, 78, 21]


@pytest.fixture()
def colliding_fronts_source_points():
    # this data was pulled from the aorta_image data set manually.
    return [71, 213, 16]


@pytest.fixture()
def colliding_fronts_target_points():
    # this data was pulled from the aorta_image data set manually.
    return [58, 78, 22]


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