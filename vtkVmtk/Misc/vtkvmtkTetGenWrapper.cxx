/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkTetGenWrapper.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:48 $
Version:   $Revision: 1.8 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkTetGenWrapper.h"
#include "vtkvmtkConstants.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkIntArray.h"
#include "vtkStdString.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "tetgen.h"


vtkStandardNewMacro(vtkvmtkTetGenWrapper);

vtkvmtkTetGenWrapper::vtkvmtkTetGenWrapper()
{
  this->PLC = 0;                // -p switch, 0
  this->Refine = 0;             // -r switch, 0
  this->Coarsen = 0;            // -R switch, 0
  this->Quality = 0;            // -q switch, 0
  this->NoBoundarySplit = 0;    // -Y switch, 0
  this->VarVolume = 0;          // -a switch without number, 0
  this->FixedVolume = 0;        // -a switch with number, 0
  this->MaxVolume = -1.0;       //    number after -a, -1.0
  this->RemoveSliver = 0;       // -s switch, 0
  this->MinRatio = 2.0;         //    number after -q, 2.0
  this->MinDihedral = 5.0;      //    number after -qq, 5.0
  this->MaxDihedral = 165.0;    //    number after -qqq, 165.0
  this->RegionAttrib = 0;       // -A switch, 0
  this->Epsilon = 1.0e-8;       // number after -T switch, 1.0e-8
  this->NoMerge = 0;            // -M switch, 0
  this->DetectInter = 0;        // -d switch, 0
  this->CheckClosure = 0;       // -c switch, 0
  this->Order = 1;              // number after -o switch, 1 (e.g. -o2 for quadratic elements)
  this->DoCheck = 0;            // -C switch, 0
  this->UseSizingFunction = 0;  // -m switch, 0
  
  this->Verbose = 0;

  this->CellEntityIdsArrayName = NULL;
  this->TetrahedronVolumeArrayName = NULL;
  this->SizingFunctionArrayName = NULL;

  this->OutputSurfaceElements = 1;
  this->OutputVolumeElements = 1;

  this->LastRunExitStatus = -1;
}

vtkvmtkTetGenWrapper::~vtkvmtkTetGenWrapper()
{
  if (this->CellEntityIdsArrayName)
    {
    delete[] this->CellEntityIdsArrayName;
    this->CellEntityIdsArrayName = NULL;
    }

 if (this->TetrahedronVolumeArrayName)
    {
    delete[] this->TetrahedronVolumeArrayName;
    this->TetrahedronVolumeArrayName = NULL;
    }

 if (this->SizingFunctionArrayName)
    {
    delete[] this->SizingFunctionArrayName;
    this->SizingFunctionArrayName = NULL;
    }
}

int vtkvmtkTetGenWrapper::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkUnstructuredGrid *input = vtkUnstructuredGrid::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (this->VarVolume && !this->TetrahedronVolumeArrayName)
    {
    vtkErrorMacro(<<"VarVolumeOn but TetrahedronVolumeArrayName not specified");
    return 1;
    }

  vtkStdString tetgenOptionString;

  char buffer[64];

  if (this->PLC)
    {
    tetgenOptionString += "p";  
    }

  if (this->Refine)
    {
    tetgenOptionString += "r";  
    }

  if (this->Coarsen)
    {
    tetgenOptionString += "R";
    }

  if (this->Quality)
    {
    tetgenOptionString += "q";  
    sprintf(buffer,"%f",this->MinRatio);
    tetgenOptionString += buffer;
    tetgenOptionString += "q";  
    sprintf(buffer,"%f",this->MinDihedral);
    tetgenOptionString += buffer;
    tetgenOptionString += "q";  
    sprintf(buffer,"%f",this->MaxDihedral);
    tetgenOptionString += buffer;
    }

  if (this->NoBoundarySplit)
    {
    tetgenOptionString += "Y";  
    }

  if (this->RemoveSliver)
    {
    tetgenOptionString += "s";  
    }

  if (this->Order == 2)
    {
    tetgenOptionString += "o2";  
    }
  
  tetgenOptionString += "T";
  sprintf(buffer,"%e",this->Epsilon);
  tetgenOptionString += buffer;

  if (this->VarVolume)
    {
    tetgenOptionString += "a";
    }

  if (this->FixedVolume)
    {
    tetgenOptionString += "a";
    sprintf(buffer,"%f",this->MaxVolume);
    tetgenOptionString += buffer;
    }

  if (this->RegionAttrib)
    {
    tetgenOptionString += "A";
    }

  if (this->NoMerge)
    {
    tetgenOptionString += "M";
    }

  if (this->DetectInter)
    {
    tetgenOptionString += "d";
    }

  if (this->DoCheck)
    {
    tetgenOptionString += "C";
    }

  if (this->CheckClosure)
    {
    tetgenOptionString += "c";
    }

  tetgenOptionString += "z";  
