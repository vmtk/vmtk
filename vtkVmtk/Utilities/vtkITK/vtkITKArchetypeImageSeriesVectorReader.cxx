/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   vtkvmtkITK
  Module:    $HeadURL: http://www.na-mic.org/svn/Slicer3/trunk/Libs/vtkvmtkITK/vtkvmtkITKArchetypeImageSeriesVectorReader.cxx $
  Date:      $Date: 2007-05-22 11:33:40 -0400 (Tue, 22 May 2007) $
  Version:   $Revision: 3420 $

==========================================================================*/

#include "vtkvmtkITKArchetypeImageSeriesVectorReader.h"

#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include <vtkCommand.h>

#include "itkArchetypeSeriesFileNames.h"
#include "itkImage.h"
#include "itkVector.h"
#include "itkOrientImageFilter.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileReader.h"
#include "itkImportImageContainer.h"
#include "itkImageRegion.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkGDCMImageIO.h"
#include <itksys/SystemTools.hxx>

vtkStandardNewMacro(vtkvmtkITKArchetypeImageSeriesVectorReader);
vtkCxxRevisionMacro(vtkvmtkITKArchetypeImageSeriesVectorReader, "$Revision: 3420 $");

//----------------------------------------------------------------------------
vtkvmtkITKArchetypeImageSeriesVectorReader::vtkvmtkITKArchetypeImageSeriesVectorReader()
{
}

//----------------------------------------------------------------------------
vtkvmtkITKArchetypeImageSeriesVectorReader::~vtkvmtkITKArchetypeImageSeriesVectorReader() 
{
}

//----------------------------------------------------------------------------
void vtkvmtkITKArchetypeImageSeriesVectorReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "vtk ITK Archetype Image Series Vector Reader\n";
}

