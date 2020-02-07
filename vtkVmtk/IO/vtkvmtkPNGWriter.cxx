/*=========================================================================
                                                                                                                                    
Program:   VMTK
Module:    $RCSfile: vtkvmtkPNGWriter.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:47 $
Version:   $Revision: 1.6 $
                                                                                                                                    
  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
                                                                                                                                    
=========================================================================*/

#include "vtkvmtkPNGWriter.h"
#include "vtkUnsignedCharArray.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkErrorCode.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"
#if (VTK_MAJOR_VERSION > 5)
#include "vtkInformation.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#endif

#include <math.h>

#include <string.h>

//Ari Edelkind, http://episec.com/people/edelkind/

char b64string[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

long base64_encode (char* to, char* from, unsigned int len)
{
  char *fromp = from;
  char *top = to;
  unsigned char cbyte;
  unsigned char obyte;
  char end[3];

  for (; len >= 3; len -= 3) {
    cbyte = *fromp++;
    *top++ = b64string[(int)(cbyte >> 2)];
    obyte = (cbyte << 4) & 0x30;    /* 0011 0000 */

    cbyte = *fromp++;
    obyte |= (cbyte >> 4);      /* 0000 1111 */
    *top++ = b64string[(int)obyte];
    obyte = (cbyte << 2) & 0x3C;    /* 0011 1100 */

    cbyte = *fromp++;
    obyte |= (cbyte >> 6);      /* 0000 0011 */
    *top++ = b64string[(int)obyte];
    *top++ = b64string[(int)(cbyte & 0x3F)];/* 0011 1111 */
  }

  if (len) {
    end[0] = *fromp++;
    if (--len) end[1] = *fromp++; else end[1] = 0;
    end[2] = 0;

    cbyte = end[0];
    *top++ = b64string[(int)(cbyte >> 2)];
    obyte = (cbyte << 4) & 0x30;    /* 0011 0000 */

    cbyte = end[1];
    obyte |= (cbyte >> 4);
    *top++ = b64string[(int)obyte];
    obyte = (cbyte << 2) & 0x3C;    /* 0011 1100 */

    if (len) *top++ = b64string[(int)obyte];
    else *top++ = '=';
    *top++ = '=';
  }
  *top = 0;
  return top - to;
}


vtkStandardNewMacro(vtkvmtkPNGWriter);

vtkvmtkPNGWriter::vtkvmtkPNGWriter()
{
  this->Base64Image = NULL;
  this->WriteToBase64 = 0;
  this->FlipImage = 0;
}

vtkvmtkPNGWriter::~vtkvmtkPNGWriter()
{
  if (this->Base64Image)
    {
    delete[] this->Base64Image;
    this->Base64Image = NULL;
    }
}

void vtkvmtkPNGWriter::Write()
{
  if (!this->WriteToBase64) {
    Superclass::Write();
    return;
  }

  int previousWriteToMemory = this->GetWriteToMemory();

  this->WriteToMemoryOn();

  //Superclass::Write();

  this->SetErrorCode(vtkErrorCode::NoError);

  // Error checking
  if ( this->GetInput() == NULL )
    {
    vtkErrorMacro(<<"Write:Please specify an input!");
    return;
    }
  if (!this->WriteToMemory && !this->GetFileName() && !this->GetFilePattern())
    {
    vtkErrorMacro(<<"Write:Please specify either a FileName or a file prefix and pattern");
    this->SetErrorCode(vtkErrorCode::NoFileNameError);
    return;
    }

  // Make sure the file name is allocated
  this->InternalFileName =
    new char[(this->GetFileName() ? strlen(this->GetFileName()) : 1) +
            (this->GetFilePrefix() ? strlen(this->GetFilePrefix()) : 1) +
            (this->GetFilePattern() ? strlen(this->GetFilePattern()) : 1) + 10];

  // Fill in image information.
  int *wExtent;
#if (VTK_MAJOR_VERSION <= 5)
  this->GetInput()->UpdateInformation();
  wExtent = this->GetInput()->GetWholeExtent();
#else
  this->UpdateInformation();
  vtkInformation *inInfoImage = this->GetInputPortInformation(0);
  inInfoImage->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), wExtent);
