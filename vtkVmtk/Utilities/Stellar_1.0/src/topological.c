/*****************************************************************************/
/*                                                                           */
/*  topological improvement functions                                        */
/*                                                                           */
/*****************************************************************************/

/* determine the number of reflex edges that a face has, to determine what sort
   of flips might be done to it */
int countreflex(struct tetcomplex *mesh,
                tag vtx1,
                tag vtx2,
                tag vtx3,
                int *numcoplanar,
                int coplanaredges[][2])
{
    tag adjacencies[2];      /* the tags for the top and bottom vertices */
    starreal *facepoints[3]; /* the vertices of the face */
    starreal *apexpoints[2]; /* the two apex points from the adjacency query */
    int foundtriangle;       /* was the queried face even in the mesh? */
    int reflexcount = 0;     /* the number of reflex edges detected */
    int i;                   /* loop counter */
    starreal orientation;      /* orientation of edges */
    /* tag toptet[4]; */     /* "top" tet (RHR around face points to its apex) */
    /* tag bottet[4]; */     /* "bottom" tet */
    *numcoplanar = 0;
    
    /* find the two tets that share this face */
    foundtriangle = tetcomplexadjacencies(mesh, vtx1, vtx2, vtx3, adjacencies);
    
    if (foundtriangle != 1)
    {
        printf("failed to find face %d %d %d in countreflex.\n", (int) vtx1, (int) vtx2, (int) vtx3);
    }
    
    /* make sure this triangle actually exists in the mesh */
    assert(foundtriangle == 1);
    
    /* check to see if one of the tets is a ghost tet */
    if (adjacencies[0] == GHOSTVERTEX || adjacencies[1] == GHOSTVERTEX)
    {
        return -1; /* don't try to handle this case yet */
    }
    
    /* get the vertices of the face and the apices */
    facepoints[0] = ((struct vertex *) tetcomplextag2vertex(mesh, vtx1))->coord;
    facepoints[1] = ((struct vertex *) tetcomplextag2vertex(mesh, vtx2))->coord;
    facepoints[2] = ((struct vertex *) tetcomplextag2vertex(mesh, vtx3))->coord;
    
    apexpoints[0] = ((struct vertex *) tetcomplextag2vertex(mesh, adjacencies[0]))->coord;
    apexpoints[1] = ((struct vertex *) tetcomplextag2vertex(mesh, adjacencies[1]))->coord;
    
    /* check whether each edge is a reflex edge. 
       the edge between the two apex vertices goes from 
       adjacencies[0] to adjacences[1]. We can determine if
       an edge is a reflex edge by using an orientation test:
       
       orient3d(p1,p2,adjacencies[0],adjacencies[1])
       
       will return + if the edge from p1 to p2 is convex
       will return - if the edge from p1 to p2 is reflex
       will return 0 if the edge from p1 to p2 intersects the top-bottom edge
       
       we need to run this test for each edge of the face
    */
    for (i=0; i<3; i++)
    {
        orientation = orient3d(&behave,
                               facepoints[i],
                               facepoints[(i+1) % 3],
                               apexpoints[0],
                               apexpoints[1]);
        if (orientation < 0)
        {
            /* found a reflex edge */
            reflexcount++;
        }
        else
        {
            if (orientation == 0)
            {
                /* this edge divides two coplanar faces */
                coplanaredges[*numcoplanar][0] = i;
                coplanaredges[*numcoplanar][1] = (i+1) % 3;
                (*numcoplanar)++;
                
                if (improvebehave.verbosity > 5)
                {
                    printf("found coplanar faces! marking\n");
                }
            }
        }
    }
    
    /* a tet can have at most 2 reflex edges / edges connecting coplanar faces */
    assert(reflexcount + *numcoplanar < 3);
    
    /* debugging printout to visually verify reflex edge count */
    /*
    if (reflexcount > 1)
    {
        printf("\nI think I found a face with %d reflex edges. The two tets involved are:\n", reflexcount);
        toptet[0] = adjacencies[0];
        toptet[1] = face[0];
        toptet[2] = face[1];
        toptet[3] = face[2];
        
        bottet[0] = adjacencies[1];
        bottet[1] = face[2];
        bottet[2] = face[1];
        bottet[3] = face[0];
        
        printf("two tets involved are:\n{");
        printtetverts(mesh, toptet);
        printf(",\n");
        printtetverts(mesh, bottet);
        printf("}\n");
    }
    */
    
    return reflexcount;
}

/* my wrapper for single tet insertion, gets recorded in the journal */
void inserttet(struct tetcomplex *mesh,
               tag vtx1,
               tag vtx2,
               tag vtx3,
               tag vtx4,
               bool record)
{
    int r;
    tag verts[4];
    
    verts[0] = vtx1;
    verts[1] = vtx2;
    verts[2] = vtx3;
    verts[3] = vtx4;
    
    r = tetcomplexinserttet(mesh, vtx1, vtx2, vtx3, vtx4);
    assert(r);
    
    /* if asked, record this deletion in the journal */
    if (record)
    {
        insertjournalentry(mesh, INSERTTET, verts, 4, (starreal *) NULL, (starreal *) NULL);
    }
}

/* my wrapper for single tet deletion, gets recorded in the journal */
void deletetet(struct tetcomplex *mesh,
               tag vtx1,
               tag vtx2,
               tag vtx3,
               tag vtx4,
               bool record)
{
    int r;
    tag verts[4];
    
    verts[0] = vtx1;
    verts[1] = vtx2;
    verts[2] = vtx3;
    verts[3] = vtx4;
    
    /* check that tet existst */
    assert(tetexists(mesh, vtx1, vtx2, vtx3, vtx4));
    
    r = tetcomplexdeletetet(mesh, vtx1, vtx2, vtx3, vtx4);
    assert(r);
    
    /* if asked, record this deletion in the journal */
    if (record)
    {
        insertjournalentry(mesh, DELETETET, verts, 4, (starreal *) NULL, (starreal *) NULL);
    }
}

/* bryan's own version of 2-3 flip that uses explicit
   delete and insert calls to perform the flip */
void flip23(struct tetcomplex *mesh,
            tag vtx1,
            tag vtx2,
            tag vtx3,
            tag vtxbot,
            tag vtxtop,
            bool record)
{
    int r;
    tag verts[5];
    
    verts[0] = vtx1;
    verts[1] = vtx2;
    verts[2] = vtx3;
    verts[3] = vtxbot;
    verts[4] = vtxtop;
    
    if (improvebehave.jflips)
    {
        /* use Jonathan's faster flip routine */
        tetcomplex23flip(mesh, vtxtop, vtx1, vtx2, vtx3, vtxbot);
    }
    else
    {
        /* delete the tet (top, 1, 2, 3) */
        assert(tetexists(mesh, vtxtop, vtx1, vtx2, vtx3));
        r = tetcomplexdeletetet(mesh, vtxtop, vtx1, vtx2, vtx3);
        assert(r);
    
        /* delete the tet (bot, 3, 2, 1) */
        assert(tetexists(mesh, vtxbot, vtx3, vtx2, vtx1));
        r = tetcomplexdeletetet(mesh, vtxbot, vtx3, vtx2, vtx1);
        assert(r);
    
        /* add the tet (top, 1, 2, bot) */
        r = tetcomplexinserttet(mesh, vtxtop, vtx1, vtx2, vtxbot);
        assert(r);
    
        /* add the tet (top, 2, 3, bot) */
        r = tetcomplexinserttet(mesh, vtxtop, vtx2, vtx3, vtxbot);
        assert(r);
    
        /* add the tet (top, 3, 1, bot) */
        r = tetcomplexinserttet(mesh, vtxtop, vtx3, vtx1, vtxbot);
        assert(r);
    }
    
    /* if asked, record this flip in the journal */
    if (record)
    {
        insertjournalentry(mesh, FLIP23, verts, 5, (starreal *) NULL, (starreal *) NULL);
    }
}

/* bryan's own version of a 3-2 flip that uses explicit
   delete and insert calls to perform the flip */
void flip32(struct tetcomplex *mesh,
            tag vtx1,
            tag vtx2,
            tag vtx3,
            tag vtxbot,
            tag vtxtop,
            bool record)
{
    int r;
    tag verts[5];
    
    verts[0] = vtx1;
    verts[1] = vtx2;
    verts[2] = vtx3;
    verts[3] = vtxbot;
    verts[4] = vtxtop;
    
    if (improvebehave.jflips)
    {
        /* use Jonathan's fast flip */
        tetcomplex32flip(mesh, vtxtop, vtx1, vtx2, vtx3, vtxbot);
    }
    else
    {
        /* delete the tet (top, 1, 2, bot) */
        assert(tetexists(mesh, vtxtop, vtx1, vtx2, vtxbot));
        r = tetcomplexdeletetet(mesh, vtxtop, vtx1, vtx2, vtxbot);
        assert(r);
    
        /* delete the tet (top, 2, 3, bot) */
        assert(tetexists(mesh, vtxtop, vtx2, vtx3, vtxbot));
        r = tetcomplexdeletetet(mesh, vtxtop, vtx2, vtx3, vtxbot);
        assert(r);
    
        /* delete the tet (top, 3, 1, bot) */
        r = tetcomplexdeletetet(mesh, vtxtop, vtx3, vtx1, vtxbot);
        assert(r);

        /* add the tet (top, 1, 2, 3) */
        r = tetcomplexinserttet(mesh, vtxtop, vtx1, vtx2, vtx3);
        assert(r);
    
        /* add the tet (bot, 3, 2, 1) */
        r = tetcomplexinserttet(mesh, vtxbot, vtx3, vtx2, vtx1);
        assert(r);
    }
    /* if asked, record this flip in the journal */
    if (record)
    {
        insertjournalentry(mesh, FLIP32, verts, 5, (starreal *) NULL, (starreal *) NULL);
    }
}

/* perform a 2-2 flip using the edge from vtx1 to vtx2 */
void flip22(struct tetcomplex *mesh,
            tag vtx1,
            tag vtx2,
            tag vtx3,
            tag vtxbot,
            tag vtxtop,
            bool record)
{
    int r;
    tag verts[5];
    
    verts[0] = vtx1;
    verts[1] = vtx2;
    verts[2] = vtx3;
    verts[3] = vtxbot;
    verts[4] = vtxtop;
    
    /* delete (top, 1, 2, 3) */
    r = tetcomplexdeletetet(mesh, vtxtop, vtx1, vtx2, vtx3);
    assert(r);
    
    /* delete (bot, 3, 2, 1) */
    r = tetcomplexdeletetet(mesh, vtxbot, vtx3, vtx2, vtx1);
    assert(r);
    
    /* insert (3, 1, top, bot) */
    r = tetcomplexinserttet(mesh, vtx3, vtx1, vtxtop, vtxbot);
    assert(r);
    
    /* insert (3, 2, bot, top) */
    r = tetcomplexinserttet(mesh, vtx3, vtx2, vtxbot, vtxtop);
    assert(r);
    
    if (record)
    {
        insertjournalentry(mesh, FLIP22, verts, 5, (starreal *) NULL, (starreal *) NULL);
    }
}

/* given the tet (1,2,3,4), and the vertex bodyvertex
   in its interior, delete the original tet and connect
   the interior vertex to the old vertices, forming 4 new
   tets */
void flip14(struct tetcomplex* mesh,
            tag vtx1,
            tag vtx2,
            tag vtx3,
            tag vtx4,
            tag bodyvtx,
            bool record)
{
    int r;
    tag verts[5];
    
    verts[0] = vtx1;
    verts[1] = vtx2;
    verts[2] = vtx3;
    verts[3] = vtx4;
    verts[4] = bodyvtx;
    
    if (improvebehave.jflips)
    {
        /* use Jonathan's fast flip */
        tetcomplex14flip(mesh, vtx1, vtx2, vtx3, vtx4, bodyvtx);
    }
    else
    {
        /* delete the original tet */
        r = tetcomplexdeletetet(mesh, vtx1, vtx2, vtx3, vtx4);
        assert(r);
    
        /* insert the tet (1, 2, 3, b) */
        r = tetcomplexinserttet(mesh, vtx1, vtx2, vtx3, bodyvtx);
        assert(r);
    
        /* insert the tet (1, 3, 4, b) */
        r = tetcomplexinserttet(mesh, vtx1, vtx3, vtx4, bodyvtx);
        assert(r);
    
        /* insert the tet (1, 4, 2, b) */
        r = tetcomplexinserttet(mesh, vtx1, vtx4, vtx2, bodyvtx);
        assert(r);
    
        /* insert the tet (b, 2, 3, 4) */
        r = tetcomplexinserttet(mesh, bodyvtx, vtx2, vtx3, vtx4);
        assert(r);
    }
    
    /* if asked, record this flip in the journal */
    if (record)
    {
        insertjournalentry(mesh, FLIP14, verts, 5, (starreal *) NULL, (starreal *) NULL);
    }
}

/* reverses a 1-4 flip */
void flip41(struct tetcomplex* mesh,
            tag vtx1,
            tag vtx2,
            tag vtx3,
            tag vtx4,
            tag bodyvtx,
            bool record)
{
    int r;
    tag verts[5];
    
    verts[0] = vtx1;
    verts[1] = vtx2;
    verts[2] = vtx3;
    verts[3] = vtx4;
    verts[4] = bodyvtx;
    
    if (improvebehave.jflips)
    {
        /* use Jonathan's fast flip */
        tetcomplex41flip(mesh, vtx1, vtx2, vtx3, vtx4, bodyvtx);
    }
    else
    {
        /* delete the tet (1, 2, 3, b) */
        r = tetcomplexdeletetet(mesh, vtx1, vtx2, vtx3, bodyvtx);
        assert(r);
        
        /* delete the tet (1, 3, 4, b) */
        r = tetcomplexdeletetet(mesh, vtx1, vtx3, vtx4, bodyvtx);
        assert(r);
        
        /* delete the tet (1, 4, 2, b) */
        r = tetcomplexdeletetet(mesh, vtx1, vtx4, vtx2, bodyvtx);
        assert(r);
        
        /* delete the tet (b, 2, 3, 4) */
        r = tetcomplexdeletetet(mesh, bodyvtx, vtx2, vtx3, vtx4);
        assert(r);
        
        /* insert the tet (1, 2, 3, 4) */
        r = tetcomplexinserttet(mesh, vtx1, vtx2, vtx3, vtx4);
        assert(r);
    }
    
    /* if asked, record this flip in the journal */
    if (record)
    {
        insertjournalentry(mesh, FLIP14, verts, 5, (starreal *) NULL, (starreal *) NULL);
    }
}

/* given a tetrahedron with a boundary face and a new vertex that
   lies inside that face, remove the old tet and create three new
   ones. vtx2, vtx3, vtx4 should be the boundary face oriented
   toward vtx1 with a right-hand rule */
