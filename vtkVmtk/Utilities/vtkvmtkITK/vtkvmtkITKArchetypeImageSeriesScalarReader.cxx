/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   vtkvmtkITK
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Libs/vtkvmtkITK/vtkvmtkITKArchetypeImageSeriesScalarReader.cxx $
  Date:      $Date: 2007-01-19 19:21:56 +0100 (Fri, 19 Jan 2007) $
  Version:   $Revision: 2267 $

==========================================================================*/

#include "vtkvmtkITKArchetypeImageSeriesScalarReader.h"

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

vtkCxxRevisionMacro(vtkvmtkITKArchetypeImageSeriesScalarReader, "$Revision: 2267 $");
vtkStandardNewMacro(vtkvmtkITKArchetypeImageSeriesScalarReader);


//----------------------------------------------------------------------------
vtkvmtkITKArchetypeImageSeriesScalarReader::vtkvmtkITKArchetypeImageSeriesScalarReader()
{
}

//----------------------------------------------------------------------------
vtkvmtkITKArchetypeImageSeriesScalarReader::~vtkvmtkITKArchetypeImageSeriesScalarReader() 
{
}

//----------------------------------------------------------------------------
void vtkvmtkITKArchetypeImageSeriesScalarReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "vtk ITK Archetype Image Series Scalar Reader\n";
}

//----------------------------------------------------------------------------
// This function reads a data from a file.  The datas extent/axes
// are assumed to be the same as the file extent/order.
void vtkvmtkITKArchetypeImageSeriesScalarReader::ExecuteData(vtkDataObject *output)
{
  if (!this->Superclass::Archetype)
    {
      vtkErrorMacro("An Archetype must be specified.");
      return;
    }

  vtkImageData *data = vtkImageData::SafeDownCast(output);
// removed UpdateInformation: generates an error message
//   from VTK and doesn't appear to be needed...
//data->UpdateInformation();
  data->SetExtent(0,0,0,0,0,0);
  data->AllocateScalars();
  data->SetExtent(data->GetWholeExtent());

  /// SCALAR MACRO
#define vtkvmtkITKExecuteDataFromSeries(typeN, type) \
    case typeN: \
    {\
      typedef itk::Image<type,3> image##typeN;\
      typedef itk::ImageSource<image##typeN> FilterType; \
      FilterType::Pointer filter; \
      itk::ImageSeriesReader<image##typeN>::Pointer reader##typeN = \
          itk::ImageSeriesReader<image##typeN>::New(); \
          itk::CStyleCommand::Pointer pcl=itk::CStyleCommand::New(); \
          pcl->SetCallback((itk::CStyleCommand::FunctionPointer)&ReadProgressCallback); \
          pcl->SetClientData(this); \
          reader##typeN->AddObserver(itk::ProgressEvent(),pcl); \
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
      itk::ImportImageContainer<unsigned long, type>::Pointer PixelContainer##typeN;\
      PixelContainer##typeN = filter->GetOutput()->GetPixelContainer();\
      void *ptr = static_cast<void *> (PixelContainer##typeN->GetBufferPointer());\
      (dynamic_cast<vtkImageData *>( output))->GetPointData()->GetScalars()->SetVoidArray(ptr, PixelContainer##typeN->Size(), 0);\
      PixelContainer##typeN->ContainerManageMemoryOff();\
    }\
    break

#define vtkvmtkITKExecuteDataFromFile(typeN, type) \
    case typeN: \
    {\
      typedef itk::Image<type,3> image2##typeN;\
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
      itk::ImportImageContainer<unsigned long, type>::Pointer PixelContainer2##typeN;\
      PixelContainer2##typeN = filter->GetOutput()->GetPixelContainer();\
      void *ptr = static_cast<void *> (PixelContainer2##typeN->GetBufferPointer());\
      (dynamic_cast<vtkImageData *>( output))->GetPointData()->GetScalars()->SetVoidArray(ptr, PixelContainer2##typeN->Size(), 0);\
      PixelContainer2##typeN->ContainerManageMemoryOff();\
    }\
    break
  /// END SCALAR MACRO

  // If there is only one file in the series, just use an image file reader
  if (this->FileNames.size() == 1)
    {
    if (this->GetNumberOfComponents() == 1) 
      {
      switch (this->OutputScalarType)
        {
          vtkvmtkITKExecuteDataFromFile(VTK_DOUBLE, double);
          vtkvmtkITKExecuteDataFromFile(VTK_FLOAT, float);
          vtkvmtkITKExecuteDataFromFile(VTK_LONG, long);
          vtkvmtkITKExecuteDataFromFile(VTK_UNSIGNED_LONG, unsigned long);
          vtkvmtkITKExecuteDataFromFile(VTK_INT, int);
          vtkvmtkITKExecuteDataFromFile(VTK_UNSIGNED_INT, unsigned int);
          vtkvmtkITKExecuteDataFromFile(VTK_SHORT, short);
          vtkvmtkITKExecuteDataFromFile(VTK_UNSIGNED_SHORT, unsigned short);
          vtkvmtkITKExecuteDataFromFile(VTK_CHAR, char);
          vtkvmtkITKExecuteDataFromFile(VTK_UNSIGNED_CHAR, unsigned char);
        default:
          vtkErrorMacro(<< "UpdateFromFile: Unknown data type");
        }
      }
    else 
      {
        vtkErrorMacro(<< "UpdateFromFile: Unsupported number of components (only 1 allowed): " << this->GetNumberOfComponents());
      }
    }
  else
    {
    if (this->GetNumberOfComponents() == 1) 
      {
      switch (this->OutputScalarType)
        {
          vtkvmtkITKExecuteDataFromSeries(VTK_DOUBLE, double);
          vtkvmtkITKExecuteDataFromSeries(VTK_FLOAT, float);
          vtkvmtkITKExecuteDataFromSeries(VTK_LONG, long);
          vtkvmtkITKExecuteDataFromSeries(VTK_UNSIGNED_LONG, unsigned long);
          vtkvmtkITKExecuteDataFromSeries(VTK_INT, int);
          vtkvmtkITKExecuteDataFromSeries(VTK_UNSIGNED_INT, unsigned int);
          vtkvmtkITKExecuteDataFromSeries(VTK_SHORT, short);
          vtkvmtkITKExecuteDataFromSeries(VTK_UNSIGNED_SHORT, unsigned short);
          vtkvmtkITKExecuteDataFromSeries(VTK_CHAR, char);
          vtkvmtkITKExecuteDataFromSeries(VTK_UNSIGNED_CHAR, unsigned char);
        default:
          vtkErrorMacro(<< "UpdateFromFile: Unknown data type");
        }
      }
    else 
      {
        vtkErrorMacro(<<"UpdateFromSeries: Unsupported number of components: 1 != " << this->GetNumberOfComponents());
      }
    }
}


void vtkvmtkITKArchetypeImageSeriesScalarReader::ReadProgressCallback(itk::ProcessObject* obj,const itk::ProgressEvent&,void* data)
{
  vtkvmtkITKArchetypeImageSeriesScalarReader* me=reinterpret_cast<vtkvmtkITKArchetypeImageSeriesScalarReader*>(data);
  me->Progress=obj->GetProgress();
  me->InvokeEvent(vtkCommand::ProgressEvent,&me->Progress);
}
