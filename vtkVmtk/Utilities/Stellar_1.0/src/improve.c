/*****************************************************************************/
/*                                                                           */
/*  driver functions to direct mesh improvement                              */
/*                                                                           */
/*****************************************************************************/

/* see if new means contains any better means. if so,
   update best means and return true */
bool meanimprove(starreal bestmeans[],
                 starreal newmeans[],
                 int passtype)
{
    int i;
    bool foundbetter = false;
    starreal minimprovement = improvebehave.minstepimprovement;
    
    if (passtype == INSERTPASS)
    {
        minimprovement = improvebehave.mininsertionimprovement;
    }
    if (passtype == DESPERATEPASS)
    {
        minimprovement = improvebehave.mininsertionimprovement;
    }
    
    for (i=0; i<NUMMEANTHRESHOLDS; i++)
    {
        if (newmeans[i] > bestmeans[i])
        {
            /* see if it beats it by the required threshold */
            if (newmeans[i] - bestmeans[i] > minimprovement)
            {
                if (improvebehave.verbosity > 4)
                {
                    textcolor(BRIGHT, GREEN, BLACK);
                    printf("mean improvement = %g, that's enough for success (needs %g)\n", newmeans[i] - bestmeans[i], minimprovement);
                    textcolor(RESET, WHITE, BLACK);
                }
                foundbetter = true;
            }
            else
            {
                if (improvebehave.verbosity > 4)
                {
                    printf("mean improvement = %g, positive but not enough (needs %g)!\n", newmeans[i] - bestmeans[i], minimprovement);
                }
            }
            
            bestmeans[i] = newmeans[i];
        }
    }
    
    if (improvebehave.verbosity > 4)
    {
        printf("overall, mean improvement success = %d\n", foundbetter);
    }
    
    return foundbetter;
}

/* run a pass (smoothing, topo, insertion). return true
   if we have reached the desired quality */
