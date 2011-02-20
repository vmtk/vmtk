/*****************************************************************************/
/*                                                                           */
/*  tetrahedron quality functions                                            */
/*                                                                           */
/*****************************************************************************/

/* determine if a tet has any boundary vertices.
   return the number of boundary verts as well as
   their tags in boundtags */
int boundverts(struct tetcomplex *mesh,
               tag vtx1,
               tag vtx2,
               tag vtx3,
               tag vtx4,
               tag boundtags[])
{
    int i,j,k,l;     /* loop indices */
    bool noghosts;   /* any ghost tets incident to this vertex? */
    tag tet[4];      /* array version of tet indices */
    int numincident; /* number of incident tets */
    int numbound=0;  /* number of boundary verts */
    /* a list of all the tets incident to this vertex */
    tag incidenttettags[MAXINCIDENTTETS][4];
    
    tet[0] = vtx1;
    tet[1] = vtx2;
    tet[2] = vtx3;
    tet[3] = vtx4;
    
    /* check each vertex */
    for (i = 0; i < 4; i++) 
    {
        j = (i + 1) & 3;
        if ((i & 1) == 0) {
            l = (i + 3) & 3;
            k = (i + 2) & 3;
        } else {
            l = (i + 2) & 3;
            k = (i + 3) & 3;
        }
        
        /* get all the tets incident to this vertex. */
        numincident = 0;
        noghosts = true;
        getincidenttets(mesh, 
                        tet[i],
                        tet[j],
                        tet[k],
                        tet[l], 
                        incidenttettags,
                        &numincident,
                        &noghosts);
        
        /* is this a boundary vertex? */
        if (noghosts == false)
        {
            boundtags[numbound] = tet[i];
            numbound++;
        }
    }
    
    return numbound;
}

/* Retrieve the ring of tetrahedra around a particular edge, and meanwhile
   detect whether it's a boundary edge. Potentially don't store the ring of
   tets if the array to store them is null. Return a boolean indicating 
   whether it is a bounary edge, as well as the third vertices of the boundary
   faces */
#define NOBOUNDFACE -2
bool getedgering(struct tetcomplex *mesh,
                 tag vtx1, /* first vertex of the edge */
                 tag vtx2, /* second vertex of the edge */
                 tag vtx3, /* third vertex of a tet that contains the edge */
                 tag vtx4, /* fourth vertex of a tet that contains the edge */
                 int *numringtets, /* number of tets in the ring */
                 tag ringtets[MAXRINGTETS][4], /* the vertices of tets in the ring */
                 tag boundfaceverts[2] /* the third vertex of the two boundary faces */
                )
{
    tag ring[MAXRINGTETS]; /* array of tags for ring of vertices around edge */
    tag start;             /* the first vertex tag in an edge's ring */
    int ringcount = 0;     /* the number of vertices found in the ring so far */
    tag nextprev[2];       /* the next and previous vertices around the ring */
    int foundface;         /* indicates whether the face in the adj query exists */
    bool reverse = false;  /* for going back to find the other boundary face */
    bool forward = true;
    tag ev1, ev2;          /* first and second edge vertex */
    
    /* initialize boundary face vertex tags to cardinal value */
    boundfaceverts[0] = boundfaceverts[1] = (tag) NOBOUNDFACE;
    
    /* we will proceed around the ring of tets surrounding an edge by using
       repeated face adjancency queries until we reach the start of the ring
       again. If at any point we encounter a ghost tetrahedron, we'll know 
       that this edge must lie on the boundary, and to complete the ring we
       must return to the begining and proceed around the other direction */
    
    /* if we're recording the identity of the tets in the ring */
    if (numringtets != NULL)
    {
        *numringtets = 0;
        
        /* record the first tetrahedron in the ring, which is just
           the input tet. take care to record the edge verts as the
           first two */
        ringtets[*numringtets][0] = vtx1;
        ringtets[*numringtets][1] = vtx2;
        ringtets[*numringtets][2] = vtx3;
        ringtets[*numringtets][3] = vtx4;
        assert(tetexistsa(mesh, ringtets[*numringtets]));
        (*numringtets)++;
    }
    
    /* start by going one way around the edge. If we find a boundary face,
       then look back the other way */
    while (forward || reverse)
    {
        forward = false;
        
        /* the first vertex in the ring is set to another vertex in the
           current tet. We choose one that will orient the first face adjacency
           query out of the current tet: if the edge is ij, the third vertex is k
           swap order for reverse rotation. */
        start = (reverse) ? vtx4 : vtx3;
        ev1 = (reverse) ? vtx2 : vtx1;
        ev2 = (reverse) ? vtx1 : vtx2;
        ring[0] = start;
        ringcount = 0;
        
        /* now, use adjacency tests to move around the ring, until we get back where
           we started or the ring gets too big*/
        foundface = tetcomplexadjacencies(mesh, ev1, ev2, start, nextprev);
        assert(foundface == 1);
        assert(nextprev[1] == ((reverse) ? vtx3 : vtx4));
        
        /* check if we are at the boundary */
        if (nextprev[0] == GHOSTVERTEX)
        {
            /* record the identity of the face where we hit the boundary */
            if (reverse == false)
            {
                assert(boundfaceverts[0] == NOBOUNDFACE);
                boundfaceverts[0] = ring[ringcount];
            }
            else
            {
                assert(boundfaceverts[0] != NOBOUNDFACE);
                boundfaceverts[1] = ring[ringcount];
            }
            
            if (improvebehave.verbosity > 5)
            {
                printf("on first ring query found bound face with vert %d, reverse=%d\n", (int) ring[ringcount], (int) reverse);
            }
            
            /* if we aren't on a reverse run already, schedule one */
            reverse = (reverse) ? false : true;
             
            /* don't try to proceed any further around this ring */
            continue;
        }
        
        /* we found a legitimate tet next in the ring */
        ringcount++;
        
        /* as long as we don't complete the ring, keep moving around */
        while ((ringcount < MAXRINGTETS) && (nextprev[0] != start))
        {
            /* add the next vertex to the ring */
            ring[ringcount] = nextprev[0];
            
            /* record the new tet in the ring */
            if (numringtets != NULL)
            {
                /* we found a tet! record it the list of tets, always
                   recording the two edge vertices first */
                ringtets[*numringtets][0] = ev1;
                ringtets[*numringtets][1] = ev2;
                ringtets[*numringtets][2] = ring[ringcount];
                ringtets[*numringtets][3] = ring[ringcount - 1];
                assert(tetexistsa(mesh, ringtets[*numringtets]));
                (*numringtets)++;
            }
            
            /* look for next vertex */
            foundface = tetcomplexadjacencies(mesh, ev1, ev2, nextprev[0], nextprev);
            assert(foundface == 1);
            assert(nextprev[1] == ring[ringcount-1]);
            
            /* check again for ghost vertex */
            if (nextprev[0] == GHOSTVERTEX)
            {
                /* record the identity of the face where we hit the boundary */
                if (reverse == false)
                {
                    assert(boundfaceverts[0] == NOBOUNDFACE);
                    boundfaceverts[0] = ring[ringcount];
                }
                else
                {
                    assert(boundfaceverts[0] != NOBOUNDFACE);
                    boundfaceverts[1] = ring[ringcount];
                }
                
                if (improvebehave.verbosity > 5)
                {
                    printf("on query %d found bound face with vert %d, reverse=%d\n", (int) ringcount, (int) ring[ringcount], (int) reverse);
                }
                
                /* if we aren't on a reverse run already, schedule one */
                reverse = (reverse) ? false : true;
                
                /* stop looking in this direction */
                break;
            }
        
            ringcount++;
        }
    }
    
    assert(ringcount >= 0 && ringcount < MAXRINGTETS);
    
    /* we've now finished looking forward and potentially back around
       the edge. Return true if this is a boundary edge, false if not */
    if (boundfaceverts[0] != NOBOUNDFACE)
    {
        /* we must have found both boundary faces */
        assert(boundfaceverts[1] != NOBOUNDFACE);
        /* and they can't be the same */
        assert(boundfaceverts[0] != boundfaceverts[1]);
        
        return true;
    }
    return false;
}

/* returns the number of edges of this tet that lie on the boundary
   along with a list of them */
int boundedges(struct tetcomplex *mesh,
               tag vtx1,
               tag vtx2,
               tag vtx3,
               tag vtx4,
               tag edgelist[][2],
               tag edgefaces[6][2],
               int numedgetets[6],
               tag edgetets[6][MAXRINGTETS][4])
{
    tag tet[4];            /* the current tet we are trying to improve */
    int i,j,k,l;           /* loop indices for each tet vertex */
    int temp;
    bool boundedge;        /* whether a particular edge is a boundary edge */
    int numboundedges = 0; /* number of boundary edges found */
    
    tet[0] = vtx1;
    tet[1] = vtx2;
    tet[2] = vtx3;
    tet[3] = vtx4;
    
    
    for (i = 0; i < 3; i++) 
    {
        for (j = i + 1; j < 4; j++) 
        {
            k = (i > 0) ? 0 : (j > 1) ? 1 : 2;
            l = 6 - i - j - k;
            /* to get right sequence, need to swap k and l sometimes */
            if ((i+j) % 2 == 0)
            {
                temp = k;
                k = l;
                l = temp;
            }
            
            /* now our tet is (i,j,k,l). check if this is a boundary edge */
            boundedge = getedgering(mesh, tet[i], tet[j], tet[k], tet[l],
                                    (numedgetets == NULL) ? NULL : &numedgetets[numboundedges],
                                    edgetets[numboundedges], 
                                    edgefaces[numboundedges]);
            /* if this was a boundary edge */
            if (boundedge)
            {
                /* save this edge in the list of boundary edges */
                edgelist[numboundedges][0] = tet[i];
                edgelist[numboundedges][1] = tet[j];
                numboundedges++;
            }
        }
    }
    
    return numboundedges;
}

/* determine if any of the faces of a tet lie on
   the boundary. if so, return true, and put
   the boundary faces in boundfaces */
