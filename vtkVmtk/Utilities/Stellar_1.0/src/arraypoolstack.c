/*****************************************************************************/
/*                                                                           */
/*  arraypoolstack                                                           */
/*                                                                           */
/*  This is a simple stack implementation based on arraypools.               */
/*                                                                           */
/*****************************************************************************/

/* initialize stack data structure */
void stackinit(struct arraypoolstack *stack, arraypoolulong objectbytes)
{
    /* call initialization for the arraypool */
    arraypoolinit(&(stack->pool), objectbytes, LOG2TETSPERSTACKBLOCK, 0);
    /* initialize the stack top index */
    stack->top = STACKEMPTY;
    stack->maxtop = STACKEMPTY;
}

/* reset stack data structure */
void stackrestart(struct arraypoolstack *stack)
{
    /* call restart for the arraypool */
    arraypoolrestart(&(stack->pool));
    /* initialize the stack top index */
    stack->top = STACKEMPTY;
    stack->maxtop = STACKEMPTY;
}

/* free the stack data structure */
void stackdeinit(struct arraypoolstack *stack)
{
    arraypooldeinit(&(stack->pool));
}

/* push a new element on the stack, return pointer where new element
   should go */
void* stackpush(struct arraypoolstack *stack)
{
    void *newobject;
    
    /* increment the top index */
    stack->top++;
    /* check whether we've allocated this index before */
    if (stack->top > stack->maxtop)
    {
        stack->maxtop = stack->top;
        /* we need to call force lookup to allocate new object */
        newobject = arraypoolforcelookup(&(stack->pool), (unsigned long) stack->top);
    }
    else
    {
        /* we can just do the lazy lookup */
        newobject = arraypoolfastlookup(&(stack->pool), (unsigned long) stack->top);
    }
    
    return newobject;
}

/* fetch the address of the top stack item */
void* stackpop(struct arraypoolstack *stack)
{
    /* we know the object is there so we can do the unsafe lookup */
    void *topobject = arraypoolfastlookup(&(stack->pool), (unsigned long) stack->top);
    stack->top--;
    assert(stack->top >= STACKEMPTY);
    
    return topobject;
}

/* convenience struct to use standard library quick sort funtion */
struct sorttet
{
    starreal qual;
    tag id;
};

/* compare two starreal numbers as qualities of tets to be sorted */
int comparesorttets(const void * a, const void * b)
{
    if ( ((struct sorttet *)a)->qual > ((struct sorttet *)b)->qual) return 1;
    if ( ((struct sorttet *)a)->qual < ((struct sorttet *)b)->qual) return -1;
    return 0;
}

/* function to sort an array of reals */
void qsorttets(struct sorttet *array,
               int arraylength)
{
    qsort(array, (size_t) arraylength, sizeof(struct sorttet), comparesorttets);
}

/* sort stack from worst quality to best */
/* TODO do in-place sort of arraypool? */
void sortstack(struct arraypoolstack *stack)
{
    int i;
    /* allocate an array of sorttets big enough for the stack */
    struct improvetet *tet, *fromtet, *totet;
    struct sorttet *sortarray;
    int size = stack->top + 1;
    
    struct arraypoolstack newstack;
    
    stackinit(&newstack, sizeof(struct improvetet));
    sortarray = (struct sorttet *) malloc((stack->top + 1) * sizeof(struct sorttet));
    
    if (improvebehave.verbosity > 5)
    {
        printf("starting sort of stack of size %lu... ", stack->top);
    }
    
    /* copy the qualities and tags into the array */
    for (i=0; i<=stack->top; i++)
    {
        /* fetch this item from the stack */
        tet = (struct improvetet *) arraypoolfastlookup(&(stack->pool), (unsigned long) i);
        
        /* copy into array */
        sortarray[i].id = i;
        sortarray[i].qual = tet->quality;
    }
    
    /* sort the array */
    qsorttets(sortarray, size);
    
    /* build up the new stack in the right order */
    for (i=stack->top; i>=0; i--)
    {
        /* get the right tet out of the unsorted stack */
        fromtet = (struct improvetet *) arraypoolfastlookup(&(stack->pool), (unsigned long) sortarray[i].id);
        totet = (struct improvetet *) stackpush(&newstack);
        
        /* copy it over */
        memcpy(totet, fromtet, sizeof(struct improvetet));
    }
    
    assert(stack->top == newstack.top);
    
    /* change output stack */
    stackdeinit(stack);
    memcpy(stack, &newstack, sizeof(struct arraypoolstack));
    
    /* deallocate sort array */
    free(sortarray);
    
    if (improvebehave.verbosity > 5)
    {
        printf("done\n");
    }
}

