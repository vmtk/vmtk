/* functions related to quadric error metrics */

/* compute the quadric error for a query position of a vertex */
starreal quadricerrorquery(struct tetcomplex *mesh,
                           tag vtag,
                           starreal v[3])
{
    struct quadric *q;
    starreal quad;
    
    /* fetch the quadric for this vertex */
    q = (struct quadric *) arraypoolforcelookup(&surfacequadrics, vtag);
    assert(q != NULL);
    
    /* return zero if this is not a surface vertex */
    if (q->hasquadric == false) return 0.0;
    
    /* evaluate quadric */
    /* Q(v) = v'Av + 2b'v + c */
    quad = v[0]*v[0]*q->a2 + 2*v[0]*v[1]*q->ab + 2*v[0]*v[2]*q->ac + 2*v[0]*q->ad
                           + v[1]*v[1]*q->b2   + 2*v[1]*v[2]*q->bc + 2*v[1]*q->bd
                                               + v[2]*v[2]*q->c2   + 2*v[2]*q->cd
                                                                   + q->d2;
    
    /*                                                               
    if (quad > 1.0e-8)
    {
        printf("Encountered a large quadric error of %g\n", quad);
        printf("Quadric:\n");
        printf("    a2 = %g\n", q->a2);
        printf("    ab = %g\n", q->ab);
        printf("    ac = %g\n", q->ac);
        printf("    ad = %g\n", q->ad);
        printf("    b2 = %g\n", q->b2);
        printf("    bc = %g\n", q->bc);
        printf("    bd = %g\n", q->bd);
        printf("    c2 = %g\n", q->c2);
        printf("    cd = %g\n", q->cd);
        printf("    d2 = %g\n", q->d2);
        printf("Vertex:\n");
        printf("    x = %g\n", v[0]);
        printf("    y = %g\n", v[1]);
        printf("    z = %g\n", v[2]);
    }
    assert(quad < 1.0e-8);
    */
                                                                   
    return quad;
}

/* compute the quadric error for a particular vertex's current position */
starreal quadricerror(struct tetcomplex *mesh,
                      tag vtag)
{
    /* fetch this vertex's current position */
    starreal *v = ((struct vertex *) tetcomplextag2vertex(mesh, vtag))->coord;
    
    return quadricerrorquery(mesh, vtag, v);
}

/* compute the quadric error at a vertex, normalized to
   be comparable to tetrahedron quality measures */
starreal quadricerrortet(struct tetcomplex *mesh,
                         tag vtag)
{
    /* return the quadric error of this vertex scaled
       by the quadric scale and offset by the quadric offset */
    starreal qe = improvebehave.quadricoffset - 
                  (improvebehave.quadricscale * quadricerror(mesh, vtag));
    
    /* scale down so that perfect corresponds to equilateral */
    if (improvebehave.qualmeasure == QUALMINSINE ||
        improvebehave.qualmeasure == QUALWARPEDMINSINE)
    {
        qe *= SINEEQUILATERAL;
    }
    
    /* don't return negative qualities */
    if (qe < 0.0) return 0.0;
    return qe;
}

bool hasquadric(tag vtag)
{
    struct quadric *q = (struct quadric *) arraypoolforcelookup(&surfacequadrics, vtag);
    assert(q != NULL);
    return q->hasquadric;
}

/* compute the gradient of the quadric error for query point */
void quadricgradquery(struct tetcomplex *mesh,
                      tag vtag,
                      starreal v[3],
                      starreal grad[3])
{
    struct quadric *q;
    
    /* fetch the quadric for this vertex */
    q = (struct quadric *) arraypoolforcelookup(&surfacequadrics, vtag);
    assert(q != NULL);
    
    /* return a zero gradient if this is not a surface vertex */
    if (q->hasquadric == false)
    {
        grad[0] = grad[1] = grad[2] = 0.0;
        return;
    }
    
    /* grad(Q) = 2Av + 2b */
    /* A = nn', b = dn */
    /* so */
    /* grad(Q) = 2 [xa2 + yab + zac + ad] */
    /*             [xab + yb2 + zbc + bd] */
    /*             [xac + ybc + zc2 + cd] */
    /* negative because we want to *reduce* quadric error... */
    grad[0] = -2.0 * (v[0]*q->a2 + v[1]*q->ab + v[2]*q->ac + q->ad);
    grad[1] = -2.0 * (v[0]*q->ab + v[1]*q->b2 + v[2]*q->bc + q->bd);
    grad[2] = -2.0 * (v[0]*q->ac + v[1]*q->bc + v[2]*q->c2 + q->cd);
    
    /* scale gradient by quadric scale factor */
    /*
    grad[0] *= improvebehave.quadricscale;
    grad[1] *= improvebehave.quadricscale;
    grad[2] *= improvebehave.quadricscale;
    */
    return;
}

