/*=========================================================================
                                                                                                                                    
Program:   VMTK
Module:    $RCSfile: vtkvmtkPNGWriter.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:47 $
Version:   $Revision: 1.6 $
                                                                                                                                    
  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

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

vtkCxxRevisionMacro(vtkvmtkPNGWriter, "$Revision: 1.6 $");
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
  if (!this->WriteToMemory && !this->FileName && !this->FilePattern)
    {
    vtkErrorMacro(<<"Write:Please specify either a FileName or a file prefix and pattern");
    this->SetErrorCode(vtkErrorCode::NoFileNameError);
    return;
    }

  // Make sure the file name is allocated
  this->InternalFileName =
    new char[(this->FileName ? strlen(this->FileName) : 1) +
            (this->FilePrefix ? strlen(this->FilePrefix) : 1) +
            (this->FilePattern ? strlen(this->FilePattern) : 1) + 10];

  // Fill in image information.
  this->GetInput()->UpdateInformation();
  int *wExtent;
  wExtent = this->GetInput()->GetWholeExtent();
  this->FileNumber = this->GetInput()->GetWholeExtent()[4];
  this->MinimumFileNumber = this->MaximumFileNumber = this->FileNumber;
  this->FilesDeleted = 0;
  this->UpdateProgress(0.0);
  // loop over the z axis and write the slices
  for (this->FileNumber = wExtent[4]; this->FileNumber <= wExtent[5];
       ++this->FileNumber)
    {
    this->MaximumFileNumber = this->FileNumber;
    this->GetInput()->SetUpdateExtent(wExtent[0], wExtent[1],
                                      wExtent[2], wExtent[3],
                                      this->FileNumber,
                                      this->FileNumber);
    // determine the name
    if (this->FileName)
      {
      sprintf(this->InternalFileName,"%s",this->FileName);
      }
    else
      {
      if (this->FilePrefix)
        {
        sprintf(this->InternalFileName, this->FilePattern,
                this->FilePrefix, this->FileNumber);
        }
      else
        {
        sprintf(this->InternalFileName, this->FilePattern,this->FileNumber);
        }
      }
    this->GetInput()->UpdateData();
    
    if (!this->FlipImage)
      {
      this->WriteSlice(this->GetInput());
      }
    else
      {
      vtkImageData* flippedInput = vtkImageData::New();
      flippedInput->DeepCopy(this->GetInput());
      flippedInput->SetUpdateExtent(wExtent[0], wExtent[1],
                                    wExtent[2], wExtent[3],
                                    this->FileNumber,
                                    this->FileNumber);
      vtkDataArray* inputArray = this->GetInput()->GetPointData()->GetScalars();
      vtkDataArray* flippedInputArray = flippedInput->GetPointData()->GetScalars();
      int numberOfTuples = inputArray->GetNumberOfTuples();
      for (int i=0; i<numberOfTuples; i++)
        {
        flippedInputArray->SetTuple(numberOfTuples-1-i,inputArray->GetTuple(i));
        }
      this->WriteSlice(flippedInput);
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

  char* encoded_data = new char[2*length];
  int output_length = base64_encode(encoded_data,data,length);
  this->SetBase64Image(encoded_data);

  delete[] encoded_data;

  this->SetWriteToMemory(previousWriteToMemory);
} 

void vtkvmtkPNGWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkPNGWriter::PrintSelf(os,indent);
}