/* exactly copy one stack to another */
void copystack(struct arraypoolstack *fromstack,
               struct arraypoolstack *tostack)
{
    struct improvetet *fromtet, *totet;
    int i;
    
    /* reset the to stack */
    stackrestart(tostack);
    
    /* copy every element */
    for (i=0; i<=fromstack->top; i++)
    {
        /* get the right tet out of stack */
        fromtet = (struct improvetet *) arraypoolfastlookup(&(fromstack->pool), (unsigned long) i);
        totet = (struct improvetet *) stackpush(tostack);
        
        /* copy it over */
        memcpy(totet, fromtet, sizeof(struct improvetet));
    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("copied stack\n");
        for (i=0; i<10; i++)
        {
            fromtet = (struct improvetet *) arraypoolfastlookup(&(fromstack->pool), (unsigned long) i);
            totet = (struct improvetet *) arraypoolfastlookup(&(tostack->pool), (unsigned long) i);
            printf("[%d] fromtet %g to tet %g", i, fromtet->quality, totet->quality);
            assert(fromtet->quality == totet->quality);
        }
    }
    
    assert(fromstack->top == tostack->top);
}

/* permute the order of a tet's vertices so that edge[0], edge[1]
   are the first two vertices */
void permuteedge(struct tetcomplex *mesh,
                 tag tet[4],
                 int edge[2])
{
    tag temptet[4];
    temptet[0] = tet[0];
    temptet[1] = tet[1];
    temptet[2] = tet[2];
    temptet[3] = tet[3];
    
    /* there are six cases. original tet is (0, 1, 2, 3) */
    /* edge  new permutation */
    /* 0 1   (0, 1, 2, 3)    */
    /* 0 2   (0, 2, 3, 1)    */
    /* 0 3   (0, 3, 2, 1)    */
    /* 1 2   (1, 2, 0, 3)    */
    /* 1 3   (1, 3, 2, 0)    */
    /* 2 3   (2, 3, 0, 1)    */
    
    if (edge[0] == 0 && edge[1] == 2)
    {
        tet[0] = temptet[0];
        tet[1] = temptet[2];
        tet[2] = temptet[3];
        tet[3] = temptet[1];
    }
    if (edge[0] == 0 && edge[1] == 3)
    {
        tet[0] = temptet[0];
        tet[1] = temptet[3];
        tet[2] = temptet[1];
        tet[3] = temptet[2];
    }
    if (edge[0] == 1 && edge[1] == 2)
    {
        tet[0] = temptet[1];
        tet[1] = temptet[2];
        tet[2] = temptet[0];
        tet[3] = temptet[3];
    }
    if (edge[0] == 1 && edge[1] == 3)
    {
        tet[0] = temptet[1];
        tet[1] = temptet[3];
        tet[2] = temptet[2];
        tet[3] = temptet[0];
    }
    if (edge[0] == 2 && edge[1] == 3)
    {
        tet[0] = temptet[2];
        tet[1] = temptet[3];
        tet[2] = temptet[0];
        tet[3] = temptet[1];
    }
    
    /* should have positive orientation */
    assert(tetvolume(mesh, tet[0], tet[1], tet[2], tet[3]) > 0.0);
}

/* peer into a stack of tets to see if a tet starting with edge (vtx1, vtx2)
   is in there somewhere */
bool tetinstackedge(struct arraypoolstack *tetstack,
                    tag vtx1, tag vtx2)
{
    int i;                  /* loop index */
    struct improvetet *tet; /* current tet */
    
    for (i=0; i<=tetstack->top; i++)
    {
        /* fetch this item from the stack */
        tet = (struct improvetet *) arraypoolfastlookup(&(tetstack->pool), (unsigned long) i);
        /* does it start with the vertex we were looking for? */
        if (tet->verts[0] == vtx1 && tet->verts[1] == vtx2) return true;
    }
    return false;
}

/* given a mesh and a minimum and maximum edge length,
   fill two stacks with tets edges shorter than the minimum
   or longer than the maximum. Each unique edge is represented
   in just one tet. Order the vertices in the tets
   such that the too short (too long) edge comes first */
