/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkConstants.h,v $
  Language:  C++
  Date:      $Date: 2005/11/15 17:39:24 $
  Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef VTK_VMTK_LARGE_INTEGER
#define VTK_VMTK_LARGE_INTEGER 2147483647
#endif

#ifndef VTK_VMTK_FLOAT_TOL
#define VTK_VMTK_FLOAT_TOL 1.0E-6
#endif

#ifndef VTK_VMTK_LARGE_FLOAT
#define VTK_VMTK_LARGE_FLOAT 1.0E+16
#endif

#ifndef VTK_VMTK_DOUBLE_TOL
#define VTK_VMTK_DOUBLE_TOL 1.0E-12
#endif

#ifndef VTK_VMTK_LARGE_DOUBLE
#define VTK_VMTK_LARGE_DOUBLE 1.0E+32
#endif

#ifndef VTK_VMTK_PIVOTING_TOL
#define VTK_VMTK_PIVOTING_TOL 1.0E-12
#endif

#ifndef VTK_VMTK_ITEM_TYPES
#define VTK_VMTK_ITEM_TYPES
enum
  {
    VTK_VMTK_EMPTY_NEIGHBORHOOD,
    VTK_VMTK_POLYDATA_NEIGHBORHOOD,
    VTK_VMTK_POLYDATA_MANIFOLD_NEIGHBORHOOD,
    VTK_VMTK_POLYDATA_MANIFOLD_EXTENDED_NEIGHBORHOOD,
    VTK_VMTK_UNSTRUCTUREDGRID_NEIGHBORHOOD,

    VTK_VMTK_EMPTY_STENCIL,
    VTK_VMTK_UMBRELLA_STENCIL,
    VTK_VMTK_AREA_WEIGHTED_UMBRELLA_STENCIL,
    VTK_VMTK_GRADIENT_STENCIL,
    VTK_VMTK_FE_LAPLACE_BELTRAMI_STENCIL,
    VTK_VMTK_FVFE_LAPLACE_BELTRAMI_STENCIL,

    VTK_VMTK_SPARSE_MATRIX_ROW,
  };

#endif
