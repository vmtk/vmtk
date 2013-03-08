/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkUnstructuredGridTetraFilter.h,v $
Language:  C++
Date:      $Date: 2006/07/17 09:53:14 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkUnstructuredGridTetraFilter.h"

#include "vtkCellData.h"
#include "vtkCellLinks.h"
#include "vtkCellType.h"
#include "vtkGenericCell.h"
#include "vtkIdList.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkOrderedTriangulator.h"
#include "vtkPointData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnsignedCharArray.h"

vtkStandardNewMacro(vtkvmtkUnstructuredGridTetraFilter);

vtkvmtkUnstructuredGridTetraFilter::vtkvmtkUnstructuredGridTetraFilter()
{
  this->Triangulator = vtkOrderedTriangulator::New();
  this->Triangulator->PreSortedOff();
  this->Triangulator->UseTemplatesOn();
  this->TetrahedraOnly = 0;
}

vtkvmtkUnstructuredGridTetraFilter::~vtkvmtkUnstructuredGridTetraFilter()
{
  this->Triangulator->Delete();
  this->Triangulator = NULL;
}

int vtkvmtkUnstructuredGridTetraFilter::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and output
  vtkUnstructuredGrid *input = vtkUnstructuredGrid::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  this->Execute(input, output);

  vtkDebugMacro(<<"Produced " << this->GetOutput()->GetNumberOfCells() << " cells");

  return 1;
}

int vtkvmtkUnstructuredGridTetraFilter::TriangulateQuad(vtkUnstructuredGrid* output, vtkIdList *quadPtIds, vtkIdList *ptIds)
{
  vtkIdList* neighborCellIds = vtkIdList::New();

  ptIds->Initialize();
  ptIds->InsertId(0,quadPtIds->GetId(0));
  ptIds->InsertId(1,quadPtIds->GetId(1));
  ptIds->InsertId(2,quadPtIds->GetId(2));

  output->GetCellNeighbors(-1,ptIds,neighborCellIds);

  if (neighborCellIds->GetNumberOfIds() > 0)
    {
    ptIds->InsertId(3,quadPtIds->GetId(0));
    ptIds->InsertId(4,quadPtIds->GetId(2));
    ptIds->InsertId(5,quadPtIds->GetId(3));
    }
  else
    {
    ptIds->InsertId(0,quadPtIds->GetId(0));
    ptIds->InsertId(1,quadPtIds->GetId(1));
    ptIds->InsertId(2,quadPtIds->GetId(3));
    ptIds->InsertId(3,quadPtIds->GetId(1));
    ptIds->InsertId(4,quadPtIds->GetId(2));
    ptIds->InsertId(5,quadPtIds->GetId(3));
    }
 
  neighborCellIds->Delete();

  return 1;
}


