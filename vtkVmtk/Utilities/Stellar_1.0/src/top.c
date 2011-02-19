/*****************************************************************************/
/*                                                                           */
/*   ,d88""\   d8             888 888                                        */
/*   8888    _d88__  e88888e  888 888   o8888o  88o88o                       */
/*   `Y88b    888   d888  88b 888 888       88b 888                          */
/*    `Y88b,  888   8888oo888 888 888  o888o888 888                          */
/*      8888  888   q888      888 888 C888  888 888                          */
/*   \_o88P'  "88o"  "88oooo" 888 888  "888"888 888                          */
/*                                                                           */
/* A program for improving tetrahedral meshes, using the tetrahedral complex */
/* library provided by Starbase (see Starbase.c for details)                 */
/*                                                                           */
/* Version 1.0                                                               */
/* 18 December 2008                                                          */
/*                                                                           */
/* Copyright 2006-2008 Bryan Klingner, all rights reserved.                  */
/*                                                                           */
/*  ======================= BSD license begins here. ======================= */
/*                                                                           */
/*  Redistribution and use in source and binary forms, with or without       */
/*  modification, are permitted provided that the following conditions are   */
/*  met:                                                                     */
/*                                                                           */
/*  - Redistributions of source code must retain the above copyright notice, */
/*    this list of conditions and the following disclaimer.                  */
/*                                                                           */
/*  - Redistributions in binary form must reproduce the above copyright      */
/*    notice, this list of conditions and the following disclaimer in the    */
/*    documentation and/or other materials provided with the distribution.   */
/*                                                                           */
/*  - Neither the name of Bryan Klingner nor the name of the University      */
/*    of California nor the names of its contributors may be used to endorse */
/*    or promote products derived from this software without specific prior  */
/*    written permission.                                                    */
/*                                                                           */
/*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS  */
/*  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED    */
/*  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A          */
/*  PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT      */
/*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,    */
/*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED */
/*  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR   */
/*  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF   */
/*  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING     */
/*  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS       */
/*  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.             */
/*                                                                           */
/*  ======================== BSD license ends here. ======================== */
/*                                                                           */
/* For details on the mesh improvement techniques implemented in Stellar,    */
/* refer to the paper:                                                       */
/* "Aggressive Tetrahedral Mesh Improvement," Bryan M. Klingner, Jonathan R. */
/*  Shewchuk, Proceedings of the 16th International Meshing Roundtable       */
/* and the (more complete) dissertation:                                     */
/* "Tetrahedral Mesh Improvement," Bryan Klingner,                           */
/* EECS Department, University of California, Berkeley,                      */
/* Technical Report No. UCB/EECS-2008-145                                    */
/* http://www.eecs.berkeley.edu/Pubs/TechRpts/2008/EECS-2008-145.html        */
/*****************************************************************************/

/* Include the tetrahedral complex library Starbase                          */
#include "Starbase.h"

/* for debugging assertions */
#include <assert.h>
#include <stdio.h>

/*****************************************************************************/
/*                                                                           */
/*  Defines                                                                  */
/*                                                                           */
/*****************************************************************************/
#ifndef PI
    #define PI 3.141592653589793238462643383279502884197169399375105820974944592308
#endif

/* do paranoid checks during improvement */
#define IMPROVEPARANOID false

/* verbosity of debugging output */
#define IMPROVEVERBOSITY 5

/* a really big floating point number */
#define HUGEFLOAT 1.0e100

/* the sines of some angles */
#define SINEHALF 0.00872653549837
#define SINE1  0.01745240644
#define SINE2  0.0348994967
#define SINE5  0.08715574275
#define SINE10 0.17364817767
#define SINE13 0.22495105434
#define SINE15 0.2588190451
#define SINE20 0.34202014333
#define SINE25 0.42261826174
#define SINE30 0.5
#define SINE35 0.57357643635
#define SINE40 0.64278760969
#define SINE45 0.70710678119
#define SINEEQUILATERAL 0.94280903946

/* when the journal reaches this size, half it's size (remove the older half
   of the entries) */
#define JOURNALHALFSIZE 1000000

/* vertex types */
#define INPUTVERTEX 0
#define FIXEDVERTEX 1
#define SEGMENTVERTEX 2
#define FACETVERTEX 3
#define FREEVERTEX 4
#define UNDEADVERTEX 15
/* new kind of vertex to identify the ones I've put in */
#define INSERTEDVERTEX 4

/* types of improvement passes */
#define SMOOTHPASS 0
#define TOPOPASS 1
#define CONTRACTPASS 2
#define INSERTPASS 3
#define DESPERATEPASS 4
#define DEFORMPASS 5
/* types of local improvement passes */
#define SMOOTHPASSLOCAL 6
#define TOPOPASSLOCAL 7
#define CONTRACTPASSLOCAL 8
#define INSERTPASSLOCAL 9
/* size control pass */
#define SIZECONTROLPASS 10
#define CONTRACTALLPASS 11

/* number of quality measures */
#define NUMQUALMEASURES 6

/* number of "thresholded" means used to approximate mesh quality */
#define NUMMEANTHRESHOLDS 7

/* edge cases */
#define NUMEDGECASES 10
#define NOEDGECASE 0
#define FREEFREEEDGE 1
#define FREEFACETEDGE 2
#define FREESEGMENTEDGE 3
#define FREEFIXEDEDGE 4
#define FACETFACETEDGE 5
#define FACETSEGMENTEDGE 6
#define FACETFIXEDEDGE 7
#define SEGMENTSEGMENTEDGE 8
#define SEGMENTFIXEDEDGE 9
#define FIXEDFIXEDEDGE 10

/* number of passes without improvement before static improvement quits */
#define STATICMAXPASSES 3
/* number of desperate insertion passes that can ever be attempted */
#define DESPERATEMAXPASSES 3