/* compute the gradient of the quadric error for a vertex */
void quadricgrad(struct tetcomplex *mesh,
                 tag vtag,
                 starreal grad[3])
{
    starreal *v;
    
    /* fetch this vertex's position */
    v = ((struct vertex *) tetcomplextag2vertex(mesh, vtag))->coord;
    
    /* compute the gradient for the vertex's current position */
    quadricgradquery(mesh, vtag, v, grad);
    
    return;
}

/* compute the gradient of the quadric error, scaled for tet comparison */
void quadricgradtet(struct tetcomplex* mesh,
                    tag vtag,
                    starreal grad[3])
{
    quadricgrad(mesh, vtag, grad);
    
    /* vscale(improvebehave.quadricscale, grad, grad); */
}

/* add a quadric for a newly inserted vertex */
bool addquadric(struct tetcomplex *mesh,
                tag vtag,
                tag faces[][3],
                int numfaces)
{
    int i,j;
    proxipool *pool = mesh->vertexpool;
    struct vertex *newv, *vptr[3];
    struct quadric *vquads[3];
    starreal e1[3], e2[3], normal[3];
    starreal facearea, d, normfactor;
    /* create quadric */
    struct quadric *q = (struct quadric *) arraypoolforcelookup(&surfacequadrics, vtag);
    
    /* initialize quadric */
    q->hasquadric = true;
    
    q->a2 = q->ab = q->ac = q->ad 
          = q->b2 = q->bc = q->bd 
                  = q->c2 = q->cd 
                          = q->d2 = 0.0;
    
    q->numfaces = 0;
    q->facesum = 0.0;
    q->edge2harm = 0.0;
    
    newv = (struct vertex *) proxipooltag2object(pool, vtag);
    vcopy(newv->coord, q->origpos);
    
    /* accumulate fundamental quadric for each incident face */
    for (i=0; i<numfaces; i++)
    {
        /* get the actual vertices */
        vptr[0] = (struct vertex *) proxipooltag2object(pool, faces[i][0]);
        vptr[1] = (struct vertex *) proxipooltag2object(pool, faces[i][1]);
        vptr[2] = (struct vertex *) proxipooltag2object(pool, faces[i][2]);
        
        /* get original vertex positions */
        vquads[0] = (struct quadric *) arraypoolforcelookup(&surfacequadrics, faces[i][0]);
        vquads[1] = (struct quadric *) arraypoolforcelookup(&surfacequadrics, faces[i][1]);
        vquads[2] = (struct quadric *) arraypoolforcelookup(&surfacequadrics, faces[i][2]);
        
        /* compute face normal */
        /*
        vsub(vptr[1]->coord, vptr[0]->coord, e1);
        vsub(vptr[2]->coord, vptr[0]->coord, e2);
        */
        vsub(vquads[1]->origpos, vquads[0]->origpos, e1);
        vsub(vquads[2]->origpos, vquads[0]->origpos, e2);
        cross(e1, e2, normal);
        
        /* face area is 1/2 the length of the cross product */
        facearea = vlength(normal) / 2.0;
        if (facearea < 1.e-14 && 0)
        {
            printf("In addquadric, face (%d %d %d) has odd area %g.\n", (int) faces[i][0], (int) faces[i][1], (int) faces[i][2], facearea);
            for (j=0; j<3; j++)
            {
                printf("    %d = (%g %g %g)\n", j, vquads[j]->origpos[0], vquads[j]->origpos[1], vquads[j]->origpos[2]);
            }
        }
        
        /* normalize the normal */
        vscale(1.0 / (facearea * 2.0), normal, normal);
        
        /* compute the orthogonal distance from the plane of this face to
           the origin */
        d = -dot(normal, vptr[0]->coord);
        
        q->numfaces++;
        q->facesum += facearea;
        /* add on 1/2 of 1 / l^2, because every edge will be counted twice */
        if (vlength(e1) > 0.0 && vlength(e2) > 0.0)
        {
            q->edge2harm += (0.5 / (vlength(e1) * vlength(e1)));
            q->edge2harm += (0.5 / (vlength(e2) * vlength(e2)));
        }
        
        /* accumulate the fundamental quadric from this face */
        /* normal = [a b c] */
        q->a2 += normal[0] * normal[0] * facearea;
        q->ab += normal[0] * normal[1] * facearea;
        q->ac += normal[0] * normal[2] * facearea;
        q->ad += normal[0] * d * facearea;
        q->b2 += normal[1] * normal[1] * facearea;
        q->bc += normal[1] * normal[2] * facearea;
        q->bd += normal[1] * d * facearea;
        q->c2 += normal[2] * normal[2] * facearea;
        q->cd += normal[2] * d * facearea;
        q->d2 += d * d * facearea;
    }
    
    /* compute normalization */
    /* quadric must have at least 3 surrounding faces */
    assert(q->numfaces >= 3);
    
    /* compute harmonic mean */
    q->edge2harm = ((starreal) q->numfaces) / q->edge2harm;
    
    /* compute normalization factor */
    normfactor = q->edge2harm * q->facesum;
    
    /* if the facesum is zero, bail */
    if (q->facesum <= 0.0)
    {
        return false;
    }
    
    assert(q->edge2harm != 0.0);
    assert(q->facesum != 0.0);
    assert(normfactor != 0.0);
    
    /* scale quadric by normalization factor */
    q->a2 /= normfactor;
    q->ab /= normfactor;
    q->ac /= normfactor;
    q->ad /= normfactor;
    q->b2 /= normfactor;
    q->bc /= normfactor;
    q->bd /= normfactor;
    q->c2 /= normfactor;
    q->cd /= normfactor;
    q->d2 /= normfactor;
    
    return true;
}

