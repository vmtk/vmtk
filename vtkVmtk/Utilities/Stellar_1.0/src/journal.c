/*****************************************************************************/
/*             Global journal and helper functions                           */
/*****************************************************************************/
/* improvement journal entry. this should be general enough
   to store information on any change to the mesh, and for each
   change should store enough information to invert it */
   
enum journalentryclasses
{
    ALLENTRIES,    /* match all entries */
    INSERTDELETE,  /* insertion or deletion of vertices */
    SMOOTH,        /* change position of a single vertex */
    TOPOLOGICAL,   /* topological change involving multiple vertices */
    LABEL          /* classify a vertex */
};

enum journalentrytypes
{
    INSERTVERTEX,  /* insertion of a vertex */
    DELETEVERTEX,  /* deletion of a vertex */
    SMOOTHVERTEX,  /* smoothing of a vertex */
    DELETETET,     /* single tet deletion */
    INSERTTET,     /* single tet insertion */
    FLIP23,        /* 2-3 flip, base case for face removal */
    FLIP32,        /* 3-2 flip, base case for edge removal */
    FLIP22,        /* 2-2 flip */
    FLIP41,        /* 4-1 flip (precedes deletion of interior vertex) */
    FLIP14,        /* 1-4 flip (follows insertion of interior vertex) */
    FLIP13,        /* 1-3 flip (same as 1-4 but inserted vertex is on facet) */
    FLIP31,        /* the inverse of a 1-3 flip */
    FLIP12,        /* 1-2 flip (same as 1-4 but inserted vertex is on edge) */
    FLIP21,        /* inverse of 1-2 */
    CLASSIFY       /* classify vertex (FREEVERTEX, FACETVERTEX, etc) */
};

struct journalentry
{
    int id;             /* unique identifier for this entry */
    int class;          /* class (smoothing, topological, etc) */
    int type;           /* type of entry, selected from jounalentrytypes */
    tag verts[5];       /* up to five vertices involved in the operation */
    int numverts;       /* the exact number involved in the operation, or the type (for classify)*/
    starreal newpos[3]; /* the new position of the vertex, if it was smoothed */  
    starreal oldpos[3]; /* the old position of the vertex, if it was moved */
};

/* GLOBAL */
struct arraypoolstack* journal;
struct arraypoolstack journalstack;

/* print a single journal entry */
void printjournalentry(struct journalentry *entry)
{
    switch (entry->type)
    {
        case INSERTVERTEX:
            printf("(ID: %d) Inserted new vertex %d\n", entry->id, (int) entry->verts[0]);
            break;
        case SMOOTHVERTEX:
            printf("(ID: %d) Smoothed vertex %d from (%g %g %g) to (%g %g %g)\n", entry->id, (int) entry->verts[0], entry->oldpos[0], entry->oldpos[1], entry->oldpos[2], entry->newpos[0], entry->newpos[1], entry->newpos[2]);
            break;
        case INSERTTET:
            printf("(ID: %d) Inserted tet (v1=%d, v2=%d, v3=%d, v4=%d)\n", entry->id, (int) entry->verts[0],
                                                                                      (int) entry->verts[1],
                                                                                      (int) entry->verts[2],
                                                                                      (int) entry->verts[3]);
            break;
        case DELETETET:
            printf("(ID: %d) Deleted tet (v1=%d, v2=%d, v3=%d, v4=%d)\n", entry->id, (int) entry->verts[0],
                                                                                      (int) entry->verts[1],
                                                                                      (int) entry->verts[2],
                                                                                      (int) entry->verts[3]);
            break;
        case FLIP23:
            printf("(ID: %d) Performed 2-3 flip on submesh (v1=%d, v2=%d, v3=%d, vbot=%d, vtop=%d)\n", entry->id, (int) entry->verts[0],
                                                                                              (int) entry->verts[1],
                                                                                              (int) entry->verts[2],
                                                                                              (int) entry->verts[3],
                                                                                              (int) entry->verts[4]);
            break;
        case FLIP22:
            printf("(ID: %d) Performed 2-2 flip on submesh (v1=%d, v2=%d, v3=%d, vbot=%d, vtop=%d)\n", entry->id, (int) entry->verts[0],
                                                                                              (int) entry->verts[1],
                                                                                              (int) entry->verts[2],
                                                                                              (int) entry->verts[3],
                                                                                              (int) entry->verts[4]);
            break;
        case FLIP32:
            printf("(ID: %d) Performed 3-2 flip on submesh (v1=%d, v2=%d, v3=%d, vbot=%d, vtop=%d)\n", entry->id, (int) entry->verts[0],
                                                                                              (int) entry->verts[1],
                                                                                              (int) entry->verts[2],
                                                                                              (int) entry->verts[3],
                                                                                              (int) entry->verts[4]);
            break;
        case FLIP14:
            printf("(ID: %d) Performed 1-4 flip on submesh (v1=%d, v2=%d, v3=%d, v4=%d, vfacet=%d)\n", entry->id, (int) entry->verts[0],
                                                                                              (int) entry->verts[1],
                                                                                              (int) entry->verts[2],
                                                                                              (int) entry->verts[3],
                                                                                              (int) entry->verts[4]);
            break;
        case FLIP13:
            printf("(ID: %d) Performed 1-3 flip on submesh (v1=%d, v2=%d, v3=%d, v4=%d, vfacet=%d)\n", entry->id, (int) entry->verts[0],
                                                                                              (int) entry->verts[1],
                                                                                              (int) entry->verts[2],
                                                                                              (int) entry->verts[3],
                                                                                              (int) entry->verts[4]);
            break;
        case FLIP12:
            printf("(ID: %d) Performed 1-3 flip on submesh (v1=%d, v2=%d, v3=%d, v4=%d, vfacet=%d)\n", entry->id, (int) entry->verts[0],
                                                                                              (int) entry->verts[1],
                                                                                              (int) entry->verts[2],
                                                                                              (int) entry->verts[3],
                                                                                              (int) entry->verts[4]);
            break;
        case CLASSIFY:
            printf("(ID %d) Assigned vertex %d type %d\n", entry->id, (int) entry->verts[0], entry->numverts);
            break;
        default:
            printf("(ID %d) I don't know the journal entry type %d\n", entry->id, entry->type);
    }
}