// tetgenOptionString += "Y";  

  if (this->Verbose)
    {
    tetgenOptionString += "V";  
    }
  else
    {
    tetgenOptionString += "Q";  
    }

  vtkDataArray* sizingFunctionArray = NULL;
  if (this->SizingFunctionArrayName)
    {
    sizingFunctionArray = input->GetPointData()->GetArray(this->SizingFunctionArrayName);
    }

  if (this->UseSizingFunction && sizingFunctionArray)
    {
    tetgenOptionString += "m"; 
    }

  tetgenio in_tetgenio;
  tetgenio out_tetgenio;

  in_tetgenio.firstnumber = 0;

  const int meshDimensionality = 3; //TODO: set this
  in_tetgenio.mesh_dim = meshDimensionality;

  //TODO - all point arrays except marker array
  in_tetgenio.pointattributelist = NULL; //REAL*
  in_tetgenio.numberofpointattributes = 0;
  //TODO

  in_tetgenio.pointmtrlist = NULL;
  in_tetgenio.numberofpointmtrs = 0;

  int numberOfPoints = input->GetNumberOfPoints();

  in_tetgenio.numberofpoints = numberOfPoints;
  in_tetgenio.pointlist = new REAL[meshDimensionality * numberOfPoints];
#if 0
  if (pointMarkerArray)
    {
    in_tetgenio.pointmarkerlist = new int[numberOfPoints];
    }