bool boundfaces(struct tetcomplex *mesh,
                  tag vtx1,
                  tag vtx2,
                  tag vtx3,
                  tag vtx4,
                  tag boundfaces[][3],
                  int *numboundary)
{
    int foundface;
    tag topbot[2];
    
    *numboundary = 0;
    
    /* make sure this tet actually exists */
    assert(tetexists(mesh, vtx1, vtx2, vtx3, vtx4));
    
    /* check adjacencies of each face looking for ghost vertex */
    /* faces are oriented out from tet */
    foundface = tetcomplexadjacencies(mesh, vtx1, vtx2, vtx3, topbot);
    assert(foundface == 1);
    assert(topbot[1] == vtx4);
    if (topbot[0] == GHOSTVERTEX)
    {
        boundfaces[*numboundary][0] = vtx1;
        boundfaces[*numboundary][1] = vtx2;
        boundfaces[*numboundary][2] = vtx3;
        *numboundary = *numboundary + 1;
    }
    
    foundface = tetcomplexadjacencies(mesh, vtx1, vtx3, vtx4, topbot);
    assert(foundface == 1);
    assert(topbot[1] == vtx2);
    if (topbot[0] == GHOSTVERTEX)
    {
        boundfaces[*numboundary][0] = vtx1;
        boundfaces[*numboundary][1] = vtx3;
        boundfaces[*numboundary][2] = vtx4;
        *numboundary = *numboundary + 1;
    }
    
    foundface = tetcomplexadjacencies(mesh, vtx1, vtx4, vtx2, topbot);
    assert(foundface == 1);
    assert(topbot[1] == vtx3);
    if (topbot[0] == GHOSTVERTEX)
    {
        boundfaces[*numboundary][0] = vtx1;
        boundfaces[*numboundary][1] = vtx4;
        boundfaces[*numboundary][2] = vtx2;
        *numboundary = *numboundary + 1;
    }
    
    foundface = tetcomplexadjacencies(mesh, vtx2, vtx4, vtx3, topbot);
    assert(foundface == 1);
    assert(topbot[1] == vtx1);
    if (topbot[0] == GHOSTVERTEX)
    {
        boundfaces[*numboundary][0] = vtx2;
        boundfaces[*numboundary][1] = vtx4;
        boundfaces[*numboundary][2] = vtx3;
        *numboundary = *numboundary + 1;
    }
    
    if (*numboundary > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/* returns true if any of this tet's faces lie
   on the boundary */
bool boundtet(struct tetcomplex *mesh,
              tag vtx1,
              tag vtx2,
              tag vtx3,
              tag vtx4)
{
    tag boundfacetags[4][3];
    int numboundfaces;
    return (boundfaces(mesh, vtx1, vtx2, vtx3, vtx4, boundfacetags, &numboundfaces) == true);
}

/* generate a list of all boundary faces in the mesh */
void getsurface(struct tetcomplex *mesh,
                struct arraypool *facepool,
                int *numfaces)
{
    struct tetcomplexposition pos;
    tag tet[4];         /* current tet */
    tag tetfaces[4][3]; /* a list of boundary faces for the current tet */
    int numtetfaces;    /* the number of boundary faces for the current tet */
    int i;
    tag *newface;       /* pointer to new face in pool */
    
    *numfaces = 0;
    
    tetcomplexiteratorinit(mesh, &pos);
    tetcomplexiteratenoghosts(&pos, tet);
    
    /* get boundary faces of all tets */
    while (tet[0] != STOP) 
    {
        /* does this tet have any boundary faces? */
        if (boundfaces(mesh, tet[0], tet[1], tet[2], tet[3], tetfaces, &numtetfaces))
        {
            /* if so, copy them to the pool of surface faces */
            for (i=0; i<numtetfaces; i++)
            {
                /* allocate a new face in the array pool */
                newface = (tag *) arraypoolforcelookup(facepool, (unsigned long) *numfaces);
                
                /* assign vertices from this face */
                newface[0] = tetfaces[i][0];
                newface[1] = tetfaces[i][1];
                newface[2] = tetfaces[i][2];
                
                /* increment count of faces */
                *numfaces = *numfaces + 1;
            }
        }
        
        tetcomplexiteratenoghosts(&pos, tet);
    }
}

/* return the volume of a tetrahedron */
starreal tetvolume(struct tetcomplex *mesh,
                   tag vtx1,
                   tag vtx2,
                   tag vtx3,
                   tag vtx4)
{
    starreal point[4][3];      /* the vertices of the tet */
    starreal E[3][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
    int i;
    
    /* get tet vertices */
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx1))->coord, point[0]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx2))->coord, point[1]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx3))->coord, point[2]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx4))->coord, point[3]);
    
    /* if anisotropic meshing is enabled, warp the points according the
       deformation tensor at their barycenter */
    if (improvebehave.anisotropic)
    {
        /* fetch the deformation tensor at the barycenter of this tet */
        tettensor(mesh, vtx1, vtx2, vtx3, vtx4, E);
        
        /* transform each vertex */
        for (i=0; i<4; i++)
        {
            if (improvebehave.verbosity > 5)
            {
                printf("In tetvolume transforming point (%g %g %g) ->", point[i][0], point[i][1], point[i][2]);
            }
            
            tensortransform(point[i], point[i], E);
            
            if (improvebehave.verbosity > 5)
            {
                printf(" (%g %g %g)\n", point[i][0], point[i][1], point[i][2]);
            }
        }
    }
    
    /* calculate the volume of the tetrahedron */
    return orient3d(&behave, point[0], point[1], point[2], point[3]) / 6.0;
}

/* return the length of a tet's longest (shortest) edge,
   as well as the indices of its endpoints. "getlong" 
   argument set to true computes longest, otherwise shortest */
starreal tetedge(struct tetcomplex *mesh,
                 tag vtx1,
                 tag vtx2,
                 tag vtx3,
                 tag vtx4,
                 int edge[2],
                 bool getlong)
{
    int i,j;                  /* loop indices */
    starreal point[2][3];     /* the vertices of the tet */
    starreal length;          /* the current edge length */
    starreal longest;         /* the longest (shortest) edge */
    starreal dx, dy, dz;
    starreal E[3][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
    tag tet[4];
    
    longest = (getlong) ? 0.0 : HUGEFLOAT;
    
    tet[0] = vtx1;
    tet[1] = vtx2;
    tet[2] = vtx3;
    tet[3] = vtx4;
    
    /* for each edge in the tet */
    for (i=0; i<3; i++)
    {
        for (j=i+1; j<4; j++)
        {
            /* get tet vertices */
            vcopy(((struct vertex *) tetcomplextag2vertex(mesh, tet[i]))->coord, point[0]);
            vcopy(((struct vertex *) tetcomplextag2vertex(mesh, tet[j]))->coord, point[1]);
            
            /* if anisotropic meshing is enabled, warp the points according the
               deformation tensor at their barycenter */
            if (improvebehave.anisotropic)
            {
                /* fetch the deformation tensor at the midpoint of this edge */
                edgetensor(mesh, tet[i], tet[j], E);

                /* transform each vertex */
                tensortransform(point[0], point[0], E);
                tensortransform(point[1], point[1], E);
            }
            
            dx = point[0][0] - point[1][0];
            dy = point[0][1] - point[1][1];
            dz = point[0][2] - point[1][2];
            length = sqrt(dx * dx + dy * dy + dz * dz);
            
            if ((length > longest && getlong) || (length < longest && (!getlong)))
            {
                longest = length;
                edge[0] = i;
                edge[1] = j;
            }
        }
    }
    
    assert(longest > 0.0);
    return longest;
}

/* compute the (square) of the minimum sine
   of all the dihedral angles in the tet defined
   by the four vertices (vtx1, vtx2, vtx3, vtx4)
*/
starreal minsine(struct tetcomplex *mesh,
                   tag vtx1,
                   tag vtx2,
                   tag vtx3,
                   tag vtx4)
{
    starreal point[4][3];      /* tet vertices */
    starreal edgelength[3][4]; /* the lengths of each of the edges of the tet */
    starreal facenormal[4][3]; /* the normals of each face of the tet */
    starreal dx, dy, dz;       /* intermediate values of edge lengths */
    starreal facearea2[4];     /* areas of the faces of the tet */
    starreal pyrvolume;        /* volume of tetrahedron */
    starreal sine2, minsine2;  /* the sine (squared) of the dihedral angle */
    int i, j, k, l;          /* loop indices */
    starreal E[3][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
    
    /* get tet vertices */
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx1))->coord, point[0]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx2))->coord, point[1]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx3))->coord, point[2]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx4))->coord, point[3]);
    
    /* if anisotropic meshing is enabled, warp the points according the
       deformation tensor at their barycenter */
    if (improvebehave.anisotropic)
    {
        /* fetch the deformation tensor at the barycenter of this tet */
        tettensor(mesh, vtx1, vtx2, vtx3, vtx4, E);
        
        /* transform each vertex */
        for (i=0; i<4; i++)
        {
            if (improvebehave.verbosity > 5)
            {
                printf("in minsine transforming point (%g %g %g) ->", point[i][0], point[i][1], point[i][2]);
            }
            
            tensortransform(point[i], point[i], E);
            
            if (improvebehave.verbosity > 5)
            {
                printf(" (%g %g %g)\n", point[i][0], point[i][1], point[i][2]);
            }
        }
    }
    
    /* calculate the volume*6 of the tetrahedron */
    pyrvolume = (starreal) orient3d(&behave, point[0], point[1], point[2], point[3]);
    
    /* if the volume is zero, the quality is zero, no reason to continue */
    if (pyrvolume == 0.0)
    {
        return 0.0;
    }
    
    /* for each vertex/face of the tetrahedron */
    for (i = 0; i < 4; i++) {
        j = (i + 1) & 3;
        if ((i & 1) == 0) {
            k = (i + 3) & 3;
            l = (i + 2) & 3;
        } else {
            k = (i + 2) & 3;
            l = (i + 3) & 3;
        }
        
        /* compute the normal for each face */
        facenormal[i][0] =
            (point[k][1] - point[j][1]) * (point[l][2] - point[j][2]) -
            (point[k][2] - point[j][2]) * (point[l][1] - point[j][1]);
        facenormal[i][1] =
            (point[k][2] - point[j][2]) * (point[l][0] - point[j][0]) -
            (point[k][0] - point[j][0]) * (point[l][2] - point[j][2]);
        facenormal[i][2] =
            (point[k][0] - point[j][0]) * (point[l][1] - point[j][1]) -
            (point[k][1] - point[j][1]) * (point[l][0] - point[j][0]);
            
        /* compute (2 *area)^2 for this face */
        facearea2[i] = facenormal[i][0] * facenormal[i][0] +
            facenormal[i][1] * facenormal[i][1] +
            facenormal[i][2] * facenormal[i][2];
        
        /* compute edge lengths (squared) */
        for (j = i + 1; j < 4; j++) {
            dx = point[i][0] - point[j][0];
            dy = point[i][1] - point[j][1];
            dz = point[i][2] - point[j][2];
            edgelength[i][j] = dx * dx + dy * dy + dz * dz;
        }
    }
    
    minsine2 = HUGEFLOAT;     /* start with absurdly big value for sine */
    
    /* for each edge in the tetrahedron */
    for (i = 0; i < 3; i++) {
        for (j = i + 1; j < 4; j++) {
            k = (i > 0) ? 0 : (j > 1) ? 1 : 2;
            l = 6 - i - j - k;
            
            /* compute the expression for minimum sine, squared, over 4 
               The reason it's over 4 is because the area values we have
               are actually twice the area squared */
            /* if either face area is zero, the sine is zero */
            if (facearea2[k] > 0 && facearea2[l] > 0)
            {
                sine2 = edgelength[i][j] / (facearea2[k] * facearea2[l]);
            }
            else
            {
                if (improvebehave.verbosity > 5) 
                {
                    printf("Encountered zero-area face.\n");
                    printf("Here is the tet (%d %d %d %d):\n", (int) vtx1, (int) vtx2, (int) vtx3, (int) vtx4);
                    printf(",\n");
                }
                sine2 = 0.0;
            }
            
            /* update minimum sine */
            if (sine2 < minsine2)
            {
                minsine2 = sine2;
            }
        }
    }
    
    return sqrt(minsine2) * pyrvolume;
}

