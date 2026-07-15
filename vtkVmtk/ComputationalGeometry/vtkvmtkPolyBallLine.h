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
/*! \class vtkvmtkPolyBallLine
    \brief Implements vtkImplicitFunction to evaluate the minimum sphere function for an
    envelope created by an interpolation of sphere radii along a centerline.
    \ingroup ComputationalGeometry

    See detailed description of implicit function in the documentation for vtkvmtkPolyBall class.

    Similar to vtkvmtkPolyBall, the core function of this class is to evaluate the minimum sphere
    function from on input centerline with associated sphere radii and a query point location.
    Unlike vtkvmtkPolyBall, this class constructs a continuous tubular envelope whose shape is
    defined by the linear interpolation of the circular boundary profiles (with radius equal to the
    associated sphere radii) between every consecutive point on the line. As the boundary profiles
    are constructed from the centerline sphere radii, the tubular envelope generated is guaranteed to
    lie completely within the surface volume. When evaluated, this is essentially equivalent to
    evaluating a polyball function for an infinitely large collection of spheres along an input
    dataset.

    This is the implicit function underlying vtkvmtkPolyBallModeller (used by the
    vmtkcenterlinemodeller pype script to rasterize a "tube function" image from centerlines with
    maximum inscribed sphere radii), and it is also used directly wherever vmtk needs to test whether
    a point lies inside the tubular structure swept by a set of centerlines (e.g. flow extension
    generation, centerline-based surface clipping). The Input poly data is expected to be a
    vtkPolyData composed of line/polyline cells whose points carry a point data array (see
    PolyBallRadiusArrayName) with the sphere radius at each point, such as the
    MaximumInscribedSphereRadius array produced by vmtkcenterlines / vtkvmtkPolyDataCenterlines.

    \sa vtkvmtkPolyBall, vtkvmtkPolyBallModeller
*/

#ifndef __vtkvmtkPolyBallLine_h
#define __vtkvmtkPolyBallLine_h

#include "vtkImplicitFunction.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyBallLine : public vtkImplicitFunction
{
  public:

  static vtkvmtkPolyBallLine *New();
  vtkTypeMacro(vtkvmtkPolyBallLine,vtkImplicitFunction);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  /*! Evaluate the polyball-line function at point x: the squared distance from x to the closest
      point on the tubular envelope's medial axis, minus the squared interpolated radius at that
      closest point. Negative values are inside the envelope, positive values are outside, zero is
      on the surface. As a side effect, the identity of the closest centerline segment and
      interpolated sphere are cached and can be retrieved with GetLastPolyBallCellId,
      GetLastPolyBallCellSubId, GetLastPolyBallCellPCoord, GetLastPolyBallCenter and
      GetLastPolyBallCenterRadius. */
  double EvaluateFunction(double x[3]) override;
  double EvaluateFunction(double x, double y, double z) override
  {return this->vtkImplicitFunction::EvaluateFunction(x, y, z); } ;

  /*! Evaluate the gradient of the polyball-line function at point x. Not implemented: calling this
      currently only emits a warning and leaves n unchanged. */
  void EvaluateGradient(double x[3], double n[3]) override;

  ///@{
  /*! Set/get the input poly data whose line/polyline cells define the centerline(s) used to build
      the tubular envelope. Each point of Input should carry a sphere radius value in the array
      named PolyBallRadiusArrayName when UseRadiusInformation is on. */
  vtkSetObjectMacro(Input,vtkPolyData);
  vtkGetObjectMacro(Input,vtkPolyData);
  ///@}

  ///@{
  /*! Set/get the subset of Input cell ids over which the function is evaluated. When set, only
      these cells contribute to the minimum sphere function; this is useful to restrict evaluation to
      a single branch or a group of centerlines. If both InputCellIds and InputCellId are unset
      (default), all line cells in Input are used. */
  vtkSetObjectMacro(InputCellIds,vtkIdList);
  vtkGetObjectMacro(InputCellIds,vtkIdList);
  ///@}

  ///@{
  /*! Set/get a single Input cell id over which the function is evaluated. Ignored if InputCellIds is
      set. Default is -1, meaning no single-cell restriction (all cells, or those in InputCellIds,
      are used). */
  vtkSetMacro(InputCellId,vtkIdType);
  vtkGetMacro(InputCellId,vtkIdType);
  ///@}

  ///@{
  /*! Set/get the name of the point data array of Input holding the sphere radius at each centerline
      point (e.g. the MaximumInscribedSphereRadius array produced by centerline extraction). Required
      whenever UseRadiusInformation is on.
      * Commonly named "MaximumInscribedSphereRadius".
      */
  vtkSetStringMacro(PolyBallRadiusArrayName);
  vtkGetStringMacro(PolyBallRadiusArrayName);
  ///@}

  /*! Get the id, within Input, of the cell containing the closest sphere/segment found by the most
      recent EvaluateFunction call. */
  vtkGetMacro(LastPolyBallCellId,vtkIdType);

  /*! Get the sub-id (index of the line segment within its cell, i.e. the segment between points i
      and i+1) of the closest segment found by the most recent EvaluateFunction call. */
  vtkGetMacro(LastPolyBallCellSubId,vtkIdType);

  /*! Get the parametric coordinate (in [0,1], measured from the start of the closest segment) of the
      closest sphere center found by the most recent EvaluateFunction call. */
  vtkGetMacro(LastPolyBallCellPCoord,double);

  /*! Get the (x,y,z) coordinates of the closest interpolated sphere center found by the most recent
      EvaluateFunction call. */
  vtkGetVectorMacro(LastPolyBallCenter,double,3);

  /*! Get the radius of the closest interpolated sphere found by the most recent EvaluateFunction
      call. */
  vtkGetMacro(LastPolyBallCenterRadius,double);

  ///@{
  /*! Toggle use of the per-point sphere radii stored in PolyBallRadiusArrayName when evaluating the
      function. When off, radii are treated as zero everywhere, so the function reduces to the
      squared distance to the polyline (its zero level set degenerates to the centerline itself
      rather than a tubular envelope). Default: on. */
  vtkSetMacro(UseRadiusInformation,int);
  vtkGetMacro(UseRadiusInformation,int);
  vtkBooleanMacro(UseRadiusInformation,int);
  ///@}

  /*! Dot product in Minkowski (3+1)-D space: the sum of the products of the first three (spatial)
      components minus the product of the fourth (radius) components, i.e.
      x[0]*y[0] + x[1]*y[1] + x[2]*y[2] - x[3]*y[3]. Used internally to project the query point onto
      the cone swept by the linearly interpolated sphere radii along each centerline segment; exposed
      as a static utility for reuse. */
  static double ComplexDot(double x[4], double y[4]);

  protected:
  vtkvmtkPolyBallLine();
  ~vtkvmtkPolyBallLine();

  vtkPolyData* Input;
  vtkIdList* InputCellIds;
  vtkIdType InputCellId;

  char* PolyBallRadiusArrayName;
  vtkIdType LastPolyBallCellId;
  vtkIdType LastPolyBallCellSubId;
  double LastPolyBallCellPCoord;
  double LastPolyBallCenter[3];
  double LastPolyBallCenterRadius;

  int UseRadiusInformation;

  private:
  vtkvmtkPolyBallLine(const vtkvmtkPolyBallLine&);  // Not implemented.
  void operator=(const vtkvmtkPolyBallLine&);  // Not implemented.
};

#endif

