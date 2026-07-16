"""Build PyPI packages (sdist and binary wheels) for vmtk.

The binary wheel is designed to work alongside the "vtk" wheels published on
PyPI: vmtk's C++ classes are compiled against the VTK wheel SDK that Kitware
publishes for every VTK release (the same SDK that VTK remote modules such as
VTKU3DExporter build against, see
https://github.com/KitwareMedical/VTKExternalModule and
https://github.com/Korijn/VTKU3DExporter for the approach this file is
modeled on). ITK does not provide an equivalent SDK, so ITK is built from
source once (outside the per-Python build trees, so it is reused when
building wheels for several Python versions) and linked in: statically on
Windows, as shared libraries on Linux/macOS where the wheel repair step
(auditwheel/delocate) bundles the ITK runtime libraries into the wheel.

Environment variables understood by this script:

  VMTK_VTK_VERSION        VTK version to build against and to pin as the
                          install requirement (default: 9.6.2). A wheel SDK
                          must exist for this version at vtk.org.
  VTK_WHEEL_SDK_PATH      Use an already downloaded/unpacked VTK wheel SDK
                          instead of downloading one.
  VMTK_ITK_VERSION        ITK version to build (default: 5.4.5).
  ITK_DIR                 Use an existing ITK build/install tree (directory
                          containing ITKConfig.cmake) instead of building
                          ITK from source.
  VMTK_ITK_SHARED         ON/OFF, override the default ITK library type
                          (default: OFF on Windows, ON elsewhere; a single
                          shared ITK is required on Linux/macOS so that all
                          vmtk libraries share one ITK object-factory
                          registry).
  VMTK_BUILD_TETGEN       ON/OFF (default OFF for PyPI packages: TetGen
                          1.4.3 is distributed under its own license terms
                          that differ from vmtk's BSD-style license, so it
                          is not included in packages published on PyPI;
                          the conda packages build it).
  Python3_EXECUTABLE      Python interpreter to build against (default:
                          sys.executable).
  VMTK_VERSION_SUFFIX     Pre-release suffix appended to the version read
                          from CMakeLists.txt (e.g. "b1" -> 1.5.0b1);
                          overrides the set(VMTK_VERSION_SUFFIX ...) value
                          in CMakeLists.txt.
"""

import os
import platform
import subprocess
import sys
import tarfile
import urllib.request
from pathlib import Path

from skbuild import setup


DEFAULT_VTK_VERSION = "9.6.2"
DEFAULT_ITK_VERSION = "5.4.5"

repo_root = Path(__file__).parent.resolve()
deps_dir = repo_root / "_deps"


def vmtk_version():
    """Read the vmtk version from the top-level CMakeLists.txt.

    An optional pre-release suffix (PEP 440: "a1", "b2", "rc1", ".dev0",
    ...) is appended from set(VMTK_VERSION_SUFFIX "...") or, taking
    precedence, from the VMTK_VERSION_SUFFIX environment variable, e.g.
    1.5.0 -> 1.5.0b1. Pre-releases can be uploaded to PyPI as usual; pip
    only installs them when explicitly requested (--pre or an exact pin).
    """
    version = {}
    cmakelists = (repo_root / "CMakeLists.txt").read_text(encoding="utf-8")
    for component in ("MAJOR", "MINOR", "PATCH"):
        marker = f"set(VMTK_VERSION_{component} "
        line = next(l for l in cmakelists.splitlines() if marker in l)
        version[component] = line.split(marker)[1].split(")")[0].strip()

    suffix = os.getenv("VMTK_VERSION_SUFFIX")
    if suffix is None:
        marker = "set(VMTK_VERSION_SUFFIX "
        line = next((l for l in cmakelists.splitlines() if marker in l), "")
        if line:
            suffix = line.split(marker)[1].split(")")[0].strip().strip('"')
        else:
            suffix = ""

    return "{MAJOR}.{MINOR}.{PATCH}".format(**version) + suffix


def vtk_version():
    return os.getenv("VMTK_VTK_VERSION", DEFAULT_VTK_VERSION)


def itk_version():
    return os.getenv("VMTK_ITK_VERSION", DEFAULT_ITK_VERSION)


def python_tag():
    major, minor = sys.version_info[:2]
    return f"cp{major}{minor}-cp{major}{minor}"


def vtk_wheel_sdk_platform_suffix():
    machine = platform.machine().lower()
    if sys.platform == "linux":
        if machine == "aarch64":
            return "manylinux_2_28_aarch64"
        return "manylinux2014_x86_64.manylinux_2_17_x86_64"
    if sys.platform == "darwin":
        is_arm = (
            machine == "arm64"
            # ARCHFLAGS: see https://github.com/pypa/cibuildwheel/discussions/997
            or os.getenv("ARCHFLAGS") == "-arch arm64"
        )
        return "macosx_11_0_arm64" if is_arm else "macosx_10_10_x86_64"
    if sys.platform == "win32":
        return "win_amd64"
    raise NotImplementedError(sys.platform)


