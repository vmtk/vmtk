## Program: VMTK
## Language:  Python
## Date:      February 7, 2018
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
import vmtk.vmtkcenterlineoffsetattributes as centerlineoffsetattributes
import vmtk.vmtksurfacereader as surfacereader
import os
from vtk.numpy_interface import dataset_adapter as dsa
import numpy as np


@pytest.fixture(scope='module')
def reference_system(input_datadir):
    reader = surfacereader.vmtkSurfaceReader()
    reader.InputFileName = os.path.join(input_datadir, 'aorta-centerline-referencesystem.vtp')
    reader.Execute()
    return reader.Surface


@pytest.fixture(scope='module')
def centerline_attribute_branched(input_datadir):
    reader = surfacereader.vmtkSurfaceReader()
    reader.InputFileName = os.path.join(input_datadir, 'aorta-centerline-attribute-branches.vtp')
    reader.Execute()
    return reader.Surface


def test_compare_offset_abscissa(reference_system, centerline_attribute_branched, compare_centerlines):
    name = __name__ + '_test_compare_offset_abscissa.vtp'
    offset = centerlineoffsetattributes.vmtkCenterlineOffsetAttributes()
    offset.Centerlines = centerline_attribute_branched
    offset.ReferenceSystems = reference_system
    offset.AbscissasArrayName = "Abscissas"
    offset.NormalsArrayName = "ParallelTransportNormals"
    offset.GroupIdsArrayName = "GroupIds"
    offset.CenterlineIdsArrayName = "CenterlineIds"
    offset.ReferenceSystemsNormalArrayName = "Normal"
    offset.OffsetAbscissasArrayName = "OffsetAbscissas"
    offset.OffsetNormalsArrayName = "OffsetNormals"
    offset.ReferenceGroupId = 1
    offset.Execute()

    assert compare_centerlines(offset.Centerlines, name, method='addpointarray', arrayname='Abscissas') == True


def test_compare_offset_abscissa_and_normal_without_inplace_modification(reference_system,
                                                                        centerline_attribute_branched,
                                                                        compare_centerlines):
    name = __name__ + '_test_compare_offset_abscissa_and_normal_without_inplace_modification.vtp'
    offset = centerlineoffsetattributes.vmtkCenterlineOffsetAttributes()
    offset.Centerlines = centerline_attribute_branched
    offset.ReferenceSystems = reference_system
    offset.AbscissasArrayName = "Abscissas"
    offset.NormalsArrayName = "ParallelTransportNormals"
    offset.GroupIdsArrayName = "GroupIds"
    offset.CenterlineIdsArrayName = "CenterlineIds"
    offset.ReferenceSystemsNormalArrayName = "Normal"
    offset.OffsetAbscissasArrayName = "OffsetAbscissas"
    offset.OffsetNormalsArrayName = "OffsetNormals"
    offset.ReferenceGroupId = 1
    offset.ReplaceAttributes = 0
    offset.Execute()

    assert compare_centerlines(offset.Centerlines, name, method='addpointarray', arrayname='OffsetAbscissas') == True
    assert compare_centerlines(offset.Centerlines, name, method='addpointarray', arrayname='OffsetNormals') == True
