/*****************************************************************************/
/*                                                                           */
/*  smoothing functions                                                      */
/*                                                                           */
/*****************************************************************************/

/* given two values a and b and their gradients, compute the 
   gradient of their product grad(a*b) */
void gradproduct(starreal a, 
                 starreal b, 
                 starreal grada[3],
                 starreal gradb[3],
                 starreal prod[3])
{
    prod[0] = grada[0] * b + gradb[0] * a;
    prod[1] = grada[1] * b + gradb[1] * a;
    prod[2] = grada[2] * b + gradb[2] * a;
}

/* given two values top and bottom and their gradients, compute the 
   gradient of their quotient grad(top / bottom) */
void gradquotient(starreal top, 
                  starreal bot, 
                  starreal gradtop[3],
                  starreal gradbot[3],
                  starreal quot[3])
{
    starreal denom = bot * bot;
    quot[0] = (bot * gradtop[0] - top * gradbot[0]) / denom;
    quot[1] = (bot * gradtop[1] - top * gradbot[1]) / denom;
    quot[2] = (bot * gradtop[2] - top * gradbot[2]) / denom;
}

/* get the information about this tet needed for non-smooth
   optimization of the current quality measure */
void getoptinfo(struct tetcomplex *mesh,
                 struct opttet *opttet,
                 int vtxkind,
                 starreal vtxvec[3])
{
    starreal point[4][3];       /* the vertices of the tet */
    starreal edgelength[3][4];  /* the lengths of each of the edges of the tet */
    starreal edgegrad[3][4][3]; /* the gradient of each edge length wrt vtx1 */
    starreal facenormal[4][3];  /* the normals of each face of the tet */
    starreal facearea[4];       /* areas of the faces of the tet */
    starreal facegrad[4][3];    /* the gradient of each of the face areas wrt vtx1 */
    starreal volume;            /* volume of tetrahedron */
    starreal volumegrad[3] = {0.0, 0.0, 0.0};     /* the gradient of the volume of the tet wrt vtx1 */
    int i, j, k, l;           /* loop indices */
    int edgecount=0;          /* keep track of current edge */
    starreal ejk[3];            /* vector representing edge from j to k */
    starreal ejl[3];            /* vector representing edge from j to l */
    starreal t[3];
    starreal u[3];
    starreal v[3];
    starreal e1[3] = {0.0, 0.0, 0.0};
    starreal e2[3] = {0.0, 0.0, 0.0};
    
    /* temporary variables */
    starreal diff[3];
    starreal term1[3];
    starreal term2[3];
    starreal factor;
    starreal c;
    starreal top, bot;
    starreal gradtop[3];
    starreal gradbot[3];
    starreal gradquot[3];
    
    /* radius ratio vars */
    starreal Z;
    starreal twooverZ;
    starreal gradZtfac, gradZufac, gradZvfac;
    starreal gradZt[3];
    starreal gradZu[3];
    starreal gradZv[3];
    starreal gradZ[3];
    starreal faceareasum;
    starreal rootZareasum;
    starreal facegradsum[3];
    starreal vdott;
    starreal tdotu;
    starreal udotv;
    starreal tlen2;
    starreal ulen2;
    starreal vlen2;
    starreal uminusv[3];
    starreal vminust[3];
    starreal tminusu[3];
    starreal umvlen2;
    starreal vmtlen2;
    starreal tmulen2;
    starreal normfac = sqrt(3.0) * 6.0;
    starreal rnrrgradterm1[3], rnrrgradterm2[3];
    starreal E[3][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
    
    /* V / lrms^3 ratio vars */
    starreal edgelengthsum = 0.0;
    starreal lrms;
    starreal gradlrms[3];
    starreal vlrmsterm1[3];
    starreal vlrmsterm2[3];

    if (improvebehave.verbosity > 5)
    {
        printf("computing opt info for the following tet:\n");
        printtetverts(mesh, opttet->verts);
        printf("\n");
    }

    /* get tet vertices */
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, opttet->verts[0]))->coord, point[0]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, opttet->verts[1]))->coord, point[1]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, opttet->verts[2]))->coord, point[2]);
    vcopy(((struct vertex *) tetcomplextag2vertex(mesh, opttet->verts[3]))->coord, point[3]);
    
    /* if anisotropic meshing is enabled, warp the points according the
       deformation tensor at their barycenter */
    if (improvebehave.anisotropic)
    {
        /* fetch the deformation tensor at the barycenter of this tet */
        tettensor(mesh, opttet->verts[0], opttet->verts[1], opttet->verts[2], opttet->verts[3], E);
        
        /* transform each vertex */
        for (i=0; i<4; i++)
        {
            if (improvebehave.verbosity > 5)
            {
                printf("in getoptinfo transforming point (%g %g %g) ->", point[i][0], point[i][1], point[i][2]);
            }
            
            tensortransform(point[i], point[i], E);
            
            if (improvebehave.verbosity > 5)
            {
                printf(" (%g %g %g)\n", point[i][0], point[i][1], point[i][2]);
            }
        }
    }
    
    /* set some vectors */
    vsub(point[1], point[0], t);
    vsub(point[2], point[0], v);
    vsub(point[3], point[0], u);
    
    /* calculate the volume*6 of the tetrahedron using orientation */
    volume = (starreal) orient3d(&behave, point[0], point[1], point[2], point[3]) / 6.0;
    opttet->volume = volume;
    
    if (improvebehave.verbosity > 5)
    {
        printf("tetvolume = %g\n", volume);
    }
    
    /* for each vertex/face of the tetrahedron */
    for (i = 0; i < 4; i++) {
        j = (i + 1) & 3;
        if ((i & 1) == 0) {
            k = (i + 3) & 3;
            l = (i + 2) & 3;
        } else {
            k = (i + 2) & 3;
            l = (i + 3) & 3;
        }
        
        /* compute the normal for each face */
        /* for each vertex i in the loop, the ith face is the face
           opposite i, so that face's normal is found by taking the
           cross product of two edges of the opposite face */
        /* TODO implement this cross product with Orient2D calls? */
        
        /* one edge on opposite face */
        vsub(point[k], point[j], ejk);
        
        /* another edge originating from the same vertex */
        vsub(point[l], point[j], ejl);
        
        /* compute a normal vector to this face */
        cross(ejk, ejl, facenormal[i]);
        
        /* if i=0, this is also the gradient of the volume * 6
           with respect to vertex 0 */
        if (i==0)
        {
            opttet->volumegrad[0] = volumegrad[0] = -facenormal[i][0] / 6.0;
            opttet->volumegrad[1] = volumegrad[1] = -facenormal[i][1] / 6.0;
            opttet->volumegrad[2] = volumegrad[2] = -facenormal[i][2] / 6.0;
        }
        
        /* compute (2 *area)^2 for this face */
        facearea[i] = facenormal[i][0] * facenormal[i][0] +
            facenormal[i][1] * facenormal[i][1] +
            facenormal[i][2] * facenormal[i][2];
        /* now get the real area */
        opttet->facearea[i] = facearea[i] = sqrt(facearea[i]) / 2.0;
            
        /* compute the gradient of the area for this face */
        if (i==0)
        {
            /* this face doesn't include vtx1, gradient is zero */
            opttet->facegrad[i][0] = facegrad[i][0] = 0.0;
            opttet->facegrad[i][1] = facegrad[i][1] = 0.0;
            opttet->facegrad[i][2] = facegrad[i][2] = 0.0;
        }
        else
        {
            if (improvebehave.verbosity > 4)
            {
                if (facearea[i] <= 0)
                {
                    printf("found non-positive face area %g in tet (%d %d %d %d)\n", facearea[i], (int) opttet->verts[0], (int) opttet->verts[1], (int) opttet->verts[2], (int) opttet->verts[3]);
                    printf("tetexists = %d", tetexists(mesh, opttet->verts[0], opttet->verts[1], opttet->verts[2], opttet->verts[3]));
                    printtetverts(mesh, opttet->verts);
                    printf("\n\n\n");
                }
            }
            assert(facearea[i] > 0);
            /* gradient scaled by the face's area */
            factor = 1.0 / (4.0 * facearea[i]);
            
            /* handle each face separately */
            switch(i)
            {
                /* compute the area of face 1 using u and v */
                case 1:
                    vcopy(u, e1);
                    vcopy(v, e2);
                    break;
                case 2:
                    vcopy(t, e1);
                    vcopy(u, e2);
                    break;
                case 3:
                    vcopy(v, e1);
                    vcopy(t, e2);
                    break;
            }
            
            /* find the vector from elk to elj */
            vsub(e1, e2, diff);
            
            /* compute first term of gradient */
            c = dot(e2,diff);
            term1[0] = c * e1[0];
            term1[1] = c * e1[1];
            term1[2] = c * e1[2];
            
            /* compute the second term */
            c = dot(e1,diff);
            term2[0] = c * e2[0];
            term2[1] = c * e2[1];
            term2[2] = c * e2[2];
            
            /* now, combine the terms, scaled with the 1/4A */
            opttet->facegrad[i][0] = facegrad[i][0] = factor * (term1[0] - term2[0]);
            opttet->facegrad[i][1] = facegrad[i][1] = factor * (term1[1] - term2[1]);
            opttet->facegrad[i][2] = facegrad[i][2] = factor * (term1[2] - term2[2]);
        }
        
            
        /* compute edge lengths for quality measures that need them */
        if (improvebehave.qualmeasure == QUALMINSINE ||
            improvebehave.qualmeasure == QUALWARPEDMINSINE ||
            improvebehave.qualmeasure == QUALVLRMS3RATIO)
        {
            for (j = i + 1; j < 4; j++) {
            
                /* e1 is edge from point i to point j */
                vsub(point[j], point[i], e1);
                opttet->edgelength[i][j] = edgelength[i][j] = vlength(e1);
            
                /* also compute the gradient of the length of this edge */
            
                /* if vtx1 isn't one of the edge's endpoints, the gradent is zero */
                if (i != 0)
                {
                    opttet->edgegrad[i][j][0] = edgegrad[i][j][0] = 0.0;
                    opttet->edgegrad[i][j][1] = edgegrad[i][j][1] = 0.0;
                    opttet->edgegrad[i][j][2] = edgegrad[i][j][2] = 0.0;
                }
                /* otherwise, it's in the negative direction of this edge,
                   and scaled by edge length */
                else
                { 
                    factor = -1.0 / edgelength[i][j];
                    vscale(factor, e1, edgegrad[i][j]);
                    vcopy(edgegrad[i][j], opttet->edgegrad[i][j]);
                }
            }
        }
    }
    
    /* if the quality measure is minimum sine */
    if ((improvebehave.qualmeasure == QUALMINSINE) ||
        (improvebehave.qualmeasure == QUALWARPEDMINSINE))
    {
        /* for each edge in the tetrahedron */
        for (i = 0; i < 3; i++) {
            for (j = i + 1; j < 4; j++) {
                k = (i > 0) ? 0 : (j > 1) ? 1 : 2;
                l = 6 - i - j - k;
            
                /* compute the sine of this dihedral angle */
                opttet->sine[edgecount] = (3 * volume * edgelength[i][j]) / (2 * facearea[k] * facearea[l]);
                
                /* if we are warping the minimum sine */
                if (improvebehave.qualmeasure == QUALWARPEDMINSINE)
                {
                    /* and this is an obtuse angle */
                    if (dot(facenormal[k],facenormal[l]) > 0)
                    {
                        if (improvebehave.verbosity > 5)
                        {
                            printf("in grad compute, scaling down obtuse sin from %g to %g\n", opttet->sine[edgecount], opttet->sine[edgecount] * improvebehave.sinewarpfactor);
                        }
                        
                        /* scale the sin down by WARPFACTOR */
                        opttet->sine[edgecount] *= improvebehave.sinewarpfactor;
                    }
                }
            
                /* compute the gradient of the sine
               
                   we need the gradient of this expression:
               
                   3 * V * lij
                   ------------
                   2 * Ak * Al
               
                   so, find the gradient of the top product, the bottom product, then the quotient
                */
                top = volume * edgelength[i][j];
                bot = facearea[k] * facearea[l];
            
                /* find gradient of top */
                gradproduct(volume, edgelength[i][j], volumegrad, edgegrad[i][j], gradtop);
            
                /* find gradient of bottom */
                gradproduct(facearea[k], facearea[l], facegrad[k], facegrad[l], gradbot);
            
                /* now, find the gradient of the quotient */
                gradquotient(top, bot, gradtop, gradbot, gradquot);
            
                /* now scale with constant factor */
                c = 3.0 / 2.0;
                opttet->sinegrad[edgecount][0] = c * gradquot[0];
                opttet->sinegrad[edgecount][1] = c * gradquot[1];
                opttet->sinegrad[edgecount][2] = c * gradquot[2];
            
                /* if this is a facet vertex, project gradient onto facet */
                /*
                if (vtxkind == FACETVERTEX || vtxkind == FIXEDVERTEX)
                */
                if (vtxkind == FACETVERTEX)
                {
                    vprojecttoplane(opttet->sinegrad[edgecount], vtxvec, opttet->sinegrad[edgecount]);
                }
            
                /* if this is a segment vertex, project gradient onto segment */
                if (vtxkind == SEGMENTVERTEX)
                {
                    if (improvebehave.verbosity > 5)
                    {
                        printf("when trying to project to segment, vtxvec is %g %g %g, length %g\n", vtxvec[0], vtxvec[1], vtxvec[2], vlength(vtxvec));
                        printf("sine gradient before projection is %g %g %g\n", opttet->sinegrad[edgecount][0],
                                                                            opttet->sinegrad[edgecount][1],
                                                                            opttet->sinegrad[edgecount][2]);
                    }
                    vproject(opttet->sinegrad[edgecount], vtxvec, opttet->sinegrad[edgecount]);
                
                    if (improvebehave.verbosity > 5)
                    {
                        printf("sine gradient after  projection is %g %g %g\n", opttet->sinegrad[edgecount][0],
                                        opttet->sinegrad[edgecount][1],
                                        opttet->sinegrad[edgecount][2]);
                    }
                }
            
                edgecount++;
            }
        }
    }
    
    /* compute stuff for radius ratio */
    if (improvebehave.qualmeasure == QUALRADIUSRATIO)
    {
        /* compute intermediate quantity Z */
        Z = getZ(point[0], point[1], point[2], point[3]);
        
        if (improvebehave.verbosity > 5)
        {
            printf("Z is %g\n", Z);
        }
        
        twooverZ = 2.0 / Z;
        
        /* some dot products */
        vdott = dot(v, t);
        tdotu = dot(t, u);
        udotv = dot(u, v);
        
        if (improvebehave.verbosity > 5)
        {
            printf("vdott = %g\n", vdott);
            printf("tdotu = %g\n", tdotu);
            printf("udotv = %g\n", udotv);
        }
        
        /* some vector lengths */
        vsub(u, v, uminusv);
        vsub(v, t, vminust);
        vsub(t, u, tminusu);
        tlen2 = (t[0] * t[0]) + (t[1] * t[1]) + (t[2] * t[2]);
        ulen2 = (u[0] * u[0]) + (u[1] * u[1]) + (u[2] * u[2]);
        vlen2 = (v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]);
        umvlen2 = (uminusv[0] * uminusv[0]) + (uminusv[1] * uminusv[1]) + (uminusv[2] * uminusv[2]);
        vmtlen2 = (vminust[0] * vminust[0]) + (vminust[1] * vminust[1]) + (vminust[2] * vminust[2]);
        tmulen2 = (tminusu[0] * tminusu[0]) + (tminusu[1] * tminusu[1]) + (tminusu[2] * tminusu[2]);
        
        if (improvebehave.verbosity > 5)
        {
            printf("tlen2 = %g\n", tlen2);
            printf("ulen2 = %g\n", ulen2);
            printf("vlen2 = %g\n", vlen2);
            printf("umvlen2 = %g\n", umvlen2);
            printf("vmtlen2 = %g\n", vmtlen2);
            printf("tmulen2 = %g\n", tmulen2);
        }
        
        /* compute Z's gradient */
        gradZtfac = twooverZ * 
                 (
                    (ulen2 * vdott - vlen2 * tdotu) * (ulen2 - vlen2) - 
                    (ulen2 * vlen2 + tlen2 * udotv) * (umvlen2)
                 );
        gradZufac = twooverZ * 
                 (
                    (vlen2 * tdotu - tlen2 * udotv) * (vlen2 - tlen2) - 
                    (vlen2 * tlen2 + ulen2 * vdott) * (vmtlen2)
                 );
        gradZvfac = twooverZ * 
                 (
                    (tlen2 * udotv - ulen2 * vdott) * (tlen2 - ulen2) - 
                    (tlen2 * ulen2 + vlen2 * tdotu) * (tmulen2)
                 );
        
        if (improvebehave.verbosity > 5)
        {
            printf("tfac is %g\n", gradZtfac);
            printf("ufac is %g\n", gradZufac);
            printf("vfac is %g\n", gradZvfac);
        }
        
        /* compute t, u, v components of gradient */
        vscale(gradZtfac, t, gradZt);
        vscale(gradZufac, u, gradZu);
        vscale(gradZvfac, v, gradZv);
        
        /* add the components together to form grad(Z) */
        vadd(gradZt, gradZu, gradZ);
        vadd(gradZv, gradZ, gradZ);
        
        if (improvebehave.verbosity > 5)
        {
            printf("gradZ is (%g, %g, %g)\n", gradZ[0], gradZ[1], gradZ[2]);
        }
        
        /* compute sqrt (Z * (sum of face areas)) */
        faceareasum = opttet->facearea[0] +
                      opttet->facearea[1] +
                      opttet->facearea[2] +
                      opttet->facearea[3];
        rootZareasum = sqrt(Z * faceareasum);
        
        /* set the actual root normalized radius ratio */
        opttet->rnrr = (normfac * volume) / rootZareasum;
        
        if (improvebehave.verbosity > 5)
        {
            printf("rnrr = %g\n", opttet->rnrr);
        }
        
        assert(opttet->rnrr > 0.0);
        
        if (improvebehave.verbosity > 5)
        {
            starreal thisqual = tetquality(mesh, opttet->verts[0], opttet->verts[1], opttet->verts[2], opttet->verts[3], improvebehave.qualmeasure);
            printf("rnrr = %g, thisqual = %g, diff = %g\n", opttet->rnrr, thisqual, opttet->rnrr - thisqual);
            assert(opttet->rnrr - thisqual < 1e-13);
        }
        
        /* sum of face gradients */
        vadd(facegrad[0], facegrad[1], facegradsum);
        vadd(facegrad[2], facegradsum, facegradsum);
        vadd(facegrad[3], facegradsum, facegradsum);
        
        /* compute the first term */
        vscale((1.0 / rootZareasum), volumegrad, rnrrgradterm1);
        
        if (improvebehave.verbosity > 5)
        {
            printf("firstterm is (%g, %g, %g)\n", rnrrgradterm1[0], rnrrgradterm1[1], rnrrgradterm1[2]);
        }
        
        /* compute the second term */
        vscale(Z, facegradsum, facegradsum);
        vscale(faceareasum, gradZ, gradZ);
        vadd(facegradsum, gradZ, rnrrgradterm2);
        vscale(volume / (2 * (rootZareasum * rootZareasum * rootZareasum)), rnrrgradterm2, rnrrgradterm2);
        
        if (improvebehave.verbosity > 5)
        {
            printf("secondterm is (%g, %g, %g)\n", rnrrgradterm2[0], rnrrgradterm2[1], rnrrgradterm2[2]);
        }
        
        /* finally, compute the gradient of the radius ratio */
        vsub(rnrrgradterm1, rnrrgradterm2, opttet->rnrrgrad);
        vscale(normfac, opttet->rnrrgrad, opttet->rnrrgrad);
        
        /* if this is a facet vertex, project gradient onto facet */
        /*if (vtxkind == FACETVERTEX || vtxkind == FIXEDVERTEX)*/
        if (vtxkind == FACETVERTEX)
        {
            vprojecttoplane(opttet->rnrrgrad, vtxvec, opttet->rnrrgrad);
        }
    
        /* if this is a segment vertex, project gradient onto segment */
        if (vtxkind == SEGMENTVERTEX)
        {
            vproject(opttet->rnrrgrad, vtxvec, opttet->rnrrgrad);
        }
        
        if (improvebehave.verbosity > 5 || opttet->rnrr < 0.0)
        {
            printf("computing opt info for the following tet, negative qual = %g:\n", opttet->rnrr);
            printtetverts(mesh, opttet->verts);
            printf("\n");
            printf("rnrrgrad is (%g %g %g)\n", opttet->rnrrgrad[0], opttet->rnrrgrad[1], opttet->rnrrgrad[2]);
            starexit(1);
        }
    }
    
    /* if the quality measure is volume to edge length ratio */
    if (improvebehave.qualmeasure == QUALVLRMS3RATIO)
    {
        /* for each edge in the tetrahedron */
        for (i = 0; i < 3; i++) 
        {
            for (j = i + 1; j < 4; j++) 
            {
                k = (i > 0) ? 0 : (j > 1) ? 1 : 2;
                l = 6 - i - j - k;
                
                /* accumulate edge length sum */
                edgelengthsum += edgelength[i][j] * edgelength[i][j];
            }
        }
        
        /* compute the root mean square */
        lrms = sqrt((1.0 / 6.0) * edgelengthsum);
        
        if (improvebehave.verbosity > 5)
        {
            printf("edgelengthsum = %g, lrms = %g\n", edgelengthsum, lrms);
        }
        
        normfac = 6.0 * sqrt(2.0);
        
        /* compute the raw ratio */
        opttet->vlrms3r = (normfac * volume) / (lrms * lrms * lrms);
        
        if (improvebehave.verbosity > 5)
        {
            printf("V / lrms^3 ratio is %g, tetqual call is %g\n", opttet->vlrms3r, tetquality(mesh, opttet->verts[0],
                                                                                                     opttet->verts[1],
                                                                                                     opttet->verts[2],
                                                                                                     opttet->verts[3],
                                                                                                     improvebehave.qualmeasure));
        }
        
        /* compute gradient of lrms */
        vadd(t, u, gradlrms);
        vadd(v, gradlrms, gradlrms);
        vscale((-1.0 / (6.0 * lrms)), gradlrms, gradlrms);
        
        /* compute the terms of the gradient of the ratio */
        vscale((1.0 / (lrms * lrms * lrms)), volumegrad, vlrmsterm1);
        vscale((3.0 * volume) / (lrms * lrms * lrms * lrms), gradlrms, vlrmsterm2);
        
        /* add terms and normalize */
        vsub(vlrmsterm1, vlrmsterm2, opttet->vlrms3rgrad);
        vscale(normfac, opttet->vlrms3rgrad, opttet->vlrms3rgrad);
        
        /* if this is a facet or fixed vertex, 
           project gradient onto facet or plane */
        /*if (vtxkind == FACETVERTEX || vtxkind == FIXEDVERTEX)*/
        if (vtxkind == FACETVERTEX)
        {
            vprojecttoplane(opttet->vlrms3rgrad, vtxvec, opttet->vlrms3rgrad);
        }
    
        /* if this is a segment vertex, project gradient onto segment */
        if (vtxkind == SEGMENTVERTEX)
        {
            vproject(opttet->vlrms3rgrad, vtxvec, opttet->vlrms3rgrad);
        }
        
        if (improvebehave.verbosity > 5)
        {
            printf("vlrmsgrad is (%g %g %g)\n", opttet->vlrms3rgrad[0], opttet->vlrms3rgrad[1], opttet->vlrms3rgrad[2]);
        }
    }
    
}

