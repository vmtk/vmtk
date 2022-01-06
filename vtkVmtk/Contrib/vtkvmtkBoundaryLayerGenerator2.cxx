/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkBoundaryLayerGenerator2.cxx,v $
  Language:  C++
  Date:      $$
  Version:   $$

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

  Note: this class was contributed by 
	Tangui Morvan
	Kalkulo AS
	Simula Research Laboratory
=========================================================================*/

//TODO: Bug in WarpPoints in parent class?

#include "vtkvmtkBoundaryLayerGenerator2.h"
#include "vtkvmtkConstants.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkIntArray.h"
#include "vtkMath.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkCellData.h"
#include "vtkGeometryFilter.h"
#include "vtkOrderedTriangulator.h"
#include "vtkVersion.h"

#include "vtkvmtkPolyDataBoundaryExtractor.h"


vtkStandardNewMacro(vtkvmtkBoundaryLayerGenerator2);

vtkvmtkBoundaryLayerGenerator2::vtkvmtkBoundaryLayerGenerator2()
{
  this->CellEntityIdsArrayName = NULL;
  this->OpenProfilesIdsArrayName = NULL;
  this->IncludeExtrudedOpenProfilesCells = 0;
  this->IncludeExtrudedSurfaceCells = 0;
  this->IncludeOriginalSurfaceCells = 0;
  this->LayerEntityId = 0;
  this->SurfaceEntityId = 1;
  this->OpenProfilesEntityId = 2;
  this->Triangulator = vtkOrderedTriangulator::New();
}

vtkvmtkBoundaryLayerGenerator2::~vtkvmtkBoundaryLayerGenerator2()
{
  if (this->CellEntityIdsArrayName)
    {
    delete[] this->CellEntityIdsArrayName;
    this->CellEntityIdsArrayName = NULL;
    }
  if (this->OpenProfilesIdsArrayName)
    {
    delete[] this->OpenProfilesIdsArrayName;
    this->OpenProfilesIdsArrayName = NULL;
    }
  this->Triangulator->Delete();
}