/*****************************************************************************/
/*  Topological improvement options                                          */
/*****************************************************************************/

/* number of tets to allow in a ring around an edge to be removed */
#define MAXRINGTETS 70
#define MAXRINGTETS2 50

/* maximum number of tets in sandwich set replacement during edge removal */
#define MAXNEWTETS 150

/* maximum number of faces in tree for multi-face removal */
#define MAXFACETREESIZE 50

/* minimum quality to allow a 4-verts-on-boundary tet to be created
   by a topological improvement operation */
#define MIN4BOUNDQUAL SINE1

/*****************************************************************************/
/*  Smoothing improvement options                                            */
/*****************************************************************************/

/* do paranoid checks of smoothing process */
#define SMOOTHPARANOID false

/* how close to the worst does a function have to be to be in the
   active set? */
#define ACTIVESETFACTOR 1.03

/* how many tets will we allow incident to one vertex for smoothing? */
#define MAXINCIDENTTETS 700

/* minimum step size */
#define MINSTEPSIZE 1.0e-5

/* maximum iterations in non-smooth line search */
#define MAXLINEITER 50

/* rate must be worse by this much to reset alpha */
#define RATEEPSILON 1.0e-6

/* maximum iterations of non smooth optimization */
#define MAXSMOOTHITER 50

/* minimum quality improvement in a smoothing step */
#define MINSMOOTHITERIMPROVE 1.0e-5

/* if d is within this of zero-length, call it zero */
#define DEPSILON 1.0e-5

/* if closest point computation has a factor smaller than this, make it the origin */
#define NEARESTMIN 1.0e-13

/* the minimum improvement of the minimum quality element for a 
   (topo|smoothing|insertion) pass to "succeed" */
#define MINMINIMPROVEMENT 1.0e-15

#define MINLOCALMEANIMPROVE 0.005

/* minimum improvement required for edge contraction to succeed */
#define MINCONTRACTIMPROVEMENT 1.0e-06

/* the dot product of two normals must be 1+- this value to be considered coplanar */
#define COPLANARTOL 1e-4

/* the absolute value of the dot product of two edges must be 1+- 
   this value to be considered colinear */
#define COLINEARTOL 1e-4

/* determines whether facet/segment vertices are smoothed */
#define SMOOTHFACETVERTICES 0x01
#define SMOOTHSEGMENTVERTICES 0x02
#define SMOOTHFIXEDVERTICES 0x04

/*****************************************************************************/
/*  Insertion/cavity improvement options                                     */
/*****************************************************************************/

/* do paranoid checks during insertion */
#define INSERTPARANOID false

/* minimum improvement for a submesh */
#define MINSUBMESHIMPROVEMENT 1.0e-3

/* maximum submesh improvement iterations */
#define MAXSUBMESHITERATIONS 8

/* if a tet is within this number of the worst tet, its close to worst */
#define CLOSETOWORST SINE2

/* how much bigger to allow the improvement stack to be when we've got 
   one of the worst tets */ 
#define TRYHARDFACTOR 15
#define TRYHARDMAXSUBMESHITERATIONS 20
#define TRYHARDMINSUBMESHIMPROVEMENT 1e-10

/* minimum quality of an intermediate tet */
#define MINTETQUALITY 1.0e-14
#define MINSIZETETQUALITY 1.0e-10

/* maximum size of stuff for cavity drilling */
#define MAXCAVITYFACES 10000
#define MAXCAVITYTETS 10000

/* minimum improvement of vertex insertion */
#define MININSERTIONIMPROVEMENT 1.0e-13

/* minimum positivity of orientation for a face to be oriented "toward" */
#define MINFACING 1.0e-7

/* factor by which child cavity qualities are increased */
#define CHILDFAVORFACTOR 1.0

/* maximum difference between two qualities where they are considered 'equal' */
#define MAXQUALDIFFERENCE 1.0e-15

/* maximum number of outgoing faces allowed for a tet */
#define MAXOUTFACES 200

/* maximum number of edges in a cavity dag */
#define MAXCAVITYDAGEDGES 50000

/* perform initial/final smooth of all cavity vertices */
#define INITIALCAVITYSMOOTH true
#define FINALCAVITYSMOOTH true

/* maximum stack size for cavity improvement */
#define MAXCAVITYSTACK 250

/* biggest size for cavity heap */ 
#define MAXCAVITYHEAPSIZE MAXCAVITYTETS

/* deepest level a tet can be */
#define MAXCAVDEPTH 1000

/* failure count on which to perform desperate insertion pass */
#define DESPERATEPASSNUM 2
/* if the quality of the mesh is really terrible, don't bother trying to insert 
   up to max insert quality */
#define QUALFROMDESPERATE SINE15
/* if the minimum quality is high, reach even higher by this much in desperate pass */
#define QUALUPFROMDESPERATE SINE1

/* never attempt insertion on more than this many tets, no matter what */
#define MAXINSERTTETS 4000

/*****************************************************************************/
/*  size control options                                                     */
/*****************************************************************************/

/* minimum quality of affect tets after size-control edge contraction */
#define MINCONTRACTQUAL 5.0e-2
#define MINSIZESPLITQUAL 5.0e-2
#define MEANEDGESCALE 0.8
/* maximum number of size control iterations */
/*#define MAXSIZEITERS 30*/
#define MAXSIZEITERS 10
#define CONTROLSHORTFAC 1.35
#define CONTROLLONGFAC 0.85

/*****************************************************************************/
/*                                                                           */
/*  Data structures                                                          */
/*                                                                           */
/*****************************************************************************/

/* structure to hold all the options for improvement */
struct improvebehavior
{
    /* Quality measure */
    int qualmeasure;             /* quality measure used */
    starreal sinewarpfactor;     /* warp obtuse sines by this factor */
    
