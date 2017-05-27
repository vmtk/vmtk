#!/bin/sh

# unpack.
mkdir build
cd build

# build.
cmake ../ \
    -DLIBRARY_OUTPUT_PATH=$PATH \
    -DEXECUTABLE_OUTPUT_PATH=$PATH \
    -DVMTK_INSTALL_BIN_DIR=$PREFIX/bin \
    -DVMTK_INSTALL_LIB_DIR=$PREFIX/lib \
    -DVMTK_ENABLE_DISTRIBUTION=ON

make -j 7