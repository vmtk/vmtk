## Program: VMTK
## Language:  Python
## Date:      February 12, 2018
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
import os
import vmtk.vmtkbranchmetrics as branchmetrics
import vmtk.vmtksurfacereader as surfacereader
from vtk.numpy_interface import dataset_adapter as dsa
import numpy as np


@pytest.fixture(scope='module')
def name():
    return __name__ + '_branch_metrics_surf.vtp'


@pytest.fixture(scope='module')
def centerline_attribute_branched(input_datadir):
    reader = surfacereader.vmtkSurfaceReader()
    reader.InputFileName = os.path.join(input_datadir, 'aorta-centerline-attribute-branches.vtp')
    reader.Execute()
    return reader.Surface


@pytest.fixture(scope='module')
def branch_metrics_surf(centerline_attribute_branched, aorta_surface_branches, name):
    metrics = branchmetrics.vmtkBranchMetrics()
    metrics.Surface = aorta_surface_branches
    metrics.Centerlines = centerline_attribute_branched
    metrics.AbscissasArrayName = 'Abscissas'
    metrics.NormalsArrayName = 'ParallelTransportNormals'
    metrics.RadiusArrayName = 'MaximumInscribedSphereRadius'
    metrics.GroupIdsArrayName = 'GroupIds'
    metrics.CenterlineIdsArrayName = 'CenterlineIds'
    metrics.TractIdsArrayName = 'TractIds'
    metrics.BlankingArrayName = 'Blanking'
    metrics.Execute()

    return metrics.Surface


def test_abscissa_branch_metric(branch_metrics_surf, compare_surfaces, name):
    assert compare_surfaces(branch_metrics_surf, name, method='addpointarray', arrayname='AbscissaMetric') == True


def test_angular_branch_metric(branch_metrics_surf, compare_surfaces, name):
    assert compare_surfaces(branch_metrics_surf, name, method='addpointarray', arrayname='AngularMetric') == True
