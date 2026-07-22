## Program: VMTK
## Language:  Python

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

import pytest
from vmtk import vmtkcontribscripts


def test_boundary_edges_of_closed_surface_are_empty(aorta_surface):
    # aorta-surface.vtp is a closed surface, so it has no boundary edges
    edges = vmtkcontribscripts.vmtkSurfaceFeatureEdges()
    edges.Surface = aorta_surface
    edges.BoundaryEdges = 1
    edges.FeatureEdges = 0
    edges.NonManifoldEdges = 0
    edges.Execute()

    assert edges.Surface.GetNumberOfCells() == 0


def test_boundary_edges_of_open_surface_are_nonempty(aorta_surface_openends):
    # aorta-surface-open-ends.stl has open profiles, hence boundary edges
    edges = vmtkcontribscripts.vmtkSurfaceFeatureEdges()
    edges.Surface = aorta_surface_openends
    edges.BoundaryEdges = 1
    edges.FeatureEdges = 0
    edges.NonManifoldEdges = 0
    edges.Execute()

    assert edges.Surface.GetNumberOfCells() > 0


def test_feature_edges_output_is_lines(aorta_surface):
    import vtk
    edges = vmtkcontribscripts.vmtkSurfaceFeatureEdges()
    edges.Surface = aorta_surface
    edges.BoundaryEdges = 0
    edges.FeatureEdges = 1
    edges.FeatureAngle = 30
    edges.Execute()

    out = edges.Surface
    if out.GetNumberOfCells() > 0:
        types = set(out.GetCellType(i) for i in range(out.GetNumberOfCells()))
        assert types.issubset({vtk.VTK_LINE, vtk.VTK_POLY_LINE})
