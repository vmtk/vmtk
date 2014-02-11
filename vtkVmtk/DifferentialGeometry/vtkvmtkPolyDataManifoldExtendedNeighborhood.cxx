/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataManifoldExtendedNeighborhood.cxx,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
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

#include "vtkvmtkPolyDataManifoldExtendedNeighborhood.h"

#include "vtkObjectFactory.h"
#include "vtkIdList.h"
#include "vtkCell.h"
#include "vtkMath.h"


vtkStandardNewMacro(vtkvmtkPolyDataManifoldExtendedNeighborhood);

void vtkvmtkPolyDataManifoldExtendedNeighborhood::Build()
  {
  vtkIdType i, j, k;
  vtkIdType p, p1, p2, outerP;
  vtkIdType pointId;
  double outerPoint[3], point1[3], point2[3];
  double edgeVector[3], outerVector1[3], outerVector2[3];
  vtkIdList *cellIds, *ptIds, *extendedStencilIds;
  vtkCell* cell;
  vtkPolyData* pdata = vtkPolyData::SafeDownCast(this->DataSet);

  if (pdata==NULL)
    {
    vtkErrorMacro(<< "Input data NULL or not poly data");
    }

  pointId = this->DataSetPointId;

  this->Superclass::Build();

  cellIds = vtkIdList::New();
  ptIds = vtkIdList::New();
  extendedStencilIds = vtkIdList::New();

  if (!this->IsBoundary)
    {
    if ((this->NPoints==3)||(this->NPoints==4))
      {
      for (i=0; i<this->NPoints; i++)
        {
        extendedStencilIds->InsertNextId(this->PointIds[i]);

        p1 = this->PointIds[i];
        p2 = this->PointIds[(i+1)%this->NPoints];
        pdata->GetCellEdgeNeighbors (-1, p1, p2, cellIds);
        if (cellIds->GetNumberOfIds()>1)
          {
          outerP = -1;
          for (j=0; j<2; j++)
            {
            cell = pdata->GetCell(cellIds->GetId(j));
            for (k=0; k<3; k++)
              {
              p = cell->GetPointId(k);
              if (p!=pointId  && p!=p1 && p!=p2)
                {
                outerP = p;
                break;
                }
              }
            if (outerP != -1)
              {
              break;
              }
            }
	
          pdata->GetPoint(p1,point1);
          pdata->GetPoint(p2,point2);
          pdata->GetPoint(outerP,outerPoint);
          edgeVector[0] = point1[0] - point2[0];
          edgeVector[1] = point1[1] - point2[1];
          edgeVector[2] = point1[2] - point2[2];
          outerVector1[0] = outerPoint[0] - point1[0];
          outerVector1[1] = outerPoint[1] - point1[1];
          outerVector1[2] = outerPoint[2] - point1[2];
          outerVector2[0] = outerPoint[0] - point2[0];
          outerVector2[1] = outerPoint[1] - point2[1];
          outerVector2[2] = outerPoint[2] - point2[2];

          if (vtkMath::Dot(edgeVector,outerVector1)*vtkMath::Dot(edgeVector,outerVector2) < 0.0)
            extendedStencilIds->InsertNextId(outerP);
          }
        }

      this->NPoints = extendedStencilIds->GetNumberOfIds();
      if (this->PointIds!=NULL)
        {
        delete[] this->PointIds;
        this->PointIds = NULL;
        }
      this->PointIds = new vtkIdType[this->NPoints];
      memcpy(this->PointIds,extendedStencilIds->GetPointer(0),this->NPoints*sizeof(vtkIdType));
      }
    }

  cellIds->Delete();
  ptIds->Delete();
  extendedStencilIds->Delete();
}

