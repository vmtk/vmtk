/*****************************************************************************/
/*                                                                           */
/*  Vertex insertion routines                                                */
/*                                                                           */
/*****************************************************************************/

/* check an array of tets to see if it contains the query tet */
bool tetinlist(tag v1,
               tag v2,
               tag v3,
               tag v4,
               tag list[][4],
               int listsize)
{
    int i;
    
    for (i=0; i<listsize; i++)
    {
        if (sametet(v1, v2, v3, v4, list[i][0], list[i][1], list[i][2], list[i][3]))
        {
            return true;
        }
    }
    
    return false;
}

/* check an array of faces to see if it contains the query face */
bool faceinlist(tag v1,
                tag v2,
                tag v3,
                tag list[][3],
                int listsize)
{
    int i;
    
    for (i=0; i<listsize; i++)
    {
        if (sameface(v1, v2, v3, list[i][0], list[i][1], list[i][2]))
        {
            return true;
        }
    }
    
    return false;
}

/* removed the specified face from a face list */
void deletelistface(tag v1,
                    tag v2,
                    tag v3,
                    tag list[][3],
                    int *listsize)
{
    int i,j;
    bool found = false;
    
    /* find face in the list */
    for (i=0; i<*listsize; i++)
    {
        if (sameface(v1, v2, v3, list[i][0], list[i][1], list[i][2]))
        {
            found = true;
            break;
        }
    }
    
    assert(found);
    
    /* slide all the rest of the faces back */
    for (j=i; i<*listsize-1; i++)
    {
        list[i][0] = list[i+1][0];
        list[i][1] = list[i+1][1];
        list[i][2] = list[i+1][2];
    }
    
    /* decrement the list size */
    (*listsize)--;
}

/* removed the specified tet from a tet list */
void deletelisttet(tag v1,
                   tag v2,
                   tag v3,
                   tag v4,
                   tag list[][4],
                   int *listsize)
{
    int i,j;
    bool found = false;
    
    /* find face in the list */
    for (i=0; i<*listsize; i++)
    {
        if (sametet(v1, v2, v3, v4, list[i][0], list[i][1], list[i][2], list[i][3]))
        {
            found = true;
            break;
        }
    }
    
    assert(found);
    
    /* slide all the rest of the faces back */
    for (j=i; i<*listsize-1; i++)
    {
        list[i][0] = list[i+1][0];
        list[i][1] = list[i+1][1];
        list[i][2] = list[i+1][2];
        list[i][3] = list[i+1][3];
    }
    
    /* decrement the list size */
    (*listsize)--;
}

/* add the specified tet to a list */
void addlisttet(tag v1,
                tag v2,
                tag v3,
                tag v4,
                tag list[][4],
                int *listsize)
{
    /* check that we're not adding a duplicate */
    assert(tetinlist(v1, v2, v3, v4, list, *listsize) == false);
    
    list[*listsize][0] = v1;
    list[*listsize][1] = v2;
    list[*listsize][2] = v3;
    list[*listsize][3] = v4;
    (*listsize)++;
}

/* add the specified face to a list */
void addlistface(tag v1,
                 tag v2,
                 tag v3,
                 tag list[][3],
                 int *listsize)
{
    list[*listsize][0] = v1;
    list[*listsize][1] = v2;
    list[*listsize][2] = v3;
    (*listsize)++;
}

#define NOCAVITYTET -1
#define NOCAVITYFACE -1

struct cavityface
{
    tag verts[3];
    starreal qual;
    int child;
    bool inH;
};

struct cavityedge
{
    starreal qual;
    int parent;
    int child;
    int childnum;
};

#define EDGELABEL 0
#define TETLABEL 1

struct cavityedgeortet
{
    starreal qual;
    int label;
    int parent;
    int child;
    int childnum;
};

struct cavitytet
{
    tag verts[4];
    starreal qual;
    int numoutfaces;
    int depth;
    struct cavityface outfaces[MAXOUTFACES];
    int parents[3];
    int label;
};

void printcavitytet(struct cavitytet *tet)
{
    int i;
    
    printf("    Cavity tet with verts (%d %d %d %d)\n", (int) tet->verts[0], (int) tet->verts[1], (int) tet->verts[2], (int) tet->verts[3]);
    printf("    Label: %d\n", tet->label);
    printf("    Quality: %g\n", pq(tet->qual));
    printf("    Depth: %d\n", tet->depth);
    printf("    Parents are %d, %d, and %d\n", tet->parents[0], tet->parents[1], tet->parents[2]);
    printf("    Has %d outgoing faces:\n", tet->numoutfaces);
    for(i=0; i<tet->numoutfaces; i++)
    {
        printf("        Face %d (%d %d %d) has quality %g and child %d\n", 
                        i, (int) tet->outfaces[i].verts[0], (int) tet->outfaces[i].verts[1], (int) tet->outfaces[i].verts[2],
                        pq(tet->outfaces[i].qual), tet->outfaces[i].child);
    }
}

void printcavitydag(struct tetcomplex *mesh,
                    struct cavitytet cavity[],
                    int cavitysize)
{
    int i;
    
    for (i=0; i<cavitysize; i++)
    {
        printf("Tet %d/%d:\n", i, cavitysize-1);
        printcavitytet(&cavity[i]);
    }
    
    /*printf("{");
    for (i=0; i<cavitysize; i++)
    {
        printtetverts(mesh, cavity[i].verts);
        printf(";\n");
    }
    printf("};\n");*/
}

/* check an array of tets to see if it contains the query tet.
   if so, return its location. if not, return NOCAVITYTET */
int cavitytetinlist(tag v1,
                    tag v2,
                    tag v3,
                    tag v4,
                    struct cavitytet list[],
                    int listsize)
{
    int i;
    
    for (i=0; i<listsize; i++)
    {
        if (sametet(v1, v2, v3, v4,
                    list[i].verts[0],
                    list[i].verts[1],
                    list[i].verts[2],
                    list[i].verts[3]))
        {
            return i;
        }
    }
    
    return NOCAVITYTET;
}

/* check a tet to see if it contains the query face.
   if so, return its location. if not, return NOCAVITYFACE */
int cavityfaceintet(tag v1,
                    tag v2,
                    tag v3,
                    struct cavitytet *tet)
{
    int i;
    int numfaces = tet->numoutfaces;
    
    for (i=0; i<numfaces; i++)
    {
        if (sameface(v1, v2, v3,
                     tet->outfaces[i].verts[0],
                     tet->outfaces[i].verts[1],
                     tet->outfaces[i].verts[2]))
        {
            return i;
        }
    }
    
    return NOCAVITYFACE;
}

/* add a tet to the cavity */
void addcavitytet(struct cavitytet *tet,
                  struct cavitytet list[],
                  int *listsize)
{
    /* copy the tet into the list */
    memcpy(&list[*listsize], tet, sizeof(struct cavitytet));
    /* increment list size */
    (*listsize)++;
}

/* return the number of parents a tet has */
int numparents(struct cavitytet *tet)
{
    int numparents = 0;
    if (tet->parents[0] != NOCAVITYTET) numparents++;
    if (tet->parents[1] != NOCAVITYTET) numparents++;
    if (tet->parents[2] != NOCAVITYTET) numparents++;
    return numparents;
}

/* add an outgoing face to a cavity tet */
void addcavitytetface(struct cavitytet *tet,
                      struct cavityface *face)
{
    int whichface, parentcount = 0;
    
    /* check if this face already exists (i.e., we're updating it) */
    whichface = cavityfaceintet(face->verts[0], 
                                face->verts[1], 
                                face->verts[2], 
                                tet);
    
    /* we can't add a new face if there are already 3 */
    if (whichface == NOCAVITYFACE)
    {
        /* make sure that the sum of the parents and outbound faces is 4 */
        parentcount = numparents(tet);
        /* allow more than 3 outfaces for parentless tets */
        if (parentcount != 0)
        {
            if (parentcount + tet->numoutfaces > 3)
            {
                printf("adding face (%d %d %d) with child %d would cause in+out to exceed 4.\n", (int) face->verts[0], (int) face->verts[1], (int) face->verts[2], face->child);
                printcavitytet(tet);
            }
            assert(parentcount + tet->numoutfaces < 4);
        }
        whichface = tet->numoutfaces;
        (tet->numoutfaces)++;
    }
    
    assert(tet->numoutfaces <= MAXOUTFACES);
    
    /* copy in the face */
    memcpy(&tet->outfaces[whichface], face, sizeof(struct cavityface));
}

/* return the number of faces of the tet t that are positively
   oriented with respect to the vertex v */
int numpositivefaces(struct tetcomplex * mesh,
                     tag t[4],
                     starreal *v)
{
    starreal *c[4];
    int numpositive = 0;
    
    /* fetch coordinates of tet vertices */
    c[0] = ((struct vertex *) tetcomplextag2vertex(mesh, t[0]))->coord;
    c[1] = ((struct vertex *) tetcomplextag2vertex(mesh, t[1]))->coord;
    c[2] = ((struct vertex *) tetcomplextag2vertex(mesh, t[2]))->coord;
    c[3] = ((struct vertex *) tetcomplextag2vertex(mesh, t[3]))->coord;
    
    /* test each faces orientation */
    /* faces are:
       (1, 2, 3), (0, 3, 2), (0, 1, 3), (0, 2, 1) */
    if (orient3d(&behave, v, c[1], c[2], c[3]) > MINFACING) numpositive++;
    if (orient3d(&behave, v, c[0], c[3], c[2]) > MINFACING) numpositive++;
    if (orient3d(&behave, v, c[0], c[1], c[3]) > MINFACING) numpositive++;
    if (orient3d(&behave, v, c[0], c[2], c[1]) > MINFACING) numpositive++;
    
    /* we can have at most 3 positively oriented faces */
    assert(numpositive < 4);
    
    if (improvebehave.verbosity > 5)
    {
        printf("Tet checked has %d faces oriented toward v\n", numpositive);
    }
    
    return numpositive;
}

/* check the DAG to make sure it's constructed correctly */
bool cavitydagcheck(struct tetcomplex *mesh,
                    struct cavitytet cavity[],
                    int cavitysize)
{
    int i,j,k, parentcount, child;
    tag othervert;
    
    for (i=0; i<cavitysize; i++)
    {
        if (improvebehave.verbosity > 5)
        {
            printf("checking out tet %d\n", i);
            printcavitytet(&cavity[i]);
        }
        
        assert(tetexists(mesh, 
                         cavity[i].verts[0],
                         cavity[i].verts[1],
                         cavity[i].verts[2],
                         cavity[i].verts[3]));
        
        parentcount = numparents(&cavity[i]);
        
        /* let "no parent" tets get by */
        if (parentcount == 0)
        {
            continue;
        } 
        
        /* check that numparents + numoutfaces = 4 
           (i.e., all faces of tet are accounted for) */
        assert(parentcount > 0);
        assert(parentcount + cavity[i].numoutfaces == 4);
        
        for (j=0; j<cavity[i].numoutfaces; j++)
        {
            /* check that the face has a non-negative quality */
            assert(cavity[i].outfaces[j].qual >= 0);
            
            /* find the vertex not in this face */
            othervert = GHOSTVERTEX;
            for (k=0; k<4; k++)
            {
                if ((cavity[i].verts[k] != cavity[i].outfaces[j].verts[0]) &&
                    (cavity[i].verts[k] != cavity[i].outfaces[j].verts[1]) &&
                    (cavity[i].verts[k] != cavity[i].outfaces[j].verts[2]))
                {
                    othervert = cavity[i].verts[k];
                }
            }
            assert(othervert != GHOSTVERTEX);
            
            /* check that the face is oriented outward */
            assert(tetexists(mesh,
                             othervert, 
                             cavity[i].outfaces[j].verts[2],
                             cavity[i].outfaces[j].verts[1],
                             cavity[i].outfaces[j].verts[0]));
            
            /* check that each child of this tet has this tet for a parent */
            child = cavity[i].outfaces[j].child;
            if (child != NOCAVITYTET)
            {
                assert(cavity[child].parents[0] == i || 
                       cavity[child].parents[1] == i ||
                       cavity[child].parents[2] == i);
            }
        }
    }
    
    return true;
}

/* given a vertex, it's position, an initial cavitiy of tets and 
   a set of outward-oriented faces of that cavity, build a DAG
   representing the largest star-shaped cavity from the point of
   view of the inserted vertex */
