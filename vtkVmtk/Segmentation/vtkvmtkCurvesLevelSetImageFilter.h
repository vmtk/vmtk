/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCurvesLevelSetImageFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

  Portions of this code are covered under the ITK copyright.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// .NAME vtkvmtkCurvesLevelSetImageFilter - Wrapper class around itk::CurvesLevelSetImageFilter
// .SECTION Description
// vtkvmtkCurvesLevelSetImageFilter


#ifndef __vtkvmtkCurvesLevelSetImageFilter_h
#define __vtkvmtkCurvesLevelSetImageFilter_h


#include "vtkITKImageToImageFilterFF.h"
#include "itkCurvesLevelSetImageFilter.h"
#include "vtkObjectFactory.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkCurvesLevelSetImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkvmtkCurvesLevelSetImageFilter *New();
  vtkTypeRevisionMacro(vtkvmtkCurvesLevelSetImageFilter, vtkITKImageToImageFilterFF);

  float GetIsoSurfaceValue ()
  {
    DelegateITKOutputMacro(GetIsoSurfaceValue) ;
  };

  void SetIsoSurfaceValue ( float value )
  {
     DelegateITKInputMacro ( SetIsoSurfaceValue, value );
  };
  
  void SetNumberOfIterations ( int value )
  {
    DelegateITKInputMacro ( SetNumberOfIterations, value );
  };

  void SetPropagationScaling ( float value )
  {
    DelegateITKInputMacro ( SetPropagationScaling, value );
  };

  void SetCurvatureScaling ( float value )
  {
    DelegateITKInputMacro ( SetCurvatureScaling, value );
  };

  void SetAdvectionScaling ( float value )
  {
    DelegateITKInputMacro ( SetAdvectionScaling, value );
  };

  void SetMaximumRMSError ( float value )
  {
    DelegateITKInputMacro ( SetMaximumRMSError, value );
  };

  void SetUseNegativeFeatures (int value )
  {
    DelegateITKInputMacro( SetUseNegativeFeatures, value);
  }

  void SetAutoGenerateSpeedAdvection (int value )
  {
    DelegateITKInputMacro( SetAutoGenerateSpeedAdvection, value);
  }

  void SetInterpolateSurfaceLocation (int value )
  {
    DelegateITKInputMacro( SetInterpolateSurfaceLocation, value);
  }
  
  void SetDerivativeSigma ( float value )
  {
    DelegateITKInputMacro ( SetDerivativeSigma, value );
  };

  void SetFeatureImage ( vtkImageData *value)
  {
    this->vtkFeatureExporter->SetInput(value);
  }

  void SetSpeedImage ( vtkImageData *value)
  {
    this->vtkSpeedExporter->SetInput(value);
  }

  void SetAdvectionImage ( vtkImageData *value)
  {
    this->vtkAdvectionExporter->SetInput(value);
  }

  vtkImageData *GetFeatureImage()
  {
    this->vtkFeatureImporter->Update();
    return this->vtkFeatureImporter->GetOutput();
  }

  vtkImageData *GetAdvectionImage()
  {
    this->vtkAdvectionImporter->Update();
    return this->vtkAdvectionImporter->GetOutput();
  }

  vtkImageData *GetSpeedImage()
  {
    this->vtkSpeedImporter->Update();
    return this->vtkSpeedImporter->GetOutput();
  }

  void SetFeatureScaling ( float value )
  {
    DelegateITKInputMacro ( SetFeatureScaling, value );
  };

   float GetRMSChange ()
  {
    DelegateITKOutputMacro(GetRMSChange);
  };

  int GetElapsedIterations()
  {
    DelegateITKOutputMacro(GetElapsedIterations);
  };

  float GetPropagationScaling ( )
  {
    DelegateITKOutputMacro ( GetPropagationScaling );
  };

  float GetCurvatureScaling ( )
  {
    DelegateITKOutputMacro ( GetCurvatureScaling );
  };

  float GetAdvectionScaling ( )
  {
    DelegateITKOutputMacro ( GetAdvectionScaling );
  };

  int GetAutoGenerateSpeedAdvection ( )
  {
    DelegateITKOutputMacro( GetAutoGenerateSpeedAdvection );
  }

  int GetInterpolateSurfaceLocation ( )
  {
    DelegateITKOutputMacro( GetInterpolateSurfaceLocation );
  }

  float GetDerivativeSigma ( float value )
  {
    DelegateITKOutputMacro ( GetDerivativeSigma );
  };
  
  // Description: Override vtkSource's Update so that we can access this class's GetOutput(). vtkSource's GetOutput is not virtual.
  void Update()
  {
//     if (this->vtkFeatureExporter->GetInput())
//       {
//         this->itkFeatureImporter->Update();
        
//         if (this->GetOutput(0))
//           {
//             this->GetOutput(0)->Update();
//             if ( this->GetOutput(0)->GetSource() )
//               {
//                 //          this->SetErrorCode( this->GetOutput(0)->GetSource()->GetErrorCode() );
//               }
//           }
//       }

    if (this->vtkFeatureExporter->GetInput())
      {
      this->itkFeatureImporter->Update();
      }

    if (this->vtkSpeedExporter->GetInput())
      {
      this->itkSpeedImporter->Update();
      }

    if (this->vtkAdvectionExporter->GetInput())
      {
      this->itkAdvectionImporter->Update();
      }

    if (this->GetOutput(0))
      {
      this->GetOutput(0)->Update();
      if ( this->GetOutput(0)->GetSource() )
        {
        //          this->SetErrorCode( this->GetOutput(0)->GetSource()->GetErrorCode() );
        }
      }
  }
    