/* print a single journal entry */
void printjournalentrystream(FILE* o, struct journalentry *entry, int index)
{
    int i;
    
    /* first print the ID of the operation */
    fprintf(o, "%d ", index);
    
    /* now print the type */
    switch (entry->type)
    {
        case INSERTVERTEX:
            fprintf(o,"INSERTVERTEX ");
            break;
        case SMOOTHVERTEX:
            fprintf(o,"SMOOTHVERTEX ");
            break;
        case INSERTTET:
            fprintf(o,"INSERTTET ");
            break;
        case DELETETET:
            fprintf(o,"DELETETET ");
            break;
        case FLIP23:
            fprintf(o,"FLIP23 ");
            break;
        case FLIP22:
            fprintf(o,"FLIP22 ");
            break;
        case FLIP32:
            fprintf(o,"FLIP32 ");
            break;
        case FLIP14:
            fprintf(o,"FLIP14 ");
            break;
        case FLIP13:
            fprintf(o,"FLIP13 ");
            break;
        case FLIP12:
            fprintf(o,"FLIP12 ");
            break;
        case CLASSIFY:
            fprintf(o,"CLASSIFY ");
            break;
        default:
            printf("(ID %d) I don't know the journal entry type %d\n", entry->id, entry->type);
            starexit(1);
    }
    
    /* number of vertices involved in the operation */
    fprintf(o, "%d ", entry->numverts);
    
    /* the tags of these vertices 
       (NB these are different from the vertex numbers in output) */
    for (i=0; i<entry->numverts; i++)
    {
        fprintf(o, "%d ", (int) entry->verts[i]);
    }
    
    /* if this is a smooth or insert, print the new vertex position */
    if ((entry->type == SMOOTHVERTEX) || (entry->type == INSERTVERTEX))
    {
        fprintf(o, "%.18g %.18g %1.8g ", entry->newpos[0], entry->newpos[1], entry->newpos[2]);
    }
    
    /* end the line */
    fprintf(o, "\n");
}

/* return the ID of the most recent journal entry */
int lastjournalentry(void)
{
    return ((struct journalentry *) arraypoolfastlookup(&(journal->pool), (unsigned long) journal->top))->id;
}