void buildcavitydag(struct tetcomplex *mesh,
                    tag vtag,
                    tag initialC[][4],
                    int initialCsize,
                    tag initialF[][3],
                    int initialFsize,
                    struct cavitytet outcavity[],
                    int *cavitysize,
                    bool allowvertexdeletion)
{
    static tag F[MAXCAVITYFACES][3]; /* candidate face list */
    static tag W[MAXCAVITYFACES][3]; /* wall face list */
    static tag C[MAXCAVITYTETS][4];  /* cavity tet list */
    static tag B[MAXCAVITYTETS][4];  /* blocking tet list */
    int Fsize=0, Wsize=0, Csize=0, Bsize=0;
    tag t[4];                 /* current tet */
    tag f[3];                 /* current face */
    tag outin[2];
    tag outin2[2];
    starreal *c[4];
    starreal *v;
    bool foundface;
    int Wcount = 0;
    bool facing = true;
    int i,j;
    tag otherfaces[3][3] = {{2,1,0},{3,2,0},{0,1,3}};
    int deepest = 0;
    int numnonwall = 0;
    tag nonwall[3];
    int ii;
    
    int depthlimit = improvebehave.cavdepthlimit;
    
    /* output cavity stuff */
    struct cavitytet cavtet;
    struct cavityface cavface;
    struct cavityface cavface2;
    int tetindex, parentindex;
    
    *cavitysize = 0;
    
    /* fetch position of new vertex */
    v = ((struct vertex *) tetcomplextag2vertex(mesh, vtag))->coord;
    
    /* initialize cavity tet list */
    for (i=0; i<initialCsize; i++)
    {
        addlisttet(initialC[i][0], initialC[i][1], initialC[i][2], initialC[i][3], C, &Csize);
        
        /* this tet is sure to be in the final cavity, add it */
        cavtet.verts[0] = initialC[i][0];
        cavtet.verts[1] = initialC[i][1];
        cavtet.verts[2] = initialC[i][2];
        cavtet.verts[3] = initialC[i][3];
        /* fake the qual because these tets were already split and made worse */
        cavtet.qual = 1.0;
        cavtet.depth = 0;
        cavtet.parents[0] = NOCAVITYTET;
        cavtet.parents[1] = NOCAVITYTET;
        cavtet.parents[2] = NOCAVITYTET;
        cavtet.numoutfaces = 0;
        
        addcavitytet(&cavtet, outcavity, cavitysize);
    }
    
    /* initialize candidate face list */
    for (i=0; i<initialFsize; i++)
    {
        addlistface(initialF[i][0], initialF[i][1], initialF[i][2], F, &Fsize);
    }
    
    /* now, as long as we have candidate faces */
    while (Fsize > 0)
    {
        assert(*cavitysize <= MAXCAVITYTETS);
        assert(Fsize <= MAXCAVITYFACES);
        assert(Wsize <= MAXCAVITYFACES);
        assert(Csize <= MAXCAVITYTETS);
        assert(Bsize <= MAXCAVITYTETS);
        
        if (improvebehave.verbosity > 5)
        {
            printf("going through face list, size is %d\n", Fsize);
            printf("Csize = %d, Bsize = %d, Wsize = %d\n", Csize, Bsize, Wsize);
        }
        
        /* pull a face out of F */
        f[0] = F[Fsize-1][0];
        f[1] = F[Fsize-1][1];
        f[2] = F[Fsize-1][2];
        Fsize--;
        
        if (improvebehave.verbosity > 5)
        {
            printf("just pulled face (%d %d %d) out of F\n", (int) f[0], (int) f[1], (int) f[2]);
            printf("here's F (size = %d):\n", Fsize);
            for (ii=0; ii<Fsize; ii++)
            {
                printf("%d: %d %d %d\n", ii, (int) F[ii][0], (int) F[ii][1], (int) F[ii][2]);
            }
        }
        
        /* get t, the tet on the other side of this face */
        foundface = tetcomplexadjacencies(mesh, f[0], f[1], f[2], outin);
        if (foundface == 0)
        {
            printf("couldn't find face (%d %d %d)", (int) f[0], (int) f[1], (int) f[2]);
        }
        assert(foundface);
        /* make sure the parent tet for this face is there */
        assert(outin[1] != GHOSTVERTEX);
        
        /* the inward facing tet should already be in the output cavity.
           find it, and add this face as an outgoing face */
        tetindex = cavitytetinlist(outin[1], f[2], f[1], f[0], outcavity, *cavitysize);
        assert(tetindex != NOCAVITYTET);
        
        /* compute the quality of the cavity tet with this face */
        cavface.verts[0] = f[0];
        cavface.verts[1] = f[1];
        cavface.verts[2] = f[2];
        cavface.qual = tetquality(mesh, vtag, f[2], f[1], f[0], improvebehave.qualmeasure);
        assert(cavface.qual > 0);
        
        /* check to make sure it's not a ghost vertex */
        if (outin[0] == GHOSTVERTEX)
        {
            /* note that this face has no child, and assign it to its parent tet */
            cavface.child = NOCAVITYTET;
            addcavitytetface(&outcavity[tetindex], &cavface);
            
            /* add this face to the wall list */
            addlistface(f[0], f[1], f[2], W, &Wsize);
            continue;
        }
        
        t[0] = outin[0];
        t[1] = f[0];
        t[2] = f[1];
        t[3] = f[2];
        
        if (improvebehave.verbosity > 5)
        {
            printf("investigating tet (%d %d %d %d)\n", (int) t[0], (int) t[1], (int) t[2], (int) t[3]);
        }
        
        /* fetch positions of vertices */
        c[0] = ((struct vertex *) tetcomplextag2vertex(mesh, t[0]))->coord;
        c[1] = ((struct vertex *) tetcomplextag2vertex(mesh, t[1]))->coord;
        c[2] = ((struct vertex *) tetcomplextag2vertex(mesh, t[2]))->coord;
        c[3] = ((struct vertex *) tetcomplextag2vertex(mesh, t[3]))->coord;
        
        /* is t a cavity tet? */
        if (tetinlist(t[0], t[1], t[2], t[3], C, Csize))
        {
            if (improvebehave.verbosity > 4)
            {
                printf("it's already a cavity tet. can this happen?\n");
            }
            starexit(1);
            
            /* we need to add this face to the parent tet, indicating 
               that it has no child because the tet on the other side
               doesn't depend in it's removal to exist */
            cavface.child = NOCAVITYTET;
            addcavitytetface(&outcavity[tetindex], &cavface);
            
            /* yes, do nothing */
            continue;
        }
        
        /* is t a blocking tet? */
        if (tetinlist(t[0], t[1], t[2], t[3], B, Bsize))
        {
            if (improvebehave.verbosity > 5)
            {
                printf("it's a blocking tet.\n");
            }
            
            /* if there is one other wall face of this tet, and the other two faces
               are visible from v, we can add this tet to the cavity. */
            Wcount = 1;
            facing = true;
            
            for (i=0; i<3; i++)
            {
                /* is this face already marked as a wall ? */
                if (faceinlist(t[otherfaces[i][0]], t[otherfaces[i][1]], t[otherfaces[i][2]], W, Wsize))
                {
                    Wcount++;
                }
                else
                /* it's not a wall... is it oriented toward v? */
                {
                    if (orient3d(&behave, v, c[otherfaces[i][0]], c[otherfaces[i][1]], c[otherfaces[i][2]]) <= MINFACING)
                    {
                        facing = false;
                    }
                }
            }
            
            if (improvebehave.verbosity > 5)
            {
                printf("*** facing = %d, wcount = %d\n", facing, Wcount);
            }
            
            /* only allow tets with three parents if we are allowing vertex deletion */
            if ((Wcount == 2 || (Wcount == 3 && allowvertexdeletion)) && facing)
            {
                if (Wcount == 2)
                {
                    if (improvebehave.verbosity > 5)
                    {
                        printf("found a 2-wall face tet to add!\n");
                    }
                }
                else
                {
                    if (improvebehave.verbosity > 5)
                    {
                        printf("found a 3-wall face tet to add!\n");
                    }
                }
                
                /* this tet can be added to the cavity */
                
                /* remove it from B */
                deletelisttet(t[0], t[1], t[2], t[3], B, &Bsize);
                /* add it to C */
                assert(tetexists(mesh, t[0], t[1], t[2], t[3]));
                addlisttet(t[0], t[1], t[2], t[3], C, &Csize);
                
                /* add this tet to the output cavity */
                cavtet.verts[0] = t[0];
                cavtet.verts[1] = t[1];
                cavtet.verts[2] = t[2];
                cavtet.verts[3] = t[3];
                /* compute it's original quality */
                cavtet.qual = tetquality(mesh, t[0], t[1], t[2], t[3], improvebehave.qualmeasure);
                assert(cavtet.qual > 0);
                /* we know one parent must be the one we found above */
                cavtet.parents[0] = tetindex;
                /* the depth is one more than the parent depth */
                cavtet.depth = outcavity[tetindex].depth + 1;
                /* if this is a new deepest, remember it */
                if (cavtet.depth > deepest) deepest = cavtet.depth;
                addcavitytet(&cavtet, outcavity, cavitysize);
                
                /* add this face to the parent tet with the correct child */
                cavface.child = *cavitysize - 1;
                addcavitytetface(&outcavity[tetindex], &cavface);
                
                /* remove any faces that were in W, add others to F. Handle output
                   tet faces that need to be added */
                
                numnonwall = 0;
                /* first, handle all wall face so we can set the correct depth */
                for (i=0; i<3; i++)
                {
                    /* is this already a wall face? */
                    if (faceinlist(t[otherfaces[i][0]], t[otherfaces[i][1]], t[otherfaces[i][2]], W, Wsize))
                    {
                        deletelistface(t[otherfaces[i][0]], t[otherfaces[i][1]], t[otherfaces[i][2]], W, &Wsize);
                        
                        /* because this face was previously a wall face,
                           it has some cavity tet that it belongs to. find
                           this tet in the output cavity and set it's child face */
                        
                        foundface = tetcomplexadjacencies(mesh,   
                                                          t[otherfaces[i][0]], 
                                                          t[otherfaces[i][1]], 
                                                          t[otherfaces[i][2]], 
                                                          outin2);
                        assert(foundface);
                        assert(outin2[1] != GHOSTVERTEX);
                        
                        parentindex = cavitytetinlist(outin2[1], 
                                                      t[otherfaces[i][2]], 
                                                      t[otherfaces[i][1]], 
                                                      t[otherfaces[i][0]], 
                                                      outcavity, *cavitysize);
                        assert(parentindex != NOCAVITYTET);
                    
                        /* add this face to the parent tet's outgoing faces */
                        cavface.verts[0] = t[otherfaces[i][0]];
                        cavface.verts[1] = t[otherfaces[i][1]];
                        cavface.verts[2] = t[otherfaces[i][2]];
                        cavface.qual = tetquality(mesh, vtag, 
                                                  t[otherfaces[i][2]], 
                                                  t[otherfaces[i][1]], 
                                                  t[otherfaces[i][0]], 
                                                  improvebehave.qualmeasure);
                        assert(cavface.qual > 0);
                        cavface.child = *cavitysize - 1;
                        /* make sure that this face is already in this tet */
                        assert(cavityfaceintet(cavface.verts[0], 
                                               cavface.verts[1], 
                                               cavface.verts[2], 
                                               &outcavity[parentindex]) != NOCAVITYFACE);
                        addcavitytetface(&outcavity[parentindex], &cavface);
                        
                        /* assign the parent tet as the second parent of the new cavity tet */
                        if (outcavity[*cavitysize -1].parents[1] == NOCAVITYTET)
                        {
                            outcavity[*cavitysize - 1].parents[1] = parentindex;
                        }
                        else
                        {
                            assert(outcavity[*cavitysize -1].parents[2] == NOCAVITYTET);
                            outcavity[*cavitysize - 1].parents[2] = parentindex;
                        }
                        
                        /* if this parent has a lesser depth value, update new tet's depth to be the lesser */
                        if (outcavity[parentindex].depth < outcavity[*cavitysize - 1].depth)
                        {
                            if (improvebehave.verbosity > 5)
                            {
                                printf("changing depth from %d to %d, this parent is shallower\n", outcavity[*cavitysize - 1].depth, outcavity[parentindex].depth);
                            }
                            outcavity[*cavitysize - 1].depth = outcavity[parentindex].depth;
                        }
                    }
                    else
                    {
                        /* record this non-wall face for potential addition to F later */
                        nonwall[numnonwall] = i;
                        numnonwall++;
                    }
                }
                
                for (i=0; i<numnonwall; i++)
                {
                    j = nonwall[i];
                    
                    /* this is a newly-uncovered face. there could be more tets behind it, so
                       we should add it to F, if the current tet's depth isn't more than the max */
                    if (outcavity[*cavitysize -1].depth < depthlimit)
                    {
                        if (improvebehave.verbosity > 5)
                        {
                            printf("adding new face uncovered in 2-parent tet to F (%d %d %d)\n", (int) t[otherfaces[j][2]], (int) t[otherfaces[j][1]], (int) t[otherfaces[j][0]]);
                        }
                        addlistface(t[otherfaces[j][2]], t[otherfaces[j][1]], t[otherfaces[j][0]], F, &Fsize);
                    }
                    /* we should artificially make this a wall face so the cavity doesn't get deeper */
                    else
                    {
                        if (improvebehave.verbosity > 5)
                        {
                            printf("adding new face uncovered in 2-parent tet to W because of depth limit (%d %d %d)\n", (int) t[otherfaces[j][2]], (int) t[otherfaces[j][1]], (int) t[otherfaces[j][0]]);
                        }
                        
                        /* construct output face */
                        cavface2.verts[0] = t[otherfaces[j][2]];
                        cavface2.verts[1] = t[otherfaces[j][1]];
                        cavface2.verts[2] = t[otherfaces[j][0]];
                        cavface2.qual = tetquality(mesh, vtag, cavface2.verts[2], cavface2.verts[1], cavface2.verts[0], improvebehave.qualmeasure);
                        cavface2.child = NOCAVITYTET;
                        assert(cavface2.qual > 0);
                        
                        /* add it to parent tet */
                        addcavitytetface(&outcavity[*cavitysize -1], &cavface2);
                        
                        addlistface(t[otherfaces[j][2]], t[otherfaces[j][1]], t[otherfaces[j][0]], W, &Wsize);
                    }
                }
                
                if (improvebehave.verbosity > 5)
                {
                    printf("just added this 2-wall-face cavity tet (tet %d):\n", *cavitysize - 1);
                    printcavitytet(&outcavity[*cavitysize - 1]);
                }
                assert(outcavity[*cavitysize - 1].parents[0] != NOCAVITYTET);
                assert(outcavity[*cavitysize - 1].parents[1] != NOCAVITYTET);
            }
            else
            {
                if (improvebehave.verbosity > 5)
                {
                    printf("found a blocking tet that didn't work out! facing = %d, wcount = %d\n", facing, Wcount);
                }
                
                /* note that this face has no child, and assign it to its parent tet */
                cavface.child = NOCAVITYTET;
                addcavitytetface(&outcavity[tetindex], &cavface);
                
                /* add f to W, it borders a blocking tet */
                addlistface(f[0], f[1], f[2], W, &Wsize);
            }
            continue;
        }
        
        if (improvebehave.verbosity > 5)
        {
                printf("it's not in B or C.\n");
        }
        
        /* t is neither a blocking tet nor a cavity tet */
        /* check to see if the three other faces of the tet are facing v */
        if ((orient3d(&behave, v, c[2], c[1], c[0]) > MINFACING) &&
            (orient3d(&behave, v, c[3], c[2], c[0]) > MINFACING) &&
            (orient3d(&behave, v, c[0], c[1], c[3]) > MINFACING))
        {
            if (improvebehave.verbosity > 5)
            {
                printf("Adding tet to cavity\n");
            }
            
            /* yes! we can add this tet to the cavity */
            assert(tetexists(mesh, t[0], t[1], t[2], t[3]));
            /* make sure this tet has more than one face oriented toward the new vertex */
            assert(numpositivefaces(mesh, t, v) > 1);
            
            addlisttet(t[0], t[1], t[2], t[3], C, &Csize);
            
            /* add this tet to the output cavity */
            cavtet.verts[0] = t[0];
            cavtet.verts[1] = t[1];
            cavtet.verts[2] = t[2];
            cavtet.verts[3] = t[3];
            /* compute it's original quality */
            cavtet.qual = tetquality(mesh, t[0], t[1], t[2], t[3], improvebehave.qualmeasure);
            assert(cavtet.qual > 0);
            /* it's parent must be the parent above */
            cavtet.parents[0] = tetindex;
            /* depth is one deeper than parent */
            cavtet.depth = outcavity[tetindex].depth + 1;
            /* if this is a new deepest, note it */
            if (cavtet.depth > deepest) deepest = cavtet.depth;
            
            addcavitytet(&cavtet, outcavity, cavitysize);
            
            /* note the current face's child in the parent tet */
            cavface.child = *cavitysize - 1;
            addcavitytetface(&outcavity[tetindex], &cavface);
            
            /* add t's three (outward oriented) faces to F, if the current tet isn't too deep */
            if (cavtet.depth < depthlimit)
            {
                addlistface(t[0], t[1], t[2], F, &Fsize);
                addlistface(t[0], t[2], t[3], F, &Fsize);
                addlistface(t[0], t[3], t[1], F, &Fsize);
            }
            else
            {
                if (improvebehave.verbosity > 5)
                {
                    printf("not adding faces to F because max depth reached...\n");
                }
                
                /* construct output face */
                cavface2.child = NOCAVITYTET;
                
                cavface2.verts[0] = t[0];
                cavface2.verts[1] = t[1];
                cavface2.verts[2] = t[2];
                cavface2.qual = tetquality(mesh, vtag, t[2], t[1], t[0], improvebehave.qualmeasure);
                assert(cavface2.qual > 0);
                /* add it to parent tet */
                addcavitytetface(&outcavity[*cavitysize -1], &cavface2);
                
                cavface2.verts[0] = t[0];
                cavface2.verts[1] = t[2];
                cavface2.verts[2] = t[3];
                cavface2.qual = tetquality(mesh, vtag, t[3], t[2], t[0], improvebehave.qualmeasure);
                assert(cavface2.qual > 0);
                /* add it to parent tet */
                addcavitytetface(&outcavity[*cavitysize -1], &cavface2);
                
                cavface2.verts[0] = t[0];
                cavface2.verts[1] = t[3];
                cavface2.verts[2] = t[1];
                cavface2.qual = tetquality(mesh, vtag, t[1], t[3], t[0], improvebehave.qualmeasure);
                assert(cavface2.qual > 0);
                /* add it to parent tet */
                addcavitytetface(&outcavity[*cavitysize -1], &cavface2);
                
                addlistface(t[0], t[1], t[2], W, &Wsize);
                addlistface(t[0], t[2], t[3], W, &Wsize);
                addlistface(t[0], t[3], t[1], W, &Wsize);
            }
        }
        else
        {
            /* this is a blocking tet, add it to B */
            addlisttet(t[0], t[1], t[2], t[3], B, &Bsize);
            
            /* note the current face in the parent tet */
            cavface.child = NOCAVITYTET;
            addcavitytetface(&outcavity[tetindex], &cavface);
            
            /* add the current face to the wall face list */
            addlistface(f[0], f[1], f[2], W, &Wsize);
        }
    }
    
    /* record the maximum depth */
    stats.biggestcavdepths[deepest]++;
    
    if (improvebehave.verbosity > 5)
    {
        printf("here is the cavity:\n");
        printtets(mesh, C, Csize);
        printf("here is W:\n");
        printfaces(mesh, W, Wsize);
    }
}