void filledgestacks(struct tetcomplex *mesh,
                    struct arraypoolstack *longstack,
                    struct arraypoolstack *shortstack,
                    starreal minlength,
                    starreal maxlength,
                    starreal *minedge,
                    starreal *maxedge,
                    starreal *meanedge)
{
    struct tetcomplexposition pos; /* position of iterator in the mesh */
    tag tet[4];                    /* the current tet */
    int numedges = 0;              /* number of edges in the mesh */
    tag adjacencies[2];
    tag origin;
    tag destin;
    tag apex;
    tag stopvtx;
    tag searchvtx = NOTATAG;
    tag swaptag;
    int writeflag = 0;
    int i;
    struct improvetet *stacktet;   /* point to stack tet */
    int edge[2];
    starreal edgev[3];
    starreal origcoord[3], destcoord[3], E[3][3];
    starreal edgelength;
    
    *minedge = HUGEFLOAT;
    *maxedge = 0.0;
    
    /* make sure the stacks are empty */
    if (longstack != NULL) stackrestart(longstack);
    if (shortstack != NULL) stackrestart(shortstack);
    
    /* initialize the iterator over the mesh */
    tetcomplexiteratorinit(mesh, &pos);
    /* retrieve the first tet in the mesh */
    tetcomplexiteratenoghosts(&pos, tet);
    
    /* for each tet */
    while (tet[0] != STOP) 
    {
        /* Look at all six edges of the tetrahedron. */
        /* Iteration over unique edges taken from Jonathan's outputedges() */
        for (i = 0; i < 6; i++) 
        {
            if (tet[0] < tet[1]) 
            {
                origin = tet[0];
                destin = tet[1];
                apex = tet[2];
                stopvtx = tet[3];
                edge[0] = 0;
                edge[1] = 1;
            } 
            else
            {
                origin = tet[1];
                destin = tet[0];
                apex = tet[3];
                stopvtx = tet[2];
                edge[0] = 1;
                edge[1] = 0;
            }
            
            searchvtx = apex;
            writeflag = 1;
            do 
            {
                if (!tetcomplexadjacencies(mesh, origin, destin, searchvtx,
                                         adjacencies)) 
                {
                    printf("Error computing edge statistics:\n");
                    printf("  Complex returned tetrahedron that can't be queried.\n");
                    internalerror();
                }
                if (adjacencies[0] == GHOSTVERTEX) 
                {
                    writeflag = searchvtx == apex;
                }
                searchvtx = adjacencies[0];
                if (searchvtx < apex) 
                {
                    writeflag = 0;
                }
            } 
            while (writeflag && (searchvtx != stopvtx) && (searchvtx != GHOSTVERTEX));
            
            /* if this is an edge that hasn't been counted yet */
            if (writeflag) 
            {
                /* measure the length of this edge */
                vcopy(((struct vertex *) tetcomplextag2vertex(mesh, origin))->coord, origcoord);
                vcopy(((struct vertex *) tetcomplextag2vertex(mesh, destin))->coord, destcoord);
                
                /* if anisotropic meshing is enabled, warp the points according the
                   deformation tensor at their barycenter */
                if (improvebehave.anisotropic)
                {
                    /* fetch the deformation tensor at the midpoint of this tet */
                    edgetensor(mesh, origin, destin, E);
                    
                    /* transform each vertex */
                    tensortransform(origcoord, origcoord, E);
                    tensortransform(destcoord, destcoord, E);
                }
                
                vsub(destcoord, origcoord, edgev);
                edgelength = vlength(edgev);
                
                /* keep track of minimum, maximum and average edge lengths */
                if (edgelength < *minedge) *minedge = edgelength;
                if (edgelength > *maxedge) *maxedge = edgelength;
                *meanedge += edgelength;
                numedges++;
                
                /* if this edge is too short, push it on the appropriate stack */
                if (shortstack != NULL)
                {
                    if (edgelength < minlength && !tetinstackedge(shortstack, origin, destin))
                    {
                        /* push this tet on the "too short" stack */
                        stacktet = (struct improvetet *) stackpush(shortstack);
                        stacktet->quality = edgelength;
                        stacktet->verts[0] = tet[0];
                        stacktet->verts[1] = tet[1];
                        stacktet->verts[2] = tet[2];
                        stacktet->verts[3] = tet[3];

                        /* permute the vertices of the tet on the stack so
                           that the shortest edge comes as the first two verts */
                        permuteedge(mesh, stacktet->verts, edge);
                    }
                }
                
                /* if this edge is too long push it on the appropriate stack */
                if (longstack != NULL)
                {
                    if (edgelength > maxlength && !tetinstackedge(longstack, origin, destin))
                    {
                        /* push this tet on the "too short" stack */
                        stacktet = (struct improvetet *) stackpush(longstack);
                        stacktet->quality = edgelength;
                        stacktet->verts[0] = tet[0];
                        stacktet->verts[1] = tet[1];
                        stacktet->verts[2] = tet[2];
                        stacktet->verts[3] = tet[3];

                        /* permute the vertices of the tet on the stack so
                           that the longest edge comes as the first two verts */
                        permuteedge(mesh, stacktet->verts, edge);
                    }
                }
            }
            
            /* The following shift cycles (tet[0], tet[1]) through all the edges   */
            /*   while maintaining the tetrahedron's orientation.  The schedule is */
            /*   i = 0:  0 1 2 3 => 1 2 0 3                                        */
            /*   i = 1:  1 2 0 3 => 1 3 2 0                                        */
            /*   i = 2:  1 3 2 0 => 3 2 1 0                                        */
            /*   i = 3:  3 2 1 0 => 3 0 2 1                                        */
            /*   i = 4:  3 0 2 1 => 0 2 3 1                                        */
            /*   i = 5:  0 2 3 1 => 0 1 2 3 (which isn't used).                    */
            if ((i & 1) == 0) 
            {
                swaptag = tet[0];
                tet[0] = tet[1];
                tet[1] = tet[2];
                tet[2] = swaptag;
            } 
            else
            {
                swaptag = tet[3];
                tet[3] = tet[2];
                tet[2] = tet[1];
                tet[1] = swaptag;
            }
        }
        
        /* fetch the next tet from the mesh */
        tetcomplexiteratenoghosts(&pos, tet);
    }
    
    *meanedge /= numedges;
    
    if (improvebehave.verbosity > 4)
    {
        printf("Just built edge stacks.\n");
        printf("    Total edges in mesh: %d\n", numedges);
        if (shortstack != NULL) printf("    Short stack has %d tets with edge shorter than %g\n", (int) shortstack->top+1, minlength);
        if (longstack != NULL) printf("    Long stack has %d tets with edge longer than %g\n", (int) longstack->top+1, maxlength);
    }
}


