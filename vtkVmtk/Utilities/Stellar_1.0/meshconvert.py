#!/usr/bin/env python
# encoding: utf-8
"""
meshconvert.py

A script to convert between tetrahedral mesh formats.

Created by Bryan Klingner (stellar.b@overt.org) on 2006-12-07.
"""

import sys
import getopt
import os

help_message = \
'''
meshconvert.py - a script to convert between tetrahedral mesh formats.

Usage:
   meshconvert.py [-s scale] input_file output_file
   
       input_file    the input tetrahedral (or trianglar surface) mesh file. 
                     This file must be in one of the following formats:
                         .node  - Jonathan Shewchuk's node file format.
                                  .node files must be accompanied by a
                                  .ele file that contains tetrahedra.
                         .mesh  - NETGEN's tetrahedral mesh format.
                         .vmesh - GRUMMP's tetrahedral mesh format.
                         .tet   - AIM@SHAPE repository's tetrahedral mesh format.
                         .off   - A common surface mesh format. If the input file
                                  is in .off format, only boundary triangles will
                                  be read and available for output.
                         .surf  - NETGEN's triangle surface mesh format. If the
                                  input file is in .surf format, only boundary
                                  triangles will be read and available for output.
                                  
       output_file   the output tetrahedral (or triangular surface) mesh file.
                     This file must be in one of the following formats:
                         .node  - Jonathan Shewchuk's mesh file format.
                                  An additional .ele file will also be 
                                  output that contains tetrahedra.
                         .mesh  - NETGEN's tetrahedral mesh format.
                         .obj   - A common surface mesh format. If the output
                                  file is in .obj format, only boundary triangles
                                  will be written to it.
                         .off   - A common surface mesh format. If the output
                                  file is in .off format, only boundary triangles
                                  will be written to it.
                         .surf  - NETGEN's tetrahedral mesh format. If the output
                                  file is in .surf format, only boundary triangles
                                  will be written to it.
       
       -s scale      optional vertex scale argument. All vertices geometric positions
                     will be multiplied by scale in the output file.
                     
       NOTE: As part of the conversion process, all tetrahedra will be adjusted to have
             consistent, right-handed orientation: That is, for a tetrahedron with vertices
             ordered (1, 2, 3, 4), the vertices 2, 3, 4 occur in counterclockwise order as 
             seen from vertex 1. If you curl the fingers of your right hand to follow the 
             vertices 2, 3, 4, then your thumb points toward vertex 1.
'''
class Usage(Exception):
    def __init__(self, msg):
        self.msg = msg


def main(argv=None):
    # functions for reading 
    readDict = {}
    readDict['.node'] = readNodeEle
    readDict['.mesh'] = readMesh
    readDict['.vmesh'] = readVmesh
    readDict['.tet'] = readTet
    readDict['.surf'] = readSurfTets
    readDict['.off'] = ReadOFFTets
    
    # functions for writing
    writeDict = {}
    writeDict['.node'] = writeNodeEle
    writeDict['.mesh'] = writeMesh
    writeDict['.obj'] = writeOBJTets
    writeDict['.off'] = writeOFFTets
    writeDict['.surf'] = writeSurfTets
    
    if argv is None:
        argv = sys.argv
        
    doscale = False
    
    # if they invoke with arguments, parse them
    if len(argv) > 1:
        try:
            try:
                opts, args = getopt.getopt(argv[1:], "hs:", ["help",])
            except getopt.error, msg:
                raise Usage(msg)
            
            # option processing
            for option, value in opts:
                if option == "-s":
                    doscale = True
                    scale = float(value)
                if option in ("-h", "--help"):
                    print help_message
                    return 1
        except Usage, err:
            print >> sys.stderr, sys.argv[0].split("/")[-1] + ": " + str(err.msg)
            print >> sys.stderr, "\t for help use --help"
            return 2
    else:
        print help_message
        return 1
    
    if len(argv) < 3:
        print "Not enough arguments. For help, use --help."
    
    # determine the input and output formats, and check that they make sense
    inFileName = argv[-2]
    outFileName = argv[-1]
    inFileNameBase, inType = os.path.splitext(inFileName)
    outFileNameBase, outType = os.path.splitext(outFileName)
    
    if inType not in readDict.keys():
        print "Don't know how to read input format '%s'; invoke with --help for a list of supported formats." % (inType)
        return 2
    if outType not in writeDict.keys():
        print "Don't know how to write output format '%s'; invoke with --help for a list of supported formats." % (outType)
        return 2
    
    # read the input mesh
    points, tets, boundFaces = readDict[inType](inFileNameBase)
    if doscale:
        print "Scaling vertices by %g", scale
        points = [vscale(scale, point) for point in points]
    # write the output mesh
    writeDict[outType](points, tets, boundFaces, outFileNameBase)


