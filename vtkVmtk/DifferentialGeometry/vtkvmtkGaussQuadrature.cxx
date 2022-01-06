/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkGaussQuadrature.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/15 17:39:25 $
  Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkGaussQuadrature.h"
#include "vtkObjectFactory.h"
#include "vtkCellType.h"
#include "vtkLine.h"
#include "vtkQuadraticEdge.h"
#include "vtkQuad.h"
#include "vtkQuadraticQuad.h"
#include "vtkBiQuadraticQuad.h"
#include "vtkTriangle.h"
#include "vtkQuadraticTriangle.h"
#include "vtkHexahedron.h"
#include "vtkQuadraticHexahedron.h"
#include "vtkTriQuadraticHexahedron.h"
#include "vtkWedge.h"
#include "vtkQuadraticWedge.h"
#include "vtkTetra.h"
#include "vtkQuadraticTetra.h"
#include "vtkMath.h"

vtkStandardNewMacro(vtkvmtkGaussQuadrature);


vtkvmtkGaussQuadrature::vtkvmtkGaussQuadrature()
{
  this->QuadraturePoints = NULL;
  this->QuadratureWeights = NULL;

  this->Order = 1;
  this->PreviousOrder = 0;
 
  this->CellType = VTK_EMPTY_CELL;
}

vtkvmtkGaussQuadrature::~vtkvmtkGaussQuadrature()
{
  if (this->QuadraturePoints)
  {
    this->QuadraturePoints->Delete();
    this->QuadraturePoints = NULL;
  }

  if (this->QuadratureWeights)
  {
    this->QuadratureWeights->Delete();
    this->QuadratureWeights = NULL;
  }
}

void vtkvmtkGaussQuadrature::Initialize(vtkIdType cellType)
{
  if ((cellType == this->CellType) && (this->Order == this->PreviousOrder))
  {
    return;
  }

  this->CellType = cellType;
  this->PreviousOrder = this->Order;
  
  if (this->QuadraturePoints)
  {
    this->QuadraturePoints->Delete();
    this->QuadraturePoints = NULL;
  }
  this->QuadraturePoints = vtkDoubleArray::New();
  
  if (this->QuadratureWeights)
  {
    this->QuadratureWeights->Delete();
    this->QuadratureWeights = NULL;
  }
  this->QuadratureWeights = vtkDoubleArray::New();
  
  switch(cellType)
  {
    case VTK_LINE:
    case VTK_QUADRATIC_EDGE:
    {
      this->QuadraturePoints->SetNumberOfComponents(1);
      this->Initialize1DGauss();
      break;
    }
    case VTK_QUAD:
    case VTK_QUADRATIC_QUAD:
    case VTK_BIQUADRATIC_QUAD:
    {
      vtkvmtkGaussQuadrature* q1D = vtkvmtkGaussQuadrature::New();
      q1D->SetOrder(this->Order);
      q1D->Initialize1DGauss();
      this->TensorProductQuad(q1D);
      q1D->Delete(); 
      break;
    }
    case VTK_TRIANGLE:
    case VTK_QUADRATIC_TRIANGLE:
    {
      if (this->Order == 0 || this->Order ==1)
      {
        this->QuadraturePoints->SetNumberOfComponents(2);
        this->QuadraturePoints->SetNumberOfTuples(1);
        this->QuadratureWeights->SetNumberOfTuples(1);
        double point[2];
        double weight;
        point[0] = 0.33333333333333333333333333333333;
        point[1] = 0.33333333333333333333333333333333;
        weight = 0.5;
        this->QuadraturePoints->SetTuple(0,point);
        this->QuadratureWeights->SetValue(0,weight);
        break;
      }
      vtkvmtkGaussQuadrature* gauss1D = vtkvmtkGaussQuadrature::New();
      gauss1D->SetOrder(this->Order);
      gauss1D->Initialize1DGauss();
      vtkvmtkGaussQuadrature* jacA1D = vtkvmtkGaussQuadrature::New();
      jacA1D->SetOrder(this->Order);
      jacA1D->Initialize1DJacobi(1,0);
      this->TensorProductTriangle(gauss1D,jacA1D);
      gauss1D->Delete();
      jacA1D->Delete();
      break;
    }
    case VTK_HEXAHEDRON:
    case VTK_QUADRATIC_HEXAHEDRON:
    case VTK_TRIQUADRATIC_HEXAHEDRON:
    {
      vtkvmtkGaussQuadrature* q1D = vtkvmtkGaussQuadrature::New();
      q1D->SetOrder(this->Order);
      q1D->Initialize1DGauss();
      this->TensorProductHexahedron(q1D);
      q1D->Delete(); 
      break;
    }
    case VTK_WEDGE:
    case VTK_QUADRATIC_WEDGE:
    case VTK_BIQUADRATIC_QUADRATIC_WEDGE:
    {
      vtkvmtkGaussQuadrature* q1D = vtkvmtkGaussQuadrature::New();
      q1D->SetOrder(this->Order);
      q1D->Initialize1DGauss();
      vtkvmtkGaussQuadrature* q2D = vtkvmtkGaussQuadrature::New();
      q2D->SetOrder(this->Order);
      q2D->Initialize(VTK_TRIANGLE);
      this->TensorProductWedge(q1D,q2D);
      q1D->Delete();
      q2D->Delete();
      break;
    }
    case VTK_TETRA:
    case VTK_QUADRATIC_TETRA:
    {
      if (this->Order == 0 || this->Order ==1)
      {
        this->QuadraturePoints->SetNumberOfComponents(3);
        this->QuadraturePoints->SetNumberOfTuples(1);
        this->QuadratureWeights->SetNumberOfTuples(1);
        double point[3];
        double weight;
        point[0] = 0.25;
        point[1] = 0.25;
        point[2] = 0.25;
        weight = .1666666666666666666666666666666666666666666667;
        this->QuadraturePoints->SetTuple(0,point);
        this->QuadratureWeights->SetValue(0,weight);
        break;
      }
      vtkvmtkGaussQuadrature* gauss1D = vtkvmtkGaussQuadrature::New();
      gauss1D->SetOrder(this->Order);
      gauss1D->Initialize1DGauss();
      vtkvmtkGaussQuadrature* jacA1D = vtkvmtkGaussQuadrature::New();
      jacA1D->SetOrder(this->Order);
      jacA1D->Initialize1DJacobi(1,0);
      vtkvmtkGaussQuadrature* jacB1D = vtkvmtkGaussQuadrature::New();
      jacB1D->SetOrder(this->Order);
      jacB1D->Initialize1DJacobi(2,0);
      this->TensorProductTetra(gauss1D,jacA1D,jacB1D);
      gauss1D->Delete();
      jacA1D->Delete();
      jacB1D->Delete();
      break;
    }
    default:
    {
      vtkErrorMacro("Unsupported element for Gauss quadrature.");
      return;
    }
  }
}

void vtkvmtkGaussQuadrature::ScaleTo01()
{
  if (this->QuadraturePoints->GetNumberOfComponents() != 1)
  {
    vtkErrorMacro("Error: scaling assumes Dimensionality == 1.");
    return;
  }
 
  double point[1];
  double weight;
  int numberOfQuadraturePoints = this->GetNumberOfQuadraturePoints();
  int i;
  for (i=0; i<numberOfQuadraturePoints; i++)
  {
    this->QuadraturePoints->GetTuple(i,point);
    point[0] = 0.5 * (point[0] + 1.0);
    this->QuadraturePoints->SetTuple(i,point);

    weight = this->QuadratureWeights->GetValue(i);
    weight *= 0.5;
    this->QuadratureWeights->SetValue(i,weight);
  }
}