    /* Quadric smoothing options */
    int usequadrics;             /* incorporate quadric error into the objective function */
    starreal quadricoffset;      /* quality to start every quadric at */
    starreal quadricscale;       /* factor to scale quadric by */
    
    /* Smoothing options */
    int nonsmooth;               /* enable non-smooth optimization-based smoothing */
    int facetsmooth;             /* enable smoothing of facet vertices */
    int segmentsmooth;           /* enable smoothing of segment vertices */
    int fixedsmooth;             /* enable smoothing of fixed vertices */
    
    /* Topological options */
    int edgeremoval;             /* enable edge removal */
    int edgecontraction;         /* enable edge contraction */
    int boundedgeremoval;        /* enable boundary edge removal */
    int singlefaceremoval;       /* enable single face removal (2-3 flips) */
    int multifaceremoval;        /* enable multi face removal */
    int flip22;                  /* enable 2-2 flips */
    int jflips;                  /* use Jonathan's faster flip routines */
    
    /* Insertion options */
    int enableinsert;            /* global enable of insertion */
    starreal insertthreshold;    /* percent worst tets */
    int insertbody;              /* enable body vertex insertion */
    int insertfacet;             /* enable facet insertion */
    int insertsegment;           /* enablem segment insertion */
    int cavityconsiderdeleted;   /* consider enlarging cavity for deleted tets? */
    int cavdepthlimit;           /* only allow initial cavity to includes tets this deep */
    
    /* anisotropic meshing options */
    int anisotropic;             /* globally enable space warping with deformation tensor */
    int tensor;                  /* which scaling tensor field to use */
    int tensorb;                 /* second tensor, to blend with the first one */
    starreal tensorblend;  /* between 0 and 1, how much of anisotropy to use */
    
    /* sizing options */
    int sizing;                  /* globally enable mesh element size control */
    int sizingpass;              /* enable or disable initial edge length control */
    starreal targetedgelength;   /* edge length of the ideal edge for this mesh */
    starreal longerfactor;       /* factor by which an edge can be longer */
    starreal shorterfactor;      /* factor by which an edge can be shorter */
    
    /* Dynamic improvement options */
    starreal dynminqual;         /* minimum quality demanded for dynamic improvement. */
    starreal dynimproveto;       /* after minimum quality is reached, improve to at least this level */
    int deformtype;              /* which fake deformation to use */
    int dynimprove;              /* perform dynamic improvement with fake deformation? */
    
    /* thresholds */
    starreal minstepimprovement; /* demand at least this much improvement in the mean per step */
    starreal mininsertionimprovement; /* demand in improvement for insertion */
    starreal maxinsertquality[NUMQUALMEASURES];   /* never attempt insertion in a tet better than this */
    
    /* improvement limits */
    starreal goalanglemin;       /* stop improvement if smallest angle reaches this threshold */
    starreal goalanglemax;       /* stop improvement if largest angle reaches this threshold */
    
    /* quality file output */
    int minsineout;              /* en/disable .minsine file output */
    int minangout;               /* en/disable .minang file output */
    int maxangout;               /* en/disable .maxang file output */
    int vlrmsout;                /* en/disable .vlrms file output */
    int nrrout;                 /* en/disable .rnrr file output */
    
    /* output file name prefix */
    char fileprefix[100];
    
    /* enable animation */
    int animate;
    /* for animation, only output .stats */
    int timeseries;
    
    /* verbosity */
    int verbosity;
    int usecolor;
    
    /* miscellaneous */
    int outputandquit;           /* just produce all output files for unchanged mesh */
};

/* structure to hold global improvement statistics */
struct improvestats
{
    /* smoothing stats */
    int nonsmoothattempts;
    int nonsmoothsuccesses;
    int freesmoothattempts;
    int freesmoothsuccesses;
    int facetsmoothattempts;
    int facetsmoothsuccesses;
    int segmentsmoothattempts;
    int segmentsmoothsuccesses;
    int fixedsmoothattempts;
    int fixedsmoothsuccesses;
    
    /* topological stats */
    int edgeremovals;
    int boundaryedgeremovals;
    int edgeremovalattempts;
    int boundaryedgeremovalattempts;
    int ringsizesuccess[MAXRINGTETS];
    int ringsizeattempts[MAXRINGTETS];
    int faceremovals;
    int faceremovalattempts;
    int facesizesuccess[MAXFACETREESIZE];
    int facesizeattempts[MAXFACETREESIZE];
    int flip22attempts;
    int flip22successes;
    int edgecontractionattempts;
    int edgecontractions;
    int edgecontractcaseatt[NUMEDGECASES+1];
    int edgecontractcasesuc[NUMEDGECASES+1];
    int edgecontractringatt[MAXRINGTETS];
    int edgecontractringsuc[MAXRINGTETS];
    
    /* insertion stats */
    int bodyinsertattempts;
    int bodyinsertsuccesses;
    int facetinsertattempts;
    int facetinsertsuccesses;
    int segmentinsertattempts;
    int segmentinsertsuccesses;
    int maxcavitysizes[MAXCAVITYTETS];
    int finalcavitysizes[MAXCAVITYTETS];
    int biggestcavdepths[MAXCAVDEPTH];
    int lexmaxcavdepths[MAXCAVDEPTH];
    
    /* timing stats */
#ifndef NO_TIMER
    struct timeval starttime;
#endif /* not NO_TIMER */
    int totalmsec;
    int smoothmsec;
    int topomsec;
    int contractmsec;
    int insertmsec;
    int smoothlocalmsec;
    int topolocalmsec;
    int insertlocalmsec;
    int contractlocalmsec;
    int biggestcavityusec;
    int finalcavityusec;
    int cavityimproveusec;
    
    /* general stats */
    int startnumtets;
    int finishnumtets;
    int startnumverts;
    int finishnumverts;
    starreal dynchangedvol;
    