/* normalize quadric by dividing by the sum of the face areas
   and the harmonic mean of the edge lengths squared */
void normalizequadrics(struct tetcomplex *mesh)
{
    struct quadric *q;
    tag vertextag;
    starreal normfactor; /* normalization factor */
    
    /* check each quadric */
    proxipool *pool = mesh->vertexpool;
    vertextag = proxipooliterate(pool, NOTATAG);
    while (vertextag != NOTATAG)
    {
        /* get the quadric for this vertex */
        q = (struct quadric *) arraypoolforcelookup(&surfacequadrics, vertextag);
        assert(q != NULL);
        
        /* if this isn't a surface vertex, move on */
        if (q->hasquadric == false)
        {
            /* move to next vertex */
            vertextag = proxipooliterate(pool, vertextag);
            continue;
        }
        
        /* normalize this vertex's quadric */
        if (improvebehave.verbosity > 5)
        {
            printf("Normalizing quadric for vertex %d\n", (int) vertextag);
            printf("    Quadric has %d incident faces\n", q->numfaces);
        }
        /* quadric must have at least 3 surrounding faces */
        assert(q->numfaces >= 3);
        
        /* compute harmonic mean */
        q->edge2harm = ((starreal) q->numfaces) / q->edge2harm;
        
        /* compute normalization factor */
        normfactor = q->edge2harm * q->facesum;
        
        if (improvebehave.verbosity > 5)
        {
            printf("    Edge length harmonic mean is %g\n", q->edge2harm);
            printf("    Face area sum is %g\n", q->facesum);
            printf("    Norm factor is %g\n", normfactor);
            printf("Quadric:\n");
            printf("    a2 = %g\n", q->a2);
            printf("    ab = %g\n", q->ab);
            printf("    ac = %g\n", q->ac);
            printf("    ad = %g\n", q->ad);
            printf("    b2 = %g\n", q->b2);
            printf("    bc = %g\n", q->bc);
            printf("    bd = %g\n", q->bd);
            printf("    c2 = %g\n", q->c2);
            printf("    cd = %g\n", q->cd);
            printf("    d2 = %g\n", q->d2);
        }
        
        assert(normfactor != 0.0);
        
        /* scale quadric by normalization factor */
        q->a2 /= normfactor;
        q->ab /= normfactor;
        q->ac /= normfactor;
        q->ad /= normfactor;
        q->b2 /= normfactor;
        q->bc /= normfactor;
        q->bd /= normfactor;
        q->c2 /= normfactor;
        q->cd /= normfactor;
        q->d2 /= normfactor;
        
        vertextag = proxipooliterate(pool, vertextag);
    }
}