####
# Functions to READ tet mesh formats
####
def readTet(meshFileName):
    """Read .tet format... I don't actually remember who uses this"""
    
    # append .mesh to file stem
    meshFileName += '.tet'

    # open input .tet file
    infile = open(meshFileName)

    # fetch the number of points
    numpoints = int(infile.readline().split()[0])
    # fetch the number of tets
    numtets = int(infile.readline().split()[0])

    points = []
    # read in all the points
    for iter in range(0,numpoints):
        points.append(map(float,infile.readline().strip().split()))

    tets = []
    # read in the tets
    for iter in range(0,numtets):
        tets.append(vaddscalar(1,map(int,infile.readline().strip().split()[1:])))

    # correct orientation of tets
    for tetNum, tet in enumerate(tets):
        a = points[tet[0]-1]
        b = points[tet[1]-1]
        c = points[tet[2]-1]
        d = points[tet[3]-1]
        # if tet is negative orientation, flip two verts
        if orient3d(a,b,c,d) == 0:
            print "WHOA! input zero-volume tet...\n"
        if orient3d(a,b,c,d) < 0:
            temp = tet[0]
            tets[tetNum][0] = tet[1]
            tets[tetNum][1] = temp
    
    # this function doesn't attempt to recover boundary faces
    boundFaces = []

    infile.close()

    return points, tets, boundFaces

def readMesh(meshFileName):
    """Read .mesh files, as output by NETGEN"""
    
    # append .mesh to file stem
    meshFileName += '.mesh'

    # open input .mesh file
    infile = open(meshFileName)

    # fetch the number of points
    numpoints = int(infile.readline())

    points = []
    # read in all the points
    for iter in range(0,numpoints):
        points.append(map(float,infile.readline().strip().split()))

    # fetch the number of tets
    numtets = int(infile.readline())

    tets = []
    # read in the tets
    for iter in range(0,numtets):
        tets.append(map(int,infile.readline().strip().split()[1:]))

    # correct orientation of tets
    for tetNum, tet in enumerate(tets):
        a = points[tet[0]-1]
        b = points[tet[1]-1]
        c = points[tet[2]-1]
        d = points[tet[3]-1]
        # if tet is negative orientation, flip two verts
        if orient3d(a,b,c,d) == 0:
            print "WHOA! input zero-volume tet...\n"
            sys.exit(1)
        if orient3d(a,b,c,d) < 0:
            temp = tet[0]
            tets[tetNum][0] = tet[1]
            tets[tetNum][1] = temp

    # fetch the number of boundary faces
    numfaces = int(infile.readline())

    boundFaces = []
    # read in the boundary faces
    for iter in range(0,numfaces):
        boundFaces.append(map(int,infile.readline().strip().split()[1:]))

    infile.close()

    # fix tets to reference points starting at 0
    for index, tet in enumerate(tets):
        tets[index] = vsubscalar(1,tet)

    return points, tets, boundFaces