void flip13(struct tetcomplex* mesh,
            tag vtx1,
            tag vtx2,
            tag vtx3,
            tag vtx4,
            tag facetvtx,
            bool record)
{
    int r;
    tag verts[5];
    
    verts[0] = vtx1;
    verts[1] = vtx2;
    verts[2] = vtx3;
    verts[3] = vtx4;
    verts[4] = facetvtx;
    
    /* delete the original tet */
    r = tetcomplexdeletetet(mesh, vtx1, vtx2, vtx3, vtx4);
    assert(r);
    
    /* insert the tet (1, 2, f, 4) */
    r = tetcomplexinserttet(mesh, vtx1, vtx2, facetvtx, vtx4);
    assert(r);
    
    /* insert the tet (1, 2, 3, f) */
    r = tetcomplexinserttet(mesh, vtx1, vtx2, vtx3, facetvtx);
    assert(r);
    
    /* insert the tet (1, 3, 4, f) */
    r = tetcomplexinserttet(mesh, vtx1, vtx3, vtx4, facetvtx);
    assert(r);
    
    /* if asked, record this flip in the journal */
    if (record)
    {
        insertjournalentry(mesh, FLIP13, verts, 5, (starreal *) NULL, (starreal *) NULL);
    }
}

/* the opposite of a 1-3 flip. Given three tets that share
   a facet vertex and an interior vertex, remove the facet
   vertex and replace them with one tet. vtx1 is the 
   interior vertex, facetvtx is the facet vtx, and 
   the other three vertices are on the future boundary
   facet and will form face (2,3,4) oriented toward vtx1 */
void flip31(struct tetcomplex* mesh,
            tag vtx1,
            tag vtx2,
            tag vtx3,
            tag vtx4,
            tag facetvtx,
            bool record)
{
    int r;
    tag verts[5];
    
    verts[0] = vtx1;
    verts[1] = vtx2;
    verts[2] = vtx3;
    verts[3] = vtx4;
    verts[4] = facetvtx;
    
    /* delete the tet (1, 2, f, 4) */
    r = tetcomplexdeletetet(mesh, vtx1, vtx2, facetvtx, vtx4);
    assert(r);
    
    /* delete the tet (1, 2, 3, f) */
    r = tetcomplexdeletetet(mesh, vtx1, vtx2, vtx3, facetvtx);
    assert(r);
    
    
    /* delete the tet (1, 3, 4, f) */
    r = tetcomplexdeletetet(mesh, vtx1, vtx3, vtx4, facetvtx);
    assert(r);
    
    /* insert the replacement tet */
    r = tetcomplexinserttet(mesh, vtx1, vtx2, vtx3, vtx4);
    assert(r);
    
    /* if asked, record this flip in the journal */
    if (record)
    {
        insertjournalentry(mesh, FLIP31, verts, 5, (starreal *) NULL, (starreal *) NULL);
    }
}

/* given a tetrahedron with a boundary edge and a new vertex that
   lies on that segment, remove the old tet and create two new
   ones. vtx1, vtx2 should be the boundary edge */
void flip12(struct tetcomplex* mesh,
            tag vtx1,
            tag vtx2,
            tag vtx3,
            tag vtx4,
            tag segmentvtx,
            bool record)
{
    int r;
    tag verts[5];
    
    verts[0] = vtx1;
    verts[1] = vtx2;
    verts[2] = vtx3;
    verts[3] = vtx4;
    verts[4] = segmentvtx;
    
    /* delete the original tet */
    r = tetcomplexdeletetet(mesh, vtx1, vtx2, vtx3, vtx4);
    assert(r);
    
    /* insert the tet (1, 3, 4, s) */
    r = tetcomplexinserttet(mesh, vtx1, vtx3, vtx4, segmentvtx);
    assert(r);
    
    /* insert the tet (2, 4, 3, s) */
    r = tetcomplexinserttet(mesh, vtx2, vtx4, vtx3, segmentvtx);
    assert(r);
    
    /* if asked, record this flip in the journal */
    if (record)
    {
        insertjournalentry(mesh, FLIP12, verts, 5, (starreal *) NULL, (starreal *) NULL);
    }
}

/* reverse the above 1-2 flip */
void flip21(struct tetcomplex* mesh,
            tag vtx1,
            tag vtx2,
            tag vtx3,
            tag vtx4,
            tag segmentvtx,
            bool record)
{
    int r;
    tag verts[5];
    
    verts[0] = vtx1;
    verts[1] = vtx2;
    verts[2] = vtx3;
    verts[3] = vtx4;
    verts[4] = segmentvtx;
    
    /* delete the tet (1, 3, 4, s) */
    r = tetcomplexdeletetet(mesh, vtx1, vtx3, vtx4, segmentvtx);
    assert(r);
    
    /* delete the tet (2, 4, 3, s) */
    r = tetcomplexdeletetet(mesh, vtx2, vtx4, vtx3, segmentvtx);
    assert(r);
    
    /* insert the original tet */
    r = tetcomplexinserttet(mesh, vtx1, vtx2, vtx3, vtx4);
    assert(r);
    
    /* if asked, record this flip in the journal */
    if (record)
    {
        insertjournalentry(mesh, FLIP12, verts, 5, (starreal *) NULL, (starreal *) NULL);
    }
}

/* creates the tetrahedra for the optimal triangulation of Tij */
void flip23recurse(struct tetcomplex *mesh,
                   tag vtx1,
                   tag vtx2,
                   tag *ring,
                   int ringcount,
                   int K[][MAXRINGTETS],
                   int i,
                   int j,
                   tag newtets[][4],
                   int *newtetcount)
{
    int k;
    
    /* only triangulate if we have more than an edge */
    if (j >= i+2)
    {
        k = K[i][j];
        flip23recurse(mesh, vtx1, vtx2, ring, ringcount, K, i, k, newtets, newtetcount);
        flip23recurse(mesh, vtx1, vtx2, ring, ringcount, K, k, j, newtets, newtetcount);
        
        /* eliminate the tetrahedra
        
           (vtx1, vtx2, ring[i-1], ring[k-1]) and 
           (vtx1, vtx2, ring[k-1], ring[j-1])
           
           and replace them with the tetrahedra
           
           (vtx1,      ring[i-1], ring[k-1], ring[j-1]),
           (ring[i-1], ring[k-1], ring[j-1], vtx2), and
           (vtx1,      vtx2,      ring[i-1], ring[j-1])
           
           using a 2-3 flip */
        
        /* make sure the two old tets exist */
        assert(tetexists(mesh, vtx1, vtx2, ring[i-1], ring[k-1]) == 1);
        assert(tetexists(mesh, vtx1, vtx2, ring[k-1], ring[j-1]) == 1);
                
        /* tetcomplex23flip(mesh, vtx1, ring[k-1], vtx2, ring[i-1], ring[j-1]); */
        /* use my flip instead until TODO Jonathan fixes his bug */
        flip23(mesh, vtx1, ring[k-1], vtx2, ring[i-1], ring[j-1], true);
        
        /* if we're paranoid, check consistency after every flip */
        /*
        if (1 == 0)
        {
            printf("\nThe vertices in question are:\n");
            printf("v_i=%d\nv_j=%d\nv_k=%d\n  a=%d\n  b=%d\n\n",ring[i-1],ring[j-1],ring[k-1],vtx1,vtx2);
            
            printf("I made sure the following tets existed BEFORE flip:\n");
            printf("a=%d b=%d v_i=%d v_k=%d\n", vtx1, vtx2, ring[i-1], ring[k-1]);
            printf("a=%d b=%d v_k=%d v_j=%d\n\n", vtx1, vtx2, ring[k-1], ring[j-1]);
            
            printf("I called tetcomplex23flip with \nvtx1=a \nvtx2=v_k \nvtx3=b \nvtx_bot=v_i \nvtx_top=v_j\n\n");
            
            printf("I made sure the following tets existed AFTER flip:\n");
            printf("  a=%d v_i=%d v_k=%d v_j=%d\n", vtx1, ring[i-1], ring[k-1], ring[j-1]);
            printf("v_i=%d v_k=%d v_j=%d   b=%d\n", ring[i-1], ring[k-1], ring[j-1], vtx2);
            printf("  a=%d   b=%d v_i=%d v_j=%d\n\n", vtx1, vtx2, ring[i-1], ring[j-1]);
            
            printf("And now, let's check consistency:\n");
            assert(tetcomplexconsistency(mesh) == 1);
        }
        */
        
        /* make sure the three new tets exist, and the permanent one are rightside out*/
        assert(tetexists(mesh, vtx1, ring[i-1], ring[k-1], ring[j-1]) == 1);
        assert(tetexists(mesh, ring[i-1], ring[k-1], ring[j-1], vtx2) == 1);
        assert(tetexists(mesh, vtx1, vtx2, ring[i-1], ring[j-1]) == 1);
        assert(tetquality(mesh, vtx1, ring[i-1], ring[k-1], ring[j-1], improvebehave.qualmeasure) > 0);
        assert(tetquality(mesh, ring[i-1], ring[k-1], ring[j-1], vtx2, improvebehave.qualmeasure) > 0);
        
        /* record the 2 permanent new tets */
        (*newtetcount)++;
        newtets[(*newtetcount)-1][0] = vtx1;
        newtets[(*newtetcount)-1][1] = ring[i-1];
        newtets[(*newtetcount)-1][2] = ring[k-1];
        newtets[(*newtetcount)-1][3] = ring[j-1];
        
        (*newtetcount)++;
        newtets[(*newtetcount)-1][0] = ring[i-1];
        newtets[(*newtetcount)-1][1] = ring[k-1];
        newtets[(*newtetcount)-1][2] = ring[j-1];
        newtets[(*newtetcount)-1][3] = vtx2;
    }
}

/* Replaces all tetrahedra that include the edge from vtx1 to vtx2 with
   the tetrahedra induced by the optimal triangulation of the ring of 
   vertices around that egde, computed by filltables. K is a table of 
   indices that identify the optimal triangulations of every subset of 
   the ring Tij. */
void removeedgeflips(struct tetcomplex *mesh,
                     tag vtx1,
                     tag vtx2,
                     tag *ring,
                     int ringcount,
                     int K[][MAXRINGTETS],
                     tag newtets[][4],
                     int *newtetcount,
                     bool boundary)
{
    /* k <= K[1,m] */
    int k = K[1][ringcount];
    flip23recurse(mesh, vtx1, vtx2, ring, ringcount, K, 1, k, newtets, newtetcount);
    flip23recurse(mesh, vtx1, vtx2, ring, ringcount, K, k, ringcount, newtets, newtetcount);
    
    if (boundary == false)
    {
        /* remove edge vtx1 vtx2 with a 3-2 flip, creating the tetrahedra
           (vtx1, ring[0], ring[k-1], ring[ringcount-1]) and
           (ring[0], ring[k-1], ring[ringcount-1], vtx2) */
        /* using my function instead of Jonathans TODO switch back? */
        /* tetcomplex32flip(mesh, ring[0], ring[k-1], ring[ringcount-1], vtx2, vtx1); */
        flip32(mesh, ring[0], ring[k-1], ring[ringcount-1], vtx2, vtx1, true);
    }
    else
    {
        /* because this is a boundary edge, we remove the edge with a 2-2 flip,
        */
        flip22(mesh, vtx2, vtx1, ring[k-1], ring[0], ring[ringcount-1], true);
    }
    
    /* make sure the new tets exist */
    assert(tetexists(mesh, vtx1, ring[0], ring[k-1], ring[ringcount-1]) == 1);
    assert(tetexists(mesh, ring[0], ring[k-1], ring[ringcount-1], vtx2) == 1);
    
    /* record the last 2 new tets */
    (*newtetcount)++;
    newtets[(*newtetcount)-1][0] = vtx1;
    newtets[(*newtetcount)-1][1] = ring[0];
    newtets[(*newtetcount)-1][2] = ring[k-1];
    newtets[(*newtetcount)-1][3] = ring[ringcount-1];
    
    (*newtetcount)++;
    newtets[(*newtetcount)-1][0] = ring[0];
    newtets[(*newtetcount)-1][1] = ring[k-1];
    newtets[(*newtetcount)-1][2] = ring[ringcount-1];
    newtets[(*newtetcount)-1][3] = vtx2;
}

/* fill Q and K tables for Klincsek's algorithm */
void filltables(struct tetcomplex *mesh,
                tag vtx1,
                tag vtx2,
                tag *ring,
                int ringcount,
                starreal oldminqual,
                starreal Q[][MAXRINGTETS],
                int K[][MAXRINGTETS])
{
    int i, j, k;   /* loop indices */
    starreal quala, qualb; /* qualities of new tets with verts a and b */
    starreal q;            /* quality for current table entry */
    int numboundverts;  /* number of boundary vertices in new tets */
    tag boundtags[4]; 
    
    /* for i <= m-2 downto 1 */
    for (i=ringcount-2; i>=1; i--)
    {
        /* for j <= i+2 to m */
        for (j=i+2; j<=ringcount; j++)
        {
            /* for k <= i+1 to j-1 */
            for (k=i+1; k<=j-1; k++)
            {
                if (improvebehave.verbosity > 6)
                {
                    printf("In filltables, i=%d j=%d k=%d\n",i,j,k);
                }
                
                /* q <= min(quality(a,vi,vj,vk),quality(vi,vk,vj,b)) */
                quala = tetquality(mesh, vtx1, ring[i-1], ring[k-1], ring[j-1], improvebehave.qualmeasure);
                
                /* check whether this new tet will have four boundary verts.
                   if it does, we don't want to create this tet unless it's
                   quality is awesome because it can't be later improved */
                if (quala > oldminqual && quala < MIN4BOUNDQUAL)
                {
                    numboundverts = boundverts(mesh, vtx1, ring[i-1], ring[k-1], ring[j-1], boundtags);
                    if (numboundverts == 4)
                    {
                        if (improvebehave.verbosity > 5)
                        {
                            printf("discovered a potential 4-node-on-boundary tet with qual %g, oldminqual was %g\n",pq(quala),pq(oldminqual));
                        }
                        quala = -1.0;
                    }
                }
                
                qualb = tetquality(mesh, ring[i-1], ring[k-1], ring[j-1], vtx2, improvebehave.qualmeasure);
                
                /* check whether this new tet will have four boundary verts.
                   if it does, we don't want to create this tet unless it's
                   quality is awesome because it can't be later improved */
                if (qualb > oldminqual && qualb < MIN4BOUNDQUAL)
                {
                    numboundverts = boundverts(mesh, ring[i-1], ring[k-1], ring[j-1], vtx2, boundtags);
                    if (numboundverts == 4)
                    {
                        if (improvebehave.verbosity > 5)
                        {
                            printf("discovered a potential 4-node-on-boundary tet with qual %g, oldminqual was %g\n",pq(qualb),pq(oldminqual));
                        }
                        qualb = -1.0;
                    }
                }
                
                q = (quala < qualb) ? quala : qualb;
                
                if (k < j-1)
                {
                    q = (q < Q[k][j]) ? q : Q[k][j];
                }
                if (k > i+1)
                {
                    q = (q < Q[i][j]) ? q : Q[i][j];
                }
                if ((k == i+1) || (q > Q[i][j]))
                {
                    Q[i][j] = q;
                    K[i][j] = k;
                }
            }
        }
    }
}

/* try contracting the edge from v1 to v2 that is in the
   tet (v1, v2, v3, v4). Return true if edge contraction
   succeeds in improving quality of all affected tets,
   and false otherwise (mesh unchanged on failure) */
