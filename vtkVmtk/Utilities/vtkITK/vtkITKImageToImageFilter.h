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
// vtkITKImageToImageFilter provides a foo

#ifndef __vtkITKImageToImageFilter_h
#define __vtkITKImageToImageFilter_h

#if defined(_MSC_VER) && (_MSC_VER < 1300)
#include "fstream.h"
#endif

#include "itkCommand.h"
#include "vtkSystemIncludes.h"
#include "vtkCommand.h"
#include "itkProcessObject.h"
#include "vtkImageImport.h"
#include "vtkImageExport.h"
#include "vtkImageToImageFilter.h"
#include "vtkImageCast.h"
#include "vtkImageData.h"

#ifdef VTK_USE_EXECUTIVES
#include "vtkExecutive.h"
#endif

#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif


#undef itkExceptionMacro  
#define itkExceptionMacro(x) \
  { \
  ::itk::OStringStream message; \
  message << "itk::ERROR: " << this->GetNameOfClass() \
          << "(" << this << "): " x; \
  std::cerr << message.str().c_str() << std::endl; \
  }

#undef itkGenericExceptionMacro  
#define itkGenericExceptionMacro(x) \
  { \
  ::itk::OStringStream message; \
  message << "itk::ERROR: " x; \
  std::cerr << message.str() << std::endl; \
  }

class VTK_EXPORT vtkITKImageToImageFilter : public vtkImageToImageFilter
{
public:
  vtkTypeMacro(vtkITKImageToImageFilter,vtkImageToImageFilter);

  // Description:
  // vtkITK filters typically cast their input to pixel type
  // consistent with the particular instantiation of the ITK filter.
  // If we know the types are going to be consistent, we can save
  // memory by skipping the cast. Default is to cast.
  vtkBooleanMacro(CastInput, int);
  vtkSetMacro(CastInput, int);
  vtkGetMacro(CastInput, int);

  // Description:
  // vtkITK filters can be told to release their bulk output data
  // during a pipeline update.  Setting the
  // ReleaseDataBeforeUpdateFlag can minimize peak memory utilization
  // during a pipeline update.
  virtual void SetReleaseDataBeforeUpdateFlag(int ) {};
  virtual int GetReleaseDataBeforeUpdateFlag() {return 0;};
  vtkBooleanMacro(ReleaseDataBeforeUpdateFlag, int);
  
  // Description:
  // PrintSelf routine for the portion of the pipeline that is
  // constructed in this class.
  void PrintSelf(ostream& os, vtkIndent indent)
  {
    Superclass::PrintSelf ( os, indent );
    this->vtkExporter->PrintSelf ( os, indent );
    this->vtkImporter->PrintSelf ( os, indent );
//    os << indent << "CastInput: " << (this->CastInput ? "On" : "Off") << std::endl;
  };
  
  // Description:
  // This method considers the sub filters MTimes when computing this objects
  // modified time.
  unsigned long int GetMTime()
  {
    unsigned long int t1, t2;
  
    t1 = this->Superclass::GetMTime();
    t2 = this->vtkExporter->GetMTime();
    if (t2 > t1)
      {
      t1 = t2;
      }
    t2 = this->vtkImporter->GetMTime();
    if (t2 > t1)
      {
      t1 = t2;
      }
    return t1;
  };

  // Description:
  // Pass modified message to itk filter
  void Modified()
  {
    this->Superclass::Modified();
    if (this->m_Process)
      {
      m_Process->Modified();
      }
  };
  
  // Description:
  // Pass DebugOn.
  void DebugOn()
  {
    this->m_Process->DebugOn();
  };
  
  // Description:
  // Pass DebugOff.
  void DebugOff()
  {
    this->m_Process->DebugOff();
  };
  
  // Description:
  // Pass SetNumberOfThreads.
  void SetNumberOfThreads(int val)
  {
    this->m_Process->SetNumberOfThreads(val);
  };
  
  // Description:
  // Pass SetNumberOfThreads.
  int GetNumberOfThreads()
  {
    return this->m_Process->GetNumberOfThreads();
  };
  
  // Description:
  // This method returns the cache to make a connection
  // It justs feeds the request to the sub filter.
  void SetOutput ( vtkImageData* d ) { 
#ifdef VTK_USE_EXECUTIVES
    if (d == this->vtkImporter->GetOutput())
      {
      return;
      }
    // Ask the executive to setup the new output.
    this->vtkImporter->GetExecutive()->SetOutputData(0, d);
    this->vtkImporter->Modified();
#else
    this->vtkImporter->SetOutput(d);
#endif
  };
  virtual vtkImageData *GetOutput() { return this->vtkImporter->GetOutput(); };
  virtual vtkImageData *GetOutput(int idx)
  {
    return (vtkImageData *) this->vtkImporter->GetOutput(idx);
  };