def readVmesh(meshFileName):
    """Read in .vmesh file... again, I can't recall who uses this format"""
    
    # append .vmesh to file stem
    meshFileName += '.vmesh'

    # open input .vmesh file
    infile = open(meshFileName)

    # first line: #tets #faces #boundFaces #verts
    firstLine = map(int,infile.readline().strip().split())
    numTets = firstLine[0]
    numFaces = firstLine[1]
    numBoundFaces = firstLine[2]
    numPoints = firstLine[3]

    points = []
    # read in all the points
    for iter in range(0,numPoints):
        points.append(map(float,infile.readline().strip().split()))

    faces = []
    face2tet = []
    for iter in range(0,numFaces):
        line = map(int,infile.readline().strip().split())
        face2tet.append(line[0:2])
        faces.append(line[2:])

    boundFaces = []
    for iter in range(0,numBoundFaces):
        boundFaces.append(map(int,infile.readline().strip().split()[2:]))
        boundFaces[-1][0] = boundFaces[-1][0] + 1
        boundFaces[-1][1] = boundFaces[-1][1] + 1
        boundFaces[-1][2] = boundFaces[-1][2] + 1
        boundFaces[-1].reverse()

    tets = [[]] * numTets
    mintet = 100
    maxtet = 0
    # reconstruct tets from face and face2tet information
    # for each face
    for faceNum, face in enumerate(faces):
        # for each vertex in the face
        for vert in face:
            # for each tet that has this face
            for tetNum in face2tet[faceNum]:
                # if this is a legit tet index
                if tetNum >= 0:
                    # use tetNum - 1 because .vmesh starts with tet 1
                    if (vert+1 in tets[tetNum]) == False:
                        tets[tetNum] = tets[tetNum] + [vert+1]

    # correct orientation of tets
    for tetNum, tet in enumerate(tets):
        a = points[tet[0]-1]
        b = points[tet[1]-1]
        c = points[tet[2]-1]
        d = points[tet[3]-1]
        # if tet is negative orientation, flip two verts
        if orient3d(a,b,c,d) == 0:
            print "WHOA! input zero-volume tet...\n"
        if orient3d(a,b,c,d) < 0:
            temp = tet[0]
            tets[tetNum][0] = tet[1]
            tets[tetNum][1] = temp

    return points, tets, boundFaces

def readNodeEle(filename, computeTopo=True):
    """Read a tetrahedral mesh in .node/.ele format, Jonathan Shewchuk's format.
       The .node file specifies the vertex locations and the .ele format specfies
       the tetrahedra. The .node file might start with an index of one or zero."""
    
    points, startFromZero = ReadNode(filename)
    tets = ReadEle(filename, startFromZero)

    # correct orientation of tets
    for tetNum, tet in enumerate(tets):
        a = points[tet[0]]
        b = points[tet[1]]
        c = points[tet[2]]
        d = points[tet[3]]
        # if tet is negative orientation, flip two verts
        if orient3d(a,b,c,d) == 0.0:
            print "WHOA! input zero-volume tet#%d...", tetNum
            print "a=", ' '.join(['%0.18g' % x for x in a])
            print "b=", ' '.join(['%0.18g' % x for x in b])
            print "c=", ' '.join(['%0.18g' % x for x in c])
            print "d=", ' '.join(['%0.18g' % x for x in d])
        if orient3d(a,b,c,d) < 0.0:
            print "correcting inverted tet #%d", tetNum
            temp = tet[0]
            tets[tetNum][0] = tet[1]
            tets[tetNum][1] = temp
            
    # build face topology information
    if computeTopo:
        faces, boundFaces, face2tet = GetFaceTopo(tets)
    else:
        boundFaces = None

    return points, tets, boundFaces