/* debug routine to print out edge list */
void printedgelist(struct cavityedge edges[],
                   int numedges)
{
    int i;
    for (i=0; i<numedges; i++)
    {
        printf("Edge %d/%d:\n", i, numedges-1);
        printf("    Qual: %g\n", pq(edges[i].qual));
        printf("    Parent: %d\n", edges[i].parent);
        printf("    Child: %d\n", edges[i].child);
        printf("    Childnum: %d\n", edges[i].childnum);
    }
}

/* debug routine to print out combined tet/edge list */
void printedgeortetlist(struct cavityedgeortet edges[],
                        int numedges)
{
    int i;
    for (i=0; i<numedges; i++)
    {
        printf("Edge %d/%d:\n", i, numedges-1);
        printf("    Label: %d\n", edges[i].label);
        printf("    Qual: %g\n", pq(edges[i].qual));
        printf("    Parent: %d\n", edges[i].parent);
        printf("    Child: %d\n", edges[i].child);
        printf("    Childnum: %d\n", edges[i].childnum);
    }
}

/* compare two edges, to be used for standard qsort call */
int compareedges(const void * a, const void * b)
{
    if ( ((struct cavityedge *)a)->qual > ((struct cavityedge *)b)->qual) return 1;
    if ( ((struct cavityedge *)a)->qual < ((struct cavityedge *)b)->qual) return -1;
    return 0;
}

/* compare two elements in a hybrid edge/tet list, for qsort */
int compareedgeortets(const void * a, const void * b)
{
    if ( ((struct cavityedgeortet *)a)->qual > ((struct cavityedgeortet *)b)->qual) return 1;
    if ( ((struct cavityedgeortet *)a)->qual < ((struct cavityedgeortet *)b)->qual) return -1;
    return 0;
}

/* function to sort an array of edges */
void sortedges(struct cavityedge array[],
               int arraylength)
{
    qsort(array, (size_t) arraylength, sizeof(struct cavityedge), compareedges);
}

/* function to sort an hybrid array of edges/tets */
void sortedgeortets(struct cavityedgeortet array[],
               int arraylength)
{
    qsort(array, (size_t) arraylength, sizeof(struct cavityedgeortet), compareedgeortets);
}

#define CAVLABEL 0
#define ANTICAVLABEL 1
#define NOLABEL 2

/* table of factors by which to augment deeper tets 
   TODO automatically select weights somehow */
#define DEPTHTABLESIZE 10
starreal depthtable[DEPTHTABLESIZE] = {1.0, 
                                       1.6,
                                       2.3,
                                       2.9,
                                       3.3,
                                       3.3,
                                       3.3,
                                       3.3,
                                       3.3,
                                       3.3};

/* recursively label parents and children as cavity tets */
void cavitylabel(struct cavitytet cavity[],
                 int cavitysize,
                 int tetid)
{
    int i;
    
    /* this tet shouldn't yet be labeled */
    assert(cavity[tetid].label == NOLABEL);
    
    /* label this tet as in the cavity */
    cavity[tetid].label = CAVLABEL;
    
    if (improvebehave.verbosity > 5)
    {
        printf("  In cavitylabel, just labeled tet %d\n", tetid);
    }
    
    /* go through all parents in the original graph */
    for (i=0; i<3; i++)
    {
        if (cavity[tetid].parents[i] != NOCAVITYTET)
        {
            /* if this parent is unlabeled, label it */
            if (cavity[cavity[tetid].parents[i]].label == NOLABEL)
            {
                if (improvebehave.verbosity > 5)
                {
                    printf("parent %d = %d is unlabeled. calling cavity() on it\n", i, cavity[tetid].parents[i]);
                }
                cavitylabel(cavity, cavitysize, cavity[tetid].parents[i]);
            }
        }
    }
    
    /* go through all children in H */
    for (i=0; i<cavity[tetid].numoutfaces; i++)
    {
        /* check if this edge is in H */
        if (cavity[tetid].outfaces[i].inH == true)
        {
            /* this can't be an edge leading to t... we should never add those */
            assert(cavity[tetid].outfaces[i].child != NOCAVITYTET);
            
            if (cavity[cavity[tetid].outfaces[i].child].label == NOLABEL)
            {
                if (improvebehave.verbosity > 5)
                {
                    printf("edge to child %d/%d = %d is in H, and child is unlabeled. calling cavity() on it\n", i, cavity[tetid].numoutfaces-1, cavity[tetid].outfaces[i].child);
                }
                cavitylabel(cavity, cavitysize, cavity[tetid].outfaces[i].child);
            }
        }
    }
}

/* recursively label parents and children as anti-cavity tets */
void anticavitylabel(struct cavitytet cavity[],
                 int cavitysize,
                 int tetid)
{
    int i,j,parent,edgetochild;
    
    /* this tet shouldn't yet be labeled */
    assert(cavity[tetid].label == NOLABEL);
    
    /* label this tet as in the anticavity */
    cavity[tetid].label = ANTICAVLABEL;
    
    if (improvebehave.verbosity > 5)
    {
        printf("  In anticavitylabel, just labeled tet %d\n", tetid);
    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("now considering parents in H\n");
    }
    /* go through all parents in H */
    for (i=0; i<3; i++)
    {
        parent = cavity[tetid].parents[i];
        
        if (improvebehave.verbosity > 5)
        {
            printf("considering parent %d = %d\n", i, parent);
        }
        
        if (parent != NOCAVITYTET)
        {
            /* is this parent unlabeled ? */
            if (cavity[cavity[tetid].parents[i]].label != NOLABEL)
            {
                if (improvebehave.verbosity > 5)
                {
                    printf("this parent is already labeled, moving on.\n");
                }
                continue;
            }
            
            /* find the edge from this parent down to the child */
            edgetochild = -1;
            for (j=0; j<cavity[parent].numoutfaces; j++)
            {
                if (cavity[parent].outfaces[j].child == tetid)
                {
                    if (improvebehave.verbosity > 5)
                    {
                        printf("found edge from parent down to child = %d\n", j);
                    }
                    edgetochild = j;
                }
            }
            /* make sure we found the edge */
            assert(edgetochild != -1);
            
            /* is this edge in H? */
            if (cavity[parent].outfaces[edgetochild].inH == true)
            {
                if (improvebehave.verbosity > 5)
                {
                    printf("parent %d = %d is unlabeled, and edge from it is in H. calling anticavity() on it\n", i, cavity[tetid].parents[i]);
                }
                anticavitylabel(cavity, cavitysize, cavity[tetid].parents[i]);
            }
        }
    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("now considering children in G\n");
    }
    /* go through all children in original graph G */
    for (i=0; i<cavity[tetid].numoutfaces; i++)
    {
        /* if the child is t, it's the end and is already labeled. move on */
        if (cavity[tetid].outfaces[i].child == NOCAVITYTET)
        {
            if (improvebehave.verbosity > 5)
            {
                printf("child %d is t, moving on\n", i);
            }
            continue;
        }
        
        if (cavity[cavity[tetid].outfaces[i].child].label == NOLABEL)
        {
            if (improvebehave.verbosity > 5)
            {
                printf("edge to child %d/%d = %d is unlabeled. calling anticavity() on it\n", i, cavity[tetid].numoutfaces-1, cavity[tetid].outfaces[i].child);
            }
            anticavitylabel(cavity, cavitysize, cavity[tetid].outfaces[i].child);
        }
    }
}


/* ALGORITHM 3 + CONSIDER DELETED TETS*/
/* given the DAG representing the largest possible cavity,
   find the subcavity that has the lexicographically maximum
   quality, then dig it out and connect the exposed faces to 
   the new vertex, returning the new tets as well as the worst
   quality in this cavity */
