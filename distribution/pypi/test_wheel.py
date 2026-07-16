#!/usr/bin/env python
"""Smoke test for the vmtk binary wheel.

Run against an installed vmtk wheel (cibuildwheel runs it as the test
command for every built wheel). It verifies that:

  * the wrapped vtkvmtk extension modules load (which exercises the
    linkage against the VTK libraries of the "vtk" wheel),
  * the ITK-based segmentation classes work (which exercises the ITK
    libraries, statically linked on Windows and bundled on Linux/macOS),
  * the pype mechanism and the vmtk script modules import.

No rendering window is created, so the test also runs on headless CI
machines.
"""

import sys


def test_import():
    import vmtk
    from vmtk import vtkvmtk
    print("vmtk package:", vmtk.__file__)

    # One class from each of the main wrapped kits.
    for class_name in [
        "vtkvmtkCenterlineSmoothing",       # ComputationalGeometry
        "vtkvmtkPolyDataSurfaceRemeshing",  # Misc
        "vtkvmtkCapPolyData",               # ComputationalGeometry/Misc
        "vtkvmtkGradientMagnitudeImageFilter",  # Segmentation (ITK-based)
    ]:
        cls = getattr(vtkvmtk, class_name, None)
        if cls is None:
            # Not all classes exist in all configurations; require at least
            # the ITK-based one below.
            print("note: class not found:", class_name)
            continue
        instance = cls()
        print("instantiated", instance.GetClassName())

    # TetGen is excluded from PyPI packages for license reasons (see
    # distribution/pypi/README.md); make sure it did not sneak in.
    import os
    if os.getenv("VMTK_BUILD_TETGEN", "OFF").upper() not in ("ON", "1", "TRUE"):
        assert not hasattr(vtkvmtk, "vtkvmtkTetGenWrapper"), (
            "vtkvmtkTetGenWrapper found in the wheel although TetGen "
            "must not be distributed on PyPI (license terms)"
        )
        print("TetGen correctly excluded")


def test_itk_filter():
    """Run an ITK-based filter end to end on a small synthetic image."""
    import vtk
    from vmtk import vtkvmtk

    source = vtk.vtkImageEllipsoidSource()
    source.SetWholeExtent(0, 15, 0, 15, 0, 15)
    source.SetCenter(8, 8, 8)
    source.SetRadius(4, 4, 4)
    source.SetInValue(100.0)
    source.SetOutValue(0.0)
    source.SetOutputScalarTypeToFloat()
    source.Update()

    gradient = vtkvmtk.vtkvmtkGradientMagnitudeImageFilter()
    gradient.SetInputConnection(source.GetOutputPort())
    gradient.Update()

    scalar_range = gradient.GetOutput().GetScalarRange()
    print("gradient magnitude range:", scalar_range)
    assert scalar_range[1] > 0.0, "ITK gradient magnitude produced no output"


def test_pypes():
    from vmtk import pypes, pypemain, vmtkscripts  # noqa: F401

    pipe = pypes.Pype()
    pipe.ExitOnError = 0
    pipe.Arguments = ["vmtkimagereader", "--help"]
    pipe.ParseArguments()
    print("pype parsed arguments OK")

    # joblib is an optional dependency: vmtkcenterlinesnetwork must import
    # and construct with or without it (serial fallback).
    from vmtk import vmtkcenterlinesnetwork
    script = vmtkcenterlinesnetwork.vmtkCenterlinesNetwork()
    try:
        import joblib  # noqa: F401
        have_joblib = True
    except ImportError:
        have_joblib = False
    print(f"vmtkcenterlinesnetwork OK (joblib installed: {have_joblib}, "
          f"UseJoblib: {bool(script.UseJoblib)})")


def main():
    test_import()
    test_itk_filter()
    test_pypes()
    print("vmtk wheel smoke test passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