/* create quadrics for all surface vertices */ 
void collectquadrics(tetcomplex *mesh)
{
    struct arraypool facepool;
    int numfaces = 0;
    int i,j;
    struct vertex *vptr[3];
    tag vertextag;
    tag *face;
    starreal normal[3], e1[3], e2[3], facearea, d;
    struct quadric *q;
    proxipool *pool = mesh->vertexpool;
    
    /* initialize the arraypool that stores the quadrics */
    arraypoolinit(&surfacequadrics, sizeof(struct quadric), LOG2TETSPERSTACKBLOCK, 0);
    
    /* allocate pool for faces */
    arraypoolinit(&(facepool), sizeof(tag)*3, LOG2TETSPERSTACKBLOCK, 0);
    /* find the surface faces */
    getsurface(mesh, &facepool, &numfaces);
    
    /* initialize all vertices to have no quadric */
    vertextag = proxipooliterate(pool, NOTATAG);
    while (vertextag != NOTATAG)
    {
        /* initialize this vertex to have no quadric */
        ((struct quadric *) arraypoolforcelookup(&surfacequadrics, vertextag))->hasquadric = false;
        
        /* move to next vertex */
        vertextag = proxipooliterate(pool, vertextag);
    }
    
    /* accumulate quadrics from each face to their vertices */
    for (i=0; i<numfaces; i++)
    {
        /* fetch this face from the pool */
        face = (tag *) arraypoolfastlookup(&facepool, (unsigned long) i);
        
        /* get the actual vertices */
        vptr[0] = (struct vertex *) proxipooltag2object(pool, face[0]);
        vptr[1] = (struct vertex *) proxipooltag2object(pool, face[1]);
        vptr[2] = (struct vertex *) proxipooltag2object(pool, face[2]);
        
        assert(vptr[0] != NULL);
        assert(vptr[1] != NULL);
        assert(vptr[2] != NULL);
        
        /*
        printf("here are the vertex positions at the start\n");
        printf("vptr[0]->coord is %p, vptr[0]->coord is (%g %g %g)\n", vptr[0]->coord, vptr[0]->coord[0], vptr[0]->coord[1], vptr[0]->coord[2]);
        printf("vptr[1]->coord is %p, vptr[1]->coord is (%g %g %g)\n", vptr[1]->coord, vptr[1]->coord[0], vptr[1]->coord[1], vptr[1]->coord[2]);
        printf("vptr[2]->coord is %p, vptr[2]->coord is (%g %g %g)\n", vptr[2]->coord, vptr[2]->coord[0], vptr[2]->coord[1], vptr[1]->coord[2]);
        printf("vptr is %p\n", vptr[0]);
        */
        
        /* compute face normal */
        vsub(vptr[1]->coord, vptr[0]->coord, e1);
        vsub(vptr[2]->coord, vptr[0]->coord, e2);
        cross(e1, e2, normal);
        
        /* face area is 1/2 the length of the cross product */
        facearea = vlength(normal) / 2.0;
        
        /* normalize the normal */
        vscale(1.0 / (facearea * 2.0), normal, normal);
        
        /* compute the orthogonal distance from the plane of this face to
           the origin */
        d = -dot(normal, vptr[0]->coord);
        
        /* accumulate the quadric at each of the face's vertices */
        for (j=0; j<3; j++)
        {
            /* fetch the quadric */
            q = (struct quadric *) arraypoolforcelookup(&surfacequadrics, face[j]);
            assert(q != NULL);
            
            /* if it is not set, initialize it */
            if (q->hasquadric == false)
            {
                q->hasquadric = true;
                
                q->a2 = q->ab = q->ac = q->ad 
                      = q->b2 = q->bc = q->bd 
                              = q->c2 = q->cd 
                                      = q->d2 = 0.0;
                
                q->numfaces = 0;
                q->facesum = 0.0;
                q->edge2harm = 0.0;
                
                q->origpos[0] = vptr[j]->coord[0];
                q->origpos[1] = vptr[j]->coord[1];
                q->origpos[2] = vptr[j]->coord[2];
                
                /*
                vcopy(vptr[j]->coord, q->origpos);
                */
            } 
            
            q->numfaces++;
            q->facesum += facearea;
            /* add on 1/2 of 1 / l^2, because every edge will be counted twice */
            q->edge2harm += (0.5 / (vlength(e1) * vlength(e1)));
            q->edge2harm += (0.5 / (vlength(e2) * vlength(e2)));
            
            /* accumulate the fundamental quadric from this face */
            /* normal = [a b c] */
            q->a2 += normal[0] * normal[0] * facearea;
            q->ab += normal[0] * normal[1] * facearea;
            q->ac += normal[0] * normal[2] * facearea;
            q->ad += normal[0] * d * facearea;
            q->b2 += normal[1] * normal[1] * facearea;
            q->bc += normal[1] * normal[2] * facearea;
            q->bd += normal[1] * d * facearea;
            q->c2 += normal[2] * normal[2] * facearea;
            q->cd += normal[2] * d * facearea;
            q->d2 += d * d * facearea;
        } 
    }
    
    /* now, go through quadrics again to normalize them */
    normalizequadrics(mesh);
}