/* copy the top fromtop elements from one stack to another */
void copyjournaltop(struct arraypoolstack *fromstack,
                    struct arraypoolstack *tostack,
                    int fromtop)
{
    struct journalentry *fromentry, *toentry;
    int i;
    
    /* reset the to stack */
    stackrestart(tostack);
    
    /* copy fromtop -> top */
    for (i=fromtop; i<=fromstack->top; i++)
    {
        /* get the right tet out of stack */
        fromentry = (struct journalentry *) arraypoolfastlookup(&(fromstack->pool), (unsigned long) i);
        toentry = (struct journalentry *) stackpush(tostack);
        
        /* copy it over */
        memcpy(toentry, fromentry, sizeof(struct journalentry));
    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("copied top of stack\n");
        for (i=0; i<10; i++)
        {
            fromentry = (struct journalentry *) arraypoolfastlookup(&(fromstack->pool), (unsigned long) fromstack->top - i);
            toentry = (struct journalentry *) arraypoolfastlookup(&(tostack->pool), (unsigned long) tostack->top - i);
            printf("[%d] fromentry %d toentry %d\n", i, fromentry->id, toentry->id);
        }
        printf("fromstack->top / 2 = %d, tostack->top = %d\n", (int) fromstack->top / 2, (int) tostack->top);
    }
    
    assert(fromstack->top / 2 == tostack->top - 1 || fromstack->top / 2 == tostack->top);
}

/* chop the journal down to half its current size */
void chopjournal(void)
{
    struct arraypoolstack newjournal;
    stackinit(&newjournal, sizeof(struct journalentry));
    
    if (improvebehave.verbosity > 5)
    {
        printf("halving stack from %d to %d elements\n", (int) journal->top, (int) journal->top / 2);
    }
    
    /* copy the top half of the elements */
    copyjournaltop(journal, &newjournal, journal->top / 2);
    
    /* free the old stack */
    stackdeinit(journal);
    
    /* copy the new stack to the old stack */
    memcpy(journal, &newjournal, sizeof(struct arraypoolstack));
}

/* insert the record of an improvement action in the journal */
void insertjournalentry(struct tetcomplex * mesh,
                        int type,
                        tag verts[],
                        int numverts,
                        starreal oldpos[],
                        starreal newpos[])
{
    struct journalentry* entry;
    int i;
    
    /* if journal is getting too big, chop it in half */
    if (journal->top > JOURNALHALFSIZE)
    {
        chopjournal();
    }
    
    /* create a new journal entry on the top of the stack */
    entry = (struct journalentry *) stackpush(journal);    
    /* give this entry a unique id */
    entry->id = maxjournalid++;
    /* assign the type */
    entry->type = type;
    /* copy vertices */
    entry->numverts = numverts;
    for (i=0; i<numverts; i++)
    {
        entry->verts[i] = verts[i];
    }
    
    /* type specific stuff */
    switch (type)
    {
        case SMOOTHVERTEX:
            /* copy previous and new vertex location */
            vcopy(oldpos, entry->oldpos);
            vcopy(newpos, entry->newpos);
            entry->class = SMOOTH;
            break;
        case INSERTTET:
            entry->class = TOPOLOGICAL;
        case DELETETET:
            entry->class = TOPOLOGICAL;
        case FLIP23:
            entry->class = TOPOLOGICAL;
            break;
        case FLIP22:
            entry->class = TOPOLOGICAL;
            break;
        case FLIP32:
            entry->class = TOPOLOGICAL;
            break;
        case FLIP13:
            entry->class = TOPOLOGICAL;
            break;
        case FLIP12:
            entry->class = TOPOLOGICAL;
            break;
        case FLIP14:
            entry->class = TOPOLOGICAL;
            break;
        case FLIP41:
            entry->class = TOPOLOGICAL;
            break;
        case INSERTVERTEX:
            /* copy initial vertex position */
            vcopy(newpos, entry->newpos);
            entry->class = INSERTDELETE;
            /* force set the new vertex's type to INPUTVERTEX */
            /* because we're about to classify it */
            ((struct vertextype *) arraypoolforcelookup(&vertexinfo, verts[0]))->kind = INPUTVERTEX;
            break;
        case CLASSIFY:
            entry->class = LABEL;
            /* copy newpos, which encodes constraining vector of facet or segment */
            vcopy(newpos, entry->newpos);
            break;
        default:
            printf("I don't know the journal entry type %d\n", entry->type);
            starexit(1);
    };
    
    if (improvebehave.verbosity > 5)
    {
        printf("Now adding the following entry:");
        printjournalentry(entry);
    }
}

