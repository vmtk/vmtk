/* size control functions */

/* compute statistics about tetrahedra volumes */
void volumestats(struct tetcomplex *mesh,
                 starreal *minvol,
                 starreal *maxvol,
                 starreal *meanvol)
{
    struct tetcomplexposition pos; /* position of iterator in the mesh */
    tag tet[4];                    /* the current tet */
    starreal volume;               /* the volume of the current tet */
    int numtets = 0;               /* number of tets in the mesh */
    
    *minvol = HUGEFLOAT;
    *maxvol = 0.0;
    *meanvol = 0.0;
    
    /* initialize the iterator over the mesh */
    tetcomplexiteratorinit(mesh, &pos);
    /* retrieve the first tet in the mesh */
    tetcomplexiteratenoghosts(&pos, tet);
    
    /* for each tet */
    while (tet[0] != STOP) 
    {
        /* compute the quality of this tet */
        volume = tetvolume(mesh, tet[0], tet[1], tet[2], tet[3]);
        
        /* keep track of minimum, maximum and average volume */
        if (volume < *minvol) *minvol = volume;
        if (volume > *maxvol) *maxvol = volume;
        *meanvol += volume;
        
        /* fetch the next tet from the mesh */
        tetcomplexiteratenoghosts(&pos, tet);
        numtets++;
    }
    assert(numtets != 0);
    assert(*meanvol > 0.0);
    assert(*minvol > 0.0);
    
    /* compute average */
    *meanvol /= numtets;
}

/* compute statistics about edge lengths in the mesh */
void edgelengthstats(struct tetcomplex *mesh,
                     starreal *minedge,
                     starreal *maxedge,
                     starreal *meanedge,
                     starreal *medianedge,
                     bool anisotropic)
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
    starreal edge[3];
    starreal edgelength;
    starreal origcoord[3];
    starreal destcoord[3];
    starreal E[3][3];
    
    /* a hack: store edge lengths in a tet stack to compute median */
    struct arraypoolstack edgestack;
    struct improvetet *edgetet;
    stackinit(&edgestack, sizeof(struct improvetet));
    
    *minedge = HUGEFLOAT;
    *maxedge = 0.0;
    *meanedge = 0.0;
    
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
            } 
            else
            {
                origin = tet[1];
                destin = tet[0];
                apex = tet[3];
                stopvtx = tet[2];
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
                if (improvebehave.anisotropic && anisotropic)
                {
                    /* fetch the deformation tensor at the midpoint of this tet */
                    edgetensor(mesh, origin, destin, E);
                    
                    /* transform each vertex */
                    tensortransform(origcoord, origcoord, E);
                    tensortransform(destcoord, destcoord, E);
                }
                
                vsub(destcoord, origcoord, edge);
                edgelength = vlength(edge);
                
                /* push this edge length on the stack */
                edgetet = (struct improvetet *) stackpush(&edgestack);
                edgetet->quality = edgelength;
                
                /* keep track of minimum, maximum and average edge lengths */
                if (edgelength < *minedge) *minedge = edgelength;
                if (edgelength > *maxedge) *maxedge = edgelength;
                (*meanedge) += edgelength;
                /*
                printf("origcoord is [%g %g %g]\n", origcoord[0], origcoord[1], origcoord[2]);
                printf("destcoord is [%g %g %g]\n", destcoord[0], destcoord[1], destcoord[2]);
                printf("edglength is %g\n", edgelength);
                printf("meanedge for edgenum %d is %g\n", numedges, *meanedge);
                */
                numedges++;
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
    assert(numedges != 0);
    assert(*meanedge > 0.0);
    assert(*minedge > 0.0);
    assert(*maxedge < HUGEFLOAT);
    
    /* compute average */
    *meanedge /= numedges;
    
    /* compute median */
    sortstack(&edgestack);
    *medianedge = ((struct improvetet *) arraypoolfastlookup(&(edgestack.pool), (unsigned long) (edgestack.top + 1) / 2))->quality;
    
    stackdeinit(&edgestack);
}

#define MINSIZEQUAL 1.0e-5

/* attempt to collapse edges that are too short */
void sizecontract(struct tetcomplex *mesh,
                  struct arraypoolstack *tetstack)
{
    struct arraypoolstack outstack;
    struct improvetet *stacktet, *outtet;
    bool contracted = false;
    starreal minqualbefore, minqualafter;
    