#define CONTRACTPARANOID false
bool edgecontract(struct tetcomplex *mesh,
                  tag v1,
                  tag v2,
                  tag v3,
                  tag v4,
                  starreal *minqualbefore,
                  starreal *minqualafter,
                  int *numouttets,
                  tag outtets[MAXINCIDENTTETS][4],
                  bool requireimprove)
{
    bool boundedge;               /* is this a boundary edge? */
    int numringtets;              /* number of tets in the ring around the edge */
    tag ringtets[MAXRINGTETS][4]; /* the tets in the ring */
    tag boundfacev[2];            /* vertices of faces on the boundary */
    tag tet[4], newtet[4];        /* tet arranged so less flexible vert is first */
    int i,j;                      /* loop index */
    starreal quality;             /* current tet quality */
    bool smoothed;                /* was vertex smoothing successful */
    starreal smoothqual;          /* quality after smoothing */
    int smoothkinds = 0;          /* types of vertices that can be smoothed */
    starreal e[3];                /* vector of the edge */
    starreal *segment;            /* pointer to vertex's free segment, if any */
    starreal *v1c, *v2c;          /* pointers to vertex coordinates */
    starreal edgedot=0.0;         /* dot product between edge vector and segment */
    starreal worstnewqual=HUGEFLOAT;/* worst quality of new tets before smoothing */
    starreal worstafter;
    tag toinsert[MAXINCIDENTTETS][4];
    int numtoinsert = 0;
    int edge[2];
    starreal thislong;
    
    int edgecase = NOEDGECASE;
    
    /* get global worst for paranoid insertion */
    starreal worstbefore = HUGEFLOAT;
    starreal meanquals[NUMMEANTHRESHOLDS];
    
    /* a list of all the tets incident to the edge vertices */
    int numincident[2];
    tag incidenttags[2][MAXINCIDENTTETS][4];
    bool noghosts;
    
    /* save the mesh state before we start changing stuff */
    int beforeid = lastjournalentry();
    
    /* determine the vertex type of each endpoint */
    int v1type = ((struct vertextype *) arraypoollookup(&vertexinfo, v1))->kind;
    int v2type = ((struct vertextype *) arraypoollookup(&vertexinfo, v2))->kind;
    assert(v1type != INPUTVERTEX && v2type != INPUTVERTEX);
    if (CONTRACTPARANOID)
    {
        meshquality(mesh, improvebehave.qualmeasure, meanquals, &worstbefore);
    }
    
    if (improvebehave.facetsmooth) smoothkinds |= SMOOTHFACETVERTICES;
    if (improvebehave.segmentsmooth) smoothkinds |= SMOOTHSEGMENTVERTICES;
    if (improvebehave.fixedsmooth) smoothkinds |= SMOOTHFIXEDVERTICES;
    
    /* TODO: take this out? give up with any fixed vertices */
    /*
    if (v1type == FIXEDVERTEX || v2type == FIXEDVERTEX)
    {
        return false;
    }
    */
    
    /* determine which contraction case we're dealing with */
    if (v1type == FREEVERTEX || v2type == FREEVERTEX)
    {
        if (v1type == FREEVERTEX && v2type == FREEVERTEX)
        {
            edgecase = FREEFREEEDGE;
        }
        else if (v1type == FACETVERTEX || v2type == FACETVERTEX)
        {
            edgecase = FREEFACETEDGE;
        }
        else if (v1type == SEGMENTVERTEX || v2type == SEGMENTVERTEX)
        {
            edgecase = FREESEGMENTEDGE;
        }
        else
        {
            assert(v1type == FIXEDVERTEX || v2type == FIXEDVERTEX);
            edgecase = FREEFIXEDEDGE;
        }
    }
    /* neither vertex is free */
    else if (v1type == FACETVERTEX || v2type == FACETVERTEX)
    {
        if (v1type == FACETVERTEX && v2type == FACETVERTEX)
        {
            edgecase = FACETFACETEDGE;
        }
        else if (v1type == SEGMENTVERTEX || v2type == SEGMENTVERTEX)
        {
            edgecase = FACETSEGMENTEDGE;
        }
        else
        {
            assert(v1type == FIXEDVERTEX || v2type == FIXEDVERTEX);
            edgecase = FACETFIXEDEDGE;
        }
    }
    /* nor is either vertex facet */
    else if (v1type == SEGMENTVERTEX || v2type == SEGMENTVERTEX)
    {
        if (v1type == SEGMENTVERTEX && v2type == SEGMENTVERTEX)
        {
            edgecase = SEGMENTSEGMENTEDGE;
        }
        else
        {
            assert(v1type == FIXEDVERTEX || v2type == FIXEDVERTEX);
            edgecase = SEGMENTFIXEDEDGE;
        }
    }
    /* nor is either vertex segment */
    else
    {
        assert(v1type == FIXEDVERTEX && v2type == FIXEDVERTEX);
        edgecase = FIXEDFIXEDEDGE;
    }
    assert(edgecase != NOEDGECASE);
    
    /* gather tets around edge and check if it's on the boundary */
    boundedge = getedgering(mesh, v1, v2, v3, v4, &numringtets, ringtets, boundfacev);
    
    if (improvebehave.verbosity > 5)
    {
        printf("At start of edgecontract, lastjournalid = %d\n", beforeid);
        printf("In edge contraction, v1type = %d, v2type = %d, boundedge=%d\n", v1type, v2type, boundedge);
    }
    
    /* if one of the vertices is a segment vertex, figure out edge vector */
    if (edgecase == SEGMENTSEGMENTEDGE || edgecase == SEGMENTFIXEDEDGE)
    {
        if (v1type == SEGMENTVERTEX)
        {
            /* fetch the pre-computed vertex segment vector */
            segment = ((struct vertextype *) arraypoollookup(&vertexinfo, v1))->vec;
        }
        else
        {
            assert(v2type == SEGMENTVERTEX);
            /* fetch the pre-computed vertex segment vector */
            segment = ((struct vertextype *) arraypoollookup(&vertexinfo, v2))->vec;
        }
        /* compute the normalized edge vector between the two vertices */
        v1c = ((struct vertex *) tetcomplextag2vertex(mesh, v1))->coord;
        v2c = ((struct vertex *) tetcomplextag2vertex(mesh, v2))->coord;
        
        vsub(v2c, v1c, e);
        vscale(1.0 / vlength(e), e, e);
        
        edgedot = dot(e, segment);
        
        if (improvebehave.verbosity > 5)
        {
            printf("segment is (%g %g %g), edge vector is (%g %g %g), dot product is %g\n", segment[0], segment[1], segment[2], e[0], e[1], e[2], edgedot);
        }
    }
    
    /* handle the weirdness of each case */
    switch (edgecase)
    {
        case FREEFREEEDGE:
            if (improvebehave.verbosity > 5)
            {
                printf("    case is free-free contraction.\n");
            }
            break;
        case FREEFACETEDGE:
            if (improvebehave.verbosity > 5)
            {
                printf("    case is free-facet contraction\n");
            }
            break;
        case FREESEGMENTEDGE:
            if (improvebehave.verbosity > 5)
            {
                printf("    case is free-segment contraction\n");
            }
            break;
        case FREEFIXEDEDGE:
            if (improvebehave.verbosity > 5)
            {
                printf("    case is free-fixed contraction\n");
            }
            break;
        case FACETFACETEDGE:
            if (improvebehave.verbosity > 5)
            {
                printf("    case is facet-facet contraction\n");
            }
            /* for this case, if the edge is not a boundary edge, no-go */
            if (!boundedge)
            {
                if (improvebehave.verbosity > 5) printf("    FAILS because edge is not boundary edge.\n");
                return false;
            }
            break;
        case FACETSEGMENTEDGE:
            if (improvebehave.verbosity > 5)
            {
                printf("    case is facet-segment contraction.\n");
            }
            if (!boundedge)
            {
                if (improvebehave.verbosity > 5) printf("    FAILS because edge is not boundary edge.\n");
                return false;
            }
            break;
        case FACETFIXEDEDGE:
            return false;
            if (improvebehave.verbosity > 5)
            {
                /* make sure its a boundary edge */
                if (!boundedge)
                {
                    if (improvebehave.verbosity > 5) printf("    FAILS because edge is not boundary edge.\n");
                    return false;
                }
                printf("    case is facet-fixed contraction.\n");
            }
            break;
        case SEGMENTSEGMENTEDGE:
            if (improvebehave.verbosity > 5)
            {
                printf("    case is segment-segment contraction.\n");
            }
            /* make sure its a boundary edge */
            if (!boundedge)
            {
                if (improvebehave.verbosity > 5) printf("    FAILS because edge is not boundary edge.\n");
                return false;
            }
            /* make sure that the segment and the edge are the same */
            if (fabs(edgedot) != 1.0)
            {
                if (improvebehave.verbosity > 5) printf("   FAILS because edge and segment aren't the same.\n");
                return false;
            }
            break;
        case SEGMENTFIXEDEDGE:
            return false;
            if (improvebehave.verbosity > 5)
            {
                printf("    case is segment-fixed contraction.\n");
            }
            /* make sure its a boundary edge */
            if (!boundedge)
            {
                if (improvebehave.verbosity > 5) printf("    FAILS because edge is not boundary edge.\n");
                return false;
            }
            if (fabs(edgedot) != 1.0)
            {
                if (improvebehave.verbosity > 5) printf("   FAILS because edge and segment aren't the same.\n");
                return false;
            }
            break;
        case FIXEDFIXEDEDGE:
            if (improvebehave.verbosity > 5)
            {
                printf("    case is fixed-fixed contraction. NOT SUPPORTED.\n");
            }
            return false;
            break;
        default:
            printf("Invalid edge contraction case %d. Dying.\n", edgecase);
            starexit(1);
            break;
    }
    
    /* if v2 has less freedom than v1 */
    if (v2type < v1type)
    {
        /* arrange the tet so v2 goes first */
        tet[0] = v2;
        tet[1] = v1;
        tet[2] = v4;
        tet[3] = v3;
    }
    else
    {
        /* arrange thet tet so v1 goes first */
        tet[0] = v1;
        tet[1] = v2;
        tet[2] = v3;
        tet[3] = v4;
    }
    assert(tetexistsa(mesh, tet));
    
    if (improvebehave.verbosity > 5)
    {
        printf("Going ahead. v1=%d, type=%d, v2=%d, type=%d, tet[0]=%d, tet[1]=%d\n", (int) v1, v1type, (int) v2, v2type, (int) tet[0], (int) tet[1]);
    }
    
    /* fetch all the tets incident to each endpoint and compute minimum quality */
    *minqualbefore = HUGEFLOAT;
    numincident[0] = numincident[1] = 0;
    getincidenttets(mesh, tet[0], tet[1], tet[2], tet[3],
                    incidenttags[0], &numincident[0], &noghosts);
    getincidenttets(mesh, tet[1], tet[0], tet[3], tet[2],
                    incidenttags[1], &numincident[1], &noghosts);
    
    for (i=0; i<2; i++)
    {                   
        for (j=0; j<numincident[i]; j++)
        {
            quality = tetquality(mesh, 
                                 incidenttags[i][j][0],
                                 incidenttags[i][j][1],
                                 incidenttags[i][j][2],
                                 incidenttags[i][j][3],
                                 improvebehave.qualmeasure);
            if (quality < *minqualbefore) *minqualbefore = quality;
        }
    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("In edge contract, v1=%d has %d tets incident, v2=%d has %d tets\n", (int) tet[0], numincident[0], (int) tet[1], numincident[1]);
        printf("Minimum quality before contraction is %g\n", pq(*minqualbefore));
    }
    
    stats.edgecontractionattempts++;
    stats.edgecontractringatt[numringtets]++;
    stats.edgecontractcaseatt[edgecase]++;
    
    /* delete all tets in the ring around the edge */
    for (i=0; i<numringtets; i++)
    {
        assert(tetexistsa(mesh, ringtets[i]));
        deletetet(mesh, ringtets[i][0], ringtets[i][1], ringtets[i][2], ringtets[i][3], true);
        /*
        printf("    just deleted ring tet %d/%d (%d %d %d %d)\n", i+1, numringtets, ringtets[i][0], ringtets[i][1], ringtets[i][2], ringtets[i][3]);
        */
    }
    
    newtet[0] = GHOSTVERTEX;
    newtet[1] = GHOSTVERTEX;
    newtet[2] = GHOSTVERTEX;
    newtet[3] = GHOSTVERTEX;
    
    /* replace all the tets incident to the second endpoint with tets inc. to first */
    for (i=0; i<numincident[1]; i++)
    {
        /* check that this tet wasn't deleted with ring tets */
        if (!tetexistsa(mesh, incidenttags[1][i])) 
        {
            if (improvebehave.verbosity > 5)
            {
                printf("    Skipping deletion of incident tet (%d %d %d %d), it's already gone\n", (int) incidenttags[1][i][0], (int) incidenttags[1][i][1], (int) incidenttags[1][i][2], (int) incidenttags[1][i][3]);
            }
            continue;
        }
        
        /* if we're anisotropic, check the quality in physical space */
        if (improvebehave.anisotropic)
        {
            improvebehave.anisotropic = false;
            
            quality = tetquality(mesh,
                           tet[0],
                           incidenttags[1][i][1],
                           incidenttags[1][i][2],
                           incidenttags[1][i][3],
                           improvebehave.qualmeasure);
                           
            improvebehave.anisotropic = true;
            
            if (quality <= MINSIZETETQUALITY)
            {
                if (improvebehave.verbosity > 5)
                {
                    printf("Can't contract edge because IN PHYSICAL SPACE a new tet would invert.\n");
                }

                /* undo what we've done and return failure */
                invertjournalupto(mesh, beforeid);
                return false;
            }
        }
        
        /* check if the new tet would be inverted. if so, give up */
        quality = tetquality(mesh,
                       tet[0],
                       incidenttags[1][i][1],
                       incidenttags[1][i][2],
                       incidenttags[1][i][3],
                       improvebehave.qualmeasure);
        if (quality <= MINTETQUALITY)
        {
            if (improvebehave.verbosity > 5)
            {
                printf("Can't contract edge because a new tet would invert.\n");
            }
            
            /* undo what we've done and return failure */
            invertjournalupto(mesh, beforeid);
            return false;
        }
        
        /* if this is the worst inserted tet note it */
        if (quality < worstnewqual) worstnewqual = quality;
        
        if (improvebehave.verbosity > 5)
        {
            printf("    Replacing tet (%d %d %d %d) with tet (%d %d %d %d)\n", 
            (int) incidenttags[1][i][0],
            (int) incidenttags[1][i][1],
            (int) incidenttags[1][i][2],
            (int) incidenttags[1][i][3],
            (int) tet[0],
            (int) incidenttags[1][i][1],
            (int) incidenttags[1][i][2],
            (int) incidenttags[1][i][3]);
        }
        
        deletetet(mesh, 
                  incidenttags[1][i][0],
                  incidenttags[1][i][1],
                  incidenttags[1][i][2],
                  incidenttags[1][i][3],
                  true);
        
        toinsert[numtoinsert][0] = tet[0];
        toinsert[numtoinsert][1] = incidenttags[1][i][1];
        toinsert[numtoinsert][2] = incidenttags[1][i][2];
        toinsert[numtoinsert][3] = incidenttags[1][i][3];
        numtoinsert++; 
    }
    
    if (improvebehave.verbosity > 5 || worstnewqual < MINTETQUALITY)
    {
        printf("Replacing %d tets incident to vertex %d\n", numtoinsert, (int) tet[1]);
        printf("Worst inserted tet quality is %g\n", pq(worstnewqual));
    }
    
    /* the number of tets to insert should be the number incident to the second vertex
       less the number of ring vertices */
    assert(numtoinsert == numincident[1] - numringtets);
    
    /* need a separate loop so all are deleted before insertion ? */
    for (i=0; i<numtoinsert; i++)
    {
        /*
        printf("about to insert tet (%d %d %d %d)\n", toinsert[i][0], toinsert[i][1], toinsert[i][2], toinsert[i][3]);
        */
        inserttet(mesh, 
                  toinsert[i][0],
                  toinsert[i][1],
                  toinsert[i][2],
                  toinsert[i][3],
                  true);
        /* record the most recently inserted tet */
        newtet[0] = toinsert[i][0];
        newtet[1] = toinsert[i][1];
        newtet[2] = toinsert[i][2];
        newtet[3] = toinsert[i][3];
        /* make sure we have an extant tet to base the smooth off of */
        assert(tetexistsa(mesh, newtet));
    }
    
    if (worstnewqual < MINTETQUALITY)
    {
        printf("in edge contract, worst new qual = %g is below mintetquality\n", worstnewqual);
    }
    assert(worstnewqual >= MINTETQUALITY);
    
    /* now, smooth the remaining endpoint */
    smoothed = nonsmoothsinglevertex(mesh,
                                         newtet[0],
                                         newtet[1],
                                         newtet[2],
                                         newtet[3],
                                         &smoothqual,
                                         smoothkinds);
    /* if smoothing was successful, then use its worst qual */
    if (smoothed)
    {
        *minqualafter = smoothqual;
    }
    /* otherwise, use the worst new tet */
    else
    {
        if (improvebehave.verbosity > 5)
        {
            printf("using worst inserted quality rather than smoothed quality!");
        }
        *minqualafter = worstnewqual;
    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("After edge contraction and smoothing, worst qual went from %g to %g, diff of %g\n", pq(*minqualbefore), pq(smoothqual), pq(smoothqual - *minqualbefore));
    }
    
    /* enforce minimum improvement */
    if ((*minqualafter - *minqualbefore < MINCONTRACTIMPROVEMENT) && requireimprove)
    {
        if (improvebehave.verbosity > 5)
        {
            printf("Reverting edge contraction because quality worsened.\n");
        }
        invertjournalupto(mesh, beforeid);
        return false;
    }
    
    if (!requireimprove && (*minqualafter < MINCONTRACTQUAL))
    {
        if (improvebehave.verbosity > 5)
        {
            printf("Aborting size-control edge contraction because worst new qual is %g (%g required)\n", *minqualafter, MINCONTRACTQUAL);
        }
        invertjournalupto(mesh, beforeid);
        return false;
    }
    
    /* collect tets incident to the remaining vertex and return them */
    if (numouttets != NULL)
    {
        *numouttets = 0;
        getincidenttets(mesh, newtet[0], newtet[1], newtet[2], newtet[3],
                        outtets, numouttets, &noghosts);
        
        /* check to make sure that none of the edges are too short or too long */
        if (improvebehave.sizing)
        {
            for (i=0; i<(*numouttets); i++)
            {
                thislong = tetedge(mesh, outtets[i][0], outtets[i][1], outtets[i][2], outtets[i][3], edge, true);
                if (thislong > improvebehave.targetedgelength * improvebehave.longerfactor * 0.7)
                {
                    if (improvebehave.verbosity > 5)
                    {
                        printf("Aborting edge contraction because it would create edge %g long, longest allowable is %g\n", thislong, improvebehave.targetedgelength * improvebehave.longerfactor * 0.85);
                    }
                    invertjournalupto(mesh, beforeid);
                    return false;
                }
            }
        }
        
        if (improvebehave.verbosity > 5)
        {
            textcolor(BRIGHT, GREEN, BLACK);
            printf("Edge contraction succeeded. %d tets deleted. %d tets now incident.\n", numringtets, *numouttets);
            printf("After edge contraction and smoothing=%d, worst qual went from %g to %g, diff of %g\n", smoothed, pq(*minqualbefore), pq(smoothqual), pq(smoothqual - *minqualbefore));
            textcolor(RESET, WHITE, BLACK);
        }
    
        /* the number of tets incident to the surviving vertex should be
           the sum of those incident to the original edge endpoints minus the number
           of tets in the ring */
        assert(*numouttets == numincident[0] + numincident[1] - (2 * numringtets));
    }
    
    worstafter = HUGEFLOAT;
    if (CONTRACTPARANOID)
    {
        meshquality(mesh, improvebehave.qualmeasure, meanquals, &worstafter);
        printf("worstbefore = %g, worstafter = %g, diff %g\n", pq(worstbefore), pq(worstafter), pq(worstafter - worstbefore));
        if (worstafter < worstbefore)
        {
            worsttetreport(mesh, improvebehave.qualmeasure, 1.0);
        }
        assert(worstafter >= worstbefore);
    }
    
    /* record success */
    stats.edgecontractions++;
    stats.edgecontractringsuc[numringtets]++;
    stats.edgecontractcasesuc[edgecase]++;
    
    if (edgecase == FACETFIXEDEDGE)
    {
        printf("Whoa, facet fixed worked!\n");
    }
    if (edgecase == SEGMENTFIXEDEDGE)
    {
        printf("Whee, segment fixed worked!\n");
    }
    
    return true;
}

