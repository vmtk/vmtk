/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkObjectnessMeasureImageFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.2 $

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

// .NAME vtkvmtkObjectnessMeasureImageFilter - Wrapper class around itk::ObjectnessMeasureImageFilter
// .SECTION Description
// vtkvmtkObjectnessMeasureImageFilter


#ifndef __vtkvmtkObjectnessMeasureImageFilter_h
#define __vtkvmtkObjectnessMeasureImageFilter_h


#include "vtkITKImageToImageFilterFF.h"
#include "itkMultiScaleHessianBasedMeasureImageFilter.h"
#include "itkHessianToObjectnessMeasureImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkObjectnessMeasureImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkvmtkObjectnessMeasureImageFilter *New();
  vtkTypeRevisionMacro(vtkvmtkObjectnessMeasureImageFilter, vtkITKImageToImageFilterFF);

  void SetSigmaMin(double value)
  {
    DelegateITKInputMacro(SetSigmaMinimum,value);
  }

  double GetSigmaMin()
  {
    DelegateITKOutputMacro(GetSigmaMinimum);
  }

  void SetSigmaMax(double value)
  {
    DelegateITKInputMacro(SetSigmaMaximum,value);
  }

  double GetSigmaMax()
  {
    DelegateITKOutputMacro(GetSigmaMaximum);
  }

  void SetNumberOfSigmaSteps(int value)
  {
    DelegateITKInputMacro(SetNumberOfSigmaSteps,value);
  }

  int GetNumberOfSigmaSteps()
  {
    DelegateITKOutputMacro(GetNumberOfSigmaSteps);
  }

  void SetSigmaStepMethodToEquispaced()
  {
    this->GetImageFilterPointer()->SetSigmaStepMethodToEquispaced();
    this->Modified();
  }
  
  void SetSigmaStepMethodToLogarithmic()
  {
    this->GetImageFilterPointer()->SetSigmaStepMethodToLogarithmic();
    this->Modified();
  }

  void SetAlpha(double value)
  {
    ImageFilterType* tempFilter = dynamic_cast<ImageFilterType*>(this->m_Filter.GetPointer()); 
    if (tempFilter) 
      { 
      dynamic_cast<ObjectnessFilterType*>(tempFilter->GetHessianToMeasureFilter())->SetAlpha(value); 
      this->Modified(); 
      }
  }

  double GetAlpha()
  {
    ImageFilterType* tempFilter = dynamic_cast<ImageFilterType*>(this->m_Filter.GetPointer()); 
    if (tempFilter) 
      { 
      return dynamic_cast<ObjectnessFilterType*>(tempFilter->GetHessianToMeasureFilter())->GetAlpha(); 
      }
    else
      {
      vtkErrorMacro ( << this->GetClassName() << " Error getting method. Dynamic cast returned 0" );
      return 0.0;
      }
  }

  void SetBeta(double value)
  {
    ImageFilterType* tempFilter = dynamic_cast<ImageFilterType*>(this->m_Filter.GetPointer()); 
    if (tempFilter) 
      { 
      dynamic_cast<ObjectnessFilterType*>(tempFilter->GetHessianToMeasureFilter())->SetBeta(value); 
      this->Modified(); 
      }
  }

  double GetBeta()
  {
    ImageFilterType* tempFilter = dynamic_cast<ImageFilterType*>(this->m_Filter.GetPointer()); 
    if (tempFilter) 
      { 
      return dynamic_cast<ObjectnessFilterType*>(tempFilter->GetHessianToMeasureFilter())->GetBeta(); 
      }
    else
      {
      vtkErrorMacro ( << this->GetClassName() << " Error getting method. Dynamic cast returned 0" );
      return 0.0;
      }
  }

  void SetGamma(double value)
  {
    ImageFilterType* tempFilter = dynamic_cast<ImageFilterType*>(this->m_Filter.GetPointer()); 
    if (tempFilter) 
      { 
      dynamic_cast<ObjectnessFilterType*>(tempFilter->GetHessianToMeasureFilter())->SetGamma(value); 
      this->Modified(); 
      }
  }

  double GetGamma()
  {
    ImageFilterType* tempFilter = dynamic_cast<ImageFilterType*>(this->m_Filter.GetPointer()); 
    if (tempFilter) 
      { 
      return dynamic_cast<ObjectnessFilterType*>(tempFilter->GetHessianToMeasureFilter())->GetGamma(); 
      }
    else
      {
      vtkErrorMacro ( << this->GetClassName() << " Error getting method. Dynamic cast returned 0" );
      return 0.0;
      }
  }

  void SetObjectDimension(int value)
  {
    ImageFilterType* tempFilter = dynamic_cast<ImageFilterType*>(this->m_Filter.GetPointer()); 
    if (tempFilter) 
      { 
      dynamic_cast<ObjectnessFilterType*>(tempFilter->GetHessianToMeasureFilter())->SetObjectDimension(value); 
      this->Modified(); 
      }
  }

  int GetObjectDimension()
  {
    ImageFilterType* tempFilter = dynamic_cast<ImageFilterType*>(this->m_Filter.GetPointer()); 
    if (tempFilter) 
      { 
      return dynamic_cast<ObjectnessFilterType*>(tempFilter->GetHessianToMeasureFilter())->GetObjectDimension(); 
      }
    else
      {
      vtkErrorMacro ( << this->GetClassName() << " Error getting method. Dynamic cast returned 0" );
      return 0;
      }
  }

  unsigned long int GetMTime()
  {
    unsigned long int t1 = this->Superclass::GetMTime();
    unsigned long int t2 = this->vtkScalesImporter->GetMTime();
    if (t2 > t1)
      {
      t1 = t2;
      }
    return t1;
  }

  virtual vtkImageData *GetScalesOutput() 
  { 
    this->vtkScalesImporter->Update(); 
    return this->vtkScalesImporter->GetOutput(); 
  }

