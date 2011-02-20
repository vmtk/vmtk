/*****************************************************************************/
/*                                                                           */
/*  output (screen, file) functions                                          */
/*                                                                           */
/*****************************************************************************/

/* renumber vertices to include the inserted ones */
void myoutputnumbervertices(struct behavior *behave,
                          struct inputs *in,
                          struct proxipool *pool)
{
    struct vertextype *vertexptr;
    struct vertex *thevertex;
    tag vertextag;
    arraypoolulong vertexnumber;

    if (improvebehave.verbosity > 4)
    {
        printf("Renumbering output vertices...\n");
    }

    vertexnumber = (arraypoolulong) 1;
    
    vertextag = proxipooliterate(pool, NOTATAG);
    while (vertextag != NOTATAG) 
    {
        vertexptr = (struct vertextype *) arraypoolforcelookup(&vertexinfo, vertextag);
        thevertex = (struct vertex *) proxipooltag2object(pool, vertextag);
        if ((vertexptr->kind != DEADVERTEX) &&
            (vertexptr->kind != UNDEADVERTEX)) 
        {
            thevertex->number = vertexnumber;
            vertexnumber++;
        }
        vertextag = proxipooliterate(pool, vertextag);
    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("total of %lu vertices now...\n", vertexnumber-1);
    }
    
    globalvertexcount = vertexnumber-1;
}

/* customized vertex output TODO: this is mostly copied from Jonathan's 
   output routines */
void myoutputvertices(struct behavior *behave,
                    struct inputs *in,
                    struct proxipool *pool,
                    int argc,
                    char **argv)
{
  struct vertextype *vertexptr;
  struct vertex *thevertex;
  starreal *attributes;
  FILE *outfile;
  tag vertextag;
  arraypoolulong outvertexcount;
  arraypoolulong vertexnumber;
  arraypoolulong i;
  unsigned int j;

  outvertexcount = globalvertexcount;
  
  if (!behave->quiet && !improvebehave.animate) {
    printf("Writing %s.\n", behave->outnodefilename);
  }

  outfile = fopen(behave->outnodefilename, "w");
  if (outfile == (FILE *) NULL) {
    printf("  Error:  Cannot create file %s.\n", behave->outnodefilename);
    starexit(1);
  }

  /* Number of vertices, number of dimensions, number of vertex attributes, */
  /*   and number of boundary markers (zero or one).                        */
/*
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
  if (sizeof(arraypoolulong) > sizeof(long)) {
    fprintf(outfile, "%llu  3  %u  %d\n", (unsigned long long) outvertexcount,
            in->attribcount, 1 - behave->nobound);
  } else {
    fprintf(outfile, "%lu  3  %u  %d\n", (unsigned long) outvertexcount,
            in->attribcount, 1 - behave->nobound);
  }
#else
*/
  fprintf(outfile, "%lu  3  %u  %d\n", (unsigned long) outvertexcount,
          in->attribcount, 1 - behave->nobound);
/*
#endif
*/
  
  vertexnumber = (arraypoolulong) behave->firstnumber;
  vertextag = proxipooliterate(pool, NOTATAG);
  i = 0;
  while (vertextag != NOTATAG) {
    vertexptr = (struct vertextype *) arraypoolforcelookup(&vertexinfo, vertextag);
    thevertex = (struct vertex *) proxipooltag2object(pool, vertextag);
    if ((vertexptr->kind != DEADVERTEX) &&
        (!behave->jettison || (vertexptr->kind != UNDEADVERTEX))) {
      /* Node number, x, y, and z coordinates. */
/*
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
      if (sizeof(arraypoolulong) > sizeof(long)) {
        fprintf(outfile, "%4llu    %.17g  %.17g  %.17g",
                (unsigned long long) vertexnumber,
                (starreal) thevertex->coord[0], (starreal) thevertex->coord[1],
                (starreal) thevertex->coord[2]);
      } else {
        fprintf(outfile, "%4lu    %.17g  %.17g  %.17g",
                (unsigned long) vertexnumber,
                (starreal) thevertex->coord[0], (starreal) thevertex->coord[1],
                (starreal) thevertex->coord[2]);
      }
#else
*/
      fprintf(outfile, "%4lu    %.17g  %.17g  %.17g",
              (unsigned long) vertexnumber,
              (starreal) thevertex->coord[0], (starreal) thevertex->coord[1],
              (starreal) thevertex->coord[2]);
/*
#endif
*/
      if (in->attribcount > 0) {
        attributes = (starreal *) proxipooltag2object2(pool,
                                                       in->vertextags[i]);
        for (j = 0; j < in->attribcount; j++) {
          /* Write an attribute. */
          fprintf(outfile, "  %.17g", (starreal) attributes[j]);
        }
      }
      if (behave->nobound) {
        putc('\n', outfile);
      } else {
        /* Write the boundary marker. */
        fprintf(outfile, "    %ld\n", (long) thevertex->mark);
      }

      thevertex->number = vertexnumber;
      vertexnumber++;
    }

    vertextag = proxipooliterate(pool, vertextag);
  }

  outputfilefinish(outfile, argc, argv);
}

