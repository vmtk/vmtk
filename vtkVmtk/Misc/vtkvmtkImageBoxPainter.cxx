/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkImageBoxPainter.cxx,v $
  Language:  C++
  Date:      $Date: 2005/03/04 11:13:24 $
  Version:   $Revision: 1.2 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkvmtkImageBoxPainter.h"

#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"
#if (VTK_MAJOR_VERSION > 5)
#include "vtkInformation.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#endif

vtkStandardNewMacro(vtkvmtkImageBoxPainter);

vtkvmtkImageBoxPainter::vtkvmtkImageBoxPainter() 
{
  this->PaintValue = 0.0;

  this->BoxBounds[0] = this->BoxBounds[2] = this->BoxBounds[4] = 0.0;
  this->BoxBounds[1] = this->BoxBounds[3] = this->BoxBounds[5] = 1.0;

  this->BoxExtent[0] = this->BoxExtent[2] = this->BoxExtent[4] = 0;
  this->BoxExtent[1] = this->BoxExtent[3] = this->BoxExtent[5] = 1;

  this->BoxDefinition = vtkvmtkImageBoxPainter::USE_EXTENT;
}

// The switch statement in Execute will call this method with
// the appropriate input type (IT). Note that this example assumes
// that the output data type is the same as the input data type.
// This is not always the case.
template <class IT>
void vtkvmtkImageBoxPainterExecute(vtkImageData* input,
                                        vtkImageData* output,
                                        IT* inPtr, IT* outPtr, 
                                        int* boxExtent, IT paintValue)
{
  int dims[3];
  input->GetDimensions(dims);
  if (input->GetScalarType() != output->GetScalarType())
    {
    vtkGenericWarningMacro(<< "Execute: input ScalarType, " << input->GetScalarType()
    << ", must match out ScalarType " << output->GetScalarType());
    return;
    }
  
  int size = dims[0]*dims[1]*dims[2];

  int ijk[3];
  vtkIdType id;

  for (int i=0; i<size; i++)
    {
    outPtr[i] = inPtr[i];
    }

  for (ijk[2]=boxExtent[4]; ijk[2]<=boxExtent[5]; ++ijk[2])
    {
    for (ijk[1]=boxExtent[2]; ijk[1]<=boxExtent[3]; ++ijk[1])
      {
      for (ijk[0]=boxExtent[0]; ijk[0]<=boxExtent[1]; ++ijk[0])
        {
        id = input->ComputePointId(ijk);
        outPtr[id] = paintValue;
        }
      }
    }
}

void vtkvmtkImageBoxPainter::SimpleExecute(vtkImageData* input,
                                                vtkImageData* output)
{

  void* inPtr = input->GetScalarPointer();
  void* outPtr = output->GetScalarPointer();

  double spacing[3];
  input->GetSpacing(spacing);

  if (this->BoxDefinition == vtkvmtkImageBoxPainter::USE_BOUNDS)
    {
    this->BoxExtent[0] = (vtkIdType) ceil(this->BoxBounds[0] / spacing[0]);
    this->BoxExtent[1] = (vtkIdType) floor(this->BoxBounds[1] / spacing[0]);
    this->BoxExtent[2] = (vtkIdType) ceil(this->BoxBounds[2] / spacing[1]);
    this->BoxExtent[3] = (vtkIdType) floor(this->BoxBounds[3] / spacing[1]);
    this->BoxExtent[4] = (vtkIdType) ceil(this->BoxBounds[4] / spacing[2]);
    this->BoxExtent[5] = (vtkIdType) floor(this->BoxBounds[5] / spacing[2]);
    }

  int extent[6];
#if (VTK_MAJOR_VERSION <= 5)
  input->GetWholeExtent(extent);
#else
  vtkInformation *inInfoImage = this->GetInputPortInformation(0);
  inInfoImage->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent);
#endif

  if ( this->BoxExtent[0] < extent[0] || this->BoxExtent[1] > extent[1] ||
       this->BoxExtent[2] < extent[2] || this->BoxExtent[3] > extent[3] ||
       this->BoxExtent[4] < extent[4] || this->BoxExtent[5] > extent[5] )
    {
    vtkErrorMacro(<<"BoxExtent exceeds input WholeExtent");
    return;
    }

  int relativeBoxExtent[6];
  relativeBoxExtent[0] = this->BoxExtent[0] - extent[0];
  relativeBoxExtent[1] = this->BoxExtent[1] - extent[0];
  relativeBoxExtent[2] = this->BoxExtent[2] - extent[2];
  relativeBoxExtent[3] = this->BoxExtent[3] - extent[2];
  relativeBoxExtent[4] = this->BoxExtent[4] - extent[4];
  relativeBoxExtent[5] = this->BoxExtent[5] - extent[4];

  switch(output->GetScalarType())
    {
    // This is simple a #define for a big case list. It handles
    // all data types vtk can handle.
    vtkTemplateMacro(vtkvmtkImageBoxPainterExecute( input, output,
                      static_cast<VTK_TT *>(inPtr), static_cast<VTK_TT *>(outPtr), relativeBoxExtent, 
                      static_cast<VTK_TT>(this->PaintValue)));
    default:
      vtkGenericWarningMacro("Execute: Unknown input ScalarType");
      return;
    }
}