    /* for ring tet query */
    int numringtets;
    tag ringtets[MAXRINGTETS][4];
    tag bfverts[2];
    bool boundedge;
    int i;
    
    /* initialize output stack */
    stackinit(&outstack, sizeof(struct improvetet));
    
    /* go through each tet on the stack */
    while (tetstack->top != STACKEMPTY)
    {
        /* pull the top tet off the stack */
        stacktet = (struct improvetet *) stackpop(tetstack);
        
        /* check that this tet still exists */
        if (!tetexistsa(mesh, stacktet->verts))
        {
            continue;
        }
        
        /* try to contract this edge */
        contracted = edgecontract(mesh,
                                  stacktet->verts[0],
                                  stacktet->verts[1],
                                  stacktet->verts[2],
                                  stacktet->verts[3],
                                  &minqualbefore,
                                  &minqualafter,
                                  NULL, NULL,
                                  false);
        
        if (contracted)
        {
            if (improvebehave.verbosity > 5)
            {
                printf("Succeeded in contracting edge for size control. Qual went from %g to %g\n", minqualbefore, minqualafter);
            }
        }
        else
        {
            /* we couldn't directly contract this edge. but if we can contract any edge
               of a tetrahedron that surrounds this edge, this edge will go away too */
            /* fetch all the tets that surround this edge */
            boundedge = getedgering(mesh,
                                    stacktet->verts[0],
                                    stacktet->verts[1],
                                    stacktet->verts[2],
                                    stacktet->verts[3],
                                    &numringtets, ringtets, bfverts);
            
            if (improvebehave.verbosity > 5)
            {
                printf("Couldn't directly contract short edge (%d %d). Attempting collapse on surrounding tets\n", (int) stacktet->verts[0], (int) stacktet->verts[1]);
                printf("    Edge was boundary edge: %d\n", boundedge);
                printf("    Number surrounding tets: %d\n", numringtets);
            }
            
            /* try to collapse just first edge of each tet */
            for (i=0; i<numringtets; i++)
            {
                contracted = tryedgecontract(mesh,
                                             ringtets[i][0],
                                             ringtets[i][1],
                                             ringtets[i][2],
                                             ringtets[i][3],
                                             NULL,
                                             &minqualafter,
                                             false,
                                             true);
                                             
                /* if we succeed in contracting an edge, stop trying */
                if (contracted)
                {
                    if (improvebehave.verbosity > 5)
                    {
                        printf("Contraction succeeded on subsidiary tet %d!\n", i);
                    }
                    
                    break;
                }
            }
        }
        
        if (!contracted)
        {
            if (improvebehave.verbosity > 5)
            {
                printf("Couldn't contract any edge in size control. Adding to output stack.\n");
            }
            /* is this tet already on the output stack? */
            if (!tetinstack(&outstack, stacktet->verts[0], stacktet->verts[1], stacktet->verts[2], stacktet->verts[3]))
            {
                /* push tet on output stack */
                outtet = (struct improvetet *) stackpush(&outstack);
                outtet->verts[0] = stacktet->verts[0];
                outtet->verts[1] = stacktet->verts[1];
                outtet->verts[2] = stacktet->verts[2];
                outtet->verts[3] = stacktet->verts[3];
                outtet->quality = stacktet->quality;
            }
        }
    }
    
    /* copy the output stack over the input stack */
    copystack(&outstack, tetstack);
    /* free the local output stack */
    stackdeinit(&outstack);
}

#define SIZESPLITPARANOID 0
/* attempt to split edges that are too long */
void sizesplit(struct tetcomplex *mesh,
               struct arraypoolstack *tetstack)
{
    struct arraypoolstack outstack;
    struct improvetet *stacktet, *outtet;
    int numedgetets;
    tag edgetets[MAXRINGTETS][4];
    tag edgefaces[2];
    bool boundedge, success;
    tag vnew; 
    static tag newfaces[MAXCAVITYFACES][3]; /* faces of tets created after insert for seeding cavity drilling */
    int numnewfaces;               
    static tag newtets[MAXCAVITYTETS][4]; /* tets that filled in cavity */
    int numnewtets;
    int beforeid;
    starreal minquality = HUGEFLOAT;
    starreal worstdeletedqual, origcavityqual, cavityqual;
    starreal longest = 0.0, shortest = HUGEFLOAT;
    starreal means[NUMMEANTHRESHOLDS], meshworst, physworst;
    
