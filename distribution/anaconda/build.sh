#!/bin/sh

mkdir build
cd build

PYV=`python -c "import sys;t='{v[0]}.{v[1]}'.format(v=list(sys.version_info[:2]));sys.stdout.write(t)";`
SUPERBUILD_INSTALL_PREFIX="${PREFIX}"
cmake \
    -DCMAKE_BUILD_TYPE:STRING=Debug \
    -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
    -DPYTHON_EXECUTABLE="${PYTHON}" \
    -DPYTHON_INCLUDE_DIR="$PREFIX/include/python${PYV}" \
    -DPYTHON_LIBRARY="$PREFIX/lib/libpython${PYV}.so" ..

make -j7