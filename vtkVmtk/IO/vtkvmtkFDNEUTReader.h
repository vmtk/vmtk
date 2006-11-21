/*=========================================================================
                                                                                                                                    
Program:   VMTK
Module:    $RCSfile: vtkvmtkFDNEUTReader.h,v $
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

// .NAME vtkvmtkFDNEUTReader -
// .SECTION Description
// vtkvmtkFDNEUTReader reads unstructured grid data from Fidap FDNEUT format

// .SECTION See Also
// vtkvmtkFDNEUTWriter

#ifndef __vtkvmtkFDNEUTReader_h
#define __vtkvmtkFDNEUTReader_h

#include "vtkvmtkWin32Header.h"
#include "vtkUnstructuredGridReader.h"

class VTK_VMTK_IO_EXPORT vtkvmtkFDNEUTReader : public vtkUnstructuredGridReader
{
  public:
  vtkTypeRevisionMacro(vtkvmtkFDNEUTReader,vtkUnstructuredGridReader);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkvmtkFDNEUTReader *New();

  vtkSetStringMacro(EntityName);
  vtkGetStringMacro(EntityName);

  vtkSetMacro(Scale,double);
  vtkGetMacro(Scale,double);

  vtkSetMacro(GhostNodes,int);
  vtkGetMacro(GhostNodes,int);
  vtkBooleanMacro(GhostNodes,int);

  vtkSetMacro(VolumeElementsOnly,int);
  vtkGetMacro(VolumeElementsOnly,int);
  vtkBooleanMacro(VolumeElementsOnly,int);

  vtkSetMacro(ReadEntityInformation,int);
  vtkGetMacro(ReadEntityInformation,int);
  vtkBooleanMacro(ReadEntityInformation,int);

//   vtkSetMacro(EntityStorageMode,int);
//   vtkGetMacro(EntityStorageMode,int);
//   void SetEntityStorageModeToPointData()
//     { this->SetEntityStorageMode(VTKVMTK_POINT_DATA_ENTITY_STORAGE_MODE); }
//   void SetEntityStorageModeToCellData()
//     { this->SetEntityStorageMode(VTKVMTK_CELL_DATA_ENTITY_STORAGE_MODE); }

//   //BTX
//   enum
//   {
//     VTKVMTK_POINT_DATA_ENTITY_STORAGE_MODE,
//     VTKVMTK_CELL_DATA_ENTITY_STORAGE_MODE
//   };
//   //ETX

  //BTX
  enum
  {
    EDGE = 0,
    QUADRILATERAL,
    TRIANGLE,
    BRICK,
    WEDGE,
    TETRAHEDRON
  };
  //ETX

  protected:
  vtkvmtkFDNEUTReader();
  ~vtkvmtkFDNEUTReader();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  static void OneToZeroOffset(int npts, int* pts)
  { for (int i=0; i<npts; i++) --pts[i]; }

  char* EntityName;
  double Scale;

  int GhostNodes;
  int VolumeElementsOnly;

  int ReadEntityInformation;
//   int EntityStorageMode;

  private:
  vtkvmtkFDNEUTReader(const vtkvmtkFDNEUTReader&);  // Not implemented.
  void operator=(const vtkvmtkFDNEUTReader&);  // Not implemented.
};

#endif