/* compute the minimum or maximum angle of the tet defined
   by the four vertices (vtx1, vtx2, vtx3, vtx4)
*/
starreal minmaxangle(struct tetcomplex *mesh,
                     tag vtx1,
                     tag vtx2,
                     tag vtx3,
                     tag vtx4,
                     bool max)
{
    starreal point[4][3];      /* tet vertices */
    starreal edgelength[3][4]; /* the lengths of each of the edges of the tet */
    starreal facenormal[4][3]; /* the normals of each face of the tet */
    starreal dx, dy, dz;       /* intermediate values of edge lengths */
    starreal pyrvolume;        /* volume of tetrahedron */
    int i, j, k, l;          /* loop indices */
    starreal E[3][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
    starreal minangle = HUGEFLOAT;
    starreal maxangle = 0.0;
    starreal angle, tantheta;
    starreal dotproduct;
    
    /* get tet vertices */
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx1))->coord, point[0]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx2))->coord, point[1]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx3))->coord, point[2]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx4))->coord, point[3]);
    
    /* if anisotropic meshing is enabled, warp the points according the
       deformation tensor at their barycenter */
    if (improvebehave.anisotropic)
    {
        /* fetch the deformation tensor at the barycenter of this tet */
        tettensor(mesh, vtx1, vtx2, vtx3, vtx4, E);
        
        /* transform each vertex */
        for (i=0; i<4; i++)
        {
            if (improvebehave.verbosity > 5)
            {
                printf("in minsine transforming point (%g %g %g) ->", point[i][0], point[i][1], point[i][2]);
            }
            
            tensortransform(point[i], point[i], E);
            
            if (improvebehave.verbosity > 5)
            {
                printf(" (%g %g %g)\n", point[i][0], point[i][1], point[i][2]);
            }
        }
    }
    
    /* calculate the volume*6 of the tetrahedron */
    pyrvolume = (starreal) orient3d(&behave, point[0], point[1], point[2], point[3]);
    
    /* for each vertex/face of the tetrahedron */
    for (i = 0; i < 4; i++) {
        j = (i + 1) & 3;
        if ((i & 1) == 0) {
            k = (i + 3) & 3;
            l = (i + 2) & 3;
        } else {
            k = (i + 2) & 3;
            l = (i + 3) & 3;
        }
        
        /* compute the normal for each face */
        facenormal[i][0] =
            (point[k][1] - point[j][1]) * (point[l][2] - point[j][2]) -
            (point[k][2] - point[j][2]) * (point[l][1] - point[j][1]);
        facenormal[i][1] =
            (point[k][2] - point[j][2]) * (point[l][0] - point[j][0]) -
            (point[k][0] - point[j][0]) * (point[l][2] - point[j][2]);
        facenormal[i][2] =
            (point[k][0] - point[j][0]) * (point[l][1] - point[j][1]) -
            (point[k][1] - point[j][1]) * (point[l][0] - point[j][0]);
        
        /* compute edge lengths (squared) */
        for (j = i + 1; j < 4; j++) {
            dx = point[i][0] - point[j][0];
            dy = point[i][1] - point[j][1];
            dz = point[i][2] - point[j][2];
            edgelength[i][j] = dx * dx + dy * dy + dz * dz;
        }
    }
    
    /* for each edge in the tetrahedron */
    for (i = 0; i < 3; i++) {
        for (j = i + 1; j < 4; j++) {
            k = (i > 0) ? 0 : (j > 1) ? 1 : 2;
            l = 6 - i - j - k;
            
            /* compute the tangent of the angle using the tangent formula:

               tan(theta_ij) = - 6 * V * l_ij
                               --------------
                                dot(n_k, n_l)

               because this formula is accurate in the entire range.
            */
            dotproduct = dot(facenormal[k], facenormal[l]);
            
            if (dotproduct != 0.0)
            {
                tantheta = (-pyrvolume * sqrt(edgelength[i][j])) / dotproduct;
            
                /* now compute the actual angle */
                angle = atan(tantheta);
            }
            else
            {
                angle = PI / 2.0;
            }
            
            /* adjust angle for sign of dot product */
            if (dotproduct > 0)
            {
                angle += PI;
            }

            /* make negative angles positive */
            if (angle < 0)
            {
                angle += 2.0 * PI;
            }
            
            if (dotproduct == 0.0) angle = PI / 2.0;
            
            if (improvebehave.verbosity > 5)
            {
                printf("Final angle found is %g radians, or %g degrees\n", angle, angle * (180.0 / PI));
            }
            
            if (angle < minangle) minangle = angle;
            if (angle > maxangle) maxangle = angle;
        }
    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("for tet, found min angle = %g (sine is %g), max angle = %g (sine is %g)\n", radtodeg(minangle), sin(minangle), radtodeg(maxangle), sin(maxangle));
        printf("minsine computation says %g\n", minsine(mesh, vtx1, vtx2, vtx3, vtx4));
        assert(radtodeg(maxangle) <= 180.0);
    }
    
    /*
    assert(radtodeg(maxangle) <= 180.0);
    assert(minangle >= 0.0);
    */
    
    if (max) return radtodeg(maxangle);
    return radtodeg(minangle);
}

/* warp the sine of the dihedral angle to penalize obtuse angles more than acute */
starreal warpsine(starreal sine)
{
    if (improvebehave.verbosity > 6)
    {
        printf("in quality, warping sine of obtuse from %g to %g\n", sine, sine * improvebehave.sinewarpfactor);
    }
    
    return sine * improvebehave.sinewarpfactor; 
}

/* compute the (square) of the minimum sine
   of all the dihedral angles in the tet defined
   by the four vertices (vtx1, vtx2, vtx3, vtx4)
*/
starreal warpedminsine(struct tetcomplex *mesh,
                   tag vtx1,
                   tag vtx2,
                   tag vtx3,
                   tag vtx4)
{
    starreal point[4][3];      /* the vertices of the tet */
    starreal edgelength[3][4]; /* the lengths of each of the edges of the tet */
    starreal facenormal[4][3]; /* the normals of each face of the tet */
    starreal dx, dy, dz;       /* intermediate values of edge lengths */
    starreal facearea2[4];     /* areas of the faces of the tet */
    starreal pyrvolume;        /* volume of tetrahedron */
    starreal sine2, minsine2;  /* the sine (squared) of the dihedral angle */
    int i, j, k, l;          /* loop indices */
    starreal E[3][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};

    /* get tet vertices */
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx1))->coord, point[0]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx2))->coord, point[1]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx3))->coord, point[2]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx4))->coord, point[3]);
    
    /* if anisotropic meshing is enabled, warp the points according the
       deformation tensor at their barycenter */
    if (improvebehave.anisotropic)
    {
        /* fetch the deformation tensor at the barycenter of this tet */
        tettensor(mesh, vtx1, vtx2, vtx3, vtx4, E);
        
        /* transform each vertex */
        for (i=0; i<4; i++)
        {
            if (improvebehave.verbosity > 5)
            {
                printf("in warpedminsine transforming point (%g %g %g) ->", point[i][0], point[i][1], point[i][2]);
            }
            
            tensortransform(point[i], point[i], E);
            
            if (improvebehave.verbosity > 5)
            {
                printf(" (%g %g %g)\n", point[i][0], point[i][1], point[i][2]);
            }
        }
    }
    
    /* calculate the volume*6 of the tetrahedron */
    pyrvolume = (starreal) orient3d(&behave, point[0], point[1], point[2], point[3]);
    
    /* if the volume is zero, the quality is zero, no reason to continue */
    if (pyrvolume == 0.0)
    {
        return 0.0;
    }
    
    /* for each vertex/face of the tetrahedron */
    for (i = 0; i < 4; i++) {
        j = (i + 1) & 3;
        if ((i & 1) == 0) {
            k = (i + 3) & 3;
            l = (i + 2) & 3;
        } else {
            k = (i + 2) & 3;
            l = (i + 3) & 3;
        }
        
        /* compute the normal for each face */
        facenormal[i][0] =
            (point[k][1] - point[j][1]) * (point[l][2] - point[j][2]) -
            (point[k][2] - point[j][2]) * (point[l][1] - point[j][1]);
        facenormal[i][1] =
            (point[k][2] - point[j][2]) * (point[l][0] - point[j][0]) -
            (point[k][0] - point[j][0]) * (point[l][2] - point[j][2]);
        facenormal[i][2] =
            (point[k][0] - point[j][0]) * (point[l][1] - point[j][1]) -
            (point[k][1] - point[j][1]) * (point[l][0] - point[j][0]);
            
        /* compute (2 *area)^2 for this face */
        facearea2[i] = facenormal[i][0] * facenormal[i][0] +
            facenormal[i][1] * facenormal[i][1] +
            facenormal[i][2] * facenormal[i][2];
        
        /* compute edge lengths (squared) */
        for (j = i + 1; j < 4; j++) {
            dx = point[i][0] - point[j][0];
            dy = point[i][1] - point[j][1];
            dz = point[i][2] - point[j][2];
            edgelength[i][j] = dx * dx + dy * dy + dz * dz;
        }
    }
    
    minsine2 = HUGEFLOAT;     /* start with absurdly big value for sine */
    
    /* for each edge in the tetrahedron */
    for (i = 0; i < 3; i++) {
        for (j = i + 1; j < 4; j++) {
            k = (i > 0) ? 0 : (j > 1) ? 1 : 2;
            l = 6 - i - j - k;
            
            /* compute the expression for minimum sine, squared, over 4 
               The reason it's over 4 is because the area values we have
               are actually twice the area squared */
            /* if either face area is zero, the sine is zero */
            if (facearea2[k] > 0 && facearea2[l] > 0)
            {
                sine2 = edgelength[i][j] / (facearea2[k] * facearea2[l]);
            }
            else
            {
                if (improvebehave.verbosity > 5) 
                {
                    printf("Encountered zero-area face.\n");
                    printf("Here is the tet (%d %d %d %d):\n", (int) vtx1, (int) vtx2, (int) vtx3, (int) vtx4);
                    printf(",\n");
                }
                sine2 = 0.0;
            }
            
            /* check whether this angle is obtuse */
            if (dot(facenormal[k],facenormal[l]) > 0)
            {
                /* if so, warp it down */
                sine2 = warpsine(sqrt(sine2));
                sine2 *= sine2;
            }
            
            /* update minimum sine */
            if (sine2 < minsine2)
            {
                minsine2 = sine2;
            }
        }
    }
    
    return sqrt(minsine2) * pyrvolume;
}

