#!/bin/sh

# unpack.
# TODO: replace vmtk-build-test-anaconda with vmtk-build
mkdir vmtk-build-test-anaconda
cd vmtk-build-test-anaconda

# build.
cmake ../vmtk \
    -DCMAKE_INSTALL_PREFIX=$PREFIX \
    -DENABLE_OS_SPECIFIC_INSTALL=OFF \
    -DENABLE_MATCH=OFF \
    -DENABLE_PETSC=OFF \
    -DENABLE_SLEPC=OFF \
    .. | tee cmake.log 2>&1