/* try to contract all six edges of a tet */
bool tryedgecontract(struct tetcomplex *mesh,
                     tag vtx1, tag vtx2, tag vtx3, tag vtx4,
                     struct arraypoolstack *outstack,
                     starreal *minqualafter,
                     bool requireimprove,
                     bool justfirstedge)
{
    tag tet[4];
    bool contracted = false;
    starreal minqualbefore;
    int numouttets;
    tag outtets[MAXINCIDENTTETS][4];
    struct improvetet *stacktet;
    int i,j,k,l,m,temp;
    starreal newqual;
    
    /* first, check to make sure this tet still exists in the mesh */
    if (tetexists(mesh, vtx1, vtx2, vtx3, vtx4) == 0)
    {
        return false;
    }
    
    tet[0] = vtx1;
    tet[1] = vtx2;
    tet[2] = vtx3;
    tet[3] = vtx4;
    
    if (improvebehave.verbosity > 5)
    {
        printf("** Starting tryedgecontract on top-level tet (%d %d %d %d)\n", (int) vtx1, (int) vtx2, (int) vtx3, (int) vtx4);
    }
    
    /* loop over all 6 edges of the tetrahedron */
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
            
            if (improvebehave.verbosity > 5)
            {
                printf("About to attempt edge contraction on tet (%d %d %d %d)\n", (int) tet[i], (int) tet[j], (int) tet[k], (int) tet[l]);
            }
            
            contracted = edgecontract(mesh, tet[i], tet[j], tet[k], tet[l],
                                      &minqualbefore, minqualafter,
                                      &numouttets, outtets, requireimprove);
            
            /* if contraction fails, try with order swapped */
            if (contracted == false)
            {
                contracted = edgecontract(mesh, tet[j], tet[i], tet[l], tet[k],
                                          &minqualbefore, minqualafter,
                                          &numouttets, outtets, requireimprove);
            }
            
            /* if edge contraction succeeded */
            if (contracted)
            {
                if (outstack != NULL)
                {
                    if (improvebehave.verbosity > 5)
                    {
                        printf("Pushing %d new tets on the output stack\n", numouttets);
                    }
                    
                    for (m=0; m<numouttets; m++)
                    {
                        assert(tetexistsa(mesh, outtets[m]));
                        newqual = tetquality(mesh, 
                                             outtets[m][0],
                                             outtets[m][1],
                                             outtets[m][2],
                                             outtets[m][3],
                                             improvebehave.qualmeasure);
                        assert(newqual > 0);
                        
                        /* push this tet on the output stack */
                        if (tetinstack(outstack,
                                       outtets[m][0],
                                       outtets[m][1],
                                       outtets[m][2],
                                       outtets[m][3]) == false)
                        {
                            stacktet = (struct improvetet *) stackpush(outstack);
                            stacktet->quality = newqual;
                            stacktet->verts[0] = outtets[m][0];
                            stacktet->verts[1] = outtets[m][1];
                            stacktet->verts[2] = outtets[m][2];
                            stacktet->verts[3] = outtets[m][3];
                        }
                    }
                }
                
                /* return with success */
                return true;
            }
            
            /* if we're requested just to try the first edge, bail now */
            if (justfirstedge) return false;
        }
    }
    
    return false;
}

/* attempt to remove the edge from vtx1 to vtx2. ring is the ring
   of vertices to be removed. Return 1 if edge removal successfully
   improves quality, zero otherwise. */
int removeedge(struct tetcomplex *mesh, 
               tag vtx1,
               tag vtx2,
               tag* ring,
               int ringcount,
               starreal oldminqual,
               tag newtets[][4],
               int *newtetcount,
               starreal *outminqual,
               bool boundary)
{
    static starreal Q[MAXRINGTETS][MAXRINGTETS]; /* table to hold minimal quality of each subtraingluation */
    static int K[MAXRINGTETS][MAXRINGTETS];    /* table to hold pointer to optimal subtriangulations */
    starreal newminqual;                   /* minimum tet quality in the new triangulation */
    
    /* fill the tables */
    filltables(mesh, vtx2, vtx1, ring, ringcount, oldminqual, Q, K);
    
    /* retrieve the minimum quality of the new triangulation */
    newminqual = Q[1][ringcount];
    
    if (improvebehave.verbosity > 5 && (newminqual > oldminqual))
    {
        printf("new configuration for edge remove with quality %f better than old %f...\n", pq(newminqual), pq(oldminqual));
        printtables(Q, K, ringcount);
    }
    
    /* if we have an improvement in quality, remove the edge */
    if (newminqual > oldminqual)
    {
        removeedgeflips(mesh, vtx2, vtx1, ring, ringcount, K, newtets, newtetcount, boundary);
        *outminqual = newminqual;
        return 1;
    }
    else
    {
        *outminqual = oldminqual;
        return 0;
    }
}

/* check for any boundary edge removal opportunities in this tet. */
int tryboundaryedgeremove(struct tetcomplex *mesh,
                   tag vtx1,
                   tag vtx2,
                   tag vtx3,
                   tag vtx4,
                   struct arraypoolstack *tetstack,
                   int ringsizes[MAXRINGTETS],
                   int *biggestring,
                   starreal *minafterqual)
{
    int numboundedges;
    tag edgelist[6][2];
    tag edgefaces[6][2]; /* the third vertex in the faces that lie on the boundary */
    int numedgetets[6];
    tag edgetets[6][MAXRINGTETS][4];
    int i,j;
    tag v1, v2, v3, v4; /* tags of vertices of two faces */
    starreal *vc1, *vc2, *vc3, *vc4; /* coordinates of the face vertices */
    starreal e1[3], e2[3], e3[3];   /* edges for forming cross products */
    starreal norm1[3], norm2[3];     /* normals for the two faces */
    starreal dotprod;
    starreal minbeforequal=HUGEFLOAT, curqual, newqual;
    tag newtets[MAXRINGTETS][4];  /* array to hold the new submesh after edge flipping */
    tag vertring[MAXRINGTETS];    /* half-ring of vertices around edge */
    int newtetcount = 0;          /* number of tets in new arrangement */
    bool edgeremoved = false;
    struct improvetet *stacktet;  /* for pushing newly created tets on the stack */
    bool foundface;
    tag nextprev[2];
    int ringsize;
    
    /* fetch all the boundary edges of this tet and their rings of tets */
    numboundedges = boundedges(mesh,
                               vtx1, vtx2, vtx3, vtx4,
                               edgelist,
                               edgefaces,
                               numedgetets,
                               edgetets);
                               
    if (numboundedges == 0) return 0;
    
    /* try each boundary edge */
    for (i=0; i<numboundedges; i++)
    {
        /* check if this edge's two boundary faces are in a plane,
           which makes the edge eligible for removal */
        
        v1 = edgelist[i][0]; /* first vertex of edge to be removed */
        v2 = edgelist[i][1]; /* second vertex of edge to be removed */
        v3 = edgefaces[i][0]; /* third vertex of first face */
        v4 = edgefaces[i][1]; /* third vertex of second face */
        
        /* fetch actual vertex coordinates */
        vc1 = ((struct vertex *) tetcomplextag2vertex(mesh, v1))->coord;
        vc2 = ((struct vertex *) tetcomplextag2vertex(mesh, v2))->coord;
        vc3 = ((struct vertex *) tetcomplextag2vertex(mesh, v3))->coord;
        vc4 = ((struct vertex *) tetcomplextag2vertex(mesh, v4))->coord;
        
        /* form edge vectors for the faces */
        vsub(vc2, vc1, e1);
        vsub(vc3, vc1, e2);
        vsub(vc4, vc1, e3);
         
        /* find the (unit-length) face normal of this face */
        cross(e1, e2, norm1);
        vscale((1.0 / vlength(norm1)), norm1, norm1);
        cross(e3, e1, norm2);
        vscale((1.0 / vlength(norm2)), norm2, norm2);
        
        /* compute dot product between two face normals to determine if
           faces are coplanar */
        dotprod = dot(norm1, norm2);
        
        /* we need to be exact about dot product so as not to create inverted tets */
        if (dotprod != 1.0)
        {
            if (improvebehave.verbosity > 5)
            {
                printf("edge is not in facet because dotprod = %g\n", dotprod);
            }
            
            /* this edge doesn't lie in an input face, and can't be removed */
            continue; /* move on to the next boundary edge */
        }
        
        if (improvebehave.verbosity > 5)
        {
            printf("edge IS  in facet because dotprod = %g\n", dotprod);
        }
        
        /* debug output to visualize coplanar faces */
        /*
        if (improvebehave.verbosity > 5)
        {
            tag faces[2][3] = {{v1,v2,v3},{v1,v4,v2}};
            printf("here are the coplanar faces:\n");
            printfaces(mesh, faces, 2);
            printf("here are the tets halfway around edge\n");
            printtets(mesh, edgetets[i], numedgetets[i]);
        }
        */
        
        /* compute minimum quality of tets around half-ring originally */
        for (j=0; j<numedgetets[i]; j++)
        {
            curqual = tetquality(mesh, 
                                 edgetets[i][j][0], 
                                 edgetets[i][j][1], 
                                 edgetets[i][j][2], 
                                 edgetets[i][j][3],
                                 improvebehave.qualmeasure);
            if (curqual < minbeforequal) minbeforequal = curqual;
        }
        
        /* build up half-ring of verts */
        vertring[0] = v4;
        ringsize = 1;
        foundface = tetcomplexadjacencies(mesh, v1, v2, vertring[0], nextprev);
        assert(foundface);
        assert(nextprev[0] != GHOSTVERTEX);
        /* move around the ring using adjacency queries */
        while (nextprev[0] != GHOSTVERTEX)
        {
            /* add the next vertex to the ring */
            vertring[ringsize] = nextprev[0];
            ringsize++;
            
            foundface = tetcomplexadjacencies(mesh, v1, v2, vertring[ringsize-1], nextprev);
            assert(foundface);
        }
        /* at the end, the last ring vertex should be v3... */
        assert(vertring[ringsize-1] == v3);
        
        if (improvebehave.verbosity > 5)
        {
            printf("there are %d verts in half-ring\n", ringsize);
            printf("here's the ring according to printring\n");
            printhalfring(mesh, v1, v2, vertring, ringsize);
        }
        
        /* stats stuff */
        stats.ringsizeattempts[numedgetets[i]]++;
        stats.boundaryedgeremovalattempts++;
        stats.edgeremovalattempts++;
        
        /* now that we know we have a good ring of vertices around this
           edge, we see if by removing the edge we can improve quality.
           to do this, we find the optimal triangulation of the ring
           of vertices around the edge, and see if the tets induced by
           this triangulation are all of better quality than worst tet
           in the original triangulation */
        edgeremoved = removeedge(mesh, v1, v2, vertring, ringsize, minbeforequal, newtets, &newtetcount, minafterqual, true); 
        if (edgeremoved == 1)
        {
            /* stats stuff */
            stats.edgeremovals++;
            stats.boundaryedgeremovals++;
            stats.ringsizesuccess[numedgetets[i]]++;
            
            if (ringsizes != NULL)
            {
                if (numedgetets[i] > *biggestring)
                {
                    *biggestring = numedgetets[i];
                    if (improvebehave.verbosity > 5)
                    {
                        printf("new biggest ring of %d tets\n",*biggestring);
                    }
                }
                /* TODO... fix so count is right! */
                if (numedgetets[i] > 2)
                {
                    ringsizes[numedgetets[i]-3]++;
                }
            }
            
            if (improvebehave.verbosity > 5)
            {
                printf("shit! actually removed a BOUNDARY edge improving quality from %.17g to %.17g, delta of %.17g\n", pq(minbeforequal), pq(*minafterqual),pq(*minafterqual-minbeforequal));
                printf("we started with %d tets, and now have %d\n", numedgetets[i], newtetcount);
                printf("here are the new ones:\n");
                printtets(mesh, newtets, newtetcount);
            }
            
            /* check to make sure all of the new tets are positive orientation, and in the mesh 
               also, if their quality is below our quality threshold, push them on stack */
            for (j=0; j<newtetcount; j++)
            {
                assert(tetexists(mesh, newtets[j][0], newtets[j][1], newtets[j][2], newtets[j][3]) == 1);
                newqual = tetquality(mesh, newtets[j][0], newtets[j][1], newtets[j][2], newtets[j][3], improvebehave.qualmeasure);
                assert(newqual > 0);
                
                if (tetstack != NULL)
                {
                    /* push this tet on the output stack */
                    stacktet = (struct improvetet *) stackpush(tetstack);
                    stacktet->quality = newqual;
                    stacktet->verts[0] = newtets[j][0];
                    stacktet->verts[1] = newtets[j][1];
                    stacktet->verts[2] = newtets[j][2];
                    stacktet->verts[3] = newtets[j][3];
                }
            }
            
            /* don't check the other edges of this tet */
            break;
        }
    }
    
    if (edgeremoved == 1)
    {
        return 1;
    }
    else
    {
        return 0;   
    }
}