bool pass(int passtype,
          struct tetcomplex* mesh,
          struct arraypoolstack* tetstack,
          starreal threshold,
          bool *minsuccess,
          bool *meansuccess,
          int passnum,
          starreal bestmeans[],
          struct behavior *behave,
          struct inputs *in,
          struct proxipool *vertexpool,
          int argc,
          char **argv)
{
    /* quality vars */
    starreal minqualbefore, minqualafter;
    starreal meanqualbefore[NUMMEANTHRESHOLDS], meanqualafter[NUMMEANTHRESHOLDS];
    starreal minedge, maxedge, meanedge;
    
#ifndef NO_TIMER
    /* timing vars */
    struct timeval tv1, tv2;
    struct timezone tz;
#endif /* not NO_TIMER */
    
    /* smoothing vars */
    int smoothkinds = 0;
    
    /* sine of target angles */
    starreal goalangleminsine = degtosin(improvebehave.goalanglemin);
    starreal goalanglemaxsine = degtosin(improvebehave.goalanglemax);
    starreal biggestangle;
    starreal smallestangle;
    
    bool desperate = false;
    int passstartid = 0;
    
    if (improvebehave.facetsmooth) smoothkinds |= SMOOTHFACETVERTICES;
    if (improvebehave.segmentsmooth) smoothkinds |= SMOOTHSEGMENTVERTICES;
    if (improvebehave.fixedsmooth) smoothkinds |= SMOOTHFIXEDVERTICES;
    
    assert(passtype == SMOOTHPASS ||
           passtype == TOPOPASS ||
           passtype == CONTRACTPASS ||
           passtype == CONTRACTALLPASS ||
           passtype == INSERTPASS ||
           passtype == DESPERATEPASS);
    
    if (improvebehave.verbosity > 1)
    {
        textcolor(BRIGHT, MAGENTA, BLACK);
        printf("\nPerforming improvement pass %d", passnum);
        switch (passtype)
        {
            case SMOOTHPASS:
                printf(" (smoothing)\n");
                break;
            case TOPOPASS:
                printf(" (topological)\n");
                break;
            case CONTRACTALLPASS:
            case CONTRACTPASS:
                printf(" (edge contraction)\n");
                break;
            case INSERTPASS:
            case DESPERATEPASS:
                printf(" (insertion)\n");
                break;
            default:
                printf("whoa, I know what kind of pass %d is\n", passtype);
                starexit(1);
        }
        textcolor(RESET, WHITE, BLACK);
    }
           
    /* start out by appropriately filling the stack */
    if (passtype != CONTRACTALLPASS)
    {
        fillstackqual(mesh, tetstack, improvebehave.qualmeasure, threshold, meanqualbefore, &minqualbefore);
    }
    else
    {
        /* in the case of contraction, just get one tet for each edge */
        filledgestacks(mesh, tetstack, NULL, HUGEFLOAT, 0.0, &minedge, &maxedge, &meanedge);
    }
    
    /* get global worst quality; this is what must improve */
    meshquality(mesh, improvebehave.qualmeasure, meanqualbefore, &minqualbefore);
    
    /* if this is the first pass, and we're animating, output original mesh */
    if (improvebehave.animate && passnum == 1)
    {
        outputqualmesh(behave, in, vertexpool, mesh, argc, argv, 0, passtype, 0, QUALMINSINE);
    }
    
    /* capture animation info */
    if (improvebehave.animate)
    {
        passstartid = lastjournalentry(); 
    }
    
    /* save timer before pass */
#ifndef NO_TIMER
    gettimeofday(&tv1, &tz);
#endif /* not NO_TIMER */    

    /* run the actual pass */
    switch (passtype)
    {
        case SMOOTHPASS:
            smoothpass(mesh, 
                       tetstack, 
                       NULL, NULL, 
                       improvebehave.qualmeasure, 
                       threshold, 
                       bestmeans, 
                       meanqualafter, 
                       &minqualafter, 
                       smoothkinds, 
                       false);
#ifndef NO_TIMER
            gettimeofday(&tv2, &tz);
            stats.smoothmsec += msecelapsed(tv1, tv2);
#endif /* not NO_TIMER */
            break;
        case TOPOPASS:
            topopass(mesh, 
                     tetstack, 
                     NULL, 
                     improvebehave.qualmeasure, 
                     bestmeans, 
                     meanqualafter, 
                     &minqualafter, 
                     false);
#ifndef NO_TIMER
            gettimeofday(&tv2, &tz);
            stats.topomsec += msecelapsed(tv1, tv2);
#endif /* not NO_TIMER */
            break;
        case CONTRACTPASS:
            contractworst(mesh, 
                          improvebehave.qualmeasure,
                          improvebehave.insertthreshold, 
                          bestmeans, 
                          meanqualafter, 
                          &minqualafter, 
                          true);
#ifndef NO_TIMER
            gettimeofday(&tv2, &tz);
            stats.contractmsec += msecelapsed(tv1, tv2);
#endif /* not NO_TIMER */
            break;
        case CONTRACTALLPASS:
            contractpass(mesh, 
                         tetstack,
                         NULL,
                         improvebehave.qualmeasure, 
                         bestmeans, 
                         meanqualafter, 
                         &minqualafter, 
                         true,
                         false);
#ifndef NO_TIMER
            gettimeofday(&tv2, &tz);
            stats.contractmsec += msecelapsed(tv1, tv2);
#endif /* not NO_TIMER */
            break;
        case DESPERATEPASS:
            desperate = true;
        case INSERTPASS:
            worsttetattack(mesh, 
                           improvebehave.qualmeasure, 
                           improvebehave.insertthreshold, 
                           meanqualafter, 
                           &minqualafter, 
                           desperate);
#ifndef NO_TIMER
            gettimeofday(&tv2, &tz);
            stats.insertmsec += msecelapsed(tv1, tv2);
# endif /* not NO_TIMER */
            if (desperate)
            {
                *meansuccess = meanimprove(bestmeans, meanqualafter, DESPERATEPASS);
            }
            else
            {
                *meansuccess = meanimprove(bestmeans, meanqualafter, INSERTPASS);
            }
            break;
        default:
            printf("i don't know how to run pass type %d, dying\n", passtype);
            starexit(1);
    }
    
    /* check for success */
    *meansuccess = meanimprove(bestmeans, meanqualafter, passtype);
    if (minqualafter - minqualbefore < MINMINIMPROVEMENT)
    {
        *minsuccess = false;
    }
    else
    {
        *minsuccess = true;
    }
    
    /* output the mesh after this pass, if animating */
    if (improvebehave.animate)
    {
        outputqualmesh(behave, in, vertexpool, mesh, argc, argv, passnum, passtype, passstartid, QUALMINSINE);
    }
    
    /* check whether we have reached the goal quality for minimum or maximum angle */
    if (minqualafter > goalangleminsine || minqualafter > goalanglemaxsine)
    {
        /* compute the extreme angles */
        getextremeangles(behave, mesh, &smallestangle, &biggestangle);
        
        /* we must have reached one of these angles */
        /* not necessarily true for non-angle based quality measures */
        /* assert(smallestangle > improvebehave.goalanglemin || biggestangle < improvebehave.goalanglemax); */
        
        if (improvebehave.verbosity > 3)
        {
            if (smallestangle > improvebehave.goalanglemin)
            {
                printf("Smallest angle %g degrees in ABOVE goal angle of %g degrees\n", smallestangle, improvebehave.goalanglemin);
            }
            else
            {
                printf("Smallest angle %g degrees is BELOW goal angle of %g degrees.\n", smallestangle, improvebehave.goalanglemin);
            }
        
            if (biggestangle < improvebehave.goalanglemax)
            {
                printf("Largest angle %g degrees is BELOW goal angle of %g degrees.\n", biggestangle, improvebehave.goalanglemax);
            }
            else
            {
                printf("Largest angle %g degrees is ABOVE goal angle of %g degrees.\n", biggestangle, improvebehave.goalanglemax);
            }
        }
        
        /* if we've reached both thresholds, let the main loop know we're done */
        if (smallestangle > improvebehave.goalanglemin && biggestangle < improvebehave.goalanglemax)
        {
            if (improvebehave.verbosity > 1)
            {
                textcolor(BRIGHT, GREEN, BLACK);
                printf("Both min and max goal angles reached, stopping improvement.\n");
                textcolor(RESET, WHITE, BLACK);
            }
            return true;
        }
    }
    
    return false;
}