void vtkvmtkGaussQuadrature::Initialize1DGauss()
{
  if (this->QuadraturePoints)
  {
    this->QuadraturePoints->Delete();
    this->QuadraturePoints = NULL;
  }
  this->QuadraturePoints = vtkDoubleArray::New();
 
  if (this->QuadratureWeights)
  {
    this->QuadratureWeights->Delete();
    this->QuadratureWeights = NULL;
  }
  this->QuadratureWeights = vtkDoubleArray::New();

  switch(this->Order)
  {
    case 0:
    case 1:
    {
      this->QuadraturePoints->SetNumberOfTuples (1);
      this->QuadratureWeights->SetNumberOfTuples(1);
      double* points = this->QuadraturePoints->GetPointer(0);
      double* weights = this->QuadratureWeights->GetPointer(0);
      
      points[0]     = 0.;
      weights[0]    = 2.;
      
      break;
    }
    case 2:
    case 3:
    {
      this->QuadraturePoints->SetNumberOfTuples (2);
      this->QuadratureWeights->SetNumberOfTuples(2);
      double* points = this->QuadraturePoints->GetPointer(0);
      double* weights = this->QuadratureWeights->GetPointer(0);
       
      points[0]    = -0.577350269189626; // -sqrt(3)/3
      points[1]    = -points[0];

      weights[0]   = 1.;
      weights[1]   = weights[0];

      break;
    }
    case 4:
    case 5:
    {
      this->QuadraturePoints->SetNumberOfTuples (3);
      this->QuadratureWeights->SetNumberOfTuples(3);
      double* points = this->QuadraturePoints->GetPointer(0);
      double* weights = this->QuadratureWeights->GetPointer(0);
        
      points[0]    = -0.774596669241483377035853079956;
      points[1]    = 0.;
      points[2]    = -points[0];

      weights[0]   = 0.555555555555555555555555555556;
      weights[1]   = 0.888888888888888888888888888889;
      weights[2]   = weights[0];

      break;
    }
    case 6:
    case 7:
    {
      this->QuadraturePoints->SetNumberOfTuples (4);
      this->QuadratureWeights->SetNumberOfTuples(4);
      double* points = this->QuadraturePoints->GetPointer(0);
      double* weights = this->QuadratureWeights->GetPointer(0);
        
      points[0]    = -0.861136311594052575223946488893;
      points[1]    = -0.339981043584856264802665759103;
      points[2]    = -points[1];
      points[3]    = -points[0];

      weights[0]   = 0.347854845137453857373063949222;
      weights[1]   = 0.652145154862546142626936050778;
      weights[2]   = weights[1];
      weights[3]   = weights[0];

      break;
    }
    case 8:
    case 9:
    {
      this->QuadraturePoints->SetNumberOfTuples (5);
      this->QuadratureWeights->SetNumberOfTuples(5);
      double* points = this->QuadraturePoints->GetPointer(0);
      double* weights = this->QuadratureWeights->GetPointer(0);
        
      points[0]    = -0.906179845938663992797626878299;
      points[1]    = -0.538469310105683091036314420700;
      points[2]    = 0.;
      points[3]    = -points[1];
      points[4]    = -points[0];
        
      weights[0]   = 0.236926885056189087514264040720;
      weights[1]   = 0.478628670499366468041291514836;
      weights[2]   = 0.568888888888888888888888888889;
      weights[3]   = weights[1];
      weights[4]   = weights[0];

      break;
    }
    case 10:
    case 11:
    {
      this->QuadraturePoints->SetNumberOfTuples (6);
      this->QuadratureWeights->SetNumberOfTuples(6);
      double* points = this->QuadraturePoints->GetPointer(0);
      double* weights = this->QuadratureWeights->GetPointer(0);
        
      points[0]    = -0.932469514203152027812301554494;
      points[1]    = -0.661209386466264513661399595020;
      points[2]    = -0.238619186083196908630501721681;
      points[3]    = -points[2];
      points[4]    = -points[1];
      points[5]    = -points[0];

      weights[0]   = 0.171324492379170345040296142173;
      weights[1]   = 0.360761573048138607569833513838;
      weights[2]   = 0.467913934572691047389870343990;
      weights[3]   = weights[2];
      weights[4]   = weights[1];
      weights[5]   = weights[0];

      break;
    }
    case 12:
    case 13:
    {
      this->QuadraturePoints->SetNumberOfTuples (7);
      this->QuadratureWeights->SetNumberOfTuples(7);
      double* points = this->QuadraturePoints->GetPointer(0);
      double* weights = this->QuadratureWeights->GetPointer(0);
        
      points[0]    = -0.949107912342758524526189684048;
      points[1]    = -0.741531185599394439863864773281;
      points[2]    = -0.405845151377397166906606412077;
      points[3]    = 0.;
      points[4]    = -points[2];
      points[5]    = -points[1];
      points[6]    = -points[0];

      weights[0]   = 0.12948496616887;
      weights[1]   = 0.27970539148928;
      weights[2]   = 0.38183005050512;
      weights[3]   = 0.41795918367347;
      weights[4]   = weights[2];
      weights[5]   = weights[1];
      weights[6]   = weights[0];

      break;
    }
    case 14:
    case 15:
    {
      this->QuadraturePoints->SetNumberOfTuples (8);
      this->QuadratureWeights->SetNumberOfTuples(8);
      double* points = this->QuadraturePoints->GetPointer(0);
      double* weights = this->QuadratureWeights->GetPointer(0);
        
      points[0]    = -0.960289856497536231683560868569;
      points[1]    = -0.796666477413626739591553936476;
      points[2]    = -0.525532409916328985817739049189;
      points[3]    = -0.183434642495649804939476142360;
      points[4]    = -points[3];
      points[5]    = -points[2];
      points[6]    = -points[1];
      points[7]    = -points[0];

      weights[0]   = 0.101228536290376259152531354310;
      weights[1]   = 0.222381034453374470544355994426;
      weights[2]   = 0.313706645877887287337962201987;
      weights[3]   = 0.362683783378361982965150449277;
      weights[4]   = weights[3];
      weights[5]   = weights[2];
      weights[6]   = weights[1];
      weights[7]   = weights[0];

      break;
    }
    case 16:
    case 17:
    {
      this->QuadraturePoints->SetNumberOfTuples (9);
      this->QuadratureWeights->SetNumberOfTuples(9);
      double* points = this->QuadraturePoints->GetPointer(0);
      double* weights = this->QuadratureWeights->GetPointer(0);

      points[0]    = -0.968160239507626089835576202904;
      points[1]    = -0.836031107326635794299429788070;
      points[2]    = -0.613371432700590397308702039341;
      points[3]    = -0.324253423403808929038538014643;
      points[4]    =  0.0000000000000000000000000000000;
      points[5]    = -points[3];
      points[6]    = -points[2];
      points[7]    = -points[1];
      points[8]    = -points[0];

      weights[0]   = 0.0812743883615744119718921581105;
      weights[1]   = 0.180648160694857404058472031243;
      weights[2]   = 0.260610696402935462318742869419;
      weights[3]   = 0.312347077040002840068630406584;
      weights[4]   = 0.330239355001259763164525069287;
      weights[5]   = weights[3];
      weights[6]   = weights[2];
      weights[7]   = weights[1];
      weights[8]   = weights[0];

      break;
    }
    case 18:
    case 19:
    {
      this->QuadraturePoints->SetNumberOfTuples (10);
      this->QuadratureWeights->SetNumberOfTuples(10);
      double* points = this->QuadraturePoints->GetPointer(0);
      double* weights = this->QuadratureWeights->GetPointer(0);

      points[0]    = -0.973906528517171720077964012084;
      points[1]    = -0.865063366688984510732096688423;
      points[2]    = -0.679409568299024406234327365115;
      points[3]    = -0.433395394129247190799265943166;
      points[4]    = -0.148874338981631210864826001130;
      points[5]    = -points[4];
      points[6]    = -points[3];
      points[7]    = -points[2];
      points[8]    = -points[1];
      points[9]    = -points[0];
  
      weights[0]   = 0.0666713443086881375935688098933; 
      weights[1]   = 0.149451349150580593145776339658;
      weights[2]   = 0.219086362515982043995534934228;
      weights[3]   = 0.269266719309996355091226921569;
      weights[4]   = 0.295524224714752870173892994651;
      weights[5]   = weights[4];
      weights[6]   = weights[3];
      weights[7]   = weights[2];
      weights[8]   = weights[1];
      weights[9]   = weights[0];
    
      break;
    }      

    case 20:
    case 21:
    {
      this->QuadraturePoints->SetNumberOfTuples (11);
      this->QuadratureWeights->SetNumberOfTuples(11);
      double* points = this->QuadraturePoints->GetPointer(0);
      double* weights = this->QuadratureWeights->GetPointer(0);
    
      points[0]     = -0.97822865814606;
      points[1]     = -0.88706259976810;
      points[2]     = -0.73015200557405;
      points[3]     = -0.51909612920681;
      points[4]     = -0.26954315595235;
      points[5]     = 0.;
      points[6]     = -points[4];
      points[7]     = -points[3];
      points[8]     = -points[2];
      points[9]     = -points[1];
      points[10]    = -points[0];
    
      weights[0]     = 0.05566856711617;
      weights[1]     = 0.12558036946490;
      weights[2]     = 0.18629021092773;
      weights[3]     = 0.23319376459199;
      weights[4]     = 0.26280454451025;
      weights[5]     = 0.27292508677790;
      weights[6]     = weights[4];
      weights[7]     = weights[3];
      weights[8]     = weights[2];
      weights[9]     = weights[1];
      weights[10]    = weights[0];
    
      break;
    }

    case 22:
    case 23:
    {
      this->QuadraturePoints->SetNumberOfTuples (12);
      this->QuadratureWeights->SetNumberOfTuples(12);
      double* points = this->QuadraturePoints->GetPointer(0);
      double* weights = this->QuadratureWeights->GetPointer(0);
    
      points[0]    = -0.981560634246719250690549090149;
      points[1]    = -0.904117256370474856678465866119;
      points[2]    = -0.769902674194304687036893833213;
      points[3]    = -0.587317954286617447296702418941;
      points[4]    = -0.367831498998180193752691536644;
      points[5]    = -0.125233408511468915472441369464;
      points[6]    = -points[5];
      points[7]    = -points[4];
      points[8]    = -points[3];
      points[9]    = -points[2];
      points[10]   = -points[1];
      points[11]   = -points[0];
    
      weights[0]   = 0.0471753363865118271946159614850; 
      weights[1]   = 0.106939325995318430960254718194;
      weights[2]   = 0.160078328543346226334652529543;
      weights[3]   = 0.203167426723065921749064455810;
      weights[4]   = 0.233492536538354808760849898925;
      weights[5]   = 0.249147045813402785000562436043;
      weights[6]   = weights[5];
      weights[7]   = weights[4];
      weights[8]   = weights[3];
      weights[9]   = weights[2];
      weights[10]  = weights[1];
      weights[11]  = weights[0];
    
      break;
    }      

    case 24:
    case 25:
    {
      this->QuadraturePoints->SetNumberOfTuples (13);
      this->QuadratureWeights->SetNumberOfTuples(13);
      double* points = this->QuadraturePoints->GetPointer(0);
      double* weights = this->QuadratureWeights->GetPointer(0);
    
      points[0]     = -0.98418305471859;
      points[1]     = -0.91759839922298;
      points[2]     = -0.80157809073331;
      points[3]     = -0.64234933944034;
      points[4]     = -0.44849275103645;
      points[5]     = -0.23045831595513;
      points[6]     = 0.;
      points[7]     = -points[5];
      points[8]     = -points[4];
      points[9]     = -points[3];
      points[10]    = -points[2];
      points[11]    = -points[1];
      points[12]    = -points[0];
    
      weights[0]     = 0.04048400476532;
      weights[1]     = 0.09212149983773;
      weights[2]     = 0.13887351021979;
      weights[3]     = 0.17814598076195;
      weights[4]     = 0.20781604753689;
      weights[5]     = 0.22628318026290;
      weights[6]     = 0.23255155323087;
      weights[7]     = weights[5];
      weights[8]     = weights[4];
      weights[9]     = weights[3];
      weights[10]    = weights[2];
      weights[11]    = weights[1];
      weights[12]    = weights[0];
    
      break;
    }

    case 26:
    case 27:
    {
      this->QuadraturePoints->SetNumberOfTuples (14);
      this->QuadratureWeights->SetNumberOfTuples(14);
      double* points = this->QuadraturePoints->GetPointer(0);
      double* weights = this->QuadratureWeights->GetPointer(0);
    
      points[0]     = -0.98628380869681;
      points[1]     = -0.92843488366357;
      points[2]     = -0.82720131506977;
      points[3]     = -0.68729290481169;
      points[4]     = -0.51524863635815;
      points[5]     = -0.31911236892789;
      points[6]     = -0.10805494870734;
      points[7]     = -points[6];
      points[8]     = -points[5];
      points[9]     = -points[4];
      points[10]    = -points[3];
      points[11]    = -points[2];
      points[12]    = -points[1];
      points[13]    = -points[0];
    
      weights[0]     = 0.03511946033175;
      weights[1]     = 0.08015808715976;
      weights[2]     = 0.12151857068790;
      weights[3]     = 0.15720316715819;
      weights[4]     = 0.18553839747794;
      weights[5]     = 0.20519846372130;
      weights[6]     = 0.21526385346316;
      weights[7]     = weights[6];
      weights[8]     = weights[5];
      weights[9]     = weights[4];
      weights[10]    = weights[3];
      weights[11]    = weights[2];
      weights[12]    = weights[1];
      weights[13]    = weights[0];
    
      break;
    }

    case 28:
    case 29:
    {
      this->QuadraturePoints->SetNumberOfTuples (15);
      this->QuadratureWeights->SetNumberOfTuples(15);
      double* points = this->QuadraturePoints->GetPointer(0);
      double* weights = this->QuadratureWeights->GetPointer(0);
    
      points[0]     = -0.98799251802049;
      points[1]     = -0.93727339240071;
      points[2]     = -0.84820658341043;
      points[3]     = -0.72441773136017;
      points[4]     = -0.57097217260854;
      points[5]     = -0.39415134707756;
      points[6]     = -0.20119409399743;
      points[7]     = -0.00000000000000;
      points[8]     = -points[6];
      points[9]     = -points[5];
      points[10]    = -points[4];
      points[11]    = -points[3];
      points[12]    = -points[2];
      points[13]    = -points[1];
      points[14]    = -points[0];
    
      weights[0]     = 0.03075324199612;
      weights[1]     = 0.07036604748811;
      weights[2]     = 0.10715922046717;
      weights[3]     = 0.13957067792615;
      weights[4]     = 0.16626920581699;
      weights[5]     = 0.18616100001556;
      weights[6]     = 0.19843148532711;
      weights[7]     = 0.20257824192556;
      weights[8]     = weights[6];
      weights[9]     = weights[5];
      weights[10]    = weights[4];
      weights[11]    = weights[3];
      weights[12]    = weights[2];
      weights[13]    = weights[1];
      weights[14]    = weights[0];
    
      break;
    }

    case 30:
    case 31:
    {
      this->QuadraturePoints->SetNumberOfTuples (16);
      this->QuadratureWeights->SetNumberOfTuples(16);
      double* points = this->QuadraturePoints->GetPointer(0);
      double* weights = this->QuadratureWeights->GetPointer(0);
    
      points[0]     = -0.98940093499165;
      points[1]     = -0.94457502307323;
      points[2]     = -0.86563120238783;
      points[3]     = -0.75540440835500;
      points[4]     = -0.61787624440264;
      points[5]     = -0.45801677765723;
      points[6]     = -0.28160355077926;
      points[7]     = -0.09501250983764;
      points[8]     = -points[7];
      points[9]     = -points[6];
      points[10]    = -points[5];
      points[11]    = -points[4];
      points[12]    = -points[3];
      points[13]    = -points[2];
      points[14]    = -points[1];
      points[15]    = -points[0];
    
      weights[0]     = 0.02715245941175;
      weights[1]     = 0.06225352393865;
      weights[2]     = 0.09515851168249;
      weights[3]     = 0.12462897125553;
      weights[4]     = 0.14959598881658;
      weights[5]     = 0.16915651939500;
      weights[6]     = 0.18260341504492;
      weights[7]     = 0.18945061045507;
      weights[8]     = weights[7];
      weights[9]     = weights[6];
      weights[10]    = weights[5];
      weights[11]    = weights[4];
      weights[12]    = weights[3];
      weights[13]    = weights[2];
      weights[14]    = weights[1];
      weights[15]    = weights[0];
    
      break;
    }

    case 32:
    case 33:
    {
      this->QuadraturePoints->SetNumberOfTuples (17);
      this->QuadratureWeights->SetNumberOfTuples(17);
      double* points = this->QuadraturePoints->GetPointer(0);
      double* weights = this->QuadratureWeights->GetPointer(0);
    
      points[0]     = -0.99057547531442;
      points[1]     = -0.95067552176877;
      points[2]     = -0.88023915372699;
      points[3]     = -0.78151400389680;
      points[4]     = -0.65767115921669;
      points[5]     = -0.51269053708648;
      points[6]     = -0.35123176345388;
      points[7]     = -0.17848418149585;
      points[8]     = 0.;
      points[9]     = -points[7];
      points[10]    = -points[6];
      points[11]    = -points[5];
      points[12]    = -points[4];
      points[13]    = -points[3];
      points[14]    = -points[2];
      points[15]    = -points[1];
      points[16]    = -points[0];
    
      weights[0]     = 0.02414830286855;
      weights[1]     = 0.05545952937399;
      weights[2]     = 0.08503614831718;
      weights[3]     = 0.11188384719340;
      weights[4]     = 0.13513636846853;
      weights[5]     = 0.15404576107681;
      weights[6]     = 0.16800410215645;
      weights[7]     = 0.17656270536699;
      weights[8]     = 0.17944647035621;
      weights[9]     = weights[7];
      weights[10]    = weights[6];
      weights[11]    = weights[5];
      weights[12]    = weights[4];
      weights[13]    = weights[3];
      weights[14]    = weights[2];
      weights[15]    = weights[1];
      weights[16]    = weights[0];
    
      break;
    }

    case 34:
    case 35:
    {
      this->QuadraturePoints->SetNumberOfTuples (18);
      this->QuadratureWeights->SetNumberOfTuples(18);
      double* points = this->QuadraturePoints->GetPointer(0);
      double* weights = this->QuadratureWeights->GetPointer(0);
    
      points[0]     = -0.99156516842093;
      points[1]     = -0.95582394957140;
      points[2]     = -0.89260246649756;
      points[3]     = -0.80370495897252;
      points[4]     = -0.69168704306035;
      points[5]     = -0.55977083107395;
      points[6]     = -0.41175116146284;
      points[7]     = -0.25188622569151;
      points[8]     = -0.08477501304173;
      points[9]     = -points[8];
      points[10]    = -points[7];
      points[11]    = -points[6];
      points[12]    = -points[5];
      points[13]    = -points[4];
      points[14]    = -points[3];
      points[15]    = -points[2];
      points[16]    = -points[1];
      points[17]    = -points[0];
    
      weights[0]     = 0.02161601352648;
      weights[1]     = 0.04971454889497;
      weights[2]     = 0.07642573025489;
      weights[3]     = 0.10094204410629;
      weights[4]     = 0.12255520671148;
      weights[5]     = 0.14064291467065;
      weights[6]     = 0.15468467512627;
      weights[7]     = 0.16427648374583;
      weights[8]     = 0.16914238296314;
      weights[9]     = weights[8];
      weights[10]    = weights[7];
      weights[11]    = weights[6];
      weights[12]    = weights[5];
      weights[13]    = weights[4];
      weights[14]    = weights[3];
      weights[15]    = weights[2];
      weights[16]    = weights[1];
      weights[17]    = weights[0];
    
      break;
    }

    case 36:
    case 37:
    {
      this->QuadraturePoints->SetNumberOfTuples (19);
      this->QuadratureWeights->SetNumberOfTuples(19);
      double* points = this->QuadraturePoints->GetPointer(0);
      double* weights = this->QuadratureWeights->GetPointer(0);
    
      points[0]     = -0.99240684384358;
      points[1]     = -0.96020815213483;
      points[2]     = -0.90315590361482;
      points[3]     = -0.82271465653714;
      points[4]     = -0.72096617733523;
      points[5]     = -0.60054530466168;
      points[6]     = -0.46457074137596;
      points[7]     = -0.31656409996363;
      points[8]     = -0.16035864564023;
      points[9]     = 0.;
      points[10]    = -points[8];
      points[11]    = -points[7];
      points[12]    = -points[6];
      points[13]    = -points[5];
      points[14]    = -points[4];
      points[15]    = -points[3];
      points[16]    = -points[2];
      points[17]    = -points[1];
      points[18]    = -points[0];
    
      weights[0]     = 0.01946178822973;
      weights[1]     = 0.04481422676570;
      weights[2]     = 0.06904454273764;
      weights[3]     = 0.09149002162245;
      weights[4]     = 0.11156664554733;
      weights[5]     = 0.12875396253934;
      weights[6]     = 0.14260670217361;
      weights[7]     = 0.15276604206586;
      weights[8]     = 0.15896884339395;
      weights[9]     = 0.16105444984878;
      weights[10]    = weights[8];
      weights[11]    = weights[7];
      weights[12]    = weights[6];
      weights[13]    = weights[5];
      weights[14]    = weights[4];
      weights[15]    = weights[3];
      weights[16]    = weights[2];
      weights[17]    = weights[1];
      weights[18]    = weights[0];
    
      break;
    }

    case 38:
    case 39:
    {
      this->QuadraturePoints->SetNumberOfTuples (20);
      this->QuadratureWeights->SetNumberOfTuples(20);
      double* points = this->QuadraturePoints->GetPointer(0);
      double* weights = this->QuadratureWeights->GetPointer(0);
    
      points[0]     = -0.99312859918510;
      points[1]     = -0.96397192727791;
      points[2]     = -0.91223442825133;
      points[3]     = -0.83911697182222;
      points[4]     = -0.74633190646015;
      points[5]     = -0.63605368072652;
      points[6]     = -0.51086700195083;
      points[7]     = -0.37370608871542;
      points[8]     = -0.22778585114164;
      points[9]     = -0.07652652113350;
      points[10]    = -points[9];
      points[11]    = -points[8];
      points[12]    = -points[7];
      points[13]    = -points[6];
      points[14]    = -points[5];
      points[15]    = -points[4];
      points[16]    = -points[3];
      points[17]    = -points[2];
      points[18]    = -points[1];
      points[19]    = -points[0];
    
      weights[0]     = 0.01761400713915;
      weights[1]     = 0.04060142980039;
      weights[2]     = 0.06267204833411;
      weights[3]     = 0.08327674157670;
      weights[4]     = 0.10193011981724;
      weights[5]     = 0.11819453196152;
      weights[6]     = 0.13168863844918;
      weights[7]     = 0.14209610931838;
      weights[8]     = 0.14917298647260;
      weights[9]     = 0.15275338713073;
      weights[10]    = weights[9];
      weights[11]    = weights[8];
      weights[12]    = weights[7];
      weights[13]    = weights[6];
      weights[14]    = weights[5];
      weights[15]    = weights[4];
      weights[16]    = weights[3];
      weights[17]    = weights[2];
      weights[18]    = weights[1];
      weights[19]    = weights[0];
    
      break;
    }

    case 40:
    case 41:
    {
      this->QuadraturePoints->SetNumberOfTuples (21);
      this->QuadratureWeights->SetNumberOfTuples(21);
      double* points = this->QuadraturePoints->GetPointer(0);
      double* weights = this->QuadratureWeights->GetPointer(0);
    
      points[0]     = -0.99375217062039;
      points[1]     = -0.96722683856631;
      points[2]     = -0.92009933415040;
      points[3]     = -0.85336336458332;
      points[4]     = -0.76843996347568;
      points[5]     = -0.66713880419741;
      points[6]     = -0.55161883588722;
      points[7]     = -0.42434212020744;
      points[8]     = -0.28802131680240;
      points[9]     = -0.14556185416090;
      points[10]    = 0.;
      points[11]    = -points[9];
      points[12]    = -points[8];
      points[13]    = -points[7];
      points[14]    = -points[6];
      points[15]    = -points[5];
      points[16]    = -points[4];
      points[17]    = -points[3];
      points[18]    = -points[2];
      points[19]    = -points[1];
      points[20]    = -points[0];
    
      weights[0]     = 0.01601722825777;
      weights[1]     = 0.03695378977085;
      weights[2]     = 0.05713442542686;
      weights[3]     = 0.07610011362838;
      weights[4]     = 0.09344442345603;
      weights[5]     = 0.10879729916715;
      weights[6]     = 0.12183141605373;
      weights[7]     = 0.13226893863334;
      weights[8]     = 0.13988739479107;
      weights[9]     = 0.14452440398997;
      weights[10]    = 0.14608113364969;
      weights[11]    = weights[9];
      weights[12]    = weights[8];
      weights[13]    = weights[7];
      weights[14]    = weights[6];
      weights[15]    = weights[5];
      weights[16]    = weights[4];
      weights[17]    = weights[3];
      weights[18]    = weights[2];
      weights[19]    = weights[1];
      weights[20]    = weights[0];
    
      break;
    }

    case 42:
    case 43:
    {
      this->QuadraturePoints->SetNumberOfTuples (22);
      this->QuadratureWeights->SetNumberOfTuples(22);
      double* points = this->QuadraturePoints->GetPointer(0);
      double* weights = this->QuadratureWeights->GetPointer(0);
    
      points[0]     = -0.99429458548240;
      points[1]     = -0.97006049783543;
      points[2]     = -0.92695677218717;
      points[3]     = -0.86581257772030;
      points[4]     = -0.78781680597921;
      points[5]     = -0.69448726318668;
      points[6]     = -0.58764040350691;
      points[7]     = -0.46935583798676;
      points[8]     = -0.34193582089208;
      points[9]     = -0.20786042668822;
      points[10]    = -0.06973927331972;
      points[11]    = -points[10];
      points[12]    = -points[9];
      points[13]    = -points[8];
      points[14]    = -points[7];
      points[15]    = -points[6];
      points[16]    = -points[5];
      points[17]    = -points[4];
      points[18]    = -points[3];
      points[19]    = -points[2];
      points[20]    = -points[1];
      points[21]    = -points[0];
    
      weights[0]     = 0.01462799529827;
      weights[1]     = 0.03377490158481;
      weights[2]     = 0.05229333515268;
      weights[3]     = 0.06979646842452;
      weights[4]     = 0.08594160621707;
      weights[5]     = 0.10041414444288;
      weights[6]     = 0.11293229608054;
      weights[7]     = 0.12325237681051;
      weights[8]     = 0.13117350478706;
      weights[9]     = 0.13654149834602;
      weights[10]    = 0.13925187285563;
      weights[11]    = weights[10];
      weights[12]    = weights[9];
      weights[13]    = weights[8];
      weights[14]    = weights[7];
      weights[15]    = weights[6];
      weights[16]    = weights[5];
      weights[17]    = weights[4];
      weights[18]    = weights[3];
      weights[19]    = weights[2];
      weights[20]    = weights[1];
      weights[21]    = weights[0];
    
      break;
    }

    default:
    {
      vtkErrorMacro("Quadrature rule not supported.");
      return; 
    }
  }

  this->ScaleTo01();
}

