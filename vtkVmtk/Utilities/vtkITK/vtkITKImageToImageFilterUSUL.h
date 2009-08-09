/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   vtkvmtkITK
  Module:    $HeadURL: http://www.na-mic.org/svn/Slicer3/trunk/Libs/vtkvmtkITK/vtkvmtkITKImageToImageFilterUSUL.h $
  Date:      $Date: 2006-12-21 13:21:52 +0100 (Thu, 21 Dec 2006) $
  Version:   $Revision: 1900 $

==========================================================================*/
// .NAME vtkvmtkITKImageToImageFilter - Abstract base class for connecting ITK and VTK
// .SECTION Description
// vtkvmtkITKImageToImageFilter provides a 

#ifndef __vtkvmtkITKImageToImageFilterUSUL_h
#define __vtkvmtkITKImageToImageFilterUSUL_h


#include "vtkImageToImageFilter.h"
#include "vtkvmtkITKImageToImageFilter.h"
#include "itkImageToImageFilter.h"
#include "itkVTKImageExport.h"
#include "itkVTKImageImport.h"
#include "vtkvmtkITKUtility.h"


class VTK_VMTK_ITK_EXPORT vtkvmtkITKImageToImageFilterUSUL : public vtkvmtkITKImageToImageFilter
{
public:
  vtkTypeMacro(vtkvmtkITKImageToImageFilterUSUL,vtkvmtkITKImageToImageFilter);
  static vtkvmtkITKImageToImageFilterUSUL* New() { return 0; };
  void PrintSelf(ostream& os, vtkIndent indent)
  {
    Superclass::PrintSelf ( os, indent );
    os << m_Filter;
  };

protected:
  //BTX
  
  // To/from ITK
  typedef itk::Image<unsigned short, 3> InputImageType;
  typedef itk::Image<unsigned long, 3> OutputImageType;

  typedef itk::VTKImageImport<InputImageType> ImageImportType;
  typedef itk::VTKImageExport<OutputImageType> ImageExportType;
  ImageImportType::Pointer itkImporter;
  ImageExportType::Pointer itkExporter;

  typedef itk::ImageToImageFilter<InputImageType,OutputImageType> FilterType;
  FilterType::Pointer m_Filter;

  vtkvmtkITKImageToImageFilterUSUL ( FilterType* filter )
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
    this->vtkCast->SetOutputScalarTypeToUnsignedShort();
  };

  ~vtkvmtkITKImageToImageFilterUSUL()
  {
  };
  //ETX
    
private:
  vtkvmtkITKImageToImageFilterUSUL(const vtkvmtkITKImageToImageFilterUSUL&);  // Not implemented.
  void operator=(const vtkvmtkITKImageToImageFilterUSUL&);  // Not implemented.
};

#endif




