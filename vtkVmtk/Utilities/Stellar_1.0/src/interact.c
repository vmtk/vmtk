/*****************************************************************************/
/*      Routines to interact with the user                                   */
/*****************************************************************************/

/* print out all the improvement settings */
void printimproveoptionsstream(FILE *o, struct improvebehavior *b)
{
    fprintf(o,"** Mesh Improvement Options **\n");
    
    fprintf(o,"Output file prefix: %s\n", b->fileprefix);
    
    fprintf(o,"    Quality measure:          "); fprintf(o,"%d\n", b->qualmeasure);
    fprintf(o,"    Sine warp factor:         "); fprintf(o,"%g\n", b->sinewarpfactor);
    fprintf(o,"    Quadric surface error:    "); b->usequadrics ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    fprintf(o,"    Quadric offset:           "); fprintf(o,"%g\n", b->quadricoffset);
    fprintf(o,"    Quadric scale:            "); fprintf(o,"%g\n", b->quadricscale);
    
    fprintf(o,"Smoothing options:\n");
    fprintf(o,"    Optimization-based:       "); b->nonsmooth ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    fprintf(o,"    Facet vertex smoothing:   "); b->facetsmooth ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    fprintf(o,"    Segment vertex smoothing: "); b->segmentsmooth ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    
    fprintf(o,"Topological improvement options:\n");
    fprintf(o,"    Edge removal:             "); b->edgeremoval ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    fprintf(o,"    Edge contraction:         "); b->edgecontraction ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    fprintf(o,"    Boundary edge removal:    "); b->boundedgeremoval ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    fprintf(o,"    Single-face removal:      "); b->singlefaceremoval ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    fprintf(o,"    Multi-face removal:       "); b->multifaceremoval ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    fprintf(o,"    2-2 flips:                "); b->flip22 ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    fprintf(o,"    Star internal flips:      "); b->jflips ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    
    fprintf(o,"Vertex insertion options:\n");
    fprintf(o,"    Vertex insertion:         "); b->enableinsert ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    fprintf(o,"    Insert in worst percent:  "); fprintf(o,"%g\n", b->insertthreshold);
    fprintf(o,"    Body vertex insertion:    "); b->insertbody ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    fprintf(o,"    Facet vertex insertion:   "); b->insertfacet ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    fprintf(o,"    Segment vertex insertion: "); b->insertsegment ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    
    fprintf(o,"Element sizing options:\n");
    fprintf(o,"    Size control:             "); b->sizing ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    fprintf(o,"    Size control pass:        "); b->sizingpass ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    fprintf(o,"    Target edge length:       "); fprintf(o,"%g\n", b->targetedgelength);
    fprintf(o,"    Longer edge factor:       "); fprintf(o,"%g\n", b->longerfactor);
    fprintf(o,"    Shorter edge factor:      "); fprintf(o,"%g\n", b->shorterfactor);
    
    fprintf(o,"Anisotropic meshing options:\n");
    fprintf(o,"    Anisotropic meshing:      "); b->anisotropic ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    fprintf(o,"    Tensor scaling field:     "); fprintf(o,"%d\n", b->tensor);
    fprintf(o,"    Second tensor:            "); fprintf(o,"%d\n", b->tensorb);
    fprintf(o,"    Amount:                   "); fprintf(o,"%g\n", b->tensorblend);
    
    fprintf(o,"Dynamic improvement options:\n");
    fprintf(o,"    Minimum quality:          "); fprintf(o,"%g\n", pq(b->dynminqual));
    fprintf(o,"    Improve to quality:       "); fprintf(o,"%g\n", pq(b->dynimproveto));
    fprintf(o,"    Deformation type:         "); fprintf(o,"%d\n", b->deformtype);
    fprintf(o,"    Do dynamic improvement:   "); fprintf(o,"%d\n", b->dynimprove);
    
    fprintf(o,"Thresholds:\n");
    fprintf(o,"    Minimum mean improvement: "); fprintf(o,"%g\n", b->minstepimprovement);
    fprintf(o,"    Don't insert above qual:  "); fprintf(o,"%g\n", b->maxinsertquality[b->qualmeasure]);
    
    fprintf(o,"Termination options:\n");
    fprintf(o,"    Stop if smallest angle >: "); fprintf(o,"%g\n", b->goalanglemin);
    fprintf(o,"    Stop if largest angle <:  "); fprintf(o,"%g\n", b->goalanglemax);
    
    fprintf(o,"Quality file output options:\n");
    fprintf(o,"    Output .minsine file:     "); b->minsineout ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    fprintf(o,"    Output .minang file:     "); b->minangout ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    fprintf(o,"    Output .maxang file:     "); b->maxangout ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    fprintf(o,"    Output .vlrms file:     "); b->vlrmsout ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    fprintf(o,"    Output .nrr file:     "); b->nrrout ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    
    fprintf(o,"Animation file output: "); b->animate ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    fprintf(o,"Timeseries .stats only: "); b->timeseries ? fprintf(o,"enabled\n") : fprintf(o,"disabled\n");
    
    fprintf(o,"Verbosity:             "); fprintf(o,"%d\n", b->verbosity);
    
    fprintf(o,"\n");
}

