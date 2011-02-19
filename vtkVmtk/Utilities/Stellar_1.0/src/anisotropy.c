/* Functions to transform space for anisotropic meshing */

/* given a single vector, return two more that are mutually
   orthogonal to it */
void getorthobasis(starreal v1[3],
                   starreal v2[3],
                   starreal v3[3])
{
    starreal invLen;
    
    if (fabs(v1[0]) > fabs(v1[1]))
    {
        invLen = 1.0 / sqrt(v1[0] * v1[0] + v1[2] * v1[2]);
        v2[0] = -v1[2] * invLen;
        v2[1] = 0.0;
        v2[2] = v1[0] * invLen;
    }
    else
    {
        invLen = 1.0 / sqrt(v1[1] * v1[1] + v1[2] * v1[2]);
        v2[0] = 0.0;
        v2[1] = v1[2] * invLen;
        v2[2] = -v1[1] * invLen;
    }
    cross(v1, v2, v3);
    
    assert(vlength(v1) > 0.0);
    assert(vlength(v2) > 0.0);
    assert(vlength(v3) > 0.0);
}

/* given three mutually orthogonal vectors and three corresponding
   eigenvalues, compute a transformation matrix E */
void tensorfromeigen(starreal v1[3],
                     starreal v2[3],
                     starreal v3[3],
                     starreal e1,
                     starreal e2,
                     starreal e3,
                     starreal E[3][3])
{
    starreal v1n[3], v2n[3], v3n[3];
    int i, j;
    
    /* normalize the basis vectors and scale them */
    vscale(e1 / vlength(v1), v1, v1n);
    vscale(e2 / vlength(v2), v2, v2n);
    vscale(e3 / vlength(v3), v3, v3n);
    
    /* compute transformation matrix */
    /* E = (v1 * v1') * e1 + (v2 * v2') * e2 + (v3 * v3') * e3 */
    for (i=0; i<3; i++)
    {
        for (j=0; j<3; j++)
        {
            E[i][j] = (v1n[i] * v1n[j]) + (v2n[i] * v2n[j]) + (v3n[i] * v3n[j]);
        }
    }
}

/* tensor field that compresses space orthogonal to lines radiating
   from the origin */
void sinktensor(starreal x,
                starreal y,
                starreal z,
                starreal E[3][3])
{
    starreal s;
    starreal v1[3], v2[3], v3[3];
    
    s = 2.0 * sqrt(x*x + y*y + z*z);
    /* check whether we're at the origin */
    if (s < 1.e-13)
    {
        x = 1.0;
        y = z = 0.0;
    }
    if (s > 1.0) s = 1.0;
    s += 0.8;
    
    /* set the scaled vector to be radially outward */
    v1[0] = x;
    v1[1] = y;
    v1[2] = z;
    assert(vlength(v1) > 0.0);
    
    /* find an orthogonal basis for this vector */
    getorthobasis(v1, v2, v3);
    
    /* build deformation tensor */
    tensorfromeigen(v1, v2, v3, 1.0, s, s, E);
}

/* tensor field that compresses space orthogonal to lines radiating
   from the origin */
void swirltensor(starreal x,
                 starreal y,
                 starreal z,
                 starreal E[3][3])
{
    starreal s;
    starreal v1[3], v2[3], v3[3];
    
    s = 5.0 * sqrt(x*x + y*y + z*z);
    /* check whether we're at the origin */
    if (s < 1.e-13)
    {
        x = 1.0;
        y = z = 0.0;
    }
    if (s > 1.0) s = 1.0;
    s += 1.0;
    
    /* set the scaled vector to be radially outward */
    v1[0] = x;
    v1[1] = y;
    v1[2] = z;
    
    /* find an orthogonal basis for this vector */
    getorthobasis(v1, v2, v3);
    
    /* build deformation tensor */
    tensorfromeigen(v1, v2, v3, s, 1.0, 1.0, E);
}

