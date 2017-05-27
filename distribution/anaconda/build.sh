#!/bin/sh

# unpack.
mkdir vmtk-build
cd vmtk-build

# build.
cmake ../vmtk \
    -DLIBRARY_OUTPUT_PATH=$PATH \
    -DEXECUTABLE_OUTPUT_PATH=$PATH \
    -DVMTK_INSTALL_BIN_DIR=$PREFIX/bin \
    -DVMTK_INSTALL_LIB_DIR=$PREFIX/lib

make -j 7