// 3D cells use the ordered triangulator. The ordered triangulator is used
// to create templates on the fly. Once the templates are created then they
// are used to produce the final triangulation.
//
void vtkvmtkUnstructuredGridTetraFilter::Execute(vtkUnstructuredGrid *dataSetInput,
                                                 vtkUnstructuredGrid *output)
{
  vtkPointSet *input = static_cast<vtkPointSet*>(dataSetInput); //has to be
  vtkIdType numCells = input->GetNumberOfCells();
  vtkGenericCell *cell;
  vtkIdType newCellId, j;
  int k;
  vtkCellData *inCD=input->GetCellData();
  vtkCellData *outCD=output->GetCellData();
  vtkPoints *cellPts;
  vtkIdList *cellPtIds;
  vtkIdType ptId, numTets, ncells;
  int numPts, type;
  int numSimplices, dim;
  vtkIdType pts[4];
  double x[3];

  if (numCells == 0)
    {
    return;
    }

  //avoid doing cell simplification if all cells are already simplices
  vtkUnsignedCharArray* cellTypes = dataSetInput->GetCellTypesArray();
  if (cellTypes)
    {
    int allsimplices = 1;
    for (vtkIdType cellId = 0; cellId < cellTypes->GetSize() && allsimplices; cellId++)
      {
      switch (cellTypes->GetValue(cellId))
        {
        case VTK_TETRA:
          break;
        case VTK_VERTEX:
        case VTK_LINE:
        case VTK_TRIANGLE:          
          if (this->TetrahedraOnly)
            {
            allsimplices = 0;
            }
          break;
        default:
          allsimplices = 0;
          break;
        }
      }
    if (allsimplices)
      {
      output->ShallowCopy(input);
      return;
      }
    }

  cell = vtkGenericCell::New();
  cellPts = vtkPoints::New();
  cellPtIds = vtkIdList::New();

  // Create an array of points
  vtkCellData *tempCD = vtkCellData::New();
  tempCD->ShallowCopy(inCD);
  tempCD->SetActiveGlobalIds(NULL);

  outCD->CopyAllocate(tempCD, input->GetNumberOfCells()*5);
  output->Allocate(input->GetNumberOfCells()*5);
  
  // Points are passed through
  output->SetPoints(input->GetPoints());
  output->GetPointData()->PassData(input->GetPointData());

  for (vtkIdType cellId=0; cellId < numCells; cellId++)
    {
    input->GetCell(cellId, cell);
    dim = cell->GetCellDimension();

    if ( dim != 3 ) //use ordered triangulation
      {
      continue;
      }
      
    numPts = cell->GetNumberOfPoints();
    double *p, *pPtr=cell->GetParametricCoords();
    this->Triangulator->InitTriangulation(0.0,1.0, 0.0,1.0, 0.0,1.0, numPts);
    for (p=pPtr, j=0; j<numPts; j++, p+=3)
      {
      ptId = cell->PointIds->GetId(j);
      cell->Points->GetPoint(j, x);
      this->Triangulator->InsertPoint(ptId, x, p, 0);
      }//for all cell points
    if ( cell->IsPrimaryCell() ) //use templates if topology is fixed
      {
      int numEdges=cell->GetNumberOfEdges();
      this->Triangulator->TemplateTriangulate(cell->GetCellType(),
                                              numPts,numEdges);
      }
    else //use ordered triangulator
      {
      this->Triangulator->Triangulate();
      }

    ncells = output->GetNumberOfCells();
    numTets = this->Triangulator->AddTetras(0,output);
      
    for (j=0; j < numTets; j++)
      {
      outCD->CopyData(tempCD, cellId, ncells+j);
      }
    }

  if (!this->TetrahedraOnly) //2D or lower dimension
    {
    output->BuildLinks();

    for (vtkIdType cellId=0; cellId < numCells; cellId++)
      {
      input->GetCell(cellId, cell);
      dim = cell->GetCellDimension();

      if (dim != 2) 
        {
        continue;
        }

      dim++;

      if  (cell->GetCellType() == VTK_QUAD)
        {
        this->TriangulateQuad(output,cell->GetPointIds(),cellPtIds);
        }
      else
        {
        cell->Triangulate(0, cellPtIds, cellPts);
        }

      numPts = cellPtIds->GetNumberOfIds();
      
      numSimplices = numPts / dim;
      type = 0;
      switch (dim)
        {
        case 1:
          type = VTK_VERTEX;    break;
        case 2:
          type = VTK_LINE;      break;
        case 3:
          type = VTK_TRIANGLE;  break;
        }

      for ( j=0; j < numSimplices; j++ )
        {
        for (k=0; k<dim; k++)
          {
          pts[k] = cellPtIds->GetId(dim*j+k);
          }
        newCellId = output->InsertNextCell(type, dim, pts);
        outCD->CopyData(tempCD, cellId, newCellId);
        }
      } 

    //links->Delete();
    }
  
  output->Squeeze();
  
  tempCD->Delete();

  cellPts->Delete();
  cellPtIds->Delete();
  cell->Delete();
}

void vtkvmtkUnstructuredGridTetraFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "TetrahedraOnly: " << (this->TetrahedraOnly ? "On":"Off") << "\n";
}