/* output a file with tet qualities */
void outputtetqualities(struct tetcomplex *plex,
                        struct behavior *behave,
                        char filename[],
                        int argc,
                        char **argv,
                        int qualmeasure)
{
  struct tetcomplexposition position;
  tag tet[4];
  FILE *outfile;
  arraypoolulong tetnumber;
  starreal quality;

  if (!behave->quiet && !improvebehave.animate) {
    printf("Writing %s.\n", filename);
  }

  outfile = fopen(filename, "w");
  if (outfile == (FILE *) NULL) {
    printf("  Error:  Cannot create file %s.\n", filename);
    starexit(1);
  }

  /* Number of tetrahedra, vertices per tetrahedron, attributes */
  /*   per tetrahedron.                                         */
  fprintf(outfile, "%lu  4  0\n", (unsigned long) tetcomplextetcount(plex));

  tetnumber = (arraypoolulong) behave->firstnumber;
  tetcomplexiteratorinit(plex, &position);
  tetcomplexiteratenoghosts(&position, tet);
  while (tet[0] != STOP) 
  {
    /* whether this tet is a boundary tet */
    if (boundtet(plex, tet[0], tet[1], tet[2], tet[3])) 
    {
      fprintf(outfile, "1 ");
    } else {
      fprintf(outfile, "0 ");
    }
    
    quality = tetquality(plex, tet[0], tet[1], tet[2], tet[3], qualmeasure);
    
    fprintf(outfile, "%.6g\n", quality);
    
    tetcomplexiteratenoghosts(&position, tet);
    tetnumber++;
  }

  outputfilefinish(outfile, argc, argv);
}

/* output a file with mesh surface faces */
void outputsurfacefaces(struct tetcomplex *mesh,
                        struct behavior *behave,
                        char filename[],
                        int argc,
                        char **argv)
{
  FILE *outfile;
  tag *face;
  
  struct proxipool *pool;
  struct vertex *vertexptr0;
  struct vertex *vertexptr1;
  struct vertex *vertexptr2;
  
  struct arraypool facepool;
  int numfaces = 0;
  int i;
  
  pool = mesh->vertexpool;
  
  /* allocate pool for faces */
  arraypoolinit(&(facepool), sizeof(tag)*3, LOG2TETSPERSTACKBLOCK, 0);
  /* find the surface faces */
  getsurface(mesh, &facepool, &numfaces);

  if (!behave->quiet && !improvebehave.animate) {
    printf("Writing %s.\n", filename);
  }

  outfile = fopen(filename, "w");
  if (outfile == (FILE *) NULL) {
    printf("  Error:  Cannot create file %s.\n", filename);
    starexit(1);
  }

  /* print number of faces */
  fprintf(outfile, "%d 0\n", numfaces);

  for (i=0; i<numfaces; i++)
  {
      /* fetch this face from the pool */
      face = (tag *) arraypoolfastlookup(&facepool, (unsigned long) i);
      
      vertexptr0 = (struct vertex *) proxipooltag2object(pool, face[0]);
      vertexptr1 = (struct vertex *) proxipooltag2object(pool, face[1]);
      vertexptr2 = (struct vertex *) proxipooltag2object(pool, face[2]);
      
      /* print this face to the file */
      fprintf(outfile, "%4lu  %4lu  %4lu  %4lu  \n",
        (unsigned long) i,
        (unsigned long) vertexptr0->number,
        (unsigned long) vertexptr1->number,
        (unsigned long) vertexptr2->number);
  }

  outputfilefinish(outfile, argc, argv);
  
  /* free the surface face pool */
  arraypooldeinit(&facepool);
}