/* compute the (square) of the minimum sine
   of all the dihedral angles in the tet defined
   by the four vertices (vtx1, vtx2, vtx3, vtx4)
*/
starreal minsineandedgeratio(struct tetcomplex *mesh,
                             tag vtx1,
                             tag vtx2,
                             tag vtx3,
                             tag vtx4)
{
    starreal point[4][3];       /* the vertices of the tet */
    starreal edgelength[3][4];  /* the lengths of each of the edges of the tet */
    starreal facenormal[4][3];  /* the normals of each face of the tet */
    starreal dx, dy, dz;        /* intermediate values of edge lengths */
    starreal facearea2[4];      /* areas of the faces of the tet */
    starreal pyrvolume;         /* volume of tetrahedron */
    starreal sine2, minsine2;   /* the sine (squared) of the dihedral angle */
    starreal minsine;
    starreal shortest = HUGEFLOAT;
    starreal longest = 0.0;     /* shortest and longest edges in teh tet */
    starreal edgeratio;         /* ratio of shortest to longest edge */
    int i, j, k, l;           /* loop indices */
    
    /* get tet vertices */
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx1))->coord, point[0]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx2))->coord, point[1]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx3))->coord, point[2]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx4))->coord, point[3]);
    
    /* calculate the volume*6 of the tetrahedron */
    pyrvolume = (starreal) orient3d(&behave, point[0], point[1], point[2], point[3]);
    
    /* for each vertex/face of the tetrahedron */
    for (i = 0; i < 4; i++) {
        j = (i + 1) & 3;
        if ((i & 1) == 0) {
            k = (i + 3) & 3;
            l = (i + 2) & 3;
        } else {
            k = (i + 2) & 3;
            l = (i + 3) & 3;
        }
        
        /* compute the normal for each face */
        facenormal[i][0] =
            (point[k][1] - point[j][1]) * (point[l][2] - point[j][2]) -
            (point[k][2] - point[j][2]) * (point[l][1] - point[j][1]);
        facenormal[i][1] =
            (point[k][2] - point[j][2]) * (point[l][0] - point[j][0]) -
            (point[k][0] - point[j][0]) * (point[l][2] - point[j][2]);
        facenormal[i][2] =
            (point[k][0] - point[j][0]) * (point[l][1] - point[j][1]) -
            (point[k][1] - point[j][1]) * (point[l][0] - point[j][0]);
            
        /* compute (2 *area)^2 for this face */
        facearea2[i] = facenormal[i][0] * facenormal[i][0] +
            facenormal[i][1] * facenormal[i][1] +
            facenormal[i][2] * facenormal[i][2];
        
        /* compute edge lengths (squared) */
        for (j = i + 1; j < 4; j++) {
            dx = point[i][0] - point[j][0];
            dy = point[i][1] - point[j][1];
            dz = point[i][2] - point[j][2];
            edgelength[i][j] = dx * dx + dy * dy + dz * dz;
            
            /* keep track of longest and shortest edge */
            if (edgelength[i][j] > longest) longest = edgelength[i][j];
            if (edgelength[i][j] < shortest) shortest = edgelength[i][j];
        }
    }
    
    minsine2 = 10.0e10;     /* start with absurdly big value for sine */
    
    /* for each edge in the tetrahedron */
    for (i = 0; i < 3; i++) {
        for (j = i + 1; j < 4; j++) {
            k = (i > 0) ? 0 : (j > 1) ? 1 : 2;
            l = 6 - i - j - k;
            
            /* compute the expression for minimum sine, squared, over 4 
               The reason it's over 4 is because the area values we have
               are actually twice the area squared */
            /* if either face area is zero, the sine is zero */
            if (facearea2[k] > 0 && facearea2[l] > 0)
            {
                sine2 = edgelength[i][j] / (facearea2[k] * facearea2[l]);
            }
            else
            {
                if (improvebehave.verbosity > 5) 
                {
                    printf("Encountered zero-area face.\n");
                    printf("Here is the tet (%d %d %d %d):\n", (int) vtx1, (int) vtx2, (int) vtx3, (int) vtx4);
                    printf(",\n");
                }
                sine2 = 0.0;
            }
            /* update minimum sine */
            if (sine2 < minsine2)
            {
                minsine2 = sine2;
            }
        }
    }
    
    /* edge ratio, scaled down for parity with sin of equilateral tet's dihedrals */
    edgeratio = sqrt(shortest / longest) * SINEEQUILATERAL;
    minsine = sqrt(minsine2) * pyrvolume;
    
    if (improvebehave.verbosity > 5)
    {
        printf("longest edge is %g, shortest edge is %g, ratio is %g\n", sqrt(longest), sqrt(shortest), edgeratio);
        printf("minsine is %g\n", minsine);
    }
    
    if (edgeratio < minsine) 
    {
        if (improvebehave.verbosity > 5)
        {
            printf("edge ratio dominates! edgeratio = %g, minsine = %g\n", edgeratio, minsine);
        }
        return edgeratio;
    }
    return minsine;
}

/* compute the mean of the sines
   of all the dihedral angles in the tet defined
   by the four vertices (vtx1, vtx2, vtx3, vtx4)
*/
starreal meansine(struct tetcomplex *mesh,
                   tag vtx1,
                   tag vtx2,
                   tag vtx3,
                   tag vtx4)
{
    starreal *point[4];      /* the vertices of the tet */
    starreal edgelength[3][4]; /* the lengths of each of the edges of the tet */
    starreal facenormal[4][3]; /* the normals of each face of the tet */
    starreal dx, dy, dz;       /* intermediate values of edge lengths */
    starreal facearea2[4];     /* areas of the faces of the tet */
    starreal pyrvolume;        /* volume of tetrahedron */
    starreal sine2;            /* the sine (squared) of the dihedral angle */
    starreal sinesum=0.0;      /* the accumulating sum of the sines */
    int i, j, k, l;          /* loop indices */
    
    /* get tet vertices */
    point[0] = ((struct vertex *) tetcomplextag2vertex(mesh, vtx1))->coord;
    point[1] = ((struct vertex *) tetcomplextag2vertex(mesh, vtx2))->coord;
    point[2] = ((struct vertex *) tetcomplextag2vertex(mesh, vtx3))->coord;
    point[3] = ((struct vertex *) tetcomplextag2vertex(mesh, vtx4))->coord;
    
    /* calculate the volume*6 of the tetrahedron */
    pyrvolume = (starreal) orient3d(&behave, point[0], point[1], point[2], point[3]);
    
    /* for each vertex/face of the tetrahedron */
    for (i = 0; i < 4; i++) {
        j = (i + 1) & 3;
        if ((i & 1) == 0) {
            k = (i + 3) & 3;
            l = (i + 2) & 3;
        } else {
            k = (i + 2) & 3;
            l = (i + 3) & 3;
        }
        
        /* compute the normal for each face */
        facenormal[i][0] =
            (point[k][1] - point[j][1]) * (point[l][2] - point[j][2]) -
            (point[k][2] - point[j][2]) * (point[l][1] - point[j][1]);
        facenormal[i][1] =
            (point[k][2] - point[j][2]) * (point[l][0] - point[j][0]) -
            (point[k][0] - point[j][0]) * (point[l][2] - point[j][2]);
        facenormal[i][2] =
            (point[k][0] - point[j][0]) * (point[l][1] - point[j][1]) -
            (point[k][1] - point[j][1]) * (point[l][0] - point[j][0]);
            
        /* compute (2 *area)^2 for this face */
        facearea2[i] = facenormal[i][0] * facenormal[i][0] +
            facenormal[i][1] * facenormal[i][1] +
            facenormal[i][2] * facenormal[i][2];
        
        /* compute edge lengths (squared) */
        for (j = i + 1; j < 4; j++) {
            dx = point[i][0] - point[j][0];
            dy = point[i][1] - point[j][1];
            dz = point[i][2] - point[j][2];
            edgelength[i][j] = dx * dx + dy * dy + dz * dz;
        }
    }
    
    /* for each edge in the tetrahedron */
    for (i = 0; i < 3; i++) {
        for (j = i + 1; j < 4; j++) {
            k = (i > 0) ? 0 : (j > 1) ? 1 : 2;
            l = 6 - i - j - k;
            
            /* compute the expression for minimum sine, squared, over 4 
               The reason it's over 4 is because the area values we have
               are actually twice the area squared */
            /* if either face area is zero, the sine is zero */
            if (facearea2[k] > 0 && facearea2[l] > 0)
            {
                sine2 = edgelength[i][j] / (facearea2[k] * facearea2[l]);
            }
            else
            {
                if (improvebehave.verbosity > 5) 
                {
                    printf("Encountered zero-area face.\n");
                    printf("Here is the tet (%d %d %d %d):\n", (int) vtx1, (int) vtx2, (int) vtx3, (int) vtx4);
                    printf(",\n");
                }
                sine2 = 0.0;
            }
            
            /* accumulate sine */
            sinesum += sqrt(sine2);
        }
    }
    
    /* average sine */
    return (sinesum / 6.0) * pyrvolume;
}

/* compute Z, a quantity associated with circumradius computation
   TODO this code is lifted from Jonathan's tetcircumcenter computation
   in primitives.c */
starreal getZ(starreal *tetorg,
              starreal *tetdest,
              starreal *tetfapex,
              starreal *tettapex)
{
    starreal xot, yot, zot, xdt, ydt, zdt, xft, yft, zft;
    starreal otlength, dtlength, ftlength;
    starreal xcrossdf, ycrossdf, zcrossdf;
    starreal xcrossfo, ycrossfo, zcrossfo;
    starreal xcrossod, ycrossod, zcrossod;
    starreal xct, yct, zct;

    /* Use coordinates relative to the apex of the tetrahedron. */
    xot = tetorg[0] - tettapex[0];
    yot = tetorg[1] - tettapex[1];
    zot = tetorg[2] - tettapex[2];
    xdt = tetdest[0] - tettapex[0];
    ydt = tetdest[1] - tettapex[1];
    zdt = tetdest[2] - tettapex[2];
    xft = tetfapex[0] - tettapex[0];
    yft = tetfapex[1] - tettapex[1];
    zft = tetfapex[2] - tettapex[2];
    /* Squares of lengths of the origin, destination, and face apex edges. */
    otlength = xot * xot + yot * yot + zot * zot;
    dtlength = xdt * xdt + ydt * ydt + zdt * zdt;
    ftlength = xft * xft + yft * yft + zft * zft;
    /* Cross products of the origin, destination, and face apex vectors. */
    xcrossdf = ydt * zft - yft * zdt;
    ycrossdf = zdt * xft - zft * xdt;
    zcrossdf = xdt * yft - xft * ydt;
    xcrossfo = yft * zot - yot * zft;
    ycrossfo = zft * xot - zot * xft;
    zcrossfo = xft * yot - xot * yft;
    xcrossod = yot * zdt - ydt * zot;
    ycrossod = zot * xdt - zdt * xot;
    zcrossod = xot * ydt - xdt * yot;
    
    /* Calculate offset (from apex) of circumcenter. */
    xct = (otlength * xcrossdf + dtlength * xcrossfo + ftlength * xcrossod);
    yct = (otlength * ycrossdf + dtlength * ycrossfo + ftlength * ycrossod);
    zct = (otlength * zcrossdf + dtlength * zcrossfo + ftlength * zcrossod);
        
    /* Calculate the length of this vector, which is Z */
    return sqrt(xct * xct + yct * yct + zct * zct);
}

/* the inradius to circumradius ratio */
starreal radiusratio(struct tetcomplex *mesh,
                     tag vtx1,
                     tag vtx2,
                     tag vtx3,
                     tag vtx4)
{
    starreal point[4][3];      /* the vertices of the tet */
    starreal facenormal[4][3]; /* the normals of each face of the tet */
    starreal facearea2[4];     /* areas of the faces of the tet */
    starreal pyrvolume;        /* volume of tetrahedron */
    starreal Z;                /* quantity needed for circumradius */
    starreal facesum=0.0;       /* sum of the areas of the faces */
    int i, j, k, l;          /* loop indices */
    starreal sign;
    starreal qual;
    starreal E[3][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
    
    /* get tet vertices */
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx1))->coord, point[0]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx2))->coord, point[1]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx3))->coord, point[2]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx4))->coord, point[3]);
    
    /* if anisotropic meshing is enabled, warp the points according the
       deformation tensor at their barycenter */
    if (improvebehave.anisotropic)
    {
        /* fetch the deformation tensor at the barycenter of this tet */
        tettensor(mesh, vtx1, vtx2, vtx3, vtx4, E);
        
        /* transform each vertex */
        for (i=0; i<4; i++)
        {
            if (improvebehave.verbosity > 5)
            {
                printf("in radiusratio transforming point (%g %g %g) ->", point[i][0], point[i][1], point[i][2]);
            }
            
            tensortransform(point[i], point[i], E);
            
            if (improvebehave.verbosity > 5)
            {
                printf(" (%g %g %g)\n", point[i][0], point[i][1], point[i][2]);
            }
        }
    }
    
    /* calculate the volume*6 of the tetrahedron */
    pyrvolume = (starreal) orient3d(&behave, point[0], point[1], point[2], point[3]);
    
    /* if the volume is zero, the quality is zero, no reason to continue */
    if (pyrvolume == 0.0)
    {
        return 0.0;
    }
    
    /* for each vertex/face of the tetrahedron */
    for (i = 0; i < 4; i++) {
        j = (i + 1) & 3;
        if ((i & 1) == 0) {
            k = (i + 3) & 3;
            l = (i + 2) & 3;
        } else {
            k = (i + 2) & 3;
            l = (i + 3) & 3;
        }
        
        /* compute the normal for each face */
        facenormal[i][0] =
            (point[k][1] - point[j][1]) * (point[l][2] - point[j][2]) -
            (point[k][2] - point[j][2]) * (point[l][1] - point[j][1]);
        facenormal[i][1] =
            (point[k][2] - point[j][2]) * (point[l][0] - point[j][0]) -
            (point[k][0] - point[j][0]) * (point[l][2] - point[j][2]);
        facenormal[i][2] =
            (point[k][0] - point[j][0]) * (point[l][1] - point[j][1]) -
            (point[k][1] - point[j][1]) * (point[l][0] - point[j][0]);
            
        /* compute (2 *area)^2 for this face */
        facearea2[i] = facenormal[i][0] * facenormal[i][0] +
            facenormal[i][1] * facenormal[i][1] +
            facenormal[i][2] * facenormal[i][2];
        facesum += sqrt(facearea2[i]) * 0.5;
    }
    
    /* compute Z */
    Z = getZ(point[0], point[1], point[2], point[3]);
    
    /* now we are ready to compute the radius ratio, which is
    
       (108 * V^2) / Z (A1 + A2 + A3 + A4)
    
       (use 3 instead of 108 because pyrvolume = 6V)
    */
    /* use sqrt for now... */
    sign = (pyrvolume < 0.0) ? -1.0 : 1.0;
    
    qual = sign * sqrt((3.0 * pyrvolume * pyrvolume) / (Z * facesum));
    
    if (improvebehave.verbosity > 6)
    {
        printf("the volume of this tet is %g\n", pyrvolume);
        
        if (qual > 0.0 && pyrvolume < 0.0)
        {
            printf("aha! volume is negative but qual is positive!\n");
            printf("here is the tet:\n");
            printtetvertssep(mesh, vtx1, vtx2, vtx3, vtx4);
            starexit(1);
        }
    }
    
    return qual;
}