/* test gradient computation */
bool testgrad(struct tetcomplex *mesh,
              tag vtx,
              tag vtx2,
              tag vtx3,
              tag vtx4)
{
    struct opttet tet;
    starreal *point, origpoint[3]; /* the vertex we're playing with */
    starreal change[3];
    starreal old, new, diff;
    int i,j,k,l;
    starreal grad[3];
    starreal epsilon = 1e-5;
    starreal onepe = 1+ epsilon;
    starreal oneme = 1- epsilon;
    
    point = ((struct vertex *) tetcomplextag2vertex(mesh, vtx))->coord;
    /* save the original point */
    vcopy(point,origpoint);
    
    tet.verts[0] = vtx;
    tet.verts[1] = vtx2;
    tet.verts[2] = vtx3;
    tet.verts[3] = vtx4;
    
    if (improvebehave.verbosity > 5) printf("\n****** BEGIN GRADIENT TEST *******\n");
    
    getoptinfo(mesh, &tet, FREEVERTEX, NULL);
    
    /* test volume gradient */
    old = tet.volume;
    if (improvebehave.verbosity > 5)
    {
        printf("    original volume is %g\n", tet.volume);
        printf("    volume gradient is %g %g %g\n", tet.volumegrad[0], tet.volumegrad[1], tet.volumegrad[2]);
        printf("        moving vertex 1%% of gradient...\n");
    }
    
    vcopy(tet.volumegrad, change);
    vscale(0.01, change, change);
    
    /* move the point */
    vadd(point, change, point);
    
    /* recalculate stuff */
    getoptinfo(mesh, &tet, FREEVERTEX, NULL);
    
    new = tet.volume;
    diff = new - old;
    
    /* put the original point back */
    vcopy(origpoint,point);
    
    if (improvebehave.verbosity > 5)
    {
        printf("    new volume is %g\n", new);
        printf("    difference is %g\n", diff);
    }
    
    if (diff < 0)
    {
        printf("\n\n\n!!!! ERROR, VOLUME DECREASED !!!!!\n\n\n");
        return false;
    }
    /************** end test volume gradient *********************/
    
        
    /************** test face area gradients *********************/
    if (improvebehave.verbosity > 5) printf("\n\n******** BEGIN FACE AREA GRADIENT TEST **********\n");
    for (i=0; i<4; i++)
    {
         /* put the original point back */
        vcopy(origpoint,point);
        getoptinfo(mesh, &tet, FREEVERTEX, NULL);
        
        old = tet.facearea[i];
        vcopy(tet.facegrad[i], grad);
        
        if (improvebehave.verbosity > 5)
        {
            printf("    original face area is %d is %g\n",i, old);
            printf("    face area gradient is %g %g %g\n", grad[0], grad[1], grad[2]);
            printf("        moving vertex...\n");
        }
        
        vcopy(grad, change);
        vscale(1e-7, change, change);
    
        /* move the point */
        vadd(point, change, point);
    
        /* recalculate stuff */
        getoptinfo(mesh, &tet, FREEVERTEX, NULL);
        
        new = tet.facearea[i];
        diff = new - old;
        
        if (improvebehave.verbosity > 5)
        {
            printf("    new face area is %d is %g\n",i, new);
            printf("    difference is %g\n", diff);
        }

        if (diff < 0)
        {
            printf("\n\n\n!!!! ERROR, FACE AREA DECREASED !!!!!\n\n\n");
            return false;
        }
    
    }
    /********************** end test face area gradients *********************/

    /************** test edge length gradients *********************/
    if (improvebehave.verbosity > 5) printf("\n\n******** BEGIN EDGE LENGTH GRADIENT TEST **********\n");
    for (i=0; i<6; i++)
    {
        for (j = i + 1; j < 4; j++) 
        {
            k = (i > 0) ? 0 : (j > 1) ? 1 : 2;
            l = 6 - i - j - k;
            
            /* put the original point back */
            vcopy(origpoint,point);
            getoptinfo(mesh, &tet, FREEVERTEX, NULL);
        
            old = tet.edgelength[i][j];
            vcopy(tet.edgegrad[i][j], grad);
        
            if (improvebehave.verbosity > 5)
            {
                printf("    original edge length is %d is %g\n",i, old);
                printf("    edge length gradient is %g %g %g\n", grad[0], grad[1], grad[2]);
                printf("        moving vertex...\n");
            }
        
            vcopy(grad, change);
            vscale(1e-7, change, change);
    
            /* move the point */
            vadd(point, change, point);
    
            /* recalculate stuff */
            getoptinfo(mesh, &tet, FREEVERTEX, NULL);
        
            new = tet.edgelength[i][j];
            diff = new - old;
        
            if (improvebehave.verbosity > 5)
            {
                printf("    new edge length is %d is %g\n",i, new);
                printf("    difference is %g\n", diff);
            }

            if (diff < 0)
            {
                printf("\n\n\n!!!! ERROR, EDGE LENGTH DECREASED !!!!!\n\n\n");
                return false;
            }
        }
    
    }
    /********************** end test edge length gradients *********************/
    
    /* test sine gradients */
    if (improvebehave.verbosity > 5) printf("\n\n******** BEGIN SINE GRAIDENT TEST **********\n");
    for (i=0; i<6; i++)
    {
         /* put the original point back */
        vcopy(origpoint,point);
        getoptinfo(mesh, &tet, FREEVERTEX, NULL);
        
        old = tet.sine[i];
        
        /* if sine is really close to one, too unstable */
        if (old < onepe && old > oneme)
        {
            printf("sine really close to one, let it go...\n");
            continue;
        }
        
        if (improvebehave.verbosity > 5)
        {
            printf("    original sine %d is %g\n",i, old);
            printf("    sine gradient is %g %g %g\n", tet.sinegrad[i][0], tet.sinegrad[i][1], tet.sinegrad[i][2]);
            printf("        moving vertex...\n");
        }
        
        vcopy(tet.sinegrad[i], change);
        vscale(1e-10, change, change);
    
        /* move the point */
        vadd(point, change, point);
    
        /* recalculate stuff */
        getoptinfo(mesh, &tet, FREEVERTEX, NULL);
        
        new = tet.sine[i];
        diff = new - old;
        
        if (improvebehave.verbosity > 5)
        {
            printf("    new sine %d is %g\n",i, new);
            printf("    difference is %g\n", diff);
        

            printf("here's the tet:\n\n");
            printf("tet =");
            printtetverts(mesh,tet.verts);
            printf(";\n\n");
        
            printf("grad= [%f %f %f];\n", tet.sinegrad[i][0],tet.sinegrad[i][1],tet.sinegrad[i][2]);
        }

        if (diff < 0)
        {
            printf("\n\n\n!!!! ERROR, SINE DECREASED !!!!!\n");
            printopttet(&tet);
            
            return false;
        }
    
    }
    /* put the original point back */
    vcopy(origpoint,point);
    
    return 1;

}