    /* quality stats */
    starreal finishworstqual;
    starreal startworstqual;
    starreal startminangle;
    starreal startmaxangle;
    starreal startmeanquals[NUMMEANTHRESHOLDS];
    starreal finishmeanquals[NUMMEANTHRESHOLDS];
};

/* a tet for the improvement stack, referred to by the tuple of its vertices
   and a single quality measure */
struct improvetet
{
    tag verts[4];
    starreal quality;
};

/* structure for holding quality and gradient information for non-smooth
   optimization-based vertex smoothing */
struct opttet
{
    tag verts[4];             /* vertices of the tetrahedron */
    starreal volume;            /* volume of tetrahedron */
    starreal volumegrad[3];     /* the gradient of the volume of the tet wrt vtx1 */
    starreal sine[6];           /* sine of each dihedral angle of the tet */
    starreal sinegrad[6][3];    /* gradient of each sine */
    starreal rnrr;              /* root normalized radius ratio */
    starreal rnrrgrad[3];       /* gradient of root normalized radius ratio */
    starreal vlrms3r;           /* V / lrms^3 ratio */
    starreal vlrms3rgrad[3];    /* gradient thereof */
    starreal edgelength[3][4];  /* the lengths of each of the edges of the tet */
    starreal edgegrad[3][4][3]; /* the gradient of each edge length wrt vtx1 */
    starreal facearea[4];       /* areas of the faces of the tet */
    starreal facegrad[4][3];    /* the gradient of each of the face areas wrt vtx1 */
};

typedef enum {false, true} bool;

/* store mappings from tags to vertex types */
struct vertextype
{
    int kind;        /* the kind of vector this is (FREEVERTEX, FACETVERTEX, etc) */
    starreal vec[3]; /* a vector associated with the vertex. for FACETVERTEX,
                        this is the normal to the plane that the vertex can move in.
                        for SEGMENTVERTEX, this is a vector in the direction of the
                        segment. */
};

/* stack data structure using arraypool */

/* cardinal value for empty stack */
#define STACKEMPTY -1

/* use 1024 tets per level-two block */
#define LOG2TETSPERSTACKBLOCK 10

/* a stack implemented with an arraypool */
struct arraypoolstack
{
    struct arraypool pool; /* the array pool that makes up the stack */
    long top;     /* the index of the top element in the array */
    long maxtop;  /* the maximum size the stack has ever been */
};

/* surface error quadric */
struct quadric
{
    starreal a2, ab, ac, ad;
    starreal     b2, bc, bd;
    starreal         c2, cd;
    starreal             d2;
    starreal origpos[3];     /* original vertex position */
    int numfaces;            /* number of incident faces */
    starreal facesum;        /* sum of incident face areas */
    starreal edge2harm;      /* harmonic mean of squares of inc. edge lengths */
    bool hasquadric;
};

/*****************************************************************************/
/*                                                                           */
/*  Global variables                                                         */
/*                                                                           */
/*****************************************************************************/

/* global behavior struct */
struct behavior behave;

/* global improvement behavior struct */
struct improvebehavior improvebehave;

/* global statistics */
struct improvestats stats;

/* global vertex info */
struct arraypool vertexinfo;

/* global vertex pool */
struct proxipool *vertexpoolptr;

/* global surface vertex quadrics */
struct arraypool surfacequadrics;

/* counter for journal IDs */
int maxjournalid = 1;

/* types of quality measures that may be used */
enum tetqualitymetrics
{
    QUALMINSINE,
    QUALRADIUSRATIO,
    QUALVLRMS3RATIO,
    QUALMEANSINE,
    QUALMINSINEANDEDGERATIO,
    QUALWARPEDMINSINE,
    QUALMINANGLE,
    QUALMAXANGLE
};

/* types of anisotropic sizing fields */
enum tensorfields
{
    NOTENSOR,
    STRETCHX,
    STRETCHY,
    SINK,
    SWIRL,
    CENTER,
    PERIMETER,
    RIGHT,
    SINE,
    STRAIN
};

/* types of artificial deformation functions */
enum deformtypes
{
    TWIST,
    STRETCH,
    SQUISHMIDDLE
};

/* quality thresholds for averages */
starreal meanthresholds[NUMQUALMEASURES][NUMMEANTHRESHOLDS] =
{
    /* QUALMINSINE thresholds */
    {
        SINE1,
        SINE5,
        SINE10,
        SINE15,
        SINE25,
        SINE35,
        SINE45
    },
    /* QUALRADIUSRATIO thresholds */
    {
        0.1,
        0.2,
        0.3,
        0.4,
        0.5,
        0.6,
        0.7
    },
    /* QUALVLRMS3RATIO thresholds */
    {
        0.1,
        0.2,
        0.3,
        0.4,
        0.5,
        0.6,
        0.7
    },
    /* QUALMEANSINE thresholds */
    {
        SINE1,
        SINE5,
        SINE10,
        SINE15,
        SINE25,
        SINE35,
        SINE45
    },
    /* QUALMINSINEANDEDGERATIO thresholds */
    {
        SINE1,
        SINE5,
        SINE10,
        SINE15,
        SINE25,
        SINE35,
        SINE45
    },
    /* QUALWARPEDMINSINE thresholds */
    {
        SINE1,
        SINE5,
        SINE10,
        SINE15,
        SINE25,
        SINE35,
        SINE45
    }
};

int meanthresholdangles[NUMMEANTHRESHOLDS] =
{
    1,
    5,
    10,
    15,
    25,
    35,
    45
};

int globalvertexcount;

/* bounding box for the mesh */
starreal G_boundingbox[6];
starreal G_center[3];
starreal G_range[3];

/*****************************************************************************/
/* Shared function declarations                                              */
/*****************************************************************************/

/* TODO should these really be in header files? what a pain... */