void lexmaxcavity3(struct tetcomplex *mesh,
                  tag vtag,
                  struct cavitytet cavity[],
                  int cavitysize,
                  tag outtets[][4],
                  int *numouttets,
                  starreal *worstdeleted,
                  starreal *worstincavity)
{
    int i,j;                                 /* loop counters */
    int parentless;                          /* number of tets without parents */
    bool foundparent = false;                /* did we find face parent? */
    static struct cavityedgeortet edges[MAXCAVITYDAGEDGES]; /* list of all edges in the DAG */
    int numedges=0;
    static int unionfind[MAXCAVITYTETS];            /* union-find array */
    int unionfindsize = cavitysize + 1;      /* size of the union-find array */
    int t = cavitysize;                      /* the virtual node t, at the end of array */
    int parentlabel, childlabel;             /* the groups that contain parent and child of an edge */
    starreal depthfactor;
    starreal qual;
    int deepest = 0;
    int parentcount;
    
    static tag outputfaces[MAXCAVITYFACES][3];
    int numoutputfaces = 0;
    
    /* initialize union-find array */
    for (i=0; i<unionfindsize; i++)
    {
        unionfind[i] = -1;
    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("Here's the initial DAG:\n");
        printcavitydag(mesh, cavity, cavitysize);
    }
    
    /* first, we modify the given dag a bit. it may have multiple
       parentless tets that correspond to the initial cavity in the
       DAG-building routine. we'll merge them into one "supertet,"
       s, that represents the single root of the dag.
       
       We'll add one other node to the DAG, t, and add an edge from
       all the previously childless faces of the DAG to t.
       
       The algorithm will then proceed by taking all the edges of this
       new DAG, sorted in ascending order of quality, and add them
       to the DAG iff by adding the edge, we do not connect s to t. */
    
    /* merge all the parentless tets at the start of the DAG into
       one supertet s */
    for (i=1; i<cavitysize; i++)
    {
        parentcount = numparents(&cavity[i]);
        
        /* check if this tet is parentless */
        if (parentcount == 0)
        {
        /* copy the outgoing faces back into the first tet */
            for (j=0; j<cavity[i].numoutfaces; j++)
            {
                /* copy face into the first tet */
                addcavitytetface(&cavity[0], &cavity[i].outfaces[j]);
                
                /* make the child of this face point back to the first tet */
                if (cavity[i].outfaces[j].child != NOCAVITYTET)
                {
                    /* find the parent pointer that points back to cavity tet i */
                    if (cavity[cavity[i].outfaces[j].child].parents[0] == i)
                    {
                        cavity[cavity[i].outfaces[j].child].parents[0] = 0;
                        foundparent = true;
                    }
                    if (cavity[cavity[i].outfaces[j].child].parents[1] == i)
                    {
                        assert(foundparent == false);
                        cavity[cavity[i].outfaces[j].child].parents[1] = 0;
                        foundparent = true;
                    }
                    if (cavity[cavity[i].outfaces[j].child].parents[2] == i)
                    {
                        assert(foundparent == false);
                        cavity[cavity[i].outfaces[j].child].parents[2] = 0;
                        foundparent = true;
                    }
                    assert(foundparent == true);
                    foundparent = false;
                }
            }
            
            /* update the parentless tet whose children we just stole to have
               zero outgoing faces */
            cavity[i].numoutfaces = 0;
        }
        else
        {
            /* we've reached the end of the parentless tets; bail */
            break;
        }
    }
    /* save the number of parentless tets */
    parentless = i;

    
    /* now, proceed through the DAG, recording all edges, and deleting child
       information to remove the edges, producing D' */
    for (i=0; i<cavitysize; i++)
    {
        
        /* initialize this tet's label. set s = cavity, others no label */
        if (i == 0)
        {
            cavity[i].label = CAVLABEL;
        }
        else
        {
            cavity[i].label = NOLABEL;
        }
        
        /* compute how much to exagerrate quality because of depth */
        assert(cavity[i].depth >= 0);
        if (cavity[i].depth < DEPTHTABLESIZE)
        {
            depthfactor = depthtable[cavity[i].depth];
        }
        else
        {
            depthfactor = depthtable[DEPTHTABLESIZE - 1];
        }
        assert(depthfactor >= 1.0);
        
        /* and an edgeortet for this tet, if deleted tets are considered */
        if (improvebehave.cavityconsiderdeleted)
        {
            if (i > parentless)
            {
                edges[numedges].label = TETLABEL;
                edges[numedges].parent = i;
                edges[numedges].child = 0;
                edges[numedges].qual = cavity[i].qual * depthfactor;
                numedges++;
                assert(numedges < MAXCAVITYDAGEDGES);
            }
        }
        
        /* for each outgoing face */
        for (j=0; j<cavity[i].numoutfaces; j++)
        {
            edges[numedges].label = EDGELABEL;
            edges[numedges].parent = i;
            edges[numedges].qual = cavity[i].outfaces[j].qual * depthfactor;
            /* save which outgoing face this child was */
            edges[numedges].childnum = j;
            
            if (cavity[i].outfaces[j].child == NOCAVITYTET)
            {
                /* if it has no child tet, make up a virtual edge to t */
                edges[numedges].child = t;
            }
            else
            {
                /* set edge child to be actual child */
                edges[numedges].child = cavity[i].outfaces[j].child;
            }
            numedges++;
            assert(numedges < MAXCAVITYDAGEDGES);
            
            /* initialize H by setting no edges to be in it */
            cavity[i].outfaces[j].inH = false;
        }
    }
    
    /* now, sort the edges in order of ascending quality */
    sortedgeortets(edges, numedges);
    
    /* go through each edge, adding it to D' iff it doesn't
       connect s to t */
    for (i=0; i<numedges; i++)
    {
        /* is this a tet? */
        if (edges[i].label == TETLABEL)
        {
            if (improvebehave.verbosity > 5)
            {
                printf("considering a deleted tet of qual = %g, label = %d\n", pq(edges[i].qual), cavity[edges[i].parent].label);
            }
            
            /* if this tet is unlabeled */
            if (cavity[edges[i].parent].label == NOLABEL)
            {
                /* label this tet for the cavity */
                if (improvebehave.verbosity > 4)
                {
                    printf("found a deleted tet that we want to grab, qual = %g\n", pq(edges[i].qual));
                }
                cavitylabel(cavity, cavitysize, edges[i].parent);
                continue;
            }
        }
        
        /* check parent's label */
        parentlabel = cavity[edges[i].parent].label;
        /* check child's label */
        if (edges[i].child == t)
        {
            /* this child is labeled automatically as anti-cavity */
            childlabel = ANTICAVLABEL;
        }
        else
        {
            childlabel = cavity[edges[i].child].label;
        }
        
        if (improvebehave.verbosity > 5)
        {
            printf("Considering edge %d/%d:\n", i, numedges-1);
            printf("    label       = %d\n", edges[i].label);
            printf("    qual        = %g\n", pq(edges[i].qual));
            printf("    parent      = %d\n", edges[i].parent);
            printf("    child       = %d\n", edges[i].child);
            printf("    parentlabel = %d\n", parentlabel);
            printf("    childlabel  = %d\n", childlabel);
            printf("    t           = %d\n", t);
        }
        
        
        /* if the parent is in the cavity */
        if (parentlabel == CAVLABEL)
        {
            /* and the child is in the anti-cavity */
            if (childlabel == ANTICAVLABEL)
            {
                /* record output face from parent to child */
                if (improvebehave.verbosity > 5)
                {
                    printf("this edge goes from cavity to anticavity, represents output tet.\n");
                }
                
                outputfaces[numoutputfaces][0] = cavity[edges[i].parent].outfaces[edges[i].childnum].verts[0];
                outputfaces[numoutputfaces][1] = cavity[edges[i].parent].outfaces[edges[i].childnum].verts[1];
                outputfaces[numoutputfaces][2] = cavity[edges[i].parent].outfaces[edges[i].childnum].verts[2];
                numoutputfaces++;
            }
            /* otherwise, if the child isn't labeled */
            else
            {
                if (childlabel == NOLABEL)
                {
                    if (improvebehave.verbosity > 5)
                    {
                        printf("parent is in cavity, child is unlabeled. calling cavity(child)\n");
                    }
                    cavitylabel(cavity, cavitysize, edges[i].child);
                }
            }
        }
        /* parent isn't labeled cavity. */
        else
        {
            /* is the parent wholly unlabeled ? */
            if (parentlabel == NOLABEL)
            {
                /* is the child labeled anti-cavity ? */
                if (childlabel == ANTICAVLABEL)
                {
                    if (improvebehave.verbosity > 5)
                    {
                        printf("parent is unlabeled, child is labeled anticavity. calling anticavity(parent)\n");
                    }
                    anticavitylabel(cavity, cavitysize, edges[i].parent);
                }
                else
                /* neither the parent nor the child is labeled */
                {
                    /* add the edge from parent to child to H */
                    if (improvebehave.verbosity > 5)
                    {
                        printf("parent and child unlabeled. adding edge to H\n");
                    }
                    cavity[edges[i].parent].outfaces[edges[i].childnum].inH = true;
                }
            }
        }
    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("Here's the final DAG:\n");
        printcavitydag(mesh, cavity, cavitysize);
    }
    
    /* first sanity check: every tet in the dag should be labeled */
    if (improvebehave.verbosity > 5)
    {
        for (i=0; i<cavitysize; i++)
        {
            printf("tet %d has label %d\n", i, cavity[i].label);
            if (numparents(&cavity[i]) != 0)
            {
                if (cavity[i].label == NOLABEL)
                {
                    printf("uhoh. tet %d has label %d, strange...\n", i, cavity[i].label);
                }
                assert(cavity[i].label != NOLABEL);
            }
        }
        if (improvebehave.verbosity > 5)
        {
            printf("they are all in one or another yay!\n");
            printf("here are the output faces:\n");
            printfaces(mesh, outputfaces, numoutputfaces);
        }
    }
    
    /* delete the non-root parentless tets */
    for (i=1; i<parentless; i++)
    {
        assert(tetexists(mesh, cavity[i].verts[0], 
                               cavity[i].verts[1],
                               cavity[i].verts[2],
                               cavity[i].verts[3]));
        deletetet(mesh, cavity[i].verts[0],
                        cavity[i].verts[1],
                        cavity[i].verts[2],
                        cavity[i].verts[3],
                        true);
    }
    
    /* keep track of what the deepest tet in the final cavity was */
    deepest = 0;
    
    /* delete all tets labeled as cavity */
    *worstdeleted = HUGEFLOAT;
    for (i=0; i<cavitysize; i++)
    {
        if (cavity[i].label == CAVLABEL)
        {
            /* is this the worst quality tet we're deleting? */
            if (cavity[i].qual < *worstdeleted)
            {
                *worstdeleted = cavity[i].qual;
            }
            
            /* is this the deepest tet we've encountered? */
            if (cavity[i].depth > deepest)
            {
                deepest = cavity[i].depth;
            }
            
            assert(tetexists(mesh, cavity[i].verts[0], 
                                   cavity[i].verts[1], 
                                   cavity[i].verts[2], 
                                   cavity[i].verts[3]));
            deletetet(mesh, cavity[i].verts[0],
                           cavity[i].verts[1],
                           cavity[i].verts[2],
                           cavity[i].verts[3],
                           true);
        }
    }
    
    /* record depth of deepest tet */
    stats.lexmaxcavdepths[deepest]++;
    
    /* now build the output cavity using the output faces */
    *numouttets = 0;
    for (i=0; i<numoutputfaces; i++)
    {
        assert(tetexists(mesh, vtag,
                               outputfaces[i][2],
                               outputfaces[i][1],
                               outputfaces[i][0]) == false);
        /* the face is oriented outward, so reverse it to point back */
        if (improvebehave.verbosity > 5)
        {
            printf("about to insert tet (%d %d %d %d)\n", (int) vtag, (int) outputfaces[i][2], (int) outputfaces[i][1], (int) outputfaces[i][0]);
        }
        
        inserttet(mesh, vtag, outputfaces[i][2], outputfaces[i][1], outputfaces[i][0], true);
        qual = tetquality(mesh, vtag, outputfaces[i][2], outputfaces[i][1], outputfaces[i][0], improvebehave.qualmeasure);
        assert(qual > 0);
        if (qual < *worstincavity) *worstincavity = qual;
    
        outtets[*numouttets][0] = vtag;
        outtets[*numouttets][1] = outputfaces[i][2];
        outtets[*numouttets][2] = outputfaces[i][1];
        outtets[*numouttets][3] = outputfaces[i][0];
        (*numouttets)++;
    }
    if (improvebehave.verbosity > 5)
    {
        printf("here are the output cavity tets:\n");
        printtets(mesh, outtets, *numouttets);
    }
}


/* given a vertex, it's position, and an initial cavity of tets
   and a set of outward-oriented faces of that cavity, dig out an
   optimal cavity around the vertex and connect the central vertex
   to all of the cavity faces, and return a list of the new tets */