/* try edge removal on all 6 edges of the specified tet. if removal
   succeeds, return 1. Otherwise, return 0. */
int tryedgeremove(struct tetcomplex *mesh,
                   tag vtx1,
                   tag vtx2,
                   tag vtx3,
                   tag vtx4,
                   struct arraypoolstack *tetstack,
                   int ringsizes[MAXRINGTETS],
                   int *biggestring,
                   starreal *minafterqual)
{
    tag tet[4];                   /* the current tet we are trying to improve */
    int i,j,k,l,ringi;            /* loop indices for each tet vertex */
    tag tetring[MAXRINGTETS][4];  /* tetrahedra in a ring around the edge */
    tag vertring[MAXRINGTETS];    /* tags of vertices in ring around edge */
    bool boundedge;               /* whether a particular edge is a boundary edge */
    tag bfv[2];                   /* boundary face vertex tags */
    int ringcount = 0;            /* the number of vertices found in the ring so far */
    starreal minbeforequal;       /* the minimum quality of elements in the ring */
    starreal quality;             /* the quality of the current element in the ring */
    int temp;                     /* holding for variable swap */
    int edgeremoved = 0;
    tag newtets[MAXNEWTETS][4];   /* array to hold the new submesh after edge flipping */
    int newtetcount = 0;          /* number of tets in new arrangement */
    starreal newqual;             /* quality of newly generated tets */
    struct improvetet *stacktet;  /* for pushing newly created tets on the stack */
    bool boundaryedgeremoved = false; /* was boundary edge removal successful? */
    
    /* first, check to make sure this tet still exists in the mesh */
    if (tetexists(mesh, vtx1, vtx2, vtx3, vtx4) == 0)
    {
        return 0;
    }
    
    stats.edgeremovalattempts++;
    
    tet[0] = vtx1;
    tet[1] = vtx2;
    tet[2] = vtx3;
    tet[3] = vtx4;
    
    if (improvebehave.boundedgeremoval)
    {
        boundaryedgeremoved = tryboundaryedgeremove(mesh,
                                                vtx1, vtx2, vtx3, vtx4,
                                                tetstack,
                                                ringsizes,
                                                biggestring,
                                                minafterqual);
                                                
        /* if we removed a boundary edge, don't bother with the rest of the edges */
        if (boundaryedgeremoved) return 1;
    }
    
    /* loop over all 6 edges of the tetrahedron */
    for (i = 0; i < 3; i++) 
    {
        /* don't try other edges if previous one has been removed */
        if (edgeremoved == 1)
        {
            break;
        }
        
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
            
            /* fetch the ring of tets around this edge, checking whether the
               edge is on the boundary */
            boundedge = getedgering(mesh, tet[i], tet[j], tet[k], tet[l],
                                    &ringcount, tetring, bfv);
            
            /* if it's a boundary edge, proceed to the next one */
            if (boundedge) continue;
            
            /* stats stuff */
            stats.ringsizeattempts[ringcount]++;
            stats.edgeremovalattempts++;
            
            /* there should be at least three tets in a ring */
            assert(ringcount >= 3); 
            
            /* build ring of vertices from tet list and determine minimum quality */
            minbeforequal = HUGEFLOAT;
            for (ringi=0; ringi < ringcount; ringi++)
            {
                quality = tetquality(mesh, 
                                     tetring[ringi][0], 
                                     tetring[ringi][1], 
                                     tetring[ringi][2], 
                                     tetring[ringi][3],
                                     improvebehave.qualmeasure);
                if (quality < minbeforequal) minbeforequal = quality;
                vertring[ringi] = tetring[ringi][2];
            }
            
            /* now that we know we have a good ring of vertices around this
               edge, we see if by removing the edge we can improve quality.
               to do this, we find the optimal triangulation of the ring
               of vertices around the edge, and see if the tets induced by
               this triangulation are all of better quality than worst tet
               in the original triangulation */
            edgeremoved = removeedge(mesh, tet[i], tet[j], vertring, ringcount, minbeforequal, newtets, &newtetcount, minafterqual, false);
            assert(newtetcount < MAXNEWTETS); 
            if (edgeremoved == 1)
            {
                /* stats stuff */
                stats.edgeremovals++;
                stats.ringsizesuccess[ringcount]++;
                
                if (ringsizes != NULL)
                {
                    if (ringcount > *biggestring)
                    {
                        *biggestring = ringcount;
                        if (improvebehave.verbosity > 5)
                        {
                            printf("new biggest ring of %d tets\n",*biggestring);
                        }
                    }
                    ringsizes[ringcount-3]++;
                }
                
                if (improvebehave.verbosity > 5)
                {
                    printf("shit! actually removed an edge improving quality from %.17g to %.17g, delta of %.17g\n", pq(minbeforequal), pq(*minafterqual),pq(*minafterqual-minbeforequal));
                    printf("we started with %d tets, and now have %d\n", ringcount, newtetcount);
                    printf("here are the new ones:\n");
                    printtetstags(newtets, newtetcount);
                }
                
                /* check to make sure all of the new tets are positive orientation, and in the mesh 
                   also, if their quality is below our quality threshold, push them on stack */
                for (j=0; j<newtetcount; j++)
                {
                    if (tetexists(mesh, newtets[j][0], newtets[j][1], newtets[j][2], newtets[j][3]) != 1)
                    {
                        printf("Missing new tet in output ring of edge removal!\n");
                        printf("j=%d, newtetcount=%d, MAXRINGTETS=%d\n", j, newtetcount, MAXRINGTETS);
                    }
                    assert(tetexists(mesh, newtets[j][0], newtets[j][1], newtets[j][2], newtets[j][3]) == 1);
                    newqual = tetquality(mesh, newtets[j][0], newtets[j][1], newtets[j][2], newtets[j][3], improvebehave.qualmeasure);
                    assert(newqual > 0);
                    
                    if (tetstack != NULL)
                    {
                        /* push this tet on the output stack */
                        /* push this tet on the output stack */
                        if (tetinstack(tetstack,
                                       newtets[j][0],
                                       newtets[j][1],
                                       newtets[j][2],
                                       newtets[j][3]) == false)
                        {
                            stacktet = (struct improvetet *) stackpush(tetstack);
                            stacktet->quality = newqual;
                            stacktet->verts[0] = newtets[j][0];
                            stacktet->verts[1] = newtets[j][1];
                            stacktet->verts[2] = newtets[j][2];
                            stacktet->verts[3] = newtets[j][3];
                        }
                    }
                }
                
                /* don't check the other edges of this tet */
                break;
            }  
        }
    }
    if (edgeremoved == 1)
    {
        return 1;
    }
    else
    {
        return 0;   
    }
}

bool try22flip(struct tetcomplex *mesh,
               tag face[3],
               tag bot,
               tag top,
               int edge[2],
               tag newtets[][4],
               int *numnewtets,
               bool requireimprove)
{
    int other;                    /* the vertex of the face not on the edge being flipped */
    starreal minbefore, minbefore2; /* qualities of original tets */
    starreal minafter, minafter2;   /* qualities of tets after flip */
    tag outin1[2], outin2[2];     /* for testing if these faces are on the boundary */
    bool foundface;
    
    if (improvebehave.anisotropic) return false;
    
    /* stats update */
    stats.flip22attempts++;
    
    /* compute initial minimum quality */
    minbefore = tetquality(mesh, top, face[0], face[1], face[2], improvebehave.qualmeasure);
    minbefore2 = tetquality(mesh, bot, face[2], face[1], face[0], improvebehave.qualmeasure);
    assert(minbefore > 0 && minbefore2 > 0);
    assert(tetexists(mesh, top, face[0], face[1], face[2]));
    assert(tetexists(mesh, bot, face[2], face[1], face[0]));
    
    if (minbefore2 < minbefore) minbefore = minbefore2;
    
    /* figure out the "other" vertex on the face */
    if (edge[0] != 0 && edge[1] != 0)
    {
        other = 0;
    }
    else
    {
        if (edge[0] != 1 && edge[1] != 1)
        {
            other = 1;
        }
        else
        {
            other = 2;
        }
    }
    assert(other != edge[0] && other != edge[1]);
    
    /* make sure the faces in the 2-2 flip are on the boundary */
    foundface = tetcomplexadjacencies(mesh, top, face[edge[0]], face[edge[1]], outin1);
    assert(foundface);
    foundface = tetcomplexadjacencies(mesh, bot, face[edge[1]], face[edge[0]], outin2);
    assert(foundface);
    
    /* check the first face to make sure it's a boundary tet */
    if (outin1[0] != GHOSTVERTEX || outin2[0] != GHOSTVERTEX)
    {
        /* we can't flip these, give up */
        return false;
    }
    
    /* compute quality of tets after the flip */
    minafter = tetquality(mesh, face[other], face[edge[0]], top, bot, improvebehave.qualmeasure);
    minafter2 = tetquality(mesh, face[other], face[edge[1]], bot, top, improvebehave.qualmeasure);
    
    if (!improvebehave.anisotropic)
    {
        assert(minafter >= 0 && minafter2 >= 0);
    }
    
    /* don't allow zero-quality tets to be created */
    if (minafter < MINTETQUALITY || minafter2 < MINTETQUALITY)
    {
        if (improvebehave.verbosity > 5)
        {
            printf("rejecting tet of quality %g or %g\n", pq(minafter), pq(minafter2));
        }
        return false;
    }
    
    if (minafter2 < minafter) minafter = minafter2;
    
    /* if quality is improved, or if we don't care, do the flip */
    if ((minafter > minbefore) || (requireimprove == false))
    {
        flip22(mesh, face[edge[0]], face[edge[1]], face[other], bot, top, true);
        assert(tetexists(mesh, face[other], face[edge[0]], top, bot));
        assert(tetexists(mesh, face[other], face[edge[1]], bot, top));
        assert(tetquality(mesh, face[other], face[edge[0]], top, bot, improvebehave.qualmeasure) > 0);
        assert(tetquality(mesh, face[other], face[edge[1]], bot, top, improvebehave.qualmeasure) > 0);
        
        /* record the new tets */
        *numnewtets = 2;
        newtets[0][0] = face[other];
        newtets[0][1] = face[edge[0]];
        newtets[0][2] = top;
        newtets[0][3] = bot;
        newtets[1][0] = face[other];
        newtets[1][1] = face[edge[1]];
        newtets[1][2] = bot;
        newtets[1][3] = top;
        
        stats.flip22successes++;
        
        return true;
    }
    return false;
}

/* given an edge (1,2) and a pair of "sandwich" vertices a and b,
   (a above) determine what the neighbor across the edge is.
   if there is none, return GHOSTVERTEX */
tag sandwichneighbor(struct tetcomplex *mesh,
                       tag v1,
                       tag v2,
                       tag a,
                       tag b)
{
    tag anextprev[2]; /* the next and previous vertices from the face (1,2,a) */
    tag bnextprev[2]; /* the next and previous vertices from the face (2,1,b) */
    int foundface;    /* whether a particular face was found */
    
    /* retrieve adjacency info */
    foundface = tetcomplexadjacencies(mesh, v1, v2, a, anextprev);
    assert(foundface == 1);
    foundface = tetcomplexadjacencies(mesh, v2, v1, b, bnextprev);
    assert(foundface == 1);
    
    /* check that the "ancestor" tets exist */
    assert(tetexists(mesh, anextprev[1], v2, v1, a));
    assert(tetexists(mesh, bnextprev[1], v1, v2, b));
    
    /* if the next vertex for both faces is the same, then we've got a sandwich neighbor */
    if (anextprev[0] == bnextprev[0])
    {
        return anextprev[0];
    }
    /* otherwise, no neighbor here */
    return GHOSTVERTEX;
}

/* structure to hold a face, g, and pointers to it's two children */
struct facechildren
{
    tag face[3];
    int child1;
    int child2;
};
#define NOCHILD -1