    /* initialize output stack */
    stackinit(&outstack, sizeof(struct improvetet));
    
    /* go through each tet on the stack */
    while (tetstack->top != STACKEMPTY)
    {
        /* pull the top tet off the stack */
        stacktet = (struct improvetet *) stackpop(tetstack);
        
        /* check that this tet still exists */
        if (!tetexistsa(mesh, stacktet->verts))
        {
            continue;
        }
        
        /* save the mesh state before we attempt insertion */
        beforeid = lastjournalentry(); 
        
        /* fetch the ring of tets around this edge */
        boundedge = getedgering(mesh, 
                                stacktet->verts[0],
                                stacktet->verts[1],
                                stacktet->verts[2],
                                stacktet->verts[3],
                                &numedgetets,
                                edgetets, 
                                edgefaces);
        
        /* in the case of anisotropy, don't insert on boundary edge */
        /* TODO hack this out? */
        if (improvebehave.anisotropic && improvebehave.usequadrics && boundedge)
        {
            printf("bailing on sizing insertion because it's on the boundary...\n");
            continue;
        }
        
        /* attempt to insert a vertex at the midpoint of this edge */
        success = segmentinsert(mesh,
                                stacktet->verts[0],
                                stacktet->verts[1],
                                stacktet->verts[2],
                                stacktet->verts[3], 
                                numedgetets, 
                                edgetets,
                                edgefaces,
                                &vnew, 
                                newfaces, &numnewfaces,
                                newtets, &numnewtets, boundedge);
                                
        /* if that didn't work out, reverse things */
        if (!success)
        {
            if (improvebehave.verbosity > 0)
            {
                printf("Initial edge insert for size control failed\n");
            }
            invertjournalupto(mesh, beforeid);
            /* push tet on output stack */
            if (!tetinstack(&outstack, stacktet->verts[0], stacktet->verts[1], stacktet->verts[2], stacktet->verts[3]))
            {
                /* push tet on output stack */
                outtet = (struct improvetet *) stackpush(&outstack);
                outtet->verts[0] = stacktet->verts[0];
                outtet->verts[1] = stacktet->verts[1];
                outtet->verts[2] = stacktet->verts[2];
                outtet->verts[3] = stacktet->verts[3];
                outtet->quality = stacktet->quality;
            }
            continue;
        }
        
        /* insertion succeeded. dig out surrounding cavity, but don't allow vertex deletion */
        optimalcavity(mesh, vnew,
                      newtets, numnewtets, 
                      newfaces, numnewfaces, 
                      newtets, &numnewtets,
                      &worstdeletedqual,
                      &origcavityqual, false);
        
        assert(origcavityqual > 0.0);
        assert(worstdeletedqual > 0.0);
        
        /* improve the quality of tetrahedra in the cavity */
        cavityqual = improvecavity(mesh, vnew, newtets, numnewtets, false, NULL, &shortest, &longest);
        
        if (cavityqual < 0.0)
        {
            printf("improvecavity returned with negative quality %g\n", cavityqual);
        }
        
        /* ensure some minimum quality for the cavity */
        if (cavityqual < MINSIZEQUAL || shortest < (improvebehave.targetedgelength * improvebehave.shorterfactor))
        {
            if (improvebehave.verbosity > 5)
            {
                printf("Edge insertion for size control failed. Cavity qual %g, needed %g, shortest edge %g\n", cavityqual, MINSIZEQUAL, shortest);
            }
            invertjournalupto(mesh, beforeid);
            outtet = (struct improvetet *) stackpush(&outstack);
            outtet->verts[0] = stacktet->verts[0];
            outtet->verts[1] = stacktet->verts[1];
            outtet->verts[2] = stacktet->verts[2];
            outtet->verts[3] = stacktet->verts[3];
            outtet->quality = stacktet->quality;
            continue;
        }
        
        /* we successfully split the edge! */
        if (improvebehave.verbosity > 5)
        {
            printf("Succeeded in splitting too-long edge. min qual is %g\n", minquality);
        }
        
        if (SIZESPLITPARANOID)
        {
            /* compute global worst quality in isotropic space */
            meshquality(mesh, improvebehave.qualmeasure, means, &meshworst);
            if (improvebehave.anisotropic)
            {
                improvebehave.anisotropic = false;
                meshquality(mesh, improvebehave.qualmeasure, means, &physworst);
                improvebehave.anisotropic = true;
            }
            printf("after successful sizing insertion\n");
            printf("    Worst quality, isotropic = %g\n", meshworst);
            printf("    Worst quality, physical = %g\n", physworst);
            assert(meshworst > 0.0);
            assert(physworst > 0.0);
        }
    }
    
