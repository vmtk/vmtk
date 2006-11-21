/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkDICOMImageReader.h,v $
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
// .NAME vtkvmtkDICOMImageReader - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkDICOMImageReader_h
#define __vtkvmtkDICOMImageReader_h

#include "vtkDICOMImageReader.h"
#include "vtkvmtkWin32Header.h"
#if 0
#include "DICOMParser.h"
#include "DICOMAppHelper.h"

//BTX
class VTK_VMTK_IO_EXPORT vtkvmtkDICOMAppHelper : public DICOMAppHelper
{
  public:

  float *GetImageOrientationPatient()
    {
      return this->ImageOrientationPatient;
    }


  virtual void ImageOrientationPatientCallback(DICOMParser *parser,
                                               doublebyte group,
                                               doublebyte element,
                                               DICOMParser::VRTypes type,
                                               unsigned char* val,
                                               quadbyte len);

  protected:
  
  float ImageOrientationPatient[6];
};

class VTK_VMTK_IO_EXPORT vtkvmtkDICOMParser : public DICOMParser
{
  public:
};
//ETX
#endif
class VTK_VMTK_IO_EXPORT vtkvmtkDICOMImageReader : public vtkDICOMImageReader
{
  public:
  vtkTypeRevisionMacro(vtkvmtkDICOMImageReader,vtkDICOMImageReader);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkvmtkDICOMImageReader *New();

  vtkSetMacro(AutoOrientImage,int);
  vtkGetMacro(AutoOrientImage,int);
  vtkBooleanMacro(AutoOrientImage,int);

  vtkGetStringMacro(OrientationStringX);
  vtkGetStringMacro(OrientationStringY);
  vtkGetStringMacro(OrientationStringZ);

  protected:
  vtkvmtkDICOMImageReader();
  ~vtkvmtkDICOMImageReader();

//  virtual int RequestInformation(vtkInformation* request,
//                                 vtkInformationVector** inputVector,
//                                 vtkInformationVector* outputVector);
  virtual void ExecuteInformation();
  virtual void ExecuteData(vtkDataObject *out);

  void ComputeOutputVoxelSpacing();
  void GenerateOrientationString(float direction[3], char* orientationString);
  void OrientImageData();

  int AutoOrientImage;
  char* OrientationStringX;
  char* OrientationStringY;
  char* OrientationStringZ;

  private:
  vtkvmtkDICOMImageReader(const vtkvmtkDICOMImageReader&) {};
  void operator=(const vtkvmtkDICOMImageReader&) {};
};

#endif