def GetFaceTopo(tets):
    """Recover topological information about faces"""

    # first, build list of all passible faces
    faces = []
    for tet in tets:
        # append each (outward oriented) face of the tet
        faces.append([tet[0], tet[1], tet[2]])
        faces.append([tet[0], tet[2], tet[3]])
        faces.append([tet[0], tet[3], tet[1]])
        faces.append([tet[1], tet[3], tet[2]])

    # sort the faces so the indices are in consistent order
    sortedFaces = [sorted(face) for face in faces]

    # get the unique faces
    uniqueFaces = unique(sortedFaces)

    uFaceDict = {}
    # build a dictionary of unique faces to speed lookup
    for facenum, face in enumerate(uniqueFaces):
        uFaceDict[tuple(face)] = facenum

    # build the tet -> face mapping by finding the index
    # into the unique face list for each face of each tet
    tet2face = []
    for tetNum in range(0, len(tets)):
        tet2face.append([uFaceDict[tuple(sortedFaces[tetNum*4])],
                        uFaceDict[tuple(sortedFaces[tetNum*4+1])],
                        uFaceDict[tuple(sortedFaces[tetNum*4+2])],
                        uFaceDict[tuple(sortedFaces[tetNum*4+3])]])
                        
    
    # build the face -> tet mapping by finding the one or two tets
    # that contain each face in the unique face list
    face2tet = []
    for face in uniqueFaces:
        face2tet.append([-1, -1])     

    for tetNum, tetfaces in enumerate(tet2face):
        for face in tetfaces:

            #print "creating face2tet, tetnum = %d, face = %d" % (tetNum, face)

            # if no tets have been recorded for this face, put it
            # in the first face
            if face2tet[face][0] == -1:
                face2tet[face][0] = tetNum
            else:
                # the second entry must not have a tet yet
                if (face2tet[face][1] != -1):
                    print "whoa, fount more than two tets for a face?"
                    print "tetnum is %d, face2tet[face] is" % tetNum, face2tet[face]
                assert(face2tet[face][1] == -1)
                face2tet[face][1] = tetNum

    #print "built face2tet."

    boundaryFaces = []
    # finally, get the list of boundary faces by building
    # list of all faces with just one tet
    for faceNum, facetets in enumerate(face2tet):
        if facetets[1] == -1:
            # this face has just one tet; it's a boundary face
            tet = tets[facetets[0]]
            tetfaces = [[tet[0], tet[1], tet[2]],
                        [tet[0], tet[2], tet[3]],
                        [tet[0], tet[3], tet[1]],
                        [tet[1], tet[3], tet[2]]]

            # find the properly oriented face
            foundface = False
            for face in tetfaces:
                if (uniqueFaces[faceNum] == sorted(face)):
                    #print "found correct oriented face [%d %d %d] corresponding to unique face [%d %d %d]" % (face[0], face[1], face[2], uniqueFaces[faceNum][0], uniqueFaces[faceNum][1], uniqueFaces[faceNum][2])
                    boundaryFaces.append(face)
                    foundface = True
            assert(foundface)
            foundface = False

    #print "there are %d boundary faces" % len(boundaryFaces)

    return uniqueFaces, boundaryFaces, face2tet

def readSurfTets(fileName):
    """A stub function to read surface faces from a .surf file,
       as output by NETGEN, as though it is a tet format"""
    points, tris = readSurf(fileName)

    return points, None, tris

def readSurf(fileName):
    """read .surf file format, containing the surface faces of a tet mesh
       as output by NETGEN"""
    inFileName = fileName + '.surf'

    # open input .surf file
    infile = open(inFileName)

    # read first line, make sure it's right
    header = infile.readline().strip()
    assert(header == 'surfacemesh')

    # read second line, number of vertices
    numPoints = int(infile.readline().strip())

    # read in all the points
    points = []

    for iter in range(0,numPoints):
        points.append(map(float,infile.readline().strip().split()))

    # read number of faces
    numTris = int(infile.readline().strip())

    # read in all the faces
    tris = []

    for iter in range(0,numTris):
        tri = map(int,infile.readline().strip().split())
        tri[0] = tri[0] - 1
        tri[1] = tri[1] - 1
        tri[2] = tri[2] - 1
        tris.append(tri)

    return points, tris


