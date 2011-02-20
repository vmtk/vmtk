/*****************************************************************************/
/*                                                                           */
/*  Routines to classify vertices                                            */
/*                                                                           */
/*****************************************************************************/

/* given a boundary vertex and a list of all of it's
   incident tets, determine if it lies in an input facet */
bool facetvertex(struct tetcomplex *mesh,
                 tag vtx,
                 tag incidenttets[][4],
                 int numincident,
                 starreal normal[3])
{
     bool foundface=false;    /* have we encountered an incident boundary face yet? */
     tag boundfacelist[4][3]; /* list of boundary faces for a particular tet */
     int numboundfaces;       /* number of boundary faces  for a tet */
     starreal refnorm[3] = {0.0, 0.0, 0.0};     /* "reference" normal of first face */
     starreal curnorm[3];     /* the current face's normal */
     starreal e1[3], e2[3];   /* edges for forming cross products */
     starreal *vc1, *vc2, *vc3; /* coordinates of the face vertices */
     starreal dotprod=0.0;        /* dot product of current and reference normal */
     int numfaces=0;            /* total coplanar faces surrounding vertex */
     int i,j;
     
     /* first, check to see if we lie in a facet. to do this, we need
        to build a list of all the boundary faces incident to this vertex.
        this is the same as getting all of the boundary faces of all incident
        tets that include this vertex. */
     
     if (improvebehave.verbosity > 5)
     {
         printf("**** Starting search  for incident boundary faces of vertex %d ****\n", (int) vtx);
         printf("here is what the area around the vertex looks like:\n");
         printtets(mesh, incidenttets, numincident);
     }
     
     /* for each incident tet */
     for (i=0; i<numincident; i++)
     {
         /* collect all the boundary faces */
         if (boundfaces(mesh, 
                        incidenttets[i][0],
                        incidenttets[i][1],
                        incidenttets[i][2],
                        incidenttets[i][3],
                        boundfacelist,
                        &numboundfaces))
         {
             if (improvebehave.verbosity > 5)
             {
                 printf("incident tet %d/%d had %d boundary faces\n", i+1, numincident, numboundfaces);
             }
             
             /* this tet had at least one boundary face. do any
                of it's boundary faces contain the vertex in
                question? */
             for (j=0; j<numboundfaces; j++)
             {
                 if ((boundfacelist[j][0] == vtx) ||
                     (boundfacelist[j][1] == vtx) ||
                     (boundfacelist[j][2] == vtx))
                 {
                     /* this face is incident to the vertex in question. */
                     numfaces++;
                     
                     /* fetch the coordinates of the face vertices */
                     vc1 = ((struct vertex *) tetcomplextag2vertex(mesh, boundfacelist[j][0]))->coord;
                     vc2 = ((struct vertex *) tetcomplextag2vertex(mesh, boundfacelist[j][1]))->coord;
                     vc3 = ((struct vertex *) tetcomplextag2vertex(mesh, boundfacelist[j][2]))->coord;
                     
                     /* form two edge vectors for the face */
                     vsub(vc2, vc1, e1);
                     vsub(vc3, vc1, e2);
                     
                     /* find the (unit-length) face normal of this face */
                     cross(e1, e2, curnorm);
                     vscale((1.0 / vlength(curnorm)),curnorm, curnorm);
                     
                     /* if this is the first face we have found, establish it's
                        normal as the "reference normal" which future faces
                        will be tested against for coplanarity */
                     if (foundface == false)
                     {
                         foundface = true;
                         vcopy(curnorm, refnorm);
                     }
                     else
                     {
                        /* is the dot product of this normal within
                           COPLANARTOL of the reference normal? */
                        dotprod = dot(curnorm, refnorm);
                        
                        if ((dotprod > (1.0 + COPLANARTOL)) || (dotprod < (1.0 - COPLANARTOL)))
                        {
                            if (improvebehave.verbosity > 5)
                            {
                                printf("Vertex is not facet because dotprod = %g\n", dotprod);
                            }
                            
                            /* this new normal is not close enough to coplanar.
                               we know that this vertex cannot be a facet vertex */
                            return false;
                        }
                     }
                 }
             }
         }
         else
         {
             if (improvebehave.verbosity > 5)
             {
                 printf("the #%d/%d incident tet had no boundary faces\n", i+1, numincident);
             }
         }
     }
     
     /* if we get here having seen at least 3 coplanar faces, must be facet vertex */
     if (numfaces >= 3)
     {
         if (improvebehave.verbosity > 5)
         {
             printf("Found a face lying in an input facet surrounded by %d coplanar surface faces (last dotprod = %g).\n", numfaces, dotprod);
         }
         vcopy(refnorm,normal);
         return true;
     }
     
     if (improvebehave.verbosity > 5)
     {
         printf("found coplanar faces but only %d of them\n", numfaces);
     }
     return false;
}