/* tensor field that runs sin(x) */
void sinetensor(starreal x,
                starreal y,
                starreal z,
                starreal E[3][3])
{
    starreal s;
    starreal v1[3], v2[3], v3[3];
    
    /*starreal period = 6.0;*/
    starreal period = 6.0;
    starreal scale = 1.8;
    
    /* let the y direction the vector be the derivative of sin(x), which is cos(x) */
    v1[0] = 1.0;
    v1[1] = scale * cos(x * period);
    v1[2] = 0.0;
    
    /* normalize */
    vscale(1.0 / vlength(v1), v1, v1);
    
    /* we'll scale the tensor down as it moves away from sin(x) */
    /*
    s = 1.0 / (0.6 + fabs(y*y - ((scale * sin(x * period)) * (scale * sin(x * period)))));
    s += 1.0;
    */
    /* nah, actually */
    s = 2.3;
    s = 2.1;
    
    /* find an orthogonal basis for this vector */
    getorthobasis(v1, v2, v3);
    
    if (improvebehave.verbosity > 6)
    {
        printf("in sine tensor, point is [%g %g %g], v1 = [%g %g %g], s = %g, sin(x)=%g, cos(x)=%g\n", x, y, z, v1[0], v1[1], v2[2], s, sin(x), cos(x));
    }
    
    /* build deformation tensor */
    /*
    tensorfromeigen(v1, v2, v3, 1.0, s, s, E);
    */
    /* norm */
    tensorfromeigen(v1, v2, v3, 1.0/s, 1.0, 1.0, E);
}

/* tensor with denser elements in the middle */
void centertensor(starreal x,
                  starreal y,
                  starreal z,
                  starreal E[3][3])
{
    starreal s;
    
    s = 0.7 + 3.0 / (sqrt(x*x + y*y + z*z) + 0.4);
    
    E[0][0] = s; E[0][1] = 0.0; E[0][2] = 0.0;
    E[1][0] = 0.0; E[1][1] = s; E[1][2] = 0.0;
    E[2][0] = 0.0; E[2][1] = 0.0; E[2][2] = s;
}

/* tensor with denser elements away from the middle */
void perimetertensor(starreal x,
                     starreal y,
                     starreal z,
                     starreal E[3][3])
{
    starreal s;
    
    s = (sqrt(x*x + y*y + z*z) + 0.2);
    
    E[0][0] = s; E[0][1] = 0.0; E[0][2] = 0.0;
    E[1][0] = 0.0; E[1][1] = s; E[1][2] = 0.0;
    E[2][0] = 0.0; E[2][1] = 0.0; E[2][2] = s;
}

/* tensor with denser elements on the +x side */
void righttensor(starreal x,
                 starreal y,
                 starreal z,
                 starreal E[3][3])
{
    starreal s;
    
    s = (x + 1.0);
    if (s < 1.0) s = 1.0;
    
    E[0][0] = s; E[0][1] = 0.0; E[0][2] = 0.0;
    E[1][0] = 0.0; E[1][1] = s; E[1][2] = 0.0;
    E[2][0] = 0.0; E[2][1] = 0.0; E[2][2] = s;
}

void scaletensor(starreal scalex,
                 starreal scaley,
                 starreal scalez,
                 starreal E[3][3])
{
    E[0][0] = scalex; E[0][1] = 0.0; E[0][2] = 0.0;
    E[1][0] = 0.0; E[1][1] = scaley; E[1][2] = 0.0;
    E[2][0] = 0.0; E[2][1] = 0.0; E[2][2] = scalez;
}