#endif
  if (sizingFunctionArray)
    {
    in_tetgenio.numberofpointmtrs = 1;
    in_tetgenio.pointmtrlist = new REAL[numberOfPoints];
    }

  double point[3];
  int i;
  for (i=0; i<numberOfPoints; i++)
    {
    input->GetPoint(i,point);
    in_tetgenio.pointlist[meshDimensionality * i + 0] = point[0];
    in_tetgenio.pointlist[meshDimensionality * i + 1] = point[1];
    in_tetgenio.pointlist[meshDimensionality * i + 2] = point[2];
#if 0
    if (pointMarkerArray)
      {
      in_tetgenio.pointmarkerlist[i] = static_cast<int>(pointMarkerArray->GetComponent(i,0));
      }
    else
      {
      in_tetgenio.pointmarkerlist[i] = 0;
      }
#endif
    if (sizingFunctionArray)
      {
      in_tetgenio.pointmtrlist[i] = sizingFunctionArray->GetComponent(i,0);
      if (in_tetgenio.pointmtrlist[i] == 0.0)
        {
        in_tetgenio.pointmtrlist[i] = VTK_VMTK_FLOAT_TOL;
        }
      }
    }

  vtkIdList* facetCellIds = vtkIdList::New();
  vtkIdList* tetraCellIds = vtkIdList::New();

  int numberOfCells = input->GetNumberOfCells();
  for (i=0; i<numberOfCells; i++)
    {
    vtkCell* cell = input->GetCell(i);
    int cellType = cell->GetCellType();
    switch (cellType)
      {
      case VTK_TRIANGLE: 
      case VTK_QUADRATIC_TRIANGLE:
      case VTK_POLYGON:
        facetCellIds->InsertNextId(i);
        break;
      case VTK_TETRA:
        if (this->Order != 1)
          {
          vtkErrorMacro(<<"Element of incorrect order found.");
          break;
          }
        tetraCellIds->InsertNextId(i);
        break;
      case VTK_QUADRATIC_TETRA:
        if (this->Order != 2)
          {
          vtkErrorMacro(<<"Element of incorrect order found.");
          break;
          }
        tetraCellIds->InsertNextId(i);
        break;
      default:
        vtkErrorMacro(<<"Invalid element found, cellId "<<i<<", cellType "<<cellType);
        break;
      }
    }

  int numberOfFacets = facetCellIds->GetNumberOfIds();
  int numberOfTetras = tetraCellIds->GetNumberOfIds();

  //TODO - input as vtkPointSet (point inside hole ([0],[1],[2]))
  in_tetgenio.holelist = NULL; //REAL* 
  in_tetgenio.numberofholes = 0; 
  //TODO
  
  if (numberOfFacets > 0)
    {
    in_tetgenio.numberoffacets = numberOfFacets;
    in_tetgenio.facetlist = new tetgenio::facet[numberOfFacets];
    in_tetgenio.facetmarkerlist = new int[numberOfFacets];

    vtkDataArray* facetMarkerArray = input->GetCellData()->GetArray(this->CellEntityIdsArrayName);

    for (i=0; i<numberOfFacets; i++)
      {
      vtkIdType npts, *pts;
      input->GetCellPoints(facetCellIds->GetId(i),npts,pts);
      in_tetgenio.facetlist[i].numberofpolygons = 1;
      in_tetgenio.facetlist[i].polygonlist = new tetgenio::polygon[in_tetgenio.facetlist[i].numberofpolygons];
      in_tetgenio.facetlist[i].numberofholes = 0;
      in_tetgenio.facetlist[i].holelist = NULL;
      in_tetgenio.facetlist[i].polygonlist[0].numberofvertices = npts;
      in_tetgenio.facetlist[i].polygonlist[0].vertexlist = new int[npts];
      for (int j=0; j<npts; j++)
        {
        in_tetgenio.facetlist[i].polygonlist[0].vertexlist[j] = pts[j];
        }

      if (facetMarkerArray)
        {
        in_tetgenio.facetmarkerlist[i] = static_cast<int>(facetMarkerArray->GetComponent(facetCellIds->GetId(i),0));
        }
      else
        {
        in_tetgenio.facetmarkerlist[i] = 0;
        }
      }
    }

  //TODO - all cell arrays except volume array
  in_tetgenio.tetrahedronattributelist = NULL; //REAL* 
  in_tetgenio.numberoftetrahedronattributes = 0; 
  //TODO

  if (numberOfTetras > 0)
    {
    vtkDataArray* tetrahedronVolumeArray = NULL;
    if (this->VarVolume)
      {
      tetrahedronVolumeArray = input->GetCellData()->GetArray(this->TetrahedronVolumeArrayName);
      }

    in_tetgenio.numberoftetrahedra = numberOfTetras;
    switch (this->Order)
      {
      case 1:
        in_tetgenio.numberofcorners = 4;
        break;
      case 2:
        in_tetgenio.numberofcorners = 10;
        break;
      default:
        in_tetgenio.numberofcorners = 0;
        break;
      }
  
    in_tetgenio.tetrahedronlist = new int[in_tetgenio.numberofcorners * numberOfTetras];

    if (tetrahedronVolumeArray)
      {
      in_tetgenio.tetrahedronvolumelist = new REAL[numberOfTetras];
      }

    for (i=0; i<numberOfTetras; i++)
      {
      vtkIdType npts, *pts;
      input->GetCellPoints(tetraCellIds->GetId(i),npts,pts);
      for (int j=0; j<npts; j++)
        {
        in_tetgenio.tetrahedronlist[i*in_tetgenio.numberofcorners + j] = pts[j];
        }

      if (tetrahedronVolumeArray)
        {
        in_tetgenio.tetrahedronvolumelist[i] = tetrahedronVolumeArray->GetComponent(i,0);
        }
      }
    }

  facetCellIds->Delete();
  tetraCellIds->Delete();

  //TODO - input as vtkPointSet + arrays for attributes (point inside region ([0],[1],[2]), regional attribute [3], volume constraint [4])
  in_tetgenio.regionlist = NULL; //REAL* 
  in_tetgenio.numberofregions = 0; 
  //TODO

  char tetgenOptions[512];
  strcpy(tetgenOptions,tetgenOptionString.c_str());
  cout<<"TetGen command line options: "<<tetgenOptions<<endl;
  try
    {
    tetrahedralize(tetgenOptions,&in_tetgenio,&out_tetgenio);
    }
  catch ( ... )
    {
    vtkErrorMacro(<<"TetGen quit with an exception.");
    this->LastRunExitStatus = 1;
    return 1;
    }
    
  this->LastRunExitStatus = 0;

  //TODO