/* playback a single journal entry */
void playbackjournalentry(struct tetcomplex * mesh,
                        struct journalentry *entry)
{
    starreal* pos;
    tag newtag;
    struct vertextype *info;
    struct vertex *newvertex;         /* pointer to newly allocated vertex */
    
    if (improvebehave.verbosity > 4)
    {
        printf("Now playing back the following entry:\n");
        printjournalentry(entry);
    }
    
    switch (entry->type)
    {
        case SMOOTHVERTEX:
            /* to playback a smooth, move the vertex to where it was smoothed */
            /* fetch the vertex position */
            pos = ((struct vertex *) tetcomplextag2vertex(mesh, entry->verts[0]))->coord;
            /* replace it with the new value */
            vcopy(entry->newpos, pos);
            
            break;
        case INSERTTET:
            inserttet(mesh,
                      entry->verts[0],
                      entry->verts[1],
                      entry->verts[2],
                      entry->verts[3],
                      false);
            break;
        case DELETETET:
            deletetet(mesh,
                      entry->verts[0],
                      entry->verts[1],
                      entry->verts[2],
                      entry->verts[3],
                      false);
            break;
        case FLIP23:
            flip23(mesh,
                   entry->verts[0],
                   entry->verts[1],
                   entry->verts[2],
                   entry->verts[3],
                   entry->verts[4],
                   false);
            break;
        case FLIP22:
            flip22(mesh,
                   entry->verts[0],
                   entry->verts[1],
                   entry->verts[2],
                   entry->verts[3],
                   entry->verts[4],
                   false);
            break;
        case FLIP32:
            flip32(mesh,
                   entry->verts[0],
                   entry->verts[1],
                   entry->verts[2],
                   entry->verts[3],
                   entry->verts[4],
                   false);
            break;
        case FLIP13:
            flip13(mesh,
                   entry->verts[0],
                   entry->verts[1],
                   entry->verts[2],
                   entry->verts[3],
                   entry->verts[4],
                   false);
            break;
        case FLIP12:
            flip12(mesh,
                   entry->verts[0],
                   entry->verts[1],
                   entry->verts[2],
                   entry->verts[3],
                   entry->verts[4],
                   false);
            break;
        case FLIP14:
            flip14(mesh,
                   entry->verts[0],
                   entry->verts[1],
                   entry->verts[2],
                   entry->verts[3],
                   entry->verts[4],
                   false);
            break;
        case FLIP41:
            flip41(mesh,
                   entry->verts[0],
                   entry->verts[1],
                   entry->verts[2],
                   entry->verts[3],
                   entry->verts[4],
                   false);
            break;
        case INSERTVERTEX:
            /* allocate the new vertex */
            newtag = proxipoolnew(mesh->vertexpool, 0, (void **) &newvertex);
            /* make sure it has the right ID according to the journal */
            assert(newtag == entry->verts[0]);
            break;
        case CLASSIFY:
            /* fetch vertex info structure */
            info = (struct vertextype *) arraypoolforcelookup(&vertexinfo, entry->verts[0]);
            /* set the vertex type, which is encoded in numverts */
            info->kind = entry->numverts;
            /* copy the facet or segment vector, encoded in newpos */
            vcopy(entry->newpos, info->vec);
        default:
            printf("I don't know the journal entry type %d\n", entry->type);
            starexit(1);
    };
}


/* invert a single journal entry */
void invertjournalentry(struct tetcomplex * mesh,
                        struct journalentry *entry)
{
    starreal* pos;
    
    if (improvebehave.verbosity > 5)
    {
        printf("Now inverting the following entry:\n");
        printjournalentry(entry);
    }
    