protected:
  //BTX
  typedef itk::CurvesLevelSetImageFilter<Superclass::InputImageType,Superclass::OutputImageType> ImageFilterType;

  typedef ImageFilterType::VectorImageType VectorImageType;  

  typedef itk::VTKImageImport<InputImageType> FeatureImageImportType;
  typedef itk::VTKImageImport<InputImageType> SpeedImageImportType;
  typedef itk::VTKImageImport<VectorImageType> AdvectionImageImportType;

  typedef itk::VTKImageExport<InputImageType> FeatureImageExportType;
  typedef itk::VTKImageExport<InputImageType> SpeedImageExportType;
  typedef itk::VTKImageExport<VectorImageType> AdvectionImageExportType;
  
  vtkvmtkCurvesLevelSetImageFilter() : Superclass ( ImageFilterType::New() )
  {
    this->vtkFeatureExporter = vtkImageExport::New();
    this->itkFeatureImporter = FeatureImageImportType::New();
    this->vtkSpeedExporter = vtkImageExport::New();
    this->itkSpeedImporter = SpeedImageImportType::New();
    this->vtkAdvectionExporter = vtkImageExport::New();
    this->itkAdvectionImporter = AdvectionImageImportType::New();

    this->itkFeatureExporter = FeatureImageExportType::New();
    this->vtkFeatureImporter = vtkImageImport::New();
    this->itkSpeedExporter = SpeedImageExportType::New();
    this->vtkSpeedImporter = vtkImageImport::New();
    this->itkAdvectionExporter = AdvectionImageExportType::New();
    this->vtkAdvectionImporter = vtkImageImport::New();

    ConnectPipelines(this->vtkFeatureExporter, this->itkFeatureImporter);
    ConnectPipelines(this->vtkSpeedExporter, this->itkSpeedImporter);
    ConnectPipelines(this->vtkAdvectionExporter, this->itkAdvectionImporter);

    ConnectPipelines(this->itkFeatureExporter, this->vtkFeatureImporter);
    ConnectPipelines(this->itkSpeedExporter, this->vtkSpeedImporter);
    ConnectPipelines(this->itkAdvectionExporter, this->vtkAdvectionImporter);

    (dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()))->SetFeatureImage(this->itkFeatureImporter->GetOutput());
    (dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()))->SetSpeedImage(this->itkSpeedImporter->GetOutput());
    (dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()))->SetAdvectionImage(this->itkAdvectionImporter->GetOutput());
    
    this->itkFeatureExporter->SetInput((dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()))->GetFeatureImage());
    this->itkSpeedExporter->SetInput((dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()))->GetSpeedImage());
    this->itkAdvectionExporter->SetInput((dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()))->GetAdvectionImage());
  };
  ~vtkvmtkCurvesLevelSetImageFilter() 
  {
    this->vtkFeatureExporter->Delete();
    this->vtkSpeedExporter->Delete();
    this->vtkAdvectionExporter->Delete();

    this->vtkFeatureImporter->Delete();
    this->vtkSpeedImporter->Delete();
    this->vtkAdvectionImporter->Delete();
  };
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }
  
  FeatureImageImportType::Pointer itkFeatureImporter;
  SpeedImageImportType::Pointer itkSpeedImporter;
  AdvectionImageImportType::Pointer itkAdvectionImporter;

  FeatureImageExportType::Pointer itkFeatureExporter;
  SpeedImageExportType::Pointer itkSpeedExporter;
  AdvectionImageExportType::Pointer itkAdvectionExporter;

  //ETX

  vtkImageExport *vtkFeatureExporter;
  vtkImageExport *vtkSpeedExporter;
  vtkImageExport *vtkAdvectionExporter;
  
  vtkImageImport *vtkFeatureImporter;
  vtkImageImport *vtkSpeedImporter;
  vtkImageImport *vtkAdvectionImporter;
  
private:
  vtkvmtkCurvesLevelSetImageFilter(const vtkvmtkCurvesLevelSetImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkCurvesLevelSetImageFilter&);  //
                                                                          // Not implemented
  
};

vtkCxxRevisionMacro(vtkvmtkCurvesLevelSetImageFilter, "$Revision: 1.4 $");
vtkStandardNewMacro(vtkvmtkCurvesLevelSetImageFilter);

#endif