/* returns the basis B of S union M. S is a set of points known
   to be in the basis */
void findbasis(starreal S[][3],
               starreal M[][3],
               starreal B[][3],
               int sizeS,
               int sizeM,
               int *sizeB)
{
    starreal p[3],q[3],r[3],s[3];        /* vertices */
    starreal s1[3], t1[3], d1[3], d2[3]; /* temporary vertices */
    starreal origin[3] = {0.0, 0.0, 0.0};
    starreal localS[4][3];               /* for passing to recursive calls */
    starreal localM[MAXINCIDENTTETS][3]; /* for passing to recursive colls */
    
    assert(sizeM > 0);
    
    if (improvebehave.verbosity > 5) printbasisarrays(S,M,B,sizeS,sizeM,sizeB,p);
    
    /* we assume that M was passed to us shuffled, so that taking
       the last element is equivalent to removing a random one. */
    vcopy(M[sizeM-1],p);
    
    /* if M has only one element */
    if (sizeM == 1)
    {
        /* and S has no elements */
        if (sizeS == 0)
        {
            if (improvebehave.verbosity > 5) printf("M has one element, and S is empty. Copying M to B and sending it back...\n");
            
            /* then the single element in M must be the
               entire basis, just send back M */
            pointarraycopy(M, B, 1);
            *sizeB = 1;
            return;
        }
        
        if (improvebehave.verbosity > 5) printf("M has one element p. Assume it's not in basis and set B = S...\n");
        
        /* otherwise, because we assume the last element
           we just removed is not part of the basis, assign
           the basis to be the elements of S */
        pointarraycopy(S, B, sizeS);
        *sizeB = sizeS;
    }
    /* M has more than one element. Throw one out (p), and look for the
       basis assuming p is not part of it. */
    else
    {
        if (improvebehave.verbosity > 5) printf("M has more than one element, tossing one out and recursing...\n");
        
        /* make a new copy of M minus the last element */
        pointarraycopy(M, localM, sizeM-1);
        pointarraycopy(S, localS, sizeS);
        findbasis(localS, localM, B, sizeS, sizeM-1, sizeB);
    }
    
    /* now the we have determined the basis without p, we need to 
       go back and check whether p actually is part of the basis. */
    
    switch (*sizeB)
    {
        /* if the returned basis has just one point q, we just need to check
           whether p is closer to the origin than q */
        case 1:
            /* fetch the actual coordinates from the mesh */
            vcopy(B[0],q);
        
            /* compute the vector from q to p */
            vsub(p, q, d1);
            
            /* check the sign of the dot product. >=0 means p doesn't
               improve the basis */
            if (dot(q,d1) >= 0)
            {
                /* this is a good B, send it back!*/
                return;
            }
            break;
            
        /* check whether p improves the basis using math I don't understand */        
        case 2:
            /* fetch coordinates from the mesh */
            vcopy(B[0],q);
            vcopy(B[1],r);
            
            /* compute vector s from r to p */
            vsub(p, r, s1);
            /* compute vector t from r to q */
            vsub(q, r, t1);
            
            /* now a couple of cross products */
            cross(s1, t1, d1);
            cross(r, t1, d2);
            
            /* can p improve the basis? */
            if (dot(d1,d2) >= 0)
            {
                /* nope! send back B as is. */
                return;
            }            
            break;
        case 3:
            /* fetch coordinates from the mesh */
            vcopy(B[0],q);
            vcopy(B[1],r);
            vcopy(B[2],s);
            
            /* does p improve the basis? */
            if (orient3d(&behave, p, q, r, s) * orient3d(&behave, origin, q, r, s) <= 0)
            {
                /* nope! send back B as is. */
                return;
            }
            break;
        default:
            /* B has size of 4, and any basis of this size is optimal */
            return;
            break;
    }
    
    /* if we have made it this far, we know that p actually is a part of
       any basis of S union M */
       
    /* if p was the last element of M, or if S already has three other basis
       points, we're done and just need to send back S union p. */
    if ((sizeM == 1) || (sizeS == 3))
    {
        if (improvebehave.verbosity > 5) printf("p in basis and it's the last possible point, return S U p\n");
        /* the final size of B is the size of S + 1 */
        *sizeB = sizeS + 1;
        /* copy S into B */
        pointarraycopy(S, B, sizeS);
        /* and add p at the end */
        vcopy(p, B[*sizeB - 1]);
        
        return;
    }
    /* there may be more basis points to find! move p from M to the known
       basis point set S, and go again */
    else
    {
        if (improvebehave.verbosity > 5)
        {
            printf("p in basis, more points to check. Moving p from M to S\n");
            printf("here's how everything looked before the move:\n");
            printbasisarrays(S,M,B,sizeS,sizeM,sizeB,p);
        } 
        
        /* create the new S */
        pointarraycopy(S, localS, sizeS);
        /* add p to the end of it */
        vcopy(p, localS[sizeS]);
        
        /* create the new M, leaving off the last element */
        pointarraycopy(M, localM, sizeM-1);
        
        /* find any basis points remaining in M */
        findbasis(localS, localM, B, sizeS+1, sizeM-1, sizeB);
        
        return;
    }        
}

