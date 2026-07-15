/*=========================================================================

  Program:   VMTK

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

/**
 * @file vtkvmtkConstants.h
 * This header collects preprocessor constants (tolerances, sentinel "large" values, and enumerated
 * item type ids) shared across the vtkVmtk libraries, so that numerical thresholds and mesh/stencil
 * type codes are defined once rather than repeated in every class that needs them. It is not a class
 * and has no members to document individually.
 */

/** Sentinel value used where a "very large" integer is needed (e.g. as an initial minimum-search value). */
#ifndef VTK_VMTK_LARGE_INTEGER
#define VTK_VMTK_LARGE_INTEGER 2147483647
#endif

/** Default numerical tolerance used for float-precision comparisons/degeneracy checks (see e.g. vtkvmtkMath). */
#ifndef VTK_VMTK_FLOAT_TOL
#define VTK_VMTK_FLOAT_TOL 1.0E-6
#endif

/** Sentinel value used where a "very large" float result is returned for degenerate/undefined computations. */
#ifndef VTK_VMTK_LARGE_FLOAT
#define VTK_VMTK_LARGE_FLOAT 1.0E+16
#endif

/** Default numerical tolerance used for double-precision comparisons/degeneracy checks (see e.g. vtkvmtkMath). */
#ifndef VTK_VMTK_DOUBLE_TOL
#define VTK_VMTK_DOUBLE_TOL 1.0E-12
#endif

/** Sentinel value used where a "very large" double result is returned for degenerate/undefined computations. */
#ifndef VTK_VMTK_LARGE_DOUBLE
#define VTK_VMTK_LARGE_DOUBLE 1.0E+32
#endif

/** Tolerance used by pivoting operations (e.g. in linear-system/matrix routines) to detect near-zero pivots. */
#ifndef VTK_VMTK_PIVOTING_TOL
#define VTK_VMTK_PIVOTING_TOL 1.0E-12
#endif

#ifndef VTK_VMTK_ITEM_TYPES
#define VTK_VMTK_ITEM_TYPES
/**
 * Type ids used to identify, at run time, the concrete kind of neighborhood, stencil, or sparse
 * matrix row object in play (see the vtkvmtkNeighborhood/vtkvmtkStencil/vtkvmtkSparseMatrixRow class
 * hierarchies and their subclasses): which mesh connectivity a neighborhood was built from
 * (poly data vs. unstructured grid, manifold vs. extended), and which finite-difference/finite-element
 * weighting scheme a stencil implements (simple umbrella, area-weighted umbrella, gradient-based,
 * finite-element or finite-volume/finite-element Laplace-Beltrami).
 */
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
