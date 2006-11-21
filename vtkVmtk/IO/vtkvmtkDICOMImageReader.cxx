/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkDICOMImageReader.cxx,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:47:46 $
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

#include "vtkvmtkDICOMImageReader.h"
#include "vtkImageData.h"
#include "vtkImageFlip.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkObjectFactory.h"

#include <vtkstd/vector>
#include <vtkstd/string>

#include "DICOMParser/DICOMAppHelper.h"
#include "DICOMParser/DICOMParser.h"
#include "DICOMAppHelper.h"
#include "DICOMParser.h"


vtkCxxRevisionMacro(vtkvmtkDICOMImageReader,"$Revision: 1.8 $");
vtkStandardNewMacro(vtkvmtkDICOMImageReader);
#if 0
class DICOMAppHelperImplementation
{
public:
  // map from series UID to vector of files in the series 
  dicom_stl::map<dicom_stl::string, dicom_stl::vector<dicom_stl::string>, ltstdstr> SeriesUIDMap;

  // map from filename to intraseries sortable tags
  dicom_stl::map<dicom_stl::string, DICOMOrderingElements, ltstdstr> SliceOrderingMap;

  typedef dicom_stl::map<dicom_stl::pair<doublebyte, doublebyte>, DICOMTagInfo> TagMapType;
  TagMapType TagMap;

}; 

void vtkvmtkDICOMAppHelper::ImageOrientationPatientCallback(DICOMParser *parser,
                                                           doublebyte group,
                                                           doublebyte element,
                                                           DICOMParser::VRTypes type,
                                                           unsigned char* val,
                                                           quadbyte len)
{
//  DICOMAppHelper::ImageOrientationPatientCallback(parser,group,element,type,val,len);
  // Look for the current file in the map of slice ordering data
  dicom_stl::map<dicom_stl::string, DICOMOrderingElements, ltstdstr>::iterator it;
  it = this->Implementation->SliceOrderingMap.find(parser->GetFileName());
  if (it == Implementation->SliceOrderingMap.end())
    {
    // file not found, create a new entry
    DICOMOrderingElements ord;
    if (val)
      {
      sscanf( (char*)(val), "%f\\%f\\%f\\%f\\%f\\%f",
              &ord.ImageOrientationPatient[0],
              &ord.ImageOrientationPatient[1],
              &ord.ImageOrientationPatient[2],
              &ord.ImageOrientationPatient[3],
              &ord.ImageOrientationPatient[4],
              &ord.ImageOrientationPatient[5] );
      }
    else
      {
      // no orientation defined, default to an standard axial orientation
      ord.ImageOrientationPatient[0] = 1.0;
      ord.ImageOrientationPatient[1] = 0.0;
      ord.ImageOrientationPatient[2] = 0.0;
      ord.ImageOrientationPatient[3] = 0.0;
      ord.ImageOrientationPatient[4] = 1.0;
      ord.ImageOrientationPatient[5] = 0.0;

      }

    // insert into the map
    this->Implementation->SliceOrderingMap.insert(dicom_stl::pair<const dicom_stl::string, DICOMOrderingElements>(parser->GetFileName(), ord));

    // cache the value
    memcpy( this->ImageOrientationPatient, ord.ImageOrientationPatient, 6*sizeof(float) );

    }
  else
    {
    // file found, add new values
    if (val)
      {
      sscanf( (char*)(val), "%f\\%f\\%f\\%f\\%f\\%f",
              &(*it).second.ImageOrientationPatient[0],
              &(*it).second.ImageOrientationPatient[1],
              &(*it).second.ImageOrientationPatient[2],
              &(*it).second.ImageOrientationPatient[3],
              &(*it).second.ImageOrientationPatient[4],
              &(*it).second.ImageOrientationPatient[5] );
      }
    else
      {
      // no orientation defined, default to an standard axial orientation
      (*it).second.ImageOrientationPatient[0] = 1.0;
      (*it).second.ImageOrientationPatient[1] = 0.0;
      (*it).second.ImageOrientationPatient[2] = 0.0;
      (*it).second.ImageOrientationPatient[3] = 0.0;
      (*it).second.ImageOrientationPatient[4] = 1.0;
      (*it).second.ImageOrientationPatient[5] = 0.0;
      }

    // cache the value
    memcpy( this->ImageOrientationPatient, (*it).second.ImageOrientationPatient, 6*sizeof(float) );
    }
}
#endif

vtkvmtkDICOMImageReader::vtkvmtkDICOMImageReader()
{
#if 0
  if (this->Parser)
    {
    delete this->Parser;
    }

  this->Parser = new vtkvmtkDICOMParser();

  if (this->AppHelper)
    {
    delete this->AppHelper;
    }

  this->AppHelper = new vtkvmtkDICOMAppHelper();
#endif
  this->AutoOrientImage = 1;

  this->OrientationStringX = NULL;
  this->OrientationStringY = NULL;
  this->OrientationStringZ = NULL;
}

