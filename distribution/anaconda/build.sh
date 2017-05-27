#!/bin/sh

# mkdir ${PREFIX}/vmtk-build
cd ${PREFIX}/vmtk-build

cmake ${PREFIX}/vmtk/ \
    -DCMAKE_BUILD_TYPE:STRING=Debug

make -j7

cpack --config ${PREFIX}/CPackConfig.cmake