def ReadOFF(fileName):
    """Read OFF surface mesh format"""

    inFileName = fileName + '.off'

    # read input .off file
    infile = open(inFileName)

    # check for proper header
    header = infile.readline().strip()
    assert(header == 'OFF')

    # read second line "numverts numfaces 0"
    numPoints, numTris, blah = map(int, infile.readline().strip().split())

    print "reading OFF numPoints is %d numTris is %d" % (numPoints, numTris)

    # read in points
    points = []
    for iter in range(0,numPoints):
        points.append(map(float,infile.readline().strip().split()))

    # read in triangles
    tris = []

    for iter in range(0,numTris):
        tri = map(int,infile.readline().strip().split()[1:])
        tris.append(tri)

    return points, tris

def ReadOFFTets(fileName):
    """docstring for ReadOFFTet"""
    points, tris = ReadOFF(fileName)
    return points, None, tris

# read in a .node file (JRS' Pyramid format)
def ReadNode(fileName):
    inFileName = fileName + '.node'

    # open input .node file
    infile = open(inFileName)

    # fetch the number of points
    firstline = map(int,infile.readline().strip().split())
    numPoints = firstline[0]
    numMarkers = firstline[-1]

    # read in all the points
    points = []
    for iter in range(0,numPoints):
        # check whether point numbering starts at 1 (instead of 0)
        if iter == 0:
            firstline = infile.readline().strip().split()
            if firstline[0] == '1':
                startFromZero = False
            else:
                startFromZero = True
            # now put in the actual first point
            if numMarkers != 0:
                points.append(map(float,firstline[1:-numMarkers]))
            else:
                points.append(map(float,firstline[1:]))
        else:
            if numMarkers != 0:
                points.append(map(float,infile.readline().strip().split()[1:-numMarkers]))
            else:
                points.append(map(float,infile.readline().strip().split()[1:]))

    #print "in ReadNode startfrom zero is", startFromZero, "len(points) is", len(points)

    return points, startFromZero

# read tets from a .ele file (pyramid format)
def ReadEle(fileName, startFromZero=True):
    inFileName = fileName + '.ele'

    # open input .ele file
    infile = open(inFileName)

    # fetch the number of tets
    firstline = map(int,infile.readline().strip().split())
    numTets = firstline[0]
    numMarkers = firstline[-1]

    #print "in readele num markers is", numMarkers

    # read in all the tets
    tets = []
    for iter in range(0,numTets):
        # skip the tet number
        line = map(int,infile.readline().strip().split())
        if numMarkers == 0:
            tets.append(line[1:])
        else:
            tets.append(line[1:-numMarkers])

    # if vert indices started at one, decrement all the indices
    if startFromZero == False:
        for index, tet in enumerate(tets):
            tets[index] = vsubscalar(1,tet)

    #print "len(tets) is", len(tets)
    #maxpoint = max([max(tet) for tet in tets])
    #print "in readele max vertex index is", maxpoint

    return tets


def ReadTPoly(fileName):
    """read in tpoly output from adaptive skeleton climbing"""
    infile = open(fileName)

    meshString = infile.read()

    infile.close()

    triString = meshString[2:].split('\n3\n')

    vertCount = 0
    verts = []
    norms = []
    tris = []

    for tri in triString:
        trisplit = tri.split()
        verts.append(tuple(map(float,trisplit[0:3])))
        norms.append(tuple(map(float,trisplit[3:6])))
        verts.append(tuple(map(float,trisplit[6:9])))
        norms.append(tuple(map(float,trisplit[9:12])))
        verts.append(tuple(map(float,trisplit[12:15])))
        norms.append(tuple(map(float,trisplit[15:18])))
        tris.append((vertCount, vertCount+1, vertCount+2))
        vertCount += 3

    #print "verts:", verts
    #print "norms:", norms
    #print "tris:", tris

    return verts, tris


