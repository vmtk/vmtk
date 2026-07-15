/*=========================================================================
                                                                                                                                    
Program:   VMTK
                                                                                                                                    
  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
                                                                                                                                    
=========================================================================*/
/**
 * @class   vtkvmtkPNGWriter
 * @brief   Writes .png files.
 * @ingroup IO
 *
 *
 *
 */

#ifndef __vtkvmtkPNGWriter_h
#define __vtkvmtkPNGWriter_h

#include "vtkvmtkWin32Header.h"
#include "vtkPNGWriter.h"

class VTK_VMTK_IO_EXPORT vtkvmtkPNGWriter : public vtkPNGWriter
{
public:
  static vtkvmtkPNGWriter *New();
  vtkTypeMacro(vtkvmtkPNGWriter,vtkPNGWriter);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Get/set the base64-encoded PNG image data. This is populated by Write() when WriteToBase64 is on
   * (the Set accessor is used internally for that purpose); read it after Write() to retrieve the
   * encoded image without going through disk. Default: NULL.
   */
  vtkGetStringMacro(Base64Image);
  vtkSetStringMacro(Base64Image);
  ///@}

  ///@{
  /**
   * Set/get whether Write() encodes the written image as a base64 string (retrievable through
   * GetBase64Image) instead of the default vtkPNGWriter behavior of writing only to disk/memory.
   * Default: off.
   */
  vtkSetMacro(WriteToBase64,int);
  vtkGetMacro(WriteToBase64,int);
  vtkBooleanMacro(WriteToBase64,int);
  ///@}

  ///@{
  /**
   * Set/get whether the image is flipped vertically (rows reversed) before being written/encoded.
   * Default: off.
   */
  vtkSetMacro(FlipImage,int);
  vtkGetMacro(FlipImage,int);
  vtkBooleanMacro(FlipImage,int);
  ///@}

  /**
   * Write the input image to PNG. If WriteToBase64 is off, this simply forwards to the standard
   * vtkPNGWriter behavior (writing to FileName/FilePattern or to memory). If WriteToBase64 is on, the
   * image is instead (optionally flipped, see FlipImage, and) written to an in-memory PNG buffer which
   * is then base64-encoded and stored in Base64Image.
   */
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