void vtkvmtkGaussQuadrature::Initialize1DJacobi(int alpha, int beta)
{
  if (this->QuadraturePoints)
  {
    this->QuadraturePoints->Delete();
    this->QuadraturePoints = NULL;
  }
  this->QuadraturePoints = vtkDoubleArray::New();
 
  if (this->QuadratureWeights)
  {
    this->QuadratureWeights->Delete();
    this->QuadratureWeights = NULL;
  }
  this->QuadratureWeights = vtkDoubleArray::New();
 
  if ((alpha == 1) && (beta == 0))
  {
    switch (this->Order)
    {
      case 0:
      case 1:
      {
        this->QuadraturePoints->SetNumberOfTuples (1);
        this->QuadratureWeights->SetNumberOfTuples(1);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
    
        points[0]  = 0.33333333333333; 
    
        weights[0]    = 0.50000000000000;
    
        break;
      }
      case 2:
      case 3:
      {
        this->QuadraturePoints->SetNumberOfTuples (2);
        this->QuadratureWeights->SetNumberOfTuples(2);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
          
        points[0] = 0.15505102572168;
        points[1] = 0.64494897427832;
  
        weights[0]   = 0.31804138174398;
        weights[1]   = 0.18195861825602;
  
        break;
      }
      case 4:
      case 5:
      {
        this->QuadraturePoints->SetNumberOfTuples (3);
        this->QuadratureWeights->SetNumberOfTuples(3);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
          
        points[0] = 0.08858795951270;
        points[1] = 0.40946686444073;
        points[2] = 0.78765946176085;
  
        weights[0]   = 0.20093191373896;
        weights[1]   = 0.22924110635959;
        weights[2]   = 0.06982697990145;
  
        break;
      }
      case 6:
      case 7:
      {
        this->QuadraturePoints->SetNumberOfTuples (4);
        this->QuadratureWeights->SetNumberOfTuples(4);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
          
        points[0] = 0.05710419611452;
        points[1] = 0.27684301363812;
        points[2] = 0.58359043236892;
        points[3] = 0.86024013565622;
  
        weights[0]   = 0.13550691343149;
        weights[1]   = 0.20346456801027;
        weights[2]   = 0.12984754760823;
        weights[3]   = 0.03118097095001;
  
        break;
      }
      case 8:
      case 9:
      {
        this->QuadraturePoints->SetNumberOfTuples (5);
        this->QuadratureWeights->SetNumberOfTuples(5);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
          
        points[0] = 0.03980985705147;
        points[1] = 0.19801341787361;
        points[2] = 0.43797481024739;
        points[3] = 0.69546427335364;
        points[4] = 0.90146491420117;
          
        weights[0]   = 0.09678159022665;
        weights[1]   = 0.16717463809437;
        weights[2]   = 0.14638698708467;
        weights[3]   = 0.07390887007262;
        weights[4]   = 0.01574791452169;
  
        break;
      }
      case 10:
      case 11:
      {
        this->QuadraturePoints->SetNumberOfTuples (6);
        this->QuadratureWeights->SetNumberOfTuples(6);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
          
        points[0] = 0.02931642715979;
        points[1] = 0.14807859966848;
        points[2] = 0.33698469028115;
        points[3] = 0.55867151877155;
        points[4] = 0.76923386203005;
        points[5] = 0.92694567131974;
  
        weights[0]   = 0.07231033072551;
        weights[1]   = 0.13554249723152;
        weights[2]   = 0.14079255378820;
        weights[3]   = 0.09866115089066;
        weights[4]   = 0.04395516555051;
        weights[5]   = 0.00873830181361;
  
        break;
      }
      case 12:
      case 13:
      {
        this->QuadraturePoints->SetNumberOfTuples (7);
        this->QuadratureWeights->SetNumberOfTuples(7);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
          
        points[0] = 0.02247938643871;
        points[1] = 0.11467905316090;
        points[2] = 0.26578982278459;
        points[3] = 0.45284637366944;
        points[4] = 0.64737528288683;
        points[5] = 0.81975930826311;
        points[6] = 0.94373743946308;
  
        weights[0]   = 0.05596736342349;
        weights[1]   = 0.11050925819087;
        weights[2]   = 0.12739089729959;
        weights[3]   = 0.10712506569587;
        weights[4]   = 0.06638469646549;
        weights[5]   = 0.02740835672187;
        weights[6]   = 0.00521436220281;
  
        break;
      }
      case 14:
      case 15:
      {
        this->QuadraturePoints->SetNumberOfTuples (8);
        this->QuadratureWeights->SetNumberOfTuples(8);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
          
        points[0] = 0.01777991514737;
        points[1] = 0.09132360789979;
        points[2] = 0.21430847939563;
        points[3] = 0.37193216458327;
        points[4] = 0.54518668480343;
        points[5] = 0.71317524285557;
        points[6] = 0.85563374295785;
        points[7] = 0.95536604471003;
  
        weights[0]   = 0.04455080436154;
        weights[1]   = 0.09111902363638;
        weights[2]   = 0.11250579947089;
        weights[3]   = 0.10604735943593;
        weights[4]   = 0.07919959949232;
        weights[5]   = 0.04543931950470;
        weights[6]   = 0.01784290265599;
        weights[7]   = 0.00329519144225;
  
        break;
      }
      case 16:
      case 17:
      {
        this->QuadraturePoints->SetNumberOfTuples (9);
        this->QuadratureWeights->SetNumberOfTuples(9);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
  
        points[0] = 0.01441240964887;
        points[1] = 0.07438738970920;
        points[2] = 0.17611665616299;
        points[3] = 0.30966757992764;
        points[4] = 0.46197040108101;
        points[5] = 0.61811723469529;
        points[6] = 0.76282301518504;
        points[7] = 0.88192102121000;
        points[8] = 0.96374218711679;
  
        weights[0]   = 0.03627800352333;
        weights[1]   = 0.07607425510930;
        weights[2]   = 0.09853374217235;
        weights[3]   = 0.10030880919337;
        weights[4]   = 0.08435832184492;
        weights[5]   = 0.05840119529517;
        weights[6]   = 0.03180482149105;
        weights[7]   = 0.01206000428479;
        weights[8]   = 0.00218084708577;
  
        break;
      }
      case 18:
      case 19:
      {
        this->QuadraturePoints->SetNumberOfTuples (10);
        this->QuadratureWeights->SetNumberOfTuples(10);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
  
        points[0] = 0.01191761343242;
        points[1] = 0.06173207187714;
        points[2] = 0.14711144964308;
        points[3] = 0.26115967600846;
        points[4] = 0.39463984688579;
        points[5] = 0.53673876571566;
        points[6] = 0.67594446167666;
        points[7] = 0.80097892103690;
        points[8] = 0.90171098779015;
        points[9] = 0.96997096783851;
  
        weights[0]   = 0.03009950802395;
        weights[1]   = 0.06428715450909;
        weights[2]   = 0.08621130028917;
        weights[3]   = 0.09269689367772;
        weights[4]   = 0.08455710969083;
        weights[5]   = 0.06605307556335;
        weights[6]   = 0.04340190640715;
        weights[7]   = 0.02277459145326;
        weights[8]   = 0.00841931978298;
        weights[9]   = 0.00149914060241;
  
        break;
      }      
      case 20:
      case 21:
      {
        this->QuadraturePoints->SetNumberOfTuples (11);
        this->QuadratureWeights->SetNumberOfTuples(11);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
  
        points[0]  = 0.01001828046168;
        points[1]  = 0.05203545112718;
        points[2]  = 0.12461922514445;
        points[3]  = 0.22284060704384;
        points[4]  = 0.34000815791467;
        points[5]  = 0.46813761308958;
        points[6]  = 0.59849727976714;
        points[7]  = 0.72220328489097;
        points[8]  = 0.83082489962282;
        points[9]  = 0.91695838655260;
        points[10] = 0.97472637960248;
  
        weights[0]   = 0.02536734068817;
        weights[1]   = 0.05493809113287;
        weights[2]   = 0.07562004805718;
        weights[3]   = 0.08465942288402;
        weights[4]   = 0.08187910298806;
        weights[5]   = 0.06953187515818;
        weights[6]   = 0.05159136067230;
        weights[7]   = 0.03264154671383;
        weights[8]   = 0.01666362345168;
        weights[9]   = 0.00604392096048;
        weights[10]  = 0.00106366729324;
  
        break;
      }
      case 22:
      case 23:
      {
        this->QuadraturePoints->SetNumberOfTuples (12);
        this->QuadratureWeights->SetNumberOfTuples(12);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
  
        points[0]  = 0.00853905498844;
        points[1]  = 0.04444646315539;
        points[2]  = 0.10685449088348;
        points[3]  = 0.19215105452985;
        points[4]  = 0.29538088426258;
        points[5]  = 0.41054508120146;
        points[6]  = 0.53095084931282;
        points[7]  = 0.64960065027725;
        points[8]  = 0.75959888952523;
        points[9]  = 0.85455254376493;
        points[10] = 0.92894210126442;
        points[11] = 0.97843793683415;
  
        weights[0]   = 0.02166486088692;
        weights[1]   = 0.04742785198044;
        weights[2]   = 0.06660675062670;
        weights[3]   = 0.07689660268004;
        weights[4]   = 0.07769631681553;
        weights[5]   = 0.07010933999763;
        weights[6]   = 0.05661384371367;
        weights[7]   = 0.04045165370691;
        weights[8]   = 0.02487678040927;
        weights[9]   = 0.01243600916642;
        weights[10]  = 0.00444480779567;
        weights[11]  = 0.00077518222094;
  
        break;
      }
      case 24:
      case 25:
      {
        this->QuadraturePoints->SetNumberOfTuples (13);
        this->QuadratureWeights->SetNumberOfTuples(13);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
  
        points[0]  = 0.00736465102609;
        points[1]  = 0.03839813873968;
        points[2]  = 0.09259522469900;
        points[3]  = 0.16725101139156;
        points[4]  = 0.25862354070576;
        points[5]  = 0.36213139728224;
        points[6]  = 0.47258438600412;
        points[7]  = 0.58444396402134;
        points[8]  = 0.69210100171960;
        points[9]  = 0.79015702827344;
        points[10] = 0.87369482130669;
        points[11] = 0.93852445910073;
        points[12] = 0.98138963498901;
  
        weights[0]   = 0.01871473158558;
        weights[1]   = 0.04132028941977;
        weights[2]   = 0.05895393795654;
        weights[3]   = 0.06971346828202;
        weights[4]   = 0.07284969651569;
        weights[5]   = 0.06881552948745;
        weights[6]   = 0.05912048131399;
        weights[7]   = 0.04599578067597;
        weights[8]   = 0.03193678716215;
        weights[9]   = 0.01921395716469;
        weights[10]  = 0.00944892447958;
        weights[11]  = 0.00333835925077;
        weights[12]  = 0.00057805670494;
              
        break;
      }    
      case 26:
      case 27:
      {
        this->QuadraturePoints->SetNumberOfTuples (14);
        this->QuadratureWeights->SetNumberOfTuples(14);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
  
        points[0]  = 0.00641676079282;
        points[1]  = 0.03350140453201;
        points[2]  = 0.08098549968196;
        points[3]  = 0.14680486768121;
        points[4]  = 0.22808427064926;
        points[5]  = 0.32127174398894;
        points[6]  = 0.42229465730757;
        points[7]  = 0.52673786133987;
        points[8]  = 0.63003668837040;
        points[9]  = 0.72767645288926;
        points[10] = 0.81538973944347;
        points[11] = 0.88934280881952;
        points[12] = 0.94630270006028;
        points[13] = 0.98377523409860;
      
        weights[0]   = 0.01632675432256;
        weights[1]   = 0.03629607805537;
        weights[2]   = 0.05244595350181;
        weights[3]   = 0.06321302557420;
        weights[4]   = 0.06783165960405;
        weights[5]   = 0.06639235062595;
        weights[6]   = 0.05978555902241;
        weights[7]   = 0.04951945452310;
        weights[8]   = 0.03744309645891;
        weights[9]   = 0.02542346540710;
        weights[10]  = 0.01503230374390;
        weights[11]  = 0.00729648413320;
        weights[12]  = 0.00255410131768;
        weights[13]  = 0.00043971370875;
              
        break;
      }     
      case 28:
      case 29:
      {
        this->QuadraturePoints->SetNumberOfTuples (15);
        this->QuadratureWeights->SetNumberOfTuples(15);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
  
        points[0]  = 0.00564068897251;
        points[1]  = 0.02948229864794;
        points[2]  = 0.07141295311516;
        points[3]  = 0.12983102555359;
        points[4]  = 0.20249275505010;
        points[5]  = 0.28660608625753;
        points[6]  = 0.37893868864698;
        points[7]  = 0.47594230846323;
        points[8]  = 0.57388916090669;
        points[9]  = 0.66901519502996;
        points[10] = 0.75766473903134;
        points[11] = 0.83643096060561;
        points[12] = 0.90228670067938;
        points[13] = 0.95270040990583;
        points[14] = 0.98573054526317;
      
        weights[0]   = 0.01436706962168;
        weights[1]   = 0.03211904487489;
        weights[2]   = 0.04689163994396;
        weights[3]   = 0.05739882550681;
        weights[4]   = 0.06291810664846;
        weights[5]   = 0.06334395516137;
        weights[6]   = 0.05917419205958;
        weights[7]   = 0.05141236231177;
        weights[8]   = 0.04140073518433;
        weights[9]   = 0.03060976958640;
        weights[10]  = 0.02041630927257;
        weights[11]  = 0.01190419035507;
        weights[12]  = 0.00571722239471;
        weights[13]  = 0.00198623469314;
        weights[14]  = 0.00034034238414;
              
        break;
      } 
      case 30:
      case 31:
      {
        this->QuadraturePoints->SetNumberOfTuples (16);
        this->QuadratureWeights->SetNumberOfTuples(16);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
  
        points[0]  = 0.00499729966377;
        points[1]  = 0.02614351368139;
        points[2]  = 0.06343094558384;
        points[3]  = 0.11559843757982;
        points[4]  = 0.18087055965788;
        points[5]  = 0.25702480784517;
        points[6]  = 0.34146792754782;
        points[7]  = 0.43132434359563;
        points[8]  = 0.52353411602517;
        points[9]  = 0.61495715187649;
        points[10] = 0.70248013792504;
        points[11] = 0.78312255396487;
        points[12] = 0.85413814777521;
        points[13] = 0.91310837653714;
        points[14] = 0.95802441769048;
        points[15] = 0.98735302062604;
      
        weights[0]   = 0.01273931065730;
        weights[1]   = 0.02861245259007;
        weights[2]   = 0.04212943115733;
        weights[3]   = 0.05222859440383;
        weights[4]   = 0.05825424020971;            
        weights[5]   = 0.06000085488797;
        weights[6]   = 0.05771887960913;
        weights[7]   = 0.05206413784368;
        weights[8]   = 0.04399738218914;
        weights[9]   = 0.03464781657920;
        weights[10]  = 0.02515923310641;
        weights[11]  = 0.01653958435560;
        weights[12]  = 0.00953416788464;
        weights[13]  = 0.00453923238613;
        weights[14]  = 0.00156718847359;
        weights[15]  = 0.00026749366505;
              
        break;
      }
      case 32:
      case 33:
      {
        this->QuadraturePoints->SetNumberOfTuples (17);
        this->QuadratureWeights->SetNumberOfTuples(17);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
  
        points[0]  = 0.00445799356779;
        points[1]  = 0.02334009412377;
        points[2]  = 0.05670796876908;
        points[3]  = 0.10355543293520;
        points[4]  = 0.16246000342814;
        points[5]  = 0.23163212577717;
        points[6]  = 0.30897011775220;
        points[7]  = 0.39212413472210;
        points[8]  = 0.47856759685308;
        points[9]  = 0.56567396688663;
        points[10]  = 0.65079655845338;
        points[11]  = 0.73134895273405;
        points[12]  = 0.80488357852497;
        points[13]  = 0.86916605956741;
        points[14]  = 0.92224303459230;
        points[15]  = 0.96250119782335;
        points[16]  = 0.98871404063224;
        
        weights[0]   = 0.01137270311317;
        weights[1]   = 0.02564242792014;
        weights[2]   = 0.03802572882564;
        weights[3]   = 0.04764186937449;
        weights[4]   = 0.05390793166739;
        weights[5]   = 0.05657361138232;   
        weights[6]   = 0.05573491792341;
        weights[7]   = 0.05180974192916;
        weights[8]   = 0.04547779039902;
        weights[9]   = 0.03759232548701;
        weights[10]   = 0.02907452408763;
        weights[11]   = 0.02080327801024;
        weights[12]   = 0.01351369745227;
        weights[13]   = 0.00771642221479;
        weights[14]   = 0.00364723048341;
        weights[15]   = 0.00125268343939;
        weights[16]   = 0.00021311628921;
       
        break;
      }
      case 34:
      case 35:
      {
        this->QuadraturePoints->SetNumberOfTuples (18);
        this->QuadratureWeights->SetNumberOfTuples(18);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
  
        points[0]  = 0.00400147938387;
        points[1]  = 0.02096364839377;
        points[2]  = 0.05099404158789;
        points[3]  = 0.09328039592854;
        points[4]  = 0.14667010367762;
        points[5]  = 0.20970703958884;
        points[6]  = 0.28067179006017;
        points[7]  = 0.35762864997663;
        points[8]  = 0.43847844922414;
        points[9]  = 0.52101582087078;
        points[10]  = 0.60298936055983;
        points[11]  = 0.68216303911365;
        points[12]  = 0.75637719340615;
        points[13]  = 0.82360742977487;
        points[14]  = 0.88201982536208;
        points[15]  = 0.93002088969969;
        points[16]  = 0.96630075194563;
        points[17]  = 0.98986684820260;
        
        weights[0] = 0.01021433943501;
        weights[1] = 0.02310635016331;
        weights[2] = 0.03447111892183;
        weights[3] = 0.04357390707452;
        weights[4] = 0.04990222691756;
        weights[5] = 0.05319233463169;
        weights[6] = 0.05344528780267;
        weights[7] = 0.05091692885623;
        weights[8] = 0.04608240099011;
        weights[9] = 0.03957916228850;
        weights[10] = 0.03213480612754;
        weights[11] = 0.02448759000122;
        weights[12] = 0.01730830048085;
        weights[13] = 0.01113187009738;
        weights[14] = 0.00630603888236;
        weights[15] = 0.00296244306442;
        weights[16] = 0.00101302517930;
        weights[17] = 0.00017186908410;
       
        break;
      } 
      case 36:
      case 37:
      {
        this->QuadraturePoints->SetNumberOfTuples (19);
        this->QuadratureWeights->SetNumberOfTuples(19);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
              
        points[0] = 0.00361164281856;
        points[1] = 0.01893183703159;
        points[2] = 0.04609793304843;
        points[3] = 0.08444722278421;
        points[4] = 0.13303618855810;
        points[5] = 0.19066859490476;
        points[6] = 0.25592540348210;
        points[7] = 0.32719980076381;
        points[8] = 0.40273678616508;
        points[9] = 0.48067639357855;
        points[10] = 0.55909949264903;
        points[11] = 0.63607504487927;
        points[12] = 0.70970765165390;
        points[13] = 0.77818422297676;
        points[14] = 0.83981861570871;
        points[15] = 0.89309313498184;
        points[16] = 0.93669584807437;
        points[17] = 0.96955263708022;
        points[18] = 0.99085180527096;
  
        weights[0] = 0.00922403772500;
        weights[1] = 0.02092465175192;
        weights[2] = 0.03137627528564;
        weights[3] = 0.03996237004887;
        weights[4] = 0.04623492816844;
        weights[5] = 0.04993413824027;
        weights[6] = 0.05100418662657;
        weights[7] = 0.04959104646856;
        weights[8] = 0.04602198571600;
        weights[9] = 0.04076887087172;
        weights[10] = 0.03439895795181;
        weights[11] = 0.02751805814427;
        weights[12] = 0.02071167256270;
        weights[13] = 0.01448984711207;
        weights[14] = 0.00924109192929;
        weights[15] = 0.00519978132826;
        weights[16] = 0.00243009171203;
        weights[17] = 0.00082788076092;
        weights[18] = 0.00014012759418;
  
        break;
      }
      case 38:
      case 39:
      {
        this->QuadraturePoints->SetNumberOfTuples (20);
        this->QuadratureWeights->SetNumberOfTuples(20);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
              
        points[0] = 0.00327610666905;
        points[1] = 0.01718121814526;
        points[2] = 0.04187143111777;
        points[3] = 0.07680083708962;
        points[4] = 0.12118986732368;
        points[5] = 0.17404711263554;
        points[6] = 0.23419188631359;
        points[7] = 0.30028067683595;
        points[8] = 0.37083718058440;
        points[9] = 0.44428528696301;
        points[10] = 0.51898428870357;
        points[11] = 0.59326553348351;
        points[12] = 0.66546969890551;
        points[13] = 0.73398385823566;
        points[14] = 0.79727750833659;
        points[15] = 0.85393675303589;
        points[16] = 0.90269587179345;
        points[17] = 0.94246554236319;
        points[18] = 0.97235694664744;
        points[19] = 0.99169995579293;
  
        weights[0] = 0.00837084417624;
        weights[1] = 0.01903496946364;
        weights[2] = 0.02866823737904;
        weights[3] = 0.03675023983654;
        weights[4] = 0.04289029583785;
        weights[5] = 0.04684169186242;
        weights[6] = 0.04851610505971;
        weights[7] = 0.04798546160980;
        weights[8] = 0.04547058894121;
        weights[9] = 0.04131770681654;
        weights[10] = 0.03596491915375;
        weights[11] = 0.02990174184254;
        weights[12] = 0.02362529617348;
        weights[13] = 0.01759706687915;
        weights[14] = 0.01220404612664;
        weights[15] = 0.00772766793003;
        weights[16] = 0.00432321911268;
        weights[17] = 0.00201145761396;
        weights[18] = 0.00068306227608;
        weights[19] = 0.00011538190718;
  
        break;
      }
      case 40:
      case 41:
      {
        this->QuadraturePoints->SetNumberOfTuples (21);
        this->QuadratureWeights->SetNumberOfTuples(21);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
              
        points[0] = 0.00298523728321;
        points[1] = 0.01566228055757;
        points[2] = 0.03819828824507;
        points[3] = 0.07013961906233;
        points[4] = 0.11083667332036;
        points[5] = 0.15946112944407;
        points[6] = 0.21502318535802;
        points[7] = 0.27639177909378;
        points[8] = 0.34231763295873;
        points[9] = 0.41145869156191;
        points[10] = 0.48240744461244;
        points[11] = 0.55371958072829;
        points[12] = 0.62394338972128;
        points[13] = 0.69164931501296;
        points[14] = 0.75545905444969;
        points[15] = 0.81407361651270;
        points[16] = 0.86629975897096;
        points[17] = 0.91107426580262;
        points[18] = 0.94748554440640;
        points[19] = 0.97479197566037;
        points[10] = 0.99243549072562;
  
        weights[0] = 0.00763060590649;
        weights[1] = 0.01738792607614;
        weights[2] = 0.02628726008312;
        weights[3] = 0.03388679034430;
        weights[4] = 0.03984597757127;
        weights[5] = 0.04393557063501;
        weights[6] = 0.04605027072311;
        weights[7] = 0.04621255261088;
        weights[8] = 0.04456687468601;
        weights[9] = 0.04136476039687;
        weights[10] = 0.03694201264838;
        weights[11] = 0.03168995305003;
        weights[12] = 0.02602304679581;
        weights[13] = 0.02034554938976;
        weights[14] = 0.01501987261315;
        weights[15] = 0.01033920975794;
        weights[16] = 0.00650659742363;
        weights[17] = 0.00362205286973;
        weights[18] = 0.00167875602156;
        weights[19] = 0.00056849852670;
        weights[20] = 0.00009586186853;
              
        break;
      }
      case 42:
      case 43:
      {
        this->QuadraturePoints->SetNumberOfTuples (22);
        this->QuadratureWeights->SetNumberOfTuples(22);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
              
        points[0] = 0.00273144600888;
        points[1] = 0.01433593348370;
        points[2] = 0.03498632835122;
        points[3] = 0.06430264113216;
        points[4] = 0.10173934345216;
        points[5] = 0.14659920015767;
        points[6] = 0.19804660411818;
        points[7] = 0.25512320697169;
        points[8] = 0.31676578871045;
        points[9] = 0.38182606925675;
        points[10] = 0.44909210098682;
        points[11] = 0.51731084595099;
        points[12] = 0.58521151795130;
        points[13] = 0.65152925489581;
        points[14] = 0.71502868047476;
        points[15] = 0.77452691606482;
        points[16] = 0.82891561362173;
        points[17] = 0.87718159746542;
        points[18] = 0.91842572498985;
        points[19] = 0.95187959322972;
        points[20] = 0.97691966323713;
        points[21] = 0.99307748504435;
  
        weights[0] = 0.00698425611365;
        weights[1] = 0.01594405345473;
        weights[2] = 0.02418423667181;
        weights[3] = 0.03132759699927;
        weights[4] = 0.03707705409308;
        weights[5] = 0.04122240490628;
        weights[6] = 0.04365120378616;
        weights[7] = 0.04435337090028;
        weights[8] = 0.04341875014528;
        weights[9] = 0.04102779578181;
        weights[10] = 0.03743611646681;
        weights[11] = 0.03295405878528;
        weights[12] = 0.02792287082335;
        weights[13] = 0.02268922859809;
        weights[14] = 0.01758001756010;
        weights[15] = 0.01287923028132;
        weights[16] = 0.00880867244008;
        weights[17] = 0.00551387510211;
        weights[18] = 0.00305621411314;
        weights[19] = 0.00141176685313;
        weights[20] = 0.00047692790566;
        weights[21] = 0.00008029821694;
              
        break;
      }                                             
      default:
      {
        vtkErrorMacro("Error: quadrature rule not supported.");
        return;
      }
    }
  }
  else if ((alpha == 2) && (beta == 0))
  {
    switch(this->Order)
    {
      case 0:
      case 1:
      {
        this->QuadraturePoints->SetNumberOfTuples (1);
        this->QuadratureWeights->SetNumberOfTuples(1);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
  
        points[0]  = 0.25000000000000;
  
        weights[0]    = 0.33333333333333;
  
        break;
      }
      case 2:
      case 3:
      {
        this->QuadraturePoints->SetNumberOfTuples (2);
        this->QuadratureWeights->SetNumberOfTuples(2);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
          
        points[0] = 0.12251482265544;
        points[1] = 0.54415184401123;
  
        weights[0]   = 0.23254745125351;
        weights[1]   = 0.10078588207983;
  
        break;
      }
      case 4:
      case 5:
      {
        this->QuadraturePoints->SetNumberOfTuples (3);
        this->QuadratureWeights->SetNumberOfTuples(3);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
          
        points[0] = 0.07299402407315;
        points[1] = 0.34700376603835;
        points[2] = 0.70500220988850;
  
        weights[0]   = 0.15713636106489;
        weights[1]   = 0.14624626925987;
        weights[2]   = 0.02995070300858;
  
        break;
      }
      case 6:
      case 7:
      {
        this->QuadraturePoints->SetNumberOfTuples (4);
        this->QuadratureWeights->SetNumberOfTuples(4);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
          
        points[0] = 0.04850054944700;
        points[1] = 0.23860073755186;
        points[2] = 0.51704729510437;
        points[3] = 0.79585141789677;
  
        weights[0]   = 0.11088841561128;
        weights[1]   = 0.14345878979921;
        weights[2]   = 0.06863388717292;
        weights[3]   = 0.01035224074992;
  
        break;
      }
      case 8:
      case 9:
      {
        this->QuadraturePoints->SetNumberOfTuples (5);
        this->QuadratureWeights->SetNumberOfTuples(5);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
          
        points[0] = 0.03457893991821;
        points[1] = 0.17348032077170;
        points[2] = 0.38988638706552;
        points[3] = 0.63433347263089;
        points[4] = 0.85105421294702;
          
        weights[0]   = 0.08176478428577;
        weights[1]   = 0.12619896189991;
        weights[2]   = 0.08920016122159;
        weights[3]   = 0.03205560072296;
        weights[4]   = 0.00411382520310;
  
        break;
      }
      case 10:
      case 11:
      {
        this->QuadraturePoints->SetNumberOfTuples (6);
        this->QuadratureWeights->SetNumberOfTuples(6);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
          
        points[0] = 0.02590455509367;
        points[1] = 0.13156394165798;
        points[2] = 0.30243691802289;
        points[3] = 0.50903641316475;
        points[4] = 0.71568112731171;
        points[5] = 0.88680561617756;
  
        weights[0]   = 0.06253870272658;
        weights[1]   = 0.10737649973678;
        weights[2]   = 0.09457718674854;
        weights[3]   = 0.05128957112962;
        weights[4]   = 0.01572029718495;
        weights[5]   = 0.00183107580687;
  
        break;
      }
      case 12:
      case 13:
      {
        this->QuadraturePoints->SetNumberOfTuples (7);
        this->QuadratureWeights->SetNumberOfTuples(7);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
          
        points[0] = 0.02013277377340;
        points[1] = 0.10308902914805;
        points[2] = 0.24055412604806;
        points[3] = 0.41400214459706;
        points[4] = 0.60002151327899;
        points[5] = 0.77351724659144;
        points[6] = 0.91118316656300;
  
        weights[0]   = 0.04927650177644;
        weights[1]   = 0.09069882461269;
        weights[2]   = 0.09173380327980;
        weights[3]   = 0.06314637870889;
        weights[4]   = 0.02942221128953;
        weights[5]   = 0.00816292563230;
        weights[6]   = 0.00089268803369;
  
        break;
      }
      case 14:
      case 15:
      {
        this->QuadraturePoints->SetNumberOfTuples (8);
        this->QuadratureWeights->SetNumberOfTuples(8);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
          
        points[0] = 0.01609775955192;
        points[1] = 0.08290061748565;
        points[2] = 0.19547516848874;
        points[3] = 0.34165199147720;
        points[4] = 0.50559707818449;
        points[5] = 0.66955227182436;
        points[6] = 0.81577170358328;
        points[7] = 0.92850896495991;
  
        weights[0]   = 0.03977895780670;
        weights[1]   = 0.07681809326722;
        weights[2]   = 0.08528476917194;
        weights[3]   = 0.06844718342165;
        weights[4]   = 0.04081442638854;
        weights[5]   = 0.01724686378023;
        weights[6]   = 0.00447452171301;
        weights[7]   = 0.00046851778403;
  
        break;
      }
      case 16:
      case 17:
      {
        this->QuadraturePoints->SetNumberOfTuples (9);
        this->QuadratureWeights->SetNumberOfTuples(9);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
  
        points[0] = 0.01316588559711;
        points[1] = 0.06808452959377;
        points[2] = 0.16175951676407;
        points[3] = 0.28589108833922;
        points[4] = 0.42945364538781;
        points[5] = 0.57969405635116;
        points[6] = 0.72326857174034;
        points[7] = 0.84743684201324;
        points[8] = 0.94124586421327;
  
        weights[0]   = 0.03276014511105;
        weights[1]   = 0.06548953703338;
        weights[2]   = 0.07767356916056;
        weights[3]   = 0.06928439568980;
        weights[4]   = 0.04854062786451;
        weights[5]   = 0.02634328090255;
        weights[6]   = 0.01040611657935;
        weights[7]   = 0.00257439864561;
        weights[8]   = 0.00026126234652;
  
        break;
      }
      case 18:
      case 19:
      {
        this->QuadraturePoints->SetNumberOfTuples (10);
        this->QuadratureWeights->SetNumberOfTuples(10);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
  
        points[0] = 0.01096845245617;
        points[1] = 0.05689815053366;
        points[2] = 0.13595023405023;
        points[3] = 0.24228119613252;
        points[4] = 0.36800785044934;
        points[5] = 0.50380712641487;
        points[6] = 0.63960948865471;
        points[7] = 0.76534767954811;
        points[8] = 0.87171007457441;
        points[9] = 0.95087429264052;
  
        weights[0]   = 0.02743408871016;
        weights[1]   = 0.05627293640278;
        weights[2]   = 0.07006950770867;
        weights[3]   = 0.06745221938144;
        weights[4]   = 0.05288378876696;
        weights[5]   = 0.03385456501681;
        weights[6]   = 0.01719757504655;
        weights[7]   = 0.00646988906856;
        weights[8]   = 0.00154552319474;
        weights[9]   = 0.00015324003670;
  
        break;
      }      
      case 20:
      case 21:
      {
        this->QuadraturePoints->SetNumberOfTuples (11);
        this->QuadratureWeights->SetNumberOfTuples(11);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
  
        points[0]  = 0.00927897383134;
        points[1]  = 0.04824969209430;
        points[2]  = 0.11578862662939;
        points[3]  = 0.20766834159706;
        points[4]  = 0.31811795190623;
        points[5]  = 0.44019839985886;
        points[6]  = 0.56623983915457;
        points[7]  = 0.68832423986296;
        points[8]  = 0.79878435859091;
        points[9]  = 0.89069109935439;
        points[10] = 0.95832514378665;
  
        weights[0]   = 0.02330085005155;
        weights[1]   = 0.04874586103051;
        weights[2]   = 0.06297954300041;
        weights[3]   = 0.06418355934975;
        weights[4]   = 0.05463222232105;
        weights[5]   = 0.03929021779844;
        weights[6]   = 0.02358966353881;
        weights[7]   = 0.01141459091810;
        weights[8]   = 0.00414000426322;
        weights[9]   = 0.00096302057554;
        weights[10]  = 0.00009380048601;
  
        break;
      }
      case 22:
      case 23:
      {
        this->QuadraturePoints->SetNumberOfTuples (12);
        this->QuadratureWeights->SetNumberOfTuples(12);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
  
        points[0]  = 0.00795204570266;
        points[1]  = 0.04142781045426;
        points[2]  = 0.09975762554264;
        points[3]  = 0.17981078905241;
        points[4]  = 0.27727345779932;
        points[5]  = 0.38689200999769;
        points[6]  = 0.50275736044903;
        points[7]  = 0.61862386345846;
        points[8]  = 0.72824645295307;
        points[9]  = 0.82571851421479;
        points[10] = 0.90579507354454;
        points[11] = 0.96420653529267;
  
        weights[0]   = 0.02003111258445;
        weights[1]   = 0.04255661536742;
        weights[2]   = 0.05658418474943;
        weights[3]   = 0.06025060748762;
        weights[4]   = 0.05457394116423;
        weights[5]   = 0.04276476357019;
        weights[6]   = 0.02890805183873;
        weights[7]   = 0.01654734364461;
        weights[8]   = 0.00771636702389;
        weights[9]   = 0.00272084214410;
        weights[10]  = 0.00061995339854;
        weights[11]  = 0.00005955035981;
  
        break;
      }
      case 24:
      case 25:
      {
        this->QuadraturePoints->SetNumberOfTuples (13);
        this->QuadratureWeights->SetNumberOfTuples(13);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
                                       
        points[0]  = 0.00689083130996;
        points[1]  = 0.03595336270017;
        points[2]  = 0.08681178015759;
        points[3]  = 0.15709997419651;
        points[4]  = 0.24353289958713;
        points[5]  = 0.34206951264608;
        points[6]  = 0.44810263890571;
        points[7]  = 0.55667462262945;
        points[8]  = 0.66270931356039;
        points[9]  = 0.76124976630828;
        points[10]  = 0.84769104394723;
        points[11]  = 0.91799964553721;
        points[12]  = 0.96892889422859;

        weights[0] = 0.01740122861003;
        weights[1] = 0.03742670816877;
        weights[2] = 0.05090983071260;
        weights[3] = 0.05611153713072;
        weights[4] = 0.05334002160577;
        weights[5] = 0.04462460075553;
        weights[6] = 0.03294045167837;
        weights[7] = 0.02125499671472;
        weights[8] = 0.01172825959308;
        weights[9] = 0.00531356933882;
        weights[10] = 0.00183239113924;
        weights[11] = 0.00041072089179;
        weights[12] = 0.00003901699326;
        
        break;
      } 
      case 26:
      case 27:
      {
        this->QuadraturePoints->SetNumberOfTuples (14);
        this->QuadratureWeights->SetNumberOfTuples(14);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
                                     
        points[0]  = 0.00602880887107;
        points[1]  = 0.03149425981866;
        points[2]  = 0.07621362247485;
        points[3]  = 0.13836652595947;
        points[4]  = 0.21540975706890;
        points[5]  = 0.30418952814166;
        points[6]  = 0.40107137869715;
        points[7]  = 0.50208913974998;
        points[8]  = 0.60310739672760;
        points[9]  = 0.69999091716991;
        points[10]  = 0.78877423843456;
        points[11]  = 0.86582490073160;
        points[12]  = 0.92799573874675;
        points[13]  = 0.97277712074118;

        weights[0] = 0.01525520611942;
        weights[1] = 0.03313959546989;
        weights[2] = 0.04591477211457;
        weights[3] = 0.05202586541059;
        weights[4] = 0.05138992403936;
        weights[5] = 0.04525343225437;
        weights[6] = 0.03573901029244;
        weights[7] = 0.02521618353679;
        weights[8] = 0.01569433228504;
        weights[9] = 0.00841297673712;
        weights[10] = 0.00372487567527;
        weights[11] = 0.00126178389515;
        weights[12] = 0.00027909934095;
        weights[13] = 0.00002627616167;

        break;
      } 
      case 28:
      case 29:
      {
        this->QuadraturePoints->SetNumberOfTuples (15);
        this->QuadratureWeights->SetNumberOfTuples(15);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
                                       
        points[0]  = 0.00531905200284;
        points[1]  = 0.02781456191826;
        points[2]  = 0.06743186583900;
        points[3]  = 0.12274828621550;
        points[4]  = 0.19176570627765;
        points[5]  = 0.27198996592949;
        points[6]  = 0.36052169358706;
        points[7]  = 0.45416123575448;
        points[8]  = 0.54952435559355;
        points[9]  = 0.64316460176878;
        points[10]  = 0.73169797305252;
        points[11]  = 0.81192547727501;
        points[12]  = 0.88094952871531;
        points[13]  = 0.93628182173552;
        points[14]  = 0.97595387433502;
  
        weights[0] = 0.01348166272510;
        weights[1] = 0.02952765075673;
        weights[2] = 0.04153130765151;
        weights[3] = 0.04813205766842;
        weights[4] = 0.04904126789430;
        weights[5] = 0.04499155582051;
        weights[6] = 0.03747387579201;
        weights[7] = 0.02832655317418;
        weights[8] = 0.01929341658523;
        weights[9] = 0.01166858248475;
        weights[10] = 0.00611105011019;
        weights[11] = 0.00265566298577;
        weights[12] = 0.00088657363523;
        weights[13] = 0.00019398726306;
        weights[14] = 0.00001812878557;
        
        break;
      }
      case 30:
      case 31:
      {
        this->QuadraturePoints->SetNumberOfTuples (16);
        this->QuadratureWeights->SetNumberOfTuples(16);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
                                       
        points[0] = 0.00472768712293;
        points[1] = 0.02474296761943;
        points[2] = 0.06007643771664;
        points[3] = 0.10960060925877;
        points[4] = 0.17172475948300;
        points[5] = 0.24445243174451;
        points[6] = 0.32544621002413;
        points[7] = 0.41210296411686;
        points[8] = 0.50163755615319;
        points[9] = 0.59117238531650;
        points[10] = 0.67782991741456;
        points[11] = 0.75882525794914;
        points[12] = 0.83155588302646;
        points[13] = 0.89368597454880;
        points[14] = 0.94322428571225;
        points[15] = 0.97860643749870;
  
        weights[0] = 0.01199940562466;
        weights[1] = 0.02646084251727;
        weights[2] = 0.03768582468182;
        weights[3] = 0.04449657347243;
        weights[4] = 0.04650623939725;
        weights[5] = 0.04411265767579;
        weights[6] = 0.03834804695109;
        weights[7] = 0.03061203791776;
        weights[8] = 0.02235718564947;
        weights[9] = 0.01480377665771;
        weights[10] = 0.00874754549559;
        weights[11] = 0.00449484885147;
        weights[12] = 0.00192353192606;
        weights[13] = 0.00063445999957;
        weights[14] = 0.00013757810398;
        weights[15] = 0.00001277841057;
        
        break;
      }
      case 32:
      case 33:
      {
        this->QuadraturePoints->SetNumberOfTuples (17);
        this->QuadratureWeights->SetNumberOfTuples(17);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
              
        points[0] = 0.00422976548649;
        points[1] = 0.02215270531183;
        points[2] = 0.05385601482718;
        points[3] = 0.09843493683054;
        points[4] = 0.15460792826589;
        points[5] = 0.22075922519215;
        points[6] = 0.29498586629802;
        points[7] = 0.37515254164082;
        points[8] = 0.45895305335302;
        points[9] = 0.54397667809048;
        points[10] = 0.62777753854681;
        points[11] = 0.70794500138509;
        points[12] = 0.78217310308122;
        points[13] = 0.84832708570995;
        points[14] = 0.90450542331433;
        points[15] = 0.94909701659654;
        points[16] = 0.98084389384741;
        
        weights[0] = 0.01074815103074;
        weights[1] = 0.02383776201864;
        weights[2] = 0.03430788979558;
        weights[3] = 0.04114397584696;
        weights[4] = 0.04392261877247;
        weights[5] = 0.04282540392782;
        weights[6] = 0.03855524194700;
        weights[7] = 0.03216357011229;
        weights[8] = 0.02482925611191;
        weights[9] = 0.01763878933100;
        weights[10] = 0.01141364223861;
        weights[11] = 0.00661641976034;
        weights[12] = 0.00334651729181;
        weights[13] = 0.00141388375895;
        weights[14] = 0.00046167405878;
        weights[15] = 0.00009935652534;
        weights[16] = 0.00000918080420;
        
        break;
      }      
      case 34:
      case 35:
      {
        this->QuadraturePoints->SetNumberOfTuples (18);
        this->QuadratureWeights->SetNumberOfTuples(18);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
                                      
        points[0] = 0.00380658224750;
        points[1] = 0.01994835104734;
        points[2] = 0.04854964530422;
        points[3] = 0.08887625911650;
        points[4] = 0.13988457083517;
        points[5] = 0.20025369592363;
        points[6] = 0.26842018278660;
        points[7] = 0.34261859780201;
        points[8] = 0.42092727587032;
        points[9] = 0.50131810275027;
        points[10] = 0.58170905267092;
        points[11] = 0.66001812722532;
        points[12] = 0.73421730813501;
        points[13] = 0.80238514983642;
        points[14] = 0.86275672070006;
        points[15] = 0.91376986411079;
        points[16] = 0.95410789551732;
        points[17] = 0.98274840759429;
        
        weights[0] = 0.00968239880373;
        weights[1] = 0.02157877267913;
        weights[2] = 0.03133390767164;
        weights[3] = 0.03807513425334;
        weights[4] = 0.04137718228740;
        weights[5] = 0.04128366580984;
        weights[6] = 0.03826279467224;
        weights[7] = 0.03309597620198;
        weights[8] = 0.02672342436888;
        weights[9] = 0.02007864169631;
        weights[10] = 0.01394389765361;
        weights[11] = 0.00885227822507;
        weights[12] = 0.00505024878834;
        weights[13] = 0.00252072163843;
        weights[14] = 0.00105355028395;
        weights[15] = 0.00034109120369;
        weights[16] = 0.00007293700866;
        weights[17] = 0.00000671008612;     
        
        break;
      }      
      case 36:
      case 37:
      {
        this->QuadraturePoints->SetNumberOfTuples (19);
        this->QuadratureWeights->SetNumberOfTuples(19);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
                                       
        points[0] = 0.00344389040386;
        points[1] = 0.01805697833790;
        points[2] = 0.04398739509084;
        points[3] = 0.08063327636666;
        points[4] = 0.12713640986156;
        points[5] = 0.18240698366909;
        points[6] = 0.24514956909517;
        points[7] = 0.31389356741822;
        points[8] = 0.38702770276933;
        points[9] = 0.46283779855202;
        points[10] = 0.53954696279209;
        points[11] = 0.61535724480455;
        points[12] = 0.68849179091409;
        points[13] = 0.75723651917330;
        points[14] = 0.81998035706324;
        points[15] = 0.87525316298383;
        points[16] = 0.92176068172914;
        points[17] = 0.95841690242341;
        points[18] = 0.98438280655170;
  
        weights[0] = 0.00876729705685;
        weights[1] = 0.01962085634017;
        weights[2] = 0.02870814977862;
        weights[3] = 0.03527811719937;
        weights[4] = 0.03892238622215;
        weights[5] = 0.03959824821476;
        weights[6] = 0.03760750052024;
        weights[7] = 0.03352506807051;
        weights[8] = 0.02809160793602;
        weights[9] = 0.02209056314976;
        weights[10] = 0.01623181532195;
        weights[11] = 0.01106144062900;
        weights[12] = 0.00691081107791;
        weights[13] = 0.00388978481254;
        weights[14] = 0.00191978847269;
        weights[15] = 0.00079504432545;
        weights[16] = 0.00025553085889;
        weights[17] = 0.00005434264313;
        weights[18] = 0.00000498070233;
  
        break;
      }
      case 38:
      case 39:
      {
        this->QuadraturePoints->SetNumberOfTuples (20);
        this->QuadratureWeights->SetNumberOfTuples(20);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
                                                   
        points[0] = 0.00313068374074;
        points[1] = 0.01642208813399;
        points[2] = 0.04003690046191;
        points[3] = 0.07347719178529;
        points[4] = 0.11603090765549;
        points[5] = 0.16679125348101;
        points[6] = 0.22467641938431;
        points[7] = 0.28845271218127;
        points[8] = 0.35676087010245;
        points[9] = 0.42814504093326;
        points[10] = 0.50108381548285;
        points[11] = 0.57402265818283;
        points[12] = 0.64540704577737;
        points[13] = 0.71371561101529;
        points[14] = 0.77749259203549;
        points[15] = 0.83537891463156;
        points[16] = 0.88614130291455;
        points[17] = 0.92869901491875;
        points[18] = 0.96214871215050;
        points[19] = 0.98579578884064;
        
        weights[0] = 0.00797579273624;
        weights[1] = 0.01791375232568;
        weights[2] = 0.02638256442904;
        weights[3] = 0.03273464431747;
        weights[4] = 0.03658791655311;
        weights[5] = 0.03784739037681;
        weights[6] = 0.03669742047191;
        weights[7] = 0.03355643145337;
        weights[8] = 0.02900240474550;
        weights[9] = 0.02368229041417;
        weights[10] = 0.01822050341821;
        weights[11] = 0.01314091925662;
        weights[12] = 0.00881355607822;
        weights[13] = 0.00543209080611;
        weights[14] = 0.00302248917090;
        weights[15] = 0.00147744444197;
        weights[16] = 0.00060704570427;
        weights[17] = 0.00019388830962;
        weights[18] = 0.00004103910209;
        weights[19] = 0.00000374922099;
        
        break;
      }
      case 40:
      case 41:
      {
        this->QuadraturePoints->SetNumberOfTuples (21);
        this->QuadratureWeights->SetNumberOfTuples(21);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
                                       
        points[0] = 0.00285835060009;
        points[1] = 0.01499936497208;
        points[2] = 0.03659386328114;
        points[3] = 0.06722644166794;
        points[4] = 0.10630147973769;
        points[5] = 0.15305857495572;
        points[6] = 0.20658770608253;
        points[7] = 0.26584701401082;
        points[8] = 0.32968309899010;
        points[9] = 0.39685347698562;
        points[10] = 0.46605076663517;
        points[11] = 0.53592813869303;
        points[12] = 0.60512553397610;
        points[13] = 0.67229614098693;
        points[14] = 0.73613262059729;
        points[15] = 0.79539257374613;
        points[16] = 0.84892277353769;
        points[17] = 0.89568174292547;
        points[18] = 0.93476043640123;
        points[19] = 0.96540160736574;
        points[20] = 0.98702556657876;
        
        weights[0] = 0.00728663173133;
        weights[1] = 0.01641706114419;
        weights[2] = 0.02431607495623;
        weights[3] = 0.03042385465478;
        weights[4] = 0.03438855355052;
        weights[5] = 0.03608527738478;
        weights[6] = 0.03561594477458;
        weights[7] = 0.03328104676273;
        weights[8] = 0.02952807270199;
        weights[9] = 0.02488508387518;
        weights[10] = 0.01988978006629;
        weights[11] = 0.01502452804187;
        weights[12] = 0.01066627036547;
        weights[13] = 0.00705732097434;
        weights[14] = 0.00429931006420;
        weights[15] = 0.00236862153460;
        weights[16] = 0.00114824142071;
        weights[17] = 0.00046857361253;
        weights[18] = 0.00014884999899;
        weights[19] = 0.00003137723340;
        weights[20] = 0.00000285848355;
  
        break;
      }
      case 42:
      case 43:
      {
        this->QuadraturePoints->SetNumberOfTuples (22);
        this->QuadratureWeights->SetNumberOfTuples(22);
        double* points = this->QuadraturePoints->GetPointer(0);
        double* weights = this->QuadratureWeights->GetPointer(0);
                                       
        points[0] = 0.00262007472037;
        points[1] = 0.01375365798739;
        points[2] = 0.03357522684803;
        points[3] = 0.06173556162307;
        points[4] = 0.09773269667537;
        points[5] = 0.14092439775695;
        points[6] = 0.19053995518422;
        points[7] = 0.24569399962450;
        points[8] = 0.30540231844260;
        points[9] = 0.36859942495594;
        points[10] = 0.43415757484016;
        points[11] = 0.50090689264794;
        points[12] = 0.56765625027261;
        points[13] = 0.63321452557266;
        points[14] = 0.69641186298595;
        points[15] = 0.75612055917893;
        points[16] = 0.81127520683922;
        points[17] = 0.86089175276287;
        points[18] = 0.90408517849775;
        points[19] = 0.94008566921082;
        points[20] = 0.96825388381656;
        points[21] = 0.98810245999088;
        
        weights[0] = 0.00668293152387;
        weights[1] = 0.01509806242157;
        weights[2] = 0.02247371373445;
        weights[3] = 0.02832445802520;
        weights[4] = 0.03232947395276;
        weights[5] = 0.03434859966431;
        weights[6] = 0.03442630982802;
        weights[7] = 0.03277458690039;
        weights[8] = 0.02973733662833;
        weights[9] = 0.02574182123471;
        weights[10] = 0.02124416781346;
        weights[11] = 0.01667648237965;
        weights[12] = 0.01240245351092;
        weights[13] = 0.00868669847385;
        weights[14] = 0.00568076838491;
        weights[15] = 0.00342606852096;
        weights[16] = 0.00187138795325;
        weights[17] = 0.00090066625279;
        weights[18] = 0.00036536328467;
        weights[19] = 0.00011551470918;
        weights[20] = 0.00002426317792;
        weights[21] = 0.00000220495706;         

        break;
      }
      default:
      {
        vtkErrorMacro("Quadrature rule not supported.");
        return; 
      }
    }
  }
  else
  {
    vtkErrorMacro("Unsupported combination of Alpha and Beta.");
    return; 
  }
}