void optimalcavity(struct tetcomplex *mesh,
                   tag vtag,
                   tag initialC[][4],
                   int initialCsize,
                   tag initialF[][3],
                   int initialFsize,
                   tag outtets[][4],
                   int *numouttets,
                   starreal *worstdeleted,
                   starreal *worstincavity,
                   bool allowvertexdeletion)
{
    int cavitysize;
    static struct cavitytet cavity[MAXCAVITYTETS];
#ifndef NO_TIMER
    struct timeval tv0, tv1;
    struct timezone tz;
#endif /* not NO_TIMER */

    *worstdeleted = HUGEFLOAT;
    *worstincavity = HUGEFLOAT;
    
    /* construct the DAG of the largest possible cavity */
#ifndef NO_TIMER
    gettimeofday(&tv0, &tz);
#endif /* not NO_TIMER */
    buildcavitydag(mesh, vtag, 
                   initialC, initialCsize,
                   initialF, initialFsize,
                   cavity, &cavitysize, allowvertexdeletion);
#ifndef NO_TIMER
    gettimeofday(&tv1, &tz);
    stats.biggestcavityusec += usecelapsed(tv0, tv1);
#endif /* not NO_TIMER */
    
    if (improvebehave.verbosity > 5)
    {
        printf("Largest possible cavity is %d tets\n", cavitysize);
    }
    
    /* keep track of cavity sizes seen */
    stats.maxcavitysizes[cavitysize]++;
    
    if (improvebehave.verbosity > 5)
    {
        printf("here is the DAG of biggest:\n");
        printcavitydag(mesh, cavity, cavitysize);
    }
    
    /* check consistency of DAG */
    /*
    assert(cavitydagcheck(mesh, cavity, cavitysize));
    */
    
    /* build the cavity of maximum lexicographic quality */
#ifndef NO_TIMER
    gettimeofday(&tv0, &tz);
#endif /* not NO_TIMER */
    lexmaxcavity3(mesh, vtag, cavity, cavitysize, outtets, numouttets, worstdeleted, worstincavity);
#ifndef NO_TIMER
    gettimeofday(&tv1, &tz);
    stats.finalcavityusec += usecelapsed(tv0, tv1);
#endif /* not NO_TIMER */
    
    /* keep track of lexmax cavity sizes */
    stats.finalcavitysizes[*numouttets]++;
    
    if (improvebehave.verbosity > 5)
    {
        printf("the lexmax cavity has %d tets, worst cavity qual = %g, worst deleted tet qual = %g:\n", *numouttets, pq(*worstincavity), pq(*worstdeleted));
    }
    
    /* print out the dag */
    /*
    printcavitydag(mesh, cavity, cavitysize);
    */
}

/* given a cavity of tets built around a single vertex,
   try to improve the cavity, and return the final quality */
starreal improvecavity(struct tetcomplex *mesh,
                       tag v,
                       tag tets[][4],
                       int numtets,
                       bool tryhard,
                       struct arraypoolstack *outstack,
                       starreal *shortest, starreal *longest)
{
    int i, stackiter=0;
    starreal quality;
    struct improvetet *stacktet;
    starreal smoothworst;
    starreal topoworst;
    starreal mean[NUMMEANTHRESHOLDS];
    starreal oldworst = 1.0;
    starreal newworst = 2.0;
    bool smoothed=true, toposuccess=true;
    tag newtet[4];
    int smoothkinds = 0;
    struct arraypoolstack stack[2];
    struct arraypoolstack influencestack;
    starreal physworst;
    
    /* change qual measure for insertion to consider edge length ratio */
    int qualmeasure = improvebehave.qualmeasure;
    
    int stacksize = 0;
    int maxstacksize = MAXCAVITYSTACK;
    int minimprovement = MINSUBMESHIMPROVEMENT;
    int maxpasses = MAXSUBMESHITERATIONS;
    
    if (improvebehave.facetsmooth) smoothkinds |= SMOOTHFACETVERTICES;
    if (improvebehave.segmentsmooth) smoothkinds |= SMOOTHSEGMENTVERTICES;
    if (improvebehave.fixedsmooth) smoothkinds |= SMOOTHFIXEDVERTICES;
    
    if (tryhard) 
    {
        maxstacksize *= TRYHARDFACTOR;
        minimprovement = TRYHARDMINSUBMESHIMPROVEMENT;
        maxpasses = TRYHARDMAXSUBMESHITERATIONS;
    }    
    
    /* longest and shortest edges of the affected tets */
    *longest = HUGEFLOAT;
    *shortest = 0.0;
    
    /* two stacks to keep track of all tets influenced by improvement */
    stackinit(&stack[0], sizeof(struct improvetet));
    stackinit(&stack[1], sizeof(struct improvetet));
    stackinit(&influencestack, sizeof(struct improvetet));
    
    /* initialize stack with all the cavity tets */
    for (i=0; i<numtets; i++)
    {
        /* compute tet quality */
        quality = tetquality(mesh, tets[i][0], tets[i][1], tets[i][2], tets[i][3], improvebehave.qualmeasure);
        if (quality < oldworst) oldworst = quality;
        
        /* push new tet on stack */
        stacktet = (struct improvetet *) stackpush(&stack[0]);
        stacktet->quality = quality;
        stacktet->verts[0] = tets[i][0];
        stacktet->verts[1] = tets[i][1];
        stacktet->verts[2] = tets[i][2];
        stacktet->verts[3] = tets[i][3];
    }
    
    /* iteratively try to improve cavity's worst quality */
    i = 0;
    while ((newworst > oldworst + minimprovement) && 
           (i <= maxpasses) && 
           (smoothed || toposuccess))
    {
        if (newworst != 2.0) oldworst = newworst;
        
        /* fetch a tet that the new vertex is in */
        findtetfromvertex(mesh, v, newtet);
        /* if the vertex no longer exists, stop improvemet */
        if (!tetexistsa(mesh, newtet))
        {
            break;
        }
        
        /* smooth new vertex */
        smoothed = nonsmoothsinglevertex(mesh,
                                         newtet[0],
                                         newtet[1],
                                         newtet[2],
                                         newtet[3],
                                         &smoothworst,
                                         smoothkinds);
        
        if (improvebehave.verbosity > 5)
        {
            printf("singlesmoothworst is %g\n, smoothed = %d", pq(smoothworst), smoothed);
        }
        
        /* improve cavity topology, alternating input/output stacks with bitwise XOR/AND */
        toposuccess = topopass(mesh, 
                               &stack[stackiter & 1], 
                               &stack[(stackiter & 1) ^ 1], 
                               qualmeasure, mean,
                               mean, &topoworst, true);
        stackiter++;
        
        if (improvebehave.sizing)
        {
            /* compute longest and shortest edge lengths for the output stack */
            longshortstack(mesh, &stack[stackiter & 1], longest, shortest);
        }
        
        if (improvebehave.verbosity > 5)
        {
            printf("topoworst is %g success = %d\n", pq(topoworst), toposuccess);
        }
        
        /* determine the new worst quality */
        if (toposuccess == false && smoothed == false && i == 0)
        {
            newworst = oldworst;
        }
        newworst = topoworst;
        
        if (improvebehave.verbosity > 5)
        {
            printf("Cavity improvement iter %d: %g -> %g\n", i, pq(oldworst), pq(newworst));
        }
        
        i++;
    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("in the end, went through %d passes before smooth\n", i);
    }
    
    if (FINALCAVITYSMOOTH)
    {
        smoothed = true;
        while(stacksize < maxstacksize && i < maxpasses && smoothed)
        {
            /* at the end, try smoothing pass on all tets of cavity */
            smoothed = smoothpass(mesh, 
                                  &stack[i & 1], 
                                  &stack[(i & 1) ^ 1],
                                  &influencestack,
                                  improvebehave.qualmeasure,
                                  newworst, 
                                  mean,
                                  mean,
                                  &smoothworst,
                                  smoothkinds,
                                  true);
            stacksize = stack[(i & 1) ^ 1].top; 
            
            if (improvebehave.sizing)
            {
                /* compute longest and shortest edge lengths for the output stack */
                longshortstack(mesh, &stack[(i & 1) ^ 1], longest, shortest);
            }
            
            if (improvebehave.verbosity > 5)
            {
                printf("Performed smoothing pass %d (outstack size %d) on all verts, success = %d, %g -> %g\n", i, stacksize, smoothed, pq(newworst), pq(smoothworst));
            }
    
            if (smoothed && (smoothworst - newworst > minimprovement))
            {
                /* if the smoothing pass claims success, it had better have improved quality */
                assert(smoothworst > newworst);
                newworst = smoothworst;
            }
            else
            {
                if (improvebehave.verbosity > 5)
                {
                    printf("broke the loop, smoothed = %d, qualimprove = %g\n", smoothed, pq(smoothworst - newworst));
                }
                break;
            }
            i++;
        }
        
        if (stacksize >= maxstacksize && improvebehave.verbosity > 1)
        {
            printf("bailing because stacksize %d exceeded max %d\n", stacksize, maxstacksize);
        }
    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("in the end, went through %d passes after final smooth\n", i);
    }
    
    /* if anisotropy is turned on, check for inverted tets in physical space */
    if (improvebehave.anisotropic)
    {
        improvebehave.anisotropic = false;
        /* if we did a final smooth */
        if (FINALCAVITYSMOOTH && influencestack.top != -1)
        {
            /* check the quality of this stack in physical space */
            physworst = worststackquality(mesh, &influencestack, improvebehave.qualmeasure);
        }
        else
        {
            /* check the quality of this stack in physical space */
            physworst = worststackquality(mesh, &stack[i & 1], improvebehave.qualmeasure);
        }
        improvebehave.anisotropic = true;
        
        if (improvebehave.verbosity > 0 && physworst <= 0.0)
        {
            printf("PHYSICAL worst quality after cavity improvement is %g\n", physworst);
        }
        assert(physworst > 0.0);
    }
    
    /* if there is a non-null output stack */
    if (outstack != NULL)
    {
        /* if we did a final smooth */
        if (FINALCAVITYSMOOTH && influencestack.top != -1)
        {
            /* return the entire influence stack */
            copystack(&influencestack, outstack);
            assert(outstack->top != -1);
        }
        else
        {
            /* just return output from last pass */
            copystack(&stack[i & 1], outstack);
            assert(outstack->top != -1);
        }
    }
    
    /* clean up stacks */
    stackdeinit(&stack[0]);
    stackdeinit(&stack[1]);
    stackdeinit(&influencestack);
    
    if (improvebehave.verbosity > 5)
    {
        printf("After cavity improvement, worst quality is %g\n", pq(newworst));
    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("After cavity improvement, longest edge = %g, shortest = %g, goal = %g\n", *longest, *shortest, improvebehave.targetedgelength);
    }
    
    /* return the best we could do with this cavity */
    return newworst;
}

/* insert a new vertex in the facet specified, returning
   the new vertex tag, the new vertex position, and the 
   outward-oriented faces of the new tets */