    /* copy the output stack over the input stack */
    copystack(&outstack, tetstack);
    /* free the local output stack */
    stackdeinit(&outstack);
}

/* split and collapse edges until all tetrahedra are roughly the same size */
int sizecontrol(struct tetcomplex *mesh,
                 struct behavior *behave,
                 struct inputs *in,
                 struct proxipool *vertexpool,
                 int argc,
                 char **argv)
{
    starreal minvol = HUGEFLOAT;
    starreal maxvol = 0.0;
    starreal meanvol = 0.0;
    starreal minedge = HUGEFLOAT;
    starreal maxedge = 0.0;
    starreal meanedge = 0.0;
    starreal medianedge = 0.0;
    starreal oldminedge, oldmaxedge;
    starreal shortestgoal, longestgoal;
    int numiters = 0;
    struct arraypoolstack longstack;    /* stack of tets with edges too long */
    struct arraypoolstack shortstack;   /* stack of tets with edges too short */
    struct arraypoolstack meshstack;    /* stack of all mesh tets */
    int totaltets = 0;
    starreal percentbad;
    starreal worstqual;
    starreal physworst;
    starreal means[NUMMEANTHRESHOLDS];
    
    starreal minqualbefore, minqualafter;
    starreal meanqualbefore[NUMMEANTHRESHOLDS], meanqualafter[NUMMEANTHRESHOLDS];
    starreal bestmeans[NUMMEANTHRESHOLDS];
    int smoothkinds = 0;
    
    int passstartid;
        
    if (improvebehave.facetsmooth) smoothkinds |= SMOOTHFACETVERTICES;
    if (improvebehave.segmentsmooth) smoothkinds |= SMOOTHSEGMENTVERTICES;
    if (improvebehave.fixedsmooth) smoothkinds |= SMOOTHFIXEDVERTICES;
    
    stackinit(&longstack, sizeof(struct improvetet));
    stackinit(&shortstack, sizeof(struct improvetet));
    stackinit(&meshstack, sizeof(struct improvetet));
    
    if (improvebehave.verbosity > 0)
    {
        printf("Starting size control...\n");
    }
    
    /* compute the min, max, average volume of tetrahedra in this mesh */
    volumestats(mesh, &minvol, &maxvol, &meanvol);
    
    if (improvebehave.verbosity > 1)
    {
        printf("Volume statistics:\n");
        printf("    Maximum: %g\n", maxvol);
        printf("    Minimum: %g\n", minvol);
        printf("    Average: %g\n", meanvol);
    }
    
    /* if the ideal edge length is set to zero, assign to it the value of the mean edge length */
    if (improvebehave.targetedgelength == 0.0)
    {
        /* compute physical edge length */
        edgelengthstats(mesh, &minedge, &maxedge, &meanedge, &medianedge, false);
        
        if (improvebehave.verbosity > 1)
        {
            printf("Setting ideal edge to physical median of %g\n", medianedge);
            printf("PHYSICAL SPACE Edge length statistics:\n");
            printf("    Max edge length: %g\n", maxedge);
            printf("    Min edge length: %g\n", minedge);
            printf("    Mean edge length: %g\n", meanedge);
            printf("    Median edge length: %g\n", medianedge);
        }
        
        improvebehave.targetedgelength = medianedge;
    }
    
    /* stop here unless explicit sizing pass is enabled */
    if (!improvebehave.sizingpass) return 3;
    
    /* compute anisotropic edge length */
    edgelengthstats(mesh, &minedge, &maxedge, &meanedge, &medianedge, true);
    
    longestgoal = improvebehave.targetedgelength * improvebehave.longerfactor;
    shortestgoal = improvebehave.targetedgelength * improvebehave.shorterfactor;
    
    if (improvebehave.verbosity > 1)
    {
        printf("Edge length statistics:\n");
        printf("    Max edge length: %g\n", maxedge);
        printf("    Min edge length: %g\n", minedge);
        printf("    Mean edge length: %g\n", meanedge);
        printf("    Median edge length: %g\n", medianedge);

        printf("Ideal edge length: %g\n", improvebehave.targetedgelength);
        printf("Longest allowable edge length: %g\n", longestgoal);
        printf("Shortest allowable edge length: %g\n", shortestgoal);
    }
    
    /* while some tetrahedra have edges that are too long or short, split or collapse them */
    while (
              ((minedge < shortestgoal) || (maxedge > longestgoal)) &&
              (numiters < MAXSIZEITERS)
          )
    /* while (false) */
    {
        passstartid = lastjournalentry(); 
        
        /* build stacks of tets with some edge too long or too short */
        filledgestacks(mesh, &longstack, &shortstack,
                       shortestgoal * CONTROLSHORTFAC,
                       longestgoal * CONTROLLONGFAC,
                       &minedge, &maxedge, &meanedge);
        
        /* if there aren't too many outside the bounds, bail */
        totaltets = counttets(mesh);
        percentbad = ((starreal) (longstack.top + shortstack.top)) / ((starreal) (totaltets));
        
        /* compute global worst quality in isotropic space */
        meshquality(mesh, improvebehave.qualmeasure, means, &worstqual);
        if (improvebehave.anisotropic)
        {
            improvebehave.anisotropic = false;
            meshquality(mesh, improvebehave.qualmeasure, means, &physworst);
            improvebehave.anisotropic = true;
        }
        
        if (improvebehave.verbosity > 2)
        {
            printf("Before iteration %d\n", numiters);
            printf("    Longest edge:          %g\n", maxedge);
            printf("    Longest desired edge:  %g\n", longestgoal);
            printf("    Shortest edge:         %g\n", minedge);
            printf("    Shortest desired edge: %g\n", shortestgoal);
            printf("    Mean edge:             %g\n", meanedge);
            printf("    Ideal edge:            %g\n", improvebehave.targetedgelength);
            printf("    %% Outside range:       %g\n", percentbad * 100.0);
            printf("    Worst qual (iso)       %g\n", worstqual);
            printf("    Worst qual (phys)      %g\n", physworst);
        }
        
        assert(worstqual > 0.0);
        assert(physworst > 0.0);
        
        /* stop if most are in range? */
        if (percentbad < 0.2)
        {
            break;
        }
        
        /* contract too-short edges */
        sizecontract(mesh, &shortstack);
        
        if (improvebehave.verbosity > 0)
        {
            printf("Short stack came back with %ld tets remaining\n", shortstack.top + 1 );
        }
        
        /* compute global worst quality in isotropic space */
        meshquality(mesh, improvebehave.qualmeasure, means, &worstqual);
        if (improvebehave.anisotropic)
        {
            improvebehave.anisotropic = false;
            meshquality(mesh, improvebehave.qualmeasure, means, &physworst);
            improvebehave.anisotropic = true;
        }
        printf("AFTER size contraction\n");
        printf("    Worst quality, isotropic = %g\n", worstqual);
        printf("    Worst quality, physical = %g\n", physworst);
        assert(worstqual > 0.0);
        assert(physworst > 0.0);
        
        /* split too-long edges */
        sizesplit(mesh, &longstack);
        
        if (improvebehave.verbosity > 0)
        {
            printf("Long stack came back with %ld tets remaining\n", longstack.top + 1 );
        }
        
        /* compute global worst quality in isotropic space */
        meshquality(mesh, improvebehave.qualmeasure, means, &worstqual);
        if (improvebehave.anisotropic)
        {
            improvebehave.anisotropic = false;
            meshquality(mesh, improvebehave.qualmeasure, means, &physworst);
            improvebehave.anisotropic = true;
        }
        printf("AFTER size insertion\n");
        printf("    Worst quality, isotropic = %g\n", worstqual);
        printf("    Worst quality, physical = %g\n", physworst);
        assert(worstqual > 0.0);
        assert(physworst > 0.0);
        
        oldminedge = minedge;
        oldmaxedge = maxedge;
        /* compute the same for edge length */
        edgelengthstats(mesh, &minedge, &maxedge, &meanedge, &medianedge, true);
        
        /* if there has been no reduction in extreme edge length, try a smooth + topo */
        if ((minedge <= oldminedge && maxedge >= oldmaxedge) || 1)
        {
            if (improvebehave.verbosity > 0)
            {
                printf("Performing global smoothing pass...\n");
            }
            
            /* create a stack with every tet */
            fillstackqual(mesh, &meshstack, improvebehave.qualmeasure, HUGEFLOAT, meanqualbefore, &minqualbefore);
        
            /* perform optimization smoothing pass */
            smoothpass(mesh, 
                       &meshstack, 
                       NULL, NULL, 
                       improvebehave.qualmeasure, 
                       HUGEFLOAT, 
                       bestmeans, 
                       meanqualafter, 
                       &minqualafter, 
                       smoothkinds, 
                       false);
                       
            /* create a stack with every tet */
            fillstackqual(mesh, &meshstack, improvebehave.qualmeasure, HUGEFLOAT, meanqualbefore, &minqualbefore);
            
            /* perform topological improvement pass */
            topopass(mesh, 
                     &meshstack, NULL, 
                     improvebehave.qualmeasure,
                     bestmeans, 
                     meanqualafter, 
                     &minqualafter, 
                     false);
        }
        
        /* output the mesh after this pass, if animating */
        if (improvebehave.animate)
        {
            outputqualmesh(behave, in, vertexpool, mesh, argc, argv, numiters+3, SIZECONTROLPASS, passstartid, QUALMINSINE);
        }
        
        numiters++;
    }
    
    if (improvebehave.verbosity > 0)
    {
        if (numiters < MAXSIZEITERS)
        {
            textcolor(BRIGHT, GREEN, BLACK);
            printf("Size control succeeded!\n");
            textcolor(RESET, WHITE, BLACK);
        }
        else
        {
            textcolor(BRIGHT, RED, BLACK);
            printf("Short control failed, %ld tets with too-long, %ld tets with too-short edges.\n", longstack.top+1, shortstack.top+1);
            textcolor(RESET, WHITE, BLACK);
        }
    }
    
    /* compute the same for edge length */
    edgelengthstats(mesh, &minedge, &maxedge, &meanedge, &medianedge, true);
    
    if (improvebehave.verbosity > 0)
    {
        printf("Edge length statistics after size control:\n");
        printf("    Longest edge:          %g\n", maxedge);
        printf("    Longest desired edge:  %g\n", longestgoal);
        printf("    Shortest edge:         %g\n", minedge);
        printf("    Shortest desired edge: %g\n", shortestgoal);
        printf("    Mean edge:             %g\n", meanedge);
        printf("    Median edge:           %g\n", medianedge);
        printf("    Ideal edge:            %g\n", improvebehave.targetedgelength);
    }
    
    /* outputqualmesh(behave, in, vertexpool, mesh, argc, argv, 0, SIZECONTROLPASS, 0, QUALMINSINE); */
    
    /* clean up local stacks */
    stackdeinit(&longstack);
    stackdeinit(&shortstack);
    stackdeinit(&meshstack);
    
    /* starexit(1); */
    
    return numiters+3;
}

void sizereportstream(FILE *o, struct tetcomplex *mesh)
{
    starreal minedge, maxedge, meanedge, medianedge;
    
    edgelengthstats(mesh, &minedge, &maxedge, &meanedge, &medianedge, true);
    
    fprintf(o, "Edge length statistics:\n");
    fprintf(o, "    Longest edge:          %g\n", maxedge);
    fprintf(o, "    Longest desired edge:  %g\n", improvebehave.targetedgelength * improvebehave.longerfactor);
    fprintf(o, "    Shortest edge:         %g\n", minedge);
    fprintf(o, "    Shortest desired edge: %g\n", improvebehave.targetedgelength * improvebehave.shorterfactor);
    fprintf(o, "    Mean edge:             %g\n", meanedge);
    fprintf(o, "    Median edge:           %g\n", medianedge);
    fprintf(o, "    Ideal edge:            %g\n", improvebehave.targetedgelength);
}

/* print a report on edge lengths in the mesh */
void sizereport(struct tetcomplex *mesh)
{
    sizereportstream(stdout, mesh);
}