/* get scaling tensor by sampling strain field */
void straintensor(starreal x,
                  starreal y,
                  starreal z,
                  starreal E[3][3])
{
    starreal straineigval;
    starreal point[3];
    starreal scale;
    
    point[0] = x;
    point[1] = y;
    point[2] = z;
    
    straineigval = sqrt(G_evalstrain(point));
    
    if (improvebehave.verbosity > 0 && straineigval > 1.1)
    {
        printf("At point [%g %g %g], biggest eigenvalue is %g\n", x, y, z, straineigval);
    }
    
    /* scale up for stretched elements */
    scale = 1.0 / straineigval;
    if (scale > 1000.0) scale = 1000.0;
    scaletensor(scale, scale, scale, E);
}

/* blend the scaling tensor with the identity matrix */
void blendtensor(starreal E[3][3], starreal Eb[3][3])
{
    int i,j;
    
    starreal a = improvebehave.tensorblend;
    
    for (i=0; i<3; i++)
    {
        for (j=0; j<3; j++)
        {
            /* blend between tensor and tensorb */
            E[i][j] = (a * E[i][j]) + ((1.0 - a) * Eb[i][j]);
        }
    }
}

/* return the deformation tensor for the query point */
void deformtensor(starreal x, 
                  starreal y, 
                  starreal z, 
                  starreal E[3][3])
{
    starreal Eb[3][3];
    
    starreal ox, oy, oz;
    ox = x;
    oy = y;
    oz = z;
    
    /* compute the normalized vertex position */
    /* translate to the center */
    x -= G_center[0];
    y -= G_center[1];
    z -= G_center[2];
    /* scale */
    x /= G_range[0];
    y /= G_range[1];
    z /= G_range[2];
    
    switch (improvebehave.tensor)
    {
        case NOTENSOR:
            scaletensor(1.0, 1.0, 1.0, E);
            break;
        case STRETCHX:
            scaletensor(3.0, 1.0, 1.0, E);
            break;
        case STRETCHY:
            scaletensor(1.0, 3.0, 1.0, E);
            break;
        case SINK:
            sinktensor(x, y, z, E);
            break;
        case SWIRL:
            swirltensor(x, y, z, E);
            break;
        case CENTER:
            centertensor(x, y, z, E);
            break;
        case PERIMETER:
            perimetertensor(x, y, z, E);
            break;
        case RIGHT:
            righttensor(x, y, z, E);
            break;
        case SINE:
            sinetensor(x, y, z, E);
            break;
        case STRAIN:
            straintensor(ox, oy, oz, E);
            break;
        default:
            printf("Oops, don't know the tensor type %d, dying.\n", improvebehave.tensor);
            break;
    }
    
    switch (improvebehave.tensorb)
    {
        case NOTENSOR:
            scaletensor(1.0, 1.0, 1.0, Eb);
            break;
        case STRETCHX:
            scaletensor(3.0, 1.0, 1.0, Eb);
            break;
        case STRETCHY:
            scaletensor(1.0, 3.0, 1.0, Eb);
        case SINK:
            sinktensor(x, y, z, Eb);
            break;
        case SWIRL:
            swirltensor(x, y, z, Eb);
            break;
        case CENTER:
            centertensor(x, y, z, Eb);
            break;
        case PERIMETER:
            perimetertensor(x, y, z, Eb);
            break;
        case RIGHT:
            righttensor(x, y, z, Eb);
            break;
        default:
            printf("Oops, don't know the tensor type %d, dying.\n", improvebehave.tensor);
            break;
    }
    
    /* blend the tensor with the identity matrix */
    blendtensor(E, Eb);
}

/* fetch the deformation tensor sampled at the barycenter of
   the specified tetrahedron */
