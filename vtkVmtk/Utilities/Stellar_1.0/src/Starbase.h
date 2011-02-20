/* This file was automatically generated.  Do not edit! */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#if !defined(NO_TIMER)
#include <sys/time.h>
#endif
#if defined(CPU86)
#include <float.h>
#endif
#if defined(LINUX)
#include <fpu_control.h>
#endif
#if !defined(NOMAIN)
int main(int argc,char **argv);
#endif
typedef struct behavior behavior;
typedef struct inputs inputs;
typedef struct outputs outputs;
typedef struct tetcomplex tetcomplex;
void statistics(struct behavior *behave,struct inputs *in,struct outputs *out,struct tetcomplex *plex);
void statisticsquality(struct behavior *behave,struct tetcomplex *plex);
typedef size_t starulong;
typedef starulong arraypoolulong;
void outputfaces(struct behavior *behave,struct inputs *in,struct tetcomplex *plex,arraypoolulong facecount,int argc,char **argv);
void outputedges(struct behavior *behave,struct inputs *in,struct tetcomplex *plex,arraypoolulong edgecount,int argc,char **argv);
void outputtetrahedra(struct behavior *behave,struct inputs *in,struct tetcomplex *plex,int argc,char **argv);
typedef struct proxipool proxipool;
void outputnumbervertices(struct behavior *behave,struct inputs *in,struct proxipool *pool);
void outputvertices(struct behavior *behave,struct inputs *in,struct proxipool *pool,arraypoolulong vertexcount,int argc,char **argv);
void outputpreparevertices(struct behavior *behave,struct tetcomplex *plex);
#if !defined(STARLIBRARY)
void outputfilefinish(FILE *outfile,int argc,char **argv);
#endif
typedef struct segment segment;
struct segment *inputsegments(struct behavior *behave,struct inputs *in,FILE *polyfile);
void inputsegment(struct behavior *behave,struct inputs *in,FILE *polyfile,arraypoolulong segnumber,int markflag,struct segment *seg);
int inputsegmentheader(struct behavior *behave,struct inputs *in,FILE *polyfile);
void inputtetrahedra(struct behavior *behave,struct inputs *in,struct proxipool *vertexpool,struct outputs *out,struct tetcomplex *plex);
typedef arraypoolulong tag;
void inputmaketagmap(struct proxipool *vertexpool,arraypoolulong firstnumber,tag *vertextags);
FILE *inputverticesreadsortstore(struct behavior *behave,struct inputs *in,struct proxipool *pool);
void inputverticessortstore(char *vertices,struct inputs *in,struct proxipool *pool);
void inputverticesintopool2(char *vertices,struct inputs *in,struct proxipool *pool);
void inputverticesintopool(char *vertices,struct inputs *in,struct proxipool *pool);
void inputvertexsort2(char *vertices,tag *vertextags,arraypoolulong vertexcount,size_t vertexbytes,unsigned int attribcount);
void inputvertexsort(char *vertices,tag *vertextags,arraypoolulong vertexcount,size_t vertexbytes,unsigned int attribcount);
char *inputvertexfile(struct behavior *behave,struct inputs *in,FILE **polyfile);
#if !defined(STARLIBRARY)
char *inputvertices(FILE *vertexfile,char *vertexfilename,struct inputs *in,int markflag);
char *inputfindfield(char *string);
char *inputtextline(char *string,FILE *infile,char *infilename);
#endif
int vertexcheckdelaunay(struct behavior *behave,struct tetcomplex *plex,tag vtx1,tag vtx2,tag vtx3,tag adjacencies[2]);
void vertexcheckpointintet(struct behavior *behave,struct tetcomplex *plex,tag vtx1,tag vtx2,tag vtx3,tag vtx4,tag invertex);
void vertexcheckorientation(struct behavior *behave,struct tetcomplex *plex,tag vtx1,tag vtx2,tag vtx3,tag vtx4);
struct outputs {
  arraypoolulong vertexcount;
  arraypoolulong tetcount;
  arraypoolulong facecount;
  arraypoolulong boundaryfacecount;
  arraypoolulong edgecount;
};
struct inputs {
  tag *vertextags;
  arraypoolulong vertexcount;                   /* Number of input vertices. */
  unsigned int attribcount;              /* Number of attributes per vertex. */
  arraypoolulong firstnumber;   /* Vertices are numbered starting from this. */
  arraypoolulong deadvertexcount;     /* Non-corner input vertices (killed). */
  arraypoolulong tetcount;                    /* Number of input tetrahedra. */
  unsigned int tetattribcount;      /* Number of attributes per tetrahedron. */
  arraypoolulong segmentcount;                  /* Number of input segments. */
  arraypoolulong facetcount;                      /* Number of input facets. */
  arraypoolulong holecount;                        /* Number of input holes. */
  arraypoolulong regioncount;                    /* Number of input regions. */
};
typedef struct facet facet;
typedef int vertexmarktype;
struct facet {
  vertexmarktype mark;
  arraypoolulong number;
};
struct segment {
  tag endpoint[2];
  struct segment *nextsegment[2];
  vertexmarktype mark;
  short acute[2];
  arraypoolulong number;
};
typedef struct vertexshort vertexshort;
typedef double starreal;
#if defined(SINGLE)
typedef float starreal;
#endif
struct vertexshort {
  vertexmarktype mark;
  starreal coord[3];
};
typedef struct vertex vertex;
struct vertex {
  starreal coord[3];
  vertexmarktype mark;
  arraypoolulong number;
};
#define ACTIVEVERTEX ((arraypoolulong) ~1)
#define DEADVERTEX ((arraypoolulong) ~0)
arraypoolulong tetcomplexbytes(struct tetcomplex *plex);
arraypoolulong tetcomplexghosttetcount(struct tetcomplex *plex);
arraypoolulong tetcomplextetcount(struct tetcomplex *plex);
void tetcomplexremoveghosttets(struct tetcomplex *plex);
typedef struct arraypool arraypool;
void tetcomplexbuild3dstar(struct tetcomplex *plex,tag newvertex,struct arraypool *trianglelist,arraypoolulong firstindex,arraypoolulong trianglecount);
void tetcomplex41flip(struct tetcomplex *plex,tag vtx1,tag vtx2,tag vtx3,tag vtx4,tag deletevertex);
void tetcomplex32flip(struct tetcomplex *plex,tag vtxtop,tag vtx1,tag vtx2,tag vtx3,tag vtxbot);
void tetcomplex23flip(struct tetcomplex *plex,tag vtxtop,tag vtx1,tag vtx2,tag vtx3,tag vtxbot);
void tetcomplex14flip(struct tetcomplex *plex,tag vtx1,tag vtx2,tag vtx3,tag vtx4,tag newvertex);
void tetcomplex12flipon6edges(struct tetcomplex *plex,tag vtx1,tag vtx2,tag vtx3,tag vtx4,tag newvertex);
void tetcomplex12flipon3edges(struct tetcomplex *plex,tag vtx1,tag vtx2,tag vtx3,tag vtx4,tag newvertex);
void tetcomplexsqueezetriangle(struct tetcomplex *plex,tag vtx1,tag vtx2,tag vtx3);
void tetcomplexsqueezeonedge(struct tetcomplex *plex,tag vtx1,tag vtx2,tag vtx3);
void tetcomplexsqueezeonhalfedge(struct tetcomplex *plex,tag vtx1,tag vtx2,tag vtx3);
void tetcomplexdeletetriangle(struct tetcomplex *plex,tag vtx1,tag vtx2,tag vtx3);
void tetcomplex21fliponedge(struct tetcomplex *plex,tag vtx1,tag vtx2,tag vtx3);
int tetcomplexinserttetontripod(struct tetcomplex *plex,tag vtx1,tag vtx2,tag vtx3,tag vtx4);
void tetcomplex12fliponedge(struct tetcomplex *plex,tag vtx1,tag vtx2,tag vtx3,tag vtx4,tag newvertex);
int tetcomplexinserttet(struct tetcomplex *plex,tag vtx1,tag vtx2,tag vtx3,tag vtx4);
int tetcomplexinsertorderedtet(struct tetcomplex *plex,tag vtx1,tag vtx2,tag vtx3,tag vtx4);
typedef struct tetcomplexstar tetcomplexstar;
struct tetcomplexstar *tetcomplexlookup3dstar(struct tetcomplex *plex,tag vtx);
int tetcomplexdeletetet(struct tetcomplex *plex,tag vtx1,tag vtx2,tag vtx3,tag vtx4);
int tetcomplexdeleteorderedtet(struct tetcomplex *plex,tag vtx1,tag vtx2,tag vtx3,tag vtx4);
void tetcomplexconsistency(struct tetcomplex *plex);
unsigned int tetcomplexmissingtet(struct tetcomplex *plex,tag vtx1,tag vtx2,tag vtx3,tag vtx4);
void tetcomplexprintstars(struct tetcomplex *plex);
void tetcomplexprint(struct tetcomplex *plex);
typedef struct tetcomplexposition tetcomplexposition;
void tetcomplexiteratenoghosts(struct tetcomplexposition *pos,tag nexttet[4]);
void tetcomplexiterate(struct tetcomplexposition *pos,tag nexttet[4]);
void tetcomplexiterateall(struct tetcomplexposition *pos,tag nexttet[4]);
void tetcomplexiteratorinit(struct tetcomplex *plex,struct tetcomplexposition *pos);
int tetcomplexvertex2tet(struct tetcomplex *plex,tag vtx,tag adjtet[4]);
int tetcomplexedge2tet(struct tetcomplex *plex,tag vtx1,tag vtx2,tag adjtet[4]);
int tetcomplexadjacencies(struct tetcomplex *plex,tag vtx1,tag vtx2,tag vtx3,tag adjacencies[2]);
int tetcomplexringadjacencies(struct tetcomplex *plex,tag vtx1,tag vtx2,tag vtx3,tag adjacencies[2]);
typedef struct minipoolheader minipoolheader;
#define arraypoolfastlookup(pool, index)  \
  (void *) ((pool)->toparray[(index) >> (pool)->log2objectsperblock] +  \
            ((index) & ((pool)->objectsperblock - 1)) * (pool)->objectbytes)