/* do a bunch of checks on quadrics */
void checkquadricsstream(FILE *o, tetcomplex *mesh)
{
    tag vertextag;
    starreal avgquad = 0.0;
    starreal minquad = HUGEFLOAT;
    starreal maxquad = 0.0;
    starreal thisquad;
    starreal avggrad = 0.0;
    starreal mingrad = HUGEFLOAT;
    starreal maxgrad = 0.0;
    starreal thisdot;
    starreal avgdot = 0.0;
    starreal maxdot = 0.0;
    starreal mindot = HUGEFLOAT;
    starreal thisgrad;
    starreal offset[] = {-10.0, -10.0, -10.0};
    starreal correctgrad[] = {1.0, 1.0, 1.0};
    starreal testpos[3];
    starreal grad[3];
    starreal normgrad[3];
    struct quadric *q;
    int numquads = 0;
    
    /* check each quadric */
    proxipool *pool = mesh->vertexpool;
    vertextag = proxipooliterate(pool, NOTATAG);
    while (vertextag != NOTATAG)
    {
        /* get the quadric error for this vertex */
        thisquad = quadricerror(mesh, vertextag);
        quadricgrad(mesh, vertextag, grad);
        thisgrad = vlength(grad);
        
        q = (struct quadric *) arraypoolforcelookup(&surfacequadrics, vertextag);
        assert(q != NULL);
        
        if (q->hasquadric == false)
        {
            /* move to next vertex */
            vertextag = proxipooliterate(pool, vertextag);
            
            continue;
        }
        
        /* offset the vertex to a new position */
        vadd(q->origpos, offset, testpos);
        /* compute the gradient for this new position */
        quadricgradquery(mesh, vertextag, testpos, grad);
        vscale(1.0 / vlength(grad), grad, normgrad);
        /* compute the dot product of this with the expected grad */
        thisdot = dot(correctgrad, normgrad);
        
        
        /*
        if (vertextag < 100)
        {
            printf("Testing quadric gradient for vertex %d.\n", (int) vertextag);
            printf("    Original position is (%g %g %g)\n", q->origpos[0], q->origpos[1], q->origpos[2]);
            printf("    The offset position is (%g %g %g)\n", testpos[0], testpos[1], testpos[2]);
            printf("    The gradient is (%g %g %g)\n", grad[0], grad[1], grad[2]);
            printf("    The normalized gradient is (%g %g %g)\n", normgrad[0], normgrad[1], normgrad[2]);
        }
        */
        
        /* accumulate statistics */
        if (thisquad != 0.0)
        {
            avgquad += thisquad;
            avggrad += thisgrad;
            avgdot += thisdot;
            numquads++;
        }
        
        if (thisquad > maxquad) maxquad = thisquad;
        if (thisquad < minquad) minquad = thisquad;
        if (thisgrad > maxgrad) maxgrad = thisgrad;
        if (thisgrad < mingrad) mingrad = thisgrad;
        if (thisdot > maxdot) maxdot = thisdot;
        if (thisdot < mindot) mindot = thisdot;
        
        
        /* move to next vertex */
        vertextag = proxipooliterate(pool, vertextag);
    }
    
    avgquad /= (starreal) numquads;
    avggrad /= (starreal) numquads;
    avgdot /= (starreal) numquads;
    
    fprintf(o, "Quadriccheck:\n");
    fprintf(o, "    Average quadric error: %g\n", avgquad);
    fprintf(o, "    Number non-zero:       %d\n", numquads);
    fprintf(o, "    Minimum quadric error: %g\n", minquad);
    fprintf(o, "    Maximum quadric error: %g\n", maxquad);
    fprintf(o, "    Average gradient mag:  %g\n", avggrad);
    fprintf(o, "    Minimum gradient mag:  %g\n", mingrad);
    fprintf(o, "    Maximum gradient mag:  %g\n", maxgrad);
    fprintf(o, "    Average dot mag:       %g\n", avgdot);
    fprintf(o, "    Minimum dot mag:       %g\n", mindot);
    fprintf(o, "    Maximum dot mag:       %g\n", maxdot);
}

void checkquadrics(tetcomplex *mesh)
{
    checkquadricsstream(stdout, mesh);
}