void tettensor(struct tetcomplex *mesh,
               tag vtx1,
               tag vtx2,
               tag vtx3,
               tag vtx4,
               starreal E[3][3])
{
    starreal *point[4];
    starreal barycenter[3] = {0.0, 0.0, 0.0};
    int i;
    
    /* get tet vertices */
    point[0] = ((struct vertex *) tetcomplextag2vertex(mesh, vtx1))->coord;
    point[1] = ((struct vertex *) tetcomplextag2vertex(mesh, vtx2))->coord;
    point[2] = ((struct vertex *) tetcomplextag2vertex(mesh, vtx3))->coord;
    point[3] = ((struct vertex *) tetcomplextag2vertex(mesh, vtx4))->coord;
    
    /* compute barycenter */
    for (i=0; i<4; i++)
    {
        vadd(point[i], barycenter, barycenter);
    }
    vscale(0.25, barycenter, barycenter);
    
    /* fetch the deformation tensor at the tet barycenter */
    deformtensor(barycenter[0], barycenter[1], barycenter[2], E);
    
    if (improvebehave.verbosity > 5)
    {
        printf("Computed deformation tensor for tet (%d %d %d %d):\n", (int) vtx1, (int) vtx2, (int) vtx3, (int) vtx4);
        printf("    v1 = (%g %g %g)\n", point[0][0], point[0][1], point[0][2]);
        printf("    v2 = (%g %g %g)\n", point[1][0], point[1][1], point[1][2]);
        printf("    v3 = (%g %g %g)\n", point[2][0], point[2][1], point[2][2]);
        printf("    v4 = (%g %g %g)\n", point[3][0], point[3][1], point[3][2]);
        printf("    barycenter = (%g %g %g)\n", barycenter[0], barycenter[1], barycenter[2]);
        printf("    E = [%g %g %g]\n", E[0][0], E[0][1], E[0][2]);
        printf("        [%g %g %g]\n", E[1][0], E[1][1], E[1][2]);
        printf("        [%g %g %g]\n", E[2][0], E[2][1], E[2][2]);
    }
}

/* fetch the deformation tensor sampled at the midpoint of the specified edge */
void edgetensor(struct tetcomplex *mesh,
               tag vtx1,
               tag vtx2,
               starreal E[3][3])
{
    starreal *point[2];
    starreal midpoint[3] = {0.0, 0.0, 0.0};
    int i;
    
    /* get edge vertices */
    point[0] = ((struct vertex *) tetcomplextag2vertex(mesh, vtx1))->coord;
    point[1] = ((struct vertex *) tetcomplextag2vertex(mesh, vtx2))->coord;
    
    /* compute barycenter */
    for (i=0; i<2; i++)
    {
        vadd(point[i], midpoint, midpoint);
    }
    vscale(0.5, midpoint, midpoint);
    
    /* fetch the deformation tensor at the tet barycenter */
    deformtensor(midpoint[0], midpoint[1], midpoint[2], E);
    
    if (improvebehave.verbosity > 5)
    {
        printf("Computed deformation tensor for edge (%d %d):\n", (int) vtx1, (int) vtx2);
        printf("    v1 = (%g %g %g)\n", point[0][0], point[0][1], point[0][2]);
        printf("    v2 = (%g %g %g)\n", point[1][0], point[1][1], point[1][2]);
        printf("    midpoint = (%g %g %g)\n", midpoint[0], midpoint[1], midpoint[2]);
        printf("    E = [%g %g %g]\n", E[0][0], E[0][1], E[0][2]);
        printf("        [%g %g %g]\n", E[1][0], E[1][1], E[1][2]);
        printf("        [%g %g %g]\n", E[2][0], E[2][1], E[2][2]);
    }
}

/* transform a point to isotropic space with deformation tensor E */
void tensortransform(starreal p[3], starreal pout[3], starreal E[3][3])
{
    starreal pt[3];
    
    /* copy p to temp in case p = pout */
    pt[0] = p[0];
    pt[1] = p[1];
    pt[2] = p[2];
    
    /* compute pout = Ep */
    pout[0] = (E[0][0] * pt[0]) + (E[0][1] * pt[1]) + (E[0][2] * pt[2]);
    pout[1] = (E[1][0] * pt[0]) + (E[1][1] * pt[1]) + (E[1][2] * pt[2]);
    pout[2] = (E[2][0] * pt[0]) + (E[2][1] * pt[1]) + (E[2][2] * pt[2]);
}