#define LOG2OBJECTSPERMINI 10
#define proxipooltag2mini(pool, searchtag)  \
  ((struct minipoolheader *)  \
   arraypoolfastlookup(&(pool)->minipoolarray,  \
                       (searchtag) >> LOG2OBJECTSPERMINI))
#define OBJECTSPERMINI 1024
#define proxipooltag2object2(pool, searchtag)  \
  ((void *) (proxipooltag2mini(pool, searchtag)->object2block +  \
             (searchtag & (tag) (OBJECTSPERMINI - 1)) * (pool)->objectbytes2))
#define tetcomplextag2attributes(plex, searchtag)  \
  proxipooltag2object2(plex->vertexpool, searchtag)
#define proxipooltag2object(pool, searchtag)  \
  ((void *) (proxipooltag2mini(pool, searchtag)->object1block +  \
             (searchtag & (OBJECTSPERMINI - 1)) * (pool)->objectbytes1))
#define tetcomplextag2vertex(plex, searchtag)  \
  proxipooltag2object(plex->vertexpool, searchtag)
void tetcomplexdeinit(struct tetcomplex *plex);
void tetcomplexrestart(struct tetcomplex *plex);
void tetcomplexinit(struct tetcomplex *plex,struct proxipool *vertexpool,int verbose);
typedef struct tetcomplexlinktriangle tetcomplexlinktriangle;
struct tetcomplexlinktriangle {
  tag vtx[3];
  arraypoolulong neighbor[3];
};
typedef struct link2dposition link2dposition;
typedef struct linkposition linkposition;
typedef char *molecule;
struct linkposition {
  molecule cule;      /* The molecule containing the atom (or right before). */
  int textindex;                                   /* The index of the atom. */
  int lasttextindex;   /* The last atom not part of the "next molecule" tag. */
  tag moleculetag;                       /* The tag for the molecule `cule'. */
  tag nextmoleculetag;  /* The next tag following `moleculetag' in the list. */
  tag groundtag;            /* Tag for the link's owner, used to decompress. */
  struct proxipool *pool;  /* The pool in which the molecules are allocated. */
};
struct link2dposition {
  struct linkposition innerpos;                  /* Position in the 2D link. */
  tag linktag;     /* Tag for the link's first molecule, used to decompress. */
};
struct tetcomplexposition {
  struct tetcomplex *mycomplex;          /* The complex this position is in. */
  tag starvertex;     /* The vertex whose star is currently being traversed. */
  struct link2dposition link2dposition;            /* That vertex's 2D link. */
  tag link2dvertex;                  /* Vertex in 2D link, denoting an edge. */
  struct linkposition link1dposition;           /* That edge's 1D link ring. */
  tag link1dfirstvertex;    /* First vertex in ring (needed at end of ring). */
  tag link1dprevvertex;                 /* Vertex visited on last iteration. */
};
struct tetcomplexstar {
  tag linkhead;                 /* First molecule of the link2d linked list. */
  tag linktail;                  /* Last molecule of the link2d linked list. */
};
struct arraypool {
  arraypoolulong objectbytes;       /* Size of one object in the lower tier. */
  arraypoolulong objectsperblock;           /* Objects per lower tier block. */
  arraypoolulong log2objectsperblock;      /* Base-2 logarithm of the above. */
  arraypoolulong totalmemory;     /* Total bytes used by whole tiered array. */
  arraypoolulong objects;          /* Number of currently allocated objects. */

