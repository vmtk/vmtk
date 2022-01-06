
The Vascular Modeling Toolkit is an open source project where 
people from all over the world can contribute their work, with 
everyone benefitting from friendly help and advice, and kindly 
helping others in return.

## How to help?

### Contributing to vmtk

If you want to contribute to vmtk please [fork](https://help.github.com/articles/fork-a-repo#contributing-to-a-project) the [source code](https://github.com/vmtk/vmtk)

See [github collaborating guide](https://help.github.com/categories/63/articles), and feel free to reach out to us on the [mailing list](https://groups.google.com/forum/#!forum/vmtk-users) or the issue tracker for any questions. 

### Reporting bugs and issues

See vmtk [open issues](https://github.com/vmtk/vmtk/issues/) or [create new](https://github.com/vmtk/vmtk/issues/new) issues


# Development Version

## Building

VMTK is now based on SuperBuild, a CMake feature that allows to automatically download and compile dependencies (in our case VTK and ITK). Supported versions of library dependencies:

| `VMTK` | `VTK`      | `ITK`       |
|--------|------------|-------------|
| 1.4    | ~8.2, ~9.1 | ~4.13, ~5.2 |
| 1.5    | >=9.1      | >=5.2       |

### Requirements

vmtk is cross-platform and will compile and work on Linux, Mac OSX and Windows.

In order to successfully build vmtk, the following software has to be installed in your system:
- [Git](www.git-scm.org) (>= 1.6)
- [Python](www.python.org) (3.6+)
- [CMake](www.cmake.org) (>=3.3)
- A C++ 11 compliant C++ compiler (see below for platform specific details)

As VMTK is packaged and distributed exclusively through the anaconda package manager, we highly recommend that [Anaconda Python](www.anaconda.org) is installed on your system. 

A number of module in VMTK require external python packages. Please ensure that the following are installed within your python environment:
- [Numpy](http://www.numpy.org/) (<=1.13)
- [h5py](https://www.h5py.org/)
- [joblib](https://pythonhosted.org/joblib/index.html)

These packages come as dependencies of the anaconda installation. For "from source" builds, we recommend installing dependencies using the anaconda python distribution. 

#### Linux Requirements

For Ubuntu users, you'll have to install the following packages prior to compiling:

```
sudo apt-get install libxt-dev libgl1-mesa-glx libgl1-mesa-dev libglapi-mesa libosmesa-dev build-essential
```

For other distributions, please ensure that that OpenGL (version 2), Glew, and OSMesa libraries are installed. VMTK also requires a C++ 11 compatible C++ compiler (we currently use gcc-7 and g++-7). 


#### MacOS Requirements

VMTK only support MacOSX 10.9+. Please ensure that XCode (with xcode command line tools) is installed on the system (we currently clang-9 and clang++-9). 

#### Windows Requirements

Visual Studios 2015+ is required on the system. VMTK no longer supports python 2.7 on windows builds. Please be sure to set "Release" flag instead of the default "Debug" flag in Visual Studio. 

In addition make sure that you choose the Visual Studio amd64 version during cmake configuration in order to build for an x64 system.  

### Build

Create a build directory and cd into it

```
mkdir vmtk-build
cd vmtk-build
```

Run CMake with the directory where the vmtk source tree is located as an argument e.g.

```
cmake ../vmtk
```

Or with the GUI

```
ccmake ../vmtk
```

or if you use the CMake GUI, point the source path to the vmtk source directory.
Set CMake variables as appropriate (they have sensible defaults), *configure* and *generate* for more details look at the [CMake help pages](http://www.cmake.org/cmake/help/runningcmake.html). Once you're done, you'll find either Makefiles or a Visual Studio solution or an XCode project in the build directory, depending on your platform and compiler.

Start your compiler in your build directory, as your platform requires. Note that this will download and compile both VTK and ITK in the background, so it will typically take a long time. The build process will automatically install all build products in the vmtk-build/Install directory.

### Environment variables

In order to properly run vmtk you have to set environment variables.

You can use the script vmtk_env.sh located in your vmtk-build/Install folder.
It allows to set environment variables for your current running shell session.
This is very useful if you have different versions of vmtk installed in your system:

```
source path-to-vmtk_env.sh
```

## Testing

### Acquiring Test Data

VMTK utilizes a git submodule in order to orchestrate the aquisition of large binary data files. If you have just simply cloned the vmtk repository, you will find the tests data director (`tests/vmtk-test-data`) empty. To acquire test data, please run `git submodule init` and `git submodule update` after cloning the repository.

Setting the CMake variable `VMTK_BUILD_TESTING=ON` and building the project will use this information to download the actual binary files to this directory in the build tree. It will also automatically configure the paths to this data directory referenced in the `tests` directory. 

### Running Tests

We use the [pytest](https://docs.pytest.org/en/latest/) testing framework for unit testing. After building the library, run `pytest ./` from within the `Install/tests` directory. 

### Contributing Tests

In order to contribute tests, can clone the [vmtk-test-data](https://github.com/vmtk/vmtk-test-data) repository at the same level as vmtk. Please add any test data files in the appropriate folders in the `vmtk-test-data` repository. if you create new test files within the `tests/test_FOO` directory, be sure to add that file to the accompanying `CMakeLists.txt` file as well. When your tests run locally, your data assets and test changes should be reflected and all tests should appear to pass. 

If you are contributing tests which require new data files, create a PR to the [vmtk-test-data](https://github.com/vmtk/vmtk-test-data) repository and let us know the PR# in VMTK which the data files corresponds to. Until we merge the data, our CI suite will not have access to files, so your tests that passed locally may appear to fail when they are pushed - Don't worry! It's not you, it's our system!

## Building Documentation

### C++ Class Documentation

The [VMTK C++ class documentation](http://www.vmtk.org/doc/html/index.html) is generated by [Doxygen](http://www.stack.nl/~dimitri/doxygen/index.html). The system requires that you have Doxygen, GraphViz, and a LaTeX complier set up on your system path. To enable the html documentation build, set the CMake variable `-DBUILD_DOCUMENTATION=ON` from the command line (or toggle `BUILD_VMTK_DOCUMENTATION` to `ON` if using the CMake curses interface or CMake GUI). The fully configured `doxyfile` and `doc_makeall.sh` file will be placed in the build-directory under the path `foo/VMTK-Build/vtkVmtk/Utilities/Doxygen/` (by default). To generate the website, run the `foo/VMTK-Build/vtkVmtk/Utilities/Doxygen/doc_makeall.sh` file from the terminal. The html files and other resources are placed in `foo/VMTK-Build/vtkVmtk/Utilities/Doxygen/doc/html/` directory. To update the [vmtk.org](www.vmtk.org) website documentation, clone the [vmtk.github.com](https://github.com/vmtk/vmtk.github.com) repository, copy the files into the corresponding `doc/html` directory, commit the files, and propose the changes as a pull request.

### Python vmtkScripts Documentation

[Python vmtkScripts documentation](http://www.vmtk.org/documentation/vmtkscripts.html) is generated by [the vmtkscripts2html.sh shell script](https://github.com/vmtk/vmtk.github.com/blob/master/vmtkscripts2html.sh) contained at the root level of the [vmtk.github.com](https://github.com/vmtk/vmtk.github.com) repository. To use this utility, you first need to build (from source) the version of the VMTK library you want to document. Activate that build in your current terminal session, and `cd` into the top level of your cloned `vmtk.github.com` folder. Call the `vmtkscripts2html.sh` with a single argument: the path to `Install/bin` directory of the local copy of VMTK you built (ex: `bash ./vmtkscripts2html.sh ~/projects/vmtk-build/Install/bin`). This may take a few minutes to run, but it will automatically generate the index/summary page and a corresponding detailed description page for each vmtkscript in the library. It will additionally place them in the appropriate places within the vmtk.github.com folder. Simply commit these changes and propose them as a pull request to update the website documentation.

## Questions? Concerns?

If you have any questions about the contributing process, or just want to learn more about the library, [feel free to reach out to us](http://www.vmtk.org/community).
