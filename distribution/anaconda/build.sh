#!/bin/sh

# unpack.
mkdir vmtk-build
cd vmtk-build

# build.
cmake ../vmtk \
    -DCMAKE_INSTALL_PREFIX=${PREFIX} \
    -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
    -DCMAKE_INSTALL_RPATH:STRING="${PREFIX}/lib" \
    -DPYTHON_EXECUTABLE=${PYTHON} \
    -DPYTHON_INCLUDE_PATH=${PREFIX}/include/python${PY_VER} \
    -DPYTHON_LIBRARY=${PREFIX}/lib/${PY_LIB} \
    -DVTK_INSTALL_PYTHON_MODULE_DIR=${SP_DIR} \
    -VMTK_INSTALL_BIN_DIR="${PREFIX}/bin" \
    -VMTK_INSTALL_LIB_DIR="${PREFIX}/lib" \
    -NOT VMTK_MODULE_INSTALL_LIB_DIR="${PREFIX}/lib/
    .. | tee cmake.log 2>&1