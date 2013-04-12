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
  }

  int previousWriteToMemory = this->GetWriteToMemory();

  this->WriteToMemoryOn();
  Superclass::Write();

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