/* given a mesh and a quality threshold,
   return a stack of tets with quality 
   at or below that threshold. this function
   assumes that the stack has already been 
   initialized. */
void fillstackqual(struct tetcomplex *mesh,
                   struct arraypoolstack *stack,
                   int qualmeasure,
                   starreal threshold,
                   starreal meanquals[],
                   starreal *minqual)
{
    struct tetcomplexposition pos; /* position of iterator in the mesh */
    tag tet[4];                    /* the current tet */
    starreal quality;                /* the quality of the current tet */
    struct improvetet *stacktet;   /* point to stack tet */
    int numtets = 0;
    int i;
    
    /* make sure the stack is empty */
    stackrestart(stack);
    
    /* initialize the iterator over the mesh */
    tetcomplexiteratorinit(mesh, &pos);
    /* retrieve the first tet in the mesh */
    tetcomplexiteratenoghosts(&pos, tet);
    
    *minqual = HUGEFLOAT;
    for (i=0; i<NUMMEANTHRESHOLDS; i++)
    {
        meanquals[i] = 0.0;
    }
    
    /* for each tet */
    while (tet[0] != STOP) 
    {
        /* compute the quality of this tet */
        quality = tetquality(mesh, tet[0], tet[1], tet[2], tet[3], qualmeasure);
        
        /* keep track of minimum quality */
        if (quality < *minqual) *minqual = quality;
        
        /* if this tet is worse than the threshold */
        if (quality < threshold)
        {
            /* track thresholded mean qualities only for tets actually included in the stack */
            numtets++;
            for (i=0; i<NUMMEANTHRESHOLDS; i++)
            {
                meanquals[i] += (quality < meanthresholds[improvebehave.qualmeasure][i]) ? quality : meanthresholds[improvebehave.qualmeasure][i];
            }
            
            /* push this tet on the "to be fixed" stack */
            stacktet = (struct improvetet *) stackpush(stack);
            stacktet->quality = quality;
            stacktet->verts[0] = tet[0];
            stacktet->verts[1] = tet[1];
            stacktet->verts[2] = tet[2];
            stacktet->verts[3] = tet[3];
        }
        
        /* fetch the next tet from the mesh */
        tetcomplexiteratenoghosts(&pos, tet);
    }
    
    /* compute thresholded means */
    for (i=0; i<NUMMEANTHRESHOLDS; i++)
    {
        if (numtets != 0)
        {
            meanquals[i] /= (starreal) numtets;
        }
        else
        {
            meanquals[i] = 0.0;
        }
    }
    
    /* sort the stack of tets from worst to best */
    sortstack(stack);
}