protected:
  //BTX
  typedef itk::SymmetricSecondRankTensor<float,3> HessianPixelType;
  typedef itk::Image<HessianPixelType,3> HessianImageType;
  typedef itk::HessianToObjectnessMeasureImageFilter<HessianImageType,InputImageType> ObjectnessFilterType;
  typedef itk::MultiScaleHessianBasedMeasureImageFilter<InputImageType,HessianImageType,InputImageType> ImageFilterType;
  typedef ImageFilterType::ScalesImageType ScalesImageType;

  typedef itk::VTKImageExport<ScalesImageType> ScalesImageExportType;

  vtkvmtkObjectnessMeasureImageFilter() : Superclass(ImageFilterType::New())
  {
    this->itkScalesExporter = ScalesImageExportType::New();
    this->vtkScalesImporter = vtkImageImport::New();
#if VTK_MAJOR_VERSION > 5 || (VTK_MAJOR_VERSION == 5 && VTK_MINOR_VERSION > 2)
    this->vtkScalesImporter->SetScalarArrayName("Scalars_");
#endif
    ConnectPipelines(this->itkScalesExporter, this->vtkScalesImporter);
    ObjectnessFilterType::Pointer objectnessFilter = ObjectnessFilterType::New();
    objectnessFilter->SetScaleObjectnessMeasure(false);
    objectnessFilter->SetBrightObject(true);
    ImageFilterType* imageFilter = this->GetImageFilterPointer();
    imageFilter->SetSigmaStepMethodToEquispaced();
    imageFilter->GenerateScalesOutputOn();
    imageFilter->SetHessianToMeasureFilter(objectnessFilter);
    this->itkScalesExporter->SetInput(imageFilter->GetScalesOutput());
  }

  ~vtkvmtkObjectnessMeasureImageFilter() 
  {
    this->vtkScalesImporter->Delete();
  }
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()); }

  ScalesImageExportType::Pointer itkScalesExporter;
  vtkImageImport* vtkScalesImporter;
  //ETX

private:
  vtkvmtkObjectnessMeasureImageFilter(const vtkvmtkObjectnessMeasureImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkObjectnessMeasureImageFilter&);  // Not implemented.
};

#endif




