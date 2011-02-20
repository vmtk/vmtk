/*****************************************************************************/
/*                                                                           */
/*  printing utility functions                                               */
/*                                                                           */
/*****************************************************************************/

/* print the coordinates of the for vertices of a tet for viz purposes
   (prints them in the format of a MATLAB matrix, my ghetto viz tool) */
void printtetverts(struct tetcomplex *mesh, tag *tet)
{
    starreal *point[4];
    int i;
    
    /* get tet vertices */
    point[0] = ((struct vertex *) tetcomplextag2vertex(mesh, tet[0]))->coord;
    point[1] = ((struct vertex *) tetcomplextag2vertex(mesh, tet[1]))->coord;
    point[2] = ((struct vertex *) tetcomplextag2vertex(mesh, tet[2]))->coord;
    point[3] = ((struct vertex *) tetcomplextag2vertex(mesh, tet[3]))->coord;
    
    printf("[");
    for (i=0; i<4; i++)
    {
        printf("%f %f %f", point[i][0], point[i][1], point[i][2]);
        if (i != 3)
        {
            printf(";\n");
        }
    }
    printf("]");
}

/* print the geometric locations of face vertices */
void printfaceverts(struct tetcomplex *mesh, tag *face)
{
    starreal *point[4];
    int i;
    
    /* get tet vertices */
    point[0] = ((struct vertex *) tetcomplextag2vertex(mesh, face[0]))->coord;
    point[1] = ((struct vertex *) tetcomplextag2vertex(mesh, face[1]))->coord;
    point[2] = ((struct vertex *) tetcomplextag2vertex(mesh, face[2]))->coord;
    
    printf("[");
    for (i=0; i<3; i++)
    {
        printf("%f %f %f", point[i][0], point[i][1], point[i][2]);
        if (i != 3)
        {
            printf(";\n");
        }
    }
    printf("]");
}

/* another function that prints a tet with separate vertex args */
void printtetvertssep(struct tetcomplex *mesh,
                      tag vtx1,
                      tag vtx2,
                      tag vtx3,
                      tag vtx4)
{
    tag tet[4];
    tet[0] = vtx1;
    tet[1] = vtx2;
    tet[2] = vtx3;
    tet[3] = vtx4;
    
    printtetverts(mesh, tet);
}

/* print out an array of tets */
void printtets(struct tetcomplex *mesh,
               tag tets[][4],
               int numtets)
{
    int i; /* loop counter */
    
    printf("{");
    for (i=0; i<numtets; i++)
    {
        printtetverts(mesh, tets[i]);
        if (i != numtets-1)
        {
            printf(",\n");
        }
    }
    printf("};\n");
}

/* print out an array of tets */
void printopttets(struct tetcomplex *mesh,
                  struct opttet tets[],
                  int numtets)
{
    int i; /* loop counter */
    
    printf("{");
    for (i=0; i<numtets; i++)
    {
        printtetverts(mesh, tets[i].verts);
        if (i != numtets-1)
        {
            printf(",\n");
        }
    }
    printf("};\n");
}

/* print out an array of faces */
void printfaces(struct tetcomplex *mesh,
                tag faces[][3],
                int numfaces)
{
    int i; /* loop counter */
    
    printf("{");
    for (i=0; i<numfaces; i++)
    {
        printfaceverts(mesh, faces[i]);
        if (i != numfaces-1)
        {
            printf(",\n");
        }
    }
    printf("};\n");
}

/* print out an array of tets as tags*/
void printtetstags(tag tets[][4],
                   int numtets)
{
    int i; /* loop counter */
    
    printf("[");
    for (i=0; i<numtets; i++)
    {
        printf("%d %d %d %d;\n", (int) tets[i][0], (int) tets[i][1], (int) tets[i][2], (int) tets[i][3]);
    }
    printf("]\n");
}

/* print Q and K tables from Klincsek's algorithm for debugging
   purposes */
void printtables(starreal Q[][MAXRINGTETS],
                 int K[][MAXRINGTETS],
                 int ringcount)
{
    int i,j; /* loop counters */
    
    printf("*** Table Q ***\n");
    for (i=1; i<=ringcount; i++)
    {
        printf("[");
        for (j=1; j<=ringcount; j++)
        {
            printf("% f ", Q[i][j]);
        }
        printf("]\n");
    }
    
    printf("*** Table K ***\n");
    for (i=1; i<=ringcount; i++)
    {
        printf("[");
        for (j=1; j<=ringcount; j++)
        {
            printf("%2ul ", K[i][j]);
        }
        printf("]\n");
    }
}