/* given a boundary vertex and a list of all of it's
   incident tets, determine if it lies in an input segment */
bool segmentvertex(struct tetcomplex *mesh,
                    tag vtx,
                    tag incidenttets[][4],
                    int numincident,
                    starreal edge[3])
{
      int foundfaces=0;        /* how many incident boundary face so far? */
      tag boundfacelist[4][3]; /* list of boundary faces for a particular tet */
      int numboundfaces;       /* number of boundary faces  for a tet */
      starreal refnorm1[3] = {0.0, 0.0, 0.0};    /* "reference" normal of first face */
      starreal refnorm2[3] = {0.0, 0.0, 0.0};    /* the other reference plane */
      starreal curnorm[3];     /* the current face's normal */
      starreal e1[3], e2[3];   /* edges for forming cross products */
      starreal *vc1, *vc2, *vc3; /* coordinates of the face vertices */
      starreal dotprod=0.0;        /* dot product of current and reference normal */
      int numfaces=0;            /* total coplanar faces surrounding vertex */
      int i,j;

      /* first, check to see if we lie in a facet. to do this, we need
         to build a list of all the boundary faces incident to this vertex.
         this is the same as getting all of the boundary faces of all incident
         tets that include this vertex. */

      if (improvebehave.verbosity > 5)
      {
          printf("**** Starting search  for incident boundary faces of vertex %d ****\n", (int) vtx);
          printf("here is what the area around the vertex looks like:\n");
          printtets(mesh, incidenttets, numincident);
      }

      /* for each incident tet */
      for (i=0; i<numincident; i++)
      {
          /* collect all the boundary faces */
          if (boundfaces(mesh, 
                         incidenttets[i][0],
                         incidenttets[i][1],
                         incidenttets[i][2],
                         incidenttets[i][3],
                         boundfacelist,
                         &numboundfaces))
          {
              if (improvebehave.verbosity > 5)
              {
                  printf("incident tet %d/%d had %d boundary faces\n", i+1, numincident, numboundfaces);
              }

              /* this tet had at least one boundary face. do any
                 of it's boundary faces contain the vertex in
                 question? */
              for (j=0; j<numboundfaces; j++)
              {
                  if ((boundfacelist[j][0] == vtx) ||
                      (boundfacelist[j][1] == vtx) ||
                      (boundfacelist[j][2] == vtx))
                  {
                      /* this face is incident to the vertex in question. */
                      numfaces++;

                      /* fetch the coordinates of the face vertices */
                      vc1 = ((struct vertex *) tetcomplextag2vertex(mesh, boundfacelist[j][0]))->coord;
                      vc2 = ((struct vertex *) tetcomplextag2vertex(mesh, boundfacelist[j][1]))->coord;
                      vc3 = ((struct vertex *) tetcomplextag2vertex(mesh, boundfacelist[j][2]))->coord;

                      /* form two edge vectors for the face */
                      vsub(vc2, vc1, e1);
                      vsub(vc3, vc1, e2);

                      /* find the (unit-length) face normal of this face */
                      cross(e1, e2, curnorm);
                      vscale((1.0 / vlength(curnorm)),curnorm, curnorm);

                      /* if this is the first face we have found, establish it's
                         normal as the "reference normal" which future faces
                         will be tested against for coplanarity */
                      if (foundfaces == 0)
                      {
                          foundfaces = 1;
                          vcopy(curnorm, refnorm1);
                      }
                      else
                      {
                         /* is the dot product of this normal within
                            COPLANARTOL of the first reference normal? */
                         dotprod = dot(curnorm, refnorm1);

                         /* do dot products match for first normal? */
                         if ((dotprod > (1.0 + COPLANARTOL)) || (dotprod < (1.0 - COPLANARTOL)))
                         {
                             /* nope. is there a second reference yet? */
                             if (foundfaces == 1)
                             {
                                 /* no second reference yet. make this it */
                                 foundfaces = 2;
                                 vcopy(curnorm, refnorm2);
                             }
                             else
                             {
                                 dotprod = dot(curnorm, refnorm2);
                                 if ((dotprod > (1.0 + COPLANARTOL)) || (dotprod < (1.0 - COPLANARTOL)))
                                 {
                                     if (improvebehave.verbosity > 5)
                                     {
                                         printf("curnorm = %g %g %g wasn't close enough to refnorm1 %g %g %g or refnorm2 %g %g %g\n", curnorm[0], 
                                                                                                                                      curnorm[1], 
                                                                                                                                      curnorm[2],
                                                                                                                                      refnorm1[0],
                                                                                                                                      refnorm1[1],
                                                                                                                                      refnorm1[2],
                                                                                                                                      refnorm1[0],
                                                                                                                                      refnorm1[1],
                                                                                                                                      refnorm1[2]);
                                     }
                                     /* this normal doesn't match either of the references */
                                     return false;
                                 }
                             }
                         }
                      }
                  }
              }
          }
          else
          {
              if (improvebehave.verbosity > 5)
              {
                  printf("the #%d/%d incident tet had no boundary faces\n", i+1, numincident);
              }
          }
      }

      /* if we get here having seen at least 3 faces, must be segment vertex */
      if (numfaces >= 3)
      {
          if (improvebehave.verbosity > 5)
          {
              printf("Found a vertex lying in an input segment surrounded by %d surface faces (last dotprod = %g).\n", numfaces, dotprod);
              /*
              printtets(mesh, incidenttets, numincident);
              */
          }
          /* the segment that this vertex lies on is perpendicular to both reference normals */
          cross(refnorm1, refnorm2, edge);
          return true;
      }
      return false;
}

