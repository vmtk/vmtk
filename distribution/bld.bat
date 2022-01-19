:: Program:   VMTK
:: Module:    Anaconda Distribution
:: Language:  Python
:: Date:      January 30, 2018
::
::   Copyright (c) Richard Izzo, Luca Antiga. All rights reserved.
::   See LICENSE file for details.
::
::      This software is distributed WITHOUT ANY WARRANTY; without even
::      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
::      PURPOSE.  See the above copyright notices for more information.
::
:: Note: this script was contributed by
::       Richard Izzo (Github @rlizzo)
::       University at Buffalo
::
:: This file contains the packaging and distribution shell script data for packaging
:: VMTK via the Continuum Analytics Anaconda Python distribution.
:: See https://www.continuum.io/ for distribution info

mkdir build
cd build

:: tell cmake where Python is
set PYTHON_MAJOR_VERSION=%PY_VER:~0,1%

cmake .. -LAH -G "Ninja" ^
    -Wno-dev ^
    -DVMTK_USE_SUPERBUILD:BOOL=OFF ^
    -DVMTK_USE_VTK9:BOOL:BOOL=ON ^
    -DCMAKE_BUILD_TYPE:STRING="Release" ^
	-DPython3_FIND_STRATEGY=LOCATION ^
    -DPython3_ROOT_DIR="%PREFIX%" ^
	-DPYTHON_EXECUTABLE:FILEPATH="%PYTHON%" ^
    -DUSE_SYSTEM_VTK:BOOL=ON ^
    -DUSE_SYSTEM_ITK:BOOL=ON ^
    -DCMAKE_INSTALL_PREFIX=%LIBRARY_PREFIX% ^
    -DBUILD_SHARED_LIBS:BOOL=ON ^
	-DCMAKE_PREFIX_PATH="%LIBRARY_PREFIX%" ^
    -DVMTK_RENDERING_BACKEND:STRING="OpenGL2" ^
    -DVMTK_BUILD_TESTING:BOOL=ON ^
    -DCMAKE_CXX_STANDARD=11 ^
    -DCMAKE_CXX_STANDARD_REQUIRED=ON ^
    -DCMAKE_CXX_EXTENSIONS=OFF ^
    -DVMTK_CONTRIB_SCRIPTS:BOOL=ON ^
    -DVMTK_MINIMAL_INSTALL:BOOL=OFF ^
    -DVTK_VMTK_WRAP_PYTHON:BOOL=ON ^
	-DVTK_PYTHON_VERSION:STRING="%PYTHON_MAJOR_VERSION%" ^
	-DVMTK_PYTHON_VERSION="python%PY_VER:~0,1%.%PY_VER:~2,1%"^
    -DGIT_PROTOCOL_HTTPS:BOOL=ON ^
    -DVMTK_MODULE_INSTALL_LIB_DIR:FILEPATH="%PREFIX%/Lib/site-packages/vmtk" ^
    -DVMTK_SCRIPTS_ENABLED:BOOL=ON ^
    -DVMTK_ENABLE_DISTRIBUTION:BOOL=OFF ^
    -DVMTK_WITH_LIBRARY_VERSION:BOOL=OFF ^
    -DVTK_VMTK_WRAPPED_MODULE_INSTALL_LIB_DIR:FILEPATH="%PREFIX%/Lib/site-packages/vmtk" ^
    -DVTK_VMTK_INSTALL_LIB_DIR:FILEPATH="%LIBRARY_LIB%" ^
    -DVTK_VMTK_INSTALL_BIN_DIR:FILEPATH="%LIBRARY_BIN%" ^
    -DVTK_VMTK_MODULE_INSTALL_LIB_DIR:FILEPATH="%PREFIX%/Lib/site-packages/vmtk" ^
	-DVMTK_SCRIPTS_INSTALL_LIB_DIR:FILEPATH="%PREFIX%/Lib/site-packages/vmtk" ^
    -DVMTK_SCRIPTS_INSTALL_BIN_DIR:FILEPATH="%LIBRARY_BIN%" ^
    -DVMTK_CONTRIB_SCRIPTS_INSTALL_LIB_DIR:FILEPATH="%PREFIX%/Lib/site-packages/vmtk" ^
    -DVMTK_CONTRIB_SCRIPTS_INSTALL_BIN_DIR:FILEPATH="%LIBRARY_BIN%" ^
    -DPYPES_MODULE_INSTALL_LIB_DIR:FILEPATH="%PREFIX%/Lib/site-packages/vmtk" ^
    -DPYPES_INSTALL_BIN_DIR:FILEPATH="%LIBRARY_BIN%" ^
    -DVTK_VMTK_CONTRIB:BOOL=OFF ^
    -DVMTK_USE_RENDERING:BOOL=ON

if errorlevel 1 exit 1

ninja install
if errorlevel 1 exit 1

:: This hack should no longer be necessary after setting VTK_VMTK_MODULE_INSTALL_LIB_DIR correctly
:: kept just in case useful in the future
:: Hack to move to vtkvmtk.py script to the correct location
:: set MOVE_FROM=%LIBRARY_LIB%\python%PY_VER:~0,1%.%PY_VER:~2,1%\site-packages\vmtk\vtkvmtk.py
:: move %MOVE_FROM% %PREFIX%\Lib\site-packages\vmtk\vtkvmtk.py
:: if exist "%LIBRARY_LIB%\python%PY_VER:~0,1%.%PY_VER:~2,1%" rmdir /s /q "%LIBRARY_LIB%\python%PY_VER:~0,1%.%PY_VER:~2,1%"