  // Description:
  // Set the Input of the filter.
  virtual void SetInput(vtkImageData *Input)
  {
    this->vtkCast->SetInput(Input);
  };

  // Description:
  // Turn on/off flag to control whether this object's data is released
  // after being used by a source.
  virtual void SetReleaseDataFlag(int f)
    {
      this->vtkCast->SetReleaseDataFlag(f);
    }

  // Description: Override vtkSource's Update so that we can access
  // this class's GetOutput(). vtkSource's GetOutput is not virtual.
  void Update()
    {
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
        this->vtkExporter->SetInput( 
          static_cast<vtkImageData *>(this->vtkCast->GetInput()) );
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

  // Description: Override vtkSource's UpdateWholeExtent so that we can access
  // this class's GetOutput(). vtkSource's GetOutput is not virtual.
  void UpdateWholeExtent()
    {
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
        this->vtkExporter->SetInput( 
          static_cast<vtkImageData *>(this->vtkCast->GetInput() ));
        }
      
      
      // Force the internal pipeline to update.
      if (this->GetOutput(0))
        {
        this->GetOutput(0)->GetSource()->UpdateWholeExtent();
        }
    }

  //BTX
  void HandleProgressEvent ()
  {
    if ( this->m_Process )
      {
      this->UpdateProgress ( m_Process->GetProgress() );
      }
  };
  void HandleStartEvent ()
  {
    this->InvokeEvent(vtkCommand::StartEvent,NULL);
  };
  void HandleEndEvent ()
  {
    this->InvokeEvent(vtkCommand::EndEvent,NULL);
  };
  // ETX  

 protected:

  // BTX
  // Dummy ExecuteData
  void ExecuteData (vtkDataObject *)
  {
    vtkWarningMacro(<< "This filter does not respond to Update(). Doing a GetOutput->Update() instead.");
  }
  // ETX

  vtkITKImageToImageFilter()
  {
    // Need an import, export, and a ITK pipeline
    this->vtkCast = vtkImageCast::New();
    this->vtkExporter = vtkImageExport::New();
    this->vtkImporter = vtkImageImport::New();
    this->vtkExporter->SetInput ( this->vtkCast->GetOutput() );
    this->m_Process = NULL;
    this->m_ProgressCommand = MemberCommand::New();
    this->m_ProgressCommand->SetCallbackFunction ( this, &vtkITKImageToImageFilter::HandleProgressEvent );
    this->m_StartEventCommand = MemberCommand::New();
    this->m_StartEventCommand->SetCallbackFunction ( this, &vtkITKImageToImageFilter::HandleStartEvent );
    this->m_EndEventCommand = MemberCommand::New();
    this->m_EndEventCommand->SetCallbackFunction ( this, &vtkITKImageToImageFilter::HandleEndEvent );
    // default is to cast the input pixel type
    this->CastInput = 1;
  };
  ~vtkITKImageToImageFilter()
  {
//     std::cerr << "Destructing vtkITKImageToImageFilter" << std::endl;
    this->vtkExporter->Delete();
    this->vtkImporter->Delete();
    this->vtkCast->Delete();
  };

  // BTX  
  void LinkITKProgressToVTKProgress ( itk::ProcessObject* process )
  {
    if ( process )
      {
      this->m_Process = process;
      this->m_Process->AddObserver ( itk::ProgressEvent(), this->m_ProgressCommand );
      this->m_Process->AddObserver ( itk::StartEvent(), this->m_StartEventCommand );
      this->m_Process->AddObserver ( itk::EndEvent(), this->m_EndEventCommand );
      }
  };

  typedef itk::SimpleMemberCommand<vtkITKImageToImageFilter> MemberCommand;
  typedef MemberCommand::Pointer MemberCommandPointer;

  itk::ProcessObject::Pointer m_Process;
  MemberCommandPointer m_ProgressCommand;
  MemberCommandPointer m_StartEventCommand;
  MemberCommandPointer m_EndEventCommand;
  
  // ITK Progress object
  // To/from VTK
  vtkImageCast* vtkCast;
  vtkImageImport* vtkImporter;
  vtkImageExport* vtkExporter;  
  //ETX

  int CastInput;
  
private:
  vtkITKImageToImageFilter(const vtkITKImageToImageFilter&);  // Not implemented.
  void operator=(const vtkITKImageToImageFilter&);  // Not implemented.
};

#endif