/* given a tet, classifies each of its vertices according to how many
   degrees of freedom it has. TODO: The starter code was copied from boundverts(),
   could these two be combined? */
void tetvertexclassify(struct tetcomplex *mesh,
                       tag v1,
                       tag v2,
                       tag v3,
                       tag v4)
{
    int i,j,k,l;   /* loop indices */
    bool noghosts; /* any ghost tets incident to this vertex? */
    tag tet[4];
    int numincident;               /* number of incident tets */
    starreal facetnormal[3];
    starreal segmentedge[3];
    struct vertextype* info;
    struct vertextype* vptr;
    tag inverts[1];
    
    /* a list of all the tets incident to this vertex */
    tag incidenttettags[MAXINCIDENTTETS][4];
    
    tet[0] = v1;
    tet[1] = v2;
    tet[2] = v3;
    tet[3] = v4;
    
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
        
        /* check whether this vertex has already been classified */
        vptr = (struct vertextype *) arraypoollookup(&vertexinfo, tet[i]);
        assert(vptr != NULL);
        /* it's not null. is it zero (unset?) */
        if (vptr->kind != INPUTVERTEX)
        {
            /* we've already classified this vertex, move on */
            if (improvebehave.verbosity > 5)
            {
                printf("skipping reclassification of vertex %d\n", (int) tet[i]);
            }
            continue;
        }
        
        if (improvebehave.verbosity > 5)
        {
            printf("actually trying to classify vertex %d\n", (int) tet[i]);
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
        if (noghosts)
        {
            /* this vertex is in the interior of the mesh and so is free
               to move in three dimensions. Assign it the type FREEVERTEX */
            ((struct vertextype *) arraypoolforcelookup(&vertexinfo, tet[i]))->kind = FREEVERTEX;
            
            /* save this assignment in the journal */
            inverts[0] = tet[i];
            insertjournalentry(mesh, CLASSIFY, inverts, FREEVERTEX, NULL, facetnormal);
        }
        else
        {
            /* this vertex could still have freedom to move if it lies in a
               facet or along a segment. first, is it in a facet? */
            if (facetvertex(mesh, tet[i], incidenttettags, numincident, facetnormal))
            {
                /* mark this vertex as a facet and save the facet normal */
                info = (struct vertextype *) arraypoolforcelookup(&vertexinfo, tet[i]);
                info->kind = FACETVERTEX;
                vcopy(facetnormal,info->vec);
                
                /* save this assignment in the journal */
                inverts[0] = tet[i];
                insertjournalentry(mesh, CLASSIFY, inverts, FACETVERTEX, NULL, facetnormal);
            }
            else
            {
                /* check to see if this is a segment vertex */
                if (segmentvertex(mesh, tet[i], incidenttettags, numincident, segmentedge))
                {
                    /* mark this vertex as a segment vertex */
                    info = (struct vertextype *) arraypoolforcelookup(&vertexinfo, tet[i]);
                    info->kind = SEGMENTVERTEX;
                    vcopy(segmentedge,info->vec);
                    
                    /* save this assignment in the journal */
                    inverts[0] = tet[i];
                    insertjournalentry(mesh, CLASSIFY, inverts, SEGMENTVERTEX, NULL, segmentedge);
                }
                /* this vertex can't go anywhere */
                else
                {
                    info = (struct vertextype *) arraypoolforcelookup(&vertexinfo, tet[i]);
                    info->kind = FIXEDVERTEX;
                    
                    /* save this assignment in the journal */
                    inverts[0] = tet[i];
                    insertjournalentry(mesh, CLASSIFY, inverts, FIXEDVERTEX, NULL, segmentedge);
                }
            }
        }
        
    }
}