vtkvmtkDICOMImageReader::~vtkvmtkDICOMImageReader()
{
  if (this->OrientationStringX)
    {
    delete[] this->OrientationStringX;
    }

  if (this->OrientationStringY)
    {
    delete[] this->OrientationStringY;
    }

  if (this->OrientationStringZ)
    {
    delete[] this->OrientationStringZ;
    }
}

void vtkvmtkDICOMImageReader::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkDICOMImageReader::PrintSelf(os,indent);

}

void vtkvmtkDICOMImageReader::ExecuteInformation()
{
  Superclass::ExecuteInformation();
  this->ComputeOutputVoxelSpacing();
}
#if 0        
int vtkvmtkDICOMImageReader::RequestInformation (
  vtkInformation       * vtkNotUsed( request ),
  vtkInformationVector** vtkNotUsed( inputVector ),
  vtkInformationVector * outputVector)
{
  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  this->ComputeOutputVoxelSpacing();
 
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),this->DataExtent, 6);
  outInfo->Set(vtkDataObject::SPACING(), this->DataSpacing, 3);
  outInfo->Set(vtkDataObject::ORIGIN(), this->DataOrigin, 3);

  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, this->DataScalarType,this->NumberOfScalarComponents);
  return 1;
}
#endif
void vtkvmtkDICOMImageReader::ComputeOutputVoxelSpacing()
{
  double voxelSpacing[3] = {0.0, 0.0, 0.0};

  vtkstd::vector<vtkstd::string>* dicomFileNames = (vtkstd::vector<vtkstd::string>*)this->DICOMFileNames;

  if (this->FileName)
    {
    vtkDebugMacro( << "Single file : " << this->FileName);

    this->Parser->ClearAllDICOMTagCallbacks();
    this->Parser->OpenFile(this->FileName);

    this->AppHelper->Clear();
    this->AppHelper->RegisterCallbacks(this->Parser);
                                                                                                                  
    this->Parser->ReadHeader();

    float* pixelSpacing;
    pixelSpacing = this->AppHelper->GetPixelSpacing();

    voxelSpacing[0] = pixelSpacing[0];
    voxelSpacing[1] = pixelSpacing[1];
    voxelSpacing[2] = 0.0;
    }
  else if (dicomFileNames->size() > 0)
    {
    vtkDebugMacro( << "Multiple files (" << static_cast<int>(dicomFileNames->size()) << ")");
    this->Parser->ClearAllDICOMTagCallbacks();

    this->AppHelper->Clear();
    this->AppHelper->RegisterCallbacks(this->Parser);
   
    vtkstd::vector<vtkstd::string>::iterator fiter;
                                                                                                                  
    double imagePositions[3][3];
    int count;

    for (fiter = dicomFileNames->begin(), count=0;
         fiter != dicomFileNames->end(), count<3;
         fiter++, count++)
      {
      vtkDebugMacro( << "File : " << (*fiter).c_str());
      this->Parser->OpenFile((char*)(*fiter).c_str());
      this->Parser->ReadHeader();

      float *pixelSpacing;
      pixelSpacing = this->AppHelper->GetPixelSpacing();

      voxelSpacing[0] = pixelSpacing[0];
      voxelSpacing[1] = pixelSpacing[1];
      voxelSpacing[2] = 0.0;

      float *imagePositionPatient;
      imagePositionPatient = this->AppHelper->GetImagePositionPatient();
      imagePositions[count][0]  = imagePositionPatient[0];
      imagePositions[count][1]  = imagePositionPatient[1];
      imagePositions[count][2]  = imagePositionPatient[2];

      }

    double voxelThickness = 0.0;
    int i;
    for (i=0; i<count-1; i++)
      {
      voxelThickness += sqrt( (imagePositions[i+1][0] - imagePositions[i][0]) * (imagePositions[i+1][0] - imagePositions[i][0]) + (imagePositions[i+1][1] - imagePositions[i][1]) * (imagePositions[i+1][1] - imagePositions[i][1]) + (imagePositions[i+1][2] - imagePositions[i][2]) * (imagePositions[i+1][2] - imagePositions[i][2]) );
      }
    voxelThickness /= (double)(count-1);

    voxelSpacing[2] = voxelThickness;
    }
  else
    {
    vtkDebugMacro( << "No files!");
    }

  this->DataSpacing[0] = voxelSpacing[0];
  this->DataSpacing[1] = voxelSpacing[1];
  this->DataSpacing[2] = voxelSpacing[2];
}

void vtkvmtkDICOMImageReader::ExecuteData(vtkDataObject *output)
{
  Superclass::ExecuteData(output);
  if (this->AutoOrientImage)
    {
    this->OrientImageData();
    }
}