####
# Functions to WRITE meshes out to various formats
####
def writeMesh(points, tets, boundFaces, outFileName):
    outFileName += '.mesh'
    outfile = open(outFileName, 'w')

    # number of points
    outfile.write("%d\n" % len(points))
    # points
    for point in points:
        outfile.write(' ' + ' '.join(map(str,point)) + '\n')

    # number of tets
    outfile.write("%d\n" % len(tets))
    # tets
    for tet in tets:
        # add one because .mesh numbers verts from 1
        tet = [x + 1 for x in tet]
        outfile.write(" 1 " + ' '.join(map(str,tet)) + '\n')

    # number of boundary faces
    outfile.write("%d\n" % len(boundFaces))
    # boundary faces
    for face in boundFaces:
        # add one because .mesh numbers verts from 1
        face = [x + 1 for x in face]
        outfile.write(" 1 " + ' '.join(map(str,face)) + '\n')

def writeNode(points, outFileName):
    if outFileName.find('.node') == -1:
        outFileName += '.node'
    outfile = open(outFileName, 'w')

    # boilerplate first line
    outfile.write('%s 3 0 0\n' % len(points))

    # write out the points
    for iter in range(0,len(points)):
        outfile.write(str(iter+1) + ' ' + ' '.join(['%.18g' % x for x in points[iter]]) + '\n')
    pass

    outfile.close()

def writeEle(tets, outFileName):
    outFileName += '.ele'
    outfile = open(outFileName, 'w')

    # boilerplate first line
    outfile.write('%s 4 0 \n' % len(tets))

    for iter in range(0,len(tets)):
        tets[iter] = vaddscalar(1,tets[iter])
        outfile.write(str(iter+1) + ' ' + ' '.join(map(str,tets[iter])) + '\n')
    pass

    outfile.close()

def writeNodeEle(points, tets, boundFaces, outFileName):
    writeNode(points, outFileName)
    writeEle(tets, outFileName)


def writeOBJTets(points, tets, tris, outFileName):
    """docstring for writOBJTets"""
    writeOBJ(points, tris, outFileName)
    pass

def writeOBJ(verts, tris, outFileName):
    """write out an OBJ file from a list of vertices and triangles"""
    outFileName += '.obj'
    outfile = open(outFileName, 'w')

    for vert in verts:
        #print "vert:", vert
        outfile.write('v %.18g %.18g %.18g\n' % (vert[0], vert[1], vert[2]))
    for tri in tris:
        outfile.write('f %d %d %d\n' % (tri[0]+1, tri[1]+1, tri[2]+1))

    outfile.close()

def writeSurfTets(points, tets, tris, outFileName):
    """docstring for writOBJTets"""
    writeSurf(points, tris, outFileName)
    pass

# surf surface mesh format
def writeSurf(verts, tris, outFileName):
    """write out an surf file from a list of vertices and triangles"""
    outFileName += '.surf'
    outfile = open(outFileName, 'w')

    # write header lines
    outfile.write('surfacemesh\n')
    outfile.write('%d\n' % (len(verts)))

    for vert in verts:
        #print "vert:", vert
        outfile.write('%g %g %g\n' % (vert[0], vert[1], vert[2]))

    outfile.write('%d\n' % (len(tris)))

    for tri in tris:
        outfile.write('%d %d %d\n' % (tri[0], tri[1], tri[2]))

    outfile.close()

def writeOFFTets(points, tets, tris, outFileName):
    """docstring for writOBJTets"""
    writeOFF(points, tris, outFileName)
    pass