void vtkvmtkGaussQuadrature::TensorProductQuad(vtkvmtkGaussQuadrature* q1D)
{
  int numberOf1DQuadraturePoints = q1D->GetNumberOfQuadraturePoints();

  this->QuadraturePoints->SetNumberOfComponents(2);
  this->QuadraturePoints->SetNumberOfTuples(numberOf1DQuadraturePoints * numberOf1DQuadraturePoints);
  this->QuadratureWeights->SetNumberOfTuples(numberOf1DQuadraturePoints * numberOf1DQuadraturePoints);

  double point[3];
  double weight;
  int id;
  int i, j;
  for (j=0; j<numberOf1DQuadraturePoints; j++)
  {
    for (i=0; i<numberOf1DQuadraturePoints; i++)
    {
      id = j * numberOf1DQuadraturePoints + i;
        
      point[0] = q1D->GetQuadraturePoint(i)[0];
      point[1] = q1D->GetQuadraturePoint(j)[0];
      point[2] = 0.0;

      weight = q1D->GetQuadratureWeight(i) * q1D->GetQuadratureWeight(j);
      
      this->QuadraturePoints->SetTuple(id,point);
      this->QuadratureWeights->SetValue(id,weight);
    }
  }
}
  
void vtkvmtkGaussQuadrature::TensorProductTriangle(vtkvmtkGaussQuadrature* gauss1D, vtkvmtkGaussQuadrature* jacA1D)
{
  if (gauss1D->GetNumberOfQuadraturePoints() != jacA1D->GetNumberOfQuadraturePoints())
  {
    vtkErrorMacro("Error: cannot build tensor product rule if rules have different order.");
  }

  int numberOf1DQuadraturePoints = gauss1D->GetNumberOfQuadraturePoints();

  this->QuadraturePoints->SetNumberOfComponents(2);
  this->QuadraturePoints->SetNumberOfTuples(numberOf1DQuadraturePoints * numberOf1DQuadraturePoints);
  this->QuadratureWeights->SetNumberOfTuples(numberOf1DQuadraturePoints * numberOf1DQuadraturePoints);

  double point[2];
  double weight;
  int id;
  int i, j;
  for (j=0; j<numberOf1DQuadraturePoints; j++)
  {
    for (i=0; i<numberOf1DQuadraturePoints; i++)
    {
      id = j * numberOf1DQuadraturePoints + i;
        
      point[0] = jacA1D->GetQuadraturePoint(j)[0];
      point[1] = gauss1D->GetQuadraturePoint(i)[0] * (1.0 - jacA1D->GetQuadraturePoint(j)[0]);

      weight = gauss1D->GetQuadratureWeight(i) * jacA1D->GetQuadratureWeight(j);
      
      this->QuadraturePoints->SetTuple(id,point);
      this->QuadratureWeights->SetValue(id,weight);
    }
  }
}