/* topological.c */
void inserttet(struct tetcomplex *mesh,
               tag vtx1,
               tag vtx2,
               tag vtx3,
               tag vtx4,
               bool record);
void deletetet(struct tetcomplex *mesh,
               tag vtx1,
               tag vtx2,
               tag vtx3,
               tag vtx4,
               bool record);
void flip23(struct tetcomplex *mesh,
            tag vtx1,
            tag vtx2,
            tag vtx3,
            tag vtxbot,
            tag vtxtop,
            bool record);
void flip22(struct tetcomplex *mesh,
            tag vtx1,
            tag vtx2,
            tag vtx3,
            tag vtxbot,
            tag vtxtop,
            bool record);
void flip32(struct tetcomplex *mesh,
            tag vtx1,
            tag vtx2,
            tag vtx3,
            tag vtxbot,
            tag vtxtop,
            bool record);
void flip13(struct tetcomplex* mesh,
            tag vtx1,
            tag vtx2,
            tag vtx3,
            tag vtx4,
            tag facetvtx,
            bool record);
void flip31(struct tetcomplex* mesh,
            tag vtx1,
            tag vtx2,
            tag vtx3,
            tag vtx4,
            tag facetvtx,
            bool record);
void flip12(struct tetcomplex* mesh,
            tag vtx1,
            tag vtx2,
            tag vtx3,
            tag vtx4,
            tag segmentvtx,
            bool record);
void flip21(struct tetcomplex* mesh,
            tag vtx1,
            tag vtx2,
            tag vtx3,
            tag vtx4,
            tag segmentvtx,
            bool record);
void flip14(struct tetcomplex* mesh,
            tag vtx1,
            tag vtx2,
            tag vtx3,
            tag vtx4,
            tag facetvtx,
            bool record);
void flip41(struct tetcomplex* mesh,
            tag vtx1,
            tag vtx2,
            tag vtx3,
            tag vtx4,
            tag facetvtx,
            bool record);

/* size.c */
void sizereportstream(FILE *o, struct tetcomplex *mesh);

/* print.c */
void printtetverts(struct tetcomplex *mesh, tag *tet);
void printtetvertssep(struct tetcomplex *mesh,
                      tag vtx1,
                      tag vtx2,
                      tag vtx3,
                      tag vtx4);

/* journal.c */
void findtetfromvertex(tetcomplex *mesh,
                       tag vtx,
                       tag outtet[4]);
                       
/* arraypoolstack.c */
void stackinit(struct arraypoolstack *stack, arraypoolulong objectbytes);
void stackrestart(struct arraypoolstack *stack);
void stackdeinit(struct arraypoolstack *stack);
void* stackpush(struct arraypoolstack *stack);
void* stackpop(struct arraypoolstack *stack);
void fillstackqual(struct tetcomplex *mesh,
                   struct arraypoolstack *stack,
                   int qualmeasure,
                   starreal threshold,
                   starreal *meanqual,
                   starreal *minqual);
                   
/* pointer to strain thing from Nutt's sim */
double (*G_evalstrain)(const double* point);

/*****************************************************************************/
/*                                                                           */
/*  Convenience functions                                                    */
/*                                                                           */
/*****************************************************************************/

int mytetcomplexconsistency(struct tetcomplex *plex) {
 struct tetcomplexposition pos;
 tag nexttet[4];
 arraypoolulong horrors;

 horrors = 0;
 tetcomplexiteratorinit(plex, &pos);
 tetcomplexiterateall(&pos, nexttet);
 while (nexttet[0] != STOP) {
   horrors += tetcomplexmissingtet(plex, nexttet[0], nexttet[2],
                                    nexttet[3], nexttet[1]);
   horrors += tetcomplexmissingtet(plex, nexttet[0], nexttet[3],
                                    nexttet[1], nexttet[2]);
   horrors += tetcomplexmissingtet(plex, nexttet[1], nexttet[2],
                                    nexttet[0], nexttet[3]);
   horrors += tetcomplexmissingtet(plex, nexttet[1], nexttet[3],
                                    nexttet[2], nexttet[0]);
   horrors += tetcomplexmissingtet(plex, nexttet[2], nexttet[3],
                                    nexttet[0], nexttet[1]);
   tetcomplexiterateall(&pos, nexttet);
 }

 /*if (horrors == 0) {
     printf("  Tremble before my vast wisdom, with which "
            "I find the mesh to be consistent.\n");
 } else if (horrors == 1) {
   printf("  !! !! !! !! Precisely one oozing cyst sighted.\n");
 } else {
   printf("  !! !! !! !! %d monstrosities witnessed.\n", horrors);
 }*/
 if (horrors == 0)
 {
     return 1;
 }
 else
 {
     return 0;
 }
}

/* copy the values from one array of tags to the other */
void tagarraycopy(tag A1[],
                  tag A2[],
                  int length)
{
    memcpy(A2, A1, sizeof(tag)*length);
}

/* copy the values from one array of points to the other */
void pointarraycopy(starreal A1[][3],
                    starreal A2[][3],
                    int length)
{
    memcpy(A2, A1, sizeof(starreal)*3*length);
}

/* function to sort the vertices of a tet */
#define swaptag(x, y) do {tag tmp; tmp = x; x = y; y = tmp; } while (0)
void sorttetverts(tag tet[4])
{
    if (tet[1] < tet[0]) swaptag(tet[1], tet[0]);
    if (tet[2] < tet[1]) swaptag(tet[2], tet[1]);
    if (tet[3] < tet[2]) swaptag(tet[3], tet[2]);
    if (tet[1] < tet[0]) swaptag(tet[1], tet[0]);
    if (tet[2] < tet[1]) swaptag(tet[2], tet[1]);
    if (tet[1] < tet[0]) swaptag(tet[1], tet[0]);
}