# OFF surface mesh format
def writeOFF(verts, tris, outFileName):
    """write out an OFF file from a list of vertices and triangles"""
    outFileName += '.off'
    outfile = open(outFileName, 'w')

    # write header lines
    outfile.write('OFF\n')
    outfile.write('%d %d 0\n' % (len(verts), len(tris)))

    for vert in verts:
        #print "vert:", vert
        outfile.write('%g %g %g\n' % (vert[0], vert[1], vert[2]))
    for tri in tris:
        outfile.write('3 %d %d %d\n' % (tri[0], tri[1], tri[2]))

    outfile.close()

## END file I/O functions


## Convenience math functions

# some simple geometry functions
def vadd(v1, v2):
    v3 = []
    # assumes vectors are of equal length
    for i in range(0,len(v1)):
        v3.append(v1[i] + v2[i])
    return v3

def vsub(v1, v2):
    v3 = []
    # assumes vectors are of equal length
    for i in range(0,len(v1)):
        v3.append(v1[i] - v2[i])
    return v3 

def vlength(v):
    length = 0
    for ele in v:
        length += ele * ele
    return math.sqrt(length)

def vscale(scale, v):
    return [x * scale for x in v]

def vnorm(v):
    length = vlength(v)
    vscale(1/length,v)
    return v

def vaddscalar(scalar, v):
    return [x + scalar for x in v]

def vsubscalar(scalar, v):
    return [x - scalar for x in v]

def orient3d(a,b,c,d):
    """Compute the orientation of 4 points in 3D"""
    m11 = a[0] - d[0]
    m12 = a[1] - d[1]
    m13 = a[2] - d[2]
    m21 = b[0] - d[0]
    m22 = b[1] - d[1]
    m23 = b[2] - d[2]
    m31 = c[0] - d[0]
    m32 = c[1] - d[1]
    m33 = c[2] - d[2]

    det = (m11 * m22 * m33) + (m12 * m23 * m31) + (m13 * m21 * m32) - (m11 * m23 * m32) - (m12 * m21 * m33) - (m13 * m22 * m31)

    return det

# return all the unique items in a list
def unique(s):
    """Return a list of the elements in s, but without duplicates.

    For example, unique([1,2,3,1,2,3]) is some permutation of [1,2,3],
    unique("abcabc") some permutation of ["a", "b", "c"], and
    unique(([1, 2], [2, 3], [1, 2])) some permutation of
    [[2, 3], [1, 2]].

    For best speed, all sequence elements should be hashable.  Then
    unique() will usually work in linear time.

    If not possible, the sequence elements should enjoy a total
    ordering, and if list(s).sort() doesn't raise TypeError it's
    assumed that they do enjoy a total ordering.  Then unique() will
    usually work in O(N*log2(N)) time.

    If that's not possible either, the sequence elements must support
    equality-testing.  Then unique() will usually work in quadratic
    time.
    """

    n = len(s)
    if n == 0:
        return []

    # Try using a dict first, as that's the fastest and will usually
    # work.  If it doesn't work, it will usually fail quickly, so it
    # usually doesn't cost much to *try* it.  It requires that all the
    # sequence elements be hashable, and support equality comparison.
    u = {}
    try:
        for x in s:
            u[x] = 1
    except TypeError:
        del u  # move on to the next method
    else:
        return u.keys()

    # We can't hash all the elements.  Second fastest is to sort,
    # which brings the equal elements together; then duplicates are
    # easy to weed out in a single pass.
    # NOTE:  Python's list.sort() was designed to be efficient in the
    # presence of many duplicate elements.  This isn't true of all
    # sort functions in all languages or libraries, so this approach
    # is more effective in Python than it may be elsewhere.
    try:
        t = list(s)
        t.sort()
    except TypeError:
        del t  # move on to the next method
    else:
        assert n > 0
        last = t[0]
        lasti = i = 1
        while i < n:
            if t[i] != last:
                t[lasti] = last = t[i]
                lasti += 1
            i += 1
        return t[:lasti]

    # Brute force is all that's left.
    u = []
    for x in s:
        if x not in u:
            u.append(x)
    return u

if __name__ == "__main__":
    sys.exit(main())