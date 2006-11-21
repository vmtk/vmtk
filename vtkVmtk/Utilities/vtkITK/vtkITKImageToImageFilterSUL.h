/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for
details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkITKImageToImageFilter - Abstract base class for connecting ITK and VTK
// .SECTION Description
// vtkITKImageToImageFilter provides a 

#ifndef __vtkITKImageToImageFilterSUL_h
#define __vtkITKImageToImageFilterSUL_h


#include "vtkImageToImageFilter.h"
#include "vtkITKImageToImageFilter.h"
#include "itkImageToImageFilter.h"
#include "itkVTKImageExport.h"
#include "itkVTKImageImport.h"
#include "vtkITKUtility.h"


class VTK_EXPORT vtkITKImageToImageFilterSUL : public vtkITKImageToImageFilter
{
public:
  vtkTypeMacro(vtkITKImageToImageFilterSUL,vtkITKImageToImageFilter);
  static vtkITKImageToImageFilterSUL* New() { return 0; };
  void PrintSelf(ostream& os, vtkIndent indent)
  {
    Superclass::PrintSelf ( os, indent );
    os << m_Filter;
  };
  virtual void SetReleaseDataFlag(int f)
    {
      Superclass::SetReleaseDataFlag(f);
      m_Filter->SetReleaseDataFlag(f);
    }

protected:
  //BTX
  
  // To/from ITK
  typedef itk::Image<short, 3> InputImageType;
  typedef itk::Image<unsigned long, 3> OutputImageType;

  typedef itk::VTKImageImport<InputImageType> ImageImportType;
  typedef itk::VTKImageExport<OutputImageType> ImageExportType;
  ImageImportType::Pointer itkImporter;
  ImageExportType::Pointer itkExporter;

  typedef itk::ImageToImageFilter<InputImageType,OutputImageType> FilterType;
  FilterType::Pointer m_Filter;

  vtkITKImageToImageFilterSUL ( FilterType* filter )
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
    this->vtkCast->SetOutputScalarTypeToShort();
  };

  ~vtkITKImageToImageFilterSUL()
  {
  };
  //ETX
    
private:
  vtkITKImageToImageFilterSUL(const vtkITKImageToImageFilterSUL&);  // Not implemented.
  void operator=(const vtkITKImageToImageFilterSUL&);  // Not implemented.
};

#endif




