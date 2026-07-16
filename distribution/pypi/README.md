# Building vmtk packages for PyPI

This directory, together with `setup.py` / `pyproject.toml` / `MANIFEST.in`
at the repository root and the `VMTK_WHEEL_BUILD` option in the top-level
`CMakeLists.txt`, provides everything needed to build vmtk source and binary
packages for publication on [PyPI](https://pypi.org).

The approach is modeled on how VTK remote modules are packaged as wheels,
see [KitwareMedical/VTKExternalModule](https://github.com/KitwareMedical/VTKExternalModule)
and [Korijn/VTKU3DExporter](https://github.com/Korijn/VTKU3DExporter).

## How it works

* **VTK**: the wheels are compiled against the *VTK wheel SDK* that Kitware
  publishes for every VTK release (e.g.
  `https://vtk.org/files/release/9.6/vtk-wheel-sdk-9.6.2-cp312-cp312-win_amd64.tar.xz`).
  The SDK contains the headers, CMake configuration and link libraries that
  exactly match the `vtk` wheels on PyPI, so the resulting `vmtk` wheel
  declares `vtk==<version>` as an install requirement and shares the VTK
  libraries with it at runtime (no VTK libraries are shipped in the vmtk
  wheel). `setup.py` downloads the SDK automatically into `_deps/`.

* **ITK**: ITK does not publish an SDK for its PyPI wheels, so `setup.py`
  clones and builds ITK once into `_deps/` (reused across Python versions
  and cached in CI). On Windows ITK is linked statically. On Linux/macOS a
  single shared ITK is required (with static ITK each vmtk library would get
  its own copy of the ITK object-factory registry and image IO would break),
  and the wheel repair step (`auditwheel` / `delocate`) bundles the ITK
  runtime libraries into the wheel.

* **Layout**: with `-DVMTK_WHEEL_BUILD:BOOL=ON` the CMake install rules
  place the pure Python modules (PypeS, vmtkScripts), the wrapped extension
  modules and the vmtk shared libraries side by side in a single `vmtk`
  package directory. The libraries are built with relative rpaths
  (`$ORIGIN`, `$ORIGIN/../vtkmodules` on Linux; `@loader_path` equivalents
  on macOS) so that they resolve the vmtk and VTK libraries from the
  installed package locations. On Windows, the wheel-specific
  `vmtk/__init__.py` (installed from `distribution/pypi/package_init.py`)
  registers the required DLL directories. Headers, CMake config files,
  static/import libraries and the per-script launcher scripts are excluded
  from the wheel; a `vmtk` console entry point (`vmtk.pypemain:main`,
  declared in `setup.py`) replaces the classic launcher script.

* **CI**: `.github/workflows/build-pypi-packages.yml` builds the sdist and
  wheels for Linux (x86_64 + aarch64), Windows and macOS (x86_64 + arm64)
  for CPython 3.9–3.13 with [cibuildwheel](https://cibuildwheel.pypa.io),
  runs `distribution/pypi/test_wheel.py` against every wheel, and publishes
  to PyPI via [trusted publishing](https://docs.pypi.org/trusted-publishers/)
  when a GitHub release with a `v*` tag is published (the `pypi` GitHub
  environment and the PyPI trusted publisher must be configured once
  before the first release).

## Building locally

Prerequisites: a C++ compiler (MSVC on Windows), git, and Python ≥ 3.9.
CMake and Ninja are installed automatically as build requirements.

```sh
# Binary wheel for the current Python (also builds ITK on first run, which
# takes a while; the result is reused from _deps/ afterwards)
pip wheel . -w dist -v

# Source distribution
pip install build
python -m build --sdist
```

Useful environment variables (see the `setup.py` docstring for the full
list): `VMTK_VTK_VERSION`, `VTK_WHEEL_SDK_PATH`, `VMTK_ITK_VERSION`,
`ITK_DIR`, `VMTK_ITK_SHARED`, `VMTK_BUILD_TETGEN`, `Python3_EXECUTABLE`.

On Linux/macOS a locally built (unrepaired) wheel resolves the shared ITK
libraries through an absolute rpath pointing at the ITK build tree in
`_deps/`; run `auditwheel repair --exclude "libvtk*"` (Linux) or
`delocate-wheel --exclude libvtk` (macOS) to bundle ITK into the wheel and
make it relocatable, exactly as the CI workflow does.

Test an installed wheel with:

```sh
pip install dist/vmtk-*.whl
python distribution/pypi/test_wheel.py
```

## Releases and pre-releases (beta builds)

The package version is read from `set(VMTK_VERSION_MAJOR/MINOR/PATCH ...)`
in the top-level CMakeLists.txt. GitHub releases only trigger publication
(publishing a release whose tag starts with `v` runs the build and upload
jobs); the tag name does not determine the version, so keep it consistent
with CMakeLists.txt by convention.

For a beta/pre-release, set the suffix next to the version numbers in
CMakeLists.txt, e.g.

```cmake
set(VMTK_VERSION_SUFFIX "b1")   # -> packages versioned 1.5.0b1
```

commit, and publish a GitHub release with a matching tag (e.g.
`v1.5.0b1`). Suffixes follow
[PEP 440](https://peps.python.org/pep-0440/): `a1` (alpha), `b1` (beta),
`rc1` (release candidate), `.dev0` (dev build). pip ignores pre-releases
unless the user opts in, so beta testers install with:

```sh
pip install --pre vmtk        # or: pip install vmtk==1.5.0b1
```

while plain `pip install vmtk` keeps resolving to the latest final
release. Clear the suffix back to `""` (and bump the version as needed)
for the final release. For a local experiment the suffix can also be set
without editing any file: `VMTK_VERSION_SUFFIX=b1 pip wheel . -w dist`.
Remember that PyPI never accepts the same version twice, so each beta
needs a new suffix number (`b1`, `b2`, ...).

## Notes and caveats

* The `vtk` dependency is pinned exactly (`vtk==9.6.2` by default): the
  wheel links against VTK's C++ ABI, so it is only valid for the VTK
  version whose wheel SDK it was built against. Rebuild (and bump
  `VMTK_VTK_VERSION` in `setup.py` and the workflow) to support a newer
  VTK.

* Wheel SDKs are only published for VTK versions that have official PyPI
  wheels; `setup.py` knows the download locations for both the pre-9.4
  (`files/wheel-sdks/`) and 9.4+ (`files/release/<major.minor>/`) layouts.

* TetGen is **not** built into the PyPI packages (`VMTK_BUILD_TETGEN`
  defaults to OFF in setup.py): TetGen 1.4.3 is distributed under its own
  license terms that differ from vmtk's BSD-style license, so it is
  excluded from binaries published on PyPI (the conda packages build it).
  The `vmtktetgen` script is still installed but reports a missing
  `vtkvmtkTetGenWrapper` class when executed. Set `VMTK_BUILD_TETGEN=ON`
  to build a wheel that includes TetGen for private use.

* The `vmtk/.keep` placeholder directory at the repository root exists so
  that setuptools/scikit-build can associate the CMake-installed files with
  the declared `vmtk` package (same pattern as VTKU3DExporter's
  `lib/vtkmodules/.keep`). Do not put sources there.
