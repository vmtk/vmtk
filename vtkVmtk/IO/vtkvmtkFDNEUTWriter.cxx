/*=========================================================================
                                                                                                                                    
Program:   VMTK
Module:    $RCSfile: vtkvmtkFDNEUTWriter.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:47 $
Version:   $Revision: 1.6 $
                                                                                                                                    
  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
                                                                                                                                    
=========================================================================*/

#include "vtkvmtkFDNEUTWriter.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellType.h"
#include "vtkObjectFactory.h"
#include "vtkvmtkConstants.h"


vtkStandardNewMacro(vtkvmtkFDNEUTWriter);

vtkvmtkFDNEUTWriter::vtkvmtkFDNEUTWriter()
{
}

vtkvmtkFDNEUTWriter::~vtkvmtkFDNEUTWriter()
{
}

void vtkvmtkFDNEUTWriter::WriteData()
{
  vtkUnstructuredGrid *input= vtkUnstructuredGrid::SafeDownCast(this->GetInput());

  if (!this->GetFileName())
    {
    vtkErrorMacro(<<"FileName not set.");
    return;
    }
        
  FILE* FDNEUTFile = fopen(this->GetFileName(),"w");

  if (!FDNEUTFile)
    {
    vtkErrorMacro(<<"Could not open file for writing.");
    return;
    }

  int numberOfNodes, numberOfElements, numberOfGroups;

  numberOfNodes = input->GetNumberOfPoints();
  numberOfElements = input->GetNumberOfCells();

  int cellType;
  const int numberOfCellTypes = VTK_QUADRATIC_PYRAMID+1;
  int numberOfTypeCells[numberOfCellTypes];
  int firstCellIdOfType[numberOfCellTypes];
  int k;
  for (k=0; k<numberOfCellTypes; k++)
    {
    numberOfTypeCells[k] = 0;
    firstCellIdOfType[k] = -1;
    }
  for (k=0; k<numberOfElements; k++)
    {
    cellType = input->GetCellType(k);
    switch (cellType)
      {
      case VTK_QUAD:
      case VTK_QUADRATIC_QUAD:
      case VTK_BIQUADRATIC_QUAD:
      case VTK_TRIANGLE:
      case VTK_QUADRATIC_TRIANGLE:
      case VTK_HEXAHEDRON:
      case VTK_QUADRATIC_HEXAHEDRON:
      case VTK_TRIQUADRATIC_HEXAHEDRON:
      case VTK_WEDGE:
      case VTK_QUADRATIC_WEDGE:
      case VTK_TETRA:
      case VTK_QUADRATIC_TETRA:
        ++numberOfTypeCells[cellType];
        firstCellIdOfType[cellType] = k;
        break;
      default:
        vtkWarningMacro(<<"Unsupported cell type found");
        break;
      }
    }
  numberOfGroups = 0;
  for (k=0; k<numberOfCellTypes; k++)
    {
    if (numberOfTypeCells[k] > 0)
      {
      ++numberOfGroups;
      }
    }

  fprintf(FDNEUTFile,"** FIDAP NEUTRAL FILE\n");
  fprintf(FDNEUTFile,"foo\n");   // TODO: set user-defined title
  fprintf(FDNEUTFile,"VERSION    8.6\n");
  fprintf(FDNEUTFile," Jan 2004     \n");
  fprintf(FDNEUTFile,"   NO. OF NODES   NO. ELEMENTS NO. ELT GROUPS          NDFCD          NDFVL\n");
  fprintf(FDNEUTFile,"%15d%15d%15d%15d%15d\n",numberOfNodes,numberOfElements,numberOfGroups,3,3);
  fprintf(FDNEUTFile,"   STEADY/TRANS     TURB. FLAG FREE SURF FLAG    COMPR. FLAG   RESULTS ONLY\n");
  fprintf(FDNEUTFile,"              0              0              0              0              0\n");
  fprintf(FDNEUTFile,"TEMPERATURE/SPECIES FLAGS\n");
  fprintf(FDNEUTFile," 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0\n");
  fprintf(FDNEUTFile,"PRESSURE FLAGS - IDCTS, IPENY MPDF\n");
  fprintf(FDNEUTFile,"         1         1         0\n");

  fprintf(FDNEUTFile,"NODAL COORDINATES\n");
  double point[3];
  for (k=0; k<numberOfNodes; k++)
    {
    input->GetPoint(k,point);
    fprintf(FDNEUTFile,"%10d%20.10e%20.10e%20.10e\n",k+1,point[0],point[1],point[2]);
    }

  fprintf(FDNEUTFile,"BOUNDARY CONDITIONS\n");
  fprintf(FDNEUTFile,"         0         0         0     0.0\n");

  fprintf(FDNEUTFile,"ELEMENT GROUPS\n");

  int cellCount = 1;
  int groupCount = 1;

  //  for (cellType=0; cellType<numberOfCellTypes; cellType++)
  for (cellType=numberOfCellTypes-1; cellType>=0; cellType--)
    {
    if (numberOfTypeCells[cellType] == 0)
      {
      continue;
      }

    int fdneutElementType, fdneutElementGeometry, numberOfNodesInElement;
    vtkIdType npts, *pts, *cellPoints;
    fdneutElementType = -1;
    fdneutElementGeometry = -1;

    input->GetCellPoints(firstCellIdOfType[cellType],npts,pts);
    numberOfNodesInElement = npts;
    switch (cellType)
      {
      case VTK_QUAD:
      case VTK_QUADRATIC_QUAD:
        fdneutElementType = 15;
        fdneutElementGeometry = QUADRILATERAL;
        break;
      case VTK_BIQUADRATIC_QUAD:
        fdneutElementType = 15;
        fdneutElementGeometry = QUADRILATERAL;
        break;
      case VTK_TRIANGLE:
      case VTK_QUADRATIC_TRIANGLE:
        fdneutElementType = 15;
        fdneutElementGeometry = TRIANGLE;
        break;
      case VTK_HEXAHEDRON:
      case VTK_QUADRATIC_HEXAHEDRON:
        fdneutElementType = BRICK;
        fdneutElementGeometry = BRICK;
        break;
      case VTK_TRIQUADRATIC_HEXAHEDRON:
        fdneutElementType = BRICK;
        fdneutElementGeometry = BRICK;
        break;
      case VTK_WEDGE:
      case VTK_QUADRATIC_WEDGE:
        fdneutElementType = WEDGE;
        fdneutElementGeometry = WEDGE;
        break;
      case VTK_TETRA:
      case VTK_QUADRATIC_TETRA:
        fdneutElementType = TETRAHEDRON;
        fdneutElementGeometry = TETRAHEDRON;
        break;
      }

    int groupNumber, numberOfElementsInGroup;
    groupNumber = groupCount;
    numberOfElementsInGroup = numberOfTypeCells[cellType];

    fprintf(FDNEUTFile,"GROUP:    %5d ELEMENTS:%10d NODES:   %10d GEOMETRY:%5d TYPE:%4d\n",groupNumber,numberOfElementsInGroup,numberOfNodesInElement,fdneutElementGeometry,fdneutElementType);
    fprintf(FDNEUTFile,"ENTITY NAME:   Entity%d\n",groupNumber);

    for (k=0; k<numberOfElements; k++)
      {
      if (input->GetCellType(k) != cellType)
        {
        continue;
        }
      input->GetCellPoints(k,npts,pts);
      cellPoints = new vtkIdType[npts];
      memcpy(cellPoints,pts,npts*sizeof(vtkIdType));
      this->ZeroToOneOffset(npts,cellPoints);

      if (npts != numberOfNodesInElement)
        {
        vtkErrorMacro(<<"Can't handle same cell types with different number of points");
        return;
        }

      fprintf(FDNEUTFile,"%8d",(int)cellCount);
   
      int i;
      switch (cellType)
        {
        case VTK_QUAD:
          for (i=0; i<numberOfNodesInElement; i++)
            {
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[i]);
            }
          break;
        case VTK_QUADRATIC_QUAD:
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[0]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[4]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[1]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[5]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[2]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[6]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[3]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[7]);
          if (numberOfNodesInElement==9)
            {
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[8]);
            }
          break;
        case VTK_BIQUADRATIC_QUAD:
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[0]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[4]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[1]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[5]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[2]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[6]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[3]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[7]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[8]);
          break;
        case VTK_TRIANGLE:
          for (i=0; i<numberOfNodesInElement; i++)
            {
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[i]);
            }
          break;
        case VTK_QUADRATIC_TRIANGLE:
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[0]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[3]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[1]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[4]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[2]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[5]);
          break;
        case VTK_HEXAHEDRON:
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[0]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[1]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[3]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[2]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[4]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[5]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[7]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[6]);
          break;
        case VTK_TRIQUADRATIC_HEXAHEDRON:
          if (numberOfNodesInElement != 27)
            {
            vtkErrorMacro(<< "Only 27-noded hexahedra are supported in FDNEUT.");
            }
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[0]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[8]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[1]);

          fprintf(FDNEUTFile,"%8d",(int)cellPoints[11]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[24]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[9]);

          fprintf(FDNEUTFile,"%8d",(int)cellPoints[3]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[10]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[2]);

          fprintf(FDNEUTFile,"\n");

          fprintf(FDNEUTFile,"%8d",(int)cellPoints[16]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[20]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[17]);

          fprintf(FDNEUTFile,"%8d",(int)cellPoints[23]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[26]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[21]);

          fprintf(FDNEUTFile,"%8d",(int)cellPoints[19]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[22]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[18]);

          fprintf(FDNEUTFile,"\n");

          fprintf(FDNEUTFile,"%8d",(int)cellPoints[4]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[12]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[5]);

          fprintf(FDNEUTFile,"%8d",(int)cellPoints[15]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[25]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[13]);

          fprintf(FDNEUTFile,"%8d",(int)cellPoints[7]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[14]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[6]);

          break;
        case VTK_TETRA:
          for (i=0; i<numberOfNodesInElement; i++)
            {
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[i]);
            }
          break;
        case VTK_QUADRATIC_TETRA:
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[0]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[4]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[1]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[6]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[5]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[2]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[7]);
          fprintf(FDNEUTFile,"\n");
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[8]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[9]);
          fprintf(FDNEUTFile,"%8d",(int)cellPoints[3]);
          break;
        case WEDGE:
          for (i=0; i<numberOfNodesInElement; i++)
            {
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[i]);
            }
          break;
        case VTK_QUADRATIC_WEDGE:
          if (numberOfNodesInElement==18)
            {
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[0]);
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[6]);
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[1]);

            fprintf(FDNEUTFile,"%8d",(int)cellPoints[8]);
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[7]);
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[2]);

            fprintf(FDNEUTFile,"%8d",(int)cellPoints[12]);
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[15]);
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[13]);

            fprintf(FDNEUTFile,"\n");

            fprintf(FDNEUTFile,"%8d",(int)cellPoints[17]);
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[16]);
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[14]);

            fprintf(FDNEUTFile,"%8d",(int)cellPoints[3]);
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[9]);
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[4]);

            fprintf(FDNEUTFile,"%8d",(int)cellPoints[11]);
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[10]);
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[5]);
            }
          else if (numberOfNodesInElement==15)
            {
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[0]);
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[6]);
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[1]);

            fprintf(FDNEUTFile,"%8d",(int)cellPoints[8]);
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[7]);
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[2]);

            fprintf(FDNEUTFile,"%8d",(int)cellPoints[12]);
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[13]);
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[14]);

            fprintf(FDNEUTFile,"\n");

            fprintf(FDNEUTFile,"%8d",(int)cellPoints[3]);
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[9]);
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[4]);

            fprintf(FDNEUTFile,"%8d",(int)cellPoints[11]);
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[10]);
            fprintf(FDNEUTFile,"%8d",(int)cellPoints[5]);
            }
          break;
        }
      fprintf(FDNEUTFile,"\n");
      cellCount++;
      delete[] cellPoints;
      }
    ++groupCount;
    }

  fclose(FDNEUTFile);
}

void vtkvmtkFDNEUTWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkUnstructuredGridWriter::PrintSelf(os,indent);
}