/* print edge and ring around it for ghetto visualization in matlab */
void printring(struct tetcomplex *mesh, tag a, tag b, tag *ring, int ringcount)
{
    int i;
    tag tet[4];
    
    /* output every tet in the ring */
    printf("{\n");
    for (i=0; i<ringcount; i++)
    {
        tet[0] = ring[i];
        if (i == ringcount -1)
        {
            tet[1] = ring[0];
        }
        else
        {
            tet[1] = ring[i+1];
        }
        tet[2] = b;
        tet[3] = a;
        printtetverts(mesh, tet);
        if (i != ringcount -1)
        {
            printf(",\n");
        }
    }
    printf("}\n"); 
}

/* print edge and ring around it for ghetto visualization in matlab */
void printhalfring(struct tetcomplex *mesh, tag a, tag b, tag *ring, int ringcount)
{
    int i;
    tag tet[4];
    
    /* output every tet in the ring */
    printf("{");
    for (i=0; i<ringcount-1; i++)
    {
        tet[0] = ring[i];
        tet[1] = ring[i+1];
        tet[2] = b;
        tet[3] = a;
        /*
        printf("verts (%d %d %d %d)\n", tet[0], tet[1], tet[2], tet[3]);
        */
        printtetverts(mesh, tet);
        if (i != ringcount -2)
        {
            printf(",\n");
        }
    }
    printf("};\n"); 
}

/* print out the info in a opttet */
void printopttet(struct opttet *tet)
{
    int i,j;
    
    printf("For tet with verts (%d, %d, %d, %d):\n", (int) tet->verts[0], (int) tet->verts[1], (int) tet->verts[2], (int) tet->verts[3]);
    printf("    volume = %f\n", tet->volume);
    printf("    volume gradient = (%f, %f, %f)\n", tet->volumegrad[0], tet->volumegrad[1], tet->volumegrad[2]);
    printf("    edge lengths:\n");
    for (i=0; i<6; i++)
    {
        for (j = i + 1; j < 4; j++) 
        {            
            printf("        %d-%d: %f\n",i,j,tet->edgelength[i][j]);
        }
    }
    printf("    edge length graidents:\n");
    for (i=0; i<6; i++)
    {
        for (j = i + 1; j < 4; j++) 
        {
            printf("        %d-%d: %f %f %f\n",i,j,tet->edgegrad[i][j][0],tet->edgegrad[i][j][1],tet->edgegrad[i][j][2]);
        }
    }
    printf("    face areas:\n");
    for (i=0; i<4; i++)
    {
        printf("        %d: %f\n",i,tet->facearea[i]);
    }
    printf("    gradients of face areas:\n");
    for (i=0; i<4; i++)
    {
        printf("        %d: %f %f %f\n",i,tet->facegrad[i][0],tet->facegrad[i][1],tet->facegrad[i][2]);
    }
    printf("    sines of dihedral angles:\n");
    for (i=0; i<6; i++)
    {
        printf("        %d: %f\n",i,tet->sine[i]);
    }
    printf("    gradients of sines of dihedral angles:\n");
    for (i=0; i<6; i++)
    {
        printf("        %d: (%f, %f, %f)\n",i,tet->sinegrad[i][0],tet->sinegrad[i][1],tet->sinegrad[i][2]);
    }
}

/* print out B, S, M arrays from findbasis */
void printbasisarrays(starreal S[][3],
                      starreal M[][3],
                      starreal B[][3],
                      int sizeS,
                      int sizeM,
                      int *sizeB,
                      starreal p[])
{
    int i;
    
    printf("\nCurrent basis finding arrays:\n");
    printf("    p: %f %f %f\n",p[0],p[1],p[2]);
    printf("    S: [ ");
    for (i=0; i<sizeS; i++)
    {
        printf("(%f %f %f) ", S[i][0],S[i][1],S[i][2]);
    }
    printf("]\n");
    
    printf("    M: [ ");
    for (i=0; i<sizeM; i++)
    {
        printf("(%f %f %f) ", M[i][0],M[i][1],M[i][2]);
    }
    printf("]\n");
    
    printf("    B: [ ");
    for (i=0; i<*sizeB; i++)
    {
        printf("(%f %f %f) ", B[i][0],B[i][1],B[i][2]);
    }
    printf("]\n\n");
}

