#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtklineartoquadratic.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.7 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
from vmtk import vtkvmtk
import sys

from vmtk import pypes


class vmtkLinearToQuadratic(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.Surface = None
        self.Mode = 'volume'
        self.SubdivisionMethod = 'linear'
        self.NumberOfNodesHexahedra = 27
        self.UseBiquadraticWedge = True
        self.CapSurface = False
        self.CellEntityIdsArrayName = None
        self.ProjectedCellEntityId = 1
        self.JacobianRelaxation = 0
        self.NegativeJacobianTolerance = 0.0
        self.QuadratureOrder = 10

        self.SetScriptName('vmtklineartoquadratic')
        self.SetScriptDoc('convert the elements of a mesh from linear to quadratic')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['Surface','r','vtkPolyData',1,'','the reference surface to project nodes onto','vmtksurfacereader'],
            ['Mode','mode','str',1,'["volume","surface"]','kind of elements to work with'],
            ['UseBiquadraticWedge','biquadraticwedge','bool',1,'','if on, convert linear wedges to 18-noded biquadratic quadratic wedges, otherwise use 15-noded quadratic wedges'],
            ['CapSurface','capsurface','bool',1,'','if on, cap the reference surface before projecting'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'','name of the array where entity ids relative to cells are stored'],
            ['JacobianRelaxation','jacobianrelaxation','bool',1,'','if on, relax projected nodes until Jacobians are all positive'],
            ['ProjectedCellEntityId','projectedid','int',1,'','id of the entity that is to be projected onto the reference surface'],
            ['QuadratureOrder','quadratureorder','int',1,'','quadrature order for checking negative Jacobians'],
            ['NegativeJacobianTolerance','jacobiantolerance','float',1,'','tolerance for the evaluation of negative Jacobians'],
            ['SubdivisionMethod','subdivisionmethod','str',1,'["linear","butterfly"]','subdivision method for surface elements'],
            ['NumberOfNodesHexahedra','hexahedranodes','int',1,'','Output hexahedral element type by # of nodes: 20 or 27 (see vtkQuadraticHexahedron and vtkTriQuadraticHexahedron classes in VTK documentation)']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']])

    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: No input mesh.')

        linearToQuadraticFilter = None

        if self.Mode == 'volume':
            surface = self.Surface
            if self.Surface and self.CapSurface:
                capper = vtkvmtk.vtkvmtkSimpleCapPolyData()
                capper.SetInputData(self.Surface)
                capper.SetCellEntityIdsArrayName('foo')
                capper.Update()
                surface = capper.GetOutput()
            linearToQuadraticFilter = vtkvmtk.vtkvmtkLinearToQuadraticMeshFilter()
            linearToQuadraticFilter.SetReferenceSurface(surface)
            linearToQuadraticFilter.SetUseBiquadraticWedge(self.UseBiquadraticWedge)
            linearToQuadraticFilter.SetCellEntityIdsArrayName(self.CellEntityIdsArrayName)
            linearToQuadraticFilter.SetJacobianRelaxation(self.JacobianRelaxation)
            linearToQuadraticFilter.SetProjectedCellEntityId(self.ProjectedCellEntityId)
            linearToQuadraticFilter.SetQuadratureOrder(self.QuadratureOrder)
            linearToQuadraticFilter.SetNegativeJacobianTolerance(self.NegativeJacobianTolerance)
            linearToQuadraticFilter.SetNumberOfNodesHexahedra(self.NumberOfNodesHexahedra)
        elif self.Mode == 'surface':
            linearToQuadraticFilter = vtkvmtk.vtkvmtkLinearToQuadraticSurfaceMeshFilter()
            if self.SubdivisionMethod == 'linear':
                linearToQuadraticFilter.SetSubdivisionMethodToLinear()
            elif self.SubdivisionMethod == 'butterfly':
                linearToQuadraticFilter.SetSubdivisionMethodToButterfly()
            else:
                self.PrintError('Unsupported subdivision method.')
        else:
            self.PrintError('Unsupported mode.')

        linearToQuadraticFilter.SetInputData(self.Mesh)
        linearToQuadraticFilter.Update()

        self.Mesh = linearToQuadraticFilter.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
