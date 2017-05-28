#!/bin/sh

mkdir vmtk-build
cd ./vmtk-build

cmake ../ \
    -DCMAKE_BUILD_TYPE:STRING=Debug \
    -DVMTK_ENABLE_DISTRIBUTION:BOOL=ON 

make -j7
make install