int vtkvmtkBoundaryLayerGenerator2::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkUnstructuredGrid *input = vtkUnstructuredGrid::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkPoints* inputPoints = input->GetPoints();

  vtkIdType numberOfInputPoints = inputPoints->GetNumberOfPoints();
  vtkIdType numberOfInputCells = input->GetNumberOfCells();

  if (!this->WarpVectorsArrayName)
    {
    vtkErrorMacro("WarpVectors array name not specified.");
    return 1;
    }

  if (!input->GetPointData()->GetArray(this->WarpVectorsArrayName))
    {
    vtkErrorMacro(<< "WarpVectors array with name specified does not exist!");
    return 1;
    }

  this->WarpVectorsArray = input->GetPointData()->GetArray(this->WarpVectorsArrayName);
  
  if ((!this->UseWarpVectorMagnitudeAsThickness) && (!this->ConstantThickness))
    {
    if (!this->LayerThicknessArrayName)
      {
      vtkErrorMacro("LayerThickness array name not specified.");
      return 1;
      }

    if (!input->GetPointData()->GetArray(this->LayerThicknessArrayName))
      {
      vtkErrorMacro(<< "LayerThickness array with name specified does not exist!");
      return 1;
      }

    this->LayerThicknessArray = input->GetPointData()->GetArray(this->LayerThicknessArrayName);
    }
  
  vtkPoints* outputPoints = vtkPoints::New();
  vtkPoints* warpedPoints = vtkPoints::New();

  vtkCellArray* boundaryLayerCellArray = vtkCellArray::New();
  vtkIdList* boundaryLayerCellTypes = vtkIdList::New();
  
  vtkIntArray *cellEntityIdsArray = NULL;
  
  if ((this->CellEntityIdsArrayName) && (strcmp(this->CellEntityIdsArrayName,"") != 0)) 
  {
    cellEntityIdsArray = vtkIntArray::New();
    cellEntityIdsArray->SetName(this->CellEntityIdsArrayName);
    cellEntityIdsArray->SetNumberOfComponents(1);
    }
  //Keep track of the id to use
  int currentSurfaceEntityId = this->SurfaceEntityId;

  int cellType;
  cellType = input->GetCellType(0);  // TODO: check if all elements are consistent
  bool warpQuadratic = false;
  if (cellType == VTK_QUADRATIC_TRIANGLE)
    {
    warpQuadratic = true;
    }

  vtkIdType numberOfLayerPoints = numberOfInputPoints;
  if (warpQuadratic)
    {
    numberOfLayerPoints = 2 * numberOfInputPoints;
    }  

  outputPoints->SetNumberOfPoints(numberOfInputPoints + numberOfLayerPoints * this->NumberOfSubLayers);

  double point[3];
  int i;
  for (i=0; i<numberOfInputPoints; i++)
    {
    inputPoints->GetPoint(i,point);
    outputPoints->SetPoint(i,point);
    }

  vtkIdType npts;
  const vtkIdType *pts;

  vtkIdType *surfacePts;

  if (this->IncludeSurfaceCells || this->IncludeOriginalSurfaceCells)
    {
    for (i=0; i<numberOfInputCells; i++)
      {
      input->GetCellPoints(i,npts,pts);
      cellType = input->GetCellType(i);
      surfacePts = new vtkIdType[npts];
      switch(cellType)
        {
        case VTK_TRIANGLE:
          boundaryLayerCellTypes->InsertNextId(VTK_TRIANGLE);
          surfacePts[0] = pts[0];
          surfacePts[1] = pts[1];
          surfacePts[2] = pts[2];
          break;
        case VTK_QUAD:
          boundaryLayerCellTypes->InsertNextId(VTK_QUAD);
          surfacePts[0] = pts[0];
          surfacePts[1] = pts[1];
          surfacePts[2] = pts[2];
          surfacePts[3] = pts[3];
          break;
        case VTK_QUADRATIC_TRIANGLE:
          boundaryLayerCellTypes->InsertNextId(VTK_QUADRATIC_TRIANGLE);
          surfacePts[0] = pts[0];
          surfacePts[1] = pts[1];
          surfacePts[2] = pts[2];
          surfacePts[3] = pts[3];
          surfacePts[4] = pts[4];
          surfacePts[5] = pts[5];
          break;
        default:
          vtkErrorMacro(<<"Unsupported surface element.");
          return 1;
          break;
        }
      boundaryLayerCellArray->InsertNextCell(npts,surfacePts);
      if (cellEntityIdsArray)
        {
        cellEntityIdsArray->InsertNextValue(currentSurfaceEntityId);
        }
      delete[] surfacePts;
      }
      //Switch to next surface entity
      currentSurfaceEntityId++;
    }

  if (this->InnerSurface)
    {
    this->InnerSurface->Delete();
    this->InnerSurface = NULL;
    }

  this->InnerSurface = vtkUnstructuredGrid::New();
  this->InnerSurface->DeepCopy(input);
  vtkPoints* innerSurfacePoints = vtkPoints::New();
  this->WarpPoints(inputPoints,innerSurfacePoints,this->NumberOfSubLayers-1,warpQuadratic);
  this->InnerSurface->GetPoints()->DeepCopy(innerSurfacePoints);

  //Get the ids of the open profiles if necessary
  vtkDataArray *openProfilesIdsArray = NULL;
  bool allocateArray = false;
  
  
  if (this->IncludeSurfaceCells || this->IncludeExtrudedOpenProfilesCells)
    {
    //Find or build the ids of the open profiles
    //TODO: Check that the open profiles ids are consistent
    if (this->OpenProfilesIdsArrayName)
      {
      openProfilesIdsArray = input->GetPointData()->GetArray(this->OpenProfilesIdsArrayName);
      }
      
    allocateArray = (openProfilesIdsArray == NULL);
    if (allocateArray)
      {
      //The open profiles ids have to be created
      openProfilesIdsArray = vtkIntArray::New();
      openProfilesIdsArray->SetName("OpenProfilesIds");
      openProfilesIdsArray->SetNumberOfComponents(1);
      openProfilesIdsArray->SetNumberOfTuples(numberOfInputPoints);
      openProfilesIdsArray->FillComponent(0, -1);
      
      //First convert the unstructured grid to poly data
      vtkGeometryFilter *meshToSurface = vtkGeometryFilter::New();
      meshToSurface->SetInputData(input);
      meshToSurface->MergingOff();
      meshToSurface->Update();
      
      //Extract the open profiles
      vtkvmtkPolyDataBoundaryExtractor *openProfilesExtractor = vtkvmtkPolyDataBoundaryExtractor::New();
      openProfilesExtractor->SetInputData(meshToSurface->GetOutput());
      openProfilesExtractor->Update();
              
      //Update the openProfilesIdsArray
      vtkCellArray *openProfiles = openProfilesExtractor->GetOutput()->GetLines();
      vtkDataArray *openProfilesScalars = openProfilesExtractor->GetOutput()->GetPointData()->GetScalars();
      
      vtkIdType npts = 0;
      const vtkIdType *pts = NULL;
      int profileId;
      for (profileId=0, openProfiles->InitTraversal(); openProfiles->GetNextCell(npts,pts); profileId++)
        {
        for (int i=0;i<npts;i++)
          {
          openProfilesIdsArray->SetComponent(openProfilesScalars->GetComponent(pts[i],0),0,profileId);
          }
        }
        
      openProfilesExtractor->Delete();
      meshToSurface->Delete();
      }
      
    }


  //Add the volumetric elements and the open profiles extensions if necessary
  boundaryLayerCellArray->InitTraversal();
  int k;
  for (k=0; k<this->NumberOfSubLayers; k++)
    {
    warpedPoints->Initialize();
    this->WarpPoints(inputPoints,warpedPoints,k,warpQuadratic);

    for (i=0; i<numberOfLayerPoints; i++)
      {
      warpedPoints->GetPoint(i,point);
      outputPoints->SetPoint(i + numberOfInputPoints + k*numberOfLayerPoints,point);
      }
   
    vtkIdType prismNPts, *prismPts;
    vtkIdType nTetraPts = 0;
    const vtkIdType *tetraPts = NULL;

    for (i=0; i<numberOfInputCells; i++)
      {
      input->GetCellPoints(i,npts,pts);
      cellType = input->GetCellType(i);
      if (cellType == VTK_TRIANGLE || cellType == VTK_QUAD)
        {
        prismNPts = npts * 2;
        prismPts = new vtkIdType[prismNPts];
        int j;
        for (j=0; j<npts; j++)
          {
          prismPts[j] = pts[j] + k*numberOfLayerPoints;
          }
        for (j=0; j<npts; j++)
          {
          prismPts[j+npts] = pts[j] + (k+1)*numberOfLayerPoints;
          }
        
        //Tetrahedralize the cell
        double *pCoords = NULL;
        int prismNEdges = 0;
        int prismType = 0;
        
        if (cellType == VTK_TRIANGLE)
          {
          prismType = VTK_WEDGE;
          //Wedge parametric coords
          double pCoordsWedge[] = {0.0,0.0,0.0, 1.0,0.0,0.0, 0.0,1.0,0.0,
                     0.0,0.0,1.0, 1.0,0.0,1.0, 0.0,1.0,1.0};
          pCoords = pCoordsWedge;
          prismNEdges = 9;
          }
        else if (cellType == VTK_QUAD)
          {
          prismType = VTK_HEXAHEDRON;;
          //Hexahedron parametric coords
          double pCoordsHex[] = {0.0,0.0,0.0, 1.0,0.0,0.0,
                                 1.0,1.0,0.0, 0.0,1.0,0.0,
                                 0.0,0.0,1.0, 1.0,0.0,1.0,
                                 1.0,1.0,1.0, 0.0,1.0,1.0};
          pCoords = pCoordsHex;
          prismNEdges = 12;
          }
          
        this->Triangulator->InitTriangulation(0.0,1.0, 0.0,1.0, 0.0,1.0, prismNPts);
        
        double prismPt[3];
        double *p;
        for (p=pCoords, j=0; j<prismNPts; j++, p+=3)
          {
          outputPoints->GetPoint(j, prismPt);
          this->Triangulator->InsertPoint(prismPts[j], prismPt, p, 0);
          }
        
        this->Triangulator->TemplateTriangulate(prismType, prismNPts, prismNEdges);
        
        //Go to the end of the list
        while (boundaryLayerCellArray->GetNextCell(nTetraPts,tetraPts));
        //Location from which the tetras will be inserted
        vtkIdType tetraStartLoc = boundaryLayerCellArray->GetTraversalLocation();
        //Insert the new tetras 
        vtkIdType numTets = this->Triangulator->AddTetras(0,boundaryLayerCellArray);
        
        for (j=0; j<numTets; j++)
          {
          boundaryLayerCellTypes->InsertNextId(VTK_TETRA);
          if (cellEntityIdsArray) 
            {
            cellEntityIdsArray->InsertNextValue(this->LayerEntityId);
            }
          }


        //Add the extruded open profiles
        if (openProfilesIdsArray)
          {          
          for (j=0;j<npts;j++) 
            {
            //TODO: what to do if the profileid is not -1 but different between the 2 points?
            if ((openProfilesIdsArray->GetComponent(pts[j],0) != -1) && (openProfilesIdsArray->GetComponent(pts[(j+1)%npts],0) != -1))
              {
              //The points forming the extruded open profile
              vtkIdList *openProfilePts = vtkIdList::New();
              openProfilePts->InsertNextId(prismPts[j]);
              openProfilePts->InsertNextId(prismPts[(j+1)%npts]);
              openProfilePts->InsertNextId(prismPts[j+npts]);
              openProfilePts->InsertNextId(prismPts[(j+1)%npts + npts]);
              
              vtkIdList *tetraPtsList = vtkIdList::New();
              //Go through the tetrahedra we just added
              boundaryLayerCellArray->SetTraversalLocation(tetraStartLoc);
              
              for (int k=0; k<numTets; k++)
                {
                boundaryLayerCellArray->GetNextCell(nTetraPts,tetraPts);
                tetraPtsList->Initialize();
                for (int l=0; l<nTetraPts; l++) 
                  {
                  tetraPtsList->InsertNextId(tetraPts[l]);
                  }
                tetraPtsList->IntersectWith(*openProfilePts);
                //If this tetrahedra has a face on the extruded open profile, include it
                if (tetraPtsList->GetNumberOfIds() == 3)
                  {
                  boundaryLayerCellArray->InsertNextCell(tetraPtsList);
                  boundaryLayerCellTypes->InsertNextId(VTK_TRIANGLE);
                  if (cellEntityIdsArray)
                    {
                    cellEntityIdsArray->InsertNextValue(this->OpenProfilesEntityId + openProfilesIdsArray->GetComponent(pts[j],0));
                    }
                  }
                }
                tetraPtsList->Delete();
                openProfilePts->Delete();
              }
            }
          }
          
        delete[] prismPts;
        }
        //FIXME: NOT HANDLED
//       else if (cellType == VTK_QUADRATIC_TRIANGLE)
//         {
//          prismNPts = npts * 3 - 3;
// //        prismNPts = npts * 3;
//         prismPts = new vtkIdType[prismNPts];
// 
//         boundaryLayerCellTypes->InsertNextId(VTK_QUADRATIC_WEDGE);
//         
//         prismPts[0] = pts[0] + k*numberOfLayerPoints;
//         prismPts[1] = pts[1] + k*numberOfLayerPoints;
//         prismPts[2] = pts[2] + k*numberOfLayerPoints;
// 
//         prismPts[3] = pts[0] + k*numberOfLayerPoints + numberOfLayerPoints;
//         prismPts[4] = pts[1] + k*numberOfLayerPoints + numberOfLayerPoints;
//         prismPts[5] = pts[2] + k*numberOfLayerPoints + numberOfLayerPoints;
// 
//         prismPts[6] = pts[3] + k*numberOfLayerPoints;
//         prismPts[7] = pts[4] + k*numberOfLayerPoints;
//         prismPts[8] = pts[5] + k*numberOfLayerPoints;
// 
//         prismPts[9] = pts[3] + k*numberOfLayerPoints + numberOfLayerPoints;
//         prismPts[10] = pts[4] + k*numberOfLayerPoints + numberOfLayerPoints;
//         prismPts[11] = pts[5] + k*numberOfLayerPoints + numberOfLayerPoints;
// 
//         prismPts[12] = pts[0] + k*numberOfLayerPoints + numberOfLayerPoints/2;
//         prismPts[13] = pts[1] + k*numberOfLayerPoints + numberOfLayerPoints/2;
//         prismPts[14] = pts[2] + k*numberOfLayerPoints + numberOfLayerPoints/2;
// 
//         // TODO: this creates a 18-noded wedge, which is not supported by VTK, but it works as a 15-node (the last 3 points are ignored). Better solutions? Could put it in as a vtkGenericCell, but harder to identify it afterwards
// //        prismPts[15] = pts[3] + k*numberOfLayerPoints + numberOfLayerPoints/2;
// //        prismPts[16] = pts[4] + k*numberOfLayerPoints + numberOfLayerPoints/2;
// //        prismPts[17] = pts[5] + k*numberOfLayerPoints + numberOfLayerPoints/2;
// 
//         boundaryLayerCellArray->InsertNextCell(prismNPts,prismPts);
//         if (cellEntityIdsArray) cellEntityIdsArray->InsertNextValue(this->LayerEntityId);
//         delete[] prismPts;
//         }
      else
        {
        vtkErrorMacro(<<"Unsupported surface element.");
        return 1;
        }
      }
    }
    
    if (allocateArray)
      {
      openProfilesIdsArray->Delete();
      }
    

  if (this->IncludeSurfaceCells || this->IncludeExtrudedSurfaceCells)
    {
    for (i=0; i<numberOfInputCells; i++)
      {
      input->GetCellPoints(i,npts,pts);
      cellType = input->GetCellType(i);
      surfacePts = new vtkIdType[npts];
      switch(cellType)
        {
        case VTK_TRIANGLE:
          boundaryLayerCellTypes->InsertNextId(VTK_TRIANGLE);
          surfacePts[0] = pts[0] + this->NumberOfSubLayers*numberOfLayerPoints;
          surfacePts[1] = pts[1] + this->NumberOfSubLayers*numberOfLayerPoints;
          surfacePts[2] = pts[2] + this->NumberOfSubLayers*numberOfLayerPoints;
          break;
        case VTK_QUAD:
          boundaryLayerCellTypes->InsertNextId(VTK_QUAD);
          surfacePts[0] = pts[0] + this->NumberOfSubLayers*numberOfLayerPoints;
          surfacePts[1] = pts[1] + this->NumberOfSubLayers*numberOfLayerPoints;
          surfacePts[2] = pts[2] + this->NumberOfSubLayers*numberOfLayerPoints;
          surfacePts[3] = pts[3] + this->NumberOfSubLayers*numberOfLayerPoints;
          break;
        case VTK_QUADRATIC_TRIANGLE:
          boundaryLayerCellTypes->InsertNextId(VTK_QUADRATIC_TRIANGLE);
          surfacePts[0] = pts[0] + this->NumberOfSubLayers*numberOfLayerPoints;
          surfacePts[1] = pts[1] + this->NumberOfSubLayers*numberOfLayerPoints;
          surfacePts[2] = pts[2] + this->NumberOfSubLayers*numberOfLayerPoints;
          surfacePts[3] = pts[3] + this->NumberOfSubLayers*numberOfLayerPoints;
          surfacePts[4] = pts[4] + this->NumberOfSubLayers*numberOfLayerPoints;
          surfacePts[5] = pts[5] + this->NumberOfSubLayers*numberOfLayerPoints;
          break;
        default:
          vtkErrorMacro(<<"Unsupported surface element.");
          return 1;
          break;
        }
      boundaryLayerCellArray->InsertNextCell(npts,surfacePts);
      if (cellEntityIdsArray) cellEntityIdsArray->InsertNextValue(currentSurfaceEntityId);
      delete[] surfacePts;
      }
    }

  output->SetPoints(outputPoints);

  int* boundaryLayerCellTypesInt = new int[boundaryLayerCellTypes->GetNumberOfIds()];
  for (i=0; i<boundaryLayerCellTypes->GetNumberOfIds(); i++)
    {
    boundaryLayerCellTypesInt[i] = boundaryLayerCellTypes->GetId(i);
    }

  output->SetCells(boundaryLayerCellTypesInt,boundaryLayerCellArray);
  
  delete[] boundaryLayerCellTypesInt;
  
  if (cellEntityIdsArray) 
    {
    output->GetCellData()->AddArray(cellEntityIdsArray);
    cellEntityIdsArray->Delete();
    }

  outputPoints->Delete();
  warpedPoints->Delete();
  boundaryLayerCellArray->Delete();
  boundaryLayerCellTypes->Delete();
  innerSurfacePoints->Delete();
 
  return 1;
}

void vtkvmtkBoundaryLayerGenerator2::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