/* print out global improvement stats */
void printstatsstream(FILE *o, tetcomplex *mesh)
{
    int i, maxa, maxs, maxb, maxl;
    
#ifndef NO_TIMER
    struct timeval now;
    struct timezone tz;
#endif /* not NO_TIMER */

    fprintf(o, "Mesh Improvement Statistics:\n\n");
    
    /* smoothing */
    fprintf(o, "Smoothing statistics:\n");
    fprintf(o, "    Optimization smooths:    %d / %d\n", stats.nonsmoothsuccesses, stats.nonsmoothattempts);
    fprintf(o, "    Free vertex smooths:     %d / %d\n", stats.freesmoothsuccesses, stats.freesmoothattempts);
    fprintf(o, "    Facet vertex smooths:    %d / %d\n", stats.facetsmoothsuccesses, stats.facetsmoothattempts);
    fprintf(o, "    Segment vertex smooths:  %d / %d\n", stats.segmentsmoothsuccesses, stats.segmentsmoothattempts);
    fprintf(o, "    Fixed vertex smooths:    %d / %d\n", stats.fixedsmoothsuccesses, stats.fixedsmoothattempts);
    
    /* topological */
    fprintf(o, "\nTopological improvement statistics:\n");
    fprintf(o, "    Edge contractions:       %d / %d\n", stats.edgecontractions, stats.edgecontractionattempts);
    fprintf(o, "    Edge contraction cases:\n");
    fprintf(o, "        free-free            %d / %d\n", stats.edgecontractcasesuc[FREEFREEEDGE], stats.edgecontractcaseatt[FREEFREEEDGE]);
    fprintf(o, "        free-facet           %d / %d\n", stats.edgecontractcasesuc[FREEFACETEDGE], stats.edgecontractcaseatt[FREEFACETEDGE]);
    fprintf(o, "        free-segment         %d / %d\n", stats.edgecontractcasesuc[FREESEGMENTEDGE], stats.edgecontractcaseatt[FREESEGMENTEDGE]);
    fprintf(o, "        free-fixed           %d / %d\n", stats.edgecontractcasesuc[FREEFIXEDEDGE], stats.edgecontractcaseatt[FREEFIXEDEDGE]);
    fprintf(o, "        facet-facet          %d / %d\n", stats.edgecontractcasesuc[FACETFACETEDGE], stats.edgecontractcaseatt[FACETFACETEDGE]);
    fprintf(o, "        facet-segment        %d / %d\n", stats.edgecontractcasesuc[FACETSEGMENTEDGE], stats.edgecontractcaseatt[FACETSEGMENTEDGE]);
    fprintf(o, "        facet-fixed          %d / %d\n", stats.edgecontractcasesuc[FACETFIXEDEDGE], stats.edgecontractcaseatt[FACETFIXEDEDGE]);
    fprintf(o, "        segment-segment      %d / %d\n", stats.edgecontractcasesuc[SEGMENTSEGMENTEDGE], stats.edgecontractcaseatt[SEGMENTSEGMENTEDGE]);
    fprintf(o, "        segment-fixed        %d / %d\n", stats.edgecontractcasesuc[SEGMENTFIXEDEDGE], stats.edgecontractcaseatt[SEGMENTFIXEDEDGE]);
    fprintf(o, "        fixed-fixed          %d / %d\n", stats.edgecontractcasesuc[FIXEDFIXEDEDGE], stats.edgecontractcaseatt[FIXEDFIXEDEDGE]);
    fprintf(o, "    Edge removals:           %d / %d\n", stats.edgeremovals, stats.edgeremovalattempts);
    fprintf(o, "    Boundary edge removals:  %d / %d\n", stats.boundaryedgeremovals, stats.boundaryedgeremovalattempts);
    
    /* find array bounds */
    maxa = maxs = 0;
    for (i=0; i<MAXRINGTETS; i++)
    {
        if (stats.ringsizeattempts[i] != 0) maxa = i;
        if (stats.ringsizesuccess[i] != 0) maxs = i;
    }
    
    /* print histograms */
    fprintf(o, "    Attempt ring sizes (max %d):\n", maxa);
    for (i=3; i<=maxa; i++)
    {
        fprintf(o, "    [%d]: %d\n", i, stats.ringsizeattempts[i]);
    }
    fprintf(o, "    Success ring sizes (max %d):\n", maxs);
    for (i=3; i<=maxs; i++) 
    {
        fprintf(o, "    [%d]: %d\n", i, stats.ringsizesuccess[i]);
    }
    
    fprintf(o, "    Face removals:           %d / %d\n", stats.faceremovals, stats.faceremovalattempts);
    
    /* find array bounds */
    maxa = maxs = 0;
    for (i=0; i<MAXFACETREESIZE; i++)
    {
        if (stats.facesizeattempts[i] != 0) maxa = i;
        if (stats.facesizesuccess[i] != 0) maxs = i;
    }
    
    /* print histograms */
    fprintf(o, "    Attempt tree sizes (max %d):\n", maxa);
    for (i=1; i<=maxa; i++)
    {
        fprintf(o, "    [%d]: %d\n", i, stats.facesizeattempts[i]);
    }
    fprintf(o, "    Success tree sizes (max %d):\n", maxs);
    for (i=1; i<=maxs; i++)
    {
        fprintf(o, "    [%d]: %d\n", i, stats.facesizesuccess[i]);
    }
    fprintf(o, "     2-2 flips:              %d / %d\n", stats.flip22successes, stats.flip22attempts);
    
    /* insertion stats */
    fprintf(o, "\nVertex insertion stats:\n");
    fprintf(o, "    Body insertions:         %d / %d\n", stats.bodyinsertsuccesses, stats.bodyinsertattempts);
    fprintf(o, "    Facet insertions:        %d / %d\n", stats.facetinsertsuccesses, stats.facetinsertattempts);
    fprintf(o, "    Segment insertions:      %d / %d\n", stats.segmentinsertsuccesses, stats.segmentinsertattempts);
    
    /* find array bounds */
    maxb = maxl = 0;
    for (i=0; i<MAXCAVDEPTH; i++)
    {
        if (stats.biggestcavdepths[i] != 0) maxb = i;
        if (stats.lexmaxcavdepths[i] != 0) maxl = i;
    }
    fprintf(o, "    Biggest cavity depths (max %d):\n", maxb);
    for (i=0; i<=maxb; i++)
    {
        fprintf(o, "    [%d]: %d\n", i, stats.biggestcavdepths[i]);
    }
    fprintf(o, "    Lexmax cavity depths (max %d):\n", maxl);
    for (i=0; i<=maxl; i++)
    {
        fprintf(o, "    [%d]: %d\n", i, stats.lexmaxcavdepths[i]);
    }
    
    /* get the current time for animation stuff */
    if (improvebehave.timeseries)
    {
#ifndef NO_TIMER
        gettimeofday(&now, &tz);
        stats.totalmsec = msecelapsed(stats.starttime, now);
#endif /* not NO_TIMER */
        stats.finishnumtets = counttets(mesh);
    }
    
    /* timing stats */
    fprintf(o, "\nTiming stats:\n");
    fprintf(o, "    Smoothing msec: %d\n", stats.smoothmsec);
    fprintf(o, "    Topological msec: %d\n", stats.topomsec);
    fprintf(o, "    Insertion msec: %d\n", stats.insertmsec);
    fprintf(o, "    Contraction msec: %d\n", stats.contractmsec);
    fprintf(o, "    Total msec:     %d\n", stats.totalmsec);
    fprintf(o, "    maxcavusec:     %d\n", stats.biggestcavityusec);
    fprintf(o, "    fincavusec:     %d\n", stats.finalcavityusec);
    fprintf(o, "    improvecavusec: %d\n", stats.cavityimproveusec);
    
    fprintf(o, "Star/End vertex count:       %d / %d\n", stats.startnumverts, stats.startnumverts + stats.bodyinsertsuccesses + stats.facetinsertsuccesses + stats.segmentinsertsuccesses);
    fprintf(o, "Start/End tet count:         %d / %d\n\n", stats.startnumtets, stats.finishnumtets );
    
    /* quality stats */
    fprintf(o, "Quality stats:\n");
    fprintf(o, "    Mean qualities:\n");
    for (i=0; i<NUMMEANTHRESHOLDS; i++)
    {
        fprintf(o, "    [%d]: %g (%g degrees)\n", meanthresholdangles[i], stats.finishmeanquals[i], sintodeg(stats.finishmeanquals[i]));
    }
    
    /* quality stats */
    fprintf(o, "\n    StartMean qualities:\n");
    for (i=0; i<NUMMEANTHRESHOLDS; i++)
    {
        fprintf(o, "    [%d]: %g (%g degrees)\n", meanthresholdangles[i], stats.startmeanquals[i], sintodeg(stats.startmeanquals[i]));
    }
    fprintf(o, "\n    Startworstqual: %g\n", stats.startworstqual);
    fprintf(o, "    Startminangle: %g\n", stats.startminangle);
    fprintf(o, "    Startmaxangle: %g\n", stats.startmaxangle);
    
    fprintf(o, "    dynchangedvol: %g\n", stats.dynchangedvol);
    
    /* cavity sizes */
/*    fprintf(o, "\n    Maxcavity sizes:\n");
    for (i=0; i<1000; i++)
    {
        fprintf(o, "[%d]: %d ", i, stats.maxcavitysizes[i]);
    }
    fprintf(o, "\n    Finalcavity sizes:\n");
    for (i=0; i<200; i++)
    {
        fprintf(o, "[%d]: %d ", i, stats.finalcavitysizes[i]);
    }*/
    
    sizereportstream(o, mesh);
}

void printstats(tetcomplex *mesh)
{
    printstatsstream(stdout, mesh);
}

void printmeans(starreal means[])
{
    int i;
    for (i=0; i<NUMMEANTHRESHOLDS; i++)
    {
        printf("    [%d]: %g (%g degrees)\n", meanthresholdangles[i], means[i], sintodeg(means[i]));
    }
}