void printfacetree(struct tetcomplex *mesh,
                   struct facechildren tree[],
                   int treesize)
{
    int i;
    printf("Here is the current face tree of %d faces:\n", treesize);
    
    for (i=0; i<treesize; i++)
    {
        printf("Face %d: (%d %d %d), children %d and %d\n", i, (int) tree[i].face[0], (int) tree[i].face[1], (int) tree[i].face[2], tree[i].child1, tree[i].child2);
    }
}

/* given a face f, an edge uw of that face, and two sandwiching vertices a and b,
   determine if when removing f whether also removing it's neighbor across uw, g,
   would be beneficial. if so, return true. otherwise, return false */
bool testneighbor(struct tetcomplex *mesh,
                  struct facechildren facetree[],
                  int *treesize,
                  tag a,
                  tag b,
                  tag u,
                  tag w,
                  starreal *qold,
                  starreal *qnew,
                  int *g)
{
    starreal quw, qauvw, quvwb;
    starreal ouv, ovw, nuv, nvw;
    int huv;
    int hvw;
    bool uvneighbor, vwneighbor;
    starreal *avtx;
    starreal *bvtx;
    starreal *uvtx;
    starreal *vvtx;
    starreal *wvtx;
    starreal juv, jvw, jwu;
    tag v;
    
    if (improvebehave.verbosity > 5)
    {
        printf("In testneighbor. a=%d, b=%d, u=%d, v=%d\n", (int) a, (int) b, (int) u, (int) w);
    }
    
    /* the quality of this individual tet in the new ring */
    quw = tetquality(mesh, a, b, u, w, improvebehave.qualmeasure);
    
    /* set worst-case return values */
    *g = NOCHILD;
    *qold = HUGEFLOAT;
    *qnew = quw;
    
    /* check if f has a neighbor face across uw */
    v = sandwichneighbor(mesh, u, w, a, b);
    
    /* if not, we certainly can't remove this neighbor */
    if (v == GHOSTVERTEX)
    {
        if (improvebehave.verbosity > 5)
        {
            printf("no neighbor across (%d %d)\n", (int) u, (int) w);
        }
        return false;
    }
    
    /* use orientation tests to confirm that none of the vertices
       u, v, or w lie inside conv({a,b,u,v,w}) */
    avtx = ((struct vertex *) tetcomplextag2vertex(mesh, a))->coord;
    bvtx = ((struct vertex *) tetcomplextag2vertex(mesh, b))->coord;
    uvtx = ((struct vertex *) tetcomplextag2vertex(mesh, u))->coord;
    vvtx = ((struct vertex *) tetcomplextag2vertex(mesh, v))->coord;
    wvtx = ((struct vertex *) tetcomplextag2vertex(mesh, w))->coord;
    juv = orient3d(&behave, avtx, bvtx, uvtx, vvtx);
    jvw = orient3d(&behave, avtx, bvtx, vvtx, wvtx);
    jwu = orient3d(&behave, avtx, bvtx, wvtx, uvtx);
    
    if (((juv > 0) && (jvw > 0)) ||
        ((jvw > 0) && (jwu > 0)) ||
        ((jwu > 0) && (juv > 0)))
    {
        if (improvebehave.verbosity > 5)
        {
            printf("found workable neighbor %d across edge (%d %d)\n", (int) v, (int) u, (int) w);
        }
    }
    else
    {
        if (improvebehave.verbosity > 5)
        {
            printf("can't add face (%d %d %d) to the tree because there's no positively-oriented edges\n", (int) u, (int) v, (int) w);
        }
        return false;
    }
    
    /* now test the two potential children of this face */
    uvneighbor = testneighbor(mesh, facetree, treesize, a, b, u, v, &ouv, &nuv, &huv);
    vwneighbor = testneighbor(mesh, facetree, treesize, a, b, v, w, &ovw, &nvw, &hvw);
    
    /* compute potential qualities */
    qauvw = tetquality(mesh, a, u, v, w, improvebehave.qualmeasure);
    quvwb = tetquality(mesh, u, v, w, b, improvebehave.qualmeasure);
    
    /* qold = min(qauvw, quvwb, ouv, ovw) */
    if (uvneighbor) *qold = ouv;
    if (vwneighbor && (ovw < *qold)) *qold = ovw;
    if (ouv < *qold) *qold = ouv;
    if (ovw < *qold) *qold = ovw;
    
    /* qnew = min(nuv, nvw) */
    *qnew = nuv;
    if (nvw < *qnew) *qnew = nvw;
    
    /* should g be removed if f is ? */
    if ((*qnew > *qold) || (*qnew > quw))
    {
        
        /* yes! add g and g's children to the face tree */
        /* record the vertices of the new face g = (u,v,w),
           whose verts are counterclockwise from a's point of view. */
        *g = (*treesize)++;
        facetree[*g].face[0] = u;
        facetree[*g].face[1] = v;
        facetree[*g].face[2] = w;
        facetree[*g].child1 = huv;
        facetree[*g].child2 = hvw;
        
        return true;
    }
    
    /* g shouldn't be removed, return NOCHILD */
    return false;
}

/* remove face g and, recursively, it's children */
void flip32recurse(struct tetcomplex *mesh,
                   struct facechildren tree[],
                   int treesize,
                   int g,
                   tag a,
                   tag b,
                   tag newtets[][4],
                   int *numnewtets)
{
    tag u, v, w;
    
    /* fetch face g from the tree */
    u = tree[g].face[0];
    v = tree[g].face[1];
    w = tree[g].face[2];
    
    if (improvebehave.verbosity > 5)
    {
        printf("about to do a 3-2 flip on submesh a=%d b=%d u=%d v=%d w=%d\n", (int) a, (int) b, (int) u, (int) v, (int) w);
    }
    
    /* check that the old tets exist */
    assert(tetexists(mesh, a, u, v, w));
    assert(tetexists(mesh, u, v, w, b));
    assert(tetexists(mesh, a, b, u, w));
    
    if (improvebehave.verbosity > 5)
    {
        printf("Here's the submesh before the flip\n");
        printf("{");
        printtetvertssep(mesh, a, u, v, w);
        printf(";\n");
        printtetvertssep(mesh, u, v, w, b);
        printf(";\n");
        printtetvertssep(mesh, a, b, u, w);
        printf("};");
    }
    
    /* remove g with a 3-2 flip */
    flip32(mesh, a, b, v, w, u, true);
    
    /* check that the new tets exist */
    assert(tetexists(mesh, a, b, u, v));
    assert(tetexists(mesh, a, b, v, w));
    
    /* remove children, if any */
    if (tree[g].child1 != NOCHILD)
    {
        flip32recurse(mesh, tree, treesize, tree[g].child1, a, b, newtets, numnewtets);
    }
    else
    /* this is a permanent tet */
    {
        newtets[*numnewtets][0] = a;
        newtets[*numnewtets][1] = b;
        newtets[*numnewtets][2] = u;
        newtets[*numnewtets][3] = v;
        (*numnewtets)++;
    }
    if (tree[g].child2 != NOCHILD)
    {
        flip32recurse(mesh, tree, treesize, tree[g].child2, a, b, newtets, numnewtets);
    }
    else
    /* this is a permanent tet */
    {
        newtets[*numnewtets][0] = a;
        newtets[*numnewtets][1] = b;
        newtets[*numnewtets][2] = v;
        newtets[*numnewtets][3] = w;
        (*numnewtets)++;
    }
}

/* f (u, v, w) is a triangular face under consideration for removal. all faces
   sandwiched between the same two vertices (a and b) as f are considered
   for removal */
bool removemultiface(struct tetcomplex *mesh,
                     tag u,
                     tag v,
                     tag w,
                     int *numfaces,
                     tag newtets[][4],
                     int *numnewtets)
{
    struct facechildren facetree[MAXFACETREESIZE]; /* holds tree of faces to be removed */
    int treesize = 0;
    tag a, b;
    tag abovebelow[2];
    int foundface;
    starreal qnew, qold;
    starreal ouv, ovw, owu, nuv, nvw, nwu, qauvw, quvwb;
    int guv, gvw, gwu;
    int i;
    starreal *vtx1;
    starreal *vtx2;
    starreal *vtx3;
    starreal *vtxa;
    starreal *vtxb;
    starreal *vtxu;
    starreal *vtxv;
    starreal *vtxw;
    bool foundabface = false;
    starreal juv, jvw, jwu;
    
    *numnewtets = 0;
    
    if (improvebehave.verbosity > 5)
    {
        printf("\n\nAttempting multi-face removal on face (%d %d %d)\n", (int) u, (int) v, (int) w);
    }
    
    /* figure out what a and b are with an adjacency query */
    foundface = tetcomplexadjacencies(mesh, u, v, w, abovebelow);
    assert(foundface == 1);
    a = abovebelow[0];
    b = abovebelow[1];
    
    /* if either of the vertices are not there, bail */
    if (a == GHOSTVERTEX || b == GHOSTVERTEX)
    {
        if (improvebehave.verbosity > 5)
        {
            printf("multi-face removal failed because this is a boundary face.\n");
        }
        return false;
    }
    
    vtxa = ((struct vertex *) tetcomplextag2vertex(mesh, a))->coord;
    vtxb = ((struct vertex *) tetcomplextag2vertex(mesh, b))->coord;
    vtxu = ((struct vertex *) tetcomplextag2vertex(mesh, u))->coord;
    vtxv = ((struct vertex *) tetcomplextag2vertex(mesh, v))->coord;
    vtxw = ((struct vertex *) tetcomplextag2vertex(mesh, w))->coord;
    
    /* make sure we don't have two reflex edges on this face with respect to (a,b) */
    juv = orient3d(&behave, vtxa, vtxb, vtxu, vtxv);
    jvw = orient3d(&behave, vtxa, vtxb, vtxv, vtxw);
    jwu = orient3d(&behave, vtxa, vtxb, vtxw, vtxu);
    
    if (((juv > 0) && (jvw > 0)) ||
        ((jvw > 0) && (jwu > 0)) ||
        ((jwu > 0) && (juv > 0)))
    {
        ;
    }
    else
    {
        if (improvebehave.verbosity > 5)
        {
            printf("can't even get started with this face, two reflex edges\n");
        }
        return false;
    }
    
    /* figure out what neighbors might also be removed */
    testneighbor(mesh, facetree, &treesize, a, b, u, v, &ouv, &nuv, &guv);
    testneighbor(mesh, facetree, &treesize, a, b, v, w, &ovw, &nvw, &gvw);
    testneighbor(mesh, facetree, &treesize, a, b, w, u, &owu, &nwu, &gwu);
    
    /* calculate potential qualities */
    qauvw = tetquality(mesh, a, u, v, w, improvebehave.qualmeasure);
    quvwb = tetquality(mesh, u, v, w, b, improvebehave.qualmeasure);
    
    /* qold = min(qauvw, quvwb, ouv, ovw, owu) */
    qold = qauvw;
    if (quvwb < qold) qold = quvwb;
    if (ouv < qold) qold = ouv;
    if (ovw < qold) qold = ovw;
    if (owu < qold) qold = owu;
    
    /* qnew = min(nuv, nvw, nwu) */
    qnew = nuv;
    if (nvw < qnew) qnew = nvw;
    if (nwu < qnew) qnew = nwu;
    
    /* save stats */
    stats.facesizeattempts[treesize+1]++;
    
    /* is the face removal worth it? */
    if (qnew <= qold)
    {
        if (improvebehave.verbosity > 5)
        {
            printf("multi-face removal failed because quality didn't improve\n");
        }
        return false;
    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("woot, old qual = %g, new qual = %g\n", pq(qold), pq(qnew));
    }
    
    /* now, make sure that one of the faces in our tree is the one
       that is pierced by ab */
    /* first check the face itself */
    if ( (orient3d(&behave, vtxa, vtxb, vtxu, vtxv) > 0) &&
         (orient3d(&behave, vtxa, vtxb, vtxv, vtxw) > 0) &&
         (orient3d(&behave, vtxa, vtxb, vtxw, vtxu) > 0))
    {
        foundabface = true;
    }
    else
    /* now look in the tree */
    {
        for (i=0; i<treesize; i++)
        {
            vtx1 = ((struct vertex *) tetcomplextag2vertex(mesh, facetree[i].face[0]))->coord;
            vtx2 = ((struct vertex *) tetcomplextag2vertex(mesh, facetree[i].face[1]))->coord;
            vtx3 = ((struct vertex *) tetcomplextag2vertex(mesh, facetree[i].face[2]))->coord;
        
            /* check if ab is on the positive side of each edge */
            if (orient3d(&behave, vtxa, vtxb, vtx1, vtx2) < 0) continue;
            if (orient3d(&behave, vtxa, vtxb, vtx2, vtx3) < 0) continue;
            if (orient3d(&behave, vtxa, vtxb, vtx3, vtx1) < 0) continue;
        
            /* this face is pierced by ab! sweet. */
            foundabface = true;
            break;
        }
    }
    
    if (foundabface == false)
    {
        if (improvebehave.verbosity > 5)
        {
            printf("Can't do face removal because the face pierced by ab wasn't in tree\n");
            for (i=0; i<treesize; i++)
            {
                printtetvertssep(mesh, a, facetree[i].face[0], facetree[i].face[1], facetree[i].face[2]);
                printf(";\n");
                printtetvertssep(mesh, b, facetree[i].face[2], facetree[i].face[1], facetree[i].face[0]);
                printf(";\n");
            }
            printf("};\n");
        }
        
        return false;
    }
    
    /* remove f and the rest of the faces that are beneficial */
    if (improvebehave.verbosity > 5)
    {
        printf("Yay! we can remove this group of faces. Here are all the tets in the tree:\n");
        printf("{\n");
        for (i=0; i<treesize; i++)
        {
            printtetvertssep(mesh, a, facetree[i].face[0], facetree[i].face[1], facetree[i].face[2]);
            printf(";\n");
            printtetvertssep(mesh, b, facetree[i].face[2], facetree[i].face[1], facetree[i].face[0]);
            printf(";\n");
        }
        printf("};\n");
    }
    
    /* start by removing f with a single 2-3 flip */
    flip23(mesh, u, v, w, b, a, true);
    /* make sure new tets exist */
    assert(tetexists(mesh, a, b, u, v));
    assert(tetexists(mesh, a, b, v, w));
    assert(tetexists(mesh, a, b, w, u));
    
    /* remove children (and their children) */
    if (guv != NOCHILD) 
    {
        flip32recurse(mesh, facetree, treesize, guv, a, b, newtets, numnewtets);
    }
    else
    /* this is a permanent tet */
    {
        newtets[*numnewtets][0] = a;
        newtets[*numnewtets][1] = b;
        newtets[*numnewtets][2] = u;
        newtets[*numnewtets][3] = v;
        (*numnewtets)++;
    }
    if (gvw != NOCHILD) 
    {
        flip32recurse(mesh, facetree, treesize, gvw, a, b, newtets, numnewtets);
    }
    else
    /* this is a permanent tet */
    {
        newtets[*numnewtets][0] = a;
        newtets[*numnewtets][1] = b;
        newtets[*numnewtets][2] = v;
        newtets[*numnewtets][3] = w;
        (*numnewtets)++;
    }
    if (gwu != NOCHILD)
    {
        flip32recurse(mesh, facetree, treesize, gwu, a, b, newtets, numnewtets);
    }
    else
    /* this is a permanent tet */
    {
        newtets[*numnewtets][0] = a;
        newtets[*numnewtets][1] = b;
        newtets[*numnewtets][2] = w;
        newtets[*numnewtets][3] = u;
        (*numnewtets)++;
    }
    
    *numfaces = treesize + 1;
    
    if (improvebehave.verbosity > 5)
    {
        printf("successfully removed %d faces\n", *numfaces);
    }
    
    /* save stats */
    stats.facesizesuccess[treesize+1]++;
    
    return true;
}

