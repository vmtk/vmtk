#!/bin/sh

# unpack.
mkdir vmtk-build
cd vmtk-build

# build.
cmake ../vmtk \
    -DCMAKE_INSTALL_PREFIX=$PREFIX \
    -DENABLE_OS_SPECIFIC_INSTALL=OFF \
    -DENABLE_MATCH=OFF \
    -DENABLE_PETSC=OFF \
    -DENABLE_SLEPC=OFF \
    .. | tee cmake.log 2>&1