/* compute the ratio of the tet volume to the cube of
   the rms edge length */
starreal vlrms3ratio(struct tetcomplex *mesh,
                   tag vtx1,
                   tag vtx2,
                   tag vtx3,
                   tag vtx4)
{
    starreal point[4][3];      /* the vertices of the tet */
    starreal edgelength[3][4]; /* the lengths of each of the edges of the tet */
    starreal dx, dy, dz;       /* intermediate values of edge lengths */
    starreal pyrvolume;        /* volume of tetrahedron */
    int i, j, k, l;          /* loop indices */
    starreal edgelengthsum = 0.0;
    starreal lrms;             /* root mean squared of edge length */
    starreal qual;
    starreal E[3][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
    
    /* get tet vertices */
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx1))->coord, point[0]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx2))->coord, point[1]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx3))->coord, point[2]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vtx4))->coord, point[3]);
    
    /* if anisotropic meshing is enabled, warp the points according the
       deformation tensor at their barycenter */
    if (improvebehave.anisotropic)
    {
        /* fetch the deformation tensor at the barycenter of this tet */
        tettensor(mesh, vtx1, vtx2, vtx3, vtx4, E);
        
        /* transform each vertex */
        for (i=0; i<4; i++)
        {
            if (improvebehave.verbosity > 5)
            {
                printf("in vlrms3 transforming point (%g %g %g) ->", point[i][0], point[i][1], point[i][2]);
            }
            
            tensortransform(point[i], point[i], E);
            
            if (improvebehave.verbosity > 5)
            {
                printf(" (%g %g %g)\n", point[i][0], point[i][1], point[i][2]);
            }
        }
    }
    
    /* calculate the volume*6 of the tetrahedron */
    pyrvolume = (starreal) orient3d(&behave, point[0], point[1], point[2], point[3]);
    
    /* if the volume is zero, the quality is zero, no reason to continue */
    if (pyrvolume == 0.0)
    {
        return 0.0;
    }
    
    /* for each vertex/face of the tetrahedron */
    for (i = 0; i < 4; i++) {
        j = (i + 1) & 3;
        if ((i & 1) == 0) {
            k = (i + 3) & 3;
            l = (i + 2) & 3;
        } else {
            k = (i + 2) & 3;
            l = (i + 3) & 3;
        }
        
        /* compute edge lengths (squared) */
        for (j = i + 1; j < 4; j++) {
            dx = point[i][0] - point[j][0];
            dy = point[i][1] - point[j][1];
            dz = point[i][2] - point[j][2];
            edgelength[i][j] = dx * dx + dy * dy + dz * dz;
        }
    }
    
    /* for each edge in the tetrahedron */
    for (i = 0; i < 3; i++) {
        for (j = i + 1; j < 4; j++) {
            k = (i > 0) ? 0 : (j > 1) ? 1 : 2;
            l = 6 - i - j - k;
            
            edgelengthsum += edgelength[i][j];
        }
    }
    
    /* compute the root mean square */
    lrms = sqrt((1.0 / 6.0) * edgelengthsum);
    
    /* compute the normalized ratio of volume to lrms^3 */
    qual = (sqrt(2.0) * pyrvolume) / (lrms * lrms * lrms);
    
    return qual;
}

/* harness function for all quality measures */
starreal tetquality(struct tetcomplex *mesh, 
                  tag vtx1, 
                  tag vtx2,
                  tag vtx3,
                  tag vtx4,
                  int measure)
{
    starreal quality = 0.0; /* the quality of this tetrahedron */
    
    switch (measure)
    {
        case QUALMINSINE:
            quality = minsine(mesh, vtx1, vtx2, vtx3, vtx4);
            break;
        case QUALMEANSINE:
            quality = meansine(mesh, vtx1, vtx2, vtx3, vtx4);
            break;
        case QUALMINSINEANDEDGERATIO:
            quality = minsineandedgeratio(mesh, vtx1, vtx2, vtx3, vtx4);
            break;
        case QUALRADIUSRATIO:
            quality = radiusratio(mesh, vtx1, vtx2, vtx3, vtx4);
            break;
        case QUALVLRMS3RATIO:
            quality = vlrms3ratio(mesh, vtx1, vtx2, vtx3, vtx4);
            break;
        case QUALWARPEDMINSINE:
            quality = warpedminsine(mesh, vtx1, vtx2, vtx3, vtx4);
            break;
        case QUALMINANGLE:
            quality = minmaxangle(mesh, vtx1, vtx2, vtx3, vtx4, false);
            break;
        case QUALMAXANGLE:
            quality = minmaxangle(mesh, vtx1, vtx2, vtx3, vtx4, true);
            break;
        default:
            printf("I don't know what quality measure %d is. Dying...\n", measure);
            starexit(1);
    }
    
    return quality;
}


/********* Statistics printing routines begin here                   *********/
/**                                                                         **/
/**                                                                         **/