#define FLIP23SUCCESS 1
#define FLIP22SUCCESS 2
#define MULTIFACESUCCESS 3

/* attempt a 2-3 flip to remove the face vtx1, vtx2, vtx3. it can be
   required or not that this flip improve the quality of the submesh.
   TODO is this function really necessary? Need to clean up edge removal in general... */
int try23flipold(struct tetcomplex *mesh,
               tag vtx1,
               tag vtx2,
               tag vtx3,
               tag newtets[][4],
               int *numnewtets,
               bool requireimprove)
{
    tag topbot[2]; /* the adjacencies of this face */
    starreal minbeforequal;  /* qualities of tets before the flip */
    starreal minbeforequal2;
    starreal minafterqual;   /* qualities of tets after the flip */
    starreal qual12, qual23, qual31;
    int foundface;
    int numcoplanar;
    int coplanaredges[2][2];
    tag face22[3];
    bool success;
    int numreflex;
    
    face22[0] = vtx1;
    face22[1] = vtx2;
    face22[2] = vtx3;
    
    /* get the top/bottom vertices for the flip */
    foundface = tetcomplexadjacencies(mesh, 
                                      vtx1,
                                      vtx2,
                                      vtx3,
                                      topbot);
    
    /* this face had better exist in the mesh */
    assert(foundface == 1);
    
    /* check to make sure this edge has no reflex edges */
    numreflex = countreflex(mesh, vtx1, vtx2, vtx3, &numcoplanar, coplanaredges);
    
    if (numreflex == 0)
    {
        if (numcoplanar != 0)
        {
            /* try a flip of each of the edges that adjoin
               a pair of coplanar faces */
            if (numcoplanar == 1)
            {
                success = try22flip(mesh, face22, topbot[1], topbot[0], coplanaredges[0], newtets, numnewtets, requireimprove);
                if (success)
                {
                    return FLIP22SUCCESS;
                }
            }
            return false;
        }
        /* only faces with no reflex edges are eligible for 2-3 flips */
        else
        {
            /* compute the quality of the other tets involved in the flip,
               with vertices before the flip, remember the worst */
            minbeforequal = tetquality(mesh, 
                                       topbot[0], 
                                       vtx1,
                                       vtx2,
                                       vtx3,
                                       improvebehave.qualmeasure);
            minbeforequal2 = tetquality(mesh, 
                                        topbot[1], 
                                        vtx3,
                                        vtx2,
                                        vtx1,
                                        improvebehave.qualmeasure);                           
            if (minbeforequal2 < minbeforequal)
            {
                minbeforequal = minbeforequal2;
            }
            
            /* the three tets that would exist after the flip are:
               (top, 1, 2, bot)
               (top, 2, 3, bot)
               (top, 3, 1, bot)
            */
            qual12 = tetquality(mesh, topbot[0], vtx1, vtx2, topbot[1], improvebehave.qualmeasure);
            qual23 = tetquality(mesh, topbot[0], vtx2, vtx3, topbot[1], improvebehave.qualmeasure);
            qual31 = tetquality(mesh, topbot[0], vtx3, vtx1, topbot[1], improvebehave.qualmeasure);
            
            if (qual12 < MINTETQUALITY || qual23 < MINTETQUALITY || qual31 < MINTETQUALITY)
            {
                if (improvebehave.verbosity > 5)
                {
                    printf("In 2-3 flip, rejecting tet of qual %g %g %g\n", pq(qual12), pq(qual23), pq(qual31));
                }
                return false;
            }
            
            /* these should all still be positive-orientation tets */
            assert(qual12 > 0 && qual23 > 0 && qual31 > 0);
            
            minafterqual = qual12;
            if (qual23 < minafterqual)
            {
                minafterqual = qual23;
            }
            if (qual31 < minafterqual)
            {
                minafterqual = qual31;
            }
            
            /* if minimum quality is improved or we don't care, do the flip */
            if ((minbeforequal < minafterqual) || (requireimprove == false))
            {
                /* actually perform the 2-3 flip */
                /* tetcomplex23flip(mesh, tet[j], tet[k], tet[l], topbot[1], topbot[0]); */
                /* use my flip for now TODO: switch back to Jonathan's flip after bug fix */
                flip23(mesh, vtx1, vtx2, vtx3, topbot[1], topbot[0], true);
            
                /* each of these new tets should exist in the mesh now */
                assert(tetexists(mesh, topbot[0], vtx1, vtx2, topbot[1]));
                assert(tetexists(mesh, topbot[0], vtx2, vtx3, topbot[1]));
                assert(tetexists(mesh, topbot[0], vtx3, vtx1, topbot[1]));
                
                /* record identity of new tets */
                *numnewtets = 3;
                newtets[0][0] = topbot[0];
                newtets[0][1] = vtx1;
                newtets[0][2] = vtx2;
                newtets[0][3] = topbot[1];
                newtets[1][0] = topbot[0];
                newtets[1][1] = vtx2;
                newtets[1][2] = vtx3;
                newtets[1][3] = topbot[1];
                newtets[2][0] = topbot[0];
                newtets[2][1] = vtx3;
                newtets[2][2] = vtx1;
                newtets[2][3] = topbot[1];
                
                return FLIP23SUCCESS;
            }
        }
    }
    return false;
}

/* attempt to remove the face (1,2,3). If it has coplanar boundary faces,
   use a 2-2 flip where appropriate. Otherwise, use multi-face removal.
   If a quality-insensitive flip is desired, for now use the old 2-3 single
   flip */
int tryfaceremove(struct tetcomplex *mesh,
               tag vtx1,
               tag vtx2,
               tag vtx3,
               int *numfaces,
               tag newtets[][4],
               int *numnewtets,
               bool requireimprove)
{
    tag topbot[2]; /* the adjacencies of this face */
    int foundface;
    int numcoplanar;
    int coplanaredges[2][2];
    tag face22[3];
    bool success;
    int numreflex;
    
    face22[0] = vtx1;
    face22[1] = vtx2;
    face22[2] = vtx3;
    
    /* get the top/bottom vertices for the flip */
    foundface = tetcomplexadjacencies(mesh, 
                                      vtx1,
                                      vtx2,
                                      vtx3,
                                      topbot);
    
    /* this face had better exist in the mesh */
    if (foundface == false)
    {
        return false;
    }
    
    stats.faceremovalattempts++;
    
    /* check to make sure this face has no reflex edges */
    numreflex = countreflex(mesh, vtx1, vtx2, vtx3, &numcoplanar, coplanaredges);
    
    if (numreflex == 0)
    {
        if (numcoplanar != 0)
        {
            /* try a flip of each of the edges that adjoin
               a pair of coplanar faces */
            if (numcoplanar == 1 && improvebehave.flip22 == 1 && !improvebehave.anisotropic)
            {
                stats.facesizeattempts[1]++;
                success = try22flip(mesh, face22, topbot[1], topbot[0], coplanaredges[0], newtets, numnewtets, requireimprove);
                
                if (success)
                {
                    *numfaces = 1;
                    stats.facesizesuccess[1]++;
                    stats.faceremovals++;
                    return FLIP22SUCCESS;
                }
            }
            return false;
        }
        /* only faces with no reflex edges are eligible for 2-3 flips */
        else
        {
            /* if single face removal is enabled */
            if (improvebehave.singlefaceremoval == 1)
            {
                /* if we want quality-insensitive flip, use old 2-3 flip for now */
                if (requireimprove == false)
                {
                    success = try23flipold(mesh, vtx1, vtx2, vtx3, newtets, numnewtets, requireimprove);
                    if (success) return FLIP23SUCCESS;
                }
                /* if we aren't allowed to do multi-face flips, use old 2-3 flip */
                if (improvebehave.multifaceremoval == 0)
                {
                    stats.facesizeattempts[1]++;
                    success = try23flipold(mesh, vtx1, vtx2, vtx3, newtets, numnewtets, requireimprove);
                    if (success)
                    {
                        stats.facesizesuccess[1]++;
                        stats.faceremovals++;
                        return FLIP23SUCCESS;
                    }
                }
            }
        }
    }
    
    /* attempt multi-face removal, if it is enabled */
    if (improvebehave.multifaceremoval)
    {
        success = removemultiface(mesh, vtx1, vtx2, vtx3, numfaces, newtets, numnewtets);
        if (success)
        {
            stats.faceremovals++;
            return MULTIFACESUCCESS;
        }
    }
    
    return false;
}

/* try a 2-3 flip on all four faces of the specified tet
   returns 1 if a flip was performed, 0 otherwise */
int tryremoveallfaces(struct tetcomplex *mesh,
                      tag vtx1,
                      tag vtx2,
                      tag vtx3,
                      tag vtx4,
                      struct arraypoolstack *tetstack,
                      int facesizes[],
                      int *biggestsize,
                      starreal *outqual,
                      bool requireimprove)
{
    tag tet[4];          /* the current tet we are trying to improve */
    int i,j,k,l,m;       /* loop indices for tet vertices */
    int numflipped=0;    /* did the flip succeed? */
    int result;
    tag topbot[2];
    int numfaces;
    int numnewtets;
    tag newtets[MAXRINGTETS][4];
    starreal newqual;
    struct improvetet *stacktet;  /* for pushing newly created tets on the stack */
    
    *outqual = 1.0;
    
    /* first, check to make sure this tet still exists in the mesh */
    if (tetexists(mesh, vtx1, vtx2, vtx3, vtx4) == 0)
    {
        return 0;
    }
    
    tet[0] = vtx1;
    tet[1] = vtx2;
    tet[2] = vtx3;
    tet[3] = vtx4;
    
    /* check each of this tet's four faces to see if it is eligible for 2-3 flip */
    /* this loop will loop through faces (j,k,l) oriented toward the interior of the tet 
       (node i), from the perspective of the RHR (curl fingers around face vertices, thumb
       points to the interior of the tet). */
    for (i = 0; i < 4; i++) 
    {
        j = (i + 1) & 3;
        if ((i & 1) != 0)
        {
            k = (i + 3) & 3;
            l = (i + 2) & 3;
        } 
        else
        {
            k = (i + 2) & 3;
            l = (i + 3) & 3;
        }
        
        numfaces = 1;
        
        /* check to make sure this face still exists (i.e., hasn't been erased
           in an earlier 2-2 flip) */
        if (tetcomplexadjacencies(mesh, tet[j], tet[k], tet[l], topbot) == 0)
        {
            continue;
        }
        
        /* if we want to be quality insensitive, do an old-school flip */
        if (requireimprove == false)
        {
            result = try23flipold(mesh, tet[j], tet[k], tet[l], newtets, &numnewtets, false);
        }
        else
        {
            /* try to remove this face */
            result = tryfaceremove(mesh, tet[j], tet[k], tet[l], &numfaces, newtets, &numnewtets, requireimprove);
        }
        if (result) numflipped++;
        
        /* if we're keeping statistics, remember how many faces were removed */
        if (facesizes != NULL)
        {
            if (result == FLIP22SUCCESS)
            {
                facesizes[0]++;
            }
            
            if (result == MULTIFACESUCCESS)
            {
                facesizes[numfaces]++;
                if (numfaces > *biggestsize) *biggestsize = numfaces;
            }
        }
        
        /* if we changed some stuff, add it to the output stack */
        if (result)
        {
            for (m=0; m<numnewtets; m++)
            {
                assert(tetexists(mesh, newtets[m][0], newtets[m][1], newtets[m][2], newtets[m][3]) == 1);
                newqual = tetquality(mesh, newtets[m][0], newtets[m][1], newtets[m][2], newtets[m][3], improvebehave.qualmeasure);
                assert(newqual > 0);
                
                if (newqual < *outqual) *outqual = newqual;
                
                if (tetstack != NULL)
                {
                    if (tetinstack(tetstack,
                                   newtets[m][0],
                                   newtets[m][1],
                                   newtets[m][2],
                                   newtets[m][3]) == false)
                    {
                        /* push this tet on the output stack */
                        stacktet = (struct improvetet *) stackpush(tetstack);
                        stacktet->quality = newqual;
                        stacktet->verts[0] = newtets[m][0];
                        stacktet->verts[1] = newtets[m][1];
                        stacktet->verts[2] = newtets[m][2];
                        stacktet->verts[3] = newtets[m][3];
                    }
                }
            }
        }
    }
    
    return numflipped;
}

/* perform a pass of topological improvement.
   for now, this means trying to remove each edge
   of each tet in the stack that is passed in,
   and if no edge can be removed, trying to remove
   each face. */
