#!/bin/sh

mkdir ../build
cd ../build

if [ `uname` == Linux ]; then
    CC=$PREFIX/bin/gcc CCX=$PREFIX/bin/g++ \
    cmake ../vmtk-1.4-beta.3 \
        -DCMAKE_BUILD_TYPE:STRING=debug
fi

make -j7