void vtkvmtkGaussQuadrature::TensorProductHexahedron(vtkvmtkGaussQuadrature* q1D)
{
  int numberOf1DQuadraturePoints = q1D->GetNumberOfQuadraturePoints();

  this->QuadraturePoints->SetNumberOfComponents(3);
  this->QuadraturePoints->SetNumberOfTuples(numberOf1DQuadraturePoints * numberOf1DQuadraturePoints * numberOf1DQuadraturePoints);
  this->QuadratureWeights->SetNumberOfTuples(numberOf1DQuadraturePoints * numberOf1DQuadraturePoints * numberOf1DQuadraturePoints);

  double point[3];
  double weight;
  int id;
  int i, j, k;
  for (k=0; k<numberOf1DQuadraturePoints; k++)
  {
    for (j=0; j<numberOf1DQuadraturePoints; j++)
    {
      for (i=0; i<numberOf1DQuadraturePoints; i++)
      {
        id = k * numberOf1DQuadraturePoints * numberOf1DQuadraturePoints + j * numberOf1DQuadraturePoints + i;
          
        point[0] = q1D->GetQuadraturePoint(i)[0];
        point[1] = q1D->GetQuadraturePoint(j)[0];
        point[2] = q1D->GetQuadraturePoint(k)[0];
  
        weight = q1D->GetQuadratureWeight(i) * q1D->GetQuadratureWeight(j) * q1D->GetQuadratureWeight(k);
        
        this->QuadraturePoints->SetTuple(id,point);
        this->QuadratureWeights->SetValue(id,weight);
      }
    }
  }
}
  