void statisticsqualitystream(FILE *o,
                             struct behavior *behave,
                             struct tetcomplex *plex,
                             bool anisotropic)
{
#define BINS 90                                         /* Must be even!!! */
#define RATIOBINS 100

  struct tetcomplexposition position;
  tag tet[4];
  starreal point[4][3];
  starreal tcenter[3];
  starreal tansquaretable[8];
  starreal tansquaretable2[BINS / 2 - 1];
  starreal aspecttable[16];
  starreal circumtable[16];
  starreal edgelength[3][4];
  starreal facenormal[4][3];
  starreal dx, dy, dz;
  starreal dotproduct;
  starreal tansquare;
  starreal pyrvolume;
  starreal facearea2;
  starreal pyrbiggestface2;
  starreal shortest, longest;
  starreal pyrshortest2, pyrlongest2;
  starreal smallestvolume, biggestvolume;
  starreal pyrminaltitude2;
  starreal minaltitude;
  starreal pyraspect2;
  starreal worstaspect;
  starreal pyrcircumratio2;
  starreal worstcircumratio;
  starreal smallestangle, biggestangle;
  starreal radconst, degconst;
  arraypoolulong anglecount[18];
  arraypoolulong anglecount2[BINS];
  arraypoolulong rnrrratiocount[RATIOBINS];
  arraypoolulong vlrms3ratiocount[RATIOBINS];
  arraypoolulong aspectcount[16];
  arraypoolulong circumcount[16];
  int aspectindex;
  int circumindex;
  int tendegree;
  int bindegree;
  int acutebiggestflag;
  int firstiterationflag;
  int i, ii, j, k, l;
  starreal rnrrratioqual, vlrms3ratioqual;
  int rnrrwhichbin, vlrms3whichbin;
  starreal rnrrsmallestratio = HUGEFLOAT;
  starreal vlrms3smallestratio = HUGEFLOAT;
  starreal rnrrlargestratio = 0.0;
  starreal vlrms3largestratio = 0.0;
  starreal E[3][3];
  
  starreal worstqual = HUGEFLOAT;
  starreal thisqual;
  
  radconst = PI / 18.0;
  degconst = 180.0 / PI;
  for (i = 0; i < 8; i++) {
    tansquaretable[i] = tan(radconst * (starreal) (i + 1));
    tansquaretable[i] = tansquaretable[i] * tansquaretable[i];
  }
  radconst = PI / (starreal) BINS;
  for (i = 0; i < BINS / 2 - 1; i++) {
    tansquaretable2[i] = tan(radconst * (starreal) (i + 1));
    tansquaretable2[i] = tansquaretable2[i] * tansquaretable2[i] - 0.00000001;
  }
  for (i = 0; i < 18; i++) {
    anglecount[i] = 0;
  }
  for (i = 0; i < BINS; i++) {
    anglecount2[i] = 0;
  }
  for (i = 0; i < RATIOBINS; i++)
  {
      rnrrratiocount[i] = 0;
      vlrms3ratiocount[i] = 0;
  }

  aspecttable[0]  =      1.5;      aspecttable[1]  =     2.0;
  aspecttable[2]  =      2.5;      aspecttable[3]  =     3.0;
  aspecttable[4]  =      4.0;      aspecttable[5]  =     6.0;
  aspecttable[6]  =     10.0;      aspecttable[7]  =    15.0;
  aspecttable[8]  =     25.0;      aspecttable[9]  =    50.0;
  aspecttable[10] =    100.0;      aspecttable[11] =   300.0;
  aspecttable[12] =   1000.0;      aspecttable[13] = 10000.0;
  aspecttable[14] = 100000.0;      aspecttable[15] =     0.0;
  for (i = 0; i < 16; i++) {
    aspectcount[i] = 0;
  }

  circumtable[0]  =      0.75;     circumtable[1]  =     1.0;
  circumtable[2]  =      1.5;      circumtable[3]  =     2.0;
  circumtable[4]  =      3.0;      circumtable[5]  =     5.0;
  circumtable[6]  =     10.0;      circumtable[7]  =    15.0;
  circumtable[8]  =     25.0;      circumtable[9]  =    50.0;
  circumtable[10] =    100.0;      circumtable[11] =   300.0;
  circumtable[12] =   1000.0;      circumtable[13] = 10000.0;
  circumtable[14] = 100000.0;      circumtable[15] =     0.0;
  for (i = 0; i < 16; i++) {
    circumcount[i] = 0;
  }

  shortest = 0.0;
  longest = 0.0;
  smallestvolume = 0.0;
  biggestvolume = 0.0;
  minaltitude = 0.0;
  worstaspect = 0.0;
  worstcircumratio = 0.0;
  smallestangle = 100.0;
  biggestangle = 0.0;
  acutebiggestflag = 1;

  firstiterationflag = 1;
  tetcomplexiteratorinit(plex, &position);
  tetcomplexiteratenoghosts(&position, tet);
  while (tet[0] != STOP) {
    
    /* ratio bin stuff */
    rnrrratioqual = tetquality(plex, tet[0], tet[1], tet[2], tet[3], QUALRADIUSRATIO);
    vlrms3ratioqual = tetquality(plex, tet[0], tet[1], tet[2], tet[3], QUALVLRMS3RATIO);
    rnrrratioqual = rnrrratioqual * rnrrratioqual;
    
    if (rnrrratioqual > rnrrlargestratio) rnrrlargestratio = rnrrratioqual;
    if (rnrrratioqual < rnrrsmallestratio) rnrrsmallestratio = rnrrratioqual;
    if (vlrms3ratioqual > vlrms3largestratio) vlrms3largestratio = vlrms3ratioqual;
    if (vlrms3ratioqual < vlrms3smallestratio) vlrms3smallestratio = vlrms3ratioqual;
    rnrrwhichbin = (int) (((starreal) RATIOBINS) * rnrrratioqual);
    vlrms3whichbin = (int) (((starreal) RATIOBINS) * vlrms3ratioqual);
    assert(rnrrwhichbin <= RATIOBINS);
    rnrrratiocount[rnrrwhichbin]++;
    assert(vlrms3whichbin <= RATIOBINS);
    vlrms3ratiocount[vlrms3whichbin]++;
     
    vcopy(((struct vertex *) tetcomplextag2vertex(plex, tet[0]))->coord, point[0]);
    vcopy(((struct vertex *) tetcomplextag2vertex(plex, tet[1]))->coord, point[1]);
    vcopy(((struct vertex *) tetcomplextag2vertex(plex, tet[2]))->coord, point[2]);
    vcopy(((struct vertex *) tetcomplextag2vertex(plex, tet[3]))->coord, point[3]);
    
    /* if anisotropic meshing is enabled, warp the points according the
       deformation tensor at their barycenter */
    if (anisotropic)
    {
        /* fetch the deformation tensor at the barycenter of this tet */
        tettensor(plex, tet[0], tet[1], tet[2], tet[3], E);
        
        /* transform each vertex */
        for (i=0; i<4; i++)
        {
            tensortransform(point[i], point[i], E);
        }
    }
    
    /* compute quality */
    thisqual = tetquality(plex, tet[0], tet[1], tet[2], tet[3], improvebehave.qualmeasure);
    if (thisqual < worstqual) worstqual = thisqual;
    
    pyrshortest2 = 0.0;
    pyrlongest2 = 0.0;
    pyrbiggestface2 = 0.0;

    for (i = 0; i < 4; i++) {
      j = (i + 1) & 3;
      if ((i & 1) == 0) {
        k = (i + 3) & 3;
        l = (i + 2) & 3;
      } else {
        k = (i + 2) & 3;
        l = (i + 3) & 3;
      }

      facenormal[i][0] =
        (point[k][1] - point[j][1]) * (point[l][2] - point[j][2]) -
        (point[k][2] - point[j][2]) * (point[l][1] - point[j][1]);
      facenormal[i][1] =
        (point[k][2] - point[j][2]) * (point[l][0] - point[j][0]) -
        (point[k][0] - point[j][0]) * (point[l][2] - point[j][2]);
      facenormal[i][2] =
        (point[k][0] - point[j][0]) * (point[l][1] - point[j][1]) -
        (point[k][1] - point[j][1]) * (point[l][0] - point[j][0]);
      facearea2 = facenormal[i][0] * facenormal[i][0] +
                  facenormal[i][1] * facenormal[i][1] +
                  facenormal[i][2] * facenormal[i][2];
      if (facearea2 > pyrbiggestface2) {
        pyrbiggestface2 = facearea2;
      }

      for (j = i + 1; j < 4; j++) {
        dx = point[i][0] - point[j][0];
        dy = point[i][1] - point[j][1];
        dz = point[i][2] - point[j][2];
        edgelength[i][j] = dx * dx + dy * dy + dz * dz;
        if (edgelength[i][j] > pyrlongest2) {
          pyrlongest2 = edgelength[i][j];
        }
        if ((j == 1) || (edgelength[i][j] < pyrshortest2)) {
          pyrshortest2 = edgelength[i][j];
        }
      }
    }
    if (pyrlongest2 > longest) {
      longest = pyrlongest2;
    }
    if ((pyrshortest2 < shortest) || firstiterationflag) {
      shortest = pyrshortest2;
    }

    pyrvolume = (starreal)
      orient3d(behave, point[0], point[1], point[2], point[3]);
    if ((pyrvolume < smallestvolume) || firstiterationflag) {
      smallestvolume = pyrvolume;
    }
    if (pyrvolume > biggestvolume) {
      biggestvolume = pyrvolume;
    }
    pyrminaltitude2 = pyrvolume * pyrvolume / pyrbiggestface2;
    if ((pyrminaltitude2 < minaltitude) || firstiterationflag) {
      minaltitude = pyrminaltitude2;
    }
    pyraspect2 = pyrlongest2 / pyrminaltitude2;
    if (pyraspect2 > worstaspect) {
      worstaspect = pyraspect2;
    }
    aspectindex = 0;
    while ((pyraspect2 >
            aspecttable[aspectindex] * aspecttable[aspectindex]) &&
           (aspectindex < 15)) {
      aspectindex++;
    }
    aspectcount[aspectindex]++;

    tetcircumcenter(behave, point[0], point[1], point[2], point[3], tcenter,
                    (starreal *) NULL, (starreal *) NULL, (starreal *) NULL);
    pyrcircumratio2 = (tcenter[0] * tcenter[0] + tcenter[1] * tcenter[1] +
                       tcenter[2] * tcenter[2]) / pyrshortest2;
    if (pyrcircumratio2 > worstcircumratio) {
      worstcircumratio = pyrcircumratio2;
    }
    circumindex = 0;
    while ((pyrcircumratio2 >
            circumtable[circumindex] * circumtable[circumindex]) &&
           (circumindex < 15)) {
      circumindex++;
    }
    circumcount[circumindex]++;

    for (i = 0; i < 3; i++) {
      for (j = i + 1; j < 4; j++) {
        k = (i > 0) ? 0 : (j > 1) ? 1 : 2;
        l = 6 - i - j - k;
        
        dotproduct = facenormal[i][0] * facenormal[j][0] +
                     facenormal[i][1] * facenormal[j][1] +
                     facenormal[i][2] * facenormal[j][2];
        if (dotproduct == 0.0) {
          anglecount[9]++;
          anglecount2[BINS / 2]++;
          if (acutebiggestflag) {
            biggestangle = 1.0e+300;
            acutebiggestflag = 0;
          }
        } else {
          tansquare = pyrvolume * pyrvolume * edgelength[k][l] /
                      (dotproduct * dotproduct);
          tendegree = 8;
          bindegree = BINS / 2 - 1;
          for (ii = 7; ii >= 0; ii--) {
            if (tansquare < tansquaretable[ii]) {
              tendegree = ii;
            }
          }
          for (ii = BINS / 2 - 2; ii >= 0; ii--) {
            if (tansquare < tansquaretable2[ii]) {
              bindegree = ii;
            }
          }
          if (dotproduct < 0.0) {
            anglecount[tendegree]++;
            if (tansquare > 100000000.0) {
              bindegree = BINS / 2;
            }
            anglecount2[bindegree]++;
            if (tansquare < smallestangle) {
              smallestangle = tansquare;
            }
            if (acutebiggestflag && (tansquare > biggestangle)) {
              biggestangle = tansquare;
            }
          } else {
            anglecount[17 - tendegree]++;
            anglecount2[BINS - 1 - bindegree]++;
            if (acutebiggestflag || (tansquare < biggestangle)) {
              biggestangle = tansquare;
              acutebiggestflag = 0;
            }
          }
        }
      }
    }

    tetcomplexiteratenoghosts(&position, tet);
    firstiterationflag = 0;
  }

  shortest = sqrt(shortest);
  longest = sqrt(longest);
  minaltitude = sqrt(minaltitude);
  worstaspect = sqrt(worstaspect);
  worstcircumratio = sqrt(worstcircumratio);
  smallestvolume /= 6.0;
  biggestvolume /= 6.0;
  smallestangle = degconst * atan(sqrt(smallestangle));
  if (acutebiggestflag) {
    biggestangle = degconst * atan(sqrt(biggestangle));
  } else {
    biggestangle = 180.0 - degconst * atan(sqrt(biggestangle));
  }
  
  if (anisotropic)
  {
      fprintf(o,"Mesh quality statistics (ISOTROPIC SPACE)\n\n");
  }
  else
  {
      fprintf(o,"Mesh quality statistics (PHYSICAL SPACE)\n\n");
  }
  
  fprintf(o,"  Smallest volume: %14.5g   |  Largest volume: %14.5g\n",
         smallestvolume, biggestvolume);
  fprintf(o,"  Shortest edge: %16.5g   |  Longest edge: %16.5g\n",
         shortest, longest);
  fprintf(o,"  Shortest altitude: %12.5g   |  Largest aspect ratio: %8.5g\n\n",
         minaltitude, worstaspect);

  fprintf(o,"  Tetrahedron aspect ratio histogram:\n");
  fprintf(o,"  1.2247 - %-6.6g    :   %8lu   | %6.6g - %-6.6g     :  %8lu\n",
         aspecttable[0], (unsigned long) aspectcount[0],
         aspecttable[7], aspecttable[8], (unsigned long) aspectcount[8]);
  for (i = 1; i < 7; i++) {
    fprintf(o,"  %6.6g - %-6.6g    :   %8lu   | %6.6g - %-6.6g     :  %8lu\n",
           aspecttable[i - 1], aspecttable[i], (unsigned long) aspectcount[i],
           aspecttable[i + 7], aspecttable[i + 8],
           (unsigned long) aspectcount[i + 8]);
  }
  fprintf(o,"  %6.6g - %-6.6g    :   %8lu   | %6.6g -            :  %8lu\n",
         aspecttable[6], aspecttable[7], (unsigned long) aspectcount[7],
         aspecttable[14], (unsigned long) aspectcount[15]);
  fprintf(o,"  (Aspect ratio is longest edge divided by shortest altitude)\n");

  fprintf(o,"\n  Largest circumradius-to-shortest-edge ratio: %22.5g\n\n",
         worstcircumratio);

  fprintf(o,"  Tetrahedron circumradius-to-shortest-edge ratio histogram:\n");
  fprintf(o,"  0.6123 - %-6.6g    :   %8lu   | %6.6g - %-6.6g     :  %8lu\n",
         circumtable[0], (unsigned long) circumcount[0],
         circumtable[7], circumtable[8], (unsigned long) circumcount[8]);
  for (i = 1; i < 7; i++) {
    fprintf(o,"  %6.6g - %-6.6g    :   %8lu   | %6.6g - %-6.6g     :  %8lu\n",
           circumtable[i - 1], circumtable[i], (unsigned long) circumcount[i],
           circumtable[i + 7], circumtable[i + 8],
           (unsigned long) circumcount[i + 8]);
  }
  fprintf(o,"  %6.6g - %-6.6g    :   %8lu   | %6.6g -            :  %8lu\n",
         circumtable[6], circumtable[7], (unsigned long) circumcount[7],
         circumtable[14], (unsigned long) circumcount[15]);

  fprintf(o,"\n  Smallest dihedral: %12.5g   |  Largest dihedral: %12.5g\n\n",
         smallestangle, biggestangle);

  fprintf(o,"  Dihedral angle histogram:\n");
  for (i = 0; i < 9; i++) {
    fprintf(o,"      %2d - %2d degrees:   %8lu   |    %3d - %3d degrees:  %8lu\n",
           i * 10, i * 10 + 10, (unsigned long) anglecount[i],
           i * 10 + 90, i * 10 + 100, (unsigned long) anglecount[i + 9]);
  }
  fprintf(o,"\n");

  fprintf(o,"  For a histogram, input the following line to hist.c:\n  ");
  for (i = 0; i < BINS; i++) {
    fprintf(o,"%lu ", anglecount2[i]);
  }
  fprintf(o,"%12.7g %12.7g\n\n", smallestangle, biggestangle);
  
  fprintf(o,"  For a rnrrratio histogram, input the following line to hist.c:\n  ");
  for (i = 0; i < RATIOBINS; i++) {
    fprintf(o,"%lu ", rnrrratiocount[i]);
  }
  fprintf(o,"%12.7g %12.7g\n\n", rnrrsmallestratio, rnrrlargestratio);
  
  fprintf(o,"  For a vlrms3ratio histogram, input the following line to hist.c:\n  ");
  for (i = 0; i < RATIOBINS; i++) {
    fprintf(o,"%lu ", vlrms3ratiocount[i]);
  }
  fprintf(o,"%12.7g %12.7g\n\n", vlrms3smallestratio, vlrms3largestratio);
  
  fprintf(o, "worstqual: %12.7g\n\n", worstqual);
}