/* classify the number of degrees of freedom for all tets in the mesh */
void classifyvertices(struct tetcomplex *mesh)
{
    struct arraypoolstack tetstack;   /* stack of tets  */
    struct arraypoolstack localstack; /* local stack of tets that includes only one per vertex */
    struct improvetet * stacktet;     /* tet we're pulling off the stack */
    struct improvetet * localtet;
    int i;
    starreal minqual, meanqual[NUMMEANTHRESHOLDS];
    
    stackinit(&tetstack, sizeof(struct improvetet));
    
    /* fill the stack of tets with all tets in the mesh */
    fillstackqual(mesh, &tetstack, improvebehave.qualmeasure, HUGEFLOAT, meanqual, &minqual);
    
    /* initialize the tet stack */
    stackinit(&localstack, sizeof(struct improvetet));
    
    /* try to pop all the tets off the stack */
    while (tetstack.top != STACKEMPTY)
    {
        /* pull the top tet off the stack */
        stacktet = (struct improvetet *) stackpop(&tetstack);
        
        if (improvebehave.verbosity > 5 && localstack.top % 10000 == 0)
        {
            printf("tetstack size now %lu, localstack size is %lu\n", tetstack.top, localstack.top);
        }

        /* push this tet on the local stack */
        localtet = (struct improvetet *) stackpush(&localstack);
        /* copy tet over */
        localtet->verts[0] = stacktet->verts[0];
        localtet->verts[1] = stacktet->verts[1];
        localtet->verts[2] = stacktet->verts[2];
        localtet->verts[3] = stacktet->verts[3];
        localtet->quality = stacktet->quality;
        
        /* force initialization of vertex type for all vertices of this tet to INPUTVERTEX */
        for (i=0; i<4; i++)
        {
            ((struct vertextype *) arraypoolforcelookup(&vertexinfo, stacktet->verts[i]))->kind = INPUTVERTEX;
        }
    }
   
    if (improvebehave.verbosity > 4)
    {
        printf("Done creating initial stack of verts for classification of size %lu\n", localstack.top);
    }
 
    /* go through each tet on the stack */
    while (localstack.top != STACKEMPTY)
    {
        /* pull the top tet off the stack */
        stacktet = (struct improvetet *) stackpop(&localstack);
        
        if (improvebehave.verbosity > 5 && localstack.top % 10000 == 0)
        {
            printf("localstack emptying, now size %lu\n", localstack.top);
        }

        /* assign freedom for vertices in this tet */
        tetvertexclassify(mesh,
                          stacktet->verts[0], 
                          stacktet->verts[1], 
                          stacktet->verts[2], 
                          stacktet->verts[3]);
    }
}