void printimproveoptions(struct improvebehavior *b)
{
    printimproveoptionsstream(stdout, b);
}

void parseimproveconfig(char *filename, struct improvebehavior *b)
{
    char word[100];      /* variable name from config file */
    int value=0;         /* variable value from config file */
    starreal fvalue=0.0; /* variable for floats in config file */
    char str[100];
    char fstr[100];
    
    /* temp stuff for line reading */
    int nbytes = 1000;
    char line[1000];
    int numassigned;
    
    FILE *in = fopen(filename, "r");
    assert(in != NULL);
    
    if (improvebehave.verbosity > 4)
    {
        printf("parsing settings file %s\n", filename);
    }
    
    /* read every line of the file */
    while (fgets(line, nbytes, in) != NULL)
    {
        /* attempt to fetch a variable name and value from the config file */
        numassigned = sscanf(line, "%s %d %s %s", word, &value, fstr, str);
        fvalue = atof(fstr);
        
        /* check if this is a comment */
        if (word[0] == '#' || word[0] == '\n' || numassigned == 0) continue;
        
        if (improvebehave.verbosity > 5)
        {
            printf("just read variable %s with value int %d float %g string `%s'\n", word, value, fvalue, str);
        }
        
        /* assign behavior variables based on input */
        if (strcmp(word,"qualmeasure") == 0) b->qualmeasure = value;
        if (strcmp(word,"sinewarpfactor") == 0) b->sinewarpfactor = fvalue;
        if (strcmp(word,"usequadrics") == 0) b->usequadrics = value;
        /* alias old fixed smooth option to usequadrics */
        if (strcmp(word,"usequadrics") == 0) b->fixedsmooth = value;
        if (strcmp(word,"quadricoffset") == 0) b->quadricoffset = fvalue;
        if (strcmp(word,"quadricscale") == 0) b->quadricscale = fvalue;
        
        if (strcmp(word,"nonsmooth") == 0) b->nonsmooth = value;
        if (strcmp(word,"facetsmooth") == 0) b->facetsmooth = value;
        if (strcmp(word,"segmentsmooth") == 0) b->segmentsmooth = value;
        
        if (strcmp(word,"edgeremoval") == 0) b->edgeremoval = value;
        if (strcmp(word,"edgecontraction") == 0) b->edgecontraction = value;
        if (strcmp(word,"boundedgeremoval") == 0) b->boundedgeremoval = value;
        if (strcmp(word,"singlefaceremoval") == 0) b->singlefaceremoval = value;
        if (strcmp(word,"multifaceremoval") == 0) b->multifaceremoval = value;
        if (strcmp(word,"flip22") == 0) b->flip22 = value;
        if (strcmp(word,"jflips") == 0) b->jflips = value;
        
        if (strcmp(word,"insertthreshold") == 0) b->insertthreshold = fvalue;
        if (strcmp(word,"enableinsert") == 0) b->enableinsert = value;
        if (strcmp(word,"insertfacet") == 0) b->insertfacet = value;
        if (strcmp(word,"insertbody") == 0) b->insertbody = value;
        if (strcmp(word,"insertsegment") == 0) b->insertsegment = value;
        if (strcmp(word,"cavityconsiderdeleted") == 0) b->cavityconsiderdeleted = value;
        if (strcmp(word,"cavdepthlimit") == 0) b->cavdepthlimit = value;
        
        if (strcmp(word,"minstepimprovement") == 0) b->minstepimprovement = fvalue;
        if (strcmp(word,"maxinsertquality") == 0) b->maxinsertquality[b->qualmeasure] = fvalue;
        
        if (strcmp(word,"anisotropic") == 0) b->anisotropic = value;
        if (strcmp(word,"tensor") == 0) b->tensor = value;
        if (strcmp(word,"tensorb") == 0) b->tensorb = value;
        if (strcmp(word,"tensorblend") == 0) b->tensorblend = fvalue;
        
        if (strcmp(word,"sizing") == 0) b->sizing = value;
        if (strcmp(word,"sizingpass") == 0) b->sizingpass = value;
        if (strcmp(word,"targetedgelength") == 0) b->targetedgelength = fvalue;
        if (strcmp(word,"longerfactor") == 0) b->longerfactor = fvalue;
        if (strcmp(word,"shorterfactor") == 0) b->shorterfactor = fvalue;
        
        if (strcmp(word,"minsineout") == 0) b->minsineout = value;
        if (strcmp(word,"minangout") == 0) b->minangout = value;
        if (strcmp(word,"maxangout") == 0) b->maxangout = value;
        if (strcmp(word,"vlrmsout") == 0) b->vlrmsout = value;
        if (strcmp(word,"nrrout") == 0) b->nrrout = value;
        
        if (strcmp(word,"fileprefix") == 0) strcpy(b->fileprefix, str);
        
        if (strcmp(word,"animate") == 0) b->animate = value;
        if (strcmp(word,"timeseries") == 0) b->timeseries = value;
        if (strcmp(word,"usecolor") == 0) b->usecolor = value;
        
        if (strcmp(word,"verbosity") == 0) b->verbosity = value;
        
        if (strcmp(word,"goalanglemin") == 0) b->goalanglemin = fvalue;
        if (strcmp(word,"goalanglemax") == 0) b->goalanglemax = fvalue;
        
        if (strcmp(word,"dynminqual") == 0) b->dynminqual = fvalue;
        if (strcmp(word,"dynimproveto") == 0) b->dynimproveto = fvalue;
        if (strcmp(word,"deformtype") == 0) b->deformtype = value;
        if (strcmp(word,"dynimprove") == 0) b->dynimprove = value;
        
        if (strcmp(word,"outputandquit") == 0) b->outputandquit = value;
    }
}

