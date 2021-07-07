/*=========================================================================
                                                                                                                                    
Program:   VMTK
Module:    $RCSfile: vtkvmtkFDNEUTReader.h,v $
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
// .NAME vtkvmtkFDNEUTReader - Reads FDNEUT Fidap files.
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
  vtkTypeMacro(vtkvmtkFDNEUTReader,vtkUnstructuredGridReader);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkvmtkFDNEUTReader *New();

  vtkSetStringMacro(SingleCellDataEntityArrayName);
  vtkGetStringMacro(SingleCellDataEntityArrayName);

  vtkSetMacro(GhostNodes,int);
  vtkGetMacro(GhostNodes,int);
  vtkBooleanMacro(GhostNodes,int);

  vtkSetMacro(VolumeElementsOnly,int);
  vtkGetMacro(VolumeElementsOnly,int);
  vtkBooleanMacro(VolumeElementsOnly,int);

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

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  static void OneToZeroOffset(int npts, int* pts)
  { for (int i=0; i<npts; i++) --pts[i]; }

  char* SingleCellDataEntityArrayName;

  int GhostNodes;
  int VolumeElementsOnly;

  private:
  vtkvmtkFDNEUTReader(const vtkvmtkFDNEUTReader&);  // Not implemented.
  void operator=(const vtkvmtkFDNEUTReader&);  // Not implemented.
};

#endif
