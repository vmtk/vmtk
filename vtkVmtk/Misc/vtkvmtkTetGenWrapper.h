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
 * @class   vtkvmtkTetGenWrapper
 * @brief   Wrapped tetgen volumetric meshing utility.
 * @ingroup Misc
 *
 * Wraps Hang Si's TetGen library to Delaunay-tetrahedralize a piecewise linear complex (a closed,
 * possibly multi-region surface, when PLC is on) into a volumetric mesh, or to refine/coarsen an
 * existing tetrahedral mesh. Most properties of this class correspond directly to a TetGen
 * command-line switch (noted per property below); see the TetGen manual for full details of the
 * underlying algorithm. This is the filter behind the vmtkmeshgenerator pype script.
 *
 * @sa vtkvmtkPolyDataSizingFunction
 */

#ifndef __vtkvmtkTetGenWrapper_h
#define __vtkvmtkTetGenWrapper_h

#include "vtkUnstructuredGridAlgorithm.h"
//#include "vtkPointSet.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkTetGenWrapper : public vtkUnstructuredGridAlgorithm
{
  public: 
  static vtkvmtkTetGenWrapper *New();
  vtkTypeMacro(vtkvmtkTetGenWrapper,vtkUnstructuredGridAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Toggle tetrahedralizing the input as a piecewise linear complex (TetGen "-p" switch): treat the
   * input surface as the boundary of the volume(s) to mesh, rather than as a point cloud. Default:
   * off.
   */
  vtkSetMacro(PLC,int);
  vtkGetMacro(PLC,int);
  vtkBooleanMacro(PLC,int);
  ///@}

  ///@{
  /**
   * Toggle refining a previously generated tetrahedral mesh (TetGen "-r" switch). Default: off.
   */
  vtkSetMacro(Refine,int);
  vtkGetMacro(Refine,int);
  vtkBooleanMacro(Refine,int);
  ///@}

  ///@{
  /**
   * Toggle coarsening a previously generated tetrahedral mesh (TetGen "-R" switch). Default: off.
   */
  vtkSetMacro(Coarsen,int);
  vtkGetMacro(Coarsen,int);
  vtkBooleanMacro(Coarsen,int);
  ///@}

  ///@{
  /**
   * Toggle suppressing splitting of input boundary facets/segments (TetGen "-Y" switch), preserving
   * the input surface mesh exactly in the output. Default: off.
   */
  vtkSetMacro(NoBoundarySplit,int);
  vtkGetMacro(NoBoundarySplit,int);
  vtkBooleanMacro(NoBoundarySplit,int);
  ///@}

  ///@{
  /**
   * Toggle quality mesh generation (TetGen "-q" switch), refining the mesh to satisfy the MinRatio
   * (and, if RemoveSliver is on, MinDihedral/MaxDihedral) constraints. Default: off.
   */
  vtkSetMacro(Quality,int);
  vtkGetMacro(Quality,int);
  vtkBooleanMacro(Quality,int);
  ///@}

  ///@{
  /**
   * Set/Get the maximum allowed radius-edge ratio of tetrahedra when Quality is on (number after the
   * TetGen "-q" switch). Default: 2.0.
   */
  vtkSetMacro(MinRatio,double);
  vtkGetMacro(MinRatio,double);
  ///@}

  ///@{
  /**
   * Toggle variable (mesh-size-driven) maximum tetrahedron volume constraint (TetGen "-a" switch
   * without a following number, using a sizing function -- see UseSizingFunction/
   * SizingFunctionArrayName). Default: off.
   */
  vtkSetMacro(VarVolume,int);
  vtkGetMacro(VarVolume,int);
  vtkBooleanMacro(VarVolume,int);
  ///@}

  ///@{
  /**
   * Toggle a fixed maximum tetrahedron volume constraint of MaxVolume (TetGen "-a" switch with a
   * following number). Default: off.
   */
  vtkSetMacro(FixedVolume,int);
  vtkGetMacro(FixedVolume,int);
  vtkBooleanMacro(FixedVolume,int);
  ///@}

  ///@{
  /**
   * Set/Get the maximum tetrahedron volume enforced when FixedVolume is on (number after the TetGen
   * "-a" switch). Default: -1.0 (unset).
   */
  vtkSetMacro(MaxVolume,double);
  vtkGetMacro(MaxVolume,double);
  ///@}

  ///@{
  /**
   * Toggle a post-processing pass (TetGen "-s" switch) that removes sliver tetrahedra whose dihedral
   * angles fall outside [MinDihedral, MaxDihedral]. Default: off.
   */
  vtkSetMacro(RemoveSliver,int);
  vtkGetMacro(RemoveSliver,int);
  vtkBooleanMacro(RemoveSliver,int);
  ///@}

  ///@{
  /**
   * Set/Get the maximum acceptable dihedral angle (in degrees) for sliver removal when RemoveSliver
   * is on (number after "-qqq"). Default: 165.0.
   */
  vtkSetMacro(MaxDihedral,double);
  vtkGetMacro(MaxDihedral,double);
  ///@}

  ///@{
  /**
   * Set/Get the minimum acceptable dihedral angle (in degrees) for sliver removal when RemoveSliver
   * is on (number after "-qq"). Default: 5.0.
   */
  vtkSetMacro(MinDihedral,double);
  vtkGetMacro(MinDihedral,double);
  ///@}

  ///@{
  /**
   * Toggle assigning region attributes to tetrahedra (TetGen "-A" switch), tagging each tetrahedron
   * with the id of the closed sub-region (bounded by distinct facet markers) it belongs to. Default:
   * off.
   */
  vtkSetMacro(RegionAttrib,int);
  vtkGetMacro(RegionAttrib,int);
  vtkBooleanMacro(RegionAttrib,int);
  ///@}

  ///@{
  /**
   * Set/Get the coplanar/collinear tolerance used by TetGen's geometric predicates (number after the
   * "-T" switch). Default: 1.0e-8.
   */
  vtkSetMacro(Epsilon,double);
  vtkGetMacro(Epsilon,double);
  ///@}

  ///@{
  /**
   * Toggle suppressing merging of coplanar facets (TetGen "-M" switch). Default: off.
   */
  vtkSetMacro(NoMerge,int);
  vtkGetMacro(NoMerge,int);
  vtkBooleanMacro(NoMerge,int);
  ///@}

  ///@{
  /**
   * Toggle detecting (rather than avoiding) self-intersections of the input facets (TetGen "-d"
   * switch), useful for diagnosing a non-manifold/self-intersecting input surface. Default: off.
   */
  vtkSetMacro(DetectInter,int);
  vtkGetMacro(DetectInter,int);
  vtkBooleanMacro(DetectInter,int);
  ///@}

  ///@{
  /**
   * Toggle checking whether the input surface forms a valid closed piecewise linear complex (TetGen
   * "-c" switch) before meshing. Default: off.
   */
  vtkSetMacro(CheckClosure,int);
  vtkGetMacro(CheckClosure,int);
  vtkBooleanMacro(CheckClosure,int);
  ///@}

  ///@{
  /**
   * Set/Get the element order (number after the TetGen "-o" switch): 1 for linear tetrahedra
   * (default), 2 for quadratic (10-node) tetrahedra.
   */
  vtkSetMacro(Order,int);
  vtkGetMacro(Order,int);
  ///@}

  ///@{
  /**
   * Toggle having TetGen check the consistency of the final mesh (TetGen "-C" switch) after
   * generation. Default: off.
   */
  vtkSetMacro(DoCheck,int);
  vtkGetMacro(DoCheck,int);
  vtkBooleanMacro(DoCheck,int);
  ///@}

  ///@{
  /**
   * Toggle verbose TetGen console output (TetGen "-V" switch). Default: off.
   */
  vtkSetMacro(Verbose,int);
  vtkGetMacro(Verbose,int);
  vtkBooleanMacro(Verbose,int);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array where the region/boundary marker of each output cell
   * (as assigned via RegionAttrib and/or the input surface's own markers) is stored.
   * Commonly named "CellEntityIds".
   */
  vtkSetStringMacro(CellEntityIdsArrayName);
  vtkGetStringMacro(CellEntityIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the volume of each output tetrahedron is
   * stored.
   * Commonly named "TetrahedronVolume".
   */
  vtkSetStringMacro(TetrahedronVolumeArrayName);
  vtkGetStringMacro(TetrahedronVolumeArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of the input surface holding the desired local mesh
   * size (target tetrahedron edge length/volume) at each point, used when UseSizingFunction (and
   * VarVolume) are on. See vtkvmtkPolyDataSizingFunction.
   * Commonly named "VolumeSizingFunction".
   */
  vtkSetStringMacro(SizingFunctionArrayName);
  vtkGetStringMacro(SizingFunctionArrayName);
  ///@}

  ///@{
  /**
   * Toggle including the boundary (surface) triangles in the output mesh, in addition to the
   * volumetric tetrahedra. Default: on.
   */
  vtkSetMacro(OutputSurfaceElements,int);
  vtkGetMacro(OutputSurfaceElements,int);
  vtkBooleanMacro(OutputSurfaceElements,int);
  ///@}

  ///@{
  /**
   * Toggle including the volumetric tetrahedra in the output mesh. Default: on.
   */
  vtkSetMacro(OutputVolumeElements,int);
  vtkGetMacro(OutputVolumeElements,int);
  vtkBooleanMacro(OutputVolumeElements,int);
  ///@}

  ///@{
  /**
   * Toggle using the per-point sizing function (SizingFunctionArrayName) as a spatially-varying
   * maximum tetrahedron volume constraint (TetGen "-m" switch, used together with VarVolume).
   * Default: off.
   */
  vtkSetMacro(UseSizingFunction,int);
  vtkGetMacro(UseSizingFunction,int);
  vtkBooleanMacro(UseSizingFunction,int);
  ///@}

  /**
   * Set the exit status recorded for the most recent TetGen run; set internally after each
   * RequestData(), 0 on success, non-zero on failure. Only a setter is provided (no corresponding
   * getter is declared).
   */
  vtkSetMacro(LastRunExitStatus,int);
 
  protected:
  vtkvmtkTetGenWrapper();
  ~vtkvmtkTetGenWrapper();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  int PLC;
  int Refine;
  int Coarsen;
  int NoBoundarySplit;
  int Quality;
  double MinRatio;
  int VarVolume;
  int FixedVolume;
  double MaxVolume;
  int RemoveSliver;
  double MinDihedral;
  double MaxDihedral;
  int RegionAttrib;
  double Epsilon;
  int NoMerge;
  int DetectInter;
  int CheckClosure;
  int Order;
  int DoCheck;
  int Verbose;
  int UseSizingFunction;
  int LastRunExitStatus;

  char* CellEntityIdsArrayName;
  char* TetrahedronVolumeArrayName;
  char* SizingFunctionArrayName;

  int OutputSurfaceElements;
  int OutputVolumeElements;

  private:
  vtkvmtkTetGenWrapper(const vtkvmtkTetGenWrapper&);  // Not implemented.
  void operator=(const vtkvmtkTetGenWrapper&);  // Not implemented.
};

#endif