/* finds the point on the convex hull of P nearest
   the origin */
void minconvexhullpoint(starreal P[][3],
                        int sizeP,
                        starreal nearest[])
{
    starreal B[4][3];                /* the basis for the convex hull point */
    int sizeB=0;                   /* size of the basis */
    starreal empty[4][3];            /* empty set for known basis */
    starreal *p, *q, *r;             /* basis points */
    starreal pmq[3];                 /* p minus q */
    starreal c, d, l;                /* scalar factors */
    starreal s[3], t[3], s2[3], t2[3];/* temporary points */
    starreal sxt[3], sxr[3], rxt[3], temp[3]; /* temporary cross products */
    int i;
    
    assert(sizeP > 0);
    
    /* find a basis for the minimum point on the convex hull */
    findbasis(empty, P, B, 0, sizeP, &sizeB);
    
    if (improvebehave.verbosity > 5)
    {
        printf("\nbasis size is %d\n", sizeB);
        printf("here are the basis points:\nbasis=[");
        for (i=0; i<sizeB; i++)
        {
            printf("%.17g %.17g %.17g;\n",B[i][0],B[i][1],B[i][2]);
        }
        printf("]\n");
        
    }
    
    switch(sizeB)
    {
        /* if the basis is just a single point, return that point */
        case 1:
            vcopy(B[0], nearest);
            return;
            break;
        /* for two points, find the closest point to the origin on
           the line between the two points */
        case 2:
            p = B[0];
            q = B[1];
            vsub(p,q,pmq);
            
            /*
              nearest = q - dot(q,p-q)/(length(p-q)^2) * (p-q)
            */
            l = vlength(pmq);
            
            /* if these points are the same, just return one of them */
            if (l == 0.0)
            {
                vcopy(B[0], nearest);
                return;
            }
            
            c = dot(q,pmq) / (l * l);
            vscale(c,pmq,nearest);
            vsub(q,nearest,nearest);
            
            return; 
            break;
        /* for three points, find the point closest to the origin
           on the triangle that the they form */
        case 3:
            p = B[0];
            q = B[1];
            r = B[2];
            
            vsub(p,r,s);
            vsub(q,r,t);
            
            cross(s,t,sxt);
            cross(r,t,rxt);
            cross(s,r,sxr);
            
            /* if any of these cross products is really tiny, give up
               and return the origin */
            if (vlength(sxt) < NEARESTMIN || vlength(rxt) < NEARESTMIN || vlength(sxr) < NEARESTMIN)
            {
                if (improvebehave.verbosity > 5)
                {
                    printf("Encountered tiny factor (< %g) in nearest point computation, returning origin.\n", NEARESTMIN);
                    printf("sxt= %.17g %.17g %.17g\n", sxt[0], sxt[1], sxt[2]);
                    printf("rxt= %.17g %.17g %.17g\n", rxt[0], rxt[1], rxt[2]);
                    printf("sxr= %.17g %.17g %.17g\n", sxr[0], sxr[1], sxr[2]);
                }
                nearest[0] = 0.0;
                nearest[1] = 0.0;
                nearest[2] = 0.0;
                return;
            }
            
            c = dot(sxt,rxt) / dot(sxt,sxt);
            d = dot(sxt,sxr) / dot(sxt,sxt);
            
            vscale(c,s,s2);
            vscale(d,t,t2);
            
            vsub(r,s2,temp);
            vsub(temp,t2,nearest);
            
            return;
            break;
        /* if the basis has four points, they must enclose the origin
           so just return the origin. */
        case 4:
            nearest[0] = 0.0;
            nearest[1] = 0.0;
            nearest[2] = 0.0;
            return;
            break;
        default:
            printf("Error, basis size %d is bogus, dying\n",sizeB);
            starexit(1);
            break;
    }
}



/* find the best step to take to improve all of the quality
   functions affected by a vertex vtx in the search direction
   d with an expected rate of improvement r */
