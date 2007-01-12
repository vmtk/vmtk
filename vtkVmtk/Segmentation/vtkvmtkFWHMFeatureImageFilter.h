/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkFWHMFeatureImageFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.3 $

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

// .NAME vtkvmtkCollidingFrontsImageFilter - Wrapper class around itk::CollidingFrontsImageFilter
// .SECTION Description
// vtkvmtkCollidingFrontsImageFilter


#ifndef __vtkvmtkFWHMFeatureImageFilter_h
#define __vtkvmtkFWHMFeatureImageFilter_h

#include "vtkITKImageToImageFilterFF.h"
#include "itkFWHMFeatureImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkFWHMFeatureImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkvmtkFWHMFeatureImageFilter *New();
  vtkTypeRevisionMacro(vtkvmtkFWHMFeatureImageFilter, vtkITKImageToImageFilterFF);

  void SetUseImageSpacing ( int value )
  {
    DelegateITKInputMacro ( SetUseImageSpacing, (bool) value );
  }

  void UseImageSpacingOn()
  {
    this->SetUseImageSpacing (true);
  }
  void UseImageSpacingOff()
  {
    this->SetUseImageSpacing (false);
  }

  int GetUseImageSpacing()
  { 
    DelegateITKOutputMacro ( GetUseImageSpacing ); 
  }

  void SetRadius ( int value[3] )
  {
    //    DelegateITKInputMacro ( SetRadius, (int*) value );
    this->Radius[0] = value[0];
    this->Radius[1] = value[1];
    this->Radius[2] = value[2];
    //BTX
    typedef ImageFilterType::StructuringElementRadiusType RadiusType;
    long unsigned int radiusValue[3];
    radiusValue[0] = static_cast<long unsigned int>(value[0]);
    radiusValue[1] = static_cast<long unsigned int>(value[1]);
    radiusValue[2] = static_cast<long unsigned int>(value[2]);
    //ETX
    RadiusType radius;
    radius.SetSize(radiusValue);
    this->GetImageFilterPointer()->SetRadius(radius);
    this->Modified();
  }

  vtkGetVectorMacro(Radius,int,3);

  void SetBackgroundValue ( float value )
  {
    DelegateITKInputMacro ( SetBackgroundValue, (float) value );
  }

  float GetBackgroundValue()
  { 
    DelegateITKOutputMacro ( GetBackgroundValue ); 
  }

  void Update()
  {
    this->itkImporter->Update();
    if (this->vtkExporter->GetInput())
    {
    } 
     
    // Force the internal pipeline to update.
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
  typedef itk::FWHMFeatureImageFilter<Superclass::InputImageType, Superclass::OutputImageType> ImageFilterType;
  vtkvmtkFWHMFeatureImageFilter() : Superclass ( ImageFilterType::New() )
    { 
      this->Radius = new int[3];
    }
  ~vtkvmtkFWHMFeatureImageFilter() 
    { 
      if (this->Radius)
        {
        delete[] this->Radius;
        this->Radius = NULL;
        }
    }
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }
  //ETX

  int* Radius;
  float BackgroundValue;

private:
  vtkvmtkFWHMFeatureImageFilter(const vtkvmtkFWHMFeatureImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkFWHMFeatureImageFilter&);  // Not implemented.
};

#endif