/* given a mesh and a percentage p,
   return the worst numtets * p tets in the mesh */
void fillstackpercent(struct tetcomplex *mesh,
                      struct arraypoolstack *stack,
                      int qualmeasure,
                      starreal percent,
                      starreal meanquals[],
                      starreal *minqual)
{
    struct improvetet *stacktet;   /* point to stack tet */
    struct improvetet *outstacktet;
    int i;
    
    int numouttets = 0.0;
    
    /* create a local stack for the global build */
    struct arraypoolstack alltetstack;
    
    assert(percent > 0.0 && percent <= 1.0);
    
    stackinit(&alltetstack, sizeof(struct improvetet));
    
    /* fill it with every tet in the mesh */
    fillstackqual(mesh, &alltetstack, qualmeasure, HUGEFLOAT, meanquals, minqual);
    
    /* sort it from worst to best */
    sortstack(&alltetstack);
    
    /* figure out how many tets to take */
    numouttets = (int) (percent * ((starreal) alltetstack.top + 1.0));
    assert(numouttets <= alltetstack.top + 1);
    
    if (numouttets > MAXINSERTTETS)
    {
        numouttets = MAXINSERTTETS;
    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("in percent stack num out tets is %d\n", numouttets);
    }
    
    /* make sure the output stack is empty */
    stackrestart(stack);
    
    /* now, pop numouttets off the output stack */
    for (i=0; i<numouttets; i++)
    {
        /* pull the top tet off the stack */
        stacktet = (struct improvetet *) stackpop(&alltetstack);
        
        /* make sure there are still tets in the stack */
        if (alltetstack.top == STACKEMPTY) break;
        
        /* make sure this tet still exists */
        if (tetexists(mesh,
                      stacktet->verts[0],
                      stacktet->verts[1],
                      stacktet->verts[2],
                      stacktet->verts[3]) == false) continue;
                      
        if (improvebehave.verbosity > 5)
        {
            printf("adding tet with qual %g to outstack\n", stacktet->quality);
        }
        
        outstacktet = (struct improvetet *) stackpush(stack);
        outstacktet->quality = stacktet->quality;
        outstacktet->verts[0] = stacktet->verts[0];
        outstacktet->verts[1] = stacktet->verts[1];
        outstacktet->verts[2] = stacktet->verts[2];
        outstacktet->verts[3] = stacktet->verts[3];
    }
    
    /* sort output from worst to best */
    sortstack(stack);
    
    if (improvebehave.verbosity > 5)
    {
        printf("done finding worst %g percent of tets, returning stack of %lu of them.\n", percent * 100.0, stack->top + 1);
    }
}

/* run through a stack of tets, initializing each vertex with
   a flag indicating that it has not yet been smoothed */
void initsmoothedvertlist(struct arraypoolstack *tetstack,
                          struct arraypool *vertlist)
{
    int i,j;                  /* loop index */
    struct improvetet *tet;   /* current tet */
    
    if (improvebehave.verbosity > 5)
    {
        printf("Initializing %lu tets of vertices to not smoothed... ", tetstack->top);
    }
    
    /* for every vertex in the stack */
    for (i=0; i<=tetstack->top; i++)
    {
        /* fetch this item from the stack */
        tet = (struct improvetet *) arraypoolfastlookup(&(tetstack->pool), (unsigned long) i);
        /* for each vertex */
        for (j = 0; j<4; j++)
        {
            /* initialize this vertex to not smoothed */
            *((bool *) arraypoolforcelookup(vertlist, tet->verts[j])) = false;
        }
    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("done.\n");
    }
}

/* peer into a stack of tets to see if a tet starting with vtx
   is in there somewhere */