void nonsmoothlinesearch(struct tetcomplex *mesh,
                         tag vtx,
                         starreal d[],
                         starreal inworstqual,
                         starreal *alpha,
                         starreal r,
                         struct opttet incidenttets[],
                         int numincident,
                         int vtxkind,
                         starreal vtxvec[3])
{
    int numiter = 0;      /* number of iterations */
    starreal *v;            /* the vertex to be modified */
    starreal origvertex[3]; /* to save the original vertex position */
    starreal offset[3];     /* the offset to move the vertex, alpha * d */
    starreal worstqual;     /* the current worst quality */
    starreal origworstqual; /* the original worst quality */
    starreal thisqual;    /* the quality of the current tet */
    starreal oldworstqual;  /* the worst quality at the last step */
    int i;                /* loop index */
    
    /* save the original worst quality */
    origworstqual = oldworstqual = inworstqual;
    
    /* fetch the original vertex coordinates from the mesh */
    v = ((struct vertex *) tetcomplextag2vertex(mesh, vtx))->coord;
    vcopy(v, origvertex);
    
    /* keep trying until alpha gets too small or we exceed maximum
       number of iterations */
    while ((*alpha > MINSTEPSIZE) && (numiter < MAXLINEITER))
    {
        if (improvebehave.verbosity > 5) printf("In line search, alpha=%g numiter=%d\n",*alpha,numiter);
        
        /* compute the offset from original vertex positon,
           alpha * d */
        vscale(*alpha, d, offset);
        /* move the vertex */
        vadd(v, offset, v);
        
        /* recompute all of the quality functions affected
           by v's position, taking note of the smallest one */
        worstqual = HUGEFLOAT; 
        for (i=0; i<numincident; i++)
        {
            thisqual = tetquality(mesh,
                                  incidenttets[i].verts[0],
                                  incidenttets[i].verts[1],
                                  incidenttets[i].verts[2],
                                  incidenttets[i].verts[3],
                                  improvebehave.qualmeasure);
        
            if (improvebehave.verbosity > 5 && thisqual < 0.0)
            {
                printf("in line search, after taking step we have negative quality = %g\n", thisqual);
            }
            
            /* is this the worst quality we've seen? */
            if (thisqual < worstqual) worstqual = thisqual;
        }
        
        /* if we're using surface quadrics */
        if (improvebehave.usequadrics && hasquadric(vtx) && ((struct vertextype *) arraypoolforcelookup(&vertexinfo, vtx))->kind == FIXEDVERTEX)
        /* if (improvebehave.usequadrics && hasquadric(vtx) && ((struct vertextype *) arraypoolforcelookup(&vertexinfo, vtx))->kind != FREEVERTEX) */
        {
            /* check whether the surface quadric is the worse than all the tets */
            starreal qe = quadricerrortet(mesh, vtx);
            if (qe < worstqual) 
            {
                if (improvebehave.verbosity > 5)
                {
                    printf("In line search, vertex %d's quadric error (%g raw, %g normalized) is worse than any tet quality!\n", (int) vtx, quadricerror(mesh, vtx), qe);
                }
                worstqual = qe;
            }
        }
        
        
        if (improvebehave.verbosity > 5) printf("The old worst was %g, the new worst is %g, diff is %g\n",oldworstqual,worstqual,worstqual-oldworstqual);
            
            /* if this is not the first iteration, and
           we did better on the last iteration, use
           the step size from the previous iteration */
        if ((oldworstqual > origworstqual) && (oldworstqual > worstqual))
        {
            if (improvebehave.verbosity > 5) printf("last step did better than current alpha, return it.\n");
            
            /* use the previous step's alpha */
            *alpha = (*alpha) * 2;
            assert(*alpha > 0.0);
            
            /* put vertex back where it started */
            vcopy(origvertex, v);
            
            return;
        }
        
        /* if we have succeeded in gaining 90% of the expected
           improvement, accept this initial step size */
        if ((worstqual - origworstqual) > (0.9 * (*alpha) * r))
        {
            if (improvebehave.verbosity > 5) printf("diff between origworstqual=%g and curworstqual=%g is >90%% expected improvement %g.\nthis step is a success, return alpha=%g\n",origworstqual,worstqual,*alpha * r * 0.9, *alpha);
            
            /* put vertex back where it started */
            vcopy(origvertex, v);
            
            return;
        }
        
        if (improvebehave.verbosity > 5) printf("this alpha isn't working, going to half it...\n");
        
        /* put vertex back where it started */
        vcopy(origvertex, v);
        
        /* cut alpha down by half and try again */
        *alpha = (*alpha) / 2.0;
        
        /* save the worst quality from this step */
        oldworstqual = worstqual;
    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("whoa, failed to find an appropriate alpha! numiter = %d, alpha = %g\n", numiter, *alpha);
    }
    
    /* no positive alpha could be found that improved things... give up and return zero */
    *alpha = 0.0;
}



/* given a set of tets incident to a vertex, and the quality
   of the worst quality function that varies with that vertex,
   compute the active set A of quality functions very near
   the worst */
void getactiveset(struct tetcomplex *mesh,
                  struct opttet incidenttets[],
                  int numincident,
                  starreal activegrads[][3],
                  int *numactive,
                  starreal worstqual)
{
    int i,j;
    tag vtx = incidenttets[0].verts[0];
    
    /* reset number of active gradients to zero */
    *numactive = 0;
    
    if (improvebehave.verbosity > 5)
    {
        printf("worstqual is %g\n", worstqual);
    }
    
    /* if we are including surface quadrics, give them a chance to
       enter the active set */
    assert(numincident > 0);
    /* if (improvebehave.usequadrics && hasquadric(vtx) && ((struct vertextype *) arraypoolforcelookup(&vertexinfo, vtx))->kind != FREEVERTEX) */
    if (improvebehave.usequadrics && hasquadric(vtx) && ((struct vertextype *) arraypoolforcelookup(&vertexinfo, vtx))->kind == FIXEDVERTEX)
    {
        /* if the quadric is close enough to worst, add it to the active set */
        if (quadricerrortet(mesh, vtx) <= (worstqual * ACTIVESETFACTOR))
        {
            /* fetch the gradient of this quadric */
            starreal quadgrad[3];
            quadricgradtet(mesh, vtx, quadgrad);
            
            if (improvebehave.verbosity > 5)
            {
                printf("Vertex %d's quadric error (%g raw, %g normalized, gradient = [%g %g %g]) is in the active set (worstqual = %g)!\n", (int) vtx, quadricerror(mesh, vtx), quadricerrortet(mesh, vtx), quadgrad[0], quadgrad[1], quadgrad[2], worstqual);
            }
            
            /* copy the gradient into the list of active gradients */
            vcopy(quadgrad, activegrads[*numactive]);
            (*numactive)++;
        }
    }
    
    for (i=0; i<numincident; i++)
    {
        switch (improvebehave.qualmeasure)
        {
            case QUALWARPEDMINSINE:
            case QUALMINSINE:
                for (j=0; j<6; j++)
                {
                    /* is this close enough to the worst? */
                    if (incidenttets[i].sine[j] < (worstqual * ACTIVESETFACTOR))
                    {
                        /* get the actual gradient value */
                        vcopy(incidenttets[i].sinegrad[j], activegrads[*numactive]);
                
                        (*numactive)++;
                    }
                }
                break;
            case QUALRADIUSRATIO:
                if (improvebehave.verbosity > 5)
                {
                    printf("rnrr for tet %d is %g\n", i, incidenttets[i].rnrr);
                }
                
                if (incidenttets[i].rnrr <= (worstqual * ACTIVESETFACTOR))
                {
                    /* get the actual gradient value */
                    vcopy(incidenttets[i].rnrrgrad, activegrads[*numactive]);
                    
                    (*numactive)++;
                }
                break;
            case QUALVLRMS3RATIO:
                if (improvebehave.verbosity > 5)
                {
                    printf("vlrms3r for tet %d is %g\n", i, incidenttets[i].vlrms3r);
                }

                if (incidenttets[i].vlrms3r <= (worstqual * ACTIVESETFACTOR))
                {
                    /* get the actual gradient value */
                    vcopy(incidenttets[i].vlrms3rgrad, activegrads[*numactive]);

                    (*numactive)++;
                }
                break;
            default:
                printf("i don't know how to compute the active set for qual measure %d\n", improvebehave.qualmeasure);
                starexit(1);
                break;
        }
    }
    
    if (*numactive == 0)
    {
        printf("didn't find any active quality functions.");
        for (i=0; i<numincident; i++)
        {
            printopttet(&incidenttets[i]);
        }
    }
    
    /* we must have at least found the worst */
    /* assert(*numactive > 0); */
}

/* for our initial step size, we use the distance
   to the next intersection with another quality funtion.
   this is the point at which the other quality function
   becomes the worst. we use a single-term taylor expansion
   to approximate all of the quality functions as lines,
   so we'll have to do a line search to find our ultimate
   step size. */
starreal getinitialalpha(struct tetcomplex *mesh,
                         struct opttet incidenttets[],
                         int numincident,
                         starreal d[3],
                         starreal r,
                         starreal worstqual)
{
    int i,j;
    starreal alpha = HUGEFLOAT;
    starreal newalpha;
    starreal rate;
    tag vtx = incidenttets[0].verts[0];
    
    /* if we are including surface quadrics add check for it */
    assert(numincident > 0);
    /* if (improvebehave.usequadrics && hasquadric(vtx) && ((struct vertextype *) arraypoolforcelookup(&vertexinfo, vtx))->kind != FREEVERTEX) */
    if (improvebehave.usequadrics && hasquadric(vtx) && ((struct vertextype *) arraypoolforcelookup(&vertexinfo, vtx))->kind == FIXEDVERTEX)
    {
        /* fetch the gradient of this quadric */
        starreal quadgrad[3];
        quadricgradtet(mesh, vtx, quadgrad);
        
        /* if this function improves more slowly
           than any in the active set, then it might
           end up as the objective. */
        rate = dot(d, quadgrad);
        if (rate + RATEEPSILON < r)
        {
            /* compute the approximation of when this
               function will become the objective */
            newalpha = (quadricerrortet(mesh, vtx) - worstqual) / (r - rate);
    
            /* if this is smaller than our current step size,
               use it for the step size */
            if (newalpha < alpha)
            {
                alpha = newalpha;
                if (improvebehave.verbosity > 5)
                {
                    printf("Vertex %d's quadric error sets initial alpha (%g raw, %g normalized, gradient = [%g %g %g])\n", (int) vtx, quadricerror(mesh, vtx), quadricerrortet(mesh, vtx), quadgrad[0], quadgrad[1], quadgrad[2]);
                }
            }
        }
    }
    
    for (i=0; i<numincident; i++)
    {
        switch (improvebehave.qualmeasure)
        {
            case QUALWARPEDMINSINE:
            case QUALMINSINE:
                for (j=0; j<6; j++)
                {
                    /* if this function improves more slowly
                       than any in the active set, then it might
                       end up as the objective. */
                    rate = dot(d,incidenttets[i].sinegrad[j]);
                    if (rate + RATEEPSILON < r)
                    {
                        /* compute the approximation of when this
                           function will become the objective */
                        newalpha = (incidenttets[i].sine[j] - worstqual) / (r - rate);
                
                        /* if this is smaller than our current step size,
                           use it for the step size */
                        if (newalpha < alpha)
                        {
                            alpha = newalpha;
                        }
                    }
                }
                break;
            case QUALRADIUSRATIO:
                /* if this function improves more slowly
                   than any in the active set, then it might
                   end up as the objective. */
                rate = dot(d,incidenttets[i].rnrrgrad);
                if (rate + RATEEPSILON < r)
                {
                    /* compute the approximation of when this
                       function will become the objective */
                    newalpha = (incidenttets[i].rnrr - worstqual) / (r - rate);
            
                    /* if this is smaller than our current step size,
                       use it for the step size */
                    if (newalpha < alpha)
                    {
                        alpha = newalpha;
                    }
                }
                break;
            case QUALVLRMS3RATIO:
                /* if this function improves more slowly
                   than any in the active set, then it might
                   end up as the objective. */
                rate = dot(d,incidenttets[i].vlrms3rgrad);
                if (rate + RATEEPSILON < r)
                {
                    /* compute the approximation of when this
                       function will become the objective */
                    newalpha = (incidenttets[i].vlrms3r - worstqual) / (r - rate);

                    /* if this is smaller than our current step size,
                       use it for the step size */
                    if (newalpha < alpha)
                    {
                        alpha = newalpha;
                    }
                }
                break;
            default:
                printf("i don't know how to compute alpha for qual measure %d\n", improvebehave.qualmeasure);
                starexit(1);
                break;
        }
    }
    
    if (improvebehave.anisotropic == false)
    {
        assert(alpha > 0.0);
    }
    else if (alpha < 0.0)
    {
        alpha = 0.0;
    }
    return alpha;
}