    switch (entry->type)
    {
        case SMOOTHVERTEX:
            /* to invert a smooth, just put the vertex back where it came from */
            /* fetch the vertex position */
            pos = ((struct vertex *) tetcomplextag2vertex(mesh, entry->verts[0]))->coord;
            /* replace it with the old value */
            vcopy(entry->oldpos, pos);
            
            break;
        case INSERTTET:
            /* to invert an insertion, perform a deletion */
            deletetet(mesh,
                      entry->verts[0],
                      entry->verts[1],
                      entry->verts[2],
                      entry->verts[3],
                      false);
            break;
        case DELETETET:
            /* to invert a deletion, perform an insertion */
            inserttet(mesh,
                      entry->verts[0],
                      entry->verts[1],
                      entry->verts[2],
                      entry->verts[3],
                      false);
            break;
        case FLIP23:
            /* to invert a 2-3 flip, perform a 3-2 flip on the same submesh */
            flip32(mesh,
                   entry->verts[0],
                   entry->verts[1],
                   entry->verts[2],
                   entry->verts[3],
                   entry->verts[4],
                   false);
            break;
        case FLIP22:
            /* to invert a 2-2 flip, we perform another 2-2 flip, 
               but we need to reorder the vertices. the order of
               the verts in the journal entry are (from the pre-flip
               perspective) (1,2,3,bot,top), with the flip on the 1-2
               edge. To reverse the flip we do
               (bot, top, 3, 2, 1) */
            flip22(mesh,
                   entry->verts[3],
                   entry->verts[4],
                   entry->verts[2],
                   entry->verts[1],
                   entry->verts[0],
                   false);
            break;
        case FLIP32:
            /* to invert a 3-2 flip, perform a 2-3 flip on the same submesh */
            flip23(mesh,
                   entry->verts[0],
                   entry->verts[1],
                   entry->verts[2],
                   entry->verts[3],
                   entry->verts[4],
                   false);
            break;
        case FLIP13:
            /* to invert a 1-3 flip, perform a 3-1 flip on the same submesh */
            flip31(mesh,
                   entry->verts[0],
                   entry->verts[1],
                   entry->verts[2],
                   entry->verts[3],
                   entry->verts[4],
                   false);
            break;
        case FLIP12:
            /* to invert a 1-2 flip, perform a 2-1 flip on the same submesh */
            flip21(mesh,
                   entry->verts[0],
                   entry->verts[1],
                   entry->verts[2],
                   entry->verts[3],
                   entry->verts[4],
                   false);
            break;
        case FLIP14:
            /* to invert a 1-4 flip, perform a 4-1 flip on the same submesh */
            flip41(mesh,
                   entry->verts[0],
                   entry->verts[1],
                   entry->verts[2],
                   entry->verts[3],
                   entry->verts[4],
                   false);
            break;
        case FLIP41:
            /* to invert a 1-4 flip, perform a 4-1 flip on the same submesh */
            flip14(mesh,
                   entry->verts[0],
                   entry->verts[1],
                   entry->verts[2],
                   entry->verts[3],
                   entry->verts[4],
                   false);
            break;
        case INSERTVERTEX:
            /* mark this vertex as dead so it won't be output... */
            ((struct vertextype *) arraypoolforcelookup(&vertexinfo, entry->verts[0]))->kind = DEADVERTEX;            
            /*
            vertexptr = (struct vertex *) proxipooltag2object(vertexpoolptr, entry->verts[0]);
            vertexptr->kind = DEADVERTEX;
            proxipoolfree(mesh->vertexpool, entry->verts[0]);
            */
            /* free the memory from the vertex info structure */
            /*
            arraypoolfreeindex(&vertexinfo, entry->verts[0]);
            */
            break;
        case CLASSIFY:
            /* for now, don't do anything for a classify TODO: should i do something here? */
            break;
        default:
            printf("I don't know the journal entry type %d\n", entry->type);
            starexit(1);
    };
}

/* invert the last numentries journal entries */
void invertjournaltop(struct tetcomplex *mesh,
                      int numentries)
{
    int i;
    struct journalentry *entry;
    
    if (improvebehave.verbosity > 5)
    {
        printf("Inverting the last %d entries of the journal.\n", numentries
        );
    }
    
    for (i=0; i<numentries; i++)
    {
        if (journal->top != STACKEMPTY)
        {
            entry = (struct journalentry *) stackpop(journal);
            invertjournalentry(mesh,entry);
        }
        else
        {
            printf("Journal empty, so not inverting any more entries. Removed %d before bailing.\n", i);
            break;
        }
    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("done.\n");
    }
}

/* invert all the journal entries up to (but not including) the one
   with id 'id' */
void invertjournalupto(struct tetcomplex* mesh,
                       int id)
{
    int i=0;
    struct journalentry *entry;
    
    while(journal->top - i >= 0)
    {
        /* peek at the next on down the stack */
        entry = (struct journalentry *) arraypoolfastlookup(&(journal->pool), (unsigned long) journal->top - i);
        /* if this is the one we're looking for, invert all entries up to it */
        if (entry->id == id)
        {
            invertjournaltop(mesh, i);
            return;
        } 
        
        /* look deeper next time */
        i++;
    }
    
    printf("Error: trying to invert journal up to entry id %d but it wasn't found...\n", id);
    starexit(1);
}