/* Compute the parity of the permutation of the array perm.
   The array should be length long, and contain integers from
   0 up to length -1. The even/oddness is in relation to the
   permutation frem 0...length-1, in order. It returns 0 if 
   perm is an even permutation, and 1 if it is odd */
int permutationparity(int *perm, int length)
{
    int inversions = 0; /* number of inversions */
    int i,j;            /* loop indices */
    
    for (i=0; i<length-1; i++)
    {
        for (j=i+1; j<length; j++)
        {
            /* check if i < j AND p(i) > p(j). If so, inversion */
            if (perm[i] > perm[j])
            {
                inversions++;
            }
        }
    }
    
    /* if there are an odd number of inversions, odd parity */
    return inversions % 2;
}

/* a new, faster way to test tet equivalence ? */
bool sametet(tag v11, tag v12, tag v13, tag v14,
             tag v21, tag v22, tag v23, tag v24)
{
    /* we just need to verify that every vertex in tet one is in
       tet two */
    if (
         (v11 == v21 || v11 == v22 || v11 == v23 || v11 == v24) &&
         (v12 == v21 || v12 == v22 || v12 == v23 || v12 == v24) &&
         (v13 == v21 || v13 == v22 || v13 == v23 || v13 == v24) &&
         (v14 == v21 || v14 == v22 || v14 == v23 || v14 == v24)
       )
    {
        return true;
    }
    return false;
}

/* tests whether the two tetrahedra are the same,
   where "sameness" is gauged by whether they share
   all the same vertices, and the orientation is the same.
   orientation being the same means that the order of the
   vertices of one is an even permutation of the other */
bool oldsametet(tag vtx11, tag vtx12, tag vtx13, tag vtx14,
             tag vtx21, tag vtx22, tag vtx23, tag vtx24)
{
    tag tet1sorted[4];
    tag tet2sorted[4];
    
    tet1sorted[0] = vtx11;
    tet1sorted[1] = vtx12;
    tet1sorted[2] = vtx13;
    tet1sorted[3] = vtx14;
    
    tet2sorted[0] = vtx21;
    tet2sorted[1] = vtx22;
    tet2sorted[2] = vtx23;
    tet2sorted[3] = vtx24;
    
    /* sort the vertices of each tet */
    sorttetverts(tet1sorted);
    sorttetverts(tet2sorted);
    
    /* make sure they both have the same vertices */
    if ( 
         (tet1sorted[0] != tet2sorted[0]) ||
         (tet1sorted[1] != tet2sorted[1]) ||
         (tet1sorted[2] != tet2sorted[2]) ||
         (tet1sorted[3] != tet2sorted[3])
       )
    {
        return false;
    }
    
    /* the rest is unnecessary because same verts but different
       parity implies that one of the tets is invalid anyway */
    return true;
}

/* determine of two sets of three tags represent
   the same oriented face, meaning that they have the
   same vertices and that the vertices come in the same
   order, with wrapping allowed. So (1,2,3) = (3,1,2). */
bool sameface(tag f11, tag f12, tag f13,
              tag f21, tag f22, tag f23)
{
    /* for speed, explicitly check all three possible 
       situations where the two faces are equal */
    if (
            ((f11 == f21) && (f12 == f22) && (f13 == f23)) ||
            ((f11 == f22) && (f12 == f23) && (f13 == f21)) ||
            ((f11 == f23) && (f12 == f21) && (f13 == f22))
       )
    {
        return true;
    }
    return false;
}

/* check whether the specified tet exists in the mesh. if it does, return 1.
   otherwise, return 0 */
int tetexists(struct tetcomplex *mesh, tag vtx1, tag vtx2, tag vtx3, tag vtx4)
{
    tag topbot[2];
    int foundface;
    int horrors = 0;
    
    foundface = tetcomplexadjacencies(mesh, vtx2, vtx3, vtx4, topbot);
    if (foundface == 0)
    {
        if (improvebehave.verbosity > 5)
        {
            printf("tet doesn't exist because face (%d %d %d) isn't found...\n", (int) vtx2, (int) vtx3, (int) vtx4);
        }
        return 0;
    }
    /* face was found... is top vertex correct? */
    if (topbot[0] != vtx1)
    {
        if (improvebehave.verbosity > 5)
        {
            printf("tet doesn't exist because top vertex doesn't match...\n");
        }
        return 0;
    }
    
    if (improvebehave.verbosity > 5)
    {
        printf("verified that tet with verts %d %d %d %d exists in mesh\n", (int) vtx1, (int) vtx2, (int) vtx3, (int) vtx4);
        printf("face %d %d %d was found and the vert opposite it was %d\n", (int) vtx2, (int) vtx3, (int) vtx4, (int) topbot[0]);
    }
    
    /* taken from tetcomplexconsistency(), except just checks one tet */
    if (IMPROVEPARANOID)
    {
        horrors += tetcomplexmissingtet(mesh, vtx1, vtx3,
                                         vtx4, vtx2);
        horrors += tetcomplexmissingtet(mesh, vtx1, vtx4,
                                         vtx2, vtx3);
        horrors += tetcomplexmissingtet(mesh, vtx2, vtx3,
                                         vtx1, vtx4);
        horrors += tetcomplexmissingtet(mesh, vtx2, vtx4,
                                         vtx3, vtx1);
        horrors += tetcomplexmissingtet(mesh, vtx3, vtx4,
                                         vtx1, vtx2);
    
        if (horrors != 0)
        {
            printf("everything was cool with your check but horrors != 0...\n");
            return 0;
        }
    }
    
    return 1;
}

/* wrapper for checking tet existence with an array */
int tetexistsa(tetcomplex *mesh, tag tet[4])
{
    return tetexists(mesh, tet[0], tet[1], tet[2], tet[3]);
}