/* perform non-smooth optimization of a vertex's position.
   vtx - the vertex to be smoothed, is in tet (vtx, vtx2, vtx3, vtx4).
   This tet gives us a starting point to build the list of incident
   elements.
   
   If we end up moving the vertex, return true.
   If for some reason we can't, return false.
*/
bool nonsmooth(struct tetcomplex *mesh,
               tag vtx,
               struct opttet incidenttets[],
               int numincident,
               starreal *outworstqual,
               int smoothkinds)
{
    starreal *v;           /* the vertex to be altered */
    starreal origpoint[3]; /* to save the original vertex location */
    int numactive;         /* number of quality functions in the active set */
    int i,k;               /* loop index */
    int numiter = 0;       /* number of optimization iterations */
    starreal worstqual;      /* the numerical value of the worst quality function */
    starreal thisqual;       /* quality of the current tet */
    starreal oldworstqual;   /* the numerical value of the worst quality function at the last step */
    starreal improvement;    /* how much we've improved this step */
    starreal d[3];           /* direction to move vertex */
    starreal dlength;        /* length of d */
    starreal r;              /* expected rate of improvement */
    starreal newr;           /* temp r var */
    starreal alpha;          /* step size */
    starreal newalpha;       /* candidate new step size */
    starreal rate;           /* the rate a particular function will improve in the direction d */
    starreal change[3];      /* change we'll make to the point */
    struct vertextype *vinfo;
    tag verts[4];
    starreal qe;
    starreal allgrads[3];
    /* the gradients in the active set */
    starreal activegrads[MAXINCIDENTTETS][3];
    
    vinfo = (struct vertextype *) arraypoolforcelookup(&vertexinfo, vtx);
    
    /* if fixed vertex smoothing is enabled, and this vertex is a facet
       or segment vertex, we need to check that it still is */
    if ((vinfo->kind == FACETVERTEX || vinfo->kind == SEGMENTVERTEX) &&
        improvebehave.fixedsmooth)
    {
        if (improvebehave.verbosity > 5)
        {
            printf("Reclassifying in smooth, old type is %d", vinfo->kind);
        }
        
        /* reset this vertex type */
        vinfo->kind = INPUTVERTEX;
        
        /* reclassify this vertex */
        assert(incidenttets[0].verts[0] == vtx);
        tetvertexclassify(mesh, vtx, incidenttets[0].verts[1], incidenttets[0].verts[2], incidenttets[0].verts[3]);
        
        if (improvebehave.verbosity > 5)
        {
            printf(" new type is %d\n", vinfo->kind);
        }
    }
    
    /* check whether to try to smooth facet/segment/fixed vertices */
    if (vinfo->kind == FACETVERTEX && ((smoothkinds & SMOOTHFACETVERTICES) == 0))
    {
        return false;
    }
    if (vinfo->kind == SEGMENTVERTEX && ((smoothkinds & SMOOTHSEGMENTVERTICES) == 0))
    {
        return false;
    }
    if (vinfo->kind == FIXEDVERTEX && ((smoothkinds & SMOOTHFIXEDVERTICES) == 0))
    {
        return false;
    }
    
    switch (vinfo->kind)
    {
        case FREEVERTEX:
            stats.freesmoothattempts++;
            break;
        case FACETVERTEX:
            stats.facetsmoothattempts++;
            break;
        case SEGMENTVERTEX:
            stats.segmentsmoothattempts++;
            break;
        case FIXEDVERTEX:
            stats.fixedsmoothattempts++;
            break;
        default:
            printf("i don't know vertex type %d, dying\n", vinfo->kind);
            starexit(1);
    }
    stats.nonsmoothattempts++;
    
    /* get vertex to be altered */
    v = ((struct vertex *) tetcomplextag2vertex(mesh, vtx))->coord;
    
    /* save the original position of the vertex */
    vcopy(v, origpoint);
    
    /* find the worst quality of all incident tets */
    worstqual = HUGEFLOAT; 
    for (i=0; i<numincident; i++)
    {
        thisqual = tetquality(mesh,
                              incidenttets[i].verts[0],
                              incidenttets[i].verts[1],
                              incidenttets[i].verts[2],
                              incidenttets[i].verts[3],
                              improvebehave.qualmeasure);
        
        /* is this the worst quality we've seen? */
        if (thisqual < worstqual) worstqual = thisqual;
    }
    
    if (worstqual <= 0.0)
    {
        printf("Starting up smoothing, input tet has quality %g\n", worstqual);
    }
    assert(worstqual > 0.0);
    
    /* if we're using surface quadrics */
    /* if (improvebehave.usequadrics && vinfo->kind != FREEVERTEX) */
    if (improvebehave.usequadrics && vinfo->kind == FIXEDVERTEX)
    {
        /* this vertex had better have an initialized quadric */
        assert(hasquadric(vtx));
        
        /* check whether the surface quadric is the worse than all the tets */
        qe = quadricerrortet(mesh, vtx);
        if (qe < worstqual) 
        {
            if (improvebehave.verbosity > 5)
            {
                printf("Vertex %d's quadric error (%g raw, %g normalized) is worse than any tet quality!\n", (int) vtx, quadricerror(mesh, vtx), qe);
            }
            worstqual = qe;
        }
    }
    
    *outworstqual = worstqual;
    
    allgrads[0] = allgrads[1] = allgrads[2] = 0.0;
    
    /* if this is a fixed vertex, we have to compute the plane that it
         can move in first. do this by first getting the volume gradient for all tets */
    /* we're not doing this for now, vertices are free to move anywhere */
    if (vinfo->kind == FIXEDVERTEX && false)
    {
        for (i=0; i<numincident; i++)
        {
            /* compute gradient info for this tet, faking that it's free */
            getoptinfo(mesh, &incidenttets[i], FREEVERTEX, vinfo->vec);
            /* accumulate the volume gradients as we go */
            vadd(incidenttets[i].volumegrad, allgrads, allgrads);
        }
        /* normalize the summed gradient vector */
        vscale(1.0 / vlength(allgrads), allgrads, allgrads);
        vcopy(allgrads, vinfo->vec);
        
        if (improvebehave.verbosity > 5)
        {
            printf("here are the incident tets:\n");
            printopttets(mesh, incidenttets, numincident);
            printf("normal to the constant volume plane is (%g %g %g)\n",  vinfo->vec[0], vinfo->vec[1], vinfo->vec[2]);
        }
    }
    
    /* identify the active set A of quality functions that are
       nearly as bad as f, the worst of them all */
    for (i=0; i<numincident; i++)
    {
        /* compute gradient info for this tet */
        getoptinfo(mesh, &incidenttets[i], vinfo->kind, vinfo->vec);
    }
    getactiveset(mesh,
                 incidenttets,
                 numincident,
                 activegrads,
                 &numactive,
                 worstqual);
    if (improvebehave.anisotropic == false)
    {
        assert(numactive > 0);
    }
    else if (numactive == 0)
    {
        return false;
    }
    
    /* d <- point on the convex hull of all gradients nearest origin */
    minconvexhullpoint(activegrads, numactive, d);
    
    /* if d is the origin, we can't improve this vertex with smoothing. */
    dlength = vlength(d);
    if (dlength < DEPSILON)
    {
        return false;
    }
    
    if (improvebehave.verbosity > 5 && vinfo->kind == FIXEDVERTEX)
    {
        starreal normald[3];
        vscale(1.0 / dlength, d, normald);
        printf("the search direction (length %g) for fixed is (%g, %g, %g)\n", dlength, normald[0], normald[1], normald[2]);
    }
    
    /* otherwise, it's time to do some smoothing! */
    do
    {
        /* find r, the expected rate of improvement. r is computed as the minimum
           dot product between the improvment direction d and all of the active 
           gradients, so it's like "how fast do we move in the direction of the
           gradient least favored by d." */
        
        /* start with an absurdly big r */
        r = HUGEFLOAT;
        /* find the smallest dot product */
        for (i=0; i<numactive; i++)
        {
            newr = dot(d,activegrads[i]);
            
            if (newr <= 0.0)
            {
                if (improvebehave.verbosity > 5)
                {
                    printf("\n\n r=%g is negative, hmm.\n",newr);
                    printf("d is (%g %g %g)\n", d[0], d[1], d[2]);
                    printf("the gradient in question i=%d is (%.17g %.17g %.17g)\n",i,activegrads[i][0],activegrads[i][1],activegrads[i][2]);
                    printf("here are all the gradients:\ngrads=[");
                    for(k=0; k<numactive; k++)
                    {
                        printf("%.17g %.17g %.17g;\n", activegrads[k][0],activegrads[k][1],activegrads[k][2]);
                    }
                    printf("]\nd=[%.17g %.17g %.17g]\n", d[0], d[1], d[2]);
                }
                
                /* if we have already moved the vertex some, this is still a success */
                if (vequal(v, origpoint) == false)
                {
                        /* record this change in the journal */
                        
                        verts[0] = vtx;
                        verts[1] = incidenttets[0].verts[1];
                        verts[2] = incidenttets[0].verts[2];
                        verts[3] = incidenttets[0].verts[3];
                        
                        insertjournalentry(mesh, SMOOTHVERTEX, verts, 4, origpoint, v);
                        
                        *outworstqual = worstqual;
                        
                        if (improvebehave.verbosity > 5)
                        {
                            textcolor(BRIGHT, RED, BLACK);
                            printf("Just recorded a smooth that ended with d going bad!\n");
                            textcolor(RESET, WHITE, BLACK);
                        }
                        
                        /* record stats */
                        switch (vinfo->kind)
                        {
                            case FREEVERTEX:
                                stats.freesmoothsuccesses++;
                                break;
                            case FACETVERTEX:
                                stats.facetsmoothsuccesses++;
                                break;
                            case SEGMENTVERTEX:
                                stats.segmentsmoothsuccesses++;
                                break;
                            case FIXEDVERTEX:
                                stats.fixedsmoothsuccesses++;
                                break;
                            default:
                                printf("i don't know vertex type %d, dying\n", vinfo->kind);
                                starexit(1);
                        }
                        stats.nonsmoothsuccesses++;
                        return true;
                }
                else
                {
                    return false;
                }
            }
            
            /* this had better be positive */
            assert(newr > 0.0);
            
            if (newr < r)
            {
                r = newr;
            }
        }
        
        /* save the worst quality from the previous step */
        oldworstqual = worstqual;
        
        /* initialize alpha to the nearest intersection with another
           quality function */
        alpha = getinitialalpha(mesh,
                                incidenttets,
                                numincident,
                                d, r, worstqual);
        
        if (improvebehave.anisotropic == false)
        {
            assert(alpha > 0.0);
        }
        else
        {
            assert(alpha >= 0.0);
        }

        /* if we didn't find a limit for alpha above, at least limit it
           so that we don't invert any elements. */
        if (alpha == HUGEFLOAT)
        {
            if (improvebehave.verbosity > 5) printf("using volume gradients to pick alpha...\n");
            
            for (i=0; i<numincident; i++)
            {
                /* if moving in the direction d will decrease 
                   this element's volume */
                rate = dot(d,incidenttets[i].volumegrad);
                if (rate < 0.0)
                {
                    newalpha = -incidenttets[i].volume / (2.0 * rate);
                    
                    /* if this is smaller than the current step size,
                       use it */
                    if (newalpha < alpha)
                    {
                        alpha = newalpha;
                    }
                }
            }
        }
        
        if (improvebehave.verbosity > 5) printf("In non-smooth opt, found alpha_0 = %g, numiter = %d\n", alpha, numiter);
        
        /* do normal line search */
        nonsmoothlinesearch(mesh, vtx, d, worstqual, &alpha, r, incidenttets, numincident, vinfo->kind, vinfo->vec);
        assert(alpha >= 0.0);
        
        /* move vertex in direction d step size alpha */
        vscale(alpha, d, change);
        vadd(v, change, v);
         
        /* recompute quality information */
        oldworstqual = worstqual;
        worstqual = HUGEFLOAT; 
        for (i=0; i<numincident; i++)
        {
            thisqual = tetquality(mesh,
                                  incidenttets[i].verts[0],
                                  incidenttets[i].verts[1],
                                  incidenttets[i].verts[2],
                                  incidenttets[i].verts[3],
                                  improvebehave.qualmeasure);
            
            if (thisqual < 0.0)
            {
                printf("Just smooothed a vertex then found a tet with negative quality.\n");
                printf("alpha = %g\n", alpha);
            }
            
            /* is this the worst quality we've seen? */
            if (thisqual < worstqual) worstqual = thisqual;
        }
        
        /* if we're using surface quadrics */
        /* if (improvebehave.usequadrics && vinfo->kind != FREEVERTEX) */
        if (improvebehave.usequadrics && vinfo->kind == FIXEDVERTEX)
        {
            /* this vertex had better have an initialized quadric */
            assert(hasquadric(vtx));

            /* check whether the surface quadric is the worse than all the tets */
            qe = quadricerrortet(mesh, vtx);
            if (qe < worstqual) 
            {
                if (improvebehave.verbosity > 5)
                {
                    printf("Vertex %d's quadric error (%g raw, %g normalized) is worse than any tet quality AFTER vertex move!\n", (int) vtx, quadricerror(mesh, vtx), qe);
                }
                worstqual = qe;
            }
        }
        
        if (improvebehave.verbosity > 5 || worstqual < 0.0) printf("After moving vertex worstqual went from %g to %g, diff of %g\n\n",oldworstqual,worstqual,worstqual-oldworstqual);
        
        assert(worstqual >= 0.0);
        
        /* how much did we improve this step? */
        improvement = worstqual - oldworstqual;
        /*if (improvement < 0)*/
        if (1 == 0)
        {    
            textcolor(BRIGHT, RED, BLACK);
            printf("whoa, negative improvement!\n");
            printf("In non-smooth opt, found alpha_0 = %g, numiter = %d\n", alpha, numiter);
            printf("After moving vertex worstqual went from %g to %g, diff of %g\n\n",oldworstqual,worstqual,worstqual-oldworstqual);
            textcolor(RESET, WHITE, BLACK);
            
            /* move point back to original position */
            vcopy(origpoint, v);
            return false;
        }
        assert(improvement >= 0);
        
        /* recompute the active set */
        for (i=0; i<numincident; i++)
        {
            /* compute gradient info for this tet */
            getoptinfo(mesh, &incidenttets[i], vinfo->kind, vinfo->vec);
        }
        getactiveset(mesh,
                     incidenttets,
                     numincident,
                     activegrads,
                     &numactive,
                     worstqual);
        assert(numactive > 0);
        
        /* d <- point on the convex hull of all gradients nearest origin */
        minconvexhullpoint(activegrads, numactive, d);
        
        numiter++;
        
        dlength = vlength(d);
    } while ((dlength > DEPSILON) && 
             (numiter < MAXSMOOTHITER) &&
             (improvement > MINSMOOTHITERIMPROVE));
    
    if (improvebehave.verbosity > 5)
    {
        printf("finished optimizing a vertex. d=(%g, %g, %g), numiter=%d, improvement=%g\n\n",d[0],d[1],d[2],numiter,improvement);
    }
    
    /* record this change in the journal */
    verts[0] = vtx;
    verts[1] = incidenttets[0].verts[1];
    verts[2] = incidenttets[0].verts[2];
    verts[3] = incidenttets[0].verts[3];
    
    insertjournalentry(mesh, SMOOTHVERTEX, verts, 4, origpoint, v);
    
    *outworstqual = worstqual;
    
    /* record stats */
    switch (vinfo->kind)
    {
        case FREEVERTEX:
            stats.freesmoothsuccesses++;
            break;
        case FACETVERTEX:
            stats.facetsmoothsuccesses++;
            break;
        case SEGMENTVERTEX:
            stats.segmentsmoothsuccesses++;
            break;
        case FIXEDVERTEX:
            stats.fixedsmoothsuccesses++;
            break;
        default:
            printf("i don't know vertex type %d, dying\n", vinfo->kind);
            starexit(1);
    }
    stats.nonsmoothsuccesses++;
    return true;
}