void improvestatistics(struct behavior *behave,
                       struct tetcomplex *plex,
                       bool anisotropic)
{
    statisticsqualitystream(stdout, behave, plex, anisotropic);
}

void getextremeangles(struct behavior *behave,
                      struct tetcomplex *plex,
                      starreal *outsmallestangle,
                      starreal *outbiggestangle)
{
  struct tetcomplexposition position;
  tag tet[4];
  starreal point[4][3];
  starreal tcenter[3];
  starreal tansquaretable[8];
  starreal aspecttable[16];
  starreal circumtable[16];
  starreal edgelength[3][4];
  starreal facenormal[4][3];
  starreal dx, dy, dz;
  starreal dotproduct;
  starreal tansquare;
  starreal pyrvolume;
  starreal facearea2;
  starreal pyrbiggestface2;
  starreal shortest, longest;
  starreal pyrshortest2, pyrlongest2;
  starreal smallestvolume, biggestvolume;
  starreal pyrminaltitude2;
  starreal minaltitude;
  starreal pyraspect2;
  starreal worstaspect;
  starreal pyrcircumratio2;
  starreal worstcircumratio;
  starreal smallestangle, biggestangle;
  starreal radconst, degconst;
  arraypoolulong anglecount[18];
  arraypoolulong aspectcount[16];
  arraypoolulong circumcount[16];
  int aspectindex;
  int circumindex;
  int tendegree;
  int acutebiggestflag;
  int firstiterationflag;
  int i, ii, j, k, l;
  starreal E[3][3];

  radconst = PI / 18.0;
  degconst = 180.0 / PI;
  for (i = 0; i < 8; i++) {
    tansquaretable[i] = tan(radconst * (starreal) (i + 1));
    tansquaretable[i] = tansquaretable[i] * tansquaretable[i];
  }
  for (i = 0; i < 18; i++) {
    anglecount[i] = 0;
  }

  aspecttable[0]  =      1.5;      aspecttable[1]  =     2.0;
  aspecttable[2]  =      2.5;      aspecttable[3]  =     3.0;
  aspecttable[4]  =      4.0;      aspecttable[5]  =     6.0;
  aspecttable[6]  =     10.0;      aspecttable[7]  =    15.0;
  aspecttable[8]  =     25.0;      aspecttable[9]  =    50.0;
  aspecttable[10] =    100.0;      aspecttable[11] =   300.0;
  aspecttable[12] =   1000.0;      aspecttable[13] = 10000.0;
  aspecttable[14] = 100000.0;      aspecttable[15] =     0.0;
  for (i = 0; i < 16; i++) {
    aspectcount[i] = 0;
  }

  circumtable[0]  =      0.75;     circumtable[1]  =     1.0;
  circumtable[2]  =      1.5;      circumtable[3]  =     2.0;
  circumtable[4]  =      3.0;      circumtable[5]  =     5.0;
  circumtable[6]  =     10.0;      circumtable[7]  =    15.0;
  circumtable[8]  =     25.0;      circumtable[9]  =    50.0;
  circumtable[10] =    100.0;      circumtable[11] =   300.0;
  circumtable[12] =   1000.0;      circumtable[13] = 10000.0;
  circumtable[14] = 100000.0;      circumtable[15] =     0.0;
  for (i = 0; i < 16; i++) {
    circumcount[i] = 0;
  }

  shortest = 0.0;
  longest = 0.0;
  smallestvolume = 0.0;
  biggestvolume = 0.0;
  minaltitude = 0.0;
  worstaspect = 0.0;
  worstcircumratio = 0.0;
  smallestangle = 100.0;
  biggestangle = 0.0;
  acutebiggestflag = 1;

  firstiterationflag = 1;
  tetcomplexiteratorinit(plex, &position);
  tetcomplexiteratenoghosts(&position, tet);
  while (tet[0] != STOP) {
    
      vcopy(((struct vertex *) tetcomplextag2vertex(plex, tet[0]))->coord, point[0]);
      vcopy(((struct vertex *) tetcomplextag2vertex(plex, tet[1]))->coord, point[1]);
      vcopy(((struct vertex *) tetcomplextag2vertex(plex, tet[2]))->coord, point[2]);
      vcopy(((struct vertex *) tetcomplextag2vertex(plex, tet[3]))->coord, point[3]);

      /* if anisotropic meshing is enabled, warp the points according the
         deformation tensor at their barycenter */
      if (improvebehave.anisotropic)
      {
          /* fetch the deformation tensor at the barycenter of this tet */
          tettensor(plex, tet[0], tet[1], tet[2], tet[3], E);

          /* transform each vertex */
          for (i=0; i<4; i++)
          {
              tensortransform(point[i], point[i], E);
          }
      }
    pyrshortest2 = 0.0;
    pyrlongest2 = 0.0;
    pyrbiggestface2 = 0.0;

    for (i = 0; i < 4; i++) {
      j = (i + 1) & 3;
      if ((i & 1) == 0) {
        k = (i + 3) & 3;
        l = (i + 2) & 3;
      } else {
        k = (i + 2) & 3;
        l = (i + 3) & 3;
      }

      facenormal[i][0] =
        (point[k][1] - point[j][1]) * (point[l][2] - point[j][2]) -
        (point[k][2] - point[j][2]) * (point[l][1] - point[j][1]);
      facenormal[i][1] =
        (point[k][2] - point[j][2]) * (point[l][0] - point[j][0]) -
        (point[k][0] - point[j][0]) * (point[l][2] - point[j][2]);
      facenormal[i][2] =
        (point[k][0] - point[j][0]) * (point[l][1] - point[j][1]) -
        (point[k][1] - point[j][1]) * (point[l][0] - point[j][0]);
      facearea2 = facenormal[i][0] * facenormal[i][0] +
                  facenormal[i][1] * facenormal[i][1] +
                  facenormal[i][2] * facenormal[i][2];
      if (facearea2 > pyrbiggestface2) {
        pyrbiggestface2 = facearea2;
      }

      for (j = i + 1; j < 4; j++) {
        dx = point[i][0] - point[j][0];
        dy = point[i][1] - point[j][1];
        dz = point[i][2] - point[j][2];
        edgelength[i][j] = dx * dx + dy * dy + dz * dz;
        if (edgelength[i][j] > pyrlongest2) {
          pyrlongest2 = edgelength[i][j];
        }
        if ((j == 1) || (edgelength[i][j] < pyrshortest2)) {
          pyrshortest2 = edgelength[i][j];
        }
      }
    }
    if (pyrlongest2 > longest) {
      longest = pyrlongest2;
    }
    if ((pyrshortest2 < shortest) || firstiterationflag) {
      shortest = pyrshortest2;
    }

    pyrvolume = (starreal)
      orient3d(behave, point[0], point[1], point[2], point[3]);
    if ((pyrvolume < smallestvolume) || firstiterationflag) {
      smallestvolume = pyrvolume;
    }
    if (pyrvolume > biggestvolume) {
      biggestvolume = pyrvolume;
    }
    pyrminaltitude2 = pyrvolume * pyrvolume / pyrbiggestface2;
    if ((pyrminaltitude2 < minaltitude) || firstiterationflag) {
      minaltitude = pyrminaltitude2;
    }
    pyraspect2 = pyrlongest2 / pyrminaltitude2;
    if (pyraspect2 > worstaspect) {
      worstaspect = pyraspect2;
    }
    aspectindex = 0;
    while ((pyraspect2 >
            aspecttable[aspectindex] * aspecttable[aspectindex]) &&
           (aspectindex < 15)) {
      aspectindex++;
    }
    aspectcount[aspectindex]++;

    tetcircumcenter(behave, point[0], point[1], point[2], point[3], tcenter,
                    (starreal *) NULL, (starreal *) NULL, (starreal *) NULL);
    pyrcircumratio2 = (tcenter[0] * tcenter[0] + tcenter[1] * tcenter[1] +
                       tcenter[2] * tcenter[2]) / pyrshortest2;
    if (pyrcircumratio2 > worstcircumratio) {
      worstcircumratio = pyrcircumratio2;
    }
    circumindex = 0;
    while ((pyrcircumratio2 >
            circumtable[circumindex] * circumtable[circumindex]) &&
           (circumindex < 15)) {
      circumindex++;
    }
    circumcount[circumindex]++;

    for (i = 0; i < 3; i++) {
      for (j = i + 1; j < 4; j++) {
        k = (i > 0) ? 0 : (j > 1) ? 1 : 2;
        l = 6 - i - j - k;
        dotproduct = facenormal[i][0] * facenormal[j][0] +
                     facenormal[i][1] * facenormal[j][1] +
                     facenormal[i][2] * facenormal[j][2];
        if (dotproduct == 0.0) {
          anglecount[9]++;
          if (acutebiggestflag)
          {
              biggestangle = 1.0e+300;
              acutebiggestflag = 0;
          }
        } else {
          tansquare = pyrvolume * pyrvolume * edgelength[k][l] /
                      (dotproduct * dotproduct);
          tendegree = 8;
          for (ii = 7; ii >= 0; ii--) {
            if (tansquare < tansquaretable[ii]) {
              tendegree = ii;
            }
          }
          if (dotproduct < 0.0) {
            anglecount[tendegree]++;
            if (tansquare < smallestangle) {
              smallestangle = tansquare;
            }
            if (acutebiggestflag && (tansquare > biggestangle)) {
              biggestangle = tansquare;
            }
          } else {
            anglecount[17 - tendegree]++;
            if (acutebiggestflag || (tansquare < biggestangle)) {
              biggestangle = tansquare;
              acutebiggestflag = 0;
            }
          }
        }
      }
    }

    tetcomplexiteratenoghosts(&position, tet);
    firstiterationflag = 0;
  }

  shortest = sqrt(shortest);
  longest = sqrt(longest);
  minaltitude = sqrt(minaltitude);
  worstaspect = sqrt(worstaspect);
  worstcircumratio = sqrt(worstcircumratio);
  smallestvolume /= 6.0;
  biggestvolume /= 6.0;
  smallestangle = degconst * atan(sqrt(smallestangle));
  if (acutebiggestflag) {
    biggestangle = degconst * atan(sqrt(biggestangle));
  } else {
    biggestangle = 180.0 - degconst * atan(sqrt(biggestangle));
  }

  *outsmallestangle = smallestangle;
  *outbiggestangle = biggestangle;
}

void setbeginqual(struct behavior *behave,
                  struct tetcomplex *plex)
{
    starreal smallestangle = 0.0;
    starreal biggestangle = HUGEFLOAT;
    
    /* fetch the values for these angles */
    getextremeangles(behave, plex, &smallestangle, &biggestangle);
    
    stats.startminangle = smallestangle;
    stats.startmaxangle = biggestangle;
}

/* given the two vertices of a known boundary edge,
   compute the angle between it's two incident
   boundary faces */