/* figure out number of ops and start, end indices for given id range */
void journalrangetoindex(int startid, 
                        int endid,
                        int *startindex,
                        int *endindex,
                        int *numops)
{
    int i = startid;
    struct journalentry *entry;
    bool foundend = false;
    bool foundstart = false;
    
    /* find the first entry */
    i = 0;
    while (foundend == false || foundstart == false)
    {
        /* peek at the next on down the stack */
        entry = (struct journalentry *) arraypoolfastlookup(&(journal->pool), (unsigned long) journal->top - i);
        
        /* is this the id we're looking for? */
        if (entry->id == startid)
        {
            *startindex = journal->top - i;
            foundstart = true;
        }
        if (entry->id == endid)
        {
            *endindex = journal->top - i;
            foundend = true;
        }
        
        i++;
    }
    
    *numops = (*endindex) - (*startindex);
    
    if (improvebehave.verbosity > 4)
    {
        printf("startindex = %d, endindex = %d, numops = %d\n", *startindex, *endindex, *numops);
    }
}

/* print out a range of journal entries to the specified stream */
void printjournalrangestream(FILE *o, int startindex, int endindex)
{
    int i;
    struct journalentry *entry;
    
    for (i=startindex+1; i<=endindex; i++)
    {
        if (i <= lastjournalentry())
        {
            entry = (struct journalentry *) arraypoolfastlookup(&(journal->pool), (unsigned long) i);
            printjournalentrystream(o, entry, i);
        }
        else
        {
            printf("tried to access journal entry %d, but lastjournalentry() is %d\n", i, lastjournalentry());
            starexit(1);
        }
    }
}

/* print out the top numentries entries from the journal */
void printjournaltop(int numentries)
{
    int i;
    struct journalentry *entry;
    
    printf("The last %d operations on the stack (from newest to oldest):\n", numentries);
    
    for (i=0; i<numentries; i++)
    {
        if ((journal->top - i) >= 0)
        {
            entry = (struct journalentry *) arraypoolfastlookup(&(journal->pool), (unsigned long) journal->top - i);
            printjournalentry(entry);
        }
        else
        {
            printf("Journal empty.\n");
            break;
        }
    }
}

/* find and return the last journal entry that involved vtx */
struct journalentry* findjournalentryvertex(tag vtx, int entryclass)
{
    int i=0, j;
    struct journalentry *entry;
    
    while(journal->top - i >= 0)
    {
        /* peek at the next on down the stack */
        entry = (struct journalentry *) arraypoolfastlookup(&(journal->pool), (unsigned long) journal->top - i);
        
        /* check each of the vertices that this entry affected */
        if (((entry->class == entryclass) || (entryclass == ALLENTRIES)) &&
            (entry->type != DELETETET))
        {
            for (j=0; j<entry->numverts; j++)
            {
                if (entry->verts[j] == vtx)
                {
                    return entry;
                }
            }
        }
        
        i++;
    }
    
    printf("Error: couldn't find an entry relating to vertex %d...\n", (int) vtx);
    starexit(1);
    return NULL;
}

/* given a vertex, return a tet that the vertex lies in.
   this assumes that the vertex has participated in some
   topological change to the mesh */