void vtkvmtkGaussQuadrature::TensorProductWedge(vtkvmtkGaussQuadrature* q1D, vtkvmtkGaussQuadrature* q2D)
{
  int numberOf1DQuadraturePoints = q1D->GetNumberOfQuadraturePoints();
  int numberOf2DQuadraturePoints = q2D->GetNumberOfQuadraturePoints();

  this->QuadraturePoints->SetNumberOfComponents(3);
  this->QuadraturePoints->SetNumberOfTuples(numberOf1DQuadraturePoints * numberOf2DQuadraturePoints);
  this->QuadratureWeights->SetNumberOfTuples(numberOf1DQuadraturePoints * numberOf2DQuadraturePoints);

  double point[3];
  double weight;
  int id;
  int i, j;
  for (j=0; j<numberOf1DQuadraturePoints; j++)
  {
    for (i=0; i<numberOf2DQuadraturePoints; i++)
    {
      id = j * numberOf2DQuadraturePoints + i;
        
      point[0] = q2D->GetQuadraturePoint(i)[0];
      point[1] = q2D->GetQuadraturePoint(i)[1];
      point[2] = q1D->GetQuadraturePoint(j)[0];

      weight = q2D->GetQuadratureWeight(i) * q1D->GetQuadratureWeight(j);
      
      this->QuadraturePoints->SetTuple(id,point);
      this->QuadratureWeights->SetValue(id,weight);
    }
  }
}
  