bool tetinstackfirstvert(struct arraypoolstack *tetstack,
                tag vtx)
{
    int i;                  /* loop index */
    struct improvetet *tet; /* current tet */
    
    for (i=0; i<=tetstack->top; i++)
    {
        /* fetch this item from the stack */
        tet = (struct improvetet *) arraypoolfastlookup(&(tetstack->pool), (unsigned long) i);
        /* does it start with the vertex we were looking for? */
        if (tet->verts[0] == vtx) return true;
    }
    return false;
}

/* peer into a stack of vertices to see if a particular tag is present */
bool vertinstack(struct arraypoolstack *vertstack,
                         tag vtx)
{
    int i;     /* loop index */
    tag *curv; /* current tet */
    int j=0;
    
    for (i=0; i<=vertstack->top; i++)
    {
        j++;
        
        /* fetch this item from the stack */
        curv = (tag *) arraypoolfastlookup(&(vertstack->pool), (unsigned long) i);
        /* does it start with the vertex we were looking for? */
        if (*curv == vtx) return true;
    }
    return false;
}

/* peer into a stack of tets to see if a tet
   is in there somewhere */
bool tetinstack(struct arraypoolstack *tetstack,
                tag v1,
                tag v2,
                tag v3,
                tag v4)
{
    int i;                  /* loop index */
    struct improvetet *tet; /* current tet */
    
    for (i=0; i<=tetstack->top; i++)
    {
        /* fetch this item from the stack */
        tet = (struct improvetet *) arraypoolfastlookup(&(tetstack->pool), (unsigned long) i);
        /* does this tet match? */
        if (sametet(tet->verts[0], tet->verts[1], tet->verts[2], tet->verts[3],
                    v1, v2, v3, v4))
        {
            return true;
        }
    }
    return false;
}

/* append the contents of one stack to the top of another */
/* only adds tets if they do not already exist in tostack */
void appendstack(struct arraypoolstack *fromstack,
                 struct arraypoolstack *tostack)
{
    struct improvetet *fromtet, *totet;
    int i;
    
    /* copy every element */
    for (i=0; i<=fromstack->top; i++)
    {
        /* get the right tet out of stack */
        fromtet = (struct improvetet *) arraypoolfastlookup(&(fromstack->pool), (unsigned long) i);
        
        /* if this tet is not in the target stack */
        if (tetinstack(tostack, fromtet->verts[0], fromtet->verts[1], fromtet->verts[2], fromtet->verts[3]) == false)
        {
            totet = (struct improvetet *) stackpush(tostack);
        
            /* copy it over */
            memcpy(totet, fromtet, sizeof(struct improvetet));
        }
    }
}

/* compute the mean and minimum element
   qualities in the meash (multiple thresholded means) */
void meshquality(struct tetcomplex *mesh,
                    int qualmeasure,
                    starreal *meanqual,
                    starreal *minqual)
{
    struct arraypoolstack tetstack;   /* stack of tets  */
    
    stackinit(&tetstack, sizeof(struct improvetet));
    
    /* fill the stack of tets with all tets in the mesh */
    fillstackqual(mesh, &tetstack, qualmeasure, HUGEFLOAT, meanqual, minqual);
    
    stats.finishworstqual = *minqual;
    
    /* free the stack of tets */
    stackdeinit(&tetstack);
}

/* return the worst quality of all elements in the mesh */
double worstquality(struct tetcomplex*mesh)
{
    starreal meanqual[NUMMEANTHRESHOLDS];
    starreal minqual;
    
    struct arraypoolstack tetstack;   /* stack of tets  */
    
    stackinit(&tetstack, sizeof(struct improvetet));
    
    /* fill the stack of tets with all tets in the mesh */
    fillstackqual(mesh, &tetstack, improvebehave.qualmeasure, HUGEFLOAT, meanqual, &minqual);
    
    /* free the stack of tets */
    stackdeinit(&tetstack);
    
    return minqual;
}

/* find the meand and minimum qualities
   of all the tets in the stack (that still exist) */
