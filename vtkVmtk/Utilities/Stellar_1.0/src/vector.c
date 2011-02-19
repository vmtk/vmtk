/*****************************************************************************/
/*                                                                           */
/*  Simple vector math convenience functions                                 */
/*                                                                           */
/*****************************************************************************/

/* compute the dot product of two vectors u and v */
starreal dot(starreal u[3],
           starreal v[3])
{
    return u[0]*v[0] + u[1]*v[1] + u[2]*v[2];
}

/* compute the cross product of two vectors u and v */
void cross(starreal u[3],
           starreal v[3],
           starreal prod[3])
{
    prod[0] = u[1]*v[2] - u[2]*v[1];
    prod[1] = u[2]*v[0] - u[0]*v[2];
    prod[2] = u[0]*v[1] - u[1]*v[0];
}

/* scale a vector by a constant */
void vscale(starreal scale,
            starreal v[3],            
            starreal scaled[3])
{
    scaled[0] = v[0] * scale;
    scaled[1] = v[1] * scale;
    scaled[2] = v[2] * scale;
}

/* add two vectors u and v */
void vadd(starreal u[3],
          starreal v[3],
          starreal sum[3])
{
    sum[0] = u[0] + v[0];
    sum[1] = u[1] + v[1];
    sum[2] = u[2] + v[2];
}

/* subtract two vectors u and v */
void vsub(const starreal u[3],
          const starreal v[3],
          starreal sum[3])
{
    sum[0] = u[0] - v[0];
    sum[1] = u[1] - v[1];
    sum[2] = u[2] - v[2];
}

/* copy one vector's values into another */
void vcopy(starreal u[3],
           starreal v[3])
{
    v[0] = u[0];
    v[1] = u[1];
    v[2] = u[2];
}

/* return the length of a vector */
starreal vlength(starreal u[3])
{
    return sqrt(u[0]*u[0] + u[1]*u[1] + u[2]*u[2]);
}

/* return if u and v are the same vector, false otherwise */
bool vequal(starreal u[3],
            starreal v[3])
{
    return (u[0] == v[0]) && (u[1] == v[1]) && (u[2] == v[2]);
}

/* compute the euclidean distance between two points u and v */
starreal vdist(starreal u[3],
             starreal v[3])
{
    starreal diff[3];
    vsub(u, v, diff);
    return vlength(diff);
}

/* project the vector u onto the vector v */
void vproject(starreal u[3],
              starreal v[3],
              starreal vout[3])
{
    starreal dotprod = dot(u,v);
    starreal length = vlength(v);
    if (length > 0)
    {
        vscale(dotprod / length, v, vout);
    }
    else
    {
        vscale(0.0, v, vout);
    }
}

/* project the vector v onto the plane through the origin with normal n */
void vprojecttoplane(starreal v[3],
                     starreal n[3],
                     starreal vout[3])
{
    starreal proj[3];
    vproject(v,n,proj);
    vsub(v,proj,vout);
}