//  out_tetgenio.edgelist; //int* 
//  out_tetgenio.edgemarkerlist; //int* 
//  out_tetgenio.numberofedges; 
//  out_tetgenio.neighborlist; //int* 
  //TODO

  vtkPoints* outputPoints = vtkPoints::New();

  outputPoints->SetNumberOfPoints(out_tetgenio.numberofpoints);
  for (i=0; i<out_tetgenio.numberofpoints; i++)
    {
    point[0] = out_tetgenio.pointlist[meshDimensionality * i + 0];
    point[1] = out_tetgenio.pointlist[meshDimensionality * i + 1];
    point[2] = out_tetgenio.pointlist[meshDimensionality * i + 2];
    outputPoints->SetPoint(i,point);
    }

  output->SetPoints(outputPoints);

  vtkCellArray* outputCellArray = vtkCellArray::New();

  int numberOfOutputCells = 0;

  if (this->OutputVolumeElements)
    {
    numberOfOutputCells += out_tetgenio.numberoftetrahedra;
    }

  if (this->OutputSurfaceElements)
    {
    numberOfOutputCells += out_tetgenio.numberoftrifaces;
    }

  int* outputCellTypes = new int[numberOfOutputCells];

  vtkIntArray* outputCellMarkerArray = vtkIntArray::New();
  outputCellMarkerArray->SetNumberOfTuples(numberOfOutputCells);
  outputCellMarkerArray->SetName(this->CellEntityIdsArrayName);
  outputCellMarkerArray->FillComponent(0,0.0);

  int cellIdOffset = 0;

#if 1 
  if (this->OutputVolumeElements)
    {
    int outputCellType;
    switch (this->Order)
      {
      case 1:
        outputCellType = VTK_TETRA;
        break;
      case 2:
        outputCellType = VTK_QUADRATIC_TETRA;
        break;
      default:
        outputCellType = VTK_TETRA;
      }
    
    for (i=0; i<out_tetgenio.numberoftetrahedra; i++)
      {
      outputCellArray->InsertNextCell(out_tetgenio.numberofcorners);
      for (int j=0; j<out_tetgenio.numberofcorners; j++)
        {
        outputCellArray->InsertCellPoint(out_tetgenio.tetrahedronlist[i*out_tetgenio.numberofcorners + j]);
        }
      outputCellTypes[cellIdOffset+i] = outputCellType;
      }

    cellIdOffset = out_tetgenio.numberoftetrahedra;
    }

  if (this->OutputSurfaceElements)
    {
    int outputCellType = VTK_TRIANGLE;
    int numberOfTrifaceCorners = 3;

    for (i=0; i<out_tetgenio.numberoftrifaces; i++)
      {
      outputCellArray->InsertNextCell(numberOfTrifaceCorners);
      for (int j=0; j<numberOfTrifaceCorners; j++)
        {
        outputCellArray->InsertCellPoint(out_tetgenio.trifacelist[i*numberOfTrifaceCorners + j]);
        }
      outputCellTypes[cellIdOffset+i] = outputCellType;
      outputCellMarkerArray->SetValue(cellIdOffset+i,out_tetgenio.trifacemarkerlist[i]);
      }

    cellIdOffset = out_tetgenio.numberoftrifaces;
    }
#else
  if (this->OutputSurfaceElements)
    {
    int outputCellType = VTK_TRIANGLE;
    int numberOfTrifaceCorners = 3;

    for (i=0; i<out_tetgenio.numberoftrifaces; i++)
      {
      outputCellArray->InsertNextCell(numberOfTrifaceCorners);
      for (int j=0; j<numberOfTrifaceCorners; j++)
        {
        outputCellArray->InsertCellPoint(out_tetgenio.trifacelist[i*numberOfTrifaceCorners + j]);
        }
      outputCellTypes[cellIdOffset+i] = outputCellType;
      outputCellMarkerArray->SetValue(cellIdOffset+i,out_tetgenio.trifacemarkerlist[i]+1);
      }

    cellIdOffset = out_tetgenio.numberoftrifaces;
    }

  if (this->OutputVolumeElements)
    {
    int outputCellType;
    switch (this->Order)
      {
      case 1:
        outputCellType = VTK_TETRA;
        break;
      case 2:
        outputCellType = VTK_QUADRATIC_TETRA;
        break;
      default:
        outputCellType = VTK_TETRA;
      }
    
    for (i=0; i<out_tetgenio.numberoftetrahedra; i++)
      {
      outputCellArray->InsertNextCell(out_tetgenio.numberofcorners);
      for (int j=0; j<out_tetgenio.numberofcorners; j++)
        {
        outputCellArray->InsertCellPoint(out_tetgenio.tetrahedronlist[i*out_tetgenio.numberofcorners + j]);
        }
      outputCellTypes[cellIdOffset+i] = outputCellType;
      }

    cellIdOffset = out_tetgenio.numberoftetrahedra;
    }
#endif

  output->SetCells(outputCellTypes,outputCellArray);

  output->GetCellData()->AddArray(outputCellMarkerArray);

  outputPoints->Delete();
  outputCellArray->Delete();
  delete[] outputCellTypes;

  outputCellMarkerArray->Delete();

  return 1;
}

void vtkvmtkTetGenWrapper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
