#!/bin/sh

# unpack.
mkdir vmtk-build
cd vmtk-build

# build.
cmake ../vmtk \
    -DCMAKE_INSTALL_PREFIX=$PREFIX
    .. | tee cmake.log 2>&1

make -j 7