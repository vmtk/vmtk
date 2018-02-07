## Program: VMTK
## Language:  Python
## Date:      February 2, 2018
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
import vmtk.vmtkbranchclipper as branchclipper
from vtk.numpy_interface import dataset_adapter as dsa 
import numpy as np

def test_clip_returns_3_groups(aorta_centerline_branches, aorta_surface):
    clipper = branchclipper.vmtkBranchClipper()
    clipper.Centerlines = aorta_centerline_branches
    clipper.Surface = aorta_surface
    clipper.RadiusArrayName = 'MaximumInscribedSphereRadius'
    clipper.BlankingArrayName = 'Blanking'
    clipper.GroupIdsArrayName = 'GroupIds'
    clipper.Execute()

    wrapedclip = dsa.WrapDataObject(clipper.Surface)
    uniqueGroups = np.unique(np.array(wrapedclip.PointData['GroupIds'].tolist()))

    assert np.allclose(uniqueGroups, np.array([0, 2, 3])) == True


@pytest.mark.parametrize("groupids,paramid", [
    ([0], '0'),
    ([2], '2'),
    ([3], '3'),
    ([0, 2], '02'),
    ([2, 3], '23'),
    ([0, 2, 3], '023')
])
def test_clip_groupids_surface_returns_as_expected(aorta_centerline_branches, aorta_surface, 
                                                  groupids, paramid, compare_surfaces):
    name = __name__ + '_test_clip_group_' + paramid + '_surface_returns_as_expected.vtp'
    clipper = branchclipper.vmtkBranchClipper()
    clipper.Centerlines = aorta_centerline_branches
    clipper.Surface = aorta_surface
    clipper.RadiusArrayName = 'MaximumInscribedSphereRadius'
    clipper.BlankingArrayName = 'Blanking'
    clipper.GroupIdsArrayName = 'GroupIds'
    clipper.GroupIds = groupids
    clipper.Execute()

    assert compare_surfaces(clipper.Surface, name) == True


@pytest.mark.parametrize("groupids,paramid", [
    ([0], '0'),
    ([2], '2'),
    ([3], '3')
])
def test_clip_groupids_with_insideout_surface_returns_as_expected(aorta_centerline_branches, aorta_surface, 
                                                                 groupids, paramid, compare_surfaces):
    name = __name__ + '_test_clip_groupids_' + paramid + '_with_insideout_surface_returns_as_expected.vtp'
    clipper = branchclipper.vmtkBranchClipper()
    clipper.Centerlines = aorta_centerline_branches
    clipper.Surface = aorta_surface
    clipper.RadiusArrayName = 'MaximumInscribedSphereRadius'
    clipper.BlankingArrayName = 'Blanking'
    clipper.GroupIdsArrayName = 'GroupIds'
    clipper.GroupIds = groupids
    clipper.InsideOut = 1
    clipper.Execute()

    assert compare_surfaces(clipper.Surface, name, method="addpointarray", arrayname="ClippingArray") == True
