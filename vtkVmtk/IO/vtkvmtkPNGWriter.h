/*=========================================================================
                                                                                                                                    
Program:   VMTK
Module:    $RCSfile: vtkvmtkPNGWriter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:47 $
Version:   $Revision: 1.2 $
                                                                                                                                    
  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
                                                                                                                                    
=========================================================================*/
// .NAME vtkvmtkPNGWriter - writes .png files
// .SECTION Description
//
// .SECTION See Also

#ifndef __vtkvmtkPNGWriter_h
#define __vtkvmtkPNGWriter_h

#include "vtkvmtkWin32Header.h"
#include "vtkPNGWriter.h"

class VTK_VMTK_IO_EXPORT vtkvmtkPNGWriter : public vtkPNGWriter
{
public:
  static vtkvmtkPNGWriter *New();
  vtkTypeMacro(vtkvmtkPNGWriter,vtkPNGWriter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkGetStringMacro(Base64Image);
  vtkSetStringMacro(Base64Image);

  vtkSetMacro(WriteToBase64,int);
  vtkGetMacro(WriteToBase64,int);
  vtkBooleanMacro(WriteToBase64,int);

  vtkSetMacro(FlipImage,int);
  vtkGetMacro(FlipImage,int);
  vtkBooleanMacro(FlipImage,int);

  virtual void Write() override;

protected:
  vtkvmtkPNGWriter();
  ~vtkvmtkPNGWriter();

  char* Base64Image;
  int WriteToBase64;
  int FlipImage;

private:
  vtkvmtkPNGWriter(const vtkvmtkPNGWriter&);  // Not implemented.
  void operator=(const vtkvmtkPNGWriter&);  // Not implemented.
};

#endif