  char **toparray;                             /* Pointer to the upper tier. */
  arraypoolulong toparraylen;      /* Length of the upper tier, in pointers. */
  arraypoolulong firstvirgin;          /* First never-allocated array index. */
  arraypoolulong deadindexstack;        /* Stack of freed objects, by index. */
  void *deadobjectstack;              /* Stack of freed objects, by pointer. */
};
typedef arraypoolulong proxipoolulong;
struct proxipool {
  size_t objectbytes1;                    /* Size of one object in the pool. */
  size_t objectbytes2;                      /* Size of supplementary object. */
  size_t block1offset;                /* Offset of first object from header. */
  size_t block2offset;              /* Offset of first supplementary object. */
  size_t minipoolsize;            /* Size of one minipool, including header. */

  struct arraypool minipoolarray;              /* Tiered array of minipools. */
  arraypoolulong objects;          /* Number of currently allocated objects. */
  arraypoolulong maxobjects;              /* Maximum allocated objects ever. */
  tag nextgroup;                 /* Next group of minipools to be allocated. */

  struct arraypool poolpools;                  /* Tiered array of poolpools. */
  proxipoolulong nextinitindex;   /* First uninitialized index in the array. */
  int verbosity;                /* Amount of debugging information to print. */
};
#define LINK2DCACHESIZE 16384
typedef struct {
  tag mylink2d;
  tag myvertex;
  tag mylinkring;
}link2dcache[LINK2DCACHESIZE];
struct tetcomplex {
  struct proxipool moleculepool;     /* Pool of molecules storing the links. */
  struct arraypool stars;    /* `tetcomplexstar' array addressing the links. */
  struct proxipool *vertexpool;                     /* The pool of vertices. */
  tag nextinitindex;      /* First uninitialized index in the `stars' array. */
  arraypoolulong tetcount;           /* Number of tetrahedra in the complex. */
  arraypoolulong ghosttetcount;          /* Ghost tetrahedra in the complex. */
  int consistentflag;           /* Are the stars consistent with each other? */
  int verbosity;                /* Amount of debugging information to print. */
  link2dcache cache;                  /* Cache for fast lookups in 2D links. */
};
void tricircumcenter3d(struct behavior *b,starreal *triorg,starreal *tridest,starreal *triapex,starreal *circumcenter,starreal *normal,starreal *xi,starreal *eta);
void tetcircumcenter(struct behavior *b,starreal *tetorg,starreal *tetdest,starreal *tetfapex,starreal *tettapex,starreal *circumcenter,starreal *xi,starreal *eta,starreal *zeta);
starreal nonregular(struct behavior *b,starreal *pa,starreal *pb,starreal *pc,starreal *pd,starreal *pe);
starreal orient4d(struct behavior *b,starreal *pa,starreal *pb,starreal *pc,starreal *pd,starreal *pe,starreal aheight,starreal bheight,starreal cheight,starreal dheight,starreal eheight);
starreal orient4dadapt(starreal *pa,starreal *pb,starreal *pc,starreal *pd,starreal *pe,starreal aheight,starreal bheight,starreal cheight,starreal dheight,starreal eheight,starreal permanent);
starreal orient4dexact(starreal *pa,starreal *pb,starreal *pc,starreal *pd,starreal *pe,starreal aheight,starreal bheight,starreal cheight,starreal dheight,starreal eheight);
starreal insphere(struct behavior *b,starreal *pa,starreal *pb,starreal *pc,starreal *pd,starreal *pe);
starreal insphereadapt(starreal *pa,starreal *pb,starreal *pc,starreal *pd,starreal *pe,starreal permanent);
starreal insphereexact(starreal *pa,starreal *pb,starreal *pc,starreal *pd,starreal *pe);
starreal orient3d(struct behavior *b,starreal *pa,starreal *pb,starreal *pc,starreal *pd);
starreal orient3dadapt(starreal *pa,starreal *pb,starreal *pc,starreal *pd,starreal permanent);
starreal orient2d(struct behavior *b,starreal ax,starreal ay,starreal bx,starreal by,starreal cx,starreal cy);
starreal orient2dadapt(starreal ax,starreal ay,starreal bx,starreal by,starreal cx,starreal cy,starreal detsum);
starreal estimate(int elen,starreal *e);
int scale_expansion_zeroelim(int elen,starreal *e,starreal b,starreal *h);
int fast_expansion_sum_zeroelim(int elen,starreal *e,int flen,starreal *f,starreal *h);
void primitivesinit(void);
tag link2ddeletevertex(struct proxipool *pool,link2dcache cache,tag linkhead,tag *linktail,tag groundtag,tag deletevertex);
tag link2ddeletevertexnocache(struct proxipool *pool,tag linkhead,tag *linktail,tag groundtag,tag deletevertex);
tag link2dfindinsert(struct proxipool *pool,link2dcache cache,tag linkhead,tag *linktail,tag groundtag,tag searchvertex);
void link2dinsertvertex(struct proxipool *pool,link2dcache cache,tag linkhead,tag *linktail,tag groundtag,tag newvertex,tag linkring);
void link2dinsertvertexnocache(struct proxipool *pool,tag linkhead,tag *linktail,tag groundtag,tag newvertex,tag linkring);
tag link2dfindring(struct proxipool *pool,link2dcache cache,tag link,tag groundtag,tag searchvertex);
tag link2dfindringnocache(struct proxipool *pool,tag link,tag groundtag,tag searchvertex,tag *linktail);
void link2dcacheinit(link2dcache cache);
void link2dprint(struct proxipool *pool,tag link,tag groundtag);
void link2diterate(struct link2dposition *pos,tag vertexandlink[2]);
void link2diteratorinit(struct proxipool *pool,tag link,tag groundtag,struct link2dposition *pos);
int link2disempty(struct proxipool *pool,tag link);
void link2ddelete(struct proxipool *pool,tag link);
tag link2dnew(struct proxipool *pool,proxipoolulong allocindex);
#define LINK2DPRIME 16908799u
int linkringdelete2vertices(struct proxipool *pool,tag linkring,tag groundtag,tag deletevertex);
int linkringdeletevertex(struct proxipool *pool,tag linkring,tag groundtag,tag deletevertex);
int linkringinsertvertex(struct proxipool *pool,tag linkring,tag groundtag,tag searchvertex,tag newvertex);
int linkringdeleteedge(struct proxipool *pool,tag linkring,tag groundtag,tag endpoint1,tag endpoint2);
int linkringinsertedge(struct proxipool *pool,tag linkring,tag groundtag,tag endpoint1,tag endpoint2);
typedef struct linkpossmall linkpossmall;
void linkringrotateatoms(struct proxipool *pool,tag linkring,struct linkpossmall *pos1,struct linkpossmall *pos2,struct linkpossmall *pos3);
tag linkringdeleteatoms(struct proxipool *pool,struct linkpossmall *deleteposition,int numatoms);
tag linkringdeleteatoms2(struct proxipool *pool,struct linkpossmall *deleteposition,int numatoms);
tag linkringinsertatoms(struct proxipool *pool,struct linkpossmall *insertposition,int numatoms,char *newatombuffer,proxipoolulong allocindex);
tag linkringinsertatoms2(struct proxipool *pool,struct linkpossmall *insertposition,int numatoms,char *newatombuffer,proxipoolulong allocindex);
void linkringprint(struct proxipool *pool,tag linkring,tag groundtag);
tag linkringiterate(struct linkposition *pos);
void linkringiteratorinit(struct proxipool *pool,tag linkring,tag groundtag,struct linkposition *pos);
int linkringadjacencies(struct proxipool *pool,tag linkring,tag groundtag,tag searchvertex,tag adjacencies[2]);
int linkringadjacencies2(struct proxipool *pool,tag linkring,tag groundtag,tag searchvertex,tag adjacencies[2]);
void linkringdelete(struct proxipool *pool,tag linkring);
void linkringrestart(struct proxipool *pool,tag linkring);
#define MOLECULESIZE 20
#define STOP (~ (tag) 1)
void internalerror(void);
#define linkringreadtag(moleculetag, cule, atomindex, nextmoleculetag,  \
                        nowatom, errstring)  \
  nowatom = cule[MOLECULESIZE - 1];  \
  atomindex = MOLECULESIZE - 2;  \
  if (nowatom == (char) STOP) {  \
    nextmoleculetag = STOP;  \
  } else {  \
    nextmoleculetag = nowatom & (char) 127;  \
    while (nowatom >= (char) 0) {  \
      if (atomindex < 0) {  \
        printf(errstring);  \
        printf("  Tag for next molecule not properly terminated.\n");  \
        internalerror();  \
      }  \
      nowatom = cule[atomindex];  \
      nextmoleculetag = (nextmoleculetag << 7) + (nowatom & (char) 127);  \
      atomindex--;  \
    }  \
    nextmoleculetag += (moleculetag >>  \
                        (7 * (MOLECULESIZE - 1 - atomindex))) <<  \
                       (7 * (MOLECULESIZE - 1 - atomindex));  \
  }