//----------------------------------------------------------------------------
// This function reads a data from a file.  The datas extent/axes
// are assumed to be the same as the file extent/order.
void vtkvmtkITKArchetypeImageSeriesVectorReader::ExecuteData(vtkDataObject *output)
{
  if (!this->Superclass::Archetype)
    {
      vtkErrorMacro("An Archetype must be specified.");
      return;
    }

  vtkImageData *data = vtkImageData::SafeDownCast(output);
  //data->UpdateInformation();
  data->SetExtent(0,0,0,0,0,0);
  data->AllocateScalars();
  data->SetExtent(data->GetWholeExtent());

  /// VECTOR MACRO
#define vtkvmtkITKExecuteDataFromSeriesVector(typeN, type) \
    case typeN: \
    {\
      typedef itk::Vector<type, 3>    VectorPixelType;\
      typedef itk::Image<VectorPixelType,3> image##typeN;\
      typedef itk::ImageSource<image##typeN> FilterType; \
      FilterType::Pointer filter; \
      itk::ImageSeriesReader<image##typeN>::Pointer reader##typeN = \
        itk::ImageSeriesReader<image##typeN>::New(); \
        itk::CStyleCommand::Pointer pcl=itk::CStyleCommand::New(); \
        pcl->SetCallback((itk::CStyleCommand::FunctionPointer)&ReadProgressCallback); \
      pcl->SetClientData(this); \
      reader##typeN->SetFileNames(this->FileNames); \
      reader##typeN->ReleaseDataFlagOn(); \
      if (this->UseNativeCoordinateOrientation) \
        { \
        filter = reader##typeN; \
        } \
      else \
        { \
        itk::OrientImageFilter<image##typeN,image##typeN>::Pointer orient##typeN = \
            itk::OrientImageFilter<image##typeN,image##typeN>::New(); \
        if (this->Debug) {orient##typeN->DebugOn();} \
        orient##typeN->SetInput(reader##typeN->GetOutput()); \
        orient##typeN->UseImageDirectionOn(); \
        orient##typeN->SetDesiredCoordinateOrientation(this->DesiredCoordinateOrientation); \
        filter = orient##typeN; \
        }\
      filter->UpdateLargestPossibleRegion(); \
      itk::ImportImageContainer<unsigned long, VectorPixelType>::Pointer PixelContainer##typeN;\
      PixelContainer##typeN = filter->GetOutput()->GetPixelContainer();\
      void *ptr = static_cast<void *> (PixelContainer##typeN->GetBufferPointer());\
      (dynamic_cast<vtkImageData *>( output))->GetPointData()->GetScalars()->SetVoidArray(ptr, PixelContainer##typeN->Size(), 0);\
      PixelContainer##typeN->ContainerManageMemoryOff();\
    }\
    break

#define vtkvmtkITKExecuteDataFromFileVector(typeN, type) \
    case typeN: \
    {\
      typedef itk::Vector<type, 3>    VectorPixelType;\
      typedef itk::Image<VectorPixelType,3> image2##typeN;\
      typedef itk::ImageSource<image2##typeN> FilterType; \
      FilterType::Pointer filter; \
      itk::ImageFileReader<image2##typeN>::Pointer reader2##typeN = \
            itk::ImageFileReader<image2##typeN>::New(); \
      reader2##typeN->SetFileName(this->FileNames[0].c_str()); \
      if (this->UseNativeCoordinateOrientation) \
        { \
        filter = reader2##typeN; \
        } \
      else \
        { \
        itk::OrientImageFilter<image2##typeN,image2##typeN>::Pointer orient2##typeN = \
              itk::OrientImageFilter<image2##typeN,image2##typeN>::New(); \
        if (this->Debug) {orient2##typeN->DebugOn();} \
        orient2##typeN->SetInput(reader2##typeN->GetOutput()); \
        orient2##typeN->UseImageDirectionOn(); \
        orient2##typeN->SetDesiredCoordinateOrientation(this->DesiredCoordinateOrientation); \
        filter = orient2##typeN; \
        } \
       filter->UpdateLargestPossibleRegion();\
      itk::ImportImageContainer<unsigned long, VectorPixelType>::Pointer PixelContainer2##typeN;\
      PixelContainer2##typeN = filter->GetOutput()->GetPixelContainer();\
      void *ptr = static_cast<void *> (PixelContainer2##typeN->GetBufferPointer());\
      (dynamic_cast<vtkImageData *>( output))->GetPointData()->GetScalars()->SetVoidArray(ptr, PixelContainer2##typeN->Size(), 0);\
      PixelContainer2##typeN->ContainerManageMemoryOff();\
    }\
    break
  // END VECTOR MACRO

    // If there is only one file in the series, just use an image file reader
  if (this->FileNames.size() == 1)
    {
      if (this->GetNumberOfComponents() == 3) 
      {
       switch (this->OutputScalarType)
        {
          vtkvmtkITKExecuteDataFromFileVector(VTK_DOUBLE, double);
          vtkvmtkITKExecuteDataFromFileVector(VTK_FLOAT, float);
          vtkvmtkITKExecuteDataFromFileVector(VTK_LONG, long);
          vtkvmtkITKExecuteDataFromFileVector(VTK_UNSIGNED_LONG, unsigned long);
          vtkvmtkITKExecuteDataFromFileVector(VTK_INT, int);
          vtkvmtkITKExecuteDataFromFileVector(VTK_UNSIGNED_INT, unsigned int);
          vtkvmtkITKExecuteDataFromFileVector(VTK_SHORT, short);
          vtkvmtkITKExecuteDataFromFileVector(VTK_UNSIGNED_SHORT, unsigned short);
          vtkvmtkITKExecuteDataFromFileVector(VTK_CHAR, char);
          vtkvmtkITKExecuteDataFromFileVector(VTK_UNSIGNED_CHAR, unsigned char);
        default:
          vtkErrorMacro(<< "UpdateFromFile: Unknown data type");
        }
    }
      else 
    {
      vtkDebugMacro(<< "UpdateFromFile: Unsupported Number Of Components: 3 != " << this->GetNumberOfComponents());
    }
    }
  else
    {
      if (this->GetNumberOfComponents() == 3)
      {
    switch (this->OutputScalarType)
      {
        vtkvmtkITKExecuteDataFromSeriesVector(VTK_DOUBLE, double);
        vtkvmtkITKExecuteDataFromSeriesVector(VTK_FLOAT, float);
        vtkvmtkITKExecuteDataFromSeriesVector(VTK_LONG, long);
        vtkvmtkITKExecuteDataFromSeriesVector(VTK_UNSIGNED_LONG, unsigned long);
        vtkvmtkITKExecuteDataFromSeriesVector(VTK_INT, int);
        vtkvmtkITKExecuteDataFromSeriesVector(VTK_UNSIGNED_INT, unsigned int);
        vtkvmtkITKExecuteDataFromSeriesVector(VTK_SHORT, short);
        vtkvmtkITKExecuteDataFromSeriesVector(VTK_UNSIGNED_SHORT, unsigned short);
        vtkvmtkITKExecuteDataFromSeriesVector(VTK_CHAR, char);
        vtkvmtkITKExecuteDataFromSeriesVector(VTK_UNSIGNED_CHAR, unsigned char);
      default:
        vtkErrorMacro(<< "UpdateFromFile: Unknown data type");
      }
      }
      else 
    {
          vtkDebugMacro(<< "UpdateFromFile: Unsupported Number Of Components: 3 != " << this->GetNumberOfComponents());
    }
    }
}


void vtkvmtkITKArchetypeImageSeriesVectorReader::ReadProgressCallback(itk::ProcessObject* obj,const itk::ProgressEvent&,void* data)
{
  vtkvmtkITKArchetypeImageSeriesVectorReader* me=reinterpret_cast<vtkvmtkITKArchetypeImageSeriesVectorReader*>(data);
  me->Progress=obj->GetProgress();
  me->InvokeEvent(vtkCommand::ProgressEvent,&me->Progress);
}