/* pre-improvement initialization code */
void improveinit(struct tetcomplex *mesh,
                 struct proxipool *vertexpool,
                 struct arraypoolstack *tetstack,
                 struct behavior *behave,
                 struct inputs *in,
                 int argc, 
                 char **argv,
                 starreal bestmeans[NUMMEANTHRESHOLDS])
{
    int consistent;
    starreal minqualbefore;
    starreal meanqualbefore[NUMMEANTHRESHOLDS];
    starreal worstin;
    starreal worstinitqual;
    int i;
    
    for (i=0; i<NUMMEANTHRESHOLDS; i++)
    {
        bestmeans[i] = 0.0;
    }
    
    /* assure that the mesh is consistent at the outset */
    consistent = mytetcomplexconsistency(mesh);
    assert(consistent);
    
    stats.startnumverts = countverts(vertexpool);
    stats.startnumtets = counttets(mesh);
    
    /* upon request, just spit out mesh and quit */
    if (improvebehave.outputandquit)
    {
        printf("Outputting mesh and quitting.\n");
        outputqualmesh(behave, in, vertexpool, mesh, argc, argv, 0, SMOOTHPASS, 0, improvebehave.qualmeasure);
        starexit(1);
    }
    
    if (improvebehave.verbosity > 0)
    {
        printf("Improving mesh.\n");
    }
    
    /* this array pool stores information on vertices */
    arraypoolinit(&vertexinfo, sizeof(struct vertextype), LOG2TETSPERSTACKBLOCK, 0);
    
    /* stack of tets to be improved */
    stackinit(tetstack, sizeof(struct improvetet));
    
    /* this stack stores a journal of improvement steps */
    journal = &journalstack;
    stackinit(journal, sizeof(struct journalentry));
    
    /* compute bounding box for anisotropy */
    setboundingbox(mesh);
    
    if (improvebehave.verbosity > 1)
    {
        /* print improvement configuration */
        printimproveoptions(&improvebehave);
        /* print initial mesh statistics before improvement */
        printf("Mesh quality before improvement:\n");
        improvestatistics(behave, mesh, false);
        if (improvebehave.anisotropic)
        {
            printf("Mesh quality before improvement (ISOTROPIC SPACE):\n");
            improvestatistics(behave, mesh, true);
        }
    }
    
    /* print out the worst input angle */
    worstin = worstinputangle(mesh);
    if (improvebehave.verbosity > 2)
    {
        printf("The worst input angle is %g radians (%g degrees).\n", worstin, worstin * (180.0 / PI));
    }
    
    /* classify degrees of freedom of all vertices */
    if (improvebehave.verbosity > 2)
    {
        printf("Performing vertex classification.\n");
    }
    classifyvertices(mesh);
    
    /* compute surface quadric information */
    if (improvebehave.verbosity > 2)
    {
        printf("Computing intial surface quadrics\n");
    }
    collectquadrics(mesh);
    if (improvebehave.verbosity > 2)
    {
        checkquadrics(mesh);
    }
    
    /* make sure that all the tets are right-side out before we start */
    worstinitqual = worstquality(mesh);
    if (worstinitqual <= 0.0)
    {
        textcolor(BRIGHT, RED, BLACK);
        printf("***** ALERT Input mesh has non-positive worst quality of %g, dying *****\n", worstinitqual);
        textcolor(RESET, WHITE, BLACK);
    }
    if (improvebehave.dynimprove)
    {
        assert(worstinitqual > 0.0);
    }
    
    /* build stack for initial quality evaluation */
    fillstackqual(mesh, tetstack, improvebehave.qualmeasure, HUGEFLOAT, meanqualbefore, &minqualbefore);
    meanimprove(bestmeans, meanqualbefore, SMOOTHPASS);
    
    /* set initial minimum and thresholded mean qualities */
    for (i=0; i<NUMMEANTHRESHOLDS; i++)
    {
        stats.startmeanquals[i] = meanqualbefore[i];
    }
    stats.startworstqual = minqualbefore;
    setbeginqual(behave, mesh);
}

