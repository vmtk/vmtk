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
 * @class   vtkvmtkDolfinWriter
 * @brief   Write mesh data in Dolfin file formats.
 * @ingroup IO
 *
 * vtkvmtkDolfinWriter writes Dolfin files - www.fenics.org.
 * Compatible file formats include:
 *     * Binary (.bin)
 *     * RAW    (.raw)
 *     * SVG    (.svg)
 *     * XD3    (.xd3)
 *     * XML    (.xml)
 *     * XYZ    (.xyz)
 *     * VTK    (.pvd)
 *
 */

#ifndef __vtkvmtkDolfinWriter_h
#define __vtkvmtkDolfinWriter_h

#include "vtkvmtkWin32Header.h"
#include "vtkUnstructuredGridWriter.h"

class vtkCell;
class vtkIdList;

class VTK_VMTK_IO_EXPORT vtkvmtkDolfinWriter : public vtkUnstructuredGridWriter
{
public:
  static vtkvmtkDolfinWriter *New();
  vtkTypeMacro(vtkvmtkDolfinWriter,vtkUnstructuredGridWriter);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Set/get the name of the input cell data array holding integer cell entity ids. When set, the
   * values of this array (for the triangle cells found on tetrahedron facets) are written out as the
   * Dolfin facet subdomain markers (mesh_value_collection of dimension 2); if StoreCellMarkers is
   * also on, the same array is used to write per-tetrahedron subdomain markers (dimension 3) as well.
   * If left NULL (default), no subdomain markers section is written.
   * Commonly named "CellEntityIds".
   */
  vtkSetStringMacro(BoundaryDataArrayName);
  vtkGetStringMacro(BoundaryDataArrayName);
  ///@}

  ///@{
  /**
   * Set/get a constant offset added to every facet subdomain marker value taken from
   * BoundaryDataArrayName before it is written to the output file. Default: 0.
   */
  vtkSetMacro(BoundaryDataIdOffset,int);
  vtkGetMacro(BoundaryDataIdOffset,int);
  ///@}

  ///@{
  /**
   * Set/get whether, in addition to facet (triangle) subdomain markers, per-tetrahedron cell
   * subdomain markers are also written to the output file, taken from the same BoundaryDataArrayName
   * cell data array (without BoundaryDataIdOffset applied). Default: 0 (off).
   */
  vtkSetMacro(StoreCellMarkers,int);
  vtkGetMacro(StoreCellMarkers,int);
  ///@}

protected:
  vtkvmtkDolfinWriter();
  ~vtkvmtkDolfinWriter();

  void WriteData() override;

  char* BoundaryDataArrayName;
  int BoundaryDataIdOffset;
  int StoreCellMarkers;

private:
  vtkvmtkDolfinWriter(const vtkvmtkDolfinWriter&);  // Not implemented.
  void operator=(const vtkvmtkDolfinWriter&);  // Not implemented.
};

#endif
