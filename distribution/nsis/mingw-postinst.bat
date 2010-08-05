@echo off

rem Strip off leading 'lib' on .pyd files

setlocal EnableDelayedExpansion

set VMTK_DIR=%~dp0
cd "%VMTK_DIR%lib\vtk-5.6"
for %%f in (lib*.pyd) do (
  set pydfile=%%f
  move /y !pydfile! !pydfile:lib=!
)

cd "%VMTK_DIR%lib\vmtk\vmtk"
for %%f in (lib*.pyd) do (
  set pydfile=%%f
  move /y !pydfile! !pydfile:lib=!
)