/* clean up after mesh improvement */
void improvedeinit(struct tetcomplex *mesh,
                   struct proxipool *vertexpool,
                   struct arraypoolstack *tetstack,
                   struct behavior *behave,
                   struct inputs *in,
                   int argc, 
                   char **argv)
{
    int consistent;
    starreal minqualbefore;
    starreal meanqualbefore[NUMMEANTHRESHOLDS];
    starreal worstin;
    int i;
    
    /* check final mesh consistency */
    consistent = mytetcomplexconsistency(mesh);
    if (improvebehave.verbosity > 3)
    {
        printf("\nin the end mesh consistent = %d\n", consistent);
    }
    assert(consistent);
    
    if (IMPROVEPARANOID)
    {
        /* check that the final worst boundary dihedral matches start */
        printf("The worst boundary angle at the start: %g radians (%g degrees).\n", worstin, worstin * (180.0 / PI));
        worstin = worstinputangle(mesh);
        printf("The worst boundary angle at the end: %g radians (%g degrees).\n", worstin, worstin * (180.0 / PI));
    }
    
    if (improvebehave.verbosity > 3)
    {
        /* print out the properties of the worst tetrahedra */
        worsttetreport(mesh, improvebehave.qualmeasure, 5.0);
    }
    
    /* record final qualities */
    fillstackqual(mesh, tetstack, improvebehave.qualmeasure, HUGEFLOAT, meanqualbefore, &minqualbefore);
    for (i=0; i<NUMMEANTHRESHOLDS; i++)
    {
        stats.finishmeanquals[i] = meanqualbefore[i];
    }
    
    stats.finishnumverts = countverts(vertexpool);
    stats.finishnumtets = counttets(mesh);
    
    /* print final mesh improvement statistics */
    if (improvebehave.verbosity > 2)
    {
        printstats(mesh);
        checkquadrics(mesh);
    }
    
    if (improvebehave.verbosity > 1)
    {
        /* print mesh quality statistics after improvement has been done */
        printf("\nHere are quality statistics on the improved mesh:\n");
        improvestatistics(behave, mesh, false);
        
        if (improvebehave.anisotropic)
        {
            printf("Quality stats (ISOTROPIC SPACE):\n");
            improvestatistics(behave, mesh, true);
        }
    }
    
    /* print final mesh out. If we're animating we've already got it. */
    if (improvebehave.animate == false)
    {
        outputqualmesh(behave, in, vertexpool, mesh, argc, argv, 0, 0, 0, QUALMINSINE);
    }
    
    /* clean up array pools */
    stackdeinit(tetstack);
    arraypooldeinit(&vertexinfo);
    stackdeinit(journal);
}

