
The Vascular Modeling Toolkit is an open source project where 
people from all over the world can contribute their work, with 
everyone benefitting from friendly help and advice, and kindly 
helping others in return.

## How to help?

### Contributing to vmtk

If you want to contribute to vmtk please [fork](https://help.github.com/articles/fork-a-repo#contributing-to-a-project) the [source code](https://github.com/vmtk/vmtk)

See [github collaborating guide](https://help.github.com/categories/63/articles), and feel free to reach out to us on the [mailing list](https://groups.google.com/forum/#!forum/vmtk-users) for any questions. 

### Reporting bugs and issues

See vmtk [open issues](https://github.com/vmtk/vmtk/issues/)</a> or [create new](https://github.com/vmtk/vmtk/issues/new) issues


# Development Version

## Building

VMTK is now based on SuperBuild, a CMake feature that allows to automatically download and compile dependencies (in our case VTK and ITK). This makes the whole process a lot easier than it was up until version 0.9. 

### Requirements

vmtk is cross-platform and will compile and work on Linux, Mac OSX and Windows. Most of the development is performed under Linux and Mac OSX, but feel free to update us on eventual issues on Windows.

In order to successfully compile and use vmtk, the following software has to be installed in your system:
- [Git](www.git-scm.org) (>= 1.6)
- [Python](www.python.org) (2.7, 3.5+)
- [CMake](www.cmake.org) (>=3.3)
- A C++ 11 compliant C++ compiler (see below for platform specific details)

As VMTK is packaged and distributed exclusively through the anaconda package manager, we highly recomend that [Anaconda Python](www.anaconda.org) is installed on your system. 


#### Linux Requirements
For Ubuntu users, you'll have to install the following packages prior to compiling:

```
sudo apt-get install libxt-dev libgl1-mesa-glx libgl1-mesa-dev libglapi-mesa libosmesa-dev build-essential
```

For other distributions, please ensure that that OpenGL (version 2), Glew, and OSMesa libraries are installed. VMTK also requires a C++ 11 compatible C++ compiler (we currently use gcc-7 and g++-7). 


#### MacOS Requirements
VMTK only support MacOSX 10.9+. Please ensure that XCode (with xcode command line tools) is installed on the system (we currently clang-9 and clang++-9). 

#### Windows Requirements
Visual Studios 2015+ is required on the system. VMTK no longer supports python 2.7 on windows builds.Please be sure to set "Release" flag instead of the default "Debug" flag in Visual Studio. 

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
````

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

### Aquiring Test Data

VMTK utilizes CMake External Data directives in order to orchestrate the aquisition of large binary data files. Withing the `tests/data` directory, you will find test data files (named with their appropriate name), but with the extension `.sha512`. Viewing the files will reveal that they are simple text files with a SHA512 hash inside. 

Setting the CMake variable `VMTK_BUILD_TESTING=ON` and building the project will use this information to download the actual binary files to the `.ExternalData` directory. It will also automatically configure the paths to this data directory referenced in the `tests` directory. 

### Running Tests

We use the [pytest](https://docs.pytest.org/en/latest/) testing framework for unit testing. After building the library, run `pytest ./` from within the `Install/tests` directory. 