/* output improvement statistics */
void outputstats(struct behavior *behave,
                 struct tetcomplex *mesh,
                 char filename[],
                 int argc,
                 char **argv)
{
  FILE *outfile;

  if (!behave->quiet && !improvebehave.animate) {
    printf("Writing %s.\n", filename);
  }

  outfile = fopen(filename, "w");
  if (outfile == (FILE *) NULL) {
    printf("  Error:  Cannot create file %s.\n", filename);
    starexit(1);
  }

  /* print options settings */
  printimproveoptionsstream(outfile, &improvebehave);

  /* print physical quality stats */
  if (improvebehave.anisotropic)
  {
      statisticsqualitystream(outfile, behave, mesh, true);
  }
  
  statisticsqualitystream(outfile, behave, mesh, false);

  /* print the improvement statistics */
  printstatsstream(outfile, mesh);
  
  /* print quadric stats */
  checkquadricsstream(outfile, mesh);
  
  outputfilefinish(outfile, argc, argv);
}

/* output a mapping from tag -> output vertex number
   TODO is this even still used? */
void outputtagmap(struct behavior *behave,
                    struct inputs *in,
                    struct proxipool *pool,
                    char filename[],
                    int argc,
                    char **argv)
{
  struct vertextype *vertexptr;
  struct vertex *thevertex;
  FILE *outfile;
  tag vertextag;
  arraypoolulong outvertexcount;
  arraypoolulong vertexnumber;
  arraypoolulong i;

  outvertexcount = globalvertexcount;
  
  if (!behave->quiet && !improvebehave.animate) {
    printf("Writing %s.\n", filename);
  }

  outfile = fopen(filename, "w");
  if (outfile == (FILE *) NULL) {
    printf("  Error:  Cannot create file %s.\n", filename);
    starexit(1);
  }

  vertexnumber = (arraypoolulong) behave->firstnumber;
  vertextag = proxipooliterate(pool, NOTATAG);
  i = 0;
  while (vertextag != NOTATAG) {
    vertexptr = (struct vertextype *) arraypoolforcelookup(&vertexinfo, vertextag);
    thevertex = (struct vertex *) proxipooltag2object(pool, vertextag);
    if ((vertexptr->kind != DEADVERTEX) &&
        (!behave->jettison || (vertexptr->kind != UNDEADVERTEX))) {
      /* Node number, x, y, and z coordinates. */
      fprintf(outfile, "%lu %4lu\n", (unsigned long) vertextag, (unsigned long) vertexnumber);
      thevertex->number = vertexnumber;
      vertexnumber++;
    }

    vertextag = proxipooliterate(pool, vertextag);
  }

  outputfilefinish(outfile, argc, argv);
}

