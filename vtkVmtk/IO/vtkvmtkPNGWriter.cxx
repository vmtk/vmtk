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
//#include <stdint.h>
//#include <stdlib.h>

vtkCxxRevisionMacro(vtkvmtkPNGWriter, "$Revision: 1.6 $");
vtkStandardNewMacro(vtkvmtkPNGWriter);

vtkvmtkPNGWriter::vtkvmtkPNGWriter()
{
  this->Base64Image = NULL;
  this->WriteToBase64 = 0;
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
  unsigned char* data = result->GetPointer(0);
  int length = result->GetNumberOfTuples() * result->GetNumberOfComponents();

  char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                           'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                           'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                           'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                           'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                           'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                           'w', 'x', 'y', 'z', '0', '1', '2', '3',
                           '4', '5', '6', '7', '8', '9', '+', '/'};
  int mod_table[] = {0, 2, 1};

  size_t output_length = (size_t) (4.0 * ceil((double) length / 3.0));

  char* encoded_data = new char[output_length];

  for (int i = 0, j = 0; i < length;) {
      uint32_t octet_a = i < length ? data[i++] : 0;
      uint32_t octet_b = i < length ? data[i++] : 0;
      uint32_t octet_c = i < length ? data[i++] : 0;

      uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

      encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
      encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
      encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
      encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
  }

  for (int i = 0; i < mod_table[length % 3]; i++) {
      encoded_data[output_length - 1 - i] = '=';
  }
  
  this->SetBase64Image(encoded_data);

  //NSString *base64String = [NSString stringWithFormat:@"data:image/png;base64,%@",[self base64forData:pixelData]];

  this->SetWriteToMemory(previousWriteToMemory);
} 

void vtkvmtkPNGWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkPNGWriter::PrintSelf(os,indent);
}
