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
    DelegateITKInputMacro(SetSigmaMin,value);
  }

  double GetSigmaMin()
  {
    DelegateITKOutputMacro(GetSigmaMin);
  }

  void SetSigmaMax(double value)
  {
    DelegateITKInputMacro(SetSigmaMax,value);
  }

  double GetSigmaMax()
  {
    DelegateITKOutputMacro(GetSigmaMax);
  }

  void SetNumberOfSigmaSteps(int value)
  {
    DelegateITKInputMacro(SetNumberOfSigmaSteps,value);
  }

  int GetNumberOfSigmaSteps()
  {
    DelegateITKOutputMacro(GetNumberOfSigmaSteps);
  }

  void SetAlpha(double value)
  {
    DelegateITKInputMacro(GetHessianToMeasureFilter()->SetAlpha,value);
  }

  double GetAlpha()
  {
    DelegateITKOutputMacro(GetHessianToMeasureFilter()->GetAlpha);
  }

  void SetBeta(double value)
  {
    DelegateITKInputMacro(GetHessianToMeasureFilter()->SetBeta,value);
  }

  double GetBeta()
  {
    DelegateITKOutputMacro(GetHessianToMeasureFilter()->GetBeta);
  }

  void SetGamma(double value)
  {
    DelegateITKInputMacro(GetHessianToMeasureFilter()->SetGamma,value);
  }

  double GetGamma()
  {
    DelegateITKOutputMacro(GetHessianToMeasureFilter()->GetGamma);
  }

  void SetObjectDimension(int value)
  {
    DelegateITKInputMacro(GetHessianToMeasureFilter()->SetObjectDimension,value);
  }

  double GetObjectDimension()
  {
    DelegateITKOutputMacro(GetHessianToMeasureFilter()->GetObjectDimension);
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
  typedef itk::HessianToObjectnessMeasureImageFilter<double,3> ObjectnessFilterType;
  typedef itk::MultiScaleHessianBasedMeasureImageFilter<Superclass::InputImageType,ObjectnessFilterType> ImageFilterType;
  typedef itk::VTKImageExport<InputImageType> ScalesImageExportType;

  vtkvmtkObjectnessMeasureImageFilter() : Superclass(ImageFilterType::New())
  {
    this->itkScalesExporter = ScalesImageExportType::New();
    this->vtkScalesImporter = vtkImageImport::New();
    this->vtkScalesImporter->SetScalarArrayName("Scalars_");
    ConnectPipelines(this->itkScalesExporter, this->vtkScalesImporter);
    ImageFilterType* imageFilter = this->GetImageFilterPointer();
    imageFilter->SetSigmaStepMethodToEquispaced();
    imageFilter->GenerateScalesOutputOn();
    ObjectnessFilterType* objectnessFilter = imageFilter->GetHessianToMeasureFilter();
    objectnessFilter->SetScaleObjectnessMeasure(false);
    objectnessFilter->SetBrightObject(true);
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




