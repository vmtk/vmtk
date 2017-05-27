#!/bin/sh

mkdir ${PREFIX}/vmtk-build
cd ${PREFIX}/vmtk-build

cmake ${PREFIX}/vmtk-1.4-beta.3/ \
    -DCMAKE_BUILD_TYPE:STRING=Debug

make -j7

cpack --config ${PREFIX}/CPackConfig.cmake