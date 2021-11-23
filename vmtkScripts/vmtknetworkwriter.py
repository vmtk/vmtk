#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtknetworkwriter.py,v $
## Language:  Python
## Date:      $Date: 2006/07/27 08:27:40 $
## Version:   $Revision: 1.13 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

##   Developed with support from the EC FP7/2007-2013: ARCH, Project n. 224390

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes


class vmtkNetworkWriter(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Format = ''
        self.GuessFormat = 1
        self.OutputFileName = ''
        self.Network = None
        self.Input = None
        self.InputUnits = 'mm'

        self.RadiusArrayName = 'Radius'
        self.LabelsArrayName = 'Labels'

        self.SetScriptName('vmtknetworkwriter')
        self.SetScriptDoc('write network to disk')
        self.SetInputMembers([
            ['Network','i','vtkPolyData',1,'','the input network','vmtksurfacereader'],
            ['Format','f','str',1,'["vtkxml","vtk","arch"]','file format'],
            ['InputUnits','u','str',1,'["mm","cm","m"]','units in which the input network is represented'],
            ['GuessFormat','guessformat','bool',1,'','guess file format from extension'],
            ['RadiusArrayName','radiusarray','str',1,'','name of the point data array where radius is stored (arch format)'],
            ['LabelsArrayName','labelsarray','str',1,'','name of the cell data array where labels are stored (arch format)'],
            ['OutputFileName','ofile','str',1,'','output file name'],
            ['OutputFileName','o','str',1,'','output file name (deprecated: use -ofile)']
            ])
        self.SetOutputMembers([])

    def WriteVTKNetworkFile(self):
        if self.OutputFileName == '':
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing VTK network file.')
        writer = vtk.vtkPolyDataWriter()
        writer.SetInputData(self.Network)
        writer.SetFileName(self.OutputFileName)
        writer.Write()

    def WriteVTKXMLNetworkFile(self):
        if self.OutputFileName == '':
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing VTK XML network file.')
        writer = vtk.vtkXMLPolyDataWriter()
        writer.SetInputData(self.Network)
        writer.SetFileName(self.OutputFileName)
        #writer.SetDataModeToAscii()
        writer.Write()

    def WriteARCHNetworkFile(self):
        if self.OutputFileName == '':
            self.PrintError('Error: no OutputFileName.')
        self.PrintLog('Writing ARCH network file.')

        factor = 1.0
        if self.InputUnits == 'cm':
          factor = 0.01
        elif self.InputUnits == 'mm':
          factor = 0.001

        radiusArray = None
        if self.RadiusArrayName:
            radiusArray = self.Network.GetPointData().GetArray(self.RadiusArrayName)

        labelsArray = None
        if self.LabelsArrayName != '':
            labelsArray = vtk.vtkStringArray.SafeDownCast(self.Network.GetCellData().GetAbstractArray(self.LabelsArrayName))

        from xml.dom import minidom
        xmlDocument = minidom.Document()
        xmlNetworkGraph = xmlDocument.appendChild(xmlDocument.createElement('NetworkGraph'))
        xmlNetworkGraph.setAttribute('id','1')
        xmlNetworkGraph.setAttribute('version','3.0')
        xmlNetworkGraph.setAttribute('xmlns:xsi','http://www.w3.org/2001/XMLSchema-instance')
        xmlNetworkGraph.setAttribute('xsi:noNamespaceSchemaLocation','vascular_network_v2.3.xsd')
        xmlCase = xmlNetworkGraph.appendChild(xmlDocument.createElement('case'))
        xmlPatientId = xmlCase.appendChild(xmlDocument.createElement('patient_id'))
        xmlPatientId.appendChild(xmlDocument.createTextNode('00000'))
        xmlVisit = xmlCase.appendChild(xmlDocument.createElement('visit'))
        xmlVisit.appendChild(xmlDocument.createTextNode('other'))

        nodeMap = {}
        edgeCellIds = []
        edges = []
        for i in range(self.Network.GetNumberOfCells()):
            cell = self.Network.GetCell(i)
            if not (cell.GetCellType() in (3,4)):
                continue
            pointId0 = cell.GetPointId(0)
            pointId1 = cell.GetPointId(cell.GetNumberOfPoints()-1)
            if pointId0 not in nodeMap:
                nodeId = len(nodeMap)
                nodeMap[pointId0] = nodeId
            if pointId1 not in nodeMap:
                nodeId = len(nodeMap)
                nodeMap[pointId1] = nodeId
            edgeId = len(edges)
            edgeCellIds.append(edgeId)
            edges.append((nodeMap[pointId0],nodeMap[pointId1]))
        nodes = list(nodeMap.values())[:]
        nodes.sort()

        xmlNodes = xmlNetworkGraph.appendChild(xmlDocument.createElement('nodes'))

        for node in nodes:
            xmlNode = xmlNodes.appendChild(xmlDocument.createElement('node'))
            xmlNode.setAttribute('id','%d' % node)
            #xmlNodeClassification = xmlNode.appendChild(xmlDocument.createElement('node_classification'))

        xmlEdges = xmlNetworkGraph.appendChild(xmlDocument.createElement('edges'))

        for i in range(len(edges)):
            edge = edges[i]
            xmlEdge = xmlEdges.appendChild(xmlDocument.createElement('edge'))
            xmlEdge.setAttribute('id','%d' % i)
            xmlEdge.setAttribute('node1_id','%d' % edge[0])
            xmlEdge.setAttribute('node2_id','%d' % edge[1])
            if labelsArray:
               label = labelsArray.GetValue(edgeCellIds[i])
               if 'aorta' in label or 'artery' in label or 'art' in label or 'a.' in label:
                   xmlEdge.setAttribute('side','arterial')
               if 'vena' in label or 'vein' in label or 'v.' in label:
                   xmlEdge.setAttribute('side','venous')
               xmlEdge.setAttribute('name',label)
            else:
               xmlEdge.setAttribute('side','arterial')
               xmlEdge.setAttribute('name','edge%d' % i)

            #xmlEdgeClassification = xmlEdge.appendChild(xmlDocument.createElement('edge_classification'))
            #if labelsArray:
            #    label = labelsArray.GetValue(edgeCellIds[i])
            #    if 'aorta' in label or 'artery' in label or 'art' in label or 'a.' in label:
            #        xmlEdgeClassification.setAttribute('side','arterial')
            #    if 'vena' in label or 'vein' in label or 'v.' in label:
            #        xmlEdgeClassification.setAttribute('side','venous')
            #    xmlEdgeClassification.appendChild(xmlDocument.createTextNode(label))
            xmlGeometry = xmlEdge.appendChild(xmlDocument.createElement('geometry'))

            length = 0.0
            cell = self.Network.GetCell(edgeCellIds[i])
            cellPoints = cell.GetPoints()
            prevPoint = cellPoints.GetPoint(0)
            abscissas = [0.0]
            coordinates = [[factor*el for el in prevPoint]]
            for j in range(1,cell.GetNumberOfPoints()):
                point = cellPoints.GetPoint(j)
                length += factor * vtk.vtkMath.Distance2BetweenPoints(prevPoint,point)**0.5
                abscissas.append(length)
                coordinates.append([factor*el for el in point])
                prevPoint = point

            xmlLength = xmlGeometry.appendChild(xmlDocument.createElement('length'))
            xmlLength.setAttribute('unit','m')
            xmlScalar = xmlLength.appendChild(xmlDocument.createElement('scalar'))
            xmlScalar.appendChild(xmlDocument.createTextNode('%f' % length))
            xmlCoordinatesArray = xmlGeometry.appendChild(xmlDocument.createElement('coordinates_array'))
            for j in range(len(abscissas)):
                xmlCoordinates = xmlCoordinatesArray.appendChild(xmlDocument.createElement('coordinates'))
                #TODO: check for length == 0
                xmlCoordinates.setAttribute('s','%f' % (abscissas[j]/length))
                xmlCoordinates.setAttribute('x','%f' % coordinates[j][0])
                xmlCoordinates.setAttribute('y','%f' % coordinates[j][1])
                xmlCoordinates.setAttribute('z','%f' % coordinates[j][2])

            xmlProperties = xmlEdge.appendChild(xmlDocument.createElement('properties'))

            if radiusArray:
                xmlRadiusArray = xmlProperties.appendChild(xmlDocument.createElement('radius_array'))
                xmlRadiusArray.setAttribute('unit','m')
                for j in range(cell.GetNumberOfPoints()):
                    pointId = cell.GetPointId(j)
                    radius = factor * radiusArray.GetTuple1(pointId)
                    xmlValue = xmlRadiusArray.appendChild(xmlDocument.createElement('value'))
                    xmlValue.setAttribute('s','%f' % (abscissas[j]/length))
                    xmlScalar = xmlValue.appendChild(xmlDocument.createElement('scalar'))
                    xmlScalar.appendChild(xmlDocument.createTextNode('%f' % radius))

        #xmlTransformations = xmlNetworkGraph.appendChild(xmlDocument.createElement('transformations'))

        xmlFile = open(self.OutputFileName,'w')
        xmlFile.write(xmlDocument.toprettyxml())
        xmlFile.close()

    def Execute(self):

        if self.Network == None:
            if self.Input == None:
                self.PrintError('Error: no Network.')
            self.Network = self.Input

        extensionFormats = {'vtp':'vtkxml',
                            'vtkxml':'vtkxml',
                            'xml':'arch'}

        if self.OutputFileName == 'BROWSER':
            import tkinter.filedialog
            import os.path
            initialDir = pypes.pypeScript.lastVisitedPath
            self.OutputFileName = tkinter.filedialog.asksaveasfilename(title="Output network",initialdir=initialDir)
            pypes.pypeScript.lastVisitedPath = os.path.dirname(self.OutputFileName)
            if not self.OutputFileName:
                self.PrintError('Error: no OutputFileName.')

        if self.GuessFormat and self.OutputFileName and not self.Format:
            import os.path
            extension = os.path.splitext(self.OutputFileName)[1]
            if extension:
                extension = extension[1:]
                if extension in list(extensionFormats.keys()):
                    self.Format = extensionFormats[extension]

        if (self.Format == 'vtk'):
            self.WriteVTKNetworkFile()
        elif (self.Format == 'vtkxml'):
            self.WriteVTKXMLNetworkFile()
        elif (self.Format == 'arch'):
            self.WriteARCHNetworkFile()
        else:
            self.PrintError('Error: unsupported format '+ self.Format + '.')


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