void findtetfromvertex(tetcomplex *mesh,
                        tag vtx,
                        tag outtet[4])
{
    /* first, locate a journal entry with this vertex */
    struct journalentry* entry = findjournalentryvertex(vtx, TOPOLOGICAL);
    tag tets[4][4]; /* the tets that existed after this entry */
    int numtets;    /* the number of tets that existed after this entry */
    int i,j;
    
    switch (entry->type)
    {
        case INSERTTET:
            /* to get coverage, just need single tet */
            numtets = 1;
            /* (top, 1, 2, bot) */
            tets[0][0] = entry->verts[0];
            tets[0][1] = entry->verts[1];
            tets[0][2] = entry->verts[2];
            tets[0][3] = entry->verts[3];
            
            break;
        case FLIP23:
            /* for a 2-3 flip, the 5 verts are (1,2,3,bot,top) */
            /* to get coverage over all 5 verts, only need 2 tets */
            numtets = 2;
            /* (top, 1, 2, bot) */
            tets[0][0] = entry->verts[4];
            tets[0][1] = entry->verts[0];
            tets[0][2] = entry->verts[1];
            tets[0][3] = entry->verts[3];
            
            /* (top, 2, 3, bot) */
            tets[1][0] = entry->verts[4];
            tets[1][1] = entry->verts[1];
            tets[1][2] = entry->verts[2];
            tets[1][3] = entry->verts[3];
            
            break;
        case FLIP22:
            /* for a 2-2 flip, the 5 verts are (1,2,3,bot,top) */
            /* to get coverage over all 5 verts, only need 2 tets */
            numtets = 2;
            /* (3, 1, top, bot) */
            tets[0][0] = entry->verts[2];
            tets[0][1] = entry->verts[0];
            tets[0][2] = entry->verts[4];
            tets[0][3] = entry->verts[3];
            
            /*  (3, 2, bot, top) */
            tets[1][0] = entry->verts[2];
            tets[1][1] = entry->verts[1];
            tets[1][2] = entry->verts[3];
            tets[1][3] = entry->verts[4];
            
            break;
        case FLIP32:
            /* for a 3-2 flip, the 5 verts are (1,2,3,bot,top) */
            /* to get coverage over all 5 verts, only need 2 tets */
            numtets = 2;
            
            /* (top, 1, 2, bot) */
            tets[0][0] = entry->verts[4];
            tets[0][1] = entry->verts[0];
            tets[0][2] = entry->verts[1];
            tets[0][3] = entry->verts[2];

            /* (top, 2, 3, bot) */
            tets[1][0] = entry->verts[3];
            tets[1][1] = entry->verts[2];
            tets[1][2] = entry->verts[1];
            tets[1][3] = entry->verts[0];

            break;
        case FLIP14:
            /* for a 1-4 flip, the 5 verts are (1,2,3,4,body) */
            /* to get coverage over all 5 verts, only need 2 tets */
            numtets = 2;
            /* (1, 2, 3, b) */
            tets[0][0] = entry->verts[0];
            tets[0][1] = entry->verts[1];
            tets[0][2] = entry->verts[2];
            tets[0][3] = entry->verts[4];
            
            /* (1, 3, 4, b) */
            tets[1][0] = entry->verts[0];
            tets[1][1] = entry->verts[2];
            tets[1][2] = entry->verts[3];
            tets[1][3] = entry->verts[4];
            
            break;
        case FLIP13:
            /* for a 1-3 flip, the 5 verts are (1,2,3,4,face) */
            /* to get coverage over all 5 verts, only need 2 tets */
            numtets = 2;
            
            /* (1, 2, 3, b) */
            tets[0][0] = entry->verts[0];
            tets[0][1] = entry->verts[1];
            tets[0][2] = entry->verts[4];
            tets[0][3] = entry->verts[3];
            
            /* (1, 3, 4, b) */
            tets[1][0] = entry->verts[0];
            tets[1][1] = entry->verts[1];
            tets[1][2] = entry->verts[2];
            tets[1][3] = entry->verts[4];
            
            break;
        case FLIP12:
            /* for a 1-2 flip, the 5 verts are (1,2,3,4,segment) */
            /* to get coverage over all 5 verts, only need 2 tets */
            numtets = 2;
            
            /* (1, 3, 4, s) */
            tets[0][0] = entry->verts[0];
            tets[0][1] = entry->verts[2];
            tets[0][2] = entry->verts[3];
            tets[0][3] = entry->verts[4];
            
            /* (2, 4, 3, s) */
            tets[1][0] = entry->verts[1];
            tets[1][1] = entry->verts[3];
            tets[1][2] = entry->verts[2];
            tets[1][3] = entry->verts[4];
            
            break;
        default:
            numtets = 0;
            printf("In findtetfromvertex I don't know the journal entry type %d\n", entry->type);
            starexit(1);
    }
    
    /* now, find a tet that contains the vertex */
    for (i=0; i<numtets; i++)
    {
        for (j=0; j<4; j++)
        {
            /* if the vertices match */
            if (tets[i][j] == vtx)
            {
                /* permute this tet so vtx is at the start */
                if (j == 0)
                {
                    outtet[0] = tets[i][0];
                    outtet[1] = tets[i][1];
                    outtet[2] = tets[i][2];
                    outtet[3] = tets[i][3];
                }
                else
                {
                    /* one swap (0,j)*/
                    outtet[0] = vtx;
                    outtet[j] = tets[i][0];
                    /* two swaps, same tet */
                    switch (j)
                    {
                        case 1:
                            outtet[3] = tets[i][2];
                            outtet[2] = tets[i][3];
                            break;
                        case 2:
                            outtet[1] = tets[i][3];
                            outtet[3] = tets[i][1];
                            break;
                        case 3:
                            outtet[1] = tets[i][2];
                            outtet[2] = tets[i][1];
                            break;
                    }
                }
                if (improvebehave.verbosity > 5)
                {
                    printf("the vertex is %d\n", (int) vtx);
                    printf("here is the operation:\n");
                    printjournalentry(entry);
                    printf("the tet we found is (%d %d %d %d)\n", (int) tets[i][0], (int) tets[i][1], (int) tets[i][2], (int) tets[i][3]);
                    printf("the tet we're returning is (%d %d %d %d)\n\n\n", (int) outtet[0], (int) outtet[1], (int) outtet[2], (int) outtet[3]);
                }
                assert(outtet[0] == vtx);
                /*
                assert(tetexists(mesh, outtet[0], outtet[1], outtet[2], outtet[3]));
                */
                return;
            }
        }
    }
    printf("wtf?!? we didn't find the vertex.\n");
    printf("the vertex is %d\n", (int) vtx);
    printf("here is the operation:\n");
    printjournalentry(entry);
    starexit(1);
}

