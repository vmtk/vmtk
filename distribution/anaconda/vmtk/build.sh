#!/bin/sh

## Program:   VMTK
## Module:    Anaconda Distribution
## Language:  Python
## Date:      May 29, 2017

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo; The Jacobs Institute

## This file contains the packaging and distribution shell script data for packaging
## VMTK via the Continuum Analytics Anaconda Python distribution.
## See https://www.continuum.io/ for distribution info

mkdir vmtk-build
cd ./vmtk-build

if [ `uname` == Linux ]; then
    cmake ../ \
        -DCMAKE_BUILD_TYPE:STRING="Release" \
        -DUSE_SYSTEM_VTK:BOOL=ON

    make -j${CPU_COUNT}

    rm -r "./Install/lib/cmake"
    cp -r "./Install/lib"/* "${PREFIX}/lib/"
    cp -r "./Install/bin"/* "${PREFIX}/bin/"
    cp -r "./Install/include"/* "${PREFIX}/include/"
    cp -r "./Install/share"/* "${PREFIX}/share/"
fi