bool facetinsert(struct tetcomplex *mesh,
                 tag v1,
                 tag v2,
                 tag v3,
                 tag v4,
                 tag face[3],
                 tag *vnew,
                 tag newfaces[][3],
                 int *numnewfaces,
                 tag newtets[][4],
                 int *numnewtets)
{
    struct vertex *newvertex;         /* pointer to newly allocated vertex */
    starreal *point[4];               /* actual vertices of the tet */
    starreal barycenter[3] = {0,0,0}; /* barycenter of input tet */
    tag tetverts[4];                  /* the vertices of the tet starting with the vert not on bound face */
    tag newtag;                       /* the tag of the newly inserted vertex */
    tag quadricfaces[3][3];
    tag journverts[1];
    struct quadric *quadrics[3];
    int i;
    bool quadsuccess;
    
    /* find which vertex is not part of the boundary face, and arrange tet
       vertices so first is the non-boundary-face vertex */
    if ((v1 != face[0]) && (v1 != face[1]) && (v1 != face[2]))
    {
        tetverts[0] = v1;
    }
    else
    {
        if ((v2 != face[0]) && (v2 != face[1]) && (v2 != face[2]))
        {
            tetverts[0] = v2;
        }
        else
        {
            if ((v3 != face[0]) && (v3 != face[1]) && (v3 != face[2]))
            {
                tetverts[0] = v3;
            }
            else
            {
                /* v4 must not be one of the face vertices */
                assert(((v4 != face[0]) && (v4 != face[1]) && (v4 != face[2])));
                tetverts[0] = v4;
            }
        }
    }
    
    /* the rest of the tet vertices should be the vertices of the boundary face, reversed in order
       so the face is oriented toward the fourth vertex */
    tetverts[1] = face[2];
    tetverts[2] = face[1];
    tetverts[3] = face[0];
    
    /* make sure our newly arranged tet exists */
    assert(tetexists(mesh, tetverts[0], tetverts[1], tetverts[2], tetverts[3]));
    
    /* fetch actual vertex values of face vertices */
    point[0] = ((struct vertex *) tetcomplextag2vertex(mesh, face[0]))->coord;
    point[1] = ((struct vertex *) tetcomplextag2vertex(mesh, face[1]))->coord;
    point[2] = ((struct vertex *) tetcomplextag2vertex(mesh, face[2]))->coord;
    
    /* fetch the ORIGINAL positions of the face vertices, because we want to start
       the vertex out on the original surface */
    quadrics[0] = (struct quadric *) arraypoolforcelookup(&surfacequadrics, face[0]);
    quadrics[1] = (struct quadric *) arraypoolforcelookup(&surfacequadrics, face[1]);
    quadrics[2] = (struct quadric *) arraypoolforcelookup(&surfacequadrics, face[2]);
    
    if (improvebehave.verbosity > 5)
    {
        printf("orig -> current face verts are:\n");
        for (i=0; i<3; i++)
        {
            printf("    (%g %g %g) -> (%g %g %g)\n", quadrics[i]->origpos[0], quadrics[i]->origpos[1], quadrics[i]->origpos[1], point[i][0], point[i][1], point[i][2]);
        }
    }
    
    /* compute barycenter of this face */
    vadd(barycenter, point[0], barycenter);
    vadd(barycenter, point[1], barycenter);
    vadd(barycenter, point[2], barycenter);
    /*
    vadd(barycenter, quadrics[0]->origpos, barycenter);
    vadd(barycenter, quadrics[1]->origpos, barycenter);
    vadd(barycenter, quadrics[2]->origpos, barycenter);
    */
    
    vscale(1.0 / 3.0, barycenter, barycenter);
    
    /* allocate a new vertex */
    newtag = proxipoolnew(mesh->vertexpool, 0, (void **) &newvertex);
    /* record this vertex insertion in the journal */
    journverts[0] = newtag;
    insertjournalentry(mesh, INSERTVERTEX, journverts, 1, NULL, barycenter);
    
    /* start the vertex at the face barycenter */
    vcopy(barycenter, newvertex->coord);
    
    /* delete the old tet and add the three new ones */
    flip13(mesh, tetverts[0], tetverts[1], tetverts[2], tetverts[3], newtag, true);
    /* make sure all the right tets exist now */
    assert(tetexists(mesh, tetverts[0], tetverts[1], newtag, tetverts[3]));
    assert(tetexists(mesh, tetverts[0], tetverts[2], newtag, tetverts[1]));
    assert(tetexists(mesh, tetverts[0], tetverts[3], newtag, tetverts[2]));
    
    /* assign the vertex type and compute the facet normal */
    tetvertexclassify(mesh,
                      newtag,
                      tetverts[0],
                      tetverts[1],
                      tetverts[3]);
    
    /* check that we got it right */
    /* this isn't necessarily true any more with quadrics */
    /*
    assert(((struct vertextype *) arraypoolforcelookup(&vertexinfo, newtag))->kind == FACETVERTEX);
    */
    
    /* initialize the quadric for this vertex */
    quadricfaces[0][0] = newtag;
    quadricfaces[0][1] = tetverts[2];
    quadricfaces[0][2] = tetverts[1];
    
    quadricfaces[1][0] = newtag;
    quadricfaces[1][1] = tetverts[3];
    quadricfaces[1][2] = tetverts[2];
    
    quadricfaces[2][0] = newtag;
    quadricfaces[2][1] = tetverts[1];
    quadricfaces[2][2] = tetverts[3];
    
    quadsuccess = addquadric(mesh, newtag, quadricfaces, 3);
    if (quadsuccess == false)
    {
        return false;
    }
    
    /* assign outputs */
    *vnew = newtag;
    
    *numnewtets = 3;
    newtets[0][0] = tetverts[0];
    newtets[0][1] = tetverts[1];
    newtets[0][2] = newtag;
    newtets[0][3] = tetverts[3];
    
    newtets[1][0] = tetverts[0];
    newtets[1][1] = tetverts[2];
    newtets[1][2] = newtag;
    newtets[1][3] = tetverts[1];
    
    newtets[2][0] = tetverts[0];
    newtets[2][1] = tetverts[3];
    newtets[2][2] = newtag;
    newtets[2][3] = tetverts[2];
    
    /* its possible that insertion fails outright because the vertices may have moved
       a lot due to smoothing from their original positions. so, check if any of the new
       tets are inverted, and return false if that's the case. */
    for (i=0; i<3; i++)
    {
        if (tetquality(mesh, 
                       newtets[i][0], 
                       newtets[i][1], 
                       newtets[i][2], 
                       newtets[i][3], 
                       improvebehave.qualmeasure) <= MINTETQUALITY)
        {
            if (improvebehave.verbosity > 1)
            {
                printf("Rejecting facet insertion because tet inverts.\n");
            }
            return false;
        }
        
        
    }
    
    *numnewfaces = 3;
    /* don't include outward faces on the boundary */
    /*
    newfaces[0][0] = newtag;
    newfaces[0][1] = tetverts[2];
    newfaces[0][2] = tetverts[1];
    
    newfaces[1][0] = newtag;
    newfaces[1][1] = tetverts[3];
    newfaces[1][2] = tetverts[2];
    
    newfaces[2][0] = newtag;
    newfaces[2][1] = tetverts[1];
    newfaces[2][2] = tetverts[3];
    */
    
    newfaces[0][0] = tetverts[0];
    newfaces[0][1] = tetverts[1];
    newfaces[0][2] = tetverts[2];
    
    newfaces[1][0] = tetverts[0];
    newfaces[1][1] = tetverts[3];
    newfaces[1][2] = tetverts[1];
    
    newfaces[2][0] = tetverts[0];
    newfaces[2][1] = tetverts[2];
    newfaces[2][2] = tetverts[3];
    
    return true;
}

void bodyinsert(struct tetcomplex *mesh,
                tag v1,
                tag v2,
                tag v3,
                tag v4,
                tag *vnew,
                tag newfaces[][3],
                int *numnewfaces,
                tag newtets[][4],
                int *numnewtets)
{
    tag newtag;                       /* the tag of the new vertex */
    struct vertex *newvertex;         /* pointer to newly allocated vertex */
    starreal *point[4];               /* actual vertices of the tet */
    starreal barycenter[3] = {0,0,0}; /* barycenter of input tet */
    int i;
    tag journverts[1];
    
    /* fetch actual vertex values of tet vertices */
    point[0] = ((struct vertex *) tetcomplextag2vertex(mesh, v1))->coord;
    point[1] = ((struct vertex *) tetcomplextag2vertex(mesh, v2))->coord;
    point[2] = ((struct vertex *) tetcomplextag2vertex(mesh, v3))->coord;
    point[3] = ((struct vertex *) tetcomplextag2vertex(mesh, v4))->coord;
    
    /* compute barycenter of this face */
    for (i=0; i<4; i++)
    {
        vadd(barycenter, point[i], barycenter);
    }
    vscale(0.25, barycenter, barycenter);
    
    /* allocate a new vertex */
    newtag = proxipoolnew(mesh->vertexpool, 0, (void **) &newvertex);
    /* record this vertex insertion in the journal */
    journverts[0] = newtag;
    insertjournalentry(mesh, INSERTVERTEX, journverts, 1, NULL, barycenter);
    
    /* start the vertex at the face barycenter */
    vcopy(barycenter, newvertex->coord);
    
    /* delete the old tet and add the three new ones */
    flip14(mesh, v1, v2, v3, v4, newtag, true);
    
    /* assign the vertex type */
    tetvertexclassify(mesh,
                      newtag,
                      v1,
                      v3,
                      v2);
    
    /* check that we got it right */
    assert(((struct vertextype *) arraypoolforcelookup(&vertexinfo, newtag))->kind == FREEVERTEX);
    
    /* assign outputs */
    *vnew = newtag;
    
    *numnewtets = 4;
    newtets[0][0] = v1;
    newtets[0][1] = v2;
    newtets[0][2] = v3;
    newtets[0][3] = newtag;
    
    newtets[1][0] = v1;
    newtets[1][1] = v3;
    newtets[1][2] = v4;
    newtets[1][3] = newtag;
    
    newtets[2][0] = v1;
    newtets[2][1] = v4;
    newtets[2][2] = v2;
    newtets[2][3] = newtag;
    
    newtets[3][0] = newtag;
    newtets[3][1] = v2;
    newtets[3][2] = v3;
    newtets[3][3] = v4;
    
    *numnewfaces = 4;
    newfaces[0][0] = v1;
    newfaces[0][1] = v2;
    newfaces[0][2] = v3;
    
    newfaces[1][0] = v1;
    newfaces[1][1] = v3;
    newfaces[1][2] = v4;
    
    newfaces[2][0] = v1;
    newfaces[2][1] = v4;
    newfaces[2][2] = v2;
    
    newfaces[3][0] = v2;
    newfaces[3][1] = v4;
    newfaces[3][2] = v3;
}