/* get all the tets incident on vtx1 which is in the tet 
   (vtx1, vtx2, vtx3, vtx4). This function is recursive and
   should first be called with numincident = 0.
   return 0 if the number of incident tets exceed the
   maximum allowed. Set boundary flag if vertex lies on
   boundary (i.e., has at least one ghost tet incident) */
int getincidenttets(struct tetcomplex *mesh,
                    tag vtx1,
                    tag vtx2,
                    tag vtx3,
                    tag vtx4,
                    tag incidenttets[][4],
                    int *numincident,
                    bool *noghostflag)
{
    tag neightets[3][4];  /* neighbor tetrahedra */
    tag topbot[2];        /* the top and bottom vertices from adjacency query */
    int foundface;        /* checks whether adjacency query finds the face at all */
    tag neighfaces[3][4]; /* the three faces we want to check, last vertex is leftover */
    int i,j;              /* loop indices */
    bool visited = false; /* whether each neighbor has been visited */
    tag i2, i3, i4;       /* incident tet vertices, excluding central vertex */
    tag n2, n3, n4;       /* neighbor tet vertices, excluding central vertex */
    
    /* first, check to make sure this tet still exists in the mesh */
    if (tetexists(mesh, vtx1, vtx2, vtx3, vtx4) == 0)
    {
        return 0;
    }
    
    /* add the this tet to the list of incident tets */
    incidenttets[*numincident][0] = vtx1;
    incidenttets[*numincident][1] = vtx2;
    incidenttets[*numincident][2] = vtx3;
    incidenttets[*numincident][3] = vtx4;
    *numincident = (*numincident) + 1;
    
    /* check to make sure we aren't going crazy */
    if (*numincident > MAXINCIDENTTETS)
    {
        printf("when finding incident tets, exceeded %d tets, giving up\n", MAXINCIDENTTETS);
        return 0;
    }
    
    /* the three neighbor tets to this one that are also
       incident on vtx1 can be found using adjacency queries
       on the faces:
       
       (vtx1, vxt2, vtx3),
       (vtx1, vtx4, vtx2), and
       (vtx1, vtx3, vtx4).
       
       the "top" vertices from these queries will be apexes
       of the three adjoining tetrahedra. the "bottom" vertices
       will be the fourth vertex of the current tetrahedron.
    */
    neighfaces[0][0] = vtx1;
    neighfaces[0][1] = vtx2;
    neighfaces[0][2] = vtx3;
    neighfaces[0][3] = vtx4; /* should be the bottom vertex in adj query */
    
    neighfaces[1][0] = vtx1;
    neighfaces[1][1] = vtx4;
    neighfaces[1][2] = vtx2;
    neighfaces[1][3] = vtx3; /* should be the bottom vertex in adj query */
    
    neighfaces[2][0] = vtx1;
    neighfaces[2][1] = vtx3;
    neighfaces[2][2] = vtx4;
    neighfaces[2][3] = vtx2; /* should be the bottom vertex in adj query */
    
    /* for each neighbor face */
    for (i=0; i<3; i++)
    {
        /* make sure the original tet exists from this perspective */
        assert(tetexists(mesh,
                         neighfaces[i][3],
                         neighfaces[i][0],
                         neighfaces[i][2],
                         neighfaces[i][1]));

        foundface = tetcomplexadjacencies(mesh, 
                                          neighfaces[i][0], 
                                          neighfaces[i][1], 
                                          neighfaces[i][2], 
                                          topbot);
                                      
        /* check that the face was found */
        assert(foundface == 1);

        /* if any tet adjacent to this vertex is a ghost tet,
           set the boundary flag */
        if (topbot[0] == GHOSTVERTEX)
        {
            *noghostflag = false;
        }

        /* the bottom vertex was the other vertex in the tet */
        assert(topbot[1] == neighfaces[i][3]);
    
        
        /* otherwise, we've got a neighbor tet. To keep same vertex as vtx1,
           save this tet as (neigh[0], neigh[1], top, neigh[2].  */
        neightets[i][0] = neighfaces[i][0];
        neightets[i][1] = neighfaces[i][1];
        neightets[i][2] = topbot[0];
        neightets[i][3] = neighfaces[i][2];
        
    }
    
    /* now, figure out which neighbors we've visited */
    
    /* for each neighbor tet */
    for (i=0; i<3; i++)
    {
        /* if this neighbor tet is a ghost tet, no need to visit */
        if (neightets[i][2] == GHOSTVERTEX)
        {
            visited = true;
        }
        else
        {
            visited = false;
        
            /* get the vertices of this neighbor tet, excluding central vertex */
            n2 = neightets[i][1];
            n3 = neightets[i][2];
            n4 = neightets[i][3];
        
            /* for each tet visited so far */
            for (j=0; j<*numincident; j++)
            {
                /* get the vertices of this tet, excluding central vertex */
                i2 = incidenttets[j][1];
                i3 = incidenttets[j][2];
                i4 = incidenttets[j][3];
            
                /* is this tet the same as the current neighbor tet? */
                /* because we know that all of the tets have the same first
                   vertex (the one we're find incident elements for), all
                   we need to check whether the face vertices are the same,
                   and that they come in the same order (wrapping allowed).
                   This means that face (3,2,1) is the same as face (1,3,2) */
                if (sameface(n2,n3,n4, i2,i3,i4))
                {
                    /* then mark this neighbor as visited and don't check any more */
                    visited = true;
                    break;
                } 
            }
        }
        
        /* if it hasn't been visited */
        if (visited != true)
        {
            /* find incident tets using DFS on it */
            getincidenttets(mesh, 
                            neightets[i][0], 
                            neightets[i][1], 
                            neightets[i][2],
                            neightets[i][3],
                            incidenttets,
                            numincident,
                            noghostflag);
        }
    }
    
    return 1;
}

/* returns true if the tet (1,2,3,4) has
   positive orientation, and false otherwise */