void parseimprovecommandline(int argc, char **argv, struct improvebehavior *b)
{
    int i;
    
    /* set default values for all options */
    b->qualmeasure = QUALMINSINE;
    /*
    b->qualmeasure = QUALWARPEDMINSINE;
    b->qualmeasure = QUALRADIUSRATIO;
    b->qualmeasure = QUALVLRMS3RATIO;
    */
    b->sinewarpfactor = 0.75;
    b->usequadrics = 1;
    b->quadricoffset = 0.8;
    b->quadricscale = 300.0;
    
    b->nonsmooth = 1;
    b->facetsmooth = 1;
    b->segmentsmooth = 1;
    b->fixedsmooth = 0;
    
    b->edgeremoval = 1;
    b->boundedgeremoval = 0;
    b->singlefaceremoval = 1;
    b->multifaceremoval = 1;
    b->flip22 = 1;
    b->jflips = 1;
    
    b->insertthreshold = 0.035;
    b->enableinsert = 1;
    b->insertfacet = 1;
    b->insertbody = 1;
    b->insertsegment = 1;
    b->cavityconsiderdeleted = 0;
    b->cavdepthlimit = 6;
    
    b->edgecontraction = 1;
    
    b->minstepimprovement = 1.0e-4;
    b->mininsertionimprovement = 1.0e-3;
    b->maxinsertquality[0] = SINE40;
    b->maxinsertquality[1] = 0.7;
    b->maxinsertquality[2] = 0.7; 
    b->maxinsertquality[5] = SINE40;
    
    b->anisotropic = 0;
    b->tensor = 0;
    b->tensorb = 0;
    b->tensorblend = 1.0;
    
    b->sizing = 0;
    b->sizingpass = 0;
    b->targetedgelength = 0.0;
    b->longerfactor = 2.0;
    b->shorterfactor = 0.5;
    
    b->dynminqual = SINE20;
    b->dynimproveto = SINE25;
    b->deformtype = TWIST;
    b->dynimprove = 0;
    
    b->verbosity = 1;
    b->usecolor = 0;
    
    b->outputandquit = 0;
    
    b->minsineout = 1;
    b->minangout = 0;
    b->maxangout = 0;
    b->vlrmsout = 0;
    b->nrrout = 0;
    
    strcpy(b->fileprefix, "");
    /*
    strcpy(b->fileprefix, "default");
    */
    
    b->animate = 0;
    b->timeseries = 0;
    
    b->goalanglemin = 90.0;
    b->goalanglemax = 90.0;
    
    if (improvebehave.verbosity > 5)
    {
        printf("finding config file name in args\n");
    }
    
    /* loop through each argument */
    for (i=0; i<argc; i++)
    {
        /* check if this is L */
        if (strcmp(argv[i], "-L") == 0)
        {
            /* set verbosity */
            improvebehave.verbosity = atoi(argv[i+1]);
        }
        
        /* check if this is F */
        if (strcmp(argv[i], "-F") == 0)
        {
            /* set to just output and quit */
            improvebehave.outputandquit = 1;
        }
        
        /* check if this is s */
        if (strcmp(argv[i], "-s") == 0)
        {
            /* parse and set the options from this file */
            parseimproveconfig(argv[i+1], b);
        }
    }
}









