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
 * @class   vtkvmtkFDNEUTReader
 * @brief   Reads FDNEUT Fidap files.
 * @ingroup IO
 *
 * vtkvmtkFDNEUTReader reads unstructured grid data from Fidap FDNEUT format
 *
 * @sa
 * vtkvmtkFDNEUTWriter
 */

#ifndef __vtkvmtkFDNEUTReader_h
#define __vtkvmtkFDNEUTReader_h

#include "vtkvmtkWin32Header.h"
#include "vtkUnstructuredGridReader.h"

// VTK_FILEPATH hint was introduced in VTK_VERSION_CHECK(9,1,0)
// (https://github.com/Kitware/VTK/commit/c30ddf9a6caedd65ae316080b0efd1833983844e)
#ifndef VTK_FILEPATH
#define VTK_FILEPATH
#endif

class VTK_VMTK_IO_EXPORT vtkvmtkFDNEUTReader : public vtkUnstructuredGridReader
{
  public:
  vtkTypeMacro(vtkvmtkFDNEUTReader,vtkUnstructuredGridReader);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkFDNEUTReader *New();

  ///@{
  /**
   * Set/get the name of the cell data array where a per-element-group entity id is stored. Each
   * FDNEUT "ELEMENT GROUPS" block encountered while reading is assigned an increasing integer id
   * (starting at 0, in file order), and every cell belonging to that group is tagged with it in this
   * array.
   * Commonly named "CellEntityIds".
   */
  vtkSetStringMacro(SingleCellDataEntityArrayName);
  vtkGetStringMacro(SingleCellDataEntityArrayName);
  ///@}

  ///@{
  /**
   * Set/get whether the extra "ghost" (mid-face/mid-volume) nodes present in some higher-order FDNEUT
   * element types (e.g. the 9th node of a biquadratic quad, or the 27th node of a triquadratic
   * hexahedron) are kept when building the output cells. When off, those extra nodes are discarded
   * (read from the file but not stored) and the corresponding VTK quadratic (not biquadratic) cell
   * type is produced instead. Default: on.
   */
  vtkSetMacro(GhostNodes,int);
  vtkGetMacro(GhostNodes,int);
  vtkBooleanMacro(GhostNodes,int);
  ///@}

  ///@{
  /**
   * Set/get whether only volume elements (bricks, wedges, tetrahedra) are read, skipping surface
   * elements (quadrilaterals, triangles) found in the file. Default: off (both volume and surface
   * elements are read).
   */
  vtkSetMacro(VolumeElementsOnly,int);
  vtkGetMacro(VolumeElementsOnly,int);
  vtkBooleanMacro(VolumeElementsOnly,int);
  ///@}

  int ReadMeshSimple(VTK_FILEPATH const std::string& fname, vtkDataObject* output) override;

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