/* top-level function to perform static mesh improvement */
void staticimprove(struct behavior *behave,
                   struct inputs *in,
                   struct proxipool *vertexpool,
                   struct tetcomplex *mesh,
                   int argc,
                   char **argv)
{
    struct arraypoolstack tetstack;         /* stack of tets to be improved */
    int passnum = 1;                        /* current improvement pass */
    int roundsnoimprovement = 0;            /* number of passes since mesh improved */
    bool meansuccess = false;               /* thresholded mean success */
    bool minsuccess = false;                /* minimum quality success */
    starreal bestmeans[NUMMEANTHRESHOLDS];  /* current best thresholded means */
    bool stop, stop1 = false;               /* whether to continue improvement */
    int numdesperate = 0;
    
#ifndef NO_TIMER
    /* timing vars */
    struct timeval tv0, tv2;
    struct timezone tz;
    /* get initial time */
    gettimeofday(&tv0, &tz);
    stats.starttime = tv0;
#endif /* not NO_TIMER */
    
    /* perform improvement initialization */
    improveinit(mesh, vertexpool, &tetstack, behave, in, argc, argv, bestmeans);
    
    if (improvebehave.verbosity > 1) sizereport(mesh);
    
    /********** INITIAL SMOOTHING AND TOPO PASSES **********/
    
    /* initial global smoothing pass */
    stop = pass(SMOOTHPASS, mesh, &tetstack, 
                HUGEFLOAT, &minsuccess, &meansuccess, passnum++, bestmeans, 
                behave, in, vertexpool, argc, argv);
    /* initial global topological improvement pass */
    stop = pass(TOPOPASS, mesh, &tetstack,
                HUGEFLOAT, &minsuccess, &meansuccess, passnum++, bestmeans,
                behave, in, vertexpool, argc, argv);
    /* initial global contraction improvement pass */
    if (improvebehave.anisotropic == false)
    {
        stop = pass(CONTRACTALLPASS, mesh, &tetstack,
                    HUGEFLOAT, &minsuccess, &meansuccess, passnum++, bestmeans,
                    behave, in, vertexpool, argc, argv);
    }
    
    /***************** SIZE CONTROL *************************/
    if (improvebehave.sizing)
    {
        passnum = sizecontrol(mesh, behave, in, vertexpool, argc, argv);
    }
    
    /*************** MAIN IMPROVEMENT LOOP ******************/
    while (roundsnoimprovement < STATICMAXPASSES)
    {
        /* if the mesh is already fine, stop improvement */
        if (stop) break;
        
        /* perform a smoothing pass */
        stop = pass(SMOOTHPASS, mesh, &tetstack, 
                    HUGEFLOAT, &minsuccess, &meansuccess, passnum++, bestmeans,
                    behave, in, vertexpool, argc, argv);
        if (stop) break;
        
        /* if the smoothing pass failed to sufficiently improve the mesh */
        if ((minsuccess == false) && (meansuccess == false))
        {
            /* perform a global topological pass */
            stop = pass(TOPOPASS, mesh, &tetstack,
                        HUGEFLOAT, &minsuccess, &meansuccess, passnum++, bestmeans, 
                        behave, in, vertexpool, argc, argv);
            if (stop) break;
            
            /* if the topo pass also failed */
            if ((minsuccess == false) && (meansuccess == false))
            {
                /* perform a contraction and insertion pass */
                if (improvebehave.enableinsert)
                {
                    /* potentially start with a pass of edge contraction */
                    if (improvebehave.edgecontraction)
                    {
                        stop1 = pass(CONTRACTPASS, mesh, &tetstack, 
                                    HUGEFLOAT, &minsuccess, &meansuccess, passnum++, bestmeans, 
                                    behave, in, vertexpool, argc, argv);
                    }
                    else
                    {
                        stop1 = false;
                    }
                    
                    if (roundsnoimprovement == 1 && numdesperate < DESPERATEMAXPASSES)
                    {
                        stop = pass(DESPERATEPASS, mesh, &tetstack, 
                                    HUGEFLOAT, &minsuccess, &meansuccess, passnum++, bestmeans, 
                                    behave, in, vertexpool, argc, argv);
                        numdesperate++;
                        if (improvebehave.verbosity > 2)
                        {
                            printf("Just completed desperate pass %d / %d\n", numdesperate, DESPERATEMAXPASSES);
                        }
                        if (stop || stop1) break;
                    }
                    else
                    {
                        stop = pass(INSERTPASS, mesh, &tetstack, 
                                    HUGEFLOAT, &minsuccess, &meansuccess, passnum++, bestmeans, 
                                    behave, in, vertexpool, argc, argv);
                        if (stop || stop1) break;
                    }
                }
            }
        }
        /* if this pass failed to see any improvement, note it */
        if ((minsuccess == false) && (meansuccess == false))
        {
            roundsnoimprovement++;
            if (improvebehave.verbosity > 2)
            {
                textcolor(BRIGHT, RED, BLACK);
                printf("Last %d passes there has been no improvement.\n", roundsnoimprovement);
                textcolor(RESET, WHITE, BLACK);
            }
        }
        /* reset number of rounds on smoothing success */
        else 
        {
            if (improvebehave.verbosity > 2)
            {
                textcolor(BRIGHT, GREEN, BLACK);
                printf("Resetting failed passes count on success.\n");
                textcolor(RESET, WHITE, BLACK);
            }
            roundsnoimprovement = 0;
        }
    }
    
    /******************** END MAIN IMPROVEMENT LOOP **********************/
    
#ifndef NO_TIMER
    /* record total time */
    gettimeofday(&tv2, &tz);
    stats.totalmsec = msecelapsed(tv0, tv2);
#endif /* not NO_TIMER */
    
    /* perform post-improvement cleanup */
    improvedeinit(mesh, vertexpool, &tetstack, behave, in, argc, argv);
}
