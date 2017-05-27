#!/bin/sh

mkdir ../vmtk-build
cd ../vmtk-build

cmake ../vmtk-1.4-beta.3/ \
    -DCMAKE_BUILD_TYPE:STRING=Debug

make -j7 install