bool positivetet(tetcomplex *mesh,
                 tag v1,
                 tag v2,
                 tag v3,
                 tag v4)
{
    return (orient3d(&behave,
                    ((struct vertex *) tetcomplextag2vertex(mesh, v1))->coord,
                    ((struct vertex *) tetcomplextag2vertex(mesh, v2))->coord,
                    ((struct vertex *) tetcomplextag2vertex(mesh, v3))->coord,
                    ((struct vertex *) tetcomplextag2vertex(mesh, v4))->coord) > 0);
}

/* convert from a sin of an angle to that angle in degrees (does not test obtuseness) */
starreal sintodeg(starreal insine)
{
    return asin(insine) * 180.0 / PI;
}

/* convert from an angle in degrees to sin */
starreal degtosin(starreal inangle)
{
    return sin((inangle) * (PI / 180.0));
}

starreal tantodeg(starreal intan)
{
    return atan(intan) * 180.0 / PI;
}

starreal radtodeg(starreal inangle)
{
    return (inangle * 180) / PI;
}

/* for printing color in output to the screen */
#define RESET		0
#define BRIGHT 		1
#define DIM         2
#define UNDERLINE 	3
#define BLINK		4
#define REVERSE		7
#define HIDDEN		8

#define BLACK 		0
#define RED		    1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define	WHITE		7

void textcolor(int attr, int fg, int bg)
{	char command[13];

	/* Command is the control command to the terminal */
	if (improvebehave.usecolor == true)
	{
	    sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	    printf("%s", command);
	}
}

/* zero out the stats structure */
void initimprovestats(void)
{
    memset(&stats, 0, sizeof(struct improvestats));
}

int countverts(struct proxipool *vertpool)
{
    tag vertextag;
    int numverts = 0;
    vertextag = proxipooliterate(vertpool, NOTATAG);
    
    while (vertextag != NOTATAG)
    {
        numverts++;
        vertextag = proxipooliterate(vertpool, vertextag);
    }
    
    return numverts;
}

int counttets(struct tetcomplex *mesh)
{
    struct tetcomplexposition pos; /* position of iterator in the mesh */
    tag tet[4];
    int numtets = 1;
    
    /* initialize the iterator over the mesh */
    tetcomplexiteratorinit(mesh, &pos);
    /* retrieve the first tet in the mesh */
    tetcomplexiteratenoghosts(&pos, tet);
    
    /* for each tet */
    while (tet[0] != STOP) 
    {
        numtets++;
        tetcomplexiteratenoghosts(&pos, tet);
    }
    return numtets;
}

#ifndef NO_TIMER
/* returns the number of milliseconds elapsed between tv1 and tv2 */
int msecelapsed(struct timeval tv1, struct timeval tv2)
{
    return 1000l * (tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec) / 1000l;
}

/* returns the number of microseconds elapsed between tv1 and tv2 */
int usecelapsed(struct timeval tv1, struct timeval tv2)
{
    return 1000000l * (tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec);
}
#endif /* not NO_TIMER */

/* print out quality, adjusting to current quality measure */
starreal pq(starreal qual)
{
    if (improvebehave.qualmeasure == QUALMINSINE ||
        improvebehave.qualmeasure == QUALWARPEDMINSINE)
    {
        return sintodeg(qual);
    }
    return qual;
}


/* compute the rectangular bounding box for mesh vertices, so deformation and
   anisotropy can use normalized coordinates */
void setboundingbox(tetcomplex *mesh)
{
    starreal *v;
    tag iterator;
    
    G_boundingbox[0] = HUGEFLOAT;
    G_boundingbox[1] = HUGEFLOAT;
    G_boundingbox[2] = HUGEFLOAT;
    G_boundingbox[3] = -HUGEFLOAT;
    G_boundingbox[4] = -HUGEFLOAT;
    G_boundingbox[5] = -HUGEFLOAT;
    
    if (improvebehave.verbosity > 4)
    {
        printf("Computing mesh bounding box...\n");
    }
    
    /* loop over each mesh vertex, looking for extreme values */
    iterator = proxipooliterate(mesh->vertexpool, NOTATAG);
    while (iterator != NOTATAG) 
    {
        v = ((struct vertex *) proxipooltag2object(mesh->vertexpool, iterator))->coord;
        
        if (v[0] < G_boundingbox[0]) G_boundingbox[0] = v[0];
        if (v[1] < G_boundingbox[1]) G_boundingbox[1] = v[1];
        if (v[2] < G_boundingbox[2]) G_boundingbox[2] = v[2];
        if (v[0] > G_boundingbox[3]) G_boundingbox[3] = v[0];
        if (v[1] > G_boundingbox[4]) G_boundingbox[4] = v[1];
        if (v[2] > G_boundingbox[5]) G_boundingbox[5] = v[2];
        
        iterator = proxipooliterate(mesh->vertexpool, iterator);
    }
    
    /* set the range and center for each coordinate, for vertex normalization */
    G_range[0] = G_boundingbox[3] - G_boundingbox[0];
    G_range[1] = G_boundingbox[4] - G_boundingbox[1];
    G_range[2] = G_boundingbox[5] - G_boundingbox[2];
    G_center[0] = (G_boundingbox[3] + G_boundingbox[0]) / 2.0;
    G_center[1] = (G_boundingbox[4] + G_boundingbox[1]) / 2.0;
    G_center[2] = (G_boundingbox[5] + G_boundingbox[2]) / 2.0;    
    
    if (improvebehave.verbosity > 4)
    {
        printf("min corner [%g %g %g], max corner [%g %g %g]\n", G_boundingbox[0], G_boundingbox[1], G_boundingbox[2], G_boundingbox[3], G_boundingbox[4], G_boundingbox[5]);
        printf("ranges = [%g %g %g], center = [%g %g %g]\n", G_range[0], G_range[1], G_range[2], G_center[0], G_center[1], G_center[2]);
    }
}