/* output a file with animation info */
void outputanim(struct behavior *behave,
                 struct tetcomplex *mesh,
                 char filename[],
                 int passnum,
                 int passtype,
                 int passstartid,
                 int argc,
                 char **argv)
{
    FILE *outfile;
    int startindex=-1, endindex=-1, numops;

    if (!behave->quiet && !improvebehave.animate) {
        printf("Writing %s.\n", filename);
    }

    outfile = fopen(filename, "w");
    if (outfile == (FILE *) NULL) {
        printf("  Error:  Cannot create file %s.\n", filename);
        starexit(1);
    }

    /* type of pass */
    fprintf(outfile, "passtype: ");
    switch (passtype)
    {
        case SMOOTHPASS:
            fprintf(outfile, "smooth\n");
            break;
        case TOPOPASS:
            fprintf(outfile, "topo\n");
            break;
        case INSERTPASS:
            fprintf(outfile, "insert\n");
            break;
        case DESPERATEPASS:
            fprintf(outfile, "desperateinsert\n");
            break;
        case DEFORMPASS:
            fprintf(outfile, "deform\n");
            break;
        case SIZECONTROLPASS:
            fprintf(outfile, "sizecontrol\n");
            break;
        case CONTRACTALLPASS:
        case CONTRACTPASS:
            fprintf(outfile, "contract\n");
            break;
        default:
            printf("I don't know what passtype %d is, dying.", passtype);
            starexit(1);
    }
    
    /* convert from journal id's to journal indices */
    journalrangetoindex(passstartid, lastjournalentry(), &startindex, &endindex, &numops);
    assert(startindex != -1 && endindex != -1);
    
    /* journal ID before/after the pass */
    fprintf(outfile, "startid: %d\n", startindex);
    fprintf(outfile, "endid: %d\n", endindex);
    fprintf(outfile, "numops: %d\n", numops);
    /* if it's a topo pass, print out all the shit that went down */
    if (passtype == TOPOPASS)
    {
        printjournalrangestream(outfile, startindex, endindex);
    }
    /* if doing dynamic improvement, print proportion of changed volume */
    fprintf(outfile, "dynchangedvol: %g\n", stats.dynchangedvol);

    outputfilefinish(outfile, argc, argv);
}

/* print out the entire mesh. includes node positions,
   tet node values, and tet quality values */