tag linkringnewfill(struct proxipool *pool,tag groundtag,tag *tagarray,proxipoolulong tagarraylen,proxipoolulong allocindex);
tag linkringnew(struct proxipool *pool,proxipoolulong allocindex);
int linkringtagcompress(tag groundtag,tag newtag,char *newtagatoms);
struct linkpossmall {
  molecule cule;      /* The molecule containing the atom (or right before). */
  int textindex;                                   /* The index of the atom. */
  int lasttextindex;   /* The last atom not part of the "next molecule" tag. */
  tag moleculetag;                       /* The tag for the molecule `cule'. */
  tag nextmoleculetag;  /* The next tag following `moleculetag' in the list. */
};
#define COMPRESSEDTAGLENGTH (8 * sizeof(tag) / 7 + 2)
#define MOLECULEQUEUESIZE 40
#define GHOSTVERTEX (~ (tag) 0)
typedef struct allocmap allocmap;
arraypoolulong allocmapbytes(struct allocmap *tree);
void allocmapdeletepoint(struct allocmap *tree,starreal x,starreal y,starreal z);
proxipoolulong allocmapnewpoint(struct allocmap *tree,starreal x,starreal y,starreal z);
proxipoolulong allocmapindex(struct allocmap *tree,starreal x,starreal y,starreal z);
typedef struct allocmapnode allocmapnode;
void allocmapremoveroot(struct allocmapnode **treeroot);
void allocmapinsertroot(struct allocmapnode **treeroot,struct allocmapnode *insertnode,int splayside);
void allocmapmax(struct allocmapnode **treeroot);
int allocmapsplay(struct allocmapnode **treeroot,starreal x,starreal y,starreal z,struct allocmapnode **nearnode);
struct allocmapnode *allocmapnewnode(struct allocmap *tree);
void allocmapdeinit(struct allocmap *tree);
void allocmapinit(struct allocmap *tree,int verbose);
void allocmaprestart(struct allocmap *tree);
struct allocmap {
  struct arraypool nodearray;      /* Tiered array of allocation tree nodes. */
  struct allocmapnode *allocroot;                /* Root of allocation tree. */
  struct allocmapnode *spareroot;                /* Root of spare node tree. */
  int verbosity;                /* Amount of debugging information to print. */
};
struct allocmapnode {
  struct allocmapnode *left, *right;          /* My left and right children. */
  starreal xsam, ysam, zsam;     /* Coordinates of this node's sample point. */
  proxipoolulong index;                      /* My index (in the arraypool). */
};
#define LOG2OCTREENODESPERBLOCK 8
int zorderbefore(starreal x1,starreal y1,starreal z1,starreal x2,starreal y2,starreal z2);
arraypoolulong proxipoolbytes(struct proxipool *pool);
arraypoolulong proxipoolobjects(struct proxipool *pool);
void proxipoolfree(struct proxipool *pool,tag killtag);
tag proxipoolnew(struct proxipool *pool,proxipoolulong allocindex,void **outobject);
void proxipoolinitpoolpools(struct proxipool *pool,proxipoolulong endindex);
tag proxipooliterate(struct proxipool *pool,tag thistag);
#define proxipooltag2allocindex(pool, searchtag)  \
  (proxipooltag2mini(pool, searchtag)->allocindex)