/* optimization-based smoothing for a single vertex
   v1, part of the existing tet (1,2,3,4) */
bool nonsmoothsinglevertex(struct tetcomplex *mesh,
                        tag v1,
                        tag v2,
                        tag v3,
                        tag v4,
                        starreal *worstout,
                        int kinds)
{
    int numincident = 0;
    bool noghosts = true;
    int incIter;
    bool smoothed;
    
    /* a list of all the tets incident to this vertex */
    tag incidenttettags[MAXINCIDENTTETS][4];
    /* a list of information about all the incident tets */
    struct opttet incidenttets[MAXINCIDENTTETS];
    
    if (tetexists(mesh, v1, v2, v3, v4) == false)
    {
        return false;
    }
    
    /* don't smooth if smoothing disabled */
    if (improvebehave.nonsmooth == 0) return false;
    
    getincidenttets(mesh, 
                    v1,
                    v2,
                    v3,
                    v4, 
                    incidenttettags,
                    &numincident,
                    &noghosts);
    assert(numincident > 0);
    
    if (improvebehave.verbosity > 5)
    {
        printf("here are the incident tets:\n");
        printtets(mesh, incidenttettags, numincident);
    }
    
    /* copy tags to incident tet data structure */
    for (incIter=0; incIter<numincident; incIter++)
    {
        /* copy vertex tags */
        incidenttets[incIter].verts[0] = incidenttettags[incIter][0];
        incidenttets[incIter].verts[1] = incidenttettags[incIter][1];
        incidenttets[incIter].verts[2] = incidenttettags[incIter][2];
        incidenttets[incIter].verts[3] = incidenttettags[incIter][3];
    }
    
    /* smooth the new vertex. */
    smoothed = nonsmooth(mesh,
                         v1,
                         incidenttets,
                         numincident,
                         worstout,
                         kinds);
    
    if (smoothed) return true;
    return false;
}

/* combination lap/opt smoothing for a single vertex
   v1, part of the existing tet (1,2,3,4) */
bool smoothsinglevertex(struct tetcomplex *mesh,
                        tag v1,
                        tag v2,
                        tag v3,
                        tag v4,
                        starreal threshold,
                        starreal *worstout,
                        int kinds,
                        int *optattempts,
                        int *optsuccesses,
                        struct arraypoolstack *outstack)
{
    int numincident = 0;
    bool noghosts = true;
    int i;
    bool optsmoothed=false;
    struct improvetet *outtet;
    starreal thisqual, startworstqual=HUGEFLOAT;
    
    /* a list of all the tets incident to this vertex */
    tag incidenttettags[MAXINCIDENTTETS][4];
    /* a list of information about all the incident tets */
    struct opttet incidenttets[MAXINCIDENTTETS];
    
    if (tetexists(mesh, v1, v2, v3, v4) == false)
    {
        return false;
    }
    
    getincidenttets(mesh, 
                    v1,
                    v2,
                    v3,
                    v4, 
                    incidenttettags,
                    &numincident,
                    &noghosts);
    assert(numincident > 0);
    
    if (improvebehave.verbosity > 5)
    {
        printf("here are the incident tets:\n");
        printtets(mesh, incidenttettags, numincident);
    }
    
    /* copy tags to incident tet data structure */
    for (i=0; i<numincident; i++)
    {
        /* copy vertex tags */
        incidenttets[i].verts[0] = incidenttettags[i][0];
        incidenttets[i].verts[1] = incidenttettags[i][1];
        incidenttets[i].verts[2] = incidenttettags[i][2];
        incidenttets[i].verts[3] = incidenttettags[i][3];
        
        /* compute starting worst quality */
        if (SMOOTHPARANOID)
        {
            thisqual = tetquality(mesh, 
                                  incidenttets[i].verts[0],
                                  incidenttets[i].verts[1],
                                  incidenttets[i].verts[2],
                                  incidenttets[i].verts[3],
                                  improvebehave.qualmeasure);
        
            /* is this the worst quality incident tet? */
            if (thisqual < startworstqual) startworstqual = thisqual;
        }
    }
    
    /* if that fails to acheive desired quality, use non-smooth optimization-based smoothing */
    if (improvebehave.nonsmooth)
    {
        (*optattempts)++;
        optsmoothed = nonsmooth(mesh,
                                v1,
                                incidenttets,
                                numincident,
                                worstout,
                                kinds);
                                
        if (optsmoothed) (*optsuccesses)++;
    }