bool topopass(struct tetcomplex *mesh,
              struct arraypoolstack *tetstack,
              struct arraypoolstack *outstack,
              int qualmeasure,
              starreal bestmeans[],
              starreal meanqualafter[],
              starreal *minqualafter,
              bool quiet)
{
    starreal outquality;           /* the quality of the current tet */
    struct improvetet *stacktet;   /* point to stack tet */
    struct improvetet *outtet;     /* point to stack tet */
    int removed, flipped;          /* was edge / face removal successful */
    int numremoved = 0;            /* number of edges removed */
    int numflipped = 0;            /* number of 2-3 face flips */
    int ringsizes[MAXRINGTETS];    /* histogram of size of rings of tets */
    int biggestring = 0;           /* biggest ring seen */
    int facesizes[MAXFACETREESIZE];/* histogram of face group sizes */
    int biggestface = 0;
    int i;                         /* loop index */
    int origstacksize;             /* number of tets in the original stack */
    int beforeid = lastjournalentry();
    struct arraypoolstack savestack;     /* save the input stack in case of failure */
    starreal minqualbefore, meanqualbefore[NUMMEANTHRESHOLDS];
    starreal minnow;
    bool dynfailcondition = true;
    
    *minqualafter = HUGEFLOAT;
    
    origstacksize = tetstack->top + 1;
    
    if (improvebehave.verbosity > 5 && outstack != NULL)
    {
        printf("here's the input stack:\n");
        printstack(mesh, tetstack);
    }
    /* compute worst input quality. do it global if no output stack */
    if (outstack != NULL)
    {
        stackquality(mesh, tetstack, qualmeasure, meanqualbefore, &minqualbefore);
    }
    else
    {
        meshquality(mesh, qualmeasure, meanqualbefore, &minqualbefore);
    }
    
    /* initialize ring sizes */
    for (i=0; i<MAXRINGTETS; i++)
    {
        ringsizes[i] = 0;
    }
    
    for (i=0; i<MAXFACETREESIZE; i++)
    {
        facesizes[i] = 0;
    }
    
    /* reset output stack */
    if (outstack != NULL)
    {
        stackrestart(outstack);
        /* save a copy of the input stack */
        stackinit(&savestack, sizeof(struct improvetet));
        copystack(tetstack, &savestack);
    }
    
    /* if we aren't allowed to do any topo stuff, don't even bother */
    if (improvebehave.edgeremoval == 0 &&
        improvebehave.singlefaceremoval == 0 &&
        improvebehave.multifaceremoval == 0)
    {
        if (improvebehave.verbosity > 5)
        {
            printf("Not bothering with topo pass, no topo features enabled\n");
        }
        *minqualafter = minqualbefore;
        if (outstack != NULL)
        {
            stackdeinit(outstack);
            memcpy(outstack, &savestack, sizeof(struct arraypoolstack));
            /* make sure the outquality of the saved stack is same as incoming qual */
            assert(worststackquality(mesh, outstack, qualmeasure) == minqualbefore);
        }
        return false;
    }
    
    /* go through each tet on the stack */
    while (tetstack->top != STACKEMPTY)
    {
        /* pull the top tet off the stack */
        stacktet = (struct improvetet *) stackpop(tetstack);
        
        if (improvebehave.edgeremoval)
        {
            /* try edge removal first */
            removed = tryedgeremove(mesh, 
                      stacktet->verts[0], 
                      stacktet->verts[1], 
                      stacktet->verts[2], 
                      stacktet->verts[3], 
                      outstack,
                      ringsizes,
                      &biggestring,
                      &outquality);
        }
        else
        {
            removed = 0;
        }
        
        /* if edge removal failed, try face removal */
        if (removed == 0)
        {
            /* now try multi-face removal */
            flipped = tryremoveallfaces(mesh, 
                  stacktet->verts[0], 
                  stacktet->verts[1], 
                  stacktet->verts[2], 
                  stacktet->verts[3], 
                  outstack,
                  facesizes,
                  &biggestface,
                  &outquality,
                  true);
        }
        else
        {
            flipped = 0;
        }
        
        /* record what happened */
        if (removed == 1)
        {   
            numremoved++;

            if (numremoved % 1000 == 0 && improvebehave.verbosity > 4 && quiet == false)
            {
                printf("removed %d edges. stack currently has %ld tets.\n",numremoved, tetstack->top);
            }
        }
        
        if (flipped > 0)
        {
            numflipped += flipped;
        }
        
        /* if nothing was done to this tet, push it on the output stack */
        if (removed == 0 && flipped == 0 && outstack != NULL)
        {
            /* push this tet on the output stack */
            if (tetinstack(outstack,
                           stacktet->verts[0],
                           stacktet->verts[1],
                           stacktet->verts[2],
                           stacktet->verts[3]) == false)
            {
                outtet = (struct improvetet *) stackpush(outstack);
                outtet->quality = stacktet->quality;
                outtet->verts[0] = stacktet->verts[0];
                outtet->verts[1] = stacktet->verts[1];
                outtet->verts[2] = stacktet->verts[2];
                outtet->verts[3] = stacktet->verts[3];
            }
        }
        
        removed = 0;
        flipped = 0;
    }
    
    if (outstack != NULL)
    {
        if (improvebehave.verbosity > 4 && quiet == false)
        {
            /*
            printf("here's the output stack:\n");
            printstack(mesh, outstack);
            */
            printf("just completed topological improvment pass.\n");
            printf("    input stack had %d tets\n", origstacksize);
            printf("    output stack had %lu tets\n", outstack->top+1);
            printf("    number of edge/face removals is %d/%d\n", numremoved, numflipped);
            printf("    claimed input quality is            %g\n", pq(minqualbefore));
            printf("    worst quality in the output stack is %g\n", pq(worststackquality(mesh, outstack, qualmeasure)));
        }
        stackquality(mesh, outstack, qualmeasure, meanqualafter, minqualafter);
        
        /* check for success for local dynamic improvement */
        if (improvebehave.dynimprove && quiet == false)
        {
            if (localmeanimprove(meanqualbefore, meanqualafter, 0.001) && *minqualafter >= minqualbefore)
            {
                dynfailcondition = false;
            }
        }
        
        /* if we didn't improve, undo this pass and indicate failure */
        if ((*minqualafter < minqualbefore || (numremoved == 0 && numflipped == 0)) && dynfailcondition)
        {
            invertjournalupto(mesh, beforeid);
            /* send back the saved copy of the input stack as the output stack */
            stackdeinit(outstack);
            memcpy(outstack, &savestack, sizeof(struct arraypoolstack));
            /* make sure the outquality of the saved stack is same as incoming qual */
            stackquality(mesh, outstack, qualmeasure, meanqualbefore, &minnow);
            if (minnow != minqualbefore)
            {
                printf("worststackquality = %g, minqualbefore = %g, diff = %g", pq(minnow), pq(minqualbefore), pq(minnow - minqualbefore));
            }
            assert(minnow == minqualbefore);
            *minqualafter = minqualbefore;
            return false;
        }
        
        stackdeinit(&savestack);
        return true;
        
        /*
        *minqualafter = worstquality(mesh, qualmeasure);
        */
    }
    else
    {
        /* what's the worst quality element in the mesh now? */
        meshquality(mesh, qualmeasure, meanqualafter, minqualafter);
    }
    
    if (improvebehave.verbosity > 3 && quiet == false)
    {
        printf("Completed topological improvment pass on stack of %d tets\n", origstacksize);
        if (improvebehave.verbosity > 5)
        {
            printf("    Best previous means:\n");
            printmeans(bestmeans);
            printf("    Mean qualities after:\n");
            printmeans(meanqualafter);
        }
        printf("    Worst quality before: %g\n", pq(minqualbefore));
        if (minqualbefore < *minqualafter)
        {
            textcolor(BRIGHT, GREEN, BLACK);
        }
        printf("    Worst quality after:  %g\n", pq(*minqualafter));
        textcolor(RESET, WHITE, BLACK);
        if (improvebehave.verbosity > 4)
        {
            printf("    Edges removed: %d\n", numremoved);
            printf("    Ring size histogram:\n");
            for (i=0; i<biggestring-2; i++)
            {
                printf("    [%d]: %d\n",i+3,ringsizes[i]);
            }
            printf("    Face removals: %d\n", numflipped);
            printf("      2-2 flips: %d\n", facesizes[0]);
            for (i=1; i<=biggestface; i++)
            {
                printf("    [%d]: %d\n",i,facesizes[i]);
            }
        }
        assert(*minqualafter >= minqualbefore);
    }
    
    if (outstack != NULL)
    {
        stackdeinit(&savestack);
    }
    
    return true;
}

/* for each tet in the stack, try to contract its edges */
bool contractpass(struct tetcomplex *mesh,
                  struct arraypoolstack *tetstack,
                  struct arraypoolstack *outstack,
                  int qualmeasure,
                  starreal bestmeans[],
                  starreal meanqualafter[],
                  starreal *minqualafter,
                  bool justfirstedge,
                  bool quiet)
{
    starreal outquality;           /* the quality of the current tet */
    struct improvetet *stacktet;   /* point to stack tet */
    struct improvetet *outtet;     /* point to stack tet */
    int contracted;                /* was edge / face removal successful */
    int numcontracted = 0;         /* number of edge contractions */
    int origstacksize;             /* number of tets in the original stack */
    int beforeid = lastjournalentry();
    struct arraypoolstack savestack;     /* save the input stack in case of failure */
    starreal minqualbefore, meanqualbefore[NUMMEANTHRESHOLDS];
    starreal minnow;
    
    *minqualafter = HUGEFLOAT;
    
    origstacksize = tetstack->top + 1;
    
    if (improvebehave.verbosity > 5 && outstack != NULL)
    {
        printf("here's the input stack:\n");
        printstack(mesh, tetstack);
    }
    /* compute worst input quality. do it global if no output stack */
    if (outstack != NULL)
    {
        stackquality(mesh, tetstack, qualmeasure, meanqualbefore, &minqualbefore);
    }
    else
    {
        meshquality(mesh, qualmeasure, meanqualbefore, &minqualbefore);
    }
    
    /* reset output stack */
    if (outstack != NULL)
    {
        stackrestart(outstack);
        /* save a copy of the input stack */
        stackinit(&savestack, sizeof(struct improvetet));
        copystack(tetstack, &savestack);
    }
    
    /* go through each tet on the stack */
    while (tetstack->top != STACKEMPTY)
    {
        /* pull the top tet off the stack */
        stacktet = (struct improvetet *) stackpop(tetstack);
        
        /* try to contract this edge */
        contracted = tryedgecontract(mesh,
                                     stacktet->verts[0], 
                                     stacktet->verts[1], 
                                     stacktet->verts[2], 
                                     stacktet->verts[3], 
                                     outstack,
                                     &outquality,
                                     true,
                                     justfirstedge);
        
        if (contracted == 1) numcontracted++;
        
        /* if nothing was done to this tet, push it on the output stack */
        if (contracted == 0 && outstack != NULL)
        {
            /* push this tet on the output stack */
            if (tetinstack(outstack,
                           stacktet->verts[0],
                           stacktet->verts[1],
                           stacktet->verts[2],
                           stacktet->verts[3]) == false)
            {
                /* push this tet on the output stack */
                outtet = (struct improvetet *) stackpush(outstack);
                outtet->quality = stacktet->quality;
                outtet->verts[0] = stacktet->verts[0];
                outtet->verts[1] = stacktet->verts[1];
                outtet->verts[2] = stacktet->verts[2];
                outtet->verts[3] = stacktet->verts[3];
            }
        }
        
        contracted = 0;
    }
    
    if (outstack != NULL)
    {
        if (improvebehave.verbosity > 4)
        {
            /*
            printf("here's the output stack:\n");
            printstack(mesh, outstack);
            */
            printf("just completed contraction improvment pass.\n");
            printf("    input stack had %d tets\n", origstacksize);
            printf("    output stack had %lu tets\n", outstack->top+1);
            printf("    number of edge contractions is %d\n", numcontracted);
            printf("    claimed input quality is            %g\n", pq(minqualbefore));
            printf("    worst quality in the output stack is %g\n", pq(worststackquality(mesh, outstack, qualmeasure)));
        }
        stackquality(mesh, outstack, qualmeasure, meanqualafter, minqualafter);
        
        /* if we didn't improve, undo this pass and indicate failure */
        if (*minqualafter < minqualbefore || (numcontracted == 0))
        {
            invertjournalupto(mesh, beforeid);
            /* send back the saved copy of the input stack as the output stack */
            stackdeinit(outstack);
            memcpy(outstack, &savestack, sizeof(struct arraypoolstack));
            /* make sure the outquality of the saved stack is same as incoming qual */
            stackquality(mesh, outstack, qualmeasure, meanqualbefore, &minnow);
            if (minnow != minqualbefore)
            {
                printf("worststackquality = %g, minqualbefore = %g, diff = %g", pq(minnow), pq(minqualbefore), pq(minnow - minqualbefore));
            }
            assert(minnow == minqualbefore);
            *minqualafter = minqualbefore;
            return false;
        }
        
        stackdeinit(&savestack);
        return true;
        
        /*
        *minqualafter = worstquality(mesh, qualmeasure);
        */
    }
    else
    {
        /* what's the worst quality element in the mesh now? */
        meshquality(mesh, qualmeasure, meanqualafter, minqualafter);
    }
    
    if (improvebehave.verbosity > 3 && quiet == false)
    {
        printf("Completed contraction pass on stack of %d tets\n", origstacksize);
        if (improvebehave.verbosity > 4)
        {
            printf("    Best previous means:\n");
            printmeans(bestmeans);
            printf("    Mean qualities after:\n");
            printmeans(meanqualafter);
        }
        printf("    Worst quality before: %g\n", pq(minqualbefore));
        if (minqualbefore < *minqualafter)
        {
            textcolor(BRIGHT, GREEN, BLACK);
        }
        printf("    Worst quality after:  %g\n", pq(*minqualafter));
        textcolor(RESET, WHITE, BLACK);
        if (improvebehave.verbosity > 4)
        {
            printf("    Edges contracted: %d\n", numcontracted);
        }
        assert(*minqualafter >= minqualbefore);
    }
    
    if (outstack != NULL)
    {
        stackdeinit(&savestack);
    }
    
    return true;
}

/* go after the worst tets with contraction */
void contractworst(struct tetcomplex *mesh,
                    int qualmeasure,
                    starreal percentinsert,
                    starreal bestmeans[],
                    starreal outmeanqual[],
                    starreal *outminqual,
                    bool desperate)
{
    starreal minqual, meanqual[NUMMEANTHRESHOLDS];/* quality of the worst tet in the mesh */
    struct arraypoolstack tetstack;      /* stack of tets  */
    starreal meshworstqual, origmeshworstqual;
    starreal fillthresh;
    
    /* initialize the tet stack */
    stackinit(&tetstack, sizeof(struct improvetet));
    
    meshquality(mesh, qualmeasure, meanqual, &minqual);
    
    if (desperate == false)
    {    
        /* fill the stack of with the worst percent insert tets */
        fillstackpercent(mesh, &tetstack, qualmeasure, 1.0, meanqual, &minqual);
        if (improvebehave.verbosity > 4)
        {
            textcolor(BRIGHT, MAGENTA, BLACK);
            printf("Attempting edge contraction on the worst %g percent of tets (%ld).\n", percentinsert * 100.0, tetstack.top);
            textcolor(RESET, WHITE, BLACK);
        }
    }
    else
    {
        if (minqual + QUALFROMDESPERATE < improvebehave.maxinsertquality[improvebehave.qualmeasure])
        {
            fillthresh = minqual + QUALFROMDESPERATE;
        }
        else if (minqual > improvebehave.maxinsertquality[improvebehave.qualmeasure])
        {
            fillthresh = minqual + QUALUPFROMDESPERATE;
        }
        else
        {
            fillthresh = improvebehave.maxinsertquality[improvebehave.qualmeasure];
        }
        fillstackqual(mesh, &tetstack, qualmeasure, fillthresh, meanqual, &minqual);
        if (improvebehave.verbosity > 4)
        {
            textcolor(BRIGHT, MAGENTA, BLACK);
            printf("Attempting DESPERATE edge contraction on the %ld tets with qual less than %g (%g degrees).\n", tetstack.top, fillthresh, pq(fillthresh));
            textcolor(RESET, WHITE, BLACK);
        }
    }
    
    origmeshworstqual = meshworstqual = minqual;
    
    /* perform insertion pass on stack of tets */
    contractpass(mesh, &tetstack, NULL, qualmeasure, bestmeans, outmeanqual, outminqual, false, false);
    
    /* free the stack of tets */
    stackdeinit(&tetstack);
    
    meshquality(mesh, qualmeasure, outmeanqual, outminqual);
    
    if (!improvebehave.fixedsmooth) 
    {
        if (*outminqual < origmeshworstqual)
        {
            printf("crap, mesh min qual = %g after contract pass, %g before.\n", pq(*outminqual), pq(origmeshworstqual));
        }
        assert(*outminqual >= origmeshworstqual);
    }
    
    /* print report */
    if (improvebehave.verbosity > 3)
    {
        if (meshworstqual > origmeshworstqual)
        {
            textcolor(BRIGHT, GREEN, BLACK);
            printf("    Worst angle in mesh improved from %g to %g degrees\n", pq(origmeshworstqual), pq(meshworstqual));
            textcolor(RESET, WHITE, BLACK);
        }
    }
}

  