void proxipooldeinit(struct proxipool *pool);
void proxipoolinit(struct proxipool *pool,size_t objectbytes1,size_t objectbytes2,int verbose);
void proxipoolrestart(struct proxipool *pool);
void proxipoolrestartmini(struct proxipool *pool,struct minipoolheader *mini);
typedef struct poolpool poolpool;
struct poolpool {
  tag freelisthead;            /* Head of list of minipools with free space. */
  tag freelisttail;            /* Tail of list of minipools with free space. */
  tag mygroup;                 /* Next minipool to be allocated in my group. */
};
typedef unsigned short miniindex;
struct minipoolheader {
  char *object1block;    /* Pointer to the block of objects in the minipool. */
  char *object2block;          /* Optional pointer to supplementary objects. */
  proxipoolulong allocindex;                       /* Index of the poolpool. */
  tag nextminifree;     /* Next in linked list of minipools with free space. */
  miniindex firstvirgin;        /* First never-allocated object in minipool. */
  miniindex freestack; /* Head of linked stack of freed objects in minipool. */
};
#define LOG2POOLPOOLSPERBLOCK 8
#define MINISPERGROUP 8
#define NOTAMINIINDEX ((miniindex) ~0)
#define NOTATAG ((tag) ~0)
arraypoolulong arraypoolbytes(struct arraypool *pool);
arraypoolulong arraypoolallocated(struct arraypool *pool);
void arraypoolfreeindex(struct arraypool *pool,arraypoolulong dyingindex);
arraypoolulong arraypoolnewindex(struct arraypool *pool,void **newptr);
void arraypoolfreeptr(struct arraypool *pool,void *dyingobject);
void *arraypoolnewptr(struct arraypool *pool);
void *arraypoollookup(struct arraypool *pool,arraypoolulong index);
void *arraypoolforcelookup(struct arraypool *pool,arraypoolulong index);
char *arraypoolsecondtier(struct arraypool *pool,arraypoolulong index);
void arraypooldeinit(struct arraypool *pool);
void arraypoolinit(struct arraypool *pool,arraypoolulong objectbytes,arraypoolulong log2objectsperblock,int alloctype);
void arraypoolrestart(struct arraypool *pool);
#define NOTAPOOLINDEX ((arraypoolulong) ~0)
#define TOPRESIZEFACTOR 3
#define TOPARRAYSTARTSIZE 128
unsigned long randomnation(unsigned long choices);
extern unsigned long randomseed;
void parsecommandline(int argc,char **argv,struct behavior *b);
#if !defined(STARLIBRARY)
void info(void);
void syntax(void);
#endif
void starfree(void *memptr);
void *starmalloc(size_t size);
void starexit(int status);
extern starreal isperrboundA,isperrboundB,isperrboundC;
extern starreal o3derrboundA,o3derrboundB,o3derrboundC;
extern starreal o2derrboundA,o2derrboundB,o2derrboundC;
extern starreal resulterrbound;
extern starreal epsilon;
extern starreal splitter;
typedef ptrdiff_t starlong;
#define FILENAMESIZE 2048
struct behavior {

/*  Switches for the tetrahedralizer.                                        */
/*  Read the instructions to find out the meaning of these switches.         */