/* write out current journal to a file  TODO this is unfinished...*/
void writejournal(struct behavior *b)
{
    char filename[300];
    FILE *outfile;
    int i=0, j;
    struct journalentry *entry;
    tag lverts[5] = {0, 0, 0, 0, 0};
    starreal lold[3] = {0.0, 0.0, 0.0};
    starreal lnew[3] = {0.0, 0.0, 0.0};
    
    /* copy over the input file name */
    strcpy(filename, b->innodefilename);
    
    /* if the filename currently has an extension, clip it off */
    if (filename[strlen(filename) - 5] == '.')
    {
        filename[strlen(filename) - 5] = '\0';
    }
    
    /* mention that this is the journal file */
    strcat(filename, ".journal");
    
    if (!b->quiet) 
    {
        printf("Writing out journal of %ld entries to %s.\n", journal->top, filename);
    }
    
    /* open the output file */
    outfile = fopen(filename, "w");
    if (outfile == (FILE *) NULL) 
    {
        printf("  Error:  Cannot create file %s.\n", filename);
        starexit(1);
    }
    
    /* first line: number of journal entries */
    fprintf(outfile, "%ld\n", journal->top);
    
    /* now print out each journal entry */
    for (i=0; i<=journal->top; i++)
    {
        /* fetch the ith journal entry */
        entry = (struct journalentry *) arraypoolfastlookup(&(journal->pool), (unsigned long) i);
        
        /* copy vertex tags to local with zero padding */
        for (j=0; j<entry->numverts; j++)
        {
            lverts[j] = entry->verts[j];
        }
        
        /* copy new position for smooths and inserts */
        if (entry->type == INSERTVERTEX || entry->type == SMOOTHVERTEX)
        {
            for (j=0; j<3; j++)
            {
                lnew[j] = entry->newpos[j];
            }
        }
        
        /* copy old position for smooths */
        if (entry->type == SMOOTHVERTEX)
        {
            for (j=0; j<3; j++)
            {
                lold[j] = entry->oldpos[j];
            }
        }
        
        /* print out the info for this entry */
        fprintf(outfile, "%d %d %d %d %d %d %d %d %d %g %g %g %g %g %g\n",
                entry->id,
                entry->class,
                entry->type,
                (int) lverts[0], (int) lverts[1], (int) lverts[2], (int) lverts[3], (int) lverts[4],
                entry->numverts,
                lnew[0], lnew[1], lnew[2],
                lold[0], lold[1], lold[2]);
    }
    
    /* close the file */
    fclose(outfile);
}

/* read a journal from a file  TODO unfinished...*/
void readjournal(char *journalfilename,
                 struct arraypoolstack *outjournal)
{
    /* char inputline[INPUTLINESIZE]; */ /* as string of maximum line size */
    /* char *stringptr; */               /* a pointer within the input */
    
    FILE *infile;
    
    /* Read the tetrahedra from an .ele file. */
    printf("Opening journal for reading: %s.\n", journalfilename);
    

    infile = fopen(journalfilename, "r");
    if (infile == (FILE *) NULL) 
    {
        printf("  Error:  Cannot access file %s.\n", journalfilename);
        starexit(1);
    }
}


