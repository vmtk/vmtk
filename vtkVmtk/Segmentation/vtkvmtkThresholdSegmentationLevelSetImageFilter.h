/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkThresholdSegmentationLevelSetImageFilter.h,v $
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

// .NAME vtkvmtkThresholdSegmentationLevelSetImageFilter - Wrapper class around itk::ThresholdSegmentationLevelSetImageFilter
// .SECTION Description
// vtkvmtkThresholdSegmentationLevelSetImageFilter


#ifndef __vtkvmtkThresholdSegmentationLevelSetImageFilter_h
#define __vtkvmtkThresholdSegmentationLevelSetImageFilter_h


#include "vtkITKImageToImageFilterFF.h"
#include "itkThresholdSegmentationLevelSetImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkThresholdSegmentationLevelSetImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkvmtkThresholdSegmentationLevelSetImageFilter *New();
  vtkTypeRevisionMacro(vtkvmtkThresholdSegmentationLevelSetImageFilter, vtkITKImageToImageFilterFF);

  void SetUpperThreshold ( float value )
  {
    DelegateITKInputMacro ( SetUpperThreshold, value );
  };
  
  float GetUpperThreshold ()
  {
    DelegateITKOutputMacro ( GetUpperThreshold );
  };

  void SetLowerThreshold ( float value )
  {
    DelegateITKInputMacro ( SetLowerThreshold, value );
  };

  float GetLowerThreshold ()
  {
    DelegateITKOutputMacro ( GetLowerThreshold );
  };

  void SetEdgeWeight ( float value )
  {
    DelegateITKInputMacro ( SetEdgeWeight, value );
  };

  float GetEdgeWeight ()
  {
    DelegateITKOutputMacro ( GetEdgeWeight );
  };

  void SetSmoothingIterations ( int value )
  {
    DelegateITKInputMacro ( SetSmoothingIterations, value );
  };

  int GetSmoothingIterations ()
  {
    DelegateITKOutputMacro ( GetSmoothingIterations );
  };

  void SetSmoothingTimeStep ( float value )
  {
    DelegateITKInputMacro ( SetSmoothingTimeStep, value );
  };

  float GetSmoothingTimeStep ()
  {
    DelegateITKOutputMacro ( GetSmoothingTimeStep );
  };

  void SetSmoothingConductance ( float value )
  {
    DelegateITKInputMacro ( SetSmoothingConductance, value );
  };

  float GetSmoothingConductance ()
  {
    DelegateITKOutputMacro ( GetSmoothingConductance );
  };

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

  void SetUseImageSpacing (int value )
  {
    DelegateITKInputMacro( SetUseImageSpacing, value);
  }

  void SetAutoGenerateSpeedAdvection (int value )
  {
    DelegateITKInputMacro( SetAutoGenerateSpeedAdvection, value);
  }

  void SetInterpolateSurfaceLocation (int value )
  {
    DelegateITKInputMacro( SetInterpolateSurfaceLocation, value);
  }
  
  void SetFeatureImage ( vtkImageData *value)
  {
    this->vtkFeatureExporter->SetInput(value);
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

  // Description: Override vtkSource's Update so that we can access this class's GetOutput(). vtkSource's GetOutput is not virtual.
  void Update()
  {
    if (this->vtkFeatureExporter->GetInput())
      {
        this->itkFeatureImporter->Update();
        
        if (this->GetOutput(0))
          {
            this->GetOutput(0)->Update();
            if ( this->GetOutput(0)->GetSource() )
              {
                //          this->SetErrorCode( this->GetOutput(0)->GetSource()->GetErrorCode() );
              }
          }
      }
  }

protected:
  //BTX
  typedef itk::ThresholdSegmentationLevelSetImageFilter<Superclass::InputImageType,Superclass::OutputImageType> ImageFilterType;
  typedef itk::VTKImageImport<InputImageType> FeatureImageImportType;
  typedef itk::VTKImageExport<InputImageType> SpeedImageExportType;
  
  vtkvmtkThresholdSegmentationLevelSetImageFilter() : Superclass ( ImageFilterType::New() )
  {
    this->vtkFeatureExporter = vtkImageExport::New();
    this->itkFeatureImporter = FeatureImageImportType::New();
    this->itkSpeedExporter = SpeedImageExportType::New();
    this->vtkSpeedImporter = vtkImageImport::New();
    ConnectPipelines(this->itkSpeedExporter, this->vtkSpeedImporter);
    ConnectPipelines(this->vtkFeatureExporter, this->itkFeatureImporter);
    (dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()))->SetFeatureImage(this->itkFeatureImporter->GetOutput());
    this->itkSpeedExporter->SetInput((dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()))->GetSpeedImage());
  };
  ~vtkvmtkThresholdSegmentationLevelSetImageFilter() 
  {
    this->vtkSpeedImporter->Delete();
    this->vtkFeatureExporter->Delete();
  };
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }
  
  FeatureImageImportType::Pointer itkFeatureImporter;
  SpeedImageExportType::Pointer itkSpeedExporter;
  //ETX

  vtkImageExport *vtkFeatureExporter;
  vtkImageImport *vtkSpeedImporter;

private:
  vtkvmtkThresholdSegmentationLevelSetImageFilter(const vtkvmtkThresholdSegmentationLevelSetImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkThresholdSegmentationLevelSetImageFilter&);  //
                                                                          // Not implemented
  
};

#endif




