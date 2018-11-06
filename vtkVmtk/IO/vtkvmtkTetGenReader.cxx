/*=========================================================================
                                                                                                                                    
Program:   VMTK
Module:    vtkvmtkTetGenReader.cxx
Language:  C++
Date:      Sat Feb 19 15:14:48 CET 2011
Version:   Revision: 1.0
                                                                                                                                    
  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
                                                                                                                                    
=========================================================================*/

#include "vtkvmtkTetGenReader.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUnstructuredGrid.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkvmtkConstants.h"

#include <sstream>


vtkStandardNewMacro(vtkvmtkTetGenReader);

vtkvmtkTetGenReader::vtkvmtkTetGenReader()
{
  this->BoundaryDataArrayName = NULL;
}

vtkvmtkTetGenReader::~vtkvmtkTetGenReader()
{
  if (this->BoundaryDataArrayName)
    {
    delete[] this->BoundaryDataArrayName;
    this->BoundaryDataArrayName = NULL;
    }
}

void vtkvmtkTetGenReader::Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters)
{
  if (tokens.size() > 0)
    {
    tokens.clear();
    }

  std::string::size_type lastPos = str.find_first_not_of(delimiters,0);
  std::string::size_type pos = str.find_first_of(delimiters,lastPos);

  while (std::string::npos != pos || std::string::npos != lastPos)
    {
    tokens.push_back(str.substr(lastPos,pos-lastPos));
    lastPos = str.find_first_not_of(delimiters, pos);
    pos = str.find_first_of(delimiters, lastPos);
    }
}