starreal getboundaryedgeangle(struct tetcomplex *mesh,
                            tag v1,
                            tag v2,
                            tag vleft,
                            tag vright)
{
    starreal point[4][3];      /* the actual positions of the four vertices*/
    starreal E[3][3];
    starreal e[3][3];          /* edges for normal computation */
    starreal norm[2][3];       /* face normals */
    starreal edgelength;       /* length of boundary edge */
    starreal volume;           /* volume of tet formed by four vertices */
    starreal dotprod;          /* the dot product of the two inward face normals */
    starreal tantheta;         /* tangent of the angle */
    starreal angle;            /* angle we are computing */
    
    /* make sure neither of these are GHOST vertices themselves */
    assert(vleft != GHOSTVERTEX);
    assert(vright != GHOSTVERTEX);
    
    /* fetch actual vertex values */
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, v1))->coord, point[0]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, v2))->coord, point[1]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vleft))->coord, point[2]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, vright))->coord, point[3]);
            
    /* if anisotropic meshing is enabled, warp the points according the
       deformation tensor at their barycenter */
    if (improvebehave.anisotropic)
    {
        /* fetch the deformation tensor at the barycenter of points */
        tettensor(mesh, v1, v2, vleft, vright, E);

        /* transform each vertex */
        tensortransform(point[0], point[0], E);
        tensortransform(point[1], point[1], E);
        tensortransform(point[2], point[2], E);
        tensortransform(point[3], point[3], E);
    }
    
    /* e[0] from v1 to v2 */
    vsub(point[1], point[0], e[0]);
    /* e[1] from v1 to right */
    vsub(point[2], point[0], e[1]);
    /* e[2] from v1 to left */
    vsub(point[3], point[0], e[2]);
    
    /* compute the length of the edge in question */
    edgelength = vlength(e[0]);
    
    /* compute face normals, pointing out of the mesh interior */
    cross(e[1], e[0], norm[0]);
    cross(e[0], e[2], norm[1]);
    
    /* compute 6 * volume of the tetrahedron with these four vertices */
    volume = (starreal) orient3d(&behave, point[0], point[1], point[2], point[3]);
    
    /* find the dot product of the two vectors */
    dotprod = dot(norm[0], norm[1]);
    
    /* if the dot product is zero, then the angle is +-90 degrees */
    if (dotprod == 0)
    {
        /* if the volume is negative, angle is 270 degrees */
        angle = (volume > 0) ? PI / 2.0 : (3.0 * PI) / 2.0;
    }
    else
    {
        /* compute the tangent of the angle using the tangent formula:
    
           tan(theta_ij) = - 6 * V * l_ij
                           --------------
                            dot(n_k, n_l)
                        
           because this formula is accurate in the entire range.
        */
        tantheta = (-volume * edgelength) / dotprod;
        
        /* now compute the actual angle */
        angle = atan(tantheta);
        
        if (improvebehave.verbosity > 5)
        {
            printf("Raw arctan answer was %g, volume is %g and dotprod is %g\n", angle, volume, dotprod);
        }
        
        /* adjust angle for sign of dot product */
        if (dotprod > 0)
        {
            angle += PI;
        }
        
        /* make negative angles positive */
        if (angle < 0)
        {
            angle += 2.0 * PI;
        }
        
        if (improvebehave.verbosity > 5)
        {
            printf("Final angle found is %g radians, or %g degrees\n", angle, angle * (180.0 / PI));
        }
        
        /* zero angles must actually be 360 degrees...?? */
        if (angle == 0)
        {
            if (improvebehave.verbosity > 5)
            {
                printf("correcting zero to 360...\n");
            }
            angle = 2.0 * PI;
        }
    }
    
    return angle;
}

/* given an input mesh, find the worst "input" angle.
   that is, find the smallest angle between two faces
   of the boundary */
starreal worstinputangle(struct tetcomplex *mesh)
{
    starreal angle;                     /* angle between two surface faces */
    starreal worstangle = 2.0 * PI;     /* worst input angle */
    starreal minqual, meanqual[NUMMEANTHRESHOLDS];         /* quality of the worst tet in the mesh */
    struct arraypoolstack tetstack;   /* stack of tets  */
    int numboundedges;                /* number of boundary edges for a tet */
    tag edgelist[6][2];               /* list of boundary edges for a tet */
    tag edgefaces[6][2];
    struct improvetet * stacktet;     /* tet we're pulling off the stack */
    int i;
    
    stackinit(&tetstack, sizeof(struct improvetet));
    
    /* fill the stack of tets with all tets in the mesh */
    fillstackqual(mesh, &tetstack, improvebehave.qualmeasure, HUGEFLOAT, meanqual, &minqual);
    
    /* go through each tet on the stack */
    while (tetstack.top != STACKEMPTY)
    {
        /* pull the top tet off the stack */
        stacktet = (struct improvetet *) stackpop(&tetstack);
        
        /* check for any boundary edges */
        numboundedges = boundedges(mesh,
                                   stacktet->verts[0], 
                                   stacktet->verts[1], 
                                   stacktet->verts[2], 
                                   stacktet->verts[3],
                                   edgelist,
                                   edgefaces,
                                   NULL,
                                   NULL);
        
        if (numboundedges != 0)
        {
            if (improvebehave.verbosity > 5)
            {
                printf("tet (%d %d %d %d) has %d bound edges:\n", (int) stacktet->verts[0], 
                                                                  (int) stacktet->verts[1], 
                                                                  (int) stacktet->verts[2], 
                                                                  (int) stacktet->verts[3],
                                                                  numboundedges);
            }
            /* for each boundary edge */
            for (i=0; i<numboundedges; i++)
            {
                if (improvebehave.verbosity > 5)
                {
                    printf("testing boundary edge (%d, %d)\n", (int) edgelist[i][0], (int) edgelist[i][1]);
                }
                
                /* compute the angle between the boundary faces */
                angle = getboundaryedgeangle(mesh, edgelist[i][0], edgelist[i][1], edgefaces[i][0], edgefaces[i][1]);
            
                /* if this angle is smaller than what we've seen, update */
                if (angle < worstangle)
                {
                    if (improvebehave.verbosity > 5)
                    {
                        printf("New worst angle of %g radians (%g degrees) found\n", angle, angle * (180.0 / PI));
                    }
                    worstangle = angle;
                }
            }
        }
    }
    
    /* free the stack of tets */
    stackdeinit(&tetstack);
    
    return worstangle; 
}

/* gather some information about the worst tets in the mesh */
/* according to the given quality measure, report information
   about all the tets within degreesfromworst of the worst tet */
void worsttetreport(struct tetcomplex *mesh,
                    int qualmeasure,
                    starreal degreesfromworst)
{
    starreal minqual, meanqual[NUMMEANTHRESHOLDS];            /* quality of the worst tet in the mesh */
    struct arraypoolstack tetstack;      /* stack of tets  */
    struct improvetet *stacktet;         /* current tet */
    starreal threshold;                    /* tets worse than this will be investigated */  
    int numbadtets=0;                    /* number of naughty tets */
    int numboundverts;                   /* number of boundary verts in the current tet */
    int numboundedges;
    int numboundfaces;
    int boundvertshist[5] = {0,0,0,0,0}; /* histogram of the number of boundary vertices */
    int boundfaceshist[5] = {0,0,0,0,0};
    int boundedgeshist[7] = {0,0,0,0,0,0,0};
    tag boundtags[4];                   /* boundary verts of a particular tet */
    bool bfaces;
    tag boundfacetags[4][3];
    tag boundedgetags[6][2];
    tag boundedgefaces[6][2];
    int i;
    int nbvworst=0, nbeworst=0, nbfworst=0;
    starreal worstseen = 100.0;
    tag worstverts[4] = {0,0,0,0};
    
    /* initialize the tet stack */
    stackinit(&tetstack, sizeof(struct improvetet));
    
    /* fill the stack of tets with all tets in the mesh */
    fillstackqual(mesh, &tetstack, qualmeasure, HUGEFLOAT, meanqual, &minqual);
    
    /* set the threshold below which we'll investigate this tet */
    threshold = minqual + sin(degreesfromworst * (PI / 180.0));
    
    /* now go through the stack collecting information */
    while (tetstack.top != STACKEMPTY)
    {
        /* pull the top tet off the stack */
        stacktet = (struct improvetet *) stackpop(&tetstack);
        
        /* don't go any further if this tet isn't one of the worst */
        if (stacktet->quality > threshold) continue;
        
        numbadtets++;
        
        /* fetch the number of boundary vertices for this tet */
        numboundverts = boundverts(mesh, 
                                   stacktet->verts[0],
                                   stacktet->verts[1],
                                   stacktet->verts[2],
                                   stacktet->verts[3],
                                   boundtags);
                                   
        numboundedges = boundedges(mesh, 
                                   stacktet->verts[0],
                                   stacktet->verts[1],
                                   stacktet->verts[2],
                                   stacktet->verts[3],
                                   boundedgetags,
                                   boundedgefaces,
                                   NULL,
                                   NULL);
                                  
        bfaces = boundfaces(mesh,
                            stacktet->verts[0],
                            stacktet->verts[1],
                            stacktet->verts[2],
                            stacktet->verts[3],
                            boundfacetags,
                            &numboundfaces);
                            
        if (stacktet->quality <= worstseen)
        {
            worstseen = stacktet->quality;
            worstverts[0] = stacktet->verts[0];
            worstverts[1] = stacktet->verts[1];
            worstverts[2] = stacktet->verts[2];
            worstverts[3] = stacktet->verts[3]; 
            nbvworst = numboundverts;
            nbeworst = numboundedges;
            nbfworst = numboundfaces;
        }
                                   
        boundvertshist[numboundverts]++;
        boundedgeshist[numboundedges]++;
        boundfaceshist[numboundfaces]++;
    }
    
    /* free the stack of tets */
    stackdeinit(&tetstack);
    
    /* print report */
    printf("\nWorst tet report:\n");
    printf("    Worst overall quality: %g\n", pq(minqual));
    printf("    Worst tet (%d, %d, %d, %d) had %d vertices, %d edges, and %d faces on boundary.\n",
                                                                            (int) worstverts[0],
                                                                            (int) worstverts[1],
                                                                            (int) worstverts[2],
                                                                            (int) worstverts[3],
                                                                            nbvworst,
                                                                            nbeworst,
                                                                            nbfworst);
    printf("    Here is the worst tet:\n");
    printtetvertssep(mesh, worstverts[0], worstverts[1], worstverts[2], worstverts[3]);
    printf("\n");
    printf("    Number of tets within %g degrees of worst: %d\n", degreesfromworst, numbadtets);
    printf("    Number of boundary vertices in these tets:\n");
    for (i=0; i<5; i++)
    {
        printf("        [%d]: %d\n", i, boundvertshist[i]);
    }
    printf("    Number of boundary edges in these tets:\n");
    for (i=0; i<7; i++)
    {
        printf("        [%d]: %d\n", i, boundedgeshist[i]);
    }
    printf("    Number of boundary faces in these tets:\n");
    for (i=0; i<5; i++)
    {
        printf("        [%d]: %d\n", i, boundfaceshist[i]);
    }
    printf("\n");
}

/* determine if there is threshold improvement in some mean */
bool localmeanimprove(starreal oldmeans[], starreal newmeans[], starreal threshold)
{
    int i;
    bool foundbetter = false;
    
    for (i=0; i<NUMMEANTHRESHOLDS; i++)
    {
        if (newmeans[i] > oldmeans[i])
        {
            /* see if it beats it by the required threshold */
            if (newmeans[i] - oldmeans[i] > threshold)
            {
                if (improvebehave.verbosity > 4)
                {
                    textcolor(BRIGHT, GREEN, BLACK);
                    printf("mean local improvement = %g, that's enough for success (needs %g)\n", newmeans[i] - oldmeans[i], threshold);
                    textcolor(RESET, WHITE, BLACK);
                }
                foundbetter = true;
            }
            else
            {
                if (improvebehave.verbosity > 4)
                {
                    printf("mean local improvement = %g, positive but not enough (needs %g)!\n", newmeans[i] - oldmeans[i], threshold);
                }
            }
        }
    }
    
    return foundbetter;
}