#endif

  this->FileNumber = wExtent[4];
  this->MinimumFileNumber = this->MaximumFileNumber = this->FileNumber;
  this->FilesDeleted = 0;
  this->UpdateProgress(0.0);
  // loop over the z axis and write the slices
  for (this->FileNumber = wExtent[4]; this->FileNumber <= wExtent[5];
       ++this->FileNumber)
    {
    this->MaximumFileNumber = this->FileNumber;
    int updatedExtent[6] = {wExtent[0], wExtent[1],
                            wExtent[2], wExtent[3],
                            this->FileNumber,
                            this->FileNumber};
#if (VTK_MAJOR_VERSION <= 5)
    this->GetInput()->SetUpdateExtent(updatedExtent);
#endif
    // determine the name
    if (this->GetFileName())
      {
      sprintf(this->InternalFileName,"%s",this->GetFileName());
      }
    else
      {
      if (this->GetFilePrefix())
        {
        sprintf(this->InternalFileName, this->GetFilePattern(),
                this->GetFilePrefix(), this->FileNumber);
        }
      else
        {
        sprintf(this->InternalFileName, this->GetFilePattern(), this->FileNumber);
        }
      }
#if (VTK_MAJOR_VERSION <= 5)
    this->GetInput()->UpdateData();
#endif
    
    if (!this->FlipImage)
      {
#if (VTK_MAJOR_VERSION <= 5)
      this->WriteSlice(this->GetInput());
#else
      this->WriteSlice(this->GetInput(), updatedExtent);
#endif
      }
    else
      {
      vtkImageData* flippedInput = vtkImageData::New();
      flippedInput->DeepCopy(this->GetInput());
#if (VTK_MAJOR_VERSION <= 5)
      flippedInput->SetUpdateExtent(updatedExtent);
#endif
      vtkDataArray* inputArray = this->GetInput()->GetPointData()->GetScalars();
      vtkDataArray* flippedInputArray = flippedInput->GetPointData()->GetScalars();
      for (int i=0; i<wExtent[3]-wExtent[2]+1; i++)
        {
        for (int j=0; j<wExtent[1]-wExtent[0]+1; j++)
          {
          flippedInputArray->SetTuple((wExtent[3]-wExtent[2]-i)*(wExtent[1]-wExtent[0]+1)+j,inputArray->GetTuple(i*(wExtent[1]-wExtent[0]+1)+j));
          }
        }
#if (VTK_MAJOR_VERSION <= 5)
      this->WriteSlice(flippedInput);
#else
      this->WriteSlice(flippedInput, updatedExtent);
#endif
      flippedInput->Delete();
      }

    if (this->ErrorCode == vtkErrorCode::OutOfDiskSpaceError)
      {
      this->DeleteFiles();
      break;
      }
    this->UpdateProgress((this->FileNumber - wExtent[4])/
                         (wExtent[5] - wExtent[4] + 1.0));
    }
  delete [] this->InternalFileName;
  this->InternalFileName = NULL;

  vtkUnsignedCharArray* result = this->GetResult();
  char* data = (char*)result->GetPointer(0);
  int length = result->GetNumberOfTuples() * result->GetNumberOfComponents();

  char* flipped_data = new char[length];
  if (this->FlipImage)
    {
    for (int i=0; i<length; i++)
      {
      flipped_data[i] = data[length-i-1];
      }
    data = flipped_data;
    }

  char* encoded_data = new char[2*length];
  int output_length = base64_encode(encoded_data,data,length);
  this->SetBase64Image(encoded_data);

  delete[] encoded_data;
  delete[] flipped_data;

  this->SetWriteToMemory(previousWriteToMemory);
} 

void vtkvmtkPNGWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkPNGWriter::PrintSelf(os,indent);
}