  int poly;                                                    /* -p switch. */
  int refine;                                                  /* -r switch. */
  int quality;                                                 /* -q switch. */
  /* Maximum acceptable tetrahedron circumradius-to-shortest edge ratio:     */
  starreal qmeasure;                           /* Specified after -q switch. */
  starreal minangle;          /* Min dehedral angle bound, after -q/ switch. */
  starreal goodangle;                         /* Cosine squared of minangle. */
  int varvolume;                      /* -a switch without number following. */
  int fixedvolume;                       /* -a switch with number following. */
  starreal maxvolume;    /* Maximum volume bound, specified after -a switch. */
  int usertest;                                                /* -u switch. */
  int regionattrib;                                            /* -A switch. */
  int convex;                                                  /* -c switch. */
  int weighted;                         /* 1 for -w switch, 2 for -W switch. */
  int conformdel;                                              /* -D switch. */
  int jettison;                                                /* -j switch. */
  int edgesout;                                                /* -e switch. */
  int facesout;                                                /* -f switch. */
  int voronoi;                                                 /* -v switch. */
  int neighbors;                                               /* -n switch. */
  int geomview;                                                /* -g switch. */
  int nobound;                                                 /* -B switch. */
  int nopolywritten;                                           /* -P switch. */
  int nonodewritten;                                           /* -N switch. */
  int noelewritten;                                            /* -E switch. */
  int noiterationnum;                                          /* -I switch. */
  int noholes;                                                 /* -O switch. */
  int noexact;                                                 /* -X switch. */
  /* All items are numbered starting from `firstnumber':                     */
  starulong firstnumber;                            /* Inverse of -z switch. */
  int order;                    /* Element order, specified after -o switch. */
  int nobisect;                 /* Count of how often -Y switch is selected. */
  starlong steiner;   /* Max # of Steiner points, specified after -S switch. */
  int jumpwalk;                                                /* -J switch. */
  int norandom;                                                /* -k switch. */
  int fullrandom;                                              /* -K switch. */
  int docheck;                                                 /* -C switch. */
  int quiet;                                                   /* -Q switch. */
  int verbose;                  /* Count of how often -V switch is selected. */
  /* Determines whether new vertices will be added, other than the input:    */
  int addvertices;                              /* -p, -q, -a, or -u switch. */
  /* Determines whether segments and facets are used at all:                 */
  int usefacets;                        /* -p, -r, -q, -a, -u, or -c switch. */

