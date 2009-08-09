/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCollidingFrontsImageFilter.h,v $
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

// .NAME vtkvmtkCollidingFrontsImageFilter - Wrapper class around itk::CollidingFrontsImageFilter
// .SECTION Description
// vtkvmtkCollidingFrontsImageFilter


#ifndef __vtkvmtkCollidingFrontsImageFilter_h
#define __vtkvmtkCollidingFrontsImageFilter_h

#include "vtkvmtkITKImageToImageFilterFF.h"
#include "itkCollidingFrontsImageFilter.h"
#include "vtkIdList.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkCollidingFrontsImageFilter : public vtkvmtkITKImageToImageFilterFF
{
 public:
  static vtkvmtkCollidingFrontsImageFilter *New();
  vtkTypeRevisionMacro(vtkvmtkCollidingFrontsImageFilter, vtkvmtkITKImageToImageFilterFF);

  void SetApplyConnectivity ( int value )
  {
    DelegateITKInputMacro ( SetApplyConnectivity, (bool) value );
  }

  void ApplyConnectivityOn()
  {
    this->SetApplyConnectivity (true);
  }
  void ApplyConnectivityOff()
  {
    this->SetApplyConnectivity (false);
  }
  int GetApplyConnectivity()
  { DelegateITKOutputMacro ( GetApplyConnectivity ); }

  void SetNegativeEpsilon( double value )
  { DelegateITKInputMacro ( SetNegativeEpsilon, value ); }

  double GetNegativeEpsilon()
  { DelegateITKOutputMacro ( GetNegativeEpsilon ); }

  void SetStopOnTargets ( int value )
  {
    DelegateITKInputMacro ( SetStopOnTargets, (bool) value );
  }

  void StopOnTargetsOn()
  {
    this->SetStopOnTargets (true);
  }
  void StopOnTargetsOff()
  {
    this->SetStopOnTargets (false);
  }
  int GetStopOnTargets()
  { DelegateITKOutputMacro ( GetStopOnTargets ); }

  vtkSetObjectMacro(Seeds1,vtkIdList);
  vtkGetObjectMacro(Seeds1,vtkIdList);

  vtkSetObjectMacro(Seeds2,vtkIdList);
  vtkGetObjectMacro(Seeds2,vtkIdList);

  void Update()
  {
  //BTX
    this->itkImporter->Update();
    if (this->vtkExporter->GetInput())
    {
    ImageFilterType::NodeContainerPointer seeds1 = ImageFilterType::NodeContainer::New();
    int i;
    for (i=0; i<this->Seeds1->GetNumberOfIds(); i++)
      {
      // TODO: here we get the point. We should get the cell center instead.
      Superclass::InputImageType::PointType seedPoint(this->vtkExporter->GetInput()->GetPoint(this->Seeds1->GetId(i)));
      ImageFilterType::NodeType::IndexType seedIndex;
      this->itkImporter->GetOutput()->TransformPhysicalPointToIndex(seedPoint,seedIndex);
      ImageFilterType::PixelType seedValue = itk::NumericTraits<ImageFilterType::PixelType>::Zero;
      ImageFilterType::NodeType seed;
      seed.SetValue(seedValue);
      seed.SetIndex(seedIndex);
      seeds1->InsertElement(i,seed);
      }
    this->GetImageFilterPointer()->SetSeedPoints1(seeds1);

    ImageFilterType::NodeContainerPointer seeds2 = ImageFilterType::NodeContainer::New();
    for (i=0; i<this->Seeds2->GetNumberOfIds(); i++)
      {
      // TODO: here we get the point. We should get the cell center instead.
      Superclass::InputImageType::PointType seedPoint(this->vtkExporter->GetInput()->GetPoint(this->Seeds2->GetId(i)));
      ImageFilterType::NodeType::IndexType seedIndex;
      this->itkImporter->GetOutput()->TransformPhysicalPointToIndex(seedPoint,seedIndex);
      ImageFilterType::PixelType seedValue = itk::NumericTraits<ImageFilterType::PixelType>::Zero;
      ImageFilterType::NodeType seed;
      seed.SetValue(seedValue);
      seed.SetIndex(seedIndex);
      seeds2->InsertElement(i,seed);
      }
    this->GetImageFilterPointer()->SetSeedPoints2(seeds2);
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
  //ETX
  }

protected:
  //BTX
  typedef itk::CollidingFrontsImageFilter<Superclass::InputImageType, Superclass::OutputImageType> ImageFilterType;
  vtkvmtkCollidingFrontsImageFilter() : Superclass ( ImageFilterType::New() )
    { 
      this->Seeds1 = NULL; 
      this->Seeds2 = NULL; 
    }
  ~vtkvmtkCollidingFrontsImageFilter() 
    { 
      if (this->Seeds1) 
        {
        this->Seeds1->Delete();
        }
      if (this->Seeds2) 
        {
        this->Seeds2->Delete();
        }
    }
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }
  //ETX
  
  vtkIdList* Seeds1;
  vtkIdList* Seeds2;
                                                                            
private:
  vtkvmtkCollidingFrontsImageFilter(const vtkvmtkCollidingFrontsImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkCollidingFrontsImageFilter&);  // Not implemented.
};

#endif