void outputqualmesh(struct behavior *b,
                    struct inputs *in,
                    struct proxipool *vertexpool,
                    struct tetcomplex *mesh,
                    int argc,
                    char **argv,
                    int passnum,
                    int passtype,
                    int passstartid,
                    int qualmeasure)
{
    char qualfilename[300];
    char minangfilename[300];
    char maxangfilename[300];
    char vlrmsfilename[300];
    char rnrrfilename[300];
    char facefilename[300];
    char statsfilename[300];
    char animfilename[300];
    char tagmapfilename[300];
    char framenumber[10];
    int meshnumber;
    int increment;
    char workstring[FILENAMESIZE];
    int j;
    
    sprintf(framenumber, "_p%04d", passnum);
    
    increment = 0;
    /* clip off extension */
    b->innodefilename[strlen(b->innodefilename) - 5] = '\0';
    strcpy(workstring, b->innodefilename);
    /* Find the last period in the filename. */
    j = 1;
    while (workstring[j] != '\0') {
      if ((workstring[j] == '.') && 
          (workstring[j + 1] != '\0') && 
          (workstring[j + 1] != '/') && 
          (workstring[j + 1] != '\\') &&
          (workstring[j + 1] != '.')) {
        increment = j + 1;
      }
      j++;
    }
    
    /* The iteration number is zero by default, unless there's an iteration */
    /*   number in the filename.                                            */
    meshnumber = 0;
    if (increment > 0) {
      /* Read the iteration number from the end of the filename. */
      j = increment;
      do {
        if ((workstring[j] >= '0') && (workstring[j] <= '9')) {
          
          meshnumber = meshnumber * 10 + (int) (workstring[j] - '0');
        } else {
          /* Oops, not a digit; this isn't an iteration number after all. */
          meshnumber = 0;
          break;
        }
        j++;
      } while (workstring[j] != '\0');
    }
    
    if (increment == 0)
    {
        increment = strlen(b->innodefilename);
        workstring[increment] = '.';
        increment++;
    }
    
    workstring[increment] = '%';
    workstring[increment + 1] = 'd';
    workstring[increment + 2] = '\0';
    sprintf(b->innodefilename, workstring, meshnumber + 1);
    
    /* create custome output filename */
    if (passnum == 0 && argv !=NULL)
    {
        /* copy the specified prefix into temp */
        if (strcmp(improvebehave.fileprefix,"") != 0)
        {
            strcpy(b->innodefilename, improvebehave.fileprefix);
        }
    }
    
    /* node file name */
    strcpy(b->outnodefilename, b->innodefilename);
    if (improvebehave.animate)
    {
        strcat(b->outnodefilename, framenumber);
    }
    strcat(b->outnodefilename, ".node");
    
    /* ele file name */
    strcpy(b->outelefilename, b->innodefilename);
    if (improvebehave.animate)
    {
        strcat(b->outelefilename, framenumber);
    }
    strcat(b->outelefilename, ".ele");
    
    /* face file name */
    strcpy(facefilename, b->innodefilename);
    if (improvebehave.animate)
    {
        strcat(facefilename, framenumber);
    }
    strcat(facefilename, ".face");
    
    /* quality file name */
    strcpy(qualfilename, b->innodefilename);
    if (improvebehave.animate)
    {
        strcat(qualfilename, framenumber);
    }
    strcat(qualfilename, ".minsine");
    
    /* minimum angle file name */
    strcpy(minangfilename, b->innodefilename);
    if (improvebehave.animate)
    {
        strcat(minangfilename, framenumber);
    }
    strcat(minangfilename, ".minang");
    
    /* maximum angle file name */
    strcpy(maxangfilename, b->innodefilename);
    if (improvebehave.animate)
    {
        strcat(maxangfilename, framenumber);
    }
    strcat(maxangfilename, ".maxang");
    
    /* vlrms file name */
    strcpy(vlrmsfilename, b->innodefilename);
    if (improvebehave.animate)
    {
        strcat(vlrmsfilename, framenumber);
    }
    strcat(vlrmsfilename, ".vlrms");
    
    /* rnrr file name */
    strcpy(rnrrfilename, b->innodefilename);
    if (improvebehave.animate)
    {
        strcat(rnrrfilename, framenumber);
    }
    strcat(rnrrfilename, ".nrr");
    
    /* stats file name */
    strcpy(statsfilename, b->innodefilename);
    if (improvebehave.animate)
    {
        strcat(statsfilename, framenumber);
    }
    strcat(statsfilename, ".stats");
    
    /* animation file name */
    if (improvebehave.animate && passnum != 0)
    {
        strcpy(animfilename, b->innodefilename);
        strcat(animfilename, framenumber);
        strcat(animfilename, ".anim");
        strcpy(tagmapfilename, b->innodefilename);
        strcat(tagmapfilename, framenumber);
        strcat(tagmapfilename, ".tagmap");
    }
    
    /* don't output bulky information if all we want is stats for timeseries */
    if (improvebehave.timeseries == 0)
    {
        /* renumber vertices */
        myoutputnumbervertices(b, in, vertexpool);
    
        /* output vertices */
        myoutputvertices(b, in, vertexpool, argc, argv);
    
        /* output tetrahedra */
        outputtetrahedra(b, in, mesh, argc, argv);
    
        /* output surface faces */
        outputsurfacefaces(mesh, b, facefilename, argc, argv);
    
        /* output tetrahedra -> quality mapping */
        if (improvebehave.minsineout)
        {
            outputtetqualities(mesh, b, qualfilename, argc, argv, QUALMINSINE);
        }
        
        /* output tetrahedra -> minimum angle mapping */
        if (improvebehave.minangout)
        {
            outputtetqualities(mesh, b, minangfilename, argc, argv, QUALMINANGLE);
        }
        
        /* output tetrahedra -> maximum angle mapping */
        if (improvebehave.maxangout)
        {
            outputtetqualities(mesh, b, maxangfilename, argc, argv, QUALMAXANGLE);
        }
        
        /* output tetrahedra -> maximum angle mapping */
        if (improvebehave.vlrmsout)
        {
            outputtetqualities(mesh, b, vlrmsfilename, argc, argv, QUALVLRMS3RATIO);
        }
        
        /* output tetrahedra -> maximum angle mapping */
        if (improvebehave.nrrout)
        {
            outputtetqualities(mesh, b, rnrrfilename, argc, argv, QUALRADIUSRATIO);
        }
    }
    
    /* output improvement stats */
    outputstats(b, mesh, statsfilename, argc, argv);
    
    /* output animation info from previous frame to this one */
    if (improvebehave.animate && passnum != 0 && improvebehave.timeseries == 0)
    {
        outputanim(b, mesh, animfilename, passnum, passtype, passstartid, argc, argv);
        outputtagmap(b, in, vertexpool, tagmapfilename, argc, argv);
    }
}