bool segmentinsert(struct tetcomplex *mesh,
                   tag v1,
                   tag v2,
                   tag v3,
                   tag v4,
                   int numtets,
                   tag tets[][4],
                   tag boundfaces[],
                   tag *vnew,
                   tag newfaces[][3],
                   int *numnewfaces,
                   tag newtets[][4],
                   int *numnewtets,
                   bool boundedge)
{
    tag newtag;                       /* the tag of the new vertex */
    struct vertex *newvertex;         /* pointer to newly allocated vertex */
    starreal *point[4];               /* actual vertices of the tet */
    starreal midpoint[3] = {0.,0.,0.};/* barycenter of boundary edge */
    tag splitedge[2];                 /* edge that will be split by insertion */
    tag quadricfaces[4][3];           /* four surface faces surrounding inserted vertex,
                                         used to initialize quadric */
    starreal qual;
    struct quadric *quadrics[2];
    
    int i;
    tag journverts[1];
    bool quadsuccess;
    
    /* all input tets should be arranged such that the edge we're splitting is made up
       of the first two vertices */
    
    /* pull out the edge that we'll be splitting */
    splitedge[0] = tets[0][0];
    splitedge[1] = tets[0][1];
    
    /* fetch actual vertex values of edge vertices */
    point[0] = ((struct vertex *) tetcomplextag2vertex(mesh, splitedge[0]))->coord;
    point[1] = ((struct vertex *) tetcomplextag2vertex(mesh, splitedge[1]))->coord;
    
    /* fetch the ORIGINAL positions of the edge vertices, because we want to start
       the vertex out on the original surface */
    quadrics[0] = (struct quadric *) arraypoolforcelookup(&surfacequadrics, splitedge[0]);
    quadrics[1] = (struct quadric *) arraypoolforcelookup(&surfacequadrics, splitedge[1]);
    
    if (improvebehave.verbosity > 5)
    {
        printf("Here is the original surface edge:\n");
        printf("[%g %g %g;\n", point[0][0], point[0][1], point[0][2]);
        printf("%g %g %g];\n", point[1][0], point[1][1], point[1][2]);
    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("edge endpoints (%d %d) original positions (%g %g %g) and (%g %g %g)\n",
               (int) splitedge[0], (int) splitedge[1], quadrics[0]->origpos[0], quadrics[0]->origpos[1], quadrics[0]->origpos[2],
               quadrics[1]->origpos[0], quadrics[1]->origpos[1], quadrics[1]->origpos[2]);
    }
    
    /* compute midpoint of this edge */
    /* vadd(midpoint, quadrics[0]->origpos, midpoint);
    vadd(midpoint, quadrics[1]->origpos, midpoint); */
    vadd(midpoint, point[0], midpoint);
    vadd(midpoint, point[1], midpoint);
    vscale(0.5, midpoint, midpoint);
    
    /* allocate a new vertex */
    newtag = proxipoolnew(mesh->vertexpool, 0, (void **) &newvertex);
    /* record this vertex insertion in the journal */
    journverts[0] = newtag;
    insertjournalentry(mesh, INSERTVERTEX, journverts, 1, NULL, midpoint);
    
    /* start the vertex at the edge midpoint */
    vcopy(midpoint, newvertex->coord);
    
    *numnewfaces = 0;
    *numnewtets = 0;
    *vnew = newtag;
    
    /* for every tet that surrounds this edge, perform a 1-2 flip */
    for (i=0; i<numtets; i++)
    {
        /* make sure the first two verts of this tet are the edge to be split */
        assert((tets[i][0] == splitedge[0] && tets[i][1] == splitedge[1]) ||
               (tets[i][0] == splitedge[1] && tets[i][1] == splitedge[0]));
        
        /* make sure the old tet exists */
        assert(tetexists(mesh, tets[i][0], tets[i][1], tets[i][2], tets[i][3]));
        
        /* delete the old tet and add the two new ones */
        flip12(mesh, tets[i][0], tets[i][1], tets[i][2], tets[i][3], newtag, true);
        
        /* make sure all the right tets exist now */
        assert(tetexists(mesh, tets[i][0], tets[i][2], tets[i][3], newtag));
        assert(tetexists(mesh, tets[i][1], tets[i][3], tets[i][2], newtag));
        
        /* add these tets to the output */
        newtets[*numnewtets][0] = tets[i][0];
        newtets[*numnewtets][1] = tets[i][2];
        newtets[*numnewtets][2] = tets[i][3];
        newtets[*numnewtets][3] = newtag;
        
        /* if anisotropy, check if this tet is inverted in PHYSICAL SPACE */
        if (improvebehave.anisotropic)
        {
            improvebehave.anisotropic = false;
            qual = tetquality(mesh,
                           newtets[*numnewtets][0],
                           newtets[*numnewtets][1],
                           newtets[*numnewtets][2],
                           newtets[*numnewtets][3],
                           improvebehave.qualmeasure); 
            improvebehave.anisotropic = true;
            
            if (qual <= MINSIZETETQUALITY)
            {
                if (improvebehave.verbosity > 1)
                {
                    printf("rejecting segment insert because tet would invert in PHYSICAL SPACE\n");
                }
                return false;
            }
        }
        
        /* if this tet is inverted, give up */
        qual = tetquality(mesh,
                       newtets[*numnewtets][0],
                       newtets[*numnewtets][1],
                       newtets[*numnewtets][2],
                       newtets[*numnewtets][3],
                       improvebehave.qualmeasure); 
        if (improvebehave.verbosity > 5)
        {
            printf("New tet (%d %d %d %d) has quality %g\n", (int) newtets[*numnewtets][0],
                                                             (int) newtets[*numnewtets][1],
                                                             (int) newtets[*numnewtets][2],
                                                             (int) newtets[*numnewtets][3],
                                                             qual);
        }
        
        if (qual <= MINTETQUALITY)
        {
            if (improvebehave.verbosity > 1)
            {
                printf("rejecting segment insert because tet would invert\n");
            }
            return false;
        }
        
        (*numnewtets)++;
        
        newtets[*numnewtets][0] = tets[i][1];
        newtets[*numnewtets][1] = tets[i][3];
        newtets[*numnewtets][2] = tets[i][2];
        newtets[*numnewtets][3] = newtag;
        
        /* if anisotropy, check if this tet is inverted in PHYSICAL SPACE */
        if (improvebehave.anisotropic)
        {
            improvebehave.anisotropic = false;
            qual = tetquality(mesh,
                           newtets[*numnewtets][0],
                           newtets[*numnewtets][1],
                           newtets[*numnewtets][2],
                           newtets[*numnewtets][3],
                           improvebehave.qualmeasure); 
            improvebehave.anisotropic = true;
            
            if (qual <= MINSIZETETQUALITY)
            {
                if (improvebehave.verbosity > 1)
                {
                    printf("rejecting segment insert because tet would invert in PHYSICAL SPACE\n");
                }
                return false;
            }
        }
        
        qual = tetquality(mesh,
                       newtets[*numnewtets][0],
                       newtets[*numnewtets][1],
                       newtets[*numnewtets][2],
                       newtets[*numnewtets][3],
                       improvebehave.qualmeasure); 
        if (improvebehave.verbosity > 5)
        {
            printf("New tet (%d %d %d %d) has quality %g\n", (int) newtets[*numnewtets][0],
                                                             (int) newtets[*numnewtets][1],
                                                             (int) newtets[*numnewtets][2],
                                                             (int) newtets[*numnewtets][3],
                                                             qual);
        }
        
        if (qual <= MINTETQUALITY)
        {
            if (improvebehave.verbosity > 1)
            {
                printf("rejecting segment insert because tet would invert\n");
            }
            return false;
        }
        
        (*numnewtets)++;
        
        /* add the "external" faces of the new tets to the output */
        newfaces[*numnewfaces][0] = tets[i][0];
        newfaces[*numnewfaces][1] = tets[i][2];
        newfaces[*numnewfaces][2] = tets[i][3];
        (*numnewfaces)++;
        
        newfaces[*numnewfaces][0] = tets[i][1];
        newfaces[*numnewfaces][1] = tets[i][3];
        newfaces[*numnewfaces][2] = tets[i][2];
        (*numnewfaces)++;
    }
    
    /* check if any of the new tets are inverted, and return false if so */
    
    /* add the last four faces of the split boundary faces to output */
    /* actually, exclude these faces because they lie on the boundary */
    /*
    newfaces[*numnewfaces][0] = newtag;
    newfaces[*numnewfaces][1] = splitedge[1];
    newfaces[*numnewfaces][2] = boundfaces[0];
    (*numnewfaces)++;
    
    newfaces[*numnewfaces][0] = newtag;
    newfaces[*numnewfaces][1] = boundfaces[0];
    newfaces[*numnewfaces][2] = splitedge[0];
    (*numnewfaces)++;
    
    newfaces[*numnewfaces][0] = newtag;
    newfaces[*numnewfaces][1] = boundfaces[1];
    newfaces[*numnewfaces][2] = splitedge[1];
    (*numnewfaces)++;
    
    newfaces[*numnewfaces][0] = newtag;
    newfaces[*numnewfaces][1] = splitedge[0];
    newfaces[*numnewfaces][2] = boundfaces[1];
    (*numnewfaces)++;
    */
    
    /*
    tag outin[2];
    bool foundface;
    foundface = tetcomplexadjacencies(mesh, newtag, splitedge[1], boundfaces[0], outin);
    assert(foundface);
    printf("(new, edge[1], bf[0]: out = %d, in = %d\n", outin[0], outin[1]);
    foundface = tetcomplexadjacencies(mesh, newtag, boundfaces[0], splitedge[0], outin);
    assert(foundface);
    printf("(new, bf[0], edge[0]: out = %d, in = %d\n", outin[0], outin[1]);
    foundface = tetcomplexadjacencies(mesh, newtag, boundfaces[1], splitedge[1], outin);
    assert(foundface);
    printf("(new, bf[1], edge[1]: out = %d, in = %d\n", outin[0], outin[1]);
    foundface = tetcomplexadjacencies(mesh, newtag, splitedge[0], boundfaces[1], outin);
    assert(foundface);
    printf("(new, edge[0], bf[1]: out = %d, in = %d\n", outin[0], outin[1]);
    */
    
    /* initialize the quadric for this vertex */
    if (boundedge)
    {
        quadricfaces[0][0] = newtag;
        quadricfaces[0][1] = splitedge[1];
        quadricfaces[0][2] = boundfaces[0];
        quadricfaces[1][0] = newtag;
        quadricfaces[1][1] = boundfaces[0];
        quadricfaces[1][2] = splitedge[0];
        quadricfaces[2][0] = newtag;
        quadricfaces[2][1] = boundfaces[1];
        quadricfaces[2][2] = splitedge[1];
        quadricfaces[3][0] = newtag;
        quadricfaces[3][1] = splitedge[0];
        quadricfaces[3][2] = boundfaces[1];
        quadsuccess = addquadric(mesh, newtag, quadricfaces, 4);
        if (quadsuccess == false)
        {
            if (improvebehave.verbosity > 0)
            {
                printf("segment insertion failed because adding the quadric failed.\n");
            }
            return false;
        }
    }
    
    /* assign the vertex type and compute the facet normal */
    tetvertexclassify(mesh,
                      newtag,
                      tets[0][1],
                      tets[0][2],
                      tets[0][3]);
    
    /* check that we got it right; new vertex can either be segment or facet vertex */
    /* this is no longer necessarily true because of quadric smoothing */
    /*
    assert((((struct vertextype *) arraypoolforcelookup(&vertexinfo, newtag))->kind == SEGMENTVERTEX) ||
           (((struct vertextype *) arraypoolforcelookup(&vertexinfo, newtag))->kind == FACETVERTEX));
    */
    return true;
}

/* general vertex insertion routine */
bool insertvertex(struct tetcomplex *mesh,
                  tag v1,
                  tag v2,
                  tag v3,
                  tag v4,
                  starreal inquality,
                  starreal meshworstquality,
                  starreal *outquality,
                  struct arraypoolstack *outstack,
                  bool tryhard)
{
    int nbverts, nbedges, nbfaces; /* number of boundary components */
    tag boundtags[4];              /* boundary vertex tags */
    tag boundfacetags[4][3];       /* boundary face tags */
    tag boundedgetags[6][2];       /* boundary edge tags */
    tag boundedgefaces[6][2];      /* the third vertices of the faces in the ring around an edge that lie on the boundary */
    int numboundedgetets[6];       /* number of tets in ring around each boundary edge */
    tag boundedgetets[6][MAXRINGTETS][4]; /* the list of the tets in the ring around each edge */
    bool bfaces;
    
    tag vnew;                      /* newly inserted vertex tag */
    static tag newfaces[MAXCAVITYFACES][3]; /* faces of tets created after insert for seeding cavity drilling */
    int numnewfaces;               
    static tag newtets[MAXCAVITYTETS][4]; /* tets that filled in cavity */
    int numnewtets;
    starreal minquality;           /* quality we have to beat for inserion to succeed */
    starreal origcavityqual;       /* quality of the worst tet in the cavity */
    starreal worstdeletedqual;     /* quality of the worst tet deleted to form the cavity */
    starreal cavityqual;           /* quality of the improved cavity */
    starreal longest, shortest;    /* shortest and longest edges in a cavity */
    bool success;
    bool sizeok;
    
    struct arraypoolstack cavitystack; /* save the tets affected by cavity improvement */
    struct arraypoolstack *cavitystackptr; /* may be null if outstack is null */
    
#ifndef NO_TIMER
    struct timeval tv0, tv1;
    struct timezone tz;
#endif /* not NO_TIMER */

    int i;
    
    /* remember the last journal entry before we change anything */
    int beforeid = lastjournalentry(); 
    
    /* make sure this tet still exists */
    if (tetexists(mesh, v1, v2, v3, v4) == false) return false;
    
    if (improvebehave.verbosity > 5)
    {
        printf("Attempting general vertex insertion on tet (%d %d %d %d)\n", (int) v1, (int) v2, (int) v3, (int) v4);
        printtetvertssep(mesh, v1, v2, v3, v4);
    }
    
    /* start by gathering boundary information on this tet */
    nbverts = boundverts(mesh, v1, v2, v3, v4, boundtags);
    nbedges = boundedges(mesh, v1, v2, v3, v4, boundedgetags, 
                                               boundedgefaces,
                                               numboundedgetets,
                                               boundedgetets);
    bfaces = boundfaces(mesh, v1, v2, v3, v4,  boundfacetags, &nbfaces);
    
    /* initialize cavity stack */
    stackinit(&cavitystack, sizeof(struct improvetet));
    if (outstack == NULL)
    {
        cavitystackptr = NULL;
    }
    else
    {
        cavitystackptr = &cavitystack;
    }
    
    /*************************************/
    /********** FACET INSERTION **********/
    /*************************************/
    /* if the tet has any boundary faces, try insertion on them first */
    /* TODO: why? */
    if (improvebehave.insertfacet)
    {
        for (i=0; i<nbfaces; i++)
        {
            if (improvebehave.verbosity > 5)
            {
                printf("Attempting facet insertion...\n");
            }
            
            /* stats */
            stats.facetinsertattempts++;
            
            /* perform the initial insertion using a 1-3 flip, locating the new
               vertex at the barycenter of the facet */
            success = facetinsert(mesh, v1, v2, v3, v4, 
                        boundfacetags[i], 
                        &vnew, 
                        newfaces, &numnewfaces,
                        newtets, &numnewtets);
            
            /* if one of the new tets is inverted, give up */
            if (success == false)
            {
                invertjournalupto(mesh, beforeid);
                continue;
            }
            
            /* dig out optimal cavity */
            optimalcavity(mesh, vnew,
                        newtets, numnewtets, 
                        newfaces, numnewfaces, 
                        newtets, &numnewtets,
                        &worstdeletedqual,
                        &origcavityqual, true);
                    
            if (improvebehave.verbosity > 5)
            {
                printf("The quality of the split tet was %g vs worst deleted to form cavity of %g\n", pq(inquality), pq(worstdeletedqual));
            }
        
            /* the quality we have to beat when improving the cavity is the
               worst of the split tet and those of the dug-out cavity */
            minquality = (worstdeletedqual < inquality) ? worstdeletedqual : inquality;
        
            if (improvebehave.verbosity > 5)
            {
                printf("Here is the cavity returned (it has %d tets):\n", numnewtets);
                printtets(mesh, newtets, numnewtets);
            }
        
            /* improve the cavity */
#ifndef NO_TIMER
            gettimeofday(&tv0, &tz);
#endif /* not NO_TIMER */
            cavityqual = improvecavity(mesh, vnew, newtets, numnewtets, tryhard, cavitystackptr, &longest, &shortest);
#ifndef NO_TIMER            
            gettimeofday(&tv1, &tz);
            stats.cavityimproveusec += usecelapsed(tv0, tv1);
#endif
            
            if ( improvebehave.sizing &&
                 ((longest > improvebehave.targetedgelength * improvebehave.longerfactor) ||
                 (shortest < improvebehave.targetedgelength * improvebehave.shorterfactor))
               )
            {
                if (improvebehave.verbosity > 3)
                {
                    printf("Aborting insertion because it would create bad edge of length %g/%g, target is %g\n", longest, shortest, improvebehave.targetedgelength);
                }
                sizeok = false;
            }
            else
            {
                sizeok = true;
            }
            
            /* did we succeed? */
            if (cavityqual > (minquality + MININSERTIONIMPROVEMENT) && sizeok)
            {
                /* yes. keep the insertion */
                if (improvebehave.verbosity > 4)
                {
                    printf("Facet vertex insertion succeeded. minqual = %g, cavityqual = %g\n", pq(minquality), pq(cavityqual));
                }
                
                /* add the affected tets to the output stack */
                if (outstack != NULL)
                {
                    appendstack(&cavitystack, outstack);
                }
                stackdeinit(&cavitystack);
                
                /* stats */
                stats.facetinsertsuccesses++;
                *outquality = cavityqual;
                return true;
            }
            else
            {
                /* no. reverse the damage */
                if (improvebehave.verbosity > 5)
                {
                    printf("Facet vertex insertion failed. minqual = %g, cavityqual = %g\n", pq(minquality), pq(cavityqual));
                }
            
                invertjournalupto(mesh, beforeid);
            }
        }
    }
    
    /*************************************/
    /********** BODY INSERTION ***********/
    /*************************************/
    if (improvebehave.insertbody)
    {
        if (improvebehave.verbosity > 5)
        {
            printf("Attempting bady insertion...\n");
        }
        
        /* stats */
        stats.bodyinsertattempts++;
        
        /* perform the initial insertion using a 1-4 flip, locating the new
           vertex at the barycenter of the tet */
        bodyinsert(mesh, v1, v2, v3, v4,
                   &vnew, 
                   newfaces, &numnewfaces,
                   newtets, &numnewtets);
    
        /* dig out optimal cavity */
        optimalcavity(mesh, vnew,
                    newtets, numnewtets, 
                    newfaces, numnewfaces, 
                    newtets, &numnewtets,
                    &worstdeletedqual,
                    &origcavityqual, true);
                
        if (improvebehave.verbosity > 5)
        {
            printf("The quality of the split tet was %g vs worst deleted to form cavity of %g\n", pq(inquality), pq(worstdeletedqual));
        }
    
        /* the quality we have to beat when improving the cavity is the
           worst of the split tet and those of the dug-out cavity */
        minquality = (worstdeletedqual < inquality) ? worstdeletedqual : inquality;
    
        if (improvebehave.verbosity > 5)
        {
            printf("Here is the cavity returned (it has %d tets):\n", numnewtets);
            printtets(mesh, newtets, numnewtets);
        }
    
        /* improve the cavity */
#ifndef NO_TIMER
        gettimeofday(&tv0, &tz);
#endif /* not NO_TIMER */
        cavityqual = improvecavity(mesh, vnew, newtets, numnewtets, tryhard, cavitystackptr, &longest, &shortest);
#ifndef NO_TIMER
        gettimeofday(&tv1, &tz);
        stats.cavityimproveusec += usecelapsed(tv0, tv1);
#endif /* not NO_TIMER */

        if ( improvebehave.sizing &&
             ((longest > improvebehave.targetedgelength * improvebehave.longerfactor) ||
             (shortest < improvebehave.targetedgelength * improvebehave.shorterfactor))
           )
        {
            if (improvebehave.verbosity > 3)
            {
                printf("Aborting insertion because it would create bad edge of length %g/%g, target is %g\n", longest, shortest, improvebehave.targetedgelength);
            }
            sizeok = false;
        }
        else
        {
            sizeok = true;
        }
        
        /* did we succeed? */
        if (cavityqual > (minquality + MININSERTIONIMPROVEMENT) && sizeok)
        {
            /* yes. keep the insertion */
            if (improvebehave.verbosity > 4)
            {
                printf("Body vertex insertion succeeded. minqual = %g, cavityqual = %g\n", pq(minquality), pq(cavityqual));
                if (cavitystackptr != NULL)
                {
                    printf("    and by the way, the worst stack quality is %g\n", worststackquality(mesh,cavitystackptr,improvebehave.qualmeasure));
                }
            }
            
            /* add the affected tets to the output stack */
            if (outstack != NULL)
            {
                appendstack(&cavitystack, outstack);
            }
            stackdeinit(&cavitystack);
            
            /* stats */
            stats.bodyinsertsuccesses++;
            *outquality = cavityqual;
            return true;
        }
        else
        {
            /* no. reverse the damage */
            if (improvebehave.verbosity > 5)
            {
                printf("Body vertex insertion failed. minqual = %g, cavityqual = %g\n", pq(minquality), pq(cavityqual));
            }
        
            invertjournalupto(mesh, beforeid);
        }
    }
    
    /*************************************/
    /******** SEGMENT INSERTION **********/
    /*************************************/
    if (improvebehave.insertsegment)
    {
        for (i=0; i<nbedges; i++)
        {
            if (improvebehave.verbosity > 5)
            {
                printf("Attempting segment insertion...\n");
            }
            
            /* stats */
            stats.segmentinsertattempts++;
            
            /* perform initial vertex insertion */
            success = segmentinsert(mesh, v1, v2, v3, v4, 
                          numboundedgetets[i], boundedgetets[i],
                          boundedgefaces[i],
                          &vnew, 
                          newfaces, &numnewfaces,
                          newtets, &numnewtets, true);
            
            /* if one of the new tets is inverted, give up */
            if (success == false)
            {
                invertjournalupto(mesh, beforeid);
                continue;
            }
            
            if (improvebehave.verbosity > 5)
            {
                printf("segment insertion returned this set of split tets:\n");
                printtets(mesh, newtets, numnewtets);
            }
            
            /* dig out optimal cavity */
            optimalcavity(mesh, vnew,
                        newtets, numnewtets, 
                        newfaces, numnewfaces, 
                        newtets, &numnewtets,
                        &worstdeletedqual,
                        &origcavityqual, true);
                    
            if (improvebehave.verbosity > 5)
            {
                printf("The quality of the split tet was %g vs worst deleted to form cavity of %g\n", pq(inquality), pq(worstdeletedqual));
            }
        
            /* the quality we have to beat when improving the cavity is the
               worst of the split tet and those of the dug-out cavity */
            minquality = (worstdeletedqual < inquality) ? worstdeletedqual : inquality;
        
            if (improvebehave.verbosity > 5)
            {
                printf("Here is the cavity returned (it has %d tets):\n", numnewtets);
                printtets(mesh, newtets, numnewtets);
            }
            
            /* improve the cavity */
#ifndef NO_TIMER
            gettimeofday(&tv0, &tz);
#endif /* not NO_TIMER */
            cavityqual = improvecavity(mesh, vnew, newtets, numnewtets, tryhard, cavitystackptr, &longest, &shortest);
#ifndef NO_TIMER
            gettimeofday(&tv1, &tz);
            stats.cavityimproveusec += usecelapsed(tv0, tv1);
#endif /* not NO_TIMER */
            
            if ( improvebehave.sizing &&
                 ((longest > improvebehave.targetedgelength * improvebehave.longerfactor) ||
                 (shortest < improvebehave.targetedgelength * improvebehave.shorterfactor))
               )
            {
                if (improvebehave.verbosity > 3)
                {
                    printf("Aborting insertion because it would create bad edge of length %g/%g, target is %g\n", longest, shortest, improvebehave.targetedgelength);
                }
                sizeok = false;
            }
            else
            {
                sizeok = true;
            }
            
            /* did we succeed? */
            if (cavityqual > (minquality + MININSERTIONIMPROVEMENT) && sizeok)
            {
                /* yes. keep the insertion */
                if (improvebehave.verbosity > 4)
                {
                    printf("Segment vertex insertion succeeded. minqual = %g, cavityqual = %g\n", pq(minquality), pq(cavityqual));
                }
                
                /* add the affected tets to the output stack */
                if (outstack != NULL)
                {
                    appendstack(&cavitystack, outstack);
                }
                stackdeinit(&cavitystack);
                
                /* stats */
                stats.segmentinsertsuccesses++;
                *outquality = cavityqual;
                return true;
            }
            else
            {
                /* no. reverse the damage */
                if (improvebehave.verbosity > 5)
                {
                    printf("Segment vertex insertion failed. minqual = %g, cavityqual = %g\n", pq(minquality), pq(cavityqual));
                }
            
                invertjournalupto(mesh, beforeid);
            }
        }
    }
    
    stackdeinit(&cavitystack);
    return false;
}