def vtk_wheel_sdk_name():
    return (
        f"vtk-wheel-sdk-{vtk_version()}-{python_tag()}"
        f"-{vtk_wheel_sdk_platform_suffix()}"
    )


def vtk_wheel_sdk_url():
    version = vtk_version()
    major_minor = ".".join(version.split(".")[:2])
    if tuple(int(x) for x in version.split(".")[:2]) >= (9, 4):
        # Starting with VTK 9.4 the wheel SDKs are published in the release
        # file area.
        base_url = f"https://vtk.org/files/release/{major_minor}/"
    else:
        base_url = "https://vtk.org/files/wheel-sdks/"
    return f"{base_url}{vtk_wheel_sdk_name()}.tar.xz"


def auto_download_vtk_wheel_sdk():
    """Download and unpack the VTK wheel SDK matching the current Python.

    Returns the path to the unpacked SDK. A previously unpacked SDK is
    reused.
    """
    install_path = deps_dir / vtk_wheel_sdk_name()
    if install_path.exists():
        return install_path

    url = vtk_wheel_sdk_url()
    archive_path = install_path.with_suffix(".tar.xz.tmp")
    install_path.parent.mkdir(parents=True, exist_ok=True)
    print(f"Downloading VTK wheel SDK: {url}")
    urllib.request.urlretrieve(url, archive_path)

    partial_path = install_path.with_name(install_path.name + ".extracting")
    with tarfile.open(archive_path) as tar:
        tar.extractall(partial_path)
    archive_path.unlink()
    # The archive contains a single top-level directory; normalize so that
    # the SDK content lives directly in install_path.
    entries = list(partial_path.iterdir())
    if len(entries) == 1 and entries[0].is_dir():
        entries[0].rename(install_path)
        partial_path.rmdir()
    else:
        partial_path.rename(install_path)
    return install_path


def vtk_cmake_dir(sdk_path):
    """Locate the VTK CMake package directory inside the wheel SDK."""
    matches = list(Path(sdk_path).glob("**/headers/cmake"))
    if len(matches) != 1:
        raise RuntimeError(
            f"Unable to find a unique headers/cmake directory in {sdk_path}"
        )
    return matches[0]


def itk_use_shared_libs():
    override = os.getenv("VMTK_ITK_SHARED")
    if override:
        return override.upper() in ("ON", "1", "TRUE", "YES")
    # A single shared ITK is required on Linux/macOS: with static ITK every
    # vmtk shared library embeds its own copy of ITK's object-factory
    # registry and image IO stops working ("There are no registered IO
    # factories"). Windows was validated with static ITK, which avoids
    # having to bundle and locate ITK DLLs.
    return sys.platform != "win32"


def auto_build_itk():
    """Clone and build ITK once, outside the per-Python build tree.

    Returns the ITK build directory (contains ITKConfig.cmake). The build
    is reused across Python versions (the ITK Python wrapping is not built,
    so the result is Python independent) and by CI caching.
    """
    version = itk_version()
    shared = itk_use_shared_libs()
    lib_type = "shared" if shared else "static"
    source_dir = deps_dir / f"itk-src-{version}"
    build_dir = deps_dir / f"itk-build-{version}-{lib_type}"
    stamp = build_dir / ".vmtk-itk-build-complete"

    if stamp.exists():
        return build_dir

    if not (source_dir / "CMakeLists.txt").exists():
        subprocess.check_call(
            [
                "git", "clone",
                "--depth", "1",
                "--branch", f"v{version}",
                "https://github.com/InsightSoftwareConsortium/ITK.git",
                str(source_dir),
            ]
        )

    configure_cmd = [
        "cmake",
        "-S", str(source_dir),
        "-B", str(build_dir),
        f"-DBUILD_SHARED_LIBS:BOOL={'ON' if shared else 'OFF'}",
        "-DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON",
        "-DCMAKE_BUILD_TYPE:STRING=Release",
        "-DBUILD_TESTING:BOOL=OFF",
        "-DBUILD_EXAMPLES:BOOL=OFF",
        "-DITK_LEGACY_SILENT:BOOL=ON",
    ]
    if sys.platform != "win32":
        # Ninja is provided by the build requirements (pyproject.toml). On
        # Windows the default (Visual Studio) generator is used instead,
        # because ninja would additionally require the MSVC environment
        # variables that scikit-build only sets up for the main build.
        configure_cmd += ["-G", "Ninja"]
    subprocess.check_call(configure_cmd)
    subprocess.check_call(
        [
            "cmake",
            "--build", str(build_dir),
            "--config", "Release",
            "--parallel", str(os.cpu_count() or 2),
        ]
    )
    stamp.touch()
    return build_dir