void vtkvmtkGaussQuadrature::TensorProductTetra(vtkvmtkGaussQuadrature* gauss1D, vtkvmtkGaussQuadrature* jacA1D, vtkvmtkGaussQuadrature* jacB1D)
{
  if (gauss1D->GetNumberOfQuadraturePoints() != jacA1D->GetNumberOfQuadraturePoints())
  {
    vtkErrorMacro("Error: cannot build tensor product rule if rules have different order.");
  }

  if (jacA1D->GetNumberOfQuadraturePoints() != jacB1D->GetNumberOfQuadraturePoints())
  {
    vtkErrorMacro("Error: cannot build tensor product rule if rules have different order.");
  }

  int numberOf1DQuadraturePoints = gauss1D->GetNumberOfQuadraturePoints();

  this->QuadraturePoints->SetNumberOfComponents(3);
  this->QuadraturePoints->SetNumberOfTuples(numberOf1DQuadraturePoints * numberOf1DQuadraturePoints * numberOf1DQuadraturePoints);
  this->QuadratureWeights->SetNumberOfTuples(numberOf1DQuadraturePoints * numberOf1DQuadraturePoints * numberOf1DQuadraturePoints);
 
  double point[3];
  double weight;
  int id;
  int i, j, k;
  for (k=0; k<numberOf1DQuadraturePoints; k++)
  {
    for (j=0; j<numberOf1DQuadraturePoints; j++)
    {
      for (i=0; i<numberOf1DQuadraturePoints; i++)
      {
        id = k * numberOf1DQuadraturePoints * numberOf1DQuadraturePoints + j * numberOf1DQuadraturePoints + i;
          
        point[0] = jacB1D->GetQuadraturePoint(k)[0];
        point[1] = jacA1D->GetQuadraturePoint(j)[0] * (1.0 - jacB1D->GetQuadraturePoint(k)[0]);
        point[2] = gauss1D->GetQuadraturePoint(i)[0] * (1.0 - jacA1D->GetQuadraturePoint(j)[0]) * (1.0 - jacB1D->GetQuadraturePoint(k)[0]);
  
        weight = gauss1D->GetQuadratureWeight(i) * jacA1D->GetQuadratureWeight(j) * jacB1D->GetQuadratureWeight(k);
        
        this->QuadraturePoints->SetTuple(id,point);
        this->QuadratureWeights->SetValue(id,weight);
      }
    }
  }
}