/* attempt insertion on a stack of tets */
bool insertpass(struct tetcomplex *mesh,
                struct arraypoolstack *tetstack,
                struct arraypoolstack *outstack,
                int qualmeasure,
                starreal meanqualafter[],
                starreal *minqualafter,
                starreal okayqual,
                bool quiet)
{
    struct improvetet *stacktet;
    bool success = false;
    starreal minqualbefore;
    starreal meanqualbefore[NUMMEANTHRESHOLDS];
    int attempts=0;
    int inserts=0;
    starreal cavityqual;
    starreal meshworstqual;
    bool tryhard = false;
    int beforeid = lastjournalentry();
    
    int origstacksize = tetstack->top + 1;
    
    /* reset output stack */
    if (outstack != NULL)
    {
        /* copy the input stack to output stack */
        stackrestart(outstack);
        copystack(tetstack, outstack);
    }
    
    /* get global worst for paranoid insertion */
    meshworstqual = HUGEFLOAT;
    if (INSERTPARANOID)
    {
        meshquality(mesh, qualmeasure, meanqualbefore, &meshworstqual);
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
    
    /* now go through the stack collecting information */
    while (tetstack->top != STACKEMPTY)
    {
        if (improvebehave.verbosity > 4 && (tetstack->top % 20 == 0))
        {
            textcolor(BRIGHT, MAGENTA, BLACK);
            printf("%ld remaining tets\r\n", tetstack->top);
            textcolor(RESET, WHITE, BLACK);
        }
        
        /* pull the top tet off the stack */
        stacktet = (struct improvetet *) stackpop(tetstack);
        
        /* make sure this tet still exists */
        if (tetexists(mesh,
                      stacktet->verts[0],
                      stacktet->verts[1],
                      stacktet->verts[2],
                      stacktet->verts[3]) == false) continue;
        
        /* because this tet might have been fixed up in a previous insertion,
           recompute its quality */
        stacktet->quality = tetquality(mesh, 
                                       stacktet->verts[0],
                                       stacktet->verts[1], 
                                       stacktet->verts[2], 
                                       stacktet->verts[3],
                                       improvebehave.qualmeasure);
        
        /* if this tet is already above the demanded quality, don't attempt insertion */
        if (stacktet->quality >= okayqual && okayqual > 0.0)
        {
            if (improvebehave.verbosity > 5)
            {
                printf("skipping insertion attempt because tet quality %g is better than %g\n", pq(stacktet->quality), pq(okayqual));
            }
            continue;
        }
        
        /* determine if we should try extra hard for this insertion to work out */
        if (okayqual > 0.0 || stacktet->quality - minqualbefore < CLOSETOWORST)
        {
            tryhard = true;
        }
        else
        {
            tryhard = false;
        }
        
        /* attempt to insert a vertex */
        success = insertvertex(mesh,
                               stacktet->verts[0],
                               stacktet->verts[1], 
                               stacktet->verts[2], 
                               stacktet->verts[3],
                               stacktet->quality,
                               minqualbefore,
                               &cavityqual,
                               outstack,
                               tryhard);
        attempts++;
        inserts += success ? 1 : 0;
        
        if (INSERTPARANOID)
        {
            starreal newworst, newmean[NUMMEANTHRESHOLDS];
            meshquality(mesh, qualmeasure, newmean, &newworst);
            if (success == false)
            {
                if (meshworstqual != newworst)
                {
                    printf("meshworstqual = %g, newworst = %g\n", pq(meshworstqual), pq(newworst));
                }
                assert(meshworstqual == newworst);
            }
            else
            {
                if (newworst < meshworstqual)
                {
                    printf("uhoh... worst qual went from %g to %g (diff = %g) on success...\n", pq(meshworstqual), pq(newworst), pq(newworst - meshworstqual));
                    printjournaltop(5);
                    worsttetreport(mesh, improvebehave.qualmeasure, 1.0);
                }
                assert(newworst >= meshworstqual);
                meshworstqual = newworst;
            }
        }
        
        success = false;
    }
    
    if (outstack != NULL)
    {
        stackquality(mesh, outstack, qualmeasure, meanqualafter, minqualafter);
        if (improvebehave.verbosity > 1)
        {
            printf("just completed insertion improvment pass.\n");
            printf("    input stack had %d tets\n", origstacksize);
            printf("    output stack had %lu tets\n", outstack->top+1);
            printf("    claimed input quality is            %g\n", pq(minqualbefore));
            printf("    worst quality in the output stack is %g\n", pq(*minqualafter));
            printf("    Insertion successes / attempts: %d/%d\n", inserts, attempts);
        }
        
        /* if the worst quality didn't improve on insertion, undo this insertion pass */
        if (*minqualafter < minqualbefore || (*minqualafter == minqualbefore && inserts == 0))
        {
            if (improvebehave.verbosity > 1)
            {
                printf("Undoing last insertion pass, minqualbefore was %g but after was %g\n", minqualbefore, *minqualafter);
            }
            invertjournalupto(mesh, beforeid);
            
            /* reset minimum output quality */
            *minqualafter = minqualbefore;
            
            return false;
        }
    }
    
    if (improvebehave.verbosity > 4)
    {
        printf("Insertion successes / attempts: %d/%d\n", inserts, attempts);
    }
    
    if (inserts > 0) return true;
    
    return false;
}


/* go after the worst tets. with insertion */
void worsttetattack(struct tetcomplex *mesh,
                    int qualmeasure,
                    starreal percentinsert,
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
        fillstackpercent(mesh, &tetstack, qualmeasure, percentinsert, meanqual, &minqual);
        if (improvebehave.verbosity > 4)
        {
            textcolor(BRIGHT, MAGENTA, BLACK);
            printf("Attempting vertex insertion on the worst %g percent of tets (%ld).\n", percentinsert * 100.0, tetstack.top);
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
            printf("Attempting DESPERATE vertex insertion on the %ld tets with qual less than %g (%g degrees).\n", tetstack.top, fillthresh, pq(fillthresh));
            textcolor(RESET, WHITE, BLACK);
        }
    }
    
    origmeshworstqual = meshworstqual = minqual;
    
    /* perform insertion pass on stack of tets */
    insertpass(mesh, &tetstack, NULL, qualmeasure, NULL, outminqual, -1.0, false);
    
    /* free the stack of tets */
    stackdeinit(&tetstack);
    
    meshquality(mesh, qualmeasure, outmeanqual, outminqual);
    
    if (!improvebehave.fixedsmooth) 
    {
        if (*outminqual < origmeshworstqual)
        {
            printf("crap, mesh min qual = %g after insert pass, %g before.\n", pq(*outminqual), pq(origmeshworstqual));
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