void vtkvmtkDICOMImageReader::GenerateOrientationString(float direction[3], char* orientationString)
{
  char RL = direction[0]<0 ? 'R' : 'L';
  char AP = direction[1]<0 ? 'A' : 'P';
  char IS = direction[2]<0 ? 'I' : 'S';

  float absDirection[3];
  absDirection[0] = fabs(direction[0]);
  absDirection[1] = fabs(direction[1]);
  absDirection[2] = fabs(direction[2]);

  char* orientationStringPointer = orientationString;
  *orientationStringPointer = '\0';

  for (int i=0; i<3; i++)
    {
    if ( absDirection[0] > 1E-4 && 
         absDirection[0] > absDirection[1] && 
         absDirection[0] > absDirection[2] )
      {
      absDirection[0] = 0.0;
      *orientationStringPointer++=RL;
      }
    else if ( absDirection[1] > 1E-4 && 
              absDirection[1] > absDirection[0] && 
              absDirection[1] > absDirection[2] )
      {
      absDirection[1] = 0.0;
      *orientationStringPointer++=AP;
      }
    else if ( absDirection[2] > 1E-4 && 
              absDirection[2] > absDirection[0] && 
              absDirection[2] > absDirection[1] )
      {
      absDirection[2] = 0.0;
      *orientationStringPointer++=IS;
      }
    else
      {
      break;
      }
    *orientationStringPointer='\0';
    }
}

void vtkvmtkDICOMImageReader::OrientImageData()
{
#if 0
  vtkvmtkDICOMAppHelper* appHelper = (vtkvmtkDICOMAppHelper*)this->AppHelper;
  float* imageOrientation = appHelper->GetImageOrientationPatient();
#endif
  float* imageOrientation = this->AppHelper->GetImageOrientationPatient();
  
  float directionX[3],directionY[3], directionZ[3];
  directionX[0] = imageOrientation[0];
  directionX[1] = imageOrientation[1];
  directionX[2] = imageOrientation[2];

  // negative because the reader already flips Y
  directionY[0] = -imageOrientation[3];
  directionY[1] = -imageOrientation[4];
  directionY[2] = -imageOrientation[5];

  vtkMath::Cross(directionX,directionY,directionZ);
  vtkMath::Normalize(directionZ);

  this->OrientationStringX = new char[4];
  this->OrientationStringY = new char[4];
  this->OrientationStringZ = new char[4];
  
  this->GenerateOrientationString(directionX,this->OrientationStringX);
  this->GenerateOrientationString(directionY,this->OrientationStringY);
  this->GenerateOrientationString(directionZ,this->OrientationStringZ);

  bool flipX = false;
  if (this->OrientationStringX[0] == 'L' || this->OrientationStringX[0] == 'P' || this->OrientationStringX[0] == 'I')
    {
    flipX = true;
    }

  bool flipY = false;
  if (this->OrientationStringY[0] == 'L' || this->OrientationStringY[0] == 'P' || this->OrientationStringY[0] == 'I')
    {
    flipY = true;
    }

  bool flipZ = false;
  if (this->OrientationStringZ[0] == 'L' || this->OrientationStringZ[0] == 'P' || this->OrientationStringZ[0] == 'I')
    {
    flipZ = true;
    }

  cout << "VTK Orientation: " << this->OrientationStringX << " " << this->OrientationStringY << " " << this->OrientationStringZ << " " << endl;
  cout << "Flipping: " << flipX << " " << flipY << " " << flipZ << endl;

  vtkImageData* output = this->GetOutput();

  vtkImageData* temp = vtkImageData::New();
  temp->DeepCopy(output);

  vtkImageData* temp0;
  vtkImageFlip* flipFilterX = NULL;
  if (flipX)
    {
    flipFilterX = vtkImageFlip::New();
    flipFilterX->SetInput(temp);
    flipFilterX->SetFilteredAxis(0);
    flipFilterX->Update();
    temp0 = flipFilterX->GetOutput();
    }
  else
    {
    temp0 = temp;
    }

  vtkImageData* temp1;
  vtkImageFlip* flipFilterY = NULL;
  if (flipY)
    {
    flipFilterY = vtkImageFlip::New();
    flipFilterY->SetInput(temp0);
    flipFilterY->SetFilteredAxis(1);
    flipFilterY->Update();
    temp1 = flipFilterY->GetOutput();
    }
  else
    {
    temp1 = temp0;
    }

  vtkImageData* temp2;
  vtkImageFlip* flipFilterZ = NULL;
  if (flipZ)
    {
    flipFilterZ = vtkImageFlip::New();
    flipFilterZ->SetInput(temp1);
    flipFilterZ->SetFilteredAxis(2);
    flipFilterZ->Update();
    temp2 = flipFilterZ->GetOutput();
    }
  else
    {
    temp2 = temp1;
    }
  
  output->DeepCopy(temp2);

  if (flipX)
    {
    flipFilterX->Delete();
    }

  if (flipY)
    {
    flipFilterY->Delete();
    }

  if (flipZ)
    {
    flipFilterZ->Delete();
    }

  temp->Delete();
}