int vtkvmtkTetGenReader::RequestData(
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER()) > 0)
    {
    return 1;
    }

  if (!this->GetFileName())
    {
    vtkErrorMacro(<<"FileName not set.");
    return 1;
    }

  std::string nodeFileName = this->GetFileName();
  nodeFileName += ".node";

  std::string eleFileName = this->GetFileName();
  eleFileName += ".ele";

  ifstream nodeStream(nodeFileName.c_str());
  ifstream eleStream(eleFileName.c_str());

  if (!nodeStream.good())
    {
    vtkErrorMacro(<<"Could not open .node file for reading.");
    return 0;
    }

  if (!eleStream.good())
    {
    vtkErrorMacro(<<"Could not open .ele file for reading.");
    return 0;
    }

  std::string line;
  std::string delimiter(" ");
  std::vector<std::string> tokens;

  std::getline(nodeStream,line);
  this->Tokenize(line,tokens,delimiter);

  int nodeCount, dim, numberOfAttributes, boundaryMarkers;

  //TODO: check number of entries of each line.
  nodeCount = atoi(tokens[0].c_str());
  dim = atoi(tokens[1].c_str());
  numberOfAttributes = atoi(tokens[2].c_str());
  boundaryMarkers = atoi(tokens[3].c_str());

  vtkPoints* outputPoints = vtkPoints::New();
  outputPoints->SetNumberOfPoints(nodeCount);

  int i, j;

  vtkDoubleArray** attributeArrays = new vtkDoubleArray*[numberOfAttributes];
  for (j=0; j<numberOfAttributes; j++)
    {
    std::stringstream nameStream; 
    nameStream << "Attribute_" << j;
    vtkDoubleArray* attributeArray = vtkDoubleArray::New();
    attributeArray->SetName(nameStream.str().c_str());
    attributeArray->SetNumberOfComponents(1);
    attributeArray->SetNumberOfTuples(nodeCount);
    attributeArrays[j] = attributeArray;
    }

  vtkIdTypeArray* boundaryDataArray = vtkIdTypeArray::New();
  if (boundaryMarkers)
    {
    boundaryDataArray->SetName(this->BoundaryDataArrayName);
    boundaryDataArray->SetNumberOfComponents(1);
    boundaryDataArray->SetNumberOfTuples(nodeCount);
    }

  double point[3], value;
  vtkIdType boundaryId;

  int firstIndex = 0;

  int index;
  for (i=0; i<nodeCount; i++)
    {
    std::getline(nodeStream,line);
    this->Tokenize(line,tokens,delimiter);
    index = atoi(tokens[0].c_str());
    if (i==0)
      {
      // Here we make the assumption that node 0 or 1 appear in the first line
      firstIndex = index;
      }
    point[0] = atof(tokens[1].c_str());
    point[1] = atof(tokens[2].c_str());
    point[2] = atof(tokens[3].c_str());
    outputPoints->SetPoint(index-firstIndex,point);
    for (j=0; j<numberOfAttributes; j++)
      {
      value = atof(tokens[4+j].c_str());
      attributeArrays[j]->SetValue(i,value);
      }
    if (boundaryMarkers)
      {
      boundaryId = atoi(tokens[4+numberOfAttributes].c_str());
      boundaryDataArray->SetValue(i,boundaryId);
      }
    }

  output->SetPoints(outputPoints);
  outputPoints->Delete();

  for (j=0; j<numberOfAttributes; j++)
    {
    output->GetPointData()->AddArray(attributeArrays[j]);
    attributeArrays[j]->Delete();
    }
  delete[] attributeArrays;

  if (boundaryMarkers)
    {
    output->GetPointData()->AddArray(boundaryDataArray);
    }

  std::getline(eleStream,line);
  this->Tokenize(line,tokens,delimiter);

  int tetCount, nodesPerTet, numberOfCellAttributes;
  tetCount = atoi(tokens[0].c_str());
  nodesPerTet = atoi(tokens[1].c_str());
  numberOfCellAttributes = atoi(tokens[2].c_str());
 
  vtkDoubleArray** cellAttributeArrays = new vtkDoubleArray*[numberOfCellAttributes];
  for (j=0; j<numberOfCellAttributes; j++)
    {
    std::stringstream nameStream; 
    nameStream << "Attribute_" << j;
    vtkDoubleArray* attributeArray = vtkDoubleArray::New();
    attributeArray->SetName(nameStream.str().c_str());
    attributeArray->SetNumberOfComponents(1);
    attributeArray->SetNumberOfTuples(nodeCount);
    cellAttributeArrays[j] = attributeArray;
    }

  int* outputCellTypes = new int[tetCount];
  vtkCellArray* outputCellArray = vtkCellArray::New();

  int outputCellType = VTK_TETRA;
  if (nodesPerTet == 10)
    {
    outputCellType = VTK_QUADRATIC_TETRA;
    }

  vtkIdTypeArray* cellBoundaryDataArray = vtkIdTypeArray::New();
  if (boundaryMarkers)
    {
    cellBoundaryDataArray->SetName(this->BoundaryDataArrayName);
    cellBoundaryDataArray->SetNumberOfComponents(1);
    cellBoundaryDataArray->SetNumberOfTuples(tetCount);
    }

  vtkIdType pointId, maxBoundaryId;
  for (i=0; i<tetCount; i++)
    {
    std::getline(eleStream,line);
    this->Tokenize(line,tokens,delimiter);
    index = atoi(tokens[0].c_str());
    outputCellArray->InsertNextCell(nodesPerTet);
    maxBoundaryId = 0;
    for (j=0; j<nodesPerTet; j++)
      {
      pointId = atoi(tokens[j+1].c_str());
      outputCellArray->InsertCellPoint(pointId-firstIndex);
      if (boundaryMarkers)
        {
        boundaryId = boundaryDataArray->GetValue(pointId-firstIndex);
        if (j>0 && boundaryId > maxBoundaryId)
          {
          maxBoundaryId = boundaryId;
          }
        }
      }
    if (boundaryMarkers)
      {
      cellBoundaryDataArray->SetValue(i,maxBoundaryId);
      }
    outputCellTypes[i] = outputCellType;
    for (j=0; j<numberOfCellAttributes; j++)
      {
      value = atof(tokens[1+nodesPerTet].c_str());
      cellAttributeArrays[j]->SetValue(i,value);
      }
    }

  for (j=0; j<numberOfCellAttributes; j++)
    {
    output->GetCellData()->AddArray(cellAttributeArrays[j]);
    cellAttributeArrays[j]->Delete();
    }
  delete[] cellAttributeArrays;

  output->SetCells(outputCellTypes,outputCellArray);

  if (boundaryMarkers)
    {
    output->GetCellData()->AddArray(cellBoundaryDataArray);
    }
  cellBoundaryDataArray->Delete();

  boundaryDataArray->Delete();

  outputCellArray->Delete();
  delete[] outputCellTypes;

  return 1;
}

void vtkvmtkTetGenReader::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}