def native_build_needed():
    """Return False for commands that do not run the CMake build (sdist,
    metadata generation, ...), so that the VTK wheel SDK download and the
    ITK build are only triggered when actually compiling."""
    argv = sys.argv[1:]
    native_commands = {
        "build", "build_ext", "bdist", "bdist_wheel", "install", "develop",
        "editable_wheel",
    }
    if any(arg in native_commands for arg in argv):
        return True
    non_native_commands = {
        "sdist", "egg_info", "dist_info", "clean", "check",
        "--name", "--version", "--fullname",
    }
    if any(arg in non_native_commands for arg in argv):
        return False
    return True


cmake_args = [
    "-DVMTK_USE_SUPERBUILD:BOOL=OFF",
    "-DVMTK_WHEEL_BUILD:BOOL=ON",
    "-DUSE_SYSTEM_VTK:BOOL=ON",
    "-DUSE_SYSTEM_ITK:BOOL=ON",
    "-DVTK_VMTK_WRAP_PYTHON:BOOL=ON",
    "-DBUILD_SHARED_LIBS:BOOL=ON",
    "-DVMTK_BUILD_TESTING:BOOL=OFF",
    "-DVMTK_USE_RENDERING:BOOL=ON",
    "-DVTK_VMTK_CONTRIB:BOOL=ON",
    "-DVMTK_CONTRIB_SCRIPTS:BOOL=ON",
    "-DVMTK_SCRIPTS_ENABLED:BOOL=ON",
    # TetGen is excluded from PyPI packages because of its license terms
    # (set VMTK_BUILD_TETGEN=ON to build it in for private use).
    "-DVMTK_BUILD_TETGEN:BOOL=" + os.getenv("VMTK_BUILD_TETGEN", "OFF"),
    "-DVTK_VMTK_BUILD_TETGEN:BOOL=" + os.getenv("VMTK_BUILD_TETGEN", "OFF"),
]

if native_build_needed():
    # Resolve VTK (wheel SDK) and ITK.
    vtk_sdk_path = os.getenv("VTK_WHEEL_SDK_PATH")
    if vtk_sdk_path is None:
        vtk_sdk_path = auto_download_vtk_wheel_sdk()

    vtk_dir = vtk_cmake_dir(vtk_sdk_path)

    itk_dir = os.getenv("ITK_DIR")
    if itk_dir is None:
        itk_dir = auto_build_itk()

    python3_executable = os.getenv("Python3_EXECUTABLE", sys.executable)

    cmake_args += [
        f"-DVTK_DIR:PATH={Path(vtk_dir).as_posix()}",
        f"-DITK_DIR:PATH={Path(itk_dir).as_posix()}",
        f"-DPython3_EXECUTABLE:FILEPATH={Path(python3_executable).as_posix()}",
    ]

    if itk_use_shared_libs():
        # Lets the wheel repair tools (auditwheel/delocate) and locally
        # built wheels resolve the shared ITK runtime libraries via an
        # rpath entry.
        itk_runtime_lib_dir = Path(itk_dir) / "lib"
        cmake_args.append(
            f"-DVMTK_WHEEL_ITK_RUNTIME_LIB_DIR:PATH={itk_runtime_lib_dir.as_posix()}"
        )

long_description = (repo_root / "README.md").read_text(encoding="utf-8")

setup(
    name="vmtk",
    version=vmtk_version(),
    description="vmtk - the Vascular Modeling Toolkit",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="http://www.vmtk.org",
    author="Luca Antiga, David Steinman, Simone Manini, Richard Izzo",
    license="BSD",
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: BSD License",
        "Programming Language :: Python :: 3",
        "Topic :: Scientific/Engineering :: Medical Science Apps.",
    ],
    keywords="vascular modeling mesh segmentation centerline vtk itk",
    # The whole package content is produced by the CMake build (see the
    # VMTK_WHEEL_BUILD option in the top-level CMakeLists.txt, which makes
    # the install rules place everything into a "vmtk" directory).
    # scikit-build assigns the CMake-installed files to the declared "vmtk"
    # package because the package_dir entry matches the install destination;
    # the placeholder vmtk/.keep file in the source tree only exists so that
    # the declared package directory is present (the same pattern is used by
    # VTKU3DExporter).
    packages=["vmtk"],
    package_dir={"vmtk": "vmtk"},
    include_package_data=False,
    cmake_args=cmake_args,
    python_requires=">=3.9",
    install_requires=[
        f"vtk=={vtk_version()}",
        "numpy",
    ],
    extras_require={
        # Optional runtime dependencies: joblib parallelizes
        # vmtkcenterlinesnetwork (serial fallback without it), h5py is
        # imported on demand by the numpy reader/writer scripts.
        "all": ["h5py", "joblib"],
    },
    entry_points={
        "console_scripts": [
            "vmtk = vmtk.pypemain:main",
        ],
    },
    project_urls={
        "Bug Reports": "https://github.com/vmtk/vmtk/issues",
        "Source": "https://github.com/vmtk/vmtk",
        "Documentation": "http://www.vmtk.org/documentation",
    },
)