  int readnodefileflag;                       /* Has a .node file been read? */

/*  Variables for file names.                                                */

#ifndef STARLIBRARY
  char innodefilename[FILENAMESIZE];                    /* Input .node file. */
  char inelefilename[FILENAMESIZE];                      /* Input .ele file. */
  char inpolyfilename[FILENAMESIZE];                    /* Input .poly file. */
  char areafilename[FILENAMESIZE];                      /* Input .area file. */
  char outnodefilename[FILENAMESIZE];                  /* Output .node file. */
  char outelefilename[FILENAMESIZE];                    /* Output .ele file. */
  char outpolyfilename[FILENAMESIZE];                  /* Output .poly file. */
  char edgefilename[FILENAMESIZE];                     /* Output .edge file. */
  char facefilename[FILENAMESIZE];                     /* Output .face file. */
  char vnodefilename[FILENAMESIZE];                  /* Output .v.node file. */
  char vpolyfilename[FILENAMESIZE];                  /* Output .v.poly file. */
  char neighborfilename[FILENAMESIZE];                /* Output .neigh file. */
  char offfilename[FILENAMESIZE];                       /* Output .off file. */
#endif /* not STARLIBRARY */

/*  Counts of operations performed.                                          */

  starulong inspherecount;            /* Number of insphere tests performed. */
  starulong orientcount;        /* Number of 3D orientation tests performed. */
  starulong orient4dcount;      /* Number of 4D orientation tests performed. */
  starulong tetcircumcentercount;/* Number of tet circumcenter calculations. */
  starulong tricircumcentercount;    /* Triangular face circumcenter calc's. */

};
#define PI 3.141592653589793238462643383279502884197169399375105820974944592308
#define INPUTLINESIZE 1024
#define INEXACT /* Nothing */
#define INTERFACE 0
