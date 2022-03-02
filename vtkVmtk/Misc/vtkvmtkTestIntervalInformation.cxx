// IntervalInformation protected member was renamed to a publicly available
// vtkIntervalInformation type on 2021-12-02 in
// https://github.com/Kitware/VTK/commit/a30f2b14bb1bf088513d52e4eebda0b0ed6df08b

// Compilation succeeds for modern VTK (that has vtkIntervalInformation)
// and fails for legacy VTK (where IntervalInformation is only available as a protected struct).

#include "vtkStreamTracer.h"

int main()
{
  vtkIntervalInformation info;
  return 0;
}
