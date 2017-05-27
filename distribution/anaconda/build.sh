#!/bin/sh

# mkdir vmtk-build
cd ./vmtk-build

cmake ../ \
    -DCMAKE_BUILD_TYPE:STRING=Debug

make -j7
make install