    if (optsmoothed)
    {
        /* if we are given an output stack, add all the incident tets to it*/
        if (outstack != NULL)
        {
            for (i=0; i<numincident; i++)
            {
                /* if we don't already have this one */
                if (tetinstack(outstack,
                               incidenttettags[i][0],
                               incidenttettags[i][1],
                               incidenttettags[i][2],
                               incidenttettags[i][3]) == false)
                {
                    outtet = (struct improvetet *) stackpush(outstack);
                    outtet->verts[0] = incidenttettags[i][0];
                    outtet->verts[1] = incidenttettags[i][1];
                    outtet->verts[2] = incidenttettags[i][2];
                    outtet->verts[3] = incidenttettags[i][3];
                }
            }
        }
        return true;
    }
    return false;
}

/* perform a pass of combination Laplacian / optimization-based smoothing. */
#define SKIPCHECKSIZE 0
bool smoothpass(struct tetcomplex *mesh,
                struct arraypoolstack *tetstack,
                struct arraypoolstack *outstack,
                struct arraypoolstack *influencestack,
                int qualmeasure,
                starreal threshold,
                starreal bestmeans[],
                starreal meanqualafter[],
                starreal *minqualafter,
                int smoothkinds,
                bool quiet)
{
    struct improvetet *stacktet;   /* point to stack tet */
    struct improvetet *outtet;     /* point to stack tet */
    int origstacksize;             /* number of tets in the original stack */
    int optattempts=0;             /* number of times optimization-based is tried */
    int optsuccesses=0;            /* number of times it succeeds */
    int fixedverts[2] = {0,0};
    int facetverts[2] = {0,0};
    int segmentverts[2] = {0,0};
    int freeverts[2] = {0,0};
    starreal worstqual;
    int i,j,k,l;                   /* loop indices */
    bool smoothed;                 /* was smoothing successful? */
    int kind;                      /* number of degrees of freedom for vertex */
    int beforeid = lastjournalentry();
    starreal minqualbefore = HUGEFLOAT;
    starreal meanqualbefore[NUMMEANTHRESHOLDS];
    int nonexist = 0;
    struct vertextype *smoothedvert;
    struct arraypool smoothedverts; /* list of vertices that have already been smoothed */
    bool *thisvertsmoothed;
    bool skip = false;
    bool dynfailcondition = true;
    
    *minqualafter = HUGEFLOAT;
    
    origstacksize = tetstack->top + 1;
    
    /* reset output stack */
    if (outstack != NULL)
    {
        assert(influencestack != NULL);
        
        /* copy the input stack to output stack */
        stackrestart(outstack);
        copystack(tetstack, outstack);
    }
    
    if (improvebehave.verbosity > 5 && outstack != NULL)
    {
        printf("input stack:\n");
        printstack(mesh, tetstack);
    }
    
    /* because if we smooth each vertex of each tet, we'll smooth some vertices
       over and over again, keep track of which vertices have been smoothed */
    
    /* initialize the list */
    arraypoolinit(&smoothedverts, sizeof(bool), LOG2TETSPERSTACKBLOCK, 0);
    if (origstacksize > SKIPCHECKSIZE)
    {
        /* set initial value of "smoothed or not" for every vertex */
        initsmoothedvertlist(tetstack, &smoothedverts);
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
    
    /* try to pop all the tets off the stack */
    while (tetstack->top != STACKEMPTY)
    {
        /* pull the top tet off the stack */
        stacktet = (struct improvetet *) stackpop(tetstack);
        
        /* make sure this tet exists */
        if (tetexists(mesh, stacktet->verts[0], 
                            stacktet->verts[1], 
                            stacktet->verts[2], 
                            stacktet->verts[3]) == false)
        {
            nonexist++;
            continue;
        }
        
        if (outstack != NULL)
        {
            /* save this tet in the influenced tets stack */
            outtet = (struct improvetet *) stackpush(influencestack);
            outtet->quality = 0.0;
            outtet->verts[0] = stacktet->verts[0];
            outtet->verts[1] = stacktet->verts[1];
            outtet->verts[2] = stacktet->verts[2];
            outtet->verts[3] = stacktet->verts[3];
        }
        
        /* we're not doing simultaneous smoothing; try to smooth each vertex of this tet */
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
            
            skip = false;
            
            /* if we have not yet smoothed this vertex */
            if (origstacksize > SKIPCHECKSIZE)
            {
                thisvertsmoothed = (bool *) arraypoollookup(&smoothedverts, stacktet->verts[i]);
                assert(thisvertsmoothed != NULL);
                if (*thisvertsmoothed == true)
                {
                    if (improvebehave.verbosity > 5)
                    {
                        printf("skipping double smoothing of vertex %d\n", (int) stacktet->verts[i]);
                    }
                    skip = true;
                }
            }
            
            if (skip == false)
            {
                if (origstacksize > SKIPCHECKSIZE)
                {
                    *((bool *) arraypoolforcelookup(&smoothedverts, stacktet->verts[i])) = true;
                }
                
                /* record that we have smoothed this vertex */
                arraypoolforcelookup(&smoothedverts, stacktet->verts[i]);
                
                /* look up vertex type */
                smoothedvert = (struct vertextype *) arraypoollookup(&vertexinfo, stacktet->verts[i]);
                assert(smoothedvert != NULL);
                assert(smoothedvert->kind != INPUTVERTEX);
                kind = smoothedvert->kind;
                /* record smoothing attempt */
                switch (kind)
                {
                    case FREEVERTEX: 
                        freeverts[0]++;
                        break;
                    case FACETVERTEX:
                        facetverts[0]++;
                        break;
                    case SEGMENTVERTEX:
                        segmentverts[0]++;
                        break;
                    case FIXEDVERTEX:
                        fixedverts[0]++;
                        break;
                    default:
                        printf("bizarre vertex type %d in smooth, dying\n", kind);
                        starexit(1);
                        break;
                }
                
                /* try to smooth it */
                /* only record affected tets if we were given an output stack */
                worstqual = 1e-100;
                smoothed = smoothsinglevertex(mesh,
                                   stacktet->verts[i],
                                   stacktet->verts[j],
                                   stacktet->verts[k],
                                   stacktet->verts[l],
                                   threshold,
                                   &worstqual,
                                   smoothkinds,
                                   &optattempts,
                                   &optsuccesses,
                                   influencestack);
                
                /* record number of successful smooths for this type of vertex */
                if (smoothed)
                {
                    switch (kind)
                    {
                        case FREEVERTEX: 
                            freeverts[1]++;
                            break;
                        case FACETVERTEX:
                            facetverts[1]++;
                            break;
                        case SEGMENTVERTEX:
                            segmentverts[1]++;
                            break;
                        case FIXEDVERTEX:
                            fixedverts[1]++;
                            break;
                    }
                }
                
                if (improvebehave.verbosity > 5 && optattempts % 1000 == 0 && quiet == false && optattempts != 0)
                {
                    printf("Attempted %d smooths, stack size currently %lu\n", optattempts, tetstack->top);
                }
            }
        }
    }
    
    if (outstack != NULL)
    {
        /* check the quality of all influenced tets */
        stackquality(mesh, influencestack, qualmeasure, meanqualafter, minqualafter);
        
        if (improvebehave.verbosity > 4 && quiet == false)
        {
            printf("just finished smoothing pass.\n");
            printf("    in stack size: %d - %d nonexistent = %d\n", origstacksize, nonexist, origstacksize-nonexist);
            printf("    influence stack size: %lu\n", influencestack->top+1);
            printf("    before min qual is %g\n", pq(minqualbefore));
            printf("    after min qual is %g\n", pq(*minqualafter));
            printf("    Mean qualities before:\n");
            printmeans(meanqualbefore);
            printf("    Mean qualities after:\n");
            printmeans(meanqualafter);
        }
        
        /* check for success for local dynamic improvement */
        if (improvebehave.dynimprove && quiet == false)
        {
            if (localmeanimprove(meanqualbefore, meanqualafter, 0.001) && *minqualafter >= minqualbefore)
            {
                dynfailcondition = false;
            }
        }
        
        /* the output stack must have at least as many tets as the input stack */
        assert(influencestack->top + 1 >= origstacksize - nonexist);
        
        /* if we failed to improve the worst quality, undo this pass */
        if (*minqualafter <= minqualbefore && dynfailcondition)
        {
            if (improvebehave.verbosity > 5)
            {
                printf("Undoing last smoothing pass, minqualbefore was %g but after was %g\n", minqualbefore, *minqualafter);
            }
            invertjournalupto(mesh, beforeid);
            
            /* reset minimum output quality */
            *minqualafter = minqualbefore;
            
            /*starreal invertworst;
            stackquality(mesh, outstack, qualmeasure, meanqualafter, &invertworst);
            
            if (invertworst != minqualbefore)
            {
                printf("after inverting journal, worst is %g, before pass it was %g\n", invertworst, minqualbefore);
            }
            
            assert(invertworst == minqualbefore); */
            
            arraypooldeinit(&smoothedverts);
            return false;
        }
    }
    
    if (quiet == false && outstack == NULL)
    {
        /* what's the worst quality element in the mesh now? */
        meshquality(mesh, qualmeasure, meanqualafter, minqualafter);
    }
    
    /* print report */
    if (improvebehave.verbosity > 3 && quiet == false)
    {
        printf("Completed pass of optimization-based smoothing on stack of %d tets.\n", origstacksize);
        if (improvebehave.verbosity > 5)
        {
            if (bestmeans != NULL)
            {
                printf("    Best previous means:\n");
                printmeans(bestmeans);
            }
            printf("    Mean qualities after:\n");
            printmeans(meanqualafter);
        }
        printf("    Worst quality before: %g\n", pq(minqualbefore));
        if (minqualbefore < *minqualafter)
        {
            textcolor(BRIGHT, GREEN, BLACK);
        }
        printf("    Worst quality after:  %g\n", pq(*minqualafter));
        textcolor(RESET, WHITE, BLACK);
        if (improvebehave.verbosity > 4)
        {
            printf("    Optimization smoothing attempts:  %d\n", optattempts);
            printf("    Optimization smoothing successes: %d\n", optsuccesses);
            printf("    Free vertices (succ/tries):       %d/%d\n", freeverts[1], freeverts[0]); 
            printf("    Facet vertices (succ/tries):      %d/%d\n", facetverts[1], facetverts[0]);
            printf("    Segment vertices (succ/tries):    %d/%d\n", segmentverts[1], segmentverts[0]);
            printf("    Fixed vertices (succ/tries):      %d/%d\n", fixedverts[1], fixedverts[0]);
        }
    }
    
    /* free list of smoothed verts */
    arraypooldeinit(&smoothedverts);
    return true;
}