void stackquality(struct tetcomplex *mesh,
                  struct arraypoolstack *tetstack,
                  int qualmeasure,
                  starreal meanqual[],
                  starreal *minqual)
{
    int i,j;                  /* loop index */
    struct improvetet *tet;   /* current tet */
    int nonexist = 0;
    starreal worstqual = HUGEFLOAT;
    int numtets = 0;
    starreal newqual;
    
    for (j=0; j<NUMMEANTHRESHOLDS; j++)
    {
        meanqual[j] = 0.0;
    }
    
    for (i=0; i<=tetstack->top; i++)
    {
        /* fetch this item from the stack */
        tet = (struct improvetet *) arraypoolfastlookup(&(tetstack->pool), (unsigned long) i);

        /* check that it exists */
        if (tetexists(mesh, tet->verts[0], tet->verts[1], tet->verts[2], tet->verts[3]) == 0)
        {
            nonexist++;
            continue;
        }
        
        /* it exists, compute it's quality */
        newqual = tetquality(mesh, tet->verts[0], tet->verts[1], tet->verts[2], tet->verts[3], qualmeasure);
        
        /* track thresholded mean qualities only for tets actually included in the stack */
        numtets++;
        for (j=0; j<NUMMEANTHRESHOLDS; j++)
        {
            meanqual[j] += (newqual < meanthresholds[improvebehave.qualmeasure][j]) ? newqual : meanthresholds[improvebehave.qualmeasure][j];
        }
        
        /* is this a new low ? */
        if (newqual < worstqual) worstqual = newqual;

    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("non-existent tets: %d / %lu\n", nonexist, tetstack->top+1);
    }
    
    *minqual = worstqual;
    
    /* compute thresholded means */
    for (i=0; i<NUMMEANTHRESHOLDS; i++)
    {
        meanqual[i] /= (starreal) numtets;
    }
}

/* find the longest and shortest edge length in a stack of tets */
void longshortstack(struct tetcomplex *mesh,
                    struct arraypoolstack *tetstack,
                    starreal *longest,
                    starreal *shortest)
{
    int i;                  /* loop index */
    struct improvetet *tet; /* current tet */
    int nonexist = 0;
    starreal thislong, thisshort;
    int edge[2];
    *longest = 0.0;
    *shortest = HUGEFLOAT;
    
    for (i=0; i<=tetstack->top; i++)
    {
        /* fetch this item from the stack */
        tet = (struct improvetet *) arraypoolfastlookup(&(tetstack->pool), (unsigned long) i);

        /* check that it exists */
        if (tetexists(mesh, tet->verts[0], tet->verts[1], tet->verts[2], tet->verts[3]) == 0)
        {
            nonexist++;
            continue;
        }
        
        /* it exists, compute it's longest and shortest edge */
        thislong = tetedge(mesh, tet->verts[0], tet->verts[1], tet->verts[2], tet->verts[3], edge, true);
        thisshort = tetedge(mesh, tet->verts[0], tet->verts[1], tet->verts[2], tet->verts[3], edge, false);
        
        if (thislong > *longest) *longest = thislong;
        if (thisshort < *shortest) *shortest = thisshort;
    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("non-existent tets: %d / %lu\n", nonexist, tetstack->top+1);
    }
}

/* find the worst quality of all the tets in the stack (that still exist) */
starreal worststackquality(struct tetcomplex *mesh,
                         struct arraypoolstack *tetstack,
                         int qualmeasure)
{
    int i;                  /* loop index */
    struct improvetet *tet; /* current tet */
    int nonexist = 0;
    starreal worstqual = HUGEFLOAT;
    starreal newqual;
    
    for (i=0; i<=tetstack->top; i++)
    {
        /* fetch this item from the stack */
        tet = (struct improvetet *) arraypoolfastlookup(&(tetstack->pool), (unsigned long) i);

        /* check that it exists */
        if (tetexists(mesh, tet->verts[0], tet->verts[1], tet->verts[2], tet->verts[3]) == 0)
        {
            nonexist++;
            continue;
        }
        
        /* it exists, compute it's quality */
        newqual = tetquality(mesh, tet->verts[0], tet->verts[1], tet->verts[2], tet->verts[3], qualmeasure);
        
        /* is this a new low ? */
        if (newqual < worstqual) worstqual = newqual;

    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("non-existent tets: %d / %lu\n", nonexist, tetstack->top+1);
    }
    
    return worstqual;
}

/* print out all the tets in a stack */
void printstack(struct tetcomplex *mesh,
                struct arraypoolstack *tetstack)
{
    int i;                  /* loop index */
    struct improvetet *tet; /* current tet */
    
    printf("{");
    for (i=0; i<=tetstack->top; i++)
    {
        /* fetch this item from the stack */
        tet = (struct improvetet *) arraypoolfastlookup(&(tetstack->pool), (unsigned long) i);

        /* print out this tet */
        printtetverts(mesh, tet->verts);
        if (i != tetstack->top) printf(",\n");
    }
    printf("};\n");
}










