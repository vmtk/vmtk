/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkFastMarchingUpwindGradientImageFilter.h,v $
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

// .NAME vtkvmtkFastMarchingUpwindGradientImageFilter - Wrapper class around itk::FastMarchingUpwindGradientImageFilter
// .SECTION Description
// vtkvmtkFastMarchingUpwindGradientImageFilter


#ifndef __vtkvmtkFastMarchingUpwindGradientImageFilter_h
#define __vtkvmtkFastMarchingUpwindGradientImageFilter_h

#include "vtkITKImageToImageFilterFF.h"
#include "itkFastMarchingUpwindGradientImageFilter.h"
#include "vtkIdList.h"
#include "vtkObjectFactory.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkFastMarchingUpwindGradientImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkvmtkFastMarchingUpwindGradientImageFilter *New();
  vtkTypeRevisionMacro(vtkvmtkFastMarchingUpwindGradientImageFilter, vtkITKImageToImageFilterFF);

//   void SetUseImageSpacing ( int value )
//   {
//     DelegateITKInputMacro ( SetUseImageSpacing, (bool) value );
//   }

//   void UseImageSpacingOn()
//   {
//     this->SetUseImageSpacing (true);
//   }
//   void UseImageSpacingOff()
//   {
//     this->SetUseImageSpacing (false);
//   }
//   int GetUseImageSpacing()
//   { DelegateITKOutputMacro ( GetUseImageSpacing ); }

  void SetGenerateGradientImage ( int value )
  {
    DelegateITKInputMacro ( SetGenerateGradientImage, (bool) value );
  }

  void GenerateGradientImageOn()
  {
    this->SetGenerateGradientImage (true);
  }

  void GenerateGradientImageOff()
  {
    this->SetGenerateGradientImage (false);
  }

  int GetGenerateGradientImage()
  { 
    DelegateITKOutputMacro ( GetGenerateGradientImage ); 
  }

  void SetTargetReachedMode ( int value )
  {
    DelegateITKInputMacro ( SetTargetReachedMode, value );
  }

  int GetTargetReachedMode()
  { 
    DelegateITKOutputMacro ( GetTargetReachedMode ); 
  }

  void SetTargetReachedModeToOneTarget()
  {
    this->SetTargetReachedMode ( ImageFilterType::OneTarget );
  }

  void SetTargetReachedModeToAllTargets()
  {
    this->SetTargetReachedMode ( ImageFilterType::AllTargets );
  }

  double GetTargetValue()
  { 
    DelegateITKOutputMacro ( GetTargetValue ); 
  }

  void SetTargetOffset ( double value )
  {
    DelegateITKInputMacro ( SetTargetOffset, value );
  }

  double GetTargetOffset()
  { 
    DelegateITKOutputMacro ( GetTargetOffset ); 
  }

  vtkSetObjectMacro(Seeds,vtkIdList);
  vtkGetObjectMacro(Seeds,vtkIdList);

  vtkSetObjectMacro(Targets,vtkIdList);
  vtkGetObjectMacro(Targets,vtkIdList);

  void Update()
  {
  //BTX
    // Wire the internal pipeline according to how the user selected
    // CastInput.
    if (this->CastInput)
      {
      // set the pipeline to do an internal cast to a pixeltype
      // consistent with the ITK instantiation
      this->vtkExporter->SetInput( this->vtkCast->GetOutput() );
      }
    else
      {
      // skip the cast operation
      this->vtkExporter->SetInput( this->vtkCast->GetInput() );
      }

    this->itkImporter->Update();
    if (this->vtkExporter->GetInput())
    {
    ImageFilterType::NodeContainerPointer seeds = ImageFilterType::NodeContainer::New();
    int i;
    for (i=0; i<this->Seeds->GetNumberOfIds(); i++)
      {
      // TODO: here we get the point. We should get the cell center instead.
      Superclass::InputImageType::PointType seedPoint(this->vtkExporter->GetInput()->GetPoint(this->Seeds->GetId(i)));
      ImageFilterType::NodeType::IndexType seedIndex;
      this->itkImporter->GetOutput()->TransformPhysicalPointToIndex(seedPoint,seedIndex);
      ImageFilterType::PixelType seedValue = itk::NumericTraits<ImageFilterType::PixelType>::Zero;
      ImageFilterType::NodeType seed;
      seed.SetValue(seedValue);
      seed.SetIndex(seedIndex);
      seeds->InsertElement(i,seed);
      }
    this->GetImageFilterPointer()->SetTrialPoints(seeds);

    ImageFilterType::NodeContainerPointer targets = ImageFilterType::NodeContainer::New();
    for (i=0; i<this->Targets->GetNumberOfIds(); i++)
      {
      // TODO: here we get the point. We should get the cell center instead.
      Superclass::InputImageType::PointType seedPoint(this->vtkExporter->GetInput()->GetPoint(this->Targets->GetId(i)));
      ImageFilterType::NodeType::IndexType seedIndex;
      this->itkImporter->GetOutput()->TransformPhysicalPointToIndex(seedPoint,seedIndex);
      ImageFilterType::PixelType seedValue = itk::NumericTraits<ImageFilterType::PixelType>::Zero;
      ImageFilterType::NodeType seed;
      seed.SetValue(seedValue);
      seed.SetIndex(seedIndex);
      targets->InsertElement(i,seed);
      }
    this->GetImageFilterPointer()->SetTargetPoints(targets);
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
  typedef itk::FastMarchingUpwindGradientImageFilter<Superclass::InputImageType, Superclass::OutputImageType> ImageFilterType;
  vtkvmtkFastMarchingUpwindGradientImageFilter() : Superclass ( ImageFilterType::New() )
    { 
      this->Seeds = NULL; 
      this->Targets = NULL; 
    }
  ~vtkvmtkFastMarchingUpwindGradientImageFilter() 
    { 
      if (this->Seeds) 
        {
        this->Seeds->Delete();
        }
      if (this->Targets) 
        {
        this->Targets->Delete();
        }
    }
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }
  //ETX
  
  vtkIdList* Seeds;
  vtkIdList* Targets;
                                                                            
private:
  vtkvmtkFastMarchingUpwindGradientImageFilter(const vtkvmtkFastMarchingUpwindGradientImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkFastMarchingUpwindGradientImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkvmtkFastMarchingUpwindGradientImageFilter, "$Revision: 1.4 $");
vtkStandardNewMacro(vtkvmtkFastMarchingUpwindGradientImageFilter);

#endif
