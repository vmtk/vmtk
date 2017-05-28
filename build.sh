#!/bin/sh

mkdir vmtk-build
cd ./vmtk-build

if [ `uname` == Linux ]; then
    cmake ../ \
        -DCMAKE_BUILD_TYPE:STRING="Release"

    make -j7

    rm -r "./Install/lib/cmake"
    cp -r "./Install/lib"/* "${PREFIX}/lib/"
    cp -r "./Install/bin"/* "${PREFIX}/bin/"
    cp -r "./Install/include"/* "${PREFIX}/include/"
    cp -r "./Install/share"/* "${PREFIX}/share/"

fi

