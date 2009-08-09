/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   vtkvmtkITK
  Module:    $HeadURL: http://www.na-mic.org/svn/Slicer3/trunk/Libs/vtkvmtkITK/vtkvmtkITKImageToImageFilterF2F.h $
  Date:      $Date: 2006-12-21 13:21:52 +0100 (Thu, 21 Dec 2006) $
  Version:   $Revision: 1900 $

==========================================================================*/

// .NAME vtkvmtkITKImageToImageFilter - Abstract base class for connecting ITK and VTK
// .SECTION Description
// vtkvmtkITKImageToImageFilter provides a 

#ifndef __vtkvmtkITKImageToImageFilterF2F_h
#define __vtkvmtkITKImageToImageFilterF2F_h


#include "vtkvmtkITKImageToImageFilter.h"
#include "vtkImageToImageFilter.h"
#include "itkImageToImageFilter.h"
#include "itkVTKImageExport.h"
#include "itkVTKImageImport.h"
#include "vtkvmtkITKUtility.h"


class VTK_VMTK_ITK_EXPORT vtkvmtkITKImageToImageFilterF2F : public vtkvmtkITKImageToImageFilter
{
public:
  vtkTypeMacro(vtkvmtkITKImageToImageFilterF2F,vtkvmtkITKImageToImageFilter);
  static vtkvmtkITKImageToImageFilterF2F* New() { return 0; };
  void PrintSelf(ostream& os, vtkIndent indent)
  {
    Superclass::PrintSelf ( os, indent );
    os << m_Filter;
  };

protected:
  //BTX
  
  // To/from ITK
  typedef itk::Vector<float,2> InputImagePixelType;
  typedef float OutputImagePixelType;
  typedef itk::Image<InputImagePixelType, 3> InputImageType;
  typedef itk::Image<OutputImagePixelType, 3> OutputImageType;

  typedef itk::VTKImageImport<InputImageType> ImageImportType;
  typedef itk::VTKImageExport<OutputImageType> ImageExportType;
  ImageImportType::Pointer itkImporter;
  ImageExportType::Pointer itkExporter;

  typedef itk::ImageToImageFilter<InputImageType,OutputImageType> GenericFilterType;
  GenericFilterType::Pointer m_Filter;

  vtkvmtkITKImageToImageFilterF2F ( GenericFilterType* filter )
  {
    // Need an import, export, and a ITK pipeline
    m_Filter = filter;
    this->itkImporter = ImageImportType::New();
    this->itkExporter = ImageExportType::New();
    ConnectPipelines(this->vtkExporter, this->itkImporter);
    ConnectPipelines(this->itkExporter, this->vtkImporter);
    this->LinkITKProgressToVTKProgress ( m_Filter );
    
    // Set up the filter pipeline
    m_Filter->SetInput ( this->itkImporter->GetOutput() );
    this->itkExporter->SetInput ( m_Filter->GetOutput() );
    this->vtkCast->SetOutputScalarTypeToFloat();
  };

  ~vtkvmtkITKImageToImageFilterF2F()
  {
  };
  //ETX
  
private:
  vtkvmtkITKImageToImageFilterF2F(const vtkvmtkITKImageToImageFilterF2F&);  // Not implemented.
  void operator=(const vtkvmtkITKImageToImageFilterF2F&);  // Not implemented.
};

#endif




