/*****************************************************************************/
/*                                                                           */
/*   ,d88""\   d8                    888                                     */
/*   8888    _d88__   o8888o  88o88o 888o888e    o8888o   d88"\  e88888e     */
/*   `Y88b    888         88b 888    888  888b       88b C88C   d888  88b    */
/*    `Y88b,  888    o888o888 888    888  8888  e888o888  "88b  8888oo888    */
/*      8888  888   C888  888 888    888  888P C888  888   `88D q888         */
/*   \_o88P'  "88o"  "888"888 888    888"888"   "888"888 \_88P   "88oooo"    */
/*                                                                           */
/*  A Tetrahedral Complex Data Structure Library.                            */
/*  (Starbase.c)                                                             */
/*                                                                           */
/*  Version 0.12                                                             */
/*  13 December 2008                                                         */
/*                                                                           */
/*  Portions of Starbase written prior to June 30, 1998 are                  */
/*  Copyright 1995, 1996, 1997, 1998                                         */
/*  Jonathan Richard Shewchuk                                                */
/*  965 Sutter Street #815                                                   */
/*  San Francisco, California  94109-6082                                    */
/*  jrs@cs.berkeley.edu                                                      */
/*                                                                           */
/*  Portions of Starbase written after June 30, 1998 are in the public       */
/*  domain, but Starbase as a whole is not.  All rights reserved.            */
/*                                                                           */
/*  This version of Starbase is provided as part of Stellar, a program for   */
/*  improving tetrahedral meshes.  Stellar and this version of Starbase are  */
/*  open source software provided under the Berkeley Source Distribution     */
/*  (BSD) license, which follows.  If you want to use Stellar in a           */
/*  commercial product, the BSD license permits you to do so freely.  Bryan  */
/*  Klingner and I request that you kindly send me an email to let us know   */
/*  what products include Stellar, but it is not a legal requirement.        */
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
/*  - Neither the name of Jonathan Shewchuk nor the name of the University   */
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
/*  Additional disclaimer:  Neither I nor any institution I have been        */
/*  associated with warrant this code in any way whatsoever.  This code is   */
/*  provided "as is".  Use at your own risk.                                 */
/*                                                                           */
/*  The triangulation data structures are adapted from the star-based        */
/*  simplex dictionary of Daniel K. Blandford, Guy E. Blelloch, David E.     */
/*  Cardoze, and Clemens Kadow, "Compact Representations of Simplicial       */
/*  Meshes in Two and Three Dimensions," International Journal of            */
/*  Computational Geometry and Applications 15(1):3-24, February 2005.       */
/*                                                                           */
/*  The algorithms for exact computation of the signs of determinants are    */
/*  described in Jonathan Richard Shewchuk, "Adaptive Precision Floating-    */
/*  Point Arithmetic and Fast Robust Geometric Predicates," Discrete &       */
/*  Computational Geometry 18(3):305-363, October 1997.  (Also available as  */
/*  Technical Report CMU-CS-96-140, School of Computer Science, Carnegie     */
/*  Mellon University, Pittsburgh, Pennsylvania, May 1996.)                  */
/*  An abbreviated version appears as Jonathan Richard Shewchuk, "Robust     */
/*  Adaptive Floating-Point Geometric Predicates," Proceedings of the        */
/*  Twelfth Annual Symposium on Computational Geometry, ACM, May 1996.       */
/*  Many of the ideas for my exact arithmetic routines originate with        */
/*  Douglas M. Priest, "Algorithms for Arbitrary Precision Floating Point    */
/*  Arithmetic," Tenth Symposium on Computer Arithmetic, pp. 132-143, IEEE   */
/*  Computer Society Press, 1991.  Many of the ideas for the correct         */
/*  evaluation of the signs of determinants are taken from Steven Fortune    */
/*  and Christopher J. Van Wyk, "Efficient Exact Arithmetic for              */
/*  Computational Geometry," Proceedings of the Ninth Annual Symposium on    */
/*  Computational Geometry, ACM, pp. 163-172, May 1993, and from Steven      */
/*  Fortune, "Numerical Stability of Algorithms for 2D Delaunay Triangu-     */
/*  lations," International Journal of Computational Geometry & Applications */
/*  5(1-2):193-213, March-June 1995.                                         */
/*                                                                           */
/*  The geometric predicates appear in my "Lecture Notes on Geometric        */
/*  Robustness" at http://www.cs.berkeley.edu/~jrs/mesh .                    */
/*                                                                           */
/*  If you make any improvements to this code, please please please let me   */
/*  know, so that I may obtain the improvements.  Even if you don't change   */
/*  the code, I'd still love to hear what it's being used for.               */
/*                                                                           */
/*****************************************************************************/


/*  For single precision (which will save some memory and reduce paging),    */
/*  define the symbol SINGLE by using the -DSINGLE compiler switch or by     */
/*  writing "#define SINGLE" below.                                          */
/*                                                                           */
/*  For double precision (which will allow you to refine meshes to a smaller */
/*  edge length), leave SINGLE undefined.                                    */

/* #define SINGLE */

#ifdef SINGLE
typedef float starreal;
#else /* not SINGLE */
typedef double starreal;
#endif /* not SINGLE */

/*  To insert lots of self-checks for internal errors, define the SELF_CHECK */
/*  symbol.  This will slow down the program significantly.  It is best to   */
/*  define the symbol using the -DSELF_CHECK compiler switch, but you could  */
/*  write "#define SELF_CHECK" below.  If you are modifying this code, I     */
/*  recommend you turn self-checks on until your work is debugged.           */

/* #define SELF_CHECK */
/* #define PARANOID */

/*  On some machines, my exact arithmetic routines might be defeated by the  */
/*  use of internal extended precision floating-point registers.  The best   */
/*  way to solve this problem is to set the floating-point registers to use  */
/*  single or double precision internally.  On 80x86 processors, this may be */
/*  accomplished by setting the CPU86 symbol in Microsoft operating systems, */
/*  or the LINUX symbol in Linux.                                            */
/*                                                                           */
/*  An inferior solution is to declare certain values as `volatile', thus    */
/*  forcing them to be stored to memory and rounded off.  Unfortunately,     */
/*  this solution might slow Triangle down quite a bit.  To use volatile     */
/*  values, write "#define INEXACT volatile" below.  Normally, however,      */
/*  INEXACT should be defined to be nothing.  ("#define INEXACT".)           */
/*                                                                           */
/*  For more discussion, see Section 5 of my paper, "Adaptive Precision      */
/*  Floating-Point Arithmetic and Fast Robust Geometric Predicates" (also    */
/*  available as Section 6.6 of my dissertation).                            */

/* #define CPU86 */
/* #define LINUX */

#define INEXACT /* Nothing */
/* #define INEXACT volatile */

/*  Maximum number of characters in a file name (including the null).        */

#define FILENAMESIZE 2048

/*  Maximum number of characters in a line read from a file (including the   */
/*  null).                                                                   */

#define INPUTLINESIZE 1024

/*  A number that speaks for itself, every kissable digit.                   */

#define PI 3.141592653589793238462643383279502884197169399375105820974944592308


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#ifdef STARTIMER
#include <sys/time.h>
#endif /* not STARTIMER */
#ifdef CPU86
#include <float.h>
#endif /* CPU86 */
#ifdef LINUX
#include <fpu_control.h>
#endif /* LINUX */


/*  `starlong' and `starulong' are the types of integer (signed and          */
/*  unsigned, respectively) of most of the indices used internally and       */
/*  externally by Starbase, including vertex and tetrahedron numbers.  They  */
/*  determine the number of internal data structures that can be allocated,  */
/*  so long choices (e.g. ptrdiff_t and size_t, defined in stddef.h) are     */
/*  recommended.  If the number of tetrahedra might be around 2^28 or more,  */
/*  use 64-bit integers.  On a machine with 32-bit pointers (memory          */
/*  addresses), though, there's no point using integers bigger than 32 bits. */
/*  On a machine with limited memory, smaller integers might allow you to    */
/*  create larger meshes.                                                    */

typedef ptrdiff_t starlong;
typedef size_t starulong;


/*  Data structure for command line switches, file names, and operation      */
/*  counts.  Used (instead of global variables) to allow reentrancy.         */

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

};                                              /* End of `struct behavior'. */

/*  Global constants.                                                        */

starreal splitter;   /* Used to split real factors for exact multiplication. */
starreal epsilon;                         /* Floating-point machine epsilon. */
starreal resulterrbound;
starreal o2derrboundA, o2derrboundB, o2derrboundC;
starreal o3derrboundA, o3derrboundB, o3derrboundC;
starreal isperrboundA, isperrboundB, isperrboundC;


/********* Memory allocation and program exit wrappers begin here    *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  starexit()   Exits the program Starbase.                                 */
/*                                                                           */
/*  Used to give Starbase a single point of exit whose contents can easily   */
/*  be replaced to help interface with client programs.                      */
/*                                                                           */
/*  status:  Should be zero on normal termination; one if an error occurs.   */
/*                                                                           */
/*****************************************************************************/

void starexit(int status)
{
  exit(status);
}

/*****************************************************************************/
/*                                                                           */
/*  starmalloc()   Allocates memory from the operating system.               */
/*                                                                           */
/*  Used to give Starbase a single point of memory allocation whose contents */
/*  can easily be replaced to help interface with client programs.           */
/*                                                                           */
/*  size:  The number of contiguous bytes of memory to allocate.             */
/*                                                                           */
/*  Returns a pointer to the allocated memory.                               */
/*                                                                           */
/*****************************************************************************/

void *starmalloc(size_t size)
{
  void *memptr;

  memptr = malloc(size);
  if (memptr == (void *) NULL) {
    printf("Error:  Out of memory.\n");
    starexit(1);
  }
  return(memptr);
}

/*****************************************************************************/
/*                                                                           */
/*  starfree()   Returns previously allocated memory to the operating system.*/
/*                                                                           */
/*  Used to give Starbase a single point of memory freeing whose contents    */
/*  can easily be replaced to help interface with client programs.           */
/*                                                                           */
/*  memptr:  A pointer to the block of memory that should be freed so that   */
/*    it is available to be reallocated.                                     */
/*                                                                           */
/*****************************************************************************/

void starfree(void *memptr)
{
  free(memptr);
}

/**                                                                         **/
/**                                                                         **/
/********* Memory allocation and program exit wrappers end here      *********/


/********* User interaction routines begin here                      *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  internalerror()   Ask the user to send me the defective product.  Exit.  */
/*                                                                           */
/*****************************************************************************/

void internalerror(void)
{
  printf("  Please report this bug to jrs@cs.berkeley.edu\n");
  printf("  Include the message above, your input data set, and the exact\n");
  printf("  command line you used to run Star.\n");
  starexit(1);
}

/*****************************************************************************/
/*                                                                           */
/*  syntax()   Print a list of command line switches.                        */
/*                                                                           */
/*****************************************************************************/

#ifndef STARLIBRARY

void syntax(void)
{
#ifdef CDT_ONLY
  printf("Star [-pAcwWjefvngBPNEIOXzo_CQVh] input_file\n");
#else /* not CDT_ONLY */
  printf("Star [-prq__a__uAcwWDjefvngBPNEIOXzo_YSkK__CQVh] input_file\n");
#endif /* not CDT_ONLY */

  printf("    -p  Tetrahedralizes a polyhedron or PLC (.poly file).\n");
#ifndef CDT_ONLY
  printf("    -r  Refines a previously generated mesh.\n");
  printf("    -q  Quality mesh generation.\n");
  printf("    -a  Applies a maximum tetrahedron volume constraint.\n");
  printf("    -u  Applies a user-defined tetrahedron constraint.\n");
#endif /* not CDT_ONLY */
  printf("    -A  Applies attributes to identify tetrahedra in certain "
         "regions.\n");
  printf("    -c  Encloses the convex hull with facets.\n");
  printf("    -w  Weighted Delaunay triangulation.\n");
  printf("    -W  Regular triangulation (lower convex hull of a height "
         "field).\n");
#ifndef CDT_ONLY
  printf("    -D  Conforming Delaunay:  attempts to make tetrahedra "
         "Delaunay.\n");
#endif /* not CDT_ONLY */
  printf("    -j  Renumber nodes and jettison unused vertices from output.\n");
  printf("    -e  Generates an edge list (.edge file).\n");
  printf("    -f  Generates a face list (.face file).\n");
  printf("    -v  Generates a Voronoi diagram.\n");
  printf("    -n  Generates a list of tetrahedron neighbors (.neigh file).\n");
  printf("    -g  Generates an .off file for Geomview.\n");
  printf("    -B  Suppresses output of boundary information.\n");
  printf("    -P  Suppresses output of .poly file.\n");
  printf("    -N  Suppresses output of .node file.\n");
  printf("    -E  Suppresses output of .ele file.\n");
  printf("    -I  Suppresses mesh iteration numbers.\n");
  printf("    -O  Ignores holes in .poly file.\n");
  printf("    -X  Suppresses use of exact arithmetic.\n");
  printf("    -z  Numbers all items starting from zero (rather than one).\n");
  printf("    -o2 Generates second-order subparametric elements.\n");
#ifndef CDT_ONLY
  printf("    -Y  Suppresses boundary facet splitting.\n");
  printf("    -S  Specifies maximum number of added Steiner points.\n");
#endif /* not CDT_ONLY */
  printf("    -k  Insert vertices in original order (no randomization).\n");
  printf("    -K  Insert vertices in fully random order (instead of BRIO).\n");
  printf("    -C  Check consistency of final mesh.\n");
  printf("    -Q  Quiet:  No terminal output except errors.\n");
  printf("    -V  Verbose:  Detailed information on what I'm doing.\n");
  printf("    -h  Help:  Detailed instructions for Star.\n");
  printf("PRE-RELEASE CODE:  DO NOT DISTRIBUTE!!!\n");
  printf("EXPECT THIS CODE TO BE BUGGY AND SLOW.\n");
  starexit(0);
}

/* bryan's one-off syntax */
void stellarsyntax(void)
{
  printf("Usage:\n\n");
  printf("Stellar [-s configfile -L verbosity -F] input_file\n");

  printf("    -s  The filename that follows will set all of Stellar's options.\n"); 
  printf("        (This is the primary method for controlling Stellar's behavior.\n");
  printf("        refer to EXAMPLE_CONFIG for a description of configuration options.)\n");
  printf("    -L  The integer >= 0 that follows will set the verbosity of Stellar's output.\n");
  printf("    -F  Compute quality statistics of the input mesh, output them and quit.\n");
  starexit(0);
}

#endif /* not STARLIBRARY */

/*****************************************************************************/
/*                                                                           */
/*  info()   Print out complete instructions.                                */
/*                                                                           */
/*****************************************************************************/

#ifndef STARLIBRARY

void info(void)
{
  printf("Star\n");
  printf(
"A Three-Dimensional Quality Mesh Generator and Delaunay Tetrahedralizer.\n");
  printf("Version 0.1\n\n");
  printf("Copyright 1995, 1996, 1997, 1998 Jonathan Richard Shewchuk\n");
  printf("2360 Woolsey #H / Berkeley, California  94705-1927\n");
  printf("Bugs/comments to jrs@cs.berkeley.edu\n");
  printf(
"Created as part of the Quake project (tools for earthquake simulation).\n");
  printf(
"Supported in part by NSF Award CMS-9318163 and an NSERC 1967 Scholarship.\n");
  printf("There is no warranty whatsoever.  Use at your own risk.\n");
  printf("PRE-RELEASE CODE:  DO NOT DISTRIBUTE!!!\n");
  printf("EXPECT THIS CODE TO BE BUGGY.\n");
#ifdef SINGLE
  printf("This executable is compiled for single precision arithmetic.\n\n\n");
#else /* not SINGLE */
  printf("This executable is compiled for double precision arithmetic.\n\n\n");
#endif /* not SINGLE */
  starexit(0);
}

/* bryan's short, one-off info function */
void stellarinfo(void)
{
  printf("Stellar\n");
  printf(
"A Tetrahedral Mesh Improvement Program.\n");
  printf("Version 0.1\n\n");
  printf("Copyright 2006-2008 Bryan Klingner\n");
  printf("Bugs/comments to stellar.b@overt.org\n");
#ifdef SINGLE
  printf("This executable is compiled for single precision arithmetic.\n\n\n");
#else /* not SINGLE */
  printf("This executable is compiled for double precision arithmetic.\n\n\n");
#endif /* not SINGLE */
  starexit(0);
}

#endif /* not STARLIBRARY */

/*****************************************************************************/
/*                                                                           */
/*  parsecommandline()   Read the command line, identify switches, and set   */
/*                       up options and file names.                          */
/*                                                                           */
/*  argc:  The number of strings on the command line.                        */
/*  argv:  An array of the strings on the command line.                      */
/*  b:  The struct storing Star's options and file names.                    */
/*                                                                           */
/*****************************************************************************/

void parsecommandline(int argc, char **argv, struct behavior *b)
{
#ifdef STARLIBRARY
#define STARTINDEX 0
#else /* not STARLIBRARY */
#define STARTINDEX 1
  int increment;
  int meshnumber;
#endif /* not STARLIBRARY */
  int i, j, k;
  char workstring[FILENAMESIZE];

  /* See the comments above the definition of `struct behavior' for the */
  /*   meaning of most of these variables.                              */
  b->inspherecount = 0;
  b->orientcount = 0;
  b->orient4dcount = 0;
  b->tetcircumcentercount = 0;
  b->tricircumcentercount = 0;

  b->poly = b->refine = b->quality = 0;
  b->varvolume = b->fixedvolume = b->usertest = 0;
  b->regionattrib = b->convex = b->weighted = b->jettison = 0;
  b->firstnumber = 1;
  b->edgesout = b->facesout = b->voronoi = b->neighbors = b->geomview = 0;
  b->nobound = b->nopolywritten = b->nonodewritten = b->noelewritten = 0;
  b->noiterationnum = 0;
  b->noholes = b->noexact = 0;
  b->docheck = 0;
  b->nobisect = 0;
  b->conformdel = 0;
  /* A negative `steiner' value indicates no constraint on number of */
  /*   Steiner points.                                               */
  b->steiner = -1;
  b->order = 1;
  b->qmeasure = 0.0;
  b->minangle = 0.0;
  /* A negative volume indicates no constraint on tetrahedron volumes. */
  b->maxvolume = -1.0;
  b->quiet = b->verbose = 0;
#ifndef STARLIBRARY
  b->innodefilename[0] = '\0';
#endif /* not STARLIBRARY */

  for (i = STARTINDEX; i < argc; i++) {
#ifndef STARLIBRARY
    if (argv[i][0] == '-') {
#endif /* not STARLIBRARY */
      for (j = STARTINDEX; argv[i][j] != '\0'; j++) {
        if (argv[i][j] == 'p') {
          b->poly = 1;
	}
#ifndef CDT_ONLY
        if (argv[i][j] == 'r') {
          b->refine = 1;
	}
        if (argv[i][j] == 'q') {
          b->quality = 1;
          if (((argv[i][j + 1] >= '0') && (argv[i][j + 1] <= '9')) ||
              (argv[i][j + 1] == '.')) {
            /* Read a numerical bound on the largest permissible */
            /*   circumradius-to-shortest edge ratio.            */
            k = 0;
            while (((argv[i][j + 1] >= '0') && (argv[i][j + 1] <= '9')) ||
                   (argv[i][j + 1] == '.')) {
              j++;
              workstring[k] = argv[i][j];
              k++;
            }
            workstring[k] = '\0';
            b->qmeasure = (starreal) strtod(workstring, (char **) NULL);
          } else {
            /* Largest permissible circumradius-to-shortest edge ratio is 2. */
            b->qmeasure = 2.0;
          }
          if ((argv[i][j + 1] == '/') || (argv[i][j + 1] == ',')) {
            j++;
            if (((argv[i][j + 1] >= '0') && (argv[i][j + 1] <= '9')) ||
                (argv[i][j + 1] == '.')) {
              /* Read a numerical bound on the smallest permissible */
              /*   dihedral angle.                                  */
              k = 0;
              while (((argv[i][j + 1] >= '0') && (argv[i][j + 1] <= '9')) ||
                     (argv[i][j + 1] == '.')) {
                j++;
                workstring[k] = argv[i][j];
                k++;
              }
              workstring[k] = '\0';
              b->minangle = (starreal) strtod(workstring, (char **) NULL);
            } else {
              /* Smallest permissible dihedral angle is 5 degrees. */
              b->minangle = 5.0;
            }
          }
        }
        if (argv[i][j] == 'a') {
          b->quality = 1;
          if (((argv[i][j + 1] >= '0') && (argv[i][j + 1] <= '9')) ||
              (argv[i][j + 1] == '.')) {
            b->fixedvolume = 1;
            /* Read a numerical bound on the largest permissible */
            /*   tetrahedron volume.                             */
            k = 0;
            while (((argv[i][j + 1] >= '0') && (argv[i][j + 1] <= '9')) ||
                   (argv[i][j + 1] == '.')) {
              j++;
              workstring[k] = argv[i][j];
              k++;
            }
            workstring[k] = '\0';
            b->maxvolume = (starreal) strtod(workstring, (char **) NULL);
            if (b->maxvolume <= 0.0) {
              printf("Error:  Maximum volume must be greater than zero.\n");
              starexit(1);
	    }
	  } else {
            b->varvolume = 1;
	  }
	}
        if (argv[i][j] == 'u') {
          b->quality = 1;
          b->usertest = 1;
        }
#endif /* not CDT_ONLY */
        if (argv[i][j] == 'A') {
          b->regionattrib = 1;
        }
        if (argv[i][j] == 'c') {
          b->convex = 1;
        }
        if (argv[i][j] == 'w') {
          b->weighted = 1;
        }
        if (argv[i][j] == 'W') {
          b->weighted = 2;
        }
        if (argv[i][j] == 'j') {
          b->jettison = 1;
        }
        if (argv[i][j] == 'z') {
          b->firstnumber = 0;
        }
        if (argv[i][j] == 'e') {
          b->edgesout = 1;
	}
        if (argv[i][j] == 'f') {
          b->facesout = 1;
	}
        if (argv[i][j] == 'v') {
          b->voronoi = 1;
	}
        if (argv[i][j] == 'n') {
          b->neighbors = 1;
	}
        if (argv[i][j] == 'g') {
          b->geomview = 1;
	}
        if (argv[i][j] == 'B') {
          b->nobound = 1;
	}
        if (argv[i][j] == 'P') {
          b->nopolywritten = 1;
	}
        if (argv[i][j] == 'N') {
          b->nonodewritten = 1;
	}
        if (argv[i][j] == 'E') {
          b->noelewritten = 1;
	}
#ifndef STARLIBRARY
        if (argv[i][j] == 'I') {
          b->noiterationnum = 1;
	}
#endif /* not STARLIBRARY */
        if (argv[i][j] == 'O') {
          b->noholes = 1;
	}
        if (argv[i][j] == 'X') {
          b->noexact = 1;
	}
        if (argv[i][j] == 'o') {
          if (argv[i][j + 1] == '2') {
            j++;
            b->order = 2;
          }
	}
#ifndef CDT_ONLY
        if (argv[i][j] == 'Y') {
          b->nobisect++;
	}
        if (argv[i][j] == 'S') {
          b->steiner = 0;
          /* Read a numerical bound on the maximum number of Steiner points. */
          while ((argv[i][j + 1] >= '0') && (argv[i][j + 1] <= '9')) {
            j++;
            b->steiner = b->steiner * 10l + (starlong) (argv[i][j] - '0');
          }
        }
        if (argv[i][j] == 'D') {
          b->conformdel = 1;
        }
#endif /* not CDT_ONLY */
        if (argv[i][j] == 'k') {
          b->norandom = 1;
        }
        if (argv[i][j] == 'K') {
          b->fullrandom = 1;
        }
        if (argv[i][j] == 'C') {
          b->docheck = 1;
        }
        if (argv[i][j] == 'Q') {
          b->quiet = 1;
        }
        if (argv[i][j] == 'V') {
          b->verbose++;
        }
#ifndef STARLIBRARY
        if ((argv[i][j] == 'h') || (argv[i][j] == 'H') ||
            (argv[i][j] == '?')) {
          stellarinfo();
	}
#endif /* not STARLIBRARY */
      }
#ifndef STARLIBRARY
    } else {
      /* Any command-line parameter not starting with "-" is assumed to be */
      /*   a file name.                                                    */
      strncpy(b->innodefilename, argv[i], FILENAMESIZE - 1);
      b->innodefilename[FILENAMESIZE - 1] = '\0';
    }
#endif /* not STARLIBRARY */
  }

#ifndef STARLIBRARY
  if (b->innodefilename[0] == '\0') {
    /* No file name specified; print a summary of the command line switches. */
    stellarsyntax();
  }

  /* Remove the filename extension (if any) and use it to infer switches */
  /*   that might not have been specified directly.                      */
  if (!strcmp(&b->innodefilename[strlen(b->innodefilename) - 5], ".node")) {
    b->innodefilename[strlen(b->innodefilename) - 5] = '\0';
  }
  if (!strcmp(&b->innodefilename[strlen(b->innodefilename) - 5], ".poly")) {
    b->innodefilename[strlen(b->innodefilename) - 5] = '\0';
    b->poly = 1;
  }
#ifndef CDT_ONLY
  if (!strcmp(&b->innodefilename[strlen(b->innodefilename) - 4], ".ele")) {
    b->innodefilename[strlen(b->innodefilename) - 4] = '\0';
    b->refine = 1;
  }
  if (!strcmp(&b->innodefilename[strlen(b->innodefilename) - 5], ".area")) {
    b->innodefilename[strlen(b->innodefilename) - 5] = '\0';
    b->refine = 1;
    b->quality = 1;
    b->varvolume = 1;
  }
#endif /* not CDT_ONLY */
#endif /* not STARLIBRARY */

  b->addvertices = b->poly || b->quality;
  b->usefacets = b->poly || b->refine || b->quality || b->convex;
  /* Compute the square of the cosine of the dihedral angle bound. */
  b->goodangle = cos(b->minangle * PI / 180.0);
  b->goodangle *= b->goodangle;
  if (b->refine && b->noiterationnum) {
    printf(
      "Error:  You cannot use the -I switch when refining a triangulation.\n");
    starexit(1);
  }
  /* Be careful not to allocate space for element volume constraints that */
  /*   will never be assigned any value (other than the default -1.0).    */
  if (!b->refine && !b->poly) {
    b->varvolume = 0;
  }
  /* Be careful not to add an extra attribute to each element unless the */
  /*   input supports it (PLC in, but not refining a preexisting mesh).  */
  if (b->refine || !b->poly) {
    b->regionattrib = 0;
  }
  /* Regular/weighted triangulations are incompatible with PLCs and meshing. */
  if (b->weighted && (b->poly || b->quality)) {
    b->weighted = 0;
    if (!b->quiet) {
      printf("Warning:  weighted triangulations (-w, -W) are incompatible\n");
      printf("  with PLCs (-p) and meshing (-q, -a, -u).  Weights ignored.\n");
    }
  }
  if (b->jettison && b->noiterationnum && !b->poly && !b->quiet) {
    printf("Warning:  -j and -I switches are largely incompatible.\n");
    printf("  The vertices are renumbered, so you will need the output\n");
    printf("  .node file to determine the new node indices.");
  } else if (b->jettison && b->nonodewritten && !b->quiet) {
    printf("Warning:  -j and -N switches are somewhat incompatible.\n");
    printf("  The vertices are renumbered, so you will need the output\n");
    printf("  .node file to determine the new node indices.");
  }

#ifndef STARLIBRARY
  strcpy(b->inpolyfilename, b->innodefilename);
  strcpy(b->inelefilename, b->innodefilename);
  strcpy(b->areafilename, b->innodefilename);
  /* Check the input filename for an iteration number. */
  increment = 0;
  strcpy(workstring, b->innodefilename);
  /* Find the last period in the filename. */
  j = 1;
  while (workstring[j] != '\0') {
    if ((workstring[j] == '.') && (workstring[j + 1] != '\0')) {
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
        increment = 0;
        meshnumber = 0;
        break;
      }
      j++;
    } while (workstring[j] != '\0');
  }
  if (b->noiterationnum) {
    /* Do not use iteration numbers. */
    strcpy(b->outnodefilename, b->innodefilename);
    strcpy(b->outelefilename, b->innodefilename);
    strcpy(b->edgefilename, b->innodefilename);
    strcpy(b->facefilename, b->innodefilename);
    strcpy(b->vnodefilename, b->innodefilename);
    strcpy(b->vpolyfilename, b->innodefilename);
    strcpy(b->neighborfilename, b->innodefilename);
    strcpy(b->offfilename, b->innodefilename);
    strcat(b->outnodefilename, ".node");
    strcat(b->outelefilename, ".ele");
    strcat(b->edgefilename, ".edge");
    strcat(b->facefilename, ".face");
    strcat(b->vnodefilename, ".v.node");
    strcat(b->vpolyfilename, ".v.poly");
    strcat(b->neighborfilename, ".neigh");
    strcat(b->offfilename, ".off");
  } else if (increment == 0) {
    /* Input iteration number is zero; output iteration number is one. */
    strcpy(b->outnodefilename, b->innodefilename);
    strcpy(b->outpolyfilename, b->innodefilename);
    strcpy(b->outelefilename, b->innodefilename);
    strcpy(b->edgefilename, b->innodefilename);
    strcpy(b->facefilename, b->innodefilename);
    strcpy(b->vnodefilename, b->innodefilename);
    strcpy(b->vpolyfilename, b->innodefilename);
    strcpy(b->neighborfilename, b->innodefilename);
    strcpy(b->offfilename, b->innodefilename);
    strcat(b->outnodefilename, ".1.node");
    strcat(b->outpolyfilename, ".1.poly");
    strcat(b->outelefilename, ".1.ele");
    strcat(b->edgefilename, ".1.edge");
    strcat(b->facefilename, ".1.face");
    strcat(b->vnodefilename, ".1.v.node");
    strcat(b->vpolyfilename, ".1.v.poly");
    strcat(b->neighborfilename, ".1.neigh");
    strcat(b->offfilename, ".1.off");
  } else {
    /* Higher iteration numbers. */
    workstring[increment] = '%';
    workstring[increment + 1] = 'd';
    workstring[increment + 2] = '\0';
    sprintf(b->outnodefilename, workstring, meshnumber + 1);
    strcpy(b->outpolyfilename, b->outnodefilename);
    strcpy(b->outelefilename, b->outnodefilename);
    strcpy(b->edgefilename, b->outnodefilename);
    strcpy(b->facefilename, b->outnodefilename);
    strcpy(b->vnodefilename, b->outnodefilename);
    strcpy(b->vpolyfilename, b->outnodefilename);
    strcpy(b->neighborfilename, b->outnodefilename);
    strcpy(b->offfilename, b->outnodefilename);
    strcat(b->outnodefilename, ".node");
    strcat(b->outpolyfilename, ".poly");
    strcat(b->outelefilename, ".ele");
    strcat(b->edgefilename, ".edge");
    strcat(b->facefilename, ".face");
    strcat(b->vnodefilename, ".v.node");
    strcat(b->vpolyfilename, ".v.poly");
    strcat(b->neighborfilename, ".neigh");
    strcat(b->offfilename, ".off");
  }
  strcat(b->innodefilename, ".node");
  strcat(b->inpolyfilename, ".poly");
  strcat(b->inelefilename, ".ele");
  strcat(b->areafilename, ".area");
#endif /* not STARLIBRARY */
}

/**                                                                         **/
/**                                                                         **/
/********* User interaction routines end here                        *********/


/********* Miscellaneous routines begin here                         *********/
/**                                                                         **/
/**                                                                         **/

/* Random number seed is not constant, but I've made it global anyway.       */
/*   This should not hurt reentrancy (unless you want repeatability).        */

unsigned long randomseed = 1;                 /* Current random number seed. */

/*****************************************************************************/
/*                                                                           */
/*  randomnation()   Generate a random number between 0 and `choices' - 1.   */
/*                                                                           */
/*  This is a simple linear congruential random number generator.  Hence, it */
/*  is a bad random number generator, but good enough for most randomized    */
/*  geometric algorithms.                                                    */
/*                                                                           */
/*****************************************************************************/

unsigned long randomnation(unsigned long choices)
{
  unsigned long newrandom;

  if (choices >= 714025lu) {
    newrandom = (randomseed * 1366lu + 150889lu) % 714025lu;
    randomseed = (newrandom * 1366lu + 150889lu) % 714025lu;
    newrandom = newrandom * (choices / 714025lu) + randomseed;
    if (newrandom >= choices) {
      return newrandom - choices;
    } else {
      return newrandom;
    }
  } else {
    randomseed = (randomseed * 1366lu + 150889lu) % 714025lu;
    return randomseed % choices;
  }
}

/**                                                                         **/
/**                                                                         **/
/********* Miscellaneous routines end here                           *********/


/********* Tiered array memory pool routines begin here              *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  Tiered arrays, which double as memory pools (struct arraypool)           */
/*                                                                           */
/*  Resizable arrays are a lovely data structure, because they allow you to  */
/*  access objects quickly by number, without limiting how many objects you  */
/*  can allocate.  Unfortunately, the usual technique for resizing arrays is */
/*  to copy everything from the old array to the new, then discard the old.  */
/*  It is not usually possible to reuse the old array's space for the new    */
/*  array.  For an application that uses the computer's entire main memory,  */
/*  this is not acceptable.                                                  */
/*                                                                           */
/*  Hence, tiered arrays, which seem to be part of the oral tradition of     */
/*  computer science, but are rarely covered in data structures textbooks.   */
/*  The simplest kind of tiered array, used here, has two tiers.  The upper  */
/*  tier is a single long array of pointers to second-tier blocks.  The      */
/*  second tier is composed of blocks, each containing the same fixed number */
/*  of objects.                                                              */
/*                                                                           */
/*  Each (nonnegative) array index addresses a particular object in a        */
/*  particular second-tier block.  The most significant bits of the index    */
/*  choose a second-tier block by addressing a pointer in the top tier       */
/*  array.  The remaining, less significant bits address an object within    */
/*  the block.  A tiered array is slightly slower than a normal array,       */
/*  because it requires an extra indiretion; but the top tier is small       */
/*  enough to cache well, so this is rarely a big handicap.                  */
/*                                                                           */
/*  Second-tier blocks are allocated one at a time, as necessary.  The       */
/*  top-tier array is resized in the usual manner.  However, the top tier    */
/*  comprises such a small percentage of the tiered array's total memory,    */
/*  it doesn't matter much if the old, discarded upper tier's memory is not  */
/*  reused.                                                                  */
/*                                                                           */
/*  The tiered array implemented here can be used in two different ways:  as */
/*  a normal array, or as a memory allocator (for objects of a single size)  */
/*  that can allocate and free objects, managing the index space itself.     */
/*  It's not usually wise to mix these two modes, though it can be done.     */
/*                                                                           */
/*  For a normal array, use the procedure arraypoolforcelookup(), which      */
/*  makes sure that an object with the given index exists, then returns a    */
/*  pointer to it.  For this behavior, initialize the arraypool by passing   */
/*  an `alloctype' of 0 to arraypoolinit().  Once an object has been         */
/*  initialized, it can be accessed quickly with the arraypoolfastlookup()   */
/*  macro, which is much faster than arraypoolforcelookup().                 */
/*                                                                           */
/*  For a memory allocator, either use arraypoolnewindex() and               */
/*  arraypoolfreeindex(), which allocate and free array indices, or          */
/*  arraypoolnewptr() and arraypoolfreeptr(), which allocate and free        */
/*  pointers.  For the former behavior, pass an `alloctype' of 1 to          */
/*  arraypoolinit().  For the latter behavior, pass an `alloctype' of 2.     */
/*                                                                           */
/*  Public interface:                                                        */
/*  typedef arraypoolulong   Unsigned array index.                           */
/*  struct arraypool   Tiered array (and memory pool) object.                */
/*  void arraypoolinit(pool, objectbytes, log2objectsperblock, alloctype)    */
/*    Initialize an arraypool for allocation of objects.                     */
/*  void arraypoolrestart(pool)   Deallocate all objects (but not to OS).    */
/*  void arraypooldeinit(pool)   Free an arraypool's memory to the OS.       */
/*  void *arraypoolforcelookup(pool, index)   Dereference; create if absent. */
/*  void *arraypoollookup(pool, index)   Dereference; return NULL if absent. */
/*  void *arraypoolfastlookup(pool, index)   Unsafe dereference; fast macro. */
/*  arraypoolulong arraypoolnewindex(pool, newptr)   Allocate object,        */
/*    returning an array index.                                              */
/*  void *arraypoolnewptr(pool)   Allocate object, returning a pointer.      */
/*  arraypoolulong arraypoolallocated(pool)   Return maximum number of       */
/*    objects allocated.                                                     */
/*  arraypoolulong arraypoolbytes(pool)   Return bytes of dynamic memory     */
/*    used by the tiered array.                                              */
/*  With `alloctype' 1 only:                                                 */
/*  void arraypoolfreeindex(pool, dyingindex)   Deallocate index for reuse.  */
/*  With `alloctype' 2 only:                                                 */
/*  void arraypoolfreeptr(pool, dyingobject)   Deallocate pointer for reuse. */
/*                                                                           */
/*  For internal use only:                                                   */
/*  char *arraypoolsecondtier(pool, index)                                   */
/*                                                                           */
/*****************************************************************************/

/*  `arraypoolulong' is the type of (unsigned) int used by the tiered array  */
/*  memory pool.  It determines the number of objects that can be allocated  */
/*  and the size of the array indices that may be used, so a long choice     */
/*  (e.g. size_t, defined in stddef.h) is recommended.  However, choosing a  */
/*  shorter type may make the tiered array occupy less memory in the case    */
/*  where arraypoolinit() is called with alloctype == 1, which forces each   */
/*  object to occupy at least as much space as an `arraypoolulong'.          */

typedef starulong arraypoolulong;


/*  TOPARRAYSTARTSIZE is the initial size of the top tier of the tiered      */
/*  array, namely the array `toparray'.  Its value is not critical, as the   */
/*  `toparray' is enlarged whenever necessary.                               */

#define TOPARRAYSTARTSIZE 128

/*  When the `toparray' runs out of room, it is enlarged, its length         */
/*  multiplied by the following factor.  If you think that the freed space   */
/*  occupied by the old array will not be reused (e.g. it's too small to be  */
/*  re-malloc'd for any other purpose), then a factor of 3 minimizes the     */
/*  average-case memory wasted.  If you think the freed space will be        */
/*  reused, then a factor of (1 + epsilon) minimizes the average-case memory */
/*  wasted, but you should choose at least 2 for speed's sake.               */

#define TOPRESIZEFACTOR 3

/*  NOTAPOOLINDEX is an index denoting no freed object in the arraypool.     */

#define NOTAPOOLINDEX ((arraypoolulong) ~0)


/*  An arraypool is a two-tiered array from which object can be allocated,   */
/*  either by index or by pointer.  `toparray' is a pointer to the upper     */
/*  tier, which is an array of pointers to the blocks that make up the lower */
/*  tier; and `toparraylen' is the upper tier's length.                      */
/*                                                                           */
/*  There are two mutually exclusive ways to store the stack of freed        */
/*  objects that are waiting to be reallocated.  `deadindexstack' and        */
/*  `deadobjectstack' store the indices or pointers, respectively, of the    */
/*  freed objects.  Indices are more flexible, as they can be converted to   */
/*  pointers, whereas pointers cannot be converted to indices.               */
/*                                                                           */
/*  Other fields are explained by comments below.  `objectsperblock' must be */
/*  2 raised to an integral power.  `objects' is a count of objects that the */
/*  arraypool has allocated for the client, NOT a count of objects malloc'd  */
/*  from the operating system.  `objects' does not include objects that have */
/*  been freed and are waiting on the dead stack to be reallocated.          */
/*  `objects' does not include objects that a client has accessed directly   */
/*  by a call to arraypoolforcelookup() without first being allocated by the */
/*  arraypool.                                                               */

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


/*****************************************************************************/
/*                                                                           */
/*  arraypoolrestart()   Deallocate all objects in an arraypool.             */
/*                                                                           */
/*  The arraypool returns to a fresh state, like after it was initialized    */
/*  by poolinit(), except that no memory is freed to the operating system.   */
/*  Rather, the previously allocated blocks are ready to be reused.          */
/*                                                                           */
/*  pool:  The arraypool to restart.                                         */
/*                                                                           */
/*****************************************************************************/

void arraypoolrestart(struct arraypool *pool)
{
  /* No objects have been freed. */
  pool->deadobjectstack = (void *) NULL;
  pool->deadindexstack = NOTAPOOLINDEX;
  /* Every object is ready to allocate. */
  pool->firstvirgin = 0;
  pool->objects = 0;
}

/*****************************************************************************/
/*                                                                           */
/*  arraypoolinit()   Initialize an arraypool for allocation of objects.     */
/*                                                                           */
/*  Before an arraypool may be used, it must be initialized by this          */
/*  procedure.  After initialization, memory can be allocated and freed in   */
/*  an arraypool, and array indices can be "dereferenced", by use of the     */
/*  other procedures.  arraypoolinit() does not allocate any memory from the */
/*  operating system itself.                                                 */
/*                                                                           */
/*  Don't call this procedure on an arraypool that has already been          */
/*  initialized (by this procedure), unless you call arraypooldeinit() on    */
/*  it first.  You will leak memory.  Also see arraypoolrestart().           */
/*                                                                           */
/*  pool:  The arraypool to initialize.                                      */
/*  objectbytes:  The number of bytes in each object.                        */
/*  log2objectsperblock:  The base-2 logarithm of the number of objects in   */
/*    each block of the second tier.  (The latter number must be 2 raised to */
/*    an integral power, and this interface ensures that.)                   */
/*  alloctype:  Sets the method used to maintain freed objects, as follows.  */
/*    0:  The arraypool does not manage freeing of objects at all.  Objects  */
/*        can be allocated, using arraypoolnewindex() or arraypoolnewptr(),  */
/*        or accessed (and allocated) directly by supplying an index to      */
/*        arraypoolforcelookup() (in which case the client must manage its   */
/*        own index space, and `objects' is not a correct count).  The       */
/*        advantage of disallowing freeing is that objects can be as short   */
/*        as one byte.                                                       */
/*    1:  The arraypool manages indices for the client via                   */
/*        arraypoolnewindex() and arraypoolfreeindex().  However, each       */
/*        object is made long enough to accommodate an arraypoolulong, if it */
/*        isn't already.  (To accommodate the stack of dead objects.)        */
/*    2:  The arraypool manages pointers for the client via                  */
/*        arraypoolnewptr() and arraypoolfreeptr().  If indices are not      */
/*        needed, this is convenient because it skips the index-to-pointer   */
/*        "dereferencing" step.  Each object is made long enough to          */
/*        accommodate a pointer.                                             */
/*                                                                           */
/*****************************************************************************/

void arraypoolinit(struct arraypool *pool,
                   arraypoolulong objectbytes,
                   arraypoolulong log2objectsperblock,
                   int alloctype)
{
  if (alloctype == 0) {
    /* Each object must be at least one byte long. */
    pool->objectbytes = objectbytes > 1 ? objectbytes : 1;
  } else if (alloctype == 1) {
    /* Each object must be at least sizeof(arraypoolulong) bytes long. */
    pool->objectbytes = objectbytes > sizeof(arraypoolulong) ?
                        objectbytes : sizeof(arraypoolulong);
  } else {
    /* Each object must be at least as long as a pointer. */
    pool->objectbytes = objectbytes > sizeof(void *) ?
                        objectbytes : sizeof(void *);
  }

  pool->log2objectsperblock = log2objectsperblock;
  /* Compute the number of objects in each block of the second tier. */
  pool->objectsperblock = ((arraypoolulong) 1) << pool->log2objectsperblock;
  /* No memory has been allocated. */
  pool->totalmemory = 0;
  /* The upper tier array has not been allocated yet. */
  pool->toparray = (char **) NULL;
  pool->toparraylen = 0;

  /* Ready all indices to be allocated. */
  arraypoolrestart(pool);
}

/*****************************************************************************/
/*                                                                           */
/*  arraypooldeinit()   Free to the operating system all memory taken by an  */
/*                      arraypool.                                           */
/*                                                                           */
/*  pool:  The arraypool to free.                                            */
/*                                                                           */
/*****************************************************************************/

void arraypooldeinit(struct arraypool *pool)
{
  arraypoolulong i;

  /* Has anything been allocated at all? */
  if (pool->toparray != (char **) NULL) {
    /* Walk through the top tier array. */
    for (i = 0; i < pool->toparraylen; i++) {
      /* Check every top-tier pointer; NULLs may be scattered randomly. */
      if (pool->toparray[i] != (char *) NULL) {
        /* Free a second-tier block. */
        starfree((void *) pool->toparray[i]);
      }
    }
    /* Free the top-tier array. */
    starfree((void *) pool->toparray);
  }

  /* The upper tier array is no longer allocated. */
  pool->toparray = (char **) NULL;
  pool->toparraylen = 0;
  pool->objects = 0;
  pool->totalmemory = 0;
}

/*****************************************************************************/
/*                                                                           */
/*  arraypoolsecondtier()   Return (and perhaps create) the second-tier      */
/*                          block containing the object with a given index.  */
/*                                                                           */
/*  This procedure takes care of allocating or resizing the top-tier array   */
/*  if necessary, and of allocating the second-tier block if it hasn't yet   */
/*  been allocated.                                                          */
/*                                                                           */
/*  pool:  The arraypool containing the object.                              */
/*  index:  The index of the object sought.                                  */
/*                                                                           */
/*  Returns a pointer to the beginning of the second-tier block.  (Not to    */
/*    the object with the specified index; just the block containing it.)    */
/*                                                                           */
/*****************************************************************************/

char *arraypoolsecondtier(struct arraypool *pool,
                          arraypoolulong index)
{
  char **newarray;
  char *secondtier;
  arraypoolulong newsize;
  arraypoolulong topindex;
  arraypoolulong i;

  /* Compute the index in the top-tier array (upper bits). */
  topindex = index >> pool->log2objectsperblock;
  /* Does the top-tier array need to be allocated or resized? */
  if (pool->toparray == (char **) NULL) {
    /* Allocate a top-tier array big enough to hold `topindex' (plus */
    /*   some slack), and NULL out its contents.                     */
    newsize = topindex + TOPARRAYSTARTSIZE;
    pool->toparray = (char **) starmalloc((size_t) (newsize * sizeof(char *)));
    pool->toparraylen = newsize;
    for (i = 0; i < newsize; i++) {
      pool->toparray[i] = (char *) NULL;
    }

    /* Account for the memory. */
    pool->totalmemory = newsize * (arraypoolulong) sizeof(char *);
  } else if (topindex >= pool->toparraylen) {
    /* Resize the top-tier array, making sure it holds `topindex'. */
    newsize = TOPRESIZEFACTOR * pool->toparraylen;
    if (topindex >= newsize) {
      newsize = topindex + TOPARRAYSTARTSIZE;
    }

    /* Allocate the new array, copy the contents, NULL out the rest, and */
    /*   free the old array.                                             */
    newarray = (char **) starmalloc((size_t) (newsize * sizeof(char *)));
    for (i = 0; i < pool->toparraylen; i++) {
      newarray[i] = pool->toparray[i];
    }
    for (i = pool->toparraylen; i < newsize; i++) {
      newarray[i] = (char *) NULL;
    }
    starfree(pool->toparray);

    /* Account for the memory. */
    pool->totalmemory += (newsize - pool->toparraylen) * sizeof(char *);

    pool->toparray = newarray;
    pool->toparraylen = newsize;
  }

  /* Find the second-tier block, or learn that it hasn't been allocated yet. */
  secondtier = pool->toparray[topindex];
  if (secondtier == (char *) NULL) {
    /* Allocate a second-tier block at this index. */
    secondtier = (char *) starmalloc((size_t) (pool->objectsperblock *
                                               pool->objectbytes));
    pool->toparray[topindex] = secondtier;
    /* Account for the memory. */
    pool->totalmemory += pool->objectsperblock * pool->objectbytes;
  }

  /* Return a pointer to the second-tier block. */
  return secondtier;
}

/*****************************************************************************/
/*                                                                           */
/*  arraypoolforcelookup()   A "dereferencing" operation:  return the        */
/*                           pointer to the object with a given index,       */
/*                           creating the object if it doesn't yet exist.    */
/*                                                                           */
/*  If memory does not yet exist for the object with the given index, this   */
/*  procedure allocates a second-tier block with room for the object.  (The  */
/*  new object's memory is not initialized in any way.)  Subsequent calls    */
/*  with the same index will always return the same object.                  */
/*                                                                           */
/*  The object with the given index is NOT allocated to the client by the    */
/*  arraypool, and the object may be subsequently returned by a call to      */
/*  arraypoolnewindex() or arraypoolnewptr(), perhaps causing an object to   */
/*  have two conflicting uses.  Generally, those procedures do not mix       */
/*  easily with this one, but they can be mixed with enough care.            */
/*                                                                           */
/*  pool:  The arraypool containing the object.                              */
/*  index:  The index of the object sought.                                  */
/*                                                                           */
/*  Returns a pointer to the object.                                         */
/*                                                                           */
/*****************************************************************************/

void *arraypoolforcelookup(struct arraypool *pool,
                           arraypoolulong index)
{
  /* Find the second-tier block and compute a pointer to the object with the */
  /*   given index.  Note that `objectsperblock' is a power of two, so the   */
  /*   & operation is a bit mask that preserves the lower bits.              */
  return (void *) (arraypoolsecondtier(pool, index) +
                   (index & (pool->objectsperblock - 1)) * pool->objectbytes);
}

/*****************************************************************************/
/*                                                                           */
/*  arraypoollookup()   A "dereferencing" operation:  return the pointer to  */
/*                      the object with a given index, or NULL if the        */
/*                      object's second-tier block doesn't yet exist.        */
/*                                                                           */
/*  Note that this procedure cannot tell whether an object with the given    */
/*  index has been allocated by the arraypool or initialized by the client   */
/*  yet; it can only tell whether memory has been allocated for the second-  */
/*  tier block that contains the object.                                     */
/*                                                                           */
/*  pool:  The arraypool containing the object.                              */
/*  index:  The index of the object sought.                                  */
/*                                                                           */
/*  Returns a pointer to the object if its block exists; NULL otherwise.     */
/*                                                                           */
/*****************************************************************************/

void *arraypoollookup(struct arraypool *pool,
                      arraypoolulong index)
{
  char *secondtier;
  arraypoolulong topindex;

  /* Has the top-tier array been allocated yet? */
  if (pool->toparray == (char **) NULL) {
    return (void *) NULL;
  }

  /* Compute the index in the top-tier array (upper bits). */
  topindex = index >> pool->log2objectsperblock;
  /* Does the top-tier index fit in the top-tier array? */
  if (topindex >= pool->toparraylen) {
    return (void *) NULL;
  }

  /* Find the second-tier block, or learn that it hasn't been allocated yet. */
  secondtier = pool->toparray[topindex];
  if (secondtier == (char *) NULL) {
    return (void *) NULL;
  }

  /* Compute a pointer to the object with the given index.  Note that        */
  /*   `objectsperblock' is a power of two, so the & operation is a bit mask */
  /*   that preserves the lower bits.                                        */
  return (void *) (secondtier +
                   (index & (pool->objectsperblock - 1)) * pool->objectbytes);
}

/*****************************************************************************/
/*                                                                           */
/*  arraypoolfastlookup()   A fast, unsafe "dereferencing" operation:        */
/*                          return the pointer to the object with a given    */
/*                          index.  The object's second-tier block must have */
/*                          been allocated.                                  */
/*                                                                           */
/*  Use this procedure only for indices that have previously been allocated  */
/*  with arraypoolnewindex() or "dereferenced" with arraypoolforcelookup().  */
/*  WARNING:  fails catastrophically, probably with a segmentation fault, if */
/*  the index's second-tier block doesn't yet exist.                         */
/*                                                                           */
/*  Implemented as a macro to meet your need for speed.                      */
/*                                                                           */
/*  pool:  The arraypool containing the object.                              */
/*         Type (struct arraypool *).                                        */
/*  index:  The index of the object sought.                                  */
/*          Type (arraypoolulong).                                           */
/*                                                                           */
/*  Returns a pointer to the object if its block exists; might crash         */
/*    otherwise.                                                             */
/*    Type (void *).                                                         */
/*                                                                           */
/*****************************************************************************/

/* Dereference the top tier `toparray' with the upper bits to find the  */
/*   second-tier bock, then add an offset computed from the lower bits. */

#define arraypoolfastlookup(pool, index)  \
  (void *) ((pool)->toparray[(index) >> (pool)->log2objectsperblock] +  \
            ((index) & ((pool)->objectsperblock - 1)) * (pool)->objectbytes)

/*****************************************************************************/
/*                                                                           */
/*  arraypoolnewptr()   Allocate space for a fresh object from an arraypool. */
/*                                                                           */
/*  Returns an object that was not already allocated, or has been freed      */
/*  since it was last allocated.  Be forewarned that this procedure is not   */
/*  aware of objects that have been accessed by arraypoolforcelookup(), and  */
/*  may allocate them, possibly resulting in conflicting uses of an object.  */
/*                                                                           */
/*  pool:  The arraypool to allocate an object from.                         */
/*                                                                           */
/*  Returns a pointer to a fresh object.                                     */
/*                                                                           */
/*****************************************************************************/

void *arraypoolnewptr(struct arraypool *pool)
{
  void *newobject;

  /* Check if any freed objects have not yet been reallocated. */
  if (pool->deadobjectstack != (void *) NULL) {
    /* Reallocate an object from the stack of dead objects. */
    newobject = pool->deadobjectstack;
    /* Remove the object from the stack. */
    pool->deadobjectstack = * (void **) newobject;
  } else {
    /* Allocate an object at index `firstvirgin'. */
    newobject = (void *) (arraypoolsecondtier(pool, pool->firstvirgin) +
                          (pool->firstvirgin & (pool->objectsperblock - 1)) *
                          pool->objectbytes);
    pool->firstvirgin++;
  }
  pool->objects++;

  return newobject;
}

/*****************************************************************************/
/*                                                                           */
/*  arraypoolfreeptr()   Deallocate an object, freeing its space for reuse.  */
/*                                                                           */
/*  The object's memory is not freed to the operating system; rather, the    */
/*  object is stored in a stack so it can be reallocated later by            */
/*  arraypoolnewptr().  (It cannot be reallocated by arraypoolnewindex(),    */
/*  though.)                                                                 */
/*                                                                           */
/*  WARNING:  Use this procedure only if the arraypool was initialized with  */
/*  alloctype == 2!  Not compatible with an alloctype of 0 or 1.             */
/*                                                                           */
/*  pool:  The arraypool the object was allocated from.                      */
/*  dyingobject:  A pointer to the object to deallocate.  Must be in `pool'! */
/*                                                                           */
/*****************************************************************************/

void arraypoolfreeptr(struct arraypool *pool,
                      void *dyingobject)
{
  /* Store the object on the stack of dead objects for later reuse. */
  * (void **) dyingobject = pool->deadobjectstack;
  pool->deadobjectstack = dyingobject;
  pool->objects--;
}

/*****************************************************************************/
/*                                                                           */
/*  arraypoolnewindex()   Allocate space for a fresh object from an          */
/*                        arraypool.                                         */
/*                                                                           */
/*  Returns an object that was not already allocated, or has been freed      */
/*  since it was last allocated.  Be forewarned that this procedure is not   */
/*  aware of objects that have been accessed by arraypoolforcelookup(), and  */
/*  may allocate them, possibly resulting in conflicting uses of an object.  */
/*                                                                           */
/*  From a fresh pool, indices are allocated starting from zero.             */
/*                                                                           */
/*  pool:  The arraypool to allocate an object from.                         */
/*  newptr:  If this pointer is not NULL, a pointer to the new object is     */
/*    written at the location that `newptr' points to.                       */
/*                                                                           */
/*  Returns the index of a fresh object.                                     */
/*                                                                           */
/*****************************************************************************/

arraypoolulong arraypoolnewindex(struct arraypool *pool,
                                 void **newptr)
{
  void *newobject;
  arraypoolulong newindex;

  /* Check if any freed objects have not yet been reallocated. */
  if (pool->deadindexstack != NOTAPOOLINDEX) {
    /* Reallocate an object (by index) from the stack of dead objects. */
    newindex = pool->deadindexstack;
    /* Find a pointer to the object. */
    newobject = arraypoollookup(pool, newindex);
    /* Memory should already exist for this index. */
    if (newobject == (void *) NULL) {
      printf("Internal error in arraypoolnewindex():\n");
      printf("  Illegal index on stack of deallocated objects.\n");
      internalerror();
    }
    /* Remove the object from the stack. */
    pool->deadindexstack = * (arraypoolulong *) newobject;
  } else {
    /* Allocate an object at index `firstvirgin'. */
    newindex = pool->firstvirgin;
    newobject = (void *) (arraypoolsecondtier(pool, pool->firstvirgin) +
                          (pool->firstvirgin & (pool->objectsperblock - 1)) *
                          pool->objectbytes);
    pool->firstvirgin++;
  }
  pool->objects++;

  /* If `newptr' is not NULL, use it to return a pointer to the object. */
  if (newptr != (void **) NULL) {
    *newptr = newobject;
  }
  return newindex;
}

/*****************************************************************************/
/*                                                                           */
/*  arraypoolfreeindex()   Deallocate an object, freeing its index and space */
/*                         for reuse.                                        */
/*                                                                           */
/*  The object's memory is not freed to the operating system; rather, the    */
/*  object is stored in a stack so it can be reallocated later by            */
/*  arraypoolnewindex().  (It cannot be reallocated by arraypoolnewptr(),    */
/*  though.)                                                                 */
/*                                                                           */
/*  WARNING:  Use this procedure only if the arraypool was initialized with  */
/*  alloctype == 1!  Not compatible with an alloctype of 0 or 2.             */
/*                                                                           */
/*  pool:  The arraypool the object was allocated from.                      */
/*  dyingindex:  The index of the object to deallocate.                      */
/*                                                                           */
/*****************************************************************************/

void arraypoolfreeindex(struct arraypool *pool,
                        arraypoolulong dyingindex)
{
  void *dyingptr;

  /* Find a pointer to the object. */
  dyingptr = arraypoollookup(pool, dyingindex);
  if (dyingptr == (void *) NULL) {
    printf("Internal error in arraypoolfreeindex():\n");
    printf("  Attempt to free an unallocated index.\n");
    internalerror();
  }

  /* Store the index on the stack of dead objects for later reuse. */
  * (arraypoolulong *) dyingptr = pool->deadindexstack;
  pool->deadindexstack = dyingindex;
  pool->objects--;
}

/*****************************************************************************/
/*                                                                           */
/*  arraypoolallocated()   Return the maximum number of allocated objects.   */
/*                                                                           */
/*  Returns the greatest number of objects that have been allocated from the */
/*  arraypool at once.  This number is one greater than the greatest index   */
/*  of any object ever allocated from the arraypool.  Therefore, it can be   */
/*  used to loop through all the objects in the pool--if no object has been  */
/*  freed, or you are clever enough to be able to distinguish between freed  */
/*  items and allocated ones.                                                */
/*                                                                           */
/*  pool:  The arraypool in question.                                        */
/*                                                                           */
/*  Returns the maximum number of objects currently allocated in `pool'.     */
/*                                                                           */
/*****************************************************************************/

arraypoolulong arraypoolallocated(struct arraypool *pool)
{
  return pool->firstvirgin;
}

/*****************************************************************************/
/*                                                                           */
/*  arraypoolbytes()   Returns the number of bytes of dynamic memory used by */
/*                     the tiered array memory pool.                         */
/*                                                                           */
/*  The result sums all the bytes dynamically allocated on the heap for the  */
/*  allocpool, including the top tier array and the second-tier blocks.      */
/*  Does not include the size of the `struct arraypool', which is presumably */
/*  part of some other object and accounted for there.  Note that arraypools */
/*  only release memory to the operating system when arraypooldeinit() is    */
/*  called, so the result is the _maximum_ number of bytes allocated since   */
/*  the initiating call to arraypoolinit().                                  */
/*                                                                           */
/*  pool:  The arraypool in question.                                        */
/*                                                                           */
/*  Returns the number of dynamically allocated bytes in `pool'.             */
/*                                                                           */
/*****************************************************************************/

arraypoolulong arraypoolbytes(struct arraypool *pool)
{
  return pool->totalmemory;
}

/**                                                                         **/
/**                                                                         **/
/********* Tiered array memory pool routines end here                *********/


/******** Proximity-based memory pool management routines begin here  ********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  Proximity-based memory pools (struct proxipool)                          */
/*                                                                           */
/*  A proximity-based memory pool is a memory allocator (for objects of a    */
/*  single size) which divides memory into separate pools, each with a       */
/*  different "allocation index."  The idea is that objects that are         */
/*  geometrically close to each other are likely to have the same allocation */
/*  index, and therefore be close to each other--both close in memory, and   */
/*  close in the space of "tags."  Each object is assigned a distance tag,   */
/*  which can be used like an array index to address the object.  The reason */
/*  I want proximate objects to have proximate tags is so that tags can be   */
/*  compressed well in the triangulation data structures.                    */
/*                                                                           */
/*  A proxipool is a tiered array.  (You should read the documentation for   */
/*  tiered arrays, "struct arraypool", before reading on.)  There are some   */
/*  important differences, of course.  Each second-tier block in a proxipool */
/*  is called a "minipool," and is assigned to a single allocation index.    */
/*  All the objects in a second-tier block have the same allocation index;   */
/*  this is the mechanism for maintaining memory proximity and tag           */
/*  proximity.  Each allocation index has a set of minipools associated with */
/*  it; this set is called a "poolpool" because it is a pool of minipools.   */
/*                                                                           */
/*  To further improve tag proximity, tags are allocated to allocation       */
/*  indices in "groups" of minipools.  For example, when the very first      */
/*  object is allocated, receiving tag 0, the proxipool will allocate a      */
/*  minipool for that object's allocation index.  Moreover, the next seven   */
/*  or so minipools' worth of tags will also be reserved for the allocation  */
/*  index--though the seven minipools are not allocated until (and unless)   */
/*  they are needed.                                                         */
/*                                                                           */
/*  Objects in a proxipool can be freed for reuse.  Each minipool maintains  */
/*  its own linked list of freed objects in the minipool (as opposed to      */
/*  having one global linked list for each allocation index).  This helps to */
/*  keep memory accesses more local.                                         */
/*                                                                           */
/*  When an object that resides in a full minipool is freed, its memory is   */
/*  available again for new objects with the same allocation index.  For     */
/*  each allocation index, a poolpool record maintains a linked list of all  */
/*  the minipools that belong to that allocation index and have free space   */
/*  available to allocate.  When an object is freed in a full minipool, the  */
/*  minipool adds itself to the end of the poolpool's linked list.           */
/*                                                                           */
/*  The poolpool records are stored in an arraypool (tiered array).          */
/*                                                                           */
/*  A new object is allocated as follows.  Find the poolpool associated with */
/*  the object's allocation index.  If the poolpool's linked list of         */
/*  minipools is not empty, allocate the object in the first minipool on the */
/*  list.  If the minipool is now full, remove it from the linked list.      */
/*  On the other hand, if the linked list was empty, allocate a new          */
/*  minipool--from the same group as the last allocated minipool if that     */
/*  group is not exhausted; otherwise, from a newly allocated group.         */
/*                                                                           */
/*  A proxipool is also built on an arraypool, in the sense that the objects */
/*  allocated from a proxipool live in the memory of an arraypool.  However, */
/*  the proxipool manages each second-tier block (minipool) itself, so it    */
/*  creates an arraypool having only one "object" per second-tier block,     */
/*  that "object" being a minipool.  Each minipool has a header at the       */
/*  beginning of the second-tier block, followed by a sequence of objects,   */
/*  which are managed by the minipool.                                       */
/*                                                                           */
/*  Some invariants of proxipools:                                           */
/*                                                                           */
/*  - Each object is allocated from a minipool that has the same allocation  */
/*    index as the object.                                                   */
/*                                                                           */
/*  - Every minipool with an unallocated slot appear in its poolpool's       */
/*    linked list.                                                           */
/*                                                                           */
/*  - No full minipool appears in a poolpool's linked list.                  */
/*                                                                           */
/*  - A new minipool is allocated only when a new object's allocation index  */
/*    indexes a poolpool whose linked list of minipools is empty.            */
/*                                                                           */
/*  - Minipools whose tags fall into the same "group" have the same          */
/*    allocation index.                                                      */
/*                                                                           */
/*  - A poolpool reserves only one group at a time, and does not reserve     */
/*    another group until it has filled all the minipools in the first       */
/*    group.                                                                 */
/*                                                                           */
/*  Public interface:                                                        */
/*  typedef proxipoolulong   Unsigned allocation index.                      */
/*  typedef tag   Unsigned tag for indexing objects in the memory pool.      */
/*  struct proxipool   Proximity-based memory pool object.                   */
/*  void proxipoolinit(pool, objectbytes1, objectbytes2, verbose)            */
/*    Initialize a proxipool for allocation of objects.                      */
/*  void proxipoolrestart(pool)   Deallocate all the objects (not to OS).    */
/*  void proxipooldeinit(pool)   Free a proxipool's memory to the OS.        */
/*  void *proxipooltag2object(pool, searchtag)   Unsafe dereference; fast    */
/*    macro.                                                                 */
/*  void *proxipooltag2object2(pool, searchtag)   Unsafe dereference to      */
/*    supplementary object; fast macro.                                      */
/*  proxipoolulong proxipooltag2allocindex(pool, searchtag)   Unsafe lookup  */
/*    of the allocation index that a tag is associated with; fast macro.     */
/*  tag proxipooliterate(pool, thistag)   Iterate over the allocated tags.   */
/*  tag proxipoolnew(pool, allocindex, outobject)   Allocate an object.      */
/*  void proxipoolfree(pool, killtag)   Deallocate tag for reuse.            */
/*  arraypoolulong proxipoolobjects(pool)   Return # of objects in pool.     */
/*  arraypoolulong proxipoolbytes(pool)   Return bytes of dynamic memory     */
/*    used by the proxipool.                                                 */
/*                                                                           */
/*  For internal use only:                                                   */
/*  NOTATAG                                                                  */
/*  NOTAMINIINDEX                                                            */
/*  OBJECTSPERMINI                                                           */
/*  LOG2OBJECTSPERMINI                                                       */
/*  MINISPERGROUP                                                            */
/*  LOG2POOLPOOLSPERBLOCK                                                    */
/*  typedef miniindex                                                        */
/*  struct minipoolheader                                                    */
/*  struct poolpool                                                          */
/*  void proxipoolrestartmini(pool, mini)                                    */
/*  struct minipoolheader *proxipooltag2mini(pool, searchtag)                */
/*  void proxipoolinitpoolpools(pool, endindex)                              */
/*                                                                           */
/*****************************************************************************/


/*  `proxipoolulong' is the type of (unsigned) integer used for allocation   */
/*  indices, here and in the allocation map (allocmap).  Its length can be   */
/*  changed.  A shorter choice consumes less memory; a longer choice         */
/*  increases the number of allocation indices that can exist.               */
/*                                                                           */
/*  In many cases, it might be possible to use a 32-bit proxipoolulong, even */
/*  if 64 bits are needed to index the vertices and other objects.  However, */
/*  proxipoolulongs take up such a tiny proportion of memory, it's probably  */
/*  not worth the risk.  On the other hand, it doesn't make sense to make    */
/*  proxipoolulongs longer than arraypoolulongs, because the allocation      */
/*  indices index objects in an arraypool.                                   */
/*                                                                           */
/*  The `tag' type is used specifically to denote tags, which are internal   */
/*  indices chosen to compress well.  A `miniindex' type indexes an object   */
/*  within a single minipool, and is kept as short as possible to minimize   */
/*  space in minipoolheaders; but a `miniindex' must be at least             */
/*  LOG2OBJECTSPERMINI + 1 bits long.                                        */

typedef arraypoolulong proxipoolulong;
typedef arraypoolulong tag;
typedef unsigned short miniindex;


/*  NOTATAG is a tag denoting no object in a proxipool.                      */

#define NOTATAG ((tag) ~0)

/*  NOTAMINIINDEX is an index denoting no freed object within a minipool.    */

#define NOTAMINIINDEX ((miniindex) ~0)


/*  The number of objects in a minipool.  (A minipool is the smallest unit   */
/*  of objects allocated at one time.)  Must be a power of 2.                */

#define OBJECTSPERMINI 1024

/*  The following MUST be the base-2 logarithm of the above.  If you change  */
/*  one, change the other to match.  Also, make sure that a `miniindex' can  */
/*  hold this many bits plus one.                                            */

#define LOG2OBJECTSPERMINI 10

/*  The number of minipools that are clustered together in a group, all      */
/*  associated with the same allocation index.  Must be a power of 2.        */

#define MINISPERGROUP 8

/*  LOG2POOLPOOLSPERBLOCK is the base-2 logarithm of the number of poolpools */
/*  allocated at a time.                                                     */

#define LOG2POOLPOOLSPERBLOCK 8


/*  A minipool is a block of OBJECTSPERMINI objects.  It consists of one     */
/*  minipoolheader, then OBJECTSPERMINI contiguous objects, then optionally  */
/*  OBJECTSPERMINI contiguous "supplementary" objects that are paired with   */
/*  the first OBJECTSPERMINI objects.  Typically, the supplementary objects  */
/*  contain information that is rarely accessed, and so ought to go into     */
/*  separate pages of virtual memory.                                        */
/*                                                                           */
/*  The minipoolheader contains information about the minipool.  To keep     */
/*  track of objects that are free to be allocated, `firstvirgin' indicates  */
/*  the index of the first object in the minipool that has never been        */
/*  allocated (implying that the objects that follow it are virgins too),    */
/*  and `freestack' is the head of a linked stack of objects in the minipool */
/*  that have been freed.  `object1block' and `object2block' indicate where  */
/*  the two blocks of OBJECTSPERMINI objects start.                          */
/*                                                                           */
/*  One goal is that objects that are spatially close should be close in     */
/*  memory too.  Therefore, each minipool has an allocation index,           */
/*  `allocindex'.  Typically, geometric objects like vertices are assigned   */
/*  allocation indices based on their positions.  For each alloction index,  */
/*  there is a structure of type `struc poolpool' (stored in an array of     */
/*  poolpools) that maintains a linked list of all the minipools associated  */
/*  with that allocation index.  `nextminifree' is the next minipool in the  */
/*  linked list of minipools that have free space.  (There is one such       */
/*  linked list for each allocation index.)                                  */

struct minipoolheader {
  char *object1block;    /* Pointer to the block of objects in the minipool. */
  char *object2block;          /* Optional pointer to supplementary objects. */
  proxipoolulong allocindex;                       /* Index of the poolpool. */
  tag nextminifree;     /* Next in linked list of minipools with free space. */
  miniindex firstvirgin;        /* First never-allocated object in minipool. */
  miniindex freestack; /* Head of linked stack of freed objects in minipool. */
};

/*  A poolpool is a pool of minipools associated with one allocation index.  */
/*  It references a list of minipools that have free space, through          */
/*  `freelisthead' and `freelisttail'.  It also references a group in which  */
/*  further minipools can be allocated.  `mygroup' specifies a group by      */
/*  tagging the next minipool that will be allocated in the group.           */

struct poolpool {
  tag freelisthead;            /* Head of list of minipools with free space. */
  tag freelisttail;            /* Tail of list of minipools with free space. */
  tag mygroup;                 /* Next minipool to be allocated in my group. */
};

/*  A proxipool is a pool from which objects can be allocated.  With the     */
/*  help of an allocation tree, it allocates objects so that they are close  */
/*  in memory to objects they are spatially close to.  `objectbytes1' is the */
/*  length of each object in bytes, and `objectbytes2' is the length of the  */
/*  supplementary information (if any) associated with each object.          */
/*  `block1offset' is the offset (in bytes) at which the first object        */
/*  appears after the beginning of each minipoolheader, and `block2offset'   */
/*  is the offset at which the first supplementary object appears.           */
/*  `minipoolsize' is the size (in bytes) of each minipool, including one    */
/*  minipoolheader, OBJECTSPERMINI objects, and perhaps OBJECTSPERMINI       */
/*  supplementary objects.                                                   */
/*                                                                           */
/*  `minipoolarray' is a tiered array of minipools.  `objects' is the number */
/*  of objects currently allocated from the proxipool, and `maxobjects' is   */
/*  the largest number that have been allocated at any one time.             */
/*  `nextgroup' is the first tag of the first minipool of the group of       */
/*  minipools that will be allocated next.                                   */
/*                                                                           */
/*  For each allocation index, each proxipool has a poolpool (pool of        */
/*  minipools) from which to allocate objects.  `poolpools' is an arraypool  */
/*  used to map allocation indices to poolpools.  `nextinitindex' is the     */
/*  index of the first poolpool that has not yet been initialized.           */
/*                                                                           */
/*  The number `verbosity' indicates how much debugging information to       */
/*  print, from none (0) to lots (4+).                                       */

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


/*****************************************************************************/
/*                                                                           */
/*  proxipoolrestartmini()   Reset a minipool to a pristine state.           */
/*                                                                           */
/*  The minipool is set so that it contains no objects, and all its space    */
/*  is available for allocating.                                             */
/*                                                                           */
/*  pool:  The proxipool containing the minipool.                            */
/*  mini:  The minipool to restart.                                          */
/*                                                                           */
/*****************************************************************************/

void proxipoolrestartmini(struct proxipool *pool,
                          struct minipoolheader *mini)
{
  /* Give the minipool pointers to its objects. */
  mini->object1block = ((char *) mini) + pool->block1offset;
  mini->object2block = pool->objectbytes2 > 0 ?
    ((char *) mini) + pool->block2offset : (char *) NULL;

  /* The minipool is not on any poolpool's list. */
  mini->nextminifree = NOTATAG;
  /* The minipool is associated with the default poolpool (index zero). */
  mini->allocindex = 0;
  /* The pool contains no objects. */
  mini->firstvirgin = 0;
  mini->freestack = NOTAMINIINDEX;
}

/*****************************************************************************/
/*                                                                           */
/*  proxipoolrestart()   Deallocate all objects in a proxipool.              */
/*                                                                           */
/*  The pool is returned to its starting state, except that no memory is     */
/*  freed to the operating system.  Rather, the previously allocated         */
/*  minipools are ready to be reused.                                        */
/*                                                                           */
/*  pool:  The proxipool to restart.                                         */
/*                                                                           */
/*****************************************************************************/

void proxipoolrestart(struct proxipool *pool)
{
  struct minipoolheader *mini;
  arraypoolulong maxindex;
  arraypoolulong i;

  /* Restart all the allocated minipools. */
  maxindex = (arraypoolulong) (pool->nextgroup >> LOG2OBJECTSPERMINI);
  for (i = 0; i < maxindex; i++) {
    mini = (struct minipoolheader *) arraypoollookup(&pool->minipoolarray, i);
    if (mini != (struct minipoolheader *) NULL) {
      proxipoolrestartmini(pool, mini);
    }
  }

  /* Free all the poolpools for reuse. */
  arraypoolrestart(&pool->poolpools);

  /* No objects have been allocated. */
  pool->objects = 0;
  /* The next group of minipools that will be allocated starts with tag */
  /*   zero.                                                            */
  pool->nextgroup = 0;
  /* No poolpool has been initialized. */
  pool->nextinitindex = 0;
}

/*****************************************************************************/
/*                                                                           */
/*  proxipoolinit()   Initialize a pool of memory for allocation of objects. */
/*                                                                           */
/*  Before a proxipool may be used, it must be initialized by this           */
/*  procedure.  After initialization, tags (with associated memory) can be   */
/*  allocated and freed in a proxipool.  proxipoolinit() does not allocate   */
/*  any memory from the operating system itself.                             */
/*                                                                           */
/*  The `pool' allocates space for objects of size `objectbytes1'.  Each     */
/*  object may also have a supplementary object with size `objectbytes2'.    */
/*  The supplementary objects are used to store information that is used     */
/*  infrequently and thus is best stored in different memory pages than the  */
/*  primary objects.                                                         */
/*                                                                           */
/*  Don't call this procedure on a proxipool that has already been           */
/*  initialized (by this procedure), unless you call proxipooldeinit() on    */
/*  it first.  You will leak memory.  Also see proxipoolrestart().           */
/*                                                                           */
/*  pool:  The proxipool to initialize.                                      */
/*  objectbytes1:  The size, in bytes, of the objects.                       */
/*  objectbytes2:  Size of the supplementary objects.  If zero, no space is  */
/*    allocated for these.                                                   */
/*  verbose:  How much debugging information proxipool procedures should     */
/*    print, from none (0) to lots (4+).                                     */
/*                                                                           */
/*****************************************************************************/

void proxipoolinit(struct proxipool *pool,
                   size_t objectbytes1,
                   size_t objectbytes2,
                   int verbose)
{
  size_t maxword;

  /* Find the size of the largest data object:  pointer, real, or long. */
  maxword = sizeof(arraypoolulong) > sizeof(void *) ?
            sizeof(arraypoolulong) : sizeof(void *);
  maxword = sizeof(starreal) > maxword ? sizeof(starreal) : maxword;

  /* The number of bytes occupied by an object. */
  pool->objectbytes1 = objectbytes1 > sizeof(miniindex) ?
                       objectbytes1 : sizeof(miniindex);
  /* The number of bytes occupied by a subsidiary object. */
  pool->objectbytes2 = objectbytes2 > 0 ? objectbytes2 : 0;

  /* The objects in a minipool begin `block1offset' bytes after the    */
  /*   minipoolheader.  It's rounded up to be a multiple of `maxword'. */
  pool->block1offset = ((sizeof(struct minipoolheader) - 1) / maxword + 1) *
                       maxword;
  /* The subsidiary objects in a minipool begin `block2offset' bytes after */
  /*   the minipoolheader.  It's rounded up to be a multiple of `maxword'. */
  pool->block2offset = ((pool->block1offset +
                         OBJECTSPERMINI * pool->objectbytes1 - 1) / maxword +
                        1) * maxword;
  /* `minipoolsize' is the total number of bytes in one minipool. */
  pool->minipoolsize = ((pool->block2offset +
                         OBJECTSPERMINI * pool->objectbytes2 - 1) / maxword +
                        1) * maxword;

  pool->verbosity = verbose;
  /* No objects in the pool yet. */
  pool->maxobjects = 0;

  /* Create a teired array of minipools.  Note that there is only one        */
  /*   minipool per block of the tiered array.  This is because I don't want */
  /*   to allocate memory for minipools that aren't being used, and because  */
  /*   the proxipool manages multiple objects within each minipool           */
  /*   differently than the arraypool manages objects within a block.        */
  arraypoolinit(&pool->minipoolarray, (arraypoolulong) pool->minipoolsize,
                (arraypoolulong) 0, 0);

  /* Create an array of poolpools. */
  arraypoolinit(&pool->poolpools, (arraypoolulong) sizeof(struct poolpool),
                (arraypoolulong) LOG2POOLPOOLSPERBLOCK, 0);

  proxipoolrestart(pool);
}

/*****************************************************************************/
/*                                                                           */
/*  proxipooldeinit()   Free to the operating system all memory taken by a   */
/*                      proxipool.                                           */
/*                                                                           */
/*  pool:  The proxipool to free.                                            */
/*                                                                           */
/*****************************************************************************/

void proxipooldeinit(struct proxipool *pool)
{
  /* Free the array of minipools. */
  arraypooldeinit(&pool->minipoolarray);
  /* Free the array of poolpools. */
  arraypooldeinit(&pool->poolpools);
}

/*****************************************************************************/
/*                                                                           */
/*  proxipooltag2mini()   Map a tag to the minipool it indexes.              */
/*                                                                           */
/*  WARNING:  fails catastrophically, probably with a segmentation fault, if */
/*  the tag's minipool doesn't yet exist.                                    */
/*                                                                           */
/*  Implemented as a macro to meet your need for speed.                      */
/*                                                                           */
/*  pool:  The proxipool in which to find a minipool.                        */
/*         Type (struct proxipool *).                                        */
/*  searchtag:  The tag whose minipool is sought.                            */
/*              Type (tag).                                                  */
/*                                                                           */
/*  Returns the minipool that contains the object with the given tag.  Might */
/*    crash if no such minipool has been allocated yet.                      */
/*    Type (struct minipoolheader *).                                        */
/*                                                                           */
/*****************************************************************************/

#define proxipooltag2mini(pool, searchtag)  \
  ((struct minipoolheader *)  \
   arraypoolfastlookup(&(pool)->minipoolarray,  \
                       (searchtag) >> LOG2OBJECTSPERMINI))

/*****************************************************************************/
/*                                                                           */
/*  proxipooltag2object()   Map a tag to the object it indexes.              */
/*                                                                           */
/*  WARNING:  fails catastrophically, probably with a segmentation fault, if */
/*  the tag's minipool doesn't yet exist.                                    */
/*                                                                           */
/*  Implemented as a macro to meet your need for speed.                      */
/*                                                                           */
/*  pool:  The proxipool in which to find an object.                         */
/*         Type (struct proxipool *).                                        */
/*  searchtag:  The tag whose object is sought.                              */
/*              Type (tag).                                                  */
/*                                                                           */
/*  Returns a pointer to the object.  Might crash if the tag has not been    */
/*    allocated yet.                                                         */
/*    Type (void *).                                                         */
/*                                                                           */
/*****************************************************************************/

#define proxipooltag2object(pool, searchtag)  \
  ((void *) (proxipooltag2mini(pool, searchtag)->object1block +  \
             (searchtag & (OBJECTSPERMINI - 1)) * (pool)->objectbytes1))

/*****************************************************************************/
/*                                                                           */
/*  proxipooltag2object2()   Map a tag to the supplementary object it        */
/*                           indexes.                                        */
/*                                                                           */
/*  WARNING:  fails catastrophically, probably with a segmentation fault, if */
/*  the tag's minipool doesn't yet exist.                                    */
/*                                                                           */
/*  Implemented as a macro to meet your need for speed.                      */
/*                                                                           */
/*  pool:  The proxipool in which to find a supplementary object.            */
/*         Type (struct proxipool *).                                        */
/*  searchtag:  The tag whose supplementary object is sought.                */
/*              Type (tag).                                                  */
/*                                                                           */
/*  Returns a pointer to the supplementary object.  Might crash if the tag   */
/*    has not been allocated yet.                                            */
/*    Type (void *).                                                         */
/*                                                                           */
/*****************************************************************************/

#define proxipooltag2object2(pool, searchtag)  \
  ((void *) (proxipooltag2mini(pool, searchtag)->object2block +  \
             (searchtag & (tag) (OBJECTSPERMINI - 1)) * (pool)->objectbytes2))

/*****************************************************************************/
/*                                                                           */
/*  proxipooltag2allocindex()   Map a tag to its allocation index.           */
/*                                                                           */
/*  WARNING:  fails catastrophically, probably with a segmentation fault, if */
/*  the tag's minipool doesn't yet exist.                                    */
/*                                                                           */
/*  Implemented as a macro to meet your need for speed.                      */
/*                                                                           */
/*  pool:  The proxipool in which an object is allocated.                    */
/*         Type (struct proxipool *).                                        */
/*  searchtag:  The tag of the object whose allocation index is sought.      */
/*              Type (tag).                                                  */
/*                                                                           */
/*  Returns the allocation index of the object.  Might crash if the tag has  */
/*    not been allocated yet.                                                */
/*    Type (proxipoolulong).                                                 */
/*                                                                           */
/*****************************************************************************/

#define proxipooltag2allocindex(pool, searchtag)  \
  (proxipooltag2mini(pool, searchtag)->allocindex)

/*****************************************************************************/
/*                                                                           */
/*  proxipooliterate()   Return the next allocated tag after a specified     */
/*                       tag.                                                */
/*                                                                           */
/*  This procedure is used to iterate over all the tags in a proxipool, from */
/*  smallest to largest, that have ever been allocated.  The iterator visits */
/*  tags that have been freed, as well as those that are still in use.       */
/*  It's up to the caller to distinguish objects that are not in use.        */
/*                                                                           */
/*  To get the iterator started (i.e. to find the first tag), call this      */
/*  procedure with `thistag' set to NOTATAG.                                 */
/*                                                                           */
/*  When there are no more tags, this procedure returns NOTATAG.             */
/*                                                                           */
/*  Note that when an object is freed, its first word or so gets             */
/*  overwritten by internal data, so it's a little bit dangerous to use this */
/*  procedure.  The caller needs to have a way to distinguish objects that   */
/*  it has previously freed, that does not rely on the first word of the     */
/*  object.  Yes, this is kludgy; but it would be slow for this procedure to */
/*  figure out whether an object has been freed or not, so speed was chosen  */
/*  over elegance.                                                           */
/*                                                                           */
/*  pool:  The proxipool whose tags you wish to iterate over.                */
/*  thistag:  The tag whose successor you seek, or NOTATAG to get started.   */
/*                                                                           */
/*  Returns the smallest allocated tag greater than `thistag', or NOTATAG if */
/*    there is no greater allocated tag.                                     */
/*                                                                           */
/*****************************************************************************/

tag proxipooliterate(struct proxipool *pool,
                     tag thistag)
{
  struct minipoolheader *mini;

  if (thistag == NOTATAG) {
    thistag = 0;
  } else {
    thistag++;
  }

  while (1) {
    if (thistag >= pool->nextgroup) {
      return NOTATAG;
    }
    mini = (struct minipoolheader *)
      arraypoollookup(&pool->minipoolarray, thistag >> LOG2OBJECTSPERMINI);
    /* Does a minipool containing tag `thistag' exist? */
    if (mini == (struct minipoolheader *) NULL) {
      /* No.  Jump to the start of the next group. */
      thistag = (thistag & (tag) ~(OBJECTSPERMINI * MINISPERGROUP - 1)) +
                OBJECTSPERMINI * MINISPERGROUP;
    } else if ((thistag & (OBJECTSPERMINI - 1)) >= mini->firstvirgin) {
      /* `thistag' has never been allocated.  Jump to the start of the */
      /*   next minipool.                                              */
      thistag = (thistag & (tag) ~(OBJECTSPERMINI - 1)) + OBJECTSPERMINI;
    } else {
      return thistag;
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  proxipoolinitpoolpools()   Initializes uninitialized poolpools.          */
/*                                                                           */
/*  The allocation map is an array of poolpools.  This procedure's job is to */
/*  initialize intervals of poolpools just before they are used.  This       */
/*  includes making sure the arraypool has allocated space for them.         */
/*                                                                           */
/*  pool:  The proxipool whose allocation map might need initializing.       */
/*  endindex:  Make sure all poolpools up to and including the one whose     */
/*    index is `endindex' are allocated and initialized.                     */
/*                                                                           */
/*****************************************************************************/

void proxipoolinitpoolpools(struct proxipool *pool,
                            proxipoolulong endindex)
{
  struct poolpool *poollist;

  /* Walk through the poolpools that have not been initialized yet, up to */
  /*   `endindex'.                                                        */
  while (pool->nextinitindex <= endindex) {
    /* Make sure memory has been allocated for this poolpool, and get */
    /*   a pointer to it.                                             */
    poollist = (struct poolpool *)
               arraypoolforcelookup(&pool->poolpools,
                                    (arraypoolulong) pool->nextinitindex);
    /* The linked lists of minipools and the groups are empty. */
    poollist->freelisthead = NOTATAG;
    poollist->freelisttail = NOTATAG;
    poollist->mygroup = NOTATAG;

    pool->nextinitindex++;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  proxipoolnew()   Allocate space for an object in a proxipool.            */
/*                                                                           */
/*  An allocation index `allocindex' is used to place the new object close   */
/*  in memory to other objects with the same allocation index.               */
/*                                                                           */
/*  pool:  The proxipool to allocate an object from.                         */
/*  allocindex:  An allocation index associated with the object to ensure    */
/*    it is allocated in memory near other objects that are geometrically    */
/*    nearby.                                                                */
/*  outobject:  If `outobject' is not NULL, a pointer to the new object is   */
/*    returned at the location `outobject' points to.                        */
/*                                                                           */
/*  Returns the tag of the new object.                                       */
/*                                                                           */
/*****************************************************************************/

tag proxipoolnew(struct proxipool *pool,
                 proxipoolulong allocindex,
                 void **outobject)
{
  struct minipoolheader *allocmini;
  struct poolpool *poollist;
  void *objectptr;
  tag alloctag, nexttag;
  miniindex objectindex;

  /* If the poolpool at index `allocindex' has not been initialized yet, */
  /*   initialize all the unitialized poolpools up to `allocindex'.      */
  if (allocindex >= pool->nextinitindex) {
    proxipoolinitpoolpools(pool, allocindex);
  }

  /* Find the assigned poolpool. */
  poollist = (struct poolpool *) arraypoolfastlookup(&pool->poolpools,
                                                     allocindex);
  /* Find the first minipool in the poolpool's list. */
  alloctag = poollist->freelisthead;
  if (alloctag != NOTATAG) {
    /* Get a pointer to the minipool. */
    allocmini = proxipooltag2mini(pool, alloctag);
  } else {
    /* The poolpool's list of minipools is empty.  Check for an unfinished */
    /*   group to allocate a new minipool from.                            */
    alloctag = poollist->mygroup;
    if (alloctag == NOTATAG) {
      /* The poolpool has no group.  Assign a new minipool (and a new group) */
      /*   to this poolpool.                                                 */
      alloctag = pool->nextgroup;
      /* Allocate a different group next time. */
      pool->nextgroup += OBJECTSPERMINI * MINISPERGROUP;
    }

    /* Allocate a new minipool that starts with the tag `alloctag'. */
    allocmini = (struct minipoolheader *)
      arraypoolforcelookup(&pool->minipoolarray,
                           (arraypoolulong) (alloctag >> LOG2OBJECTSPERMINI));

    /* Reset the new minipool to a pristine state. */
    proxipoolrestartmini(pool, allocmini);
    /* Put the new minipool on the poolpool's list of minipools. */
    poollist->freelisthead = alloctag;
    poollist->freelisttail = alloctag;

    /* Inform the minipool of the index of its poolpool. */
    allocmini->allocindex = allocindex;

    /* Is there another minipool in the same group, and if so, is the next */
    /*   minipool unallocated?                                             */
    if ((((alloctag >> LOG2OBJECTSPERMINI) & (MINISPERGROUP - 1)) <
         MINISPERGROUP - 1) &&
        (arraypoollookup(&pool->minipoolarray,
                         (arraypoolulong)
                         (alloctag >> LOG2OBJECTSPERMINI) + 1) ==
         (void *) NULL)) {
      /* Remember this group and the next minipool for use in the future. */
      poollist->mygroup = alloctag + OBJECTSPERMINI;
    } else {
      poollist->mygroup = NOTATAG;
    }
  }

  /* Check if the linked list of dead objects is empty. */
  objectindex = allocmini->freestack;
  if (objectindex != NOTAMINIINDEX) {
    /* Allocate an object from the linked list, rather than a fresh one. */
    objectptr = (void *) 
                &allocmini->object1block[pool->objectbytes1 * objectindex];
    allocmini->freestack = * (miniindex *) objectptr;
  } else {
    /* Allocate a previously unused object. */
    objectindex = allocmini->firstvirgin;
    allocmini->firstvirgin++;
    objectptr = (void *)
                &allocmini->object1block[pool->objectbytes1 * objectindex];
  }

  /* The tag of the new object is its minipool prefix plus its index. */
  alloctag += objectindex;
  /* Is the minipool full? */
  if ((allocmini->freestack == NOTAMINIINDEX) &&
      (allocmini->firstvirgin >= (miniindex) OBJECTSPERMINI)) {
    /* The minipool is full.  Remove it from the linked list. */
    nexttag = allocmini->nextminifree;
    poollist->freelisthead = nexttag;
    if (nexttag == NOTATAG) {
      poollist->freelisttail = NOTATAG;
    }
  }

  pool->objects++;
  if (pool->objects > pool->maxobjects) {
    pool->maxobjects = pool->objects;
  }

  /* Return the new object's memory address (if desired) and tag. */
  if (outobject != (void **) NULL) {
    *outobject = objectptr;
  }
  return alloctag;
}

/*****************************************************************************/
/*                                                                           */
/*  proxipoolfree()   Deallocate an object, freeing its space for reuse.     */
/*                                                                           */
/*  WARNING:  Calling this procedure with a tag that is already free will    */
/*  probably corrupt the proxipool and cause the freed tag to be allocated   */
/*  more than once.                                                          */
/*                                                                           */
/*  pool:  The proxipool the object was allocated from.                      */
/*  killtag:  The tag of the object that is no longer in use.                */
/*                                                                           */
/*****************************************************************************/

void proxipoolfree(struct proxipool *pool,
                   tag killtag)
{
  struct minipoolheader *deallocmini;
  struct minipoolheader *tailmini;
  struct poolpool *poollist;
  miniindex objectindex;

  /* Get a pointer to the minipool. */
  deallocmini = proxipooltag2mini(pool, killtag);
  objectindex = (miniindex) (killtag & (tag) (OBJECTSPERMINI - 1));

  /* Is the minipool full? */
  if ((deallocmini->freestack == NOTAMINIINDEX) &&
      (deallocmini->firstvirgin >= (miniindex) OBJECTSPERMINI)) {
    /* The minipool will no longer be full, so put it back in its poolpool's */
    /*   linked list of minipools.                                           */
    poollist = (struct poolpool *)
               arraypoolfastlookup(&pool->poolpools, deallocmini->allocindex);
    killtag = killtag & ~ (tag) (OBJECTSPERMINI - 1);
    if (poollist->freelisthead == NOTATAG) {
      /* The list is empty; insert the minipool at the head of the list. */
      poollist->freelisthead = killtag;
    } else {
      /* Insert the minipool at the tail of the list. */
      tailmini = proxipooltag2mini(pool, poollist->freelisttail);
      tailmini->nextminifree = killtag;
    }
    poollist->freelisttail = killtag;
    /* There is no next minipool in the list. */
    deallocmini->nextminifree = NOTATAG;
  }

  /* Insert the object into the linked list of dead objects. */
  * (miniindex *)
    &deallocmini->object1block[pool->objectbytes1 * objectindex] =
      deallocmini->freestack;
  deallocmini->freestack = objectindex;

  pool->objects--;
}

/*****************************************************************************/
/*                                                                           */
/*  proxipoolobjects()   Returns the number of objects allocated in the      */
/*                       proximity-based memory pool.                        */
/*                                                                           */
/*  pool:  The proxipool in question.                                        */
/*                                                                           */
/*  Returns the number of objects currently allocated in `pool'.             */
/*                                                                           */
/*****************************************************************************/

arraypoolulong proxipoolobjects(struct proxipool *pool)
{
  return pool->objects;
}

/*****************************************************************************/
/*                                                                           */
/*  proxipoolbytes()   Returns the number of bytes of dynamic memory used by */
/*                     the proximity-based memory pool.                      */
/*                                                                           */
/*  Does not include the memory for the `struct proxipool' record itself.    */
/*                                                                           */
/*  pool:  The proxipool in question.                                        */
/*                                                                           */
/*  Returns the number of dynamically allocated bytes in `pool'.             */
/*                                                                           */
/*****************************************************************************/

arraypoolulong proxipoolbytes(struct proxipool *pool)
{
  return arraypoolbytes(&pool->minipoolarray) +
         arraypoolbytes(&pool->poolpools);
}

/**                                                                         **/
/**                                                                         **/
/********* Proximity-based memory pool management routines end here  *********/


/********* Z-order computation routines begin here                   *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  zorderbefore()   Returns 1 if the point (x1, y1, z1) occurs before       */
/*                   (x2, y2, z2) in a z-order space-filling curve;          */
/*                   0 otherwise.  Coordinates are floating point.           */
/*                                                                           */
/*  I won't try to explain what a z-order curve is here; look it up on the   */
/*  Web.  This is a somewhat nonstandard z-order curve.  At the top level,   */
/*  points in different orthants are ordered as follows:                     */
/*                                                                           */
/*  nonnegative x, nonnegative y, negative z                                 */
/*  negative x, nonnegative y, negative z                                    */
/*  negative x, negative y, negative z                                       */
/*  nonnegative x, negative y, negative z                                    */
/*  nonnegative x, negative y, nonnegative z                                 */
/*  negative x, negative y, nonnegative z                                    */
/*  negative x, nonnegative y, nonnegative z                                 */
/*  nonnegative x, nonnegative y, nonnegative z (the positive orthant)       */
/*                                                                           */
/*  This is a one-level Hilbert curve.  Note that a zero coordinate is       */
/*  treated as a positive coordinate.  For example, (-1, 1, 1) precedes      */
/*  (0, 0, 1) in the ordering, because the latter point is treated as being  */
/*  in the positive orthant.                                                 */
/*                                                                           */
/*  In the positive orthant, this z-order curve progresses by decreasing z,  */
/*  then decreasing y, then increasing x.  For example, (0, 1, 1) precedes   */
/*  (1, 1, 1) precedes (0, 0, 1) precedes (1, 0, 1) precedes (0, 1, 0)       */
/*  precedes (1, 1, 0) precedes (0, 0, 0) precedes (1, 0, 0).  This ordering */
/*  is chosen to maximize the spatial locality of points in adjoining        */
/*  orthants.                                                                */
/*                                                                           */
/*  In the other orthants, the z-order curve is a reflection across one or   */
/*  more coordinate axes of the z-order curve in the positive orthant.  If   */
/*  the number of reflections is odd (i.e. for points with an odd number of  */
/*  negative coordinates), the z-order is also reversed; for example,        */
/*  (2, 3, 3) precedes (3, 3, 3), but (-3, 3, 3) precedes (-2, 3, 3).  This  */
/*  causes the z-ordering to adjoin nicely between orthants.                 */
/*                                                                           */
/*  This procedure requires IEEE 754 conformant floating-point double        */
/*  precision numbers.  It uses bit manipulation to extract exponents from   */
/*  floating-point numbers and to create numbers with specified exponents.   */
/*  It also uses the clever trick of exclusive-oring two significands to     */
/*  help figure out the first bit of disagreement between two floating-point */
/*  numbers with the same exponent.  Unfortunately, it needs to know the     */
/*  endianness of the hardware to work correctly.                            */
/*                                                                           */
/*  WARNING:  This routine does not work with denormalized numbers.          */
/*  I should fix that some time.                                             */
/*                                                                           */
/*  x1, y1, z1:  Coordinates of the first point to compare.                  */
/*  x2, y2, z2:  Coordinates of the second point to compare.                 */
/*                                                                           */
/*  Returns 1 if first point precedes the second in z-order; 0 otherwise.    */
/*                                                                           */
/*****************************************************************************/

int zorderbefore(starreal x1,
                 starreal y1,
                 starreal z1,
                 starreal x2,
                 starreal y2,
                 starreal z2)
{
  starreal xor, powerof2;
  long x1exp, x2exp, y1exp, y2exp, z1exp, z2exp;
  long xmax, ymax, zmax;
  int toggle;
  int endianness;

  /* For two points in the same orthant, toggle the result once for each */
  /*   negative coordinate in one of the points.                         */
  toggle = 0;
  /* The endianness of the hardware. */
  endianness = 1;

  /* Figure out which orthants the points are in. */
  if (z1 < 0.0) {
    if (z2 < 0.0) {
      /* Both points have negative z-coordinates; reverse the z-ordering. */
      toggle = 1;
      /* Reflect to positive z-coordinates. */
      z1 = -z1;
      z2 = -z2;
    } else {
      /* Negative z always precedes nonnegative z. */
      return 1;
    }
  } else if (z2 < 0.0) {
    /* Nonnegative z never precedes negative z. */
    return 0;
  }

  if (y1 < 0.0) {
    if (y2 < 0.0) {
      /* Both points have negative y-coordinates; reverse the z-ordering. */
      toggle = toggle ^ 1;
      /* Reflect to positive y-coordinates. */
      y1 = -y1;
      y2 = -y2;
    } else {
      /* Negative y precedes nonnegative y (unless z is negative). */
      return 1 ^ toggle;
    }
  } else if (y2 < 0.0) {
    /* Nonnegative y does not precede negative y (unless z is negative). */
    return toggle;
  }

  if (x1 < 0.0) {
    if (x2 < 0.0) {
      /* Both points have negative x-coordinates; reverse the z-ordering. */
      toggle = toggle ^ 1;
      /* Reflect to positive x-coordinates. */
      x1 = -x1;
      x2 = -x2;
    } else {
      /* Negative x precedes nonnegative x (unless toggled). */
      return 1 ^ toggle;
    }
  } else if (x2 < 0.0) {
    /* Nonnegative x does not precede negative x (unless toggled). */
    return toggle;
  }

  /* Determine the exponents of the floating-point numbers.  Note that IEEE  */
  /*   floating-point numbers do not express their exponents in two's        */
  /*   complement; rather, the exponent bits range from 1 to 2046, which     */
  /*   represent exponents from -1022 to 1023.  (Exponent bits of 0          */
  /*   represent denormalized numbers, and 2047 represents infinity or NaN.) */
  if (z1 == z2) {
    /* Check for the case where the two points are equal. */
    if ((x1 == x2) && (y1 == y2)) {
      /* A point does not precede itself. */
      return 0;
    }
    /* The z-coordinates are equal, so the other coordinates will determine */
    /*   which point comes first.  Set the z-exponents so small that they   */
    /*   cannot dominate the other exponents.                               */
    z1exp = -1l;
    z2exp = -1l;
  } else {
    /* Get the z-exponents by masking out the right bits. */
    z1exp = ((long *) &z1)[endianness] & 0x7ff00000l;
    z2exp = ((long *) &z2)[endianness] & 0x7ff00000l;
  }

  if (y1 == y2) {
    /* The y-coordinates are equal, so the other coordinates will determine */
    /*   which point comes first.  Set the y-exponents so small that they   */
    /*   cannot dominate the other exponents.                               */
    y1exp = -1l;
    y2exp = -1l;
  } else {
    /* Get the y-exponents by masking out the right bits. */
    y1exp = ((long *) &y1)[endianness] & 0x7ff00000l;
    y2exp = ((long *) &y2)[endianness] & 0x7ff00000l;
  }

  if (x1 == x2) {
    /* The x-coordinates are equal, so the other coordinates will determine */
    /*   which point comes first.  Set the x-exponents so small that they   */
    /*   cannot dominate the other exponents.                               */
    x1exp = -1l;
    x2exp = -1l;
  } else {
    /* Get the x-exponents by masking out the right bits. */
    x1exp = ((long *) &x1)[endianness] & 0x7ff00000l;
    x2exp = ((long *) &x2)[endianness] & 0x7ff00000l;
  }

  /* Compute the maximum z-, y-, and x-exponents. */
  zmax = z1exp > z2exp ? z1exp : z2exp;
  ymax = y1exp > y2exp ? y1exp : y2exp;
  xmax = x1exp > x2exp ? x1exp : x2exp;

  /* Repeat the following until one pair exponents clearly dominates. */
  /*   This loop iterates at most four times.                         */
  while (1) {
    if ((zmax >= ymax) && (zmax >= xmax)) {
      /* The largest z-exponent dominates, or at least equals, the x- and */
      /*   y-exponents.  Figure out if the z-exponents differ.            */
      if (z1exp < z2exp) {
        /* The z-exponent of the second point dominates, so (in the positive */
        /*   orthant) the second point precedes the first.                   */
        return toggle;
      } else if (z1exp > z2exp) {
        /* The z-exponent of the first point dominates, so (in the positive */
        /*   orthant) the first point precedes the second.                  */
        return 1 ^ toggle;
      } else {                                             /* z1exp == z2exp */
        /* Both points have the same z-exponent, so we need to determine  */
        /*   which bit of the two z-coordinates is the first to differ.   */
        /*   First, set `powerof2' to be 2^`z1exp', so `powerof2' has the */
        /*   same exponent as z1 and z2.                                  */
        ((long *) &powerof2)[!endianness] = 0l;
        ((long *) &powerof2)[endianness] = z1exp & 0x7ff00000l;
        /* Second, set `xor' to be a floating-point number whose exponent is */
        /*   `z1exp', and whose significand is the exclusive or of the       */
        /*   significands of z1 and z2--except the first bit of the          */
        /*   significand, which is the "hidden bit" of the IEEE format and   */
        /*   remains a 1.                                                    */
        ((long *) &xor)[!endianness] = ((long *) &z1)[!endianness] ^
                                       ((long *) &z2)[!endianness];
        ((long *) &xor)[endianness] = ((((long *) &z1)[endianness] ^
                                        ((long *) &z2)[endianness]) &
                                       ~0xfff00000l) | (z1exp & 0x7ff00000l);
        /* Third, subtract `powerof2' from `xor'.  Since they are both   */
        /*   positive and both have the same exponent, this operation is */
        /*   exact (no roundoff error), and the exponent of the result   */
        /*   will indicate the first bit where z1 and z2 disagree.       */
        xor -= powerof2;
        /* Determine the exponent of `xor'. */
        zmax = ((long *) &xor)[endianness] & 0x7ff00000l;
        /* If we were to cancel out the leading 1 bits of z1 and z2 that  */
        /*   agree, the larger of the two resulting numbers would have an */
        /*   exponent of `zmax'.  The exponent of the smaller one is      */
        /*   irrelevant, so set it really really small.                   */
        if (z1 > z2) {
          z1exp = zmax;
          z2exp = -1l;
        } else {
          z2exp = zmax;
          z1exp = -1l;
        }
        /* Now go through the loop again, because the z-exponent might */
        /*   or might not still dominate.                              */
      }
    } else if ((ymax > zmax) && (ymax >= xmax)) {
      /* The largest y-exponent dominates the z-exponents, and at least   */
      /*   equals the x-exponents.  Figure out if the y-exponents differ. */
      if (y1exp < y2exp) {
        /* The y-exponent of the second point dominates, so (in the positive */
        /*   orthant) the second point precedes the first.                   */
        return toggle;
      } else if (y1exp > y2exp) {
        /* The y-exponent of the first point dominates, so (in the positive */
        /*   orthant) the first point precedes the second.                  */
        return 1 ^ toggle;
      } else {
        /* Both points have the same y-exponent, so we need to determine  */
        /*   which bit of the two y-coordinates is the first to differ.   */
        /*   First, set `powerof2' to be 2^`y1exp', so `powerof2' has the */
        /*   same exponent as y1 and y2.                                  */
        ((long *) &powerof2)[!endianness] = 0l;
        ((long *) &powerof2)[endianness] = y1exp & 0x7ff00000l;
        /* Second, set `xor' to be a floating-point number whose exponent is */
        /*   `y1exp', and whose significand is the exclusive or of the       */
        /*   significands of y1 and y2--except the first bit of the          */
        /*   significand, which is the "hidden bit" of the IEEE format and   */
        /*   remains a 1.                                                    */
        ((long *) &xor)[!endianness] = ((long *) &y1)[!endianness] ^
                                       ((long *) &y2)[!endianness];
        ((long *) &xor)[endianness] = ((((long *) &y1)[endianness] ^
                                        ((long *) &y2)[endianness]) &
                                       ~0xfff00000l) | (y1exp & 0x7ff00000l);
        /* Third, subtract `powerof2' from `xor'.  Since they are both   */
        /*   positive and both have the same exponent, this operation is */
        /*   exact (no roundoff error), and the exponent of the result   */
        /*   will indicate the first bit where y1 and y2 disagree.       */
        xor -= powerof2;
        /* Determine the exponent of `xor'. */
        ymax = ((long *) &xor)[endianness] & 0x7ff00000l;
        /* If we were to cancel out the leading 1 bits of y1 and y2 that  */
        /*   agree, the larger of the two resulting numbers would have an */
        /*   exponent of `ymax'.  The exponent of the smaller one is      */
        /*   irrelevant, so set it really really small.                   */
        if (y1 > y2) {
          y1exp = ymax;
          y2exp = -1l;
        } else {
          y2exp = ymax;
          y1exp = -1l;
        }
        /* Now go through the loop again, because the y-exponent might */
        /*   or might not still dominate.                              */
      }
    } else {                               /* (xmax > ymax) && (xmax > zmax) */
      /* The largest x-exponent dominates the y- and z-exponents.  Figure */
      /*   out if the x-exponents differ.                                 */
      if (x1exp < x2exp) {
        /* The x-exponent of the second point dominates, so (in the positive */
        /*   orthant) the first point precedes the second.                   */
        return 1 ^ toggle;
      } else if (x1exp > x2exp) {
        /* The x-exponent of the first point dominates, so (in the positive */
        /*   orthant) the second point precedes the first.                  */
        return toggle;
      } else {
        /* Both points have the same x-exponent, so we need to determine  */
        /*   which bit of the two x-coordinates is the first to differ.   */
        /*   First, set `powerof2' to be 2^`x1exp', so `powerof2' has the */
        /*   same exponent as x1 and x2.                                  */
        ((long *) &powerof2)[!endianness] = 0l;
        ((long *) &powerof2)[endianness] = x1exp & 0x7ff00000l;
        /* Second, set `xor' to be a floating-point number whose exponent is */
        /*   `x1exp', and whose significand is the exclusive or of the       */
        /*   significands of x1 and x2--except the first bit of the          */
        /*   significand, which is the "hidden bit" of the IEEE format and   */
        /*   remains a 1.                                                    */
        ((long *) &xor)[!endianness] = ((long *) &x1)[!endianness] ^
                                       ((long *) &x2)[!endianness];
        ((long *) &xor)[endianness] = ((((long *) &x1)[endianness] ^
                                        ((long *) &x2)[endianness]) &
                                       ~0xfff00000l) | (x1exp & 0x7ff00000l);
        /* Third, subtract `powerof2' from `xor'.  Since they are both   */
        /*   positive and both have the same exponent, this operation is */
        /*   exact (no roundoff error), and the exponent of the result   */
        /*   will indicate the first bit where x1 and x2 disagree.       */
        xor -= powerof2;
        /* Determine the exponent of `xor'. */
        xmax = ((long *) &xor)[endianness] & 0x7ff00000l;
        /* If we were to cancel out the leading 1 bits of x1 and x2 that  */
        /*   agree, the larger of the two resulting numbers would have an */
        /*   exponent of `xmax'.  The exponent of the smaller one is      */
        /*   irrelevant, so set it really really small.                   */
        if (x1 > x2) {
          x1exp = xmax;
          x2exp = -1l;
        } else {
          x2exp = xmax;
          x1exp = -1l;
        }
        /* Now go through the loop again, because the x-exponent might */
        /*   or might not still dominate.                              */
      }
    }
  }
}

/**                                                                         **/
/**                                                                         **/
/********* Z-order computation routines end here                     *********/


/********* Allocation map routines begin here                        *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  Allocation maps (struct allocmap)                                        */
/*                                                                           */
/*  An allocation map is a data structure that maps each point in 3D space   */
/*  to an "allocation index" (of type proxipoolulong), which determines      */
/*  which pool of memory data should be used to allocate structures          */
/*  associated with the point.  The allocation indices are meant for use     */
/*  with proxipools (proximity-based memory pools), though other data        */
/*  structures could be designed to use them as well.                        */
/*                                                                           */
/*  An allocation map allows a client to insert or delete "sample points."   */
/*  Each sample point is assigned a unique allocation index.  A client can   */
/*  query the allocation map with a point (not usually a sample point), and  */
/*  learn the allocation index of a sample point that is "near" the query    */
/*  point.  The purpose is to allow a client to store data structures so     */
/*  they are spatially coherent:  structures that are near each other        */
/*  geometrically also tend to be near each other in memory (because they    */
/*  are likely to have the same allocation index).                           */
/*                                                                           */
/*  A good way to use an allocation map is to have it contain points that    */
/*  are a small random sample of the entire point set processed by the       */
/*  client.  The random sample ensures that the points in the allocation map */
/*  are roughly representative of the spatial distribution of the larger     */
/*  point set.  The sample size should be chosen so perhaps thousands or     */
/*  tens of thousands of point map to a single allocation index.             */
/*                                                                           */
/*  Ideally, an allocation map would always return the allocation index of   */
/*  the sample point nearest the query point, perhaps using a dynamic        */
/*  Voronoi diagram.  But not only would that be a hassle to implement; it   */
/*  would be slower than I'd like.  Instead, I order the points along a      */
/*  space-filling z-order curve and store them in a splay tree.  The space-  */
/*  filling curve reduces the search problem to one dimension, so a simple   */
/*  data structure suffices; and splay trees run like greased weasels.  The  */
/*  region of space that maps to a sample point isn't ideally shaped, but    */
/*  it's good enough.                                                        */
/*                                                                           */
/*  A sample point can be removed from the allocation map.  Part of the      */
/*  reason for this is to support dynamically changing triangulations, in    */
/*  which vertices might just disappear.  When a sample point is removed,    */
/*  its allocation index is placed in a "spare tree," from which it can be   */
/*  reallocated again.  The goal is to make sure that the memory pools (say, */
/*  in a proxipool) associated with an allocation index don't fall into      */
/*  disuse (which would be bad, as they're still taking up memory.  When a   */
/*  sample point is inserted into the allocation tree, the code finds the    */
/*  allocation index in the spare tree whose old sample point is closest to  */
/*  the new sample point, and allocates that index to the new point.  With   */
/*  luck, the new points that map to the index might partly overlap the old  */
/*  points, still in use, that used to map to the index.  When the spare     */
/*  tree is empty, the code allocates brand spankin' new indices.            */
/*                                                                           */
/*  If there are no sample points in the allocation map, every point maps to */
/*  a default allocation index of zero.  When sample points do exist, their  */
/*  allocation indices are numbered starting from one--no sample point ever  */
/*  get assigned the allocation index zero.                                  */
/*                                                                           */
/*  Public interface:                                                        */
/*  struct allocmap   Allocation map object.                                 */
/*  void allocmapinit(tree, verbose)   Initialize an empty allocation tree.  */
/*  void allocmaprestart(tree)   Deallocates all the tree nodes (not to OS). */
/*  void allocmapdeinit()   Free an allocation tree's memory to the OS.      */
/*  proxipoolulong allocmapindex(tree, x, y, z)   Query a point, returning   */
/*    an allocation index.                                                   */
/*  proxipoolulong allocmapnewpoint(tree, x, y, z)   Insert a point.         */
/*  void allocmapdeletepoint(tree, x, y, z)   Delete a point.                */
/*  arraypoolulong allocmapbytes(tree)   Returns memory taken by tree.       */
/*                                                                           */
/*  For internal use only:                                                   */
/*  struct allocmapnode *allocmapnewnode(tree)                               */
/*  int allocmapsplay(treeroot, x, y, z, nearnode)                           */
/*  void allocmapmax(treeroot)                                               */
/*  void allocmapinsertroot(treeroot, insertnode, splayside)                 */
/*  void allocmapremoveroot(treeroot)                                        */
/*                                                                           */
/*****************************************************************************/


/*  The logarithm of the number of allocation tree nodes allocated at once.  */

#define LOG2OCTREENODESPERBLOCK 8


/*  Each `allocmapnode' is a node in the allocation tree.  Its fields        */
/*  include its `left' and `right' children and its sample point             */
/*  (xsam, ysam, zsam).  Each node has an allocation index `index', which is */
/*  the node's index in the arraypool of nodes, and is also used by          */
/*  memorypools to decide which chunks of memory to allocate objects from.   */
/*                                                                           */
/*  The node with index zero is the "default" node.  This node can never be  */
/*  part of the splay tree.  It is the node returned when the splay tree is  */
/*  empty, so that every point can be assigned an allocation index even when */
/*  there are no points in the tree.  Nodes allocated to participate in the  */
/*  tree are numbered starting from one.                                     */

struct allocmapnode {
  struct allocmapnode *left, *right;          /* My left and right children. */
  starreal xsam, ysam, zsam;     /* Coordinates of this node's sample point. */
  proxipoolulong index;                      /* My index (in the arraypool). */
};

/*  Each `allocmap' is an allocation tree, used to map a point in 3D space   */
/*  to an index that specifies which minipool (in a proxipool) an object     */
/*  should be allocated from.  The idea is that points that are spatially    */
/*  close together should be close together in memory too.  The allocation   */
/*  tree data structure is a splay tree whose points are ordered along a     */
/*  z-order space-filling curve.                                             */
/*                                                                           */
/*  The nodes are allocated from the memory pool `nodearray'.  The root of   */
/*  the allocation tree is `allocroot'.  When a node is removed from the     */
/*  allocation tree, it is inserted into the spare tree, whose root is       */
/*  `spareroot'.  Nodes in the spare tree can be reused, whereupon they are  */
/*  inserted back into the allocation tree.  The purpose of the spare tree   */
/*  is to allow reuse of allocation indices in a spatially coherent way--    */
/*  ideally, a new point in the allocation tree will receive an index that   */
/*  was previously used by a point close to it.                              */
/*                                                                           */
/*  The number "verbosity" indicates how much debugging information to       */
/*  print, from none (0) to lots (4+).                                       */

struct allocmap {
  struct arraypool nodearray;      /* Tiered array of allocation tree nodes. */
  struct allocmapnode *allocroot;                /* Root of allocation tree. */
  struct allocmapnode *spareroot;                /* Root of spare node tree. */
  int verbosity;                /* Amount of debugging information to print. */
};


/*****************************************************************************/
/*                                                                           */
/*  allocmaprestart()   Deallocate all the tree nodes in an allocmap.        */
/*                                                                           */
/*  The tree returns to its starting state, except that no memory (allocated */
/*  by the arraypool of tree nodes) is freed to the operating system.        */
/*                                                                           */
/*  tree:  The allocmap to restart.                                          */
/*                                                                           */
/*****************************************************************************/

void allocmaprestart(struct allocmap *tree)
{
  struct allocmapnode *defaultnode;
  arraypoolulong defaultindex;

  /* Empty the trees. */
  tree->allocroot = (struct allocmapnode *) NULL;
  tree->spareroot = (struct allocmapnode *) NULL;

  /* Restart the pool of tree nodes. */
  arraypoolrestart(&tree->nodearray);

  /* Allocate a "default" node (which is never in either tree) having index */
  /*   zero.                                                                */
  defaultindex = arraypoolnewindex(&tree->nodearray, (void **) &defaultnode);
  if (defaultindex != 0) {
    printf("Internal error in allocmaprestart():\n");
    printf("  First index allocated from restarted arraypool is not zero.\n");
    internalerror();
  }

  defaultnode->index = 0;
  /* Give the default node a bogus sample point. */
  defaultnode->xsam = 0.0;
  defaultnode->ysam = 0.0;
  defaultnode->zsam = 0.0;
}

/*****************************************************************************/
/*                                                                           */
/*  allocmapinit()   Initialize an empty allocation tree.                    */
/*                                                                           */
/*  tree:  The allocmap to initialize.                                       */
/*  verbose:  How much debugging information allocmap procedures should      */
/*    print, from none (0) to lots (4+).                                     */
/*                                                                           */
/*****************************************************************************/

void allocmapinit(struct allocmap *tree,
                  int verbose)
{
  /* Initialize an arraypool to allocate nodes from.  Note that this is an */
  /*   array of actual nodes, not an array of pointers to nodes.           */
  arraypoolinit(&tree->nodearray,
                (arraypoolulong) sizeof(struct allocmapnode),
                (arraypoolulong) LOG2OCTREENODESPERBLOCK, 1);
  tree->verbosity = verbose;
  /* Empty out the tree and set up the default node. */
  allocmaprestart(tree);
}

/*****************************************************************************/
/*                                                                           */
/*  allocmapdeinit()   Free to the operating system all memory taken by an   */
/*                     allocation tree.                                      */
/*                                                                           */
/*  tree:  The allocmap to free.                                             */
/*                                                                           */
/*****************************************************************************/

void allocmapdeinit(struct allocmap *tree)
{
  arraypooldeinit(&tree->nodearray);
}

/*****************************************************************************/
/*                                                                           */
/*  allocmapnewnode()   Allocate a new node in an allocation tree.           */
/*                                                                           */
/*  This routine may allocate a node that has been previously freed.  If so, */
/*  the node has the same index it had before.  (Node indices never change.) */
/*                                                                           */
/*  tree:  The allocmap to allocate a new node from.                         */
/*                                                                           */
/*  Returns a pointer to the new node.                                       */
/*                                                                           */
/*****************************************************************************/

struct allocmapnode *allocmapnewnode(struct allocmap *tree)
{
  struct allocmapnode *newnode;
  arraypoolulong newindex;

  /* Allocate a node from the arraypool. */
  newindex = arraypoolnewindex(&tree->nodearray, (void **) &newnode);
  /* Teach it its own index. */
  newnode->index = (proxipoolulong) newindex;

  return newnode;
}

/*****************************************************************************/
/*                                                                           */
/*  allocmapsplay()   This top-down splay tree code searches the tree for a  */
/*                    node with coordinates (x, y, z), and splays it (if it  */
/*                    is found) or the last node encountered to the root.    */
/*                    Nodes in the tree are ordered by a z-order space-      */
/*                    filling curve.                                         */
/*                                                                           */
/*  If the search does not find a node whose sample point is exactly         */
/*  (x, y, z), consider two nodes:  the latest node that precedes (x, y, z), */
/*  and the earliest node that follows (x, y, z).  One of these nodes,       */
/*  chosen arbitrarily, is splayed to the root (if the tree is not empty).   */
/*  If both nodes exist (i.e. (x, y, z) does not precede every node in the   */
/*  tree, nor follow every node), the other node is stored in `*nearnode' on */
/*  return.                                                                  */
/*                                                                           */
/*  If the search finds an exact match, or if (x, y, z) precedes or follows  */
/*  every node in the tree, then `*nearnode' is NULL on return.              */
/*                                                                           */
/*  treeroot:  The root of the tree in which to search, twice indirected.    */
/*    On return, `*treeroot' will point to the new root.                     */
/*  x, y, z:  The coordinates of the point whose near neighbor is sought.    */
/*  nearnode:  On return, `*nearnode' may point to a node containing an      */
/*    alternative point near (x, y, z).  Output only; the value of           */
/*    `*nearnode' on entry is irrelevant.                                    */
/*                                                                           */
/*  Returns 0 if a node with value (x, y, z) is found and splayed to the     */
/*    root; -1 if (x, y, z) precedes the node splayed to the root; or 1 if   */
/*    (x, y, z) follows the node splayed to the root.  Returns 0 if the tree */
/*    is empty.                                                              */
/*                                                                           */
/*****************************************************************************/

int allocmapsplay(struct allocmapnode **treeroot,
                  starreal x,
                  starreal y,
                  starreal z,
                  struct allocmapnode **nearnode)
{
  struct allocmapnode catcher;
  struct allocmapnode *root;
  struct allocmapnode *child;
  struct allocmapnode *leftcatcher, *rightcatcher;
  int compare;

  *nearnode = (struct allocmapnode *) NULL;

  root = *treeroot;
  if (root == (struct allocmapnode *) NULL) {
    /* The tree is empty. */
    return 0;
  }

  /* `catcher' is an allocmapnode which will not be part of the final tree,  */
  /*   but is used temporarily to hold subtrees.  The subtree that will be   */
  /*   the left child of the root is stored as the right child of `catcher', */
  /*   and vice versa.  (This backward connection simplifies the algorithm.) */
  /*   `leftcatcher' is a pointer used up to build up the left subtree of    */
  /*   the root, from top to bottom right; it points to the rightmost node   */
  /*   in the subtree.  `rightcatcher' serves a symmetric purpose.           */
  leftcatcher = &catcher;
  rightcatcher = &catcher;

  /* Loop of splaying operations.  During each iteration, `root' is the node */
  /*   currently being visited, as well as the root of the subtree currently */
  /*   being searched.                                                       */
  while (1) {
    /* Compare (x, y, z) with this node's sample point.  The result is */
    /*   0 if equal; -1 if (x, y, z) precedes; 1 if (x, y, z) follows. */
    compare = ((x == root->xsam) && (y == root->ysam) && (z == root->zsam)) ?
              0 : zorderbefore(x, y, z, root->xsam, root->ysam, root->zsam) ?
              -1 : 1;
    /* Is `root' the node that will be the root in the end? */
    if ((compare == 0) ||
        ((compare < 0) && (root->left == (struct allocmapnode *) NULL)) ||
        ((compare > 0) && (root->right == (struct allocmapnode *) NULL))) {
      /* Yes, the search is over.  If `root' has children, attach them to */
      /*   the new left and right subtrees.                               */
      leftcatcher->right = root->left;
      rightcatcher->left = root->right;
      /* Attach the left and right subtrees to the new root. */
      root->left = catcher.right;
      root->right = catcher.left;
      /* The new root is `root'. */
      *treeroot = root;
      /* Find the node `nearnode' such that (x, y, z) is between `root' */
      /*   and `nearnode'.                                              */
      if ((compare < 0) && (leftcatcher != &catcher)) {
        *nearnode = leftcatcher;
      } else if ((compare > 0) && (rightcatcher != &catcher)) {
        *nearnode = rightcatcher;
      }
      return compare;
    } else if (compare < 0) {
      /* Search the left subtree of `root'. */
      child = root->left;
      /* Compare (x, y, z) with the left child's sample point. */
      compare = ((x == child->xsam) && (y == child->ysam) &&
                 (z == child->zsam)) ? 0 :
                zorderbefore(x, y, z, child->xsam, child->ysam, child->zsam) ?
                -1 : 1;
      /* Is `child' the node that will be the root in the end? */
      if ((compare == 0) ||
          ((compare < 0) && (child->left == (struct allocmapnode *) NULL)) ||
          ((compare > 0) && (child->right == (struct allocmapnode *) NULL))) {
        /* Yes, the search is over.  Perform a "zig" rotation. */
        root->left = child->right;
        /* If `child' has a left child, attach it to the new left subtree. */
        /*   Attach `root' to the new right subtree.                       */
        leftcatcher->right = child->left;
        rightcatcher->left = root;
        /* Attach the left and right subtrees to the new root. */
        child->left = catcher.right;
        child->right = catcher.left;
        /* The new root is `child'. */
        *treeroot = child;
        /* Find the node such that (x, y, z) is between `child' and */
        /*   `nearnode'.                                            */
        if (compare > 0) {
          *nearnode = root;
        } else if ((compare < 0) && (leftcatcher != &catcher)) {
          *nearnode = leftcatcher;
        }
        return compare;
      } else if (compare < 0) {
        /* The search continues to the left.  Perform a "zig-zig" rotation. */
        root->left = child->right;
        child->right = root;
        /* Attach the child to the new right subtree. */
        rightcatcher->left = child;
        rightcatcher = child;
        /* On the next iteration, examine the left-left grandchild of */
        /*   `root'.                                                  */
        root = child->left;
      } else {
        /* The search continues to the right.  Perform a "zig-zag" rotation  */
        /*   by attaching the left child to the new left subtree, and `root' */
        /*   to the new right subtree.                                       */
        leftcatcher->right = child;
        leftcatcher = child;
        rightcatcher->left = root;
        rightcatcher = root;
        /* On the next iteration, examine the left-right grandchild of */
        /*   `root'.                                                   */
        root = child->right;
      }
    } else {
      /* Search the right subtree of `root'. */
      child = root->right;
      /* Compare (x, y, z) with the right child's sample point. */
      compare = ((x == child->xsam) && (y == child->ysam) &&
                 (z == child->zsam)) ? 0 :
                zorderbefore(x, y, z, child->xsam, child->ysam, child->zsam) ?
                -1 : 1;
      /* Is `child' the node that will be the root in the end? */
      if ((compare == 0) ||
          ((compare < 0) && (child->left == (struct allocmapnode *) NULL)) ||
          ((compare > 0) && (child->right == (struct allocmapnode *) NULL))) {
        /* Yes, the search is over.  Perform a "zig" rotation. */
        root->right = child->left;
        /* If `child' has a right child, attach it to the new right subtree. */
        /*   Attach `root' to the new left subtree.                          */
        leftcatcher->right = root;
        rightcatcher->left = child->right;
        /* Attach the left and right subtrees to the new root. */
        child->left = catcher.right;
        child->right = catcher.left;
        /* The new root is `child'. */
        *treeroot = child;
        /* Find the node `nearnode' such that (x, y, z) is between `child' */
        /*   and `nearnode'.                                               */
        if (compare < 0) {
          *nearnode = root;
        } else if ((compare > 0) && (rightcatcher != &catcher)) {
          *nearnode = rightcatcher;
        }
        return compare;
      } else if (compare > 0) {
        /* The search continues to the right.  Perform a "zig-zig" rotation. */
        root->right = child->left;
        child->left = root;
        /* Attach the child to the new left subtree. */
        leftcatcher->right = child;
        leftcatcher = child;
        /* On the next iteration, examine the right-right grandchild of */
        /*   `root'.                                                    */
        root = child->right;
      } else {
        /* The search continues to the left.  Perform a "zig-zag" rotation */
        /*   by attaching `root' to the new left subtree, and the right    */
        /*   child to the new left subtree.                                */
        leftcatcher->right = root;
        leftcatcher = root;
        rightcatcher->left = child;
        rightcatcher = child;
        /* On the next iteration, examine the right-left grandchild of */
        /*   `root'.                                                   */
        root = child->left;
      }
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  allocmapmax()   Splays the rightmost node of a tree to the root.         */
/*                                                                           */
/*  treeroot:  The root of the tree to splay, twice indirected.  On return,  */
/*    `*treeroot' will point to the new root.                                */
/*                                                                           */
/*****************************************************************************/

void allocmapmax(struct allocmapnode **treeroot)
{
  struct allocmapnode catcher;
  struct allocmapnode *root;
  struct allocmapnode *rightchild;
  struct allocmapnode *leftcatcher;

  root = *treeroot;
  if (root == (struct allocmapnode *) NULL) {
    /* The tree is empty. */
    return;
  }

  /* `catcher' is an allocmapnode which will not be part of the final tree,  */
  /*   but is used temporarily to hold a subtree.  The subtree that will be  */
  /*   the left child of the root is stored as the right child of `catcher'. */
  /*   (This backward connection simplifies the algorithm.)  `leftcatcher'   */
  /*   is a pointer used up to build up the left subtree of the root, from   */
  /*   top to bottom right; it points to the rightmost node in the subtree.  */
  leftcatcher = &catcher;

  /* Search to the far right of the tree, splaying as we go. */
  while (1) {
    rightchild = root->right;
    if (rightchild == (struct allocmapnode *) NULL) {
      /* If `root' has a left child, attach it to the new left subtree. */
      leftcatcher->right = root->left;
      /* Attach the left subtree to the new root. */
      root->left = catcher.right;
      /* The new root is `root'. */
      *treeroot = root;
      return;
    } else if (rightchild->right == (struct allocmapnode *) NULL) {
      /* The right child of `root' will be the new root.  Perform a "zig" */
      /*   rotation.                                                      */
      root->right = rightchild->left;
      /* Attach `root' to the bottom right of the new left subtree. */
      leftcatcher->right = root;
      /* Attach the left subtree to the new root. */
      rightchild->left = catcher.right;
      /* The new root is `rightchild'. */
      *treeroot = rightchild;
      return;
    } else {
      /* The search continues to the right.  Perform a "zig-zig" rotation. */
      root->right = rightchild->left;
      rightchild->left = root;
      /* Attach the child to the new left subtree. */
      leftcatcher->right = rightchild;
      leftcatcher = rightchild;
      /* On the next iteration, examine the right-right grandchild of */
      /*   `root'.                                                    */
      root = rightchild->right;
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  allocmapinsertroot()   Finish inserting a node into a splay tree.        */
/*                                                                           */
/*  After a splay tree is splayed with allocmapsplay() on the coordinates    */
/*  (x, y, z), this procedure is used to finish inserting a node with point  */
/*  (x, y, z) into the splay tree.  The new node becomes the root of the     */
/*  tree.                                                                    */
/*                                                                           */
/*  treeroot:  The root of the tree in which to insert the new node, twice   */
/*    indirected.  On return, `*treeroot' will point to the new root.        */
/*  insertnode:  The node to insert into the tree.                           */
/*  splayside:  The value returned by allocmapsplay().  -1 if (x, y, z)      */
/*    precedes the node splayed to the root; 1 if (x, y, z) follows the node */
/*    splayed to the root; irrelevant if the tree is empty or the node       */
/*    splayed to the root is also (x, y, z).                                 */
/*                                                                           */
/*****************************************************************************/

void allocmapinsertroot(struct allocmapnode **treeroot,
                        struct allocmapnode *insertnode,
                        int splayside)
{
  /* Is the tree empty? */
  if (*treeroot == (struct allocmapnode *) NULL) {
    /* Empty tree; the new node has no children. */
    insertnode->left = (struct allocmapnode *) NULL;
    insertnode->right = (struct allocmapnode *) NULL;
  } else if (splayside < 0) {
    /* The new sample point precedes the root's sample point.  The root   */
    /*   becomes the new node's right child, and the root's (former) left */
    /*   child becomes the new node's left child.                         */
    insertnode->left = (*treeroot)->left;
    insertnode->right = *treeroot;
    (*treeroot)->left = (struct allocmapnode *) NULL;
  } else {
    /* The new sample point follows the root's sample point.  The root    */
    /*   becomes the new node's left child, and the root's (former) right */
    /*   child becomes the new node's right child.                        */
    insertnode->left = *treeroot;
    insertnode->right = (*treeroot)->right;
    (*treeroot)->right = (struct allocmapnode *) NULL;
  }
  /* The new node becomes the root. */
  *treeroot = insertnode;
}

/*****************************************************************************/
/*                                                                           */
/*  allocmapremoveroot()   Removes the root from a splay tree.               */
/*                                                                           */
/*  Does nothing if the tree is empty.                                       */
/*                                                                           */
/*  treeroot:  The root of the tree in which to insert the new node, twice   */
/*    indirected.  On return, `*treeroot' will point to the new root.        */
/*                                                                           */
/*****************************************************************************/

void allocmapremoveroot(struct allocmapnode **treeroot)
{
  struct allocmapnode *root;
  struct allocmapnode *left;
  struct allocmapnode *right;

  /* Do nothing if the tree is empty. */
  if (*treeroot != (struct allocmapnode *) NULL) {
    /* Remove the root from the tree. */
    root = *treeroot;
    left = root->left;
    right = root->right;
    if (left == (struct allocmapnode *) NULL) {
      /* Root has no left child, so its right child becomes the new root. */
      *treeroot = right;
    } else {
      *treeroot = left;
      /* Did the old root have a right child? */
      if (right != (struct allocmapnode *) NULL) {
        /* The largest entry in the left subtree becomes the new root. */
        allocmapmax(treeroot);
        /* Attach the former right subtree. */
        (*treeroot)->right = right;
      }
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  allocmapindex()   Find the node in an allocation tree whose sample point */
/*                    is "closest" to (x, y, z), and return its index.       */
/*                                                                           */
/*  If the tree contains a node whose sample point is (x, y, z), then that   */
/*  node's index is returned.  Otherwise, returns the node whose sample      */
/*  point is immediately before or after (x, y, z) in the z-order.  If there */
/*  are two choices (the node immediately before vs. the node immediately    */
/*  after), selects the nearest one by Euclidean distance.                   */
/*                                                                           */
/*  tree:  The allocmap in which to search.                                  */
/*  x, y, z:  The coordinates of the point whose near neighbor is sought.    */
/*                                                                           */
/*  Returns the index of the found node.  If the tree is empty, returns      */
/*    zero, which acts as a default index.                                   */
/*                                                                           */
/*****************************************************************************/

proxipoolulong allocmapindex(struct allocmap *tree,
                             starreal x,
                             starreal y,
                             starreal z)
{
  struct allocmapnode *foundnode;
  struct allocmapnode *othernode;
  int splayside;

  /* If the tree is empty, the default index is zero. */
  if (tree->allocroot == (struct allocmapnode *) NULL) {
    return 0;
  }

  /* Search for (x, y, z) and splay some node to the top. */
  splayside = allocmapsplay(&tree->allocroot, x, y, z, &othernode);
  foundnode = tree->allocroot;

  if ((splayside != 0) && (othernode != (struct allocmapnode *) NULL)) {
    /* Decide which node--`foundnode' or `othernode'--is closest to */
    /*   (x, y, z) by Euclidean distance.                           */
    if ((x - othernode->xsam) * (x - othernode->xsam) +
        (y - othernode->ysam) * (y - othernode->ysam) +
        (z - othernode->zsam) * (z - othernode->zsam) <
        (x - foundnode->xsam) * (x - foundnode->xsam) +
        (y - foundnode->ysam) * (y - foundnode->ysam) +
        (z - foundnode->zsam) * (z - foundnode->zsam)) {
      return othernode->index;
    }
  }

  return foundnode->index;
}

/*****************************************************************************/
/*                                                                           */
/*  allocmapnewpoint()   Add a new sample point (x, y, z) to the allocation  */
/*                       tree.                                               */
/*                                                                           */
/*  If the point (x, y, z) is already in the tree, this procedure does not   */
/*  insert a new one.                                                        */
/*                                                                           */
/*  tree:  The allocmap in which to insert the new point.                    */
/*  x, y, z:  The coordinates of the sample point to insert.                 */
/*                                                                           */
/*  Returns the index of the new node, or the index of the preexisting node  */
/*    if (x, y, z) is already in the tree.                                   */
/*                                                                           */
/*****************************************************************************/

proxipoolulong allocmapnewpoint(struct allocmap *tree,
                                starreal x,
                                starreal y,
                                starreal z)
{
  struct allocmapnode *newnode;
  struct allocmapnode *othernode;
  int splayside = 0;

  /* If the tree is not empty, splay the tree to find a good spot to insert */
  /*   the new node.                                                        */
  if (tree->allocroot != (struct allocmapnode *) NULL) {
    splayside = allocmapsplay(&tree->allocroot, x, y, z, &othernode);
    if (splayside == 0) {
      /* The point is already in the tree. */
      return tree->allocroot->index;
    }
  }

  /* Allocate a node to hold the new sample point. */
  if (tree->spareroot == (struct allocmapnode *) NULL) {
    /* No freed nodes left.  Allocate a brand new one. */
    newnode = allocmapnewnode(tree);
  } else {
    /* Find a freed node whose sample point was near (x, y, z) and */
    /*   remove it from the spare tree.                            */
    splayside = allocmapsplay(&tree->spareroot, x, y, z, &othernode);
    newnode = tree->spareroot;
    if ((splayside != 0) && (othernode != (struct allocmapnode *) NULL)) {
      /* Decide which node--`newnode' or `othernode'--is closest to */
      /*   (x, y, z) by Euclidean distance.                         */
      if ((x - othernode->xsam) * (x - othernode->xsam) +
          (y - othernode->ysam) * (y - othernode->ysam) +
          (z - othernode->zsam) * (z - othernode->zsam) <
          (x - newnode->xsam) * (x - newnode->xsam) +
          (y - newnode->ysam) * (y - newnode->ysam) +
          (z - newnode->zsam) * (z - newnode->zsam)) {
        allocmapsplay(&tree->spareroot, othernode->xsam, othernode->ysam,
                      othernode->zsam, &othernode);
        newnode = tree->spareroot;
      }
    }

    allocmapremoveroot(&tree->spareroot);
  }
  newnode->xsam = x;
  newnode->ysam = y;
  newnode->zsam = z;

  /* The new node becomes the root of the allocation tree. */
  allocmapinsertroot(&tree->allocroot, newnode, splayside);

  return newnode->index;
}

/*****************************************************************************/
/*                                                                           */
/*  allocmapdeletepoint()   Deletes a sample point (x, y, z) from the        */
/*                          allocation tree.                                 */
/*                                                                           */
/*  Does nothing if the point (x, y, z) is not actually in the tree.         */
/*                                                                           */
/*  The allocation index associated with (x, y, z) is placed in a "spare     */
/*  tree," so it can be reused--hopefully associated with a new point that   */
/*  is close to (x, y, z).                                                   */
/*                                                                           */
/*  tree:  The allocmap from which to delete the point.                      */
/*  x, y, z:  The coordinates of the sample point to delete.                 */
/*                                                                           */
/*****************************************************************************/

void allocmapdeletepoint(struct allocmap *tree,
                         starreal x,
                         starreal y,
                         starreal z)
{
  struct allocmapnode *deletenode;
  struct allocmapnode *dummynode;
  int splayside;

  /* Do nothing if the tree is empty. */
  if (tree->allocroot != (struct allocmapnode *) NULL) {
    /* Try to splay the point (x, y, z) to the root. */
    splayside = allocmapsplay(&tree->allocroot, x, y, z, &dummynode);

    /* Do nothing if the node is not found. */
    if (splayside == 0) {
      /* Remember the deleted node. */
      deletenode = tree->allocroot;
      /* Remove it from the tree. */
      allocmapremoveroot(&tree->allocroot);

      /* If the spare tree is not empty, splay it to find a good spot to */
      /*   insert the deleted node.                                      */
      if (tree->spareroot != (struct allocmapnode *) NULL) {
        /* Find where to insert (x, y, z) in the spare tree. */
        splayside = allocmapsplay(&tree->spareroot, x, y, z, &dummynode);
      }
      /* The deleted node becomes the root of the spare tree, ready to be */
      /*   reused.                                                        */
      allocmapinsertroot(&tree->spareroot, deletenode, splayside);
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  allocmapbytes()   Returns the size (in bytes) of the allocation tree.    */
/*                                                                           */
/*  Returns the dynamically allocated memory used by the arraypool used to   */
/*  store the nodes of the tree.  Does not include the size of the `struct   */
/*  allocmap', which is presumably part of some other object and accounted   */
/*  for there.  Note that allocmaps only release memory to the operating     */
/*  system when allocmapdeinit() is called, and the return value includes    */
/*  the memory occupied by all the spare nodes.                              */
/*                                                                           */
/*  tree:  The allocmap in question.                                         */
/*                                                                           */
/*  Returns the number of dynamically allocated bytes in `tree'.             */
/*                                                                           */
/*****************************************************************************/

arraypoolulong allocmapbytes(struct allocmap *tree)
{
  return arraypoolbytes(&tree->nodearray);
}

/**                                                                         **/
/**                                                                         **/
/********* Allocation map routines end here                          *********/


/********* Link ring routines begin here                             *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  Compressed link rings                                                    */
/*                                                                           */
/*  The link of a vertex in a 2D triangulation, and the link of an edge in   */
/*  a 3D triangulation, is a set of vertices and edges ordered circularly    */
/*  around the vertex/edge that defines the triangles/tetrahedra that        */
/*  include the vertex/edge.  I call these one-dimensional links "link       */
/*  rings", distinguishing them from higher-dimensional links like the link  */
/*  of a vertex in a 3D triangulation (which is a 2D triangulation).         */
/*                                                                           */
/*  Of course, a one-dimensional link is only truly a ring for a vertex/edge */
/*  in the interior of a triangulation.  A boundary vertex/edge has one or   */
/*  more gaps in its ring, so its link is comprised of one or more chains.   */
/*  I fill in the gaps with a "ghost vertex" whose sole job is to represent  */
/*  such gaps, so the link is always represented as a ring.                  */
/*                                                                           */
/*  Link rings should all be thought of as sharing a fixed "orientation".    */
/*  I use counterclockwise, but this code would never know if you reflected  */
/*  it.  In fact, this code is deliberately oblivious to geometry--it never  */
/*  looks at a coordinate.                                                   */
/*                                                                           */
/*  Link rings are stored in a compressed format adapted from Blandford,     */
/*  Blelloch, Cardoze, and Kadow.  See the header for a full citation.       */
/*                                                                           */
/*  Each link ring is stored in a linked list of list nodes of type          */
/*  `molecule'.  Molecules are allocated dynamically and constitute most of  */
/*  the memory occupied by a mesh.                                           */
/*                                                                           */
/*  Some invariants of link rings:                                           */
/*                                                                           */
/*  - Both vertices and link rings are represented by tags, which are        */
/*    defined as a part of the proxipool definitions.  (But vertices and     */
/*    links are allocated from different proxipools, so a vertex tag of 31   */
/*    and a link tag of 31 are unrelated to each other.)                     */
/*                                                                           */
/*  - A molecule is a short piece of memory.  Every molecule is the same     */
/*    length.  Every molecule has a tag (and is allocated from a proxipool). */
/*    Every link ring tag indexes the first molecule in the link ring.       */
/*                                                                           */
/*  - Molecules store compressed tags.  Each compressed tag is partitioned   */
/*    into 7-bit chunks called "atoms", ordered from most significant to     */
/*    least significant.  The high-order eighth bit of each atom is a "stop  */
/*    bit", set to one to signify the final atom of a compressed tag.        */
/*                                                                           */
/*  - Compressed tags vary in length.  The compression mechanism is simply   */
/*    to leave out the high-order atoms of the tag.  Some number of low-     */
/*    order atoms remain.  The number of atoms that remain depends on which  */
/*    atoms of the tag differ from the atoms of a "ground tag" used as the   */
/*    basis for compression.  Compressed vertex tags use a different ground  */
/*    tag than compressed molecule tags.                                     */
/*                                                                           */
/*  - The high-order bits of a compressed vertex are supplied by the ground  */
/*    tag, whose identity is fixed for any single link ring.  Typically, the */
/*    ground tag is the tag of the topmost vertex in the hierarchy.  For     */
/*    instance, if a vertex x has a 2D link, in which a vertex y has a link  */
/*    ring (representing the link of the edge xy), x is generally the ground */
/*    tag.  Hopefully, most of the vertices in x's link share a long prefix  */
/*    with x.                                                                */
/*                                                                           */
/* - At the end (high-index atoms) of every molecule, there is a compressed  */
/*    tag indexing the next molecule in the linked list of molecules.  This  */
/*    compressed tag is written BACKWARD, starting at index MOLECULESIZE - 1,*/
/*    so the least significant atom (whose stop bit is set) has the lowest   */
/*    index of the tag's atoms.  If a molecule is the last one in the linked */
/*    list, its "next molecule" tag is the STOP atom, which occupies just    */
/*    one byte.                                                              */
/*                                                                           */
/*  - The high-order bits of a compressed "next molecule" tag are supplied   */
/*    by the previous tag in the linked list.  (There is not a single        */
/*    "ground tag" as there is for vertex tags.)  This means that "next      */
/*    tags" are often just one byte long, because successive molecules often */
/*    have successive tags.  It also suggests that appending tags to the end */
/*    of a list is more pleasant than splicing them into the middle, which   */
/*    can reduce space efficiency by lengthening the "next molecule" tags.   */
/*                                                                           */
/*  - There are two special atoms, STOP and GHOSTVERTEX (both with their     */
/*    stop bits on).  To prevent confusion, no ordinary tag can compress to  */
/*    either of these.  A tag whose last atom matches STOP or GHOSTVERTEX is */
/*    distinguished by having at least two atoms in its compressed           */
/*    representation (even if it would otherwise compress to one atom).      */
/*                                                                           */
/*  - All bytes of a molecule not occupied by the "next molecule" tag are    */
/*    available for compressed vertices, which are written FORWARD from      */
/*    index zero.  In the last molecule of a linked list, some bytes might   */
/*    be left unused, in which case the last vertex byte must be followed by */
/*    a STOP atom to indicate that not all the bytes are used.  This is only */
/*    permitted in a molecule whose "next molecule" tag is STOP.  If there   */
/*    are no unused bytes in the last molecule of a linked list, then the    */
/*    STOP atom at position MOLECULESIZE - 1 of that molecule does double    */
/*    duty as both the "next molecule" tag and the vertex list terminator.   */
/*                                                                           */
/*  - The compressed tag pointing to the next molecule may occupy at most    */
/*    MOLECULESIZE - 1 bytes (atoms), leaving at least one byte for part of  */
/*    a compressed vertex tag.  (The default molecule size of 16 ought to    */
/*    leave a lot more space for compressed vertices than one byte, but      */
/*    perhaps in some huuuuuge mesh on a 128-bit machine of the future...)   */
/*                                                                           */
/*  - The boundaries between molecules are not related to the boundaries     */
/*    between compressed vertices.  A molecule normally holds several        */
/*    compressed vertices, and a compressed vertex might span two molecules  */
/*    (or more, if the tags are big enough and the molecules small enough).  */
/*                                                                           */
/*  - A link ring should not have any vertex appear twice except the         */
/*    GHOSTVERTEX.  This "invariant" isn't really an invariant, because some */
/*    of the procedures allow you to violate it, and the Bowyer-Watson       */
/*    implementation relies on being able to temporarily have multiple       */
/*    copies of a vertex in a link ring.  (They eventually get merged back   */
/*    into one by linkringdelete2vertices().)  However, this invariant       */
/*    ought to be restored by any algorithm upon completion.                 */
/*                                                                           */
/*  - A link ring cannot have two GHOSTVERTEXs consecutively.                */
/*                                                                           */
/*  - A link ring cannot have exactly one vertex (counting ghost vertices).  */
/*                                                                           */
/*  - A molecule cannot start (index 0) with the STOP vertex, unless the     */
/*    link ring is empty (and this is the sole molecule in the linked list). */
/*    Except in this circumstance, every molecule stores at least one atom   */
/*    from a compressed vertex.  However, the STOP _atom_ can appear at      */
/*    index 0, so long as it is the continuation of a compressed tag from    */
/*    the previous molecule (and therefore it doesn't mean STOP).            */
/*                                                                           */
/*  Here's what a molecule looks like.  Each column is an atom.              */
/*                                                                           */
/*  ------------------------H--------                                        */
/*  |0|0|1|1|0|1|1|0|0|0|1|0H1|0|0|0| <-- Stop bits.  The "next molecule"    */
/*  |1|1|0|0|1|0|1|1|1|0|0|1H0|1|1|0|     tag is written backward and        */
/*  |0|1|1|1|0|0|1|0|1|1|1|1H0|1|1|0|     terminated with a stop bit.  The   */
/*  |1|1|1|0|1|0|1|1|0|1|0|0H0|0|0|1|     vertex tags are written forward    */
/*  |1|0|0|1|1|0|1|1|1|0|1|1H0|0|0|0|     and terminated with a stop bit.    */
/*  |1|1|0|0|1|0|0|0|0|0|0|0H0|1|1|0|     In this example, the last vertex   */
/*  |0|1|1|1|1|0|1|1|1|1|1|1H0|1|0|1|     tag is continued in the next       */
/*  |1|1|0|0|1|1|1|1|1|0|0|1H0|0|1|0|     molecule.                          */
/*  ------------------------H--------                                        */
/*   \_____________________/ \_____/                                         */
/*     compressed vertices    next molecule tag (varies in width)            */
/*                                                                           */
/*  It is permissible to have a link ring with just two vertices (and no     */
/*  ghost vertex).  A link ring with vertices 1 and 2 represents two edges,  */
/*  1-2 and 2-1, which are distinguished by the vertex order (underscoring   */
/*  the importance of a link ring's orientation).                            */
/*                                                                           */
/*  It's also permissible to have a link ring with one "real" vertex and one */
/*  ghost vertex.  This link has no edges at all.  A link can have any       */
/*  number of dangling vertices that are not connected to edges, although    */
/*  I haven't included much support for creating these.  (You can do it by   */
/*  creating an edge, then deleteing one of its vertices.)                   */
/*                                                                           */
/*  The "correct" way to use the linkring interface to build link rings is   */
/*  to use linkringinsertedge() and linkringdeleteedge().  These procedures  */
/*  won't allow you to build invariant-defying link rings.  The procedures   */
/*  that insert and delete individual vertices can accomplish some things    */
/*  faster--inserting a vertex performs a 1 -> 2 bistellar flip (replacing   */
/*  one edge with two), and deleting a vertex performs a 2 -> 1 flip.  But   */
/*  they also allow you to violate invariants; for instance, you could       */
/*  create a link ring with just one vertex by deleting the others.  The     */
/*  Bowyer-Watson implementation exploits the fact that you can insert       */
/*  multiple copies of a vertex into a link ring, but it's careful to clean  */
/*  up after itself in the end.                                              */
/*                                                                           */
/*  Public interface:                                                        */
/*  GHOSTVERTEX   Tag representing a ghost vertex.                           */
/*  STOP   Tag representing an unsuccessful query.                           */
/*  struct linkposition   Represents a position in a link ring or 2D link.   */
/*  tag linkringnew(pool, allocindex)   Allocate a new, empty link ring.     */
/*  tag linkringnewfill(pool, groundtag, tagarray, tagarraylen, allocindex)  */
/*    Allocate a new link ring containing the tags specified in the input.   */
/*  void linkringrestart(pool, linkring)   Reset a link ring to empty.       */
/*  void linkringdelete(pool, linkring)   Free a link ring to the pool.      */
/*  int linkringadjacencies(pool, linkring, groundtag, searchvertex,         */
/*    adjacencies[2])   Read the two vertices neighboring a vertex in link.  */
/*  void linkringiteratorinit(pool, linkring, pos)   Initialize an iterator  */
/*    that traverses all the vertices in a link ring one by one.             */
/*  tag linkringiterate(pos)   Read and advance iterator.                    */
/*  void linkringprint(pool, linkring, groundtag)   Print a link ring.       */
/*  int linkringinsertedge(pool, linkring, groundtag, endpoint1, endpoint2)  */
/*    Insert an edge into a link ring.                                       */
/*  int linkringdeleteedge(pool, linkring, groundtag, endpoint1, endpoint2)  */
/*    Delete an edge from a link ring.                                       */
/*  int linkringinsertvertex(pool, linkring, groundtag, searchvertex,        */
/*    newvertex)   Insert a vertex in link, following a specified vertex.    */
/*  int linkringdeletevertex(pool, linkring, groundtag, deletevertex)        */
/*    Delete a vertex from a link ring.                                      */
/*  int linkringdelete2vertices(pool, linkring, groundtag, deletevertex)     */
/*    Delete a vertex and the vertex that follows it from a link ring.       */
/*                                                                           */
/*  For internal use only:                                                   */
/*  struct linkpossmall   Represents a position in a link ring or 2D link.   */
/*  int linkringtagcompress(groundtag, newtag, newtagatoms)                  */
/*  void linkringreadtag(moleculetag, cule, atomindex, nextmoleculetag,      */
/*    nowatom, errstring)                                                    */
/*  int linkringadjacencies2(pool, linkring, groundtag, searchvertex,        */
/*    adjacencies[2])                                                        */
/*  tag linkringinsertatoms(pool, insertposition, numatoms, newatombuffer,   */
/*    allocindex)                                                            */
/*  tag linkringinsertatoms2(pool, insertposition, numatoms, newatombuffer,  */
/*    allocindex)                                                            */
/*  tag linkringdeleteatoms(pool, deleteposition, numatoms)                  */
/*  tag linkringdeleteatoms2(pool, deleteposition, numatoms)                 */
/*  void linkringrotateatoms(pool, linkring, pos1, pos2)                     */
/*                                                                           */
/*****************************************************************************/


/*  `molecule' is a short piece of memory, with room for MOLECULESIZE atoms  */
/*  (characters).  Link rings are made up of molecules chained together in   */
/*  linked lists.  Each molecule has a compressed tag at the end which       */
/*  points to the next molecule in the list.  The rest of its space is       */
/*  devoted to compressed tags indexing vertices in the link ring.           */
/*                                                                           */
/*  2D links are also made of chains of molecules, with the compressed tags  */
/*  indexing both vertices and link rings.                                   */
/*                                                                           */
/*  The declaration "typedef char molecule[MOLECULESIZE]" would have been    */
/*  more appropriate, if C interpreted that as a reassignable pointer to an  */
/*  array of size MOLECULESIZE.  All molecules are the same size.            */

typedef char *molecule;


/*  GHOSTVERTEX is a tag that represents a "ghost vertex," which represents  */
/*  a gap in a link ring or a simplicial complex.  If a ghost vertex lies    */
/*  between vertices v and w in a link ring, it means that vw is not an edge */
/*  of the link, even though v and w are successive vertices in the link.    */
/*  To give another example, if a tetrahedron has a face f not shared by     */
/*  another tetrahedron in the simplicial complex, we represent it as        */
/*  sharing a face with a "ghost tetrahedron" whose fourth vertex is the     */
/*  ghost vertex.  Cast to a character, GHOSTVERTEX is also an atom that     */
/*  serves as the compressed form of the GHOSTVERTEX tag.                    */
/*                                                                           */
/*  STOP is a tag used to terminate a sequence of atoms in a molecule,       */
/*  implying either that the atoms continue in the next molecule, or there   */
/*  are no more atoms in the link ring.  STOP is also used as a "next tag"   */
/*  index at the end of a molecule, to indicate that it is the last molecule */
/*  in the linked list.  The STOP tag is not part of the interface of any of */
/*  the linkring procedures, but it is returned by some of the link2d        */
/*  procedures.  Cast to a character, STOP is the one-atom compressed form   */
/*  of the STOP tag.                                                         */

#define GHOSTVERTEX (~ (tag) 0)
#define STOP (~ (tag) 1)

/*  MOLECULESIZE is the size of one molecule in characters (and atoms).      */
/*  The choice makes a trade-off between wasted space (most compressed link  */
/*  rings do not use the entirely of the last molecule in their linked       */
/*  lists) and space occupied by the compressed "next molecule tag" index    */
/*  (which is space that can't be used for compressed vertices).  To obtain  */
/*  high speed, 20 seems to be a good choice.  (Higher values don't buy much */
/*  more speed, but cost a lot of memory.)  To obtain maximum compactness,   */
/*  8 is a good choice.                                                      */

#define MOLECULESIZE 20

/*  MOLECULEQUEUESIZE is the number of molecules that linkringdeleteatoms()  */
/*  can remember at once.  linkringdeleteatoms() should not be asked to      */
/*  delete more than this many molecules' atoms at once.  Fortunately, the   */
/*  maximum number of atoms that any procedure wants to delete at once is no */
/*  more than three compressed vertices' worth.                              */

#define MOLECULEQUEUESIZE 40

/*  COMPRESSEDTAGLENGTH is an upper bound on the maximum number of bytes a   */
/*  tag occupies after it is compressed.  Used to allocate buffers for       */
/*  atoms in transit.                                                        */

#define COMPRESSEDTAGLENGTH (8 * sizeof(tag) / 7 + 2)


/*  A linkposition represents an atom in a link ring or 2D link.  Usually,   */
/*  the atom is the first atom in some compressed vertex, or the STOP tag at */
/*  the end of the linked list.  Among other things, this struct is useful   */
/*  as an iterator that walks through a link ring or 2D link.                */
/*                                                                           */
/*  The atom in question appears at index `textindex' in molecule `cule',    */
/*  usually.  However, if textindex > lasttextindex, the atom in question is */
/*  really the first atom of the next molecule in the linked list.  This may */
/*  seem oblique, but it's sometimes useful.  For instance, when deleting    */
/*  atoms at a given position in a link ring, this representation increases  */
/*  the likelihood of being able to free an additional molecule at the end   */
/*  of the linked list for reuse elsewhere.                                  */
/*                                                                           */
/*  `moleculetag' is the tag of the current molecule (which is needed for    */
/*  use as a base for compressing the next tag when a new molecule gets      */
/*  appended to the linked list).  `lasttextindex' is the index of the last  */
/*  index in this tag used for compressed vertices (as opposed to the "next  */
/*  molecule tag").  `nextmoleculetag' is the tag of the next molecule in    */
/*  the linked list (possibly STOP).  These two fields are stored so that    */
/*  they only need to be computed once, when `cule' is first encountered.    */

struct linkposition {
  molecule cule;      /* The molecule containing the atom (or right before). */
  int textindex;                                   /* The index of the atom. */
  int lasttextindex;   /* The last atom not part of the "next molecule" tag. */
  tag moleculetag;                       /* The tag for the molecule `cule'. */
  tag nextmoleculetag;  /* The next tag following `moleculetag' in the list. */
  tag groundtag;            /* Tag for the link's owner, used to decompress. */
  struct proxipool *pool;  /* The pool in which the molecules are allocated. */
};

/*  A smaller version of a linkposition, used internally only.               */

struct linkpossmall {
  molecule cule;      /* The molecule containing the atom (or right before). */
  int textindex;                                   /* The index of the atom. */
  int lasttextindex;   /* The last atom not part of the "next molecule" tag. */
  tag moleculetag;                       /* The tag for the molecule `cule'. */
  tag nextmoleculetag;  /* The next tag following `moleculetag' in the list. */
};


/*****************************************************************************/
/*                                                                           */
/*  linkringtagcompress()   Compresses a tag, relative to another tag.       */
/*                                                                           */
/*  The compressed tag contains enough lower-order bits of the original      */
/*  tag to distinguish it from another tag, `groundtag'.  Decompression can  */
/*  be done later simply by using `groundtag' as the source of the missing   */
/*  high-order bits.                                                         */
/*                                                                           */
/*  The low-order bits are partitioned into atoms (7-bit chunks), and the    */
/*  high-order "stop bit" of the lowest-order atom is set.                   */
/*                                                                           */
/*  The compressed tag is always at least one atom long, even if `newtag' == */
/*  `groundtag'.  If `newtag' is STOP or GHOSTVERTEX, the compressed tag is  */
/*  just one atom long.  Otherwise, if the last atom of `newtag' is STOP or  */
/*  GHOSTVERTEX, then the compressed tag is at least two atoms long, so that */
/*  it cannot be mistaken for STOP or GHOSTVERTEX.                           */
/*                                                                           */
/*  IMPORTANT:  The atoms are written into the array `newtagatoms' in        */
/*  REVERSE order, with the least significant atom (with its stop bit set)   */
/*  at index zero.  (This is the right order for "next molecule" tags, but   */
/*  compressed vertices need to be reversed back.)                           */
/*                                                                           */
/*  groundtag:  The base tag against which `newtag' is compressed.           */
/*  newtag:  The tag to compress.                                            */
/*  newtagatoms:  The array into which the compressed tag's atoms are        */
/*    written BACKWARD.                                                      */
/*                                                                           */
/*  Returns the number of atoms in the compressed tag.                       */
/*                                                                           */
/*****************************************************************************/

int linkringtagcompress(tag groundtag,
                        tag newtag,
                        char *newtagatoms)
{
  int numatoms;

  /* Extract the least significant atom; set its stop bit. */
  newtagatoms[0] = (char) ((newtag & (tag) 127) | (tag) ~127);
  if ((newtag == GHOSTVERTEX) || (newtag == STOP)) {
    /* GHOSTVERTEX and STOP always compress to one atom. */
    return 1;
  }

  /* Cut the least significant atoms from the tag and the ground tag. */
  groundtag = groundtag >> 7;
  newtag = newtag >> 7;
  numatoms = 1;
  /* Keep cutting atoms until the two tags' remaining bits agree. */
  while (groundtag != newtag) {
    /* Extract the next atom. */
    newtagatoms[numatoms] = (char) (newtag & (tag) 127);
    /* Cut it off in anticipation of the next comparison. */
    groundtag = groundtag >> 7;
    newtag = newtag >> 7;
    numatoms++;
  }

  if ((numatoms == 1) &&
      ((newtagatoms[0] == (char) GHOSTVERTEX) ||
       (newtagatoms[0] == (char) STOP))) {
    /* To avoid confusion, any tag whose least significant atom matches the */
    /*   ghost vertex or STOP atom has to compress to at least two atoms.   */
    newtagatoms[1] = (char) (newtag & (tag) 127);
    numatoms++;
  }

  return numatoms;
}

/*****************************************************************************/
/*                                                                           */
/*  linkringnew()   Allocate a new, empty link ring.                         */
/*                                                                           */
/*  The parameters include an allocation index, used to determine where the  */
/*  new link ring will be stored in memory.  Link rings with the same        */
/*  allocation index go into common areas in memory.  The idea is to create  */
/*  spatial coherence:  links that are geometrically close to each other are */
/*  near each other in memory, too.                                          */
/*                                                                           */
/*  The parameters include the coordinates of a point associated with the    */
/*  link ring, so that the link ring can be allocated from memory near other */
/*  molecules that are spatially nearby.                                     */
/*                                                                           */
/*  pool:  The proxipool to allocate the link ring from.                     */
/*  allocindex:  An allocation index associated with the link ring.          */
/*                                                                           */
/*  Returns the tag of the first (and only) molecule of the new link ring.   */
/*                                                                           */
/*****************************************************************************/

tag linkringnew(struct proxipool *pool,
                proxipoolulong allocindex)
{
  molecule cule;
  tag newlinkring;

  /* Allocate a molecule to hold the (empty) link ring. */
  newlinkring = proxipoolnew(pool, allocindex, (void **) &cule);
  /* There are no vertices in this link ring. */
  cule[0] = (char) STOP;
  /* There is no next molecule. */
  cule[MOLECULESIZE - 1] = (char) STOP;
  return newlinkring;
}

/*****************************************************************************/
/*                                                                           */
/*  linkringnewfill()   Allocate a new link ring containing the tags         */
/*                      (vertices) specified in an input array.              */
/*                                                                           */
/*  The tags may include GHOSTVERTEX.  Be sure to obey the link ring         */
/*  invariants:  no vertex except GHOSTVERTEX may occur twice in a link      */
/*  ring, and GHOSTVERTEX may not occur twice consecutively.  (Remember      */
/*  that the link ring is a ring, so GHOSTVERTEX may not occur both first    */
/*  and last.)                                                               */
/*                                                                           */
/*  pool:  The proxipool to allocate the link ring from.                     */
/*  groundtag:  The ground tag that serves as a base for the compression of  */
/*    the other tags.                                                        */
/*  tagarray:  An array listing the tags to put in the link ring.            */
/*  tagarraylen:  The number of tags to put in the link ring.                */
/*  allocindex:  An allocation index associated with the link ring.          */
/*                                                                           */
/*  Returns the tag of the first molecule of the link ring.                  */
/*                                                                           */
/*****************************************************************************/

tag linkringnewfill(struct proxipool *pool,
                    tag groundtag,
                    tag *tagarray,
                    proxipoolulong tagarraylen,
                    proxipoolulong allocindex)
{
  molecule cule;
  molecule nextcule;
  tag newlinkring;
  tag moleculetag;
  tag nextculetag;
  proxipoolulong tagarrayindex;
  int vertexatomcount;
  int vertexwriteindex;
  int vertexcopyindex;
  int vertexleftindex;
  int tagsize;
  int nextculeindex;
  int i;
  char atombuffer[MOLECULESIZE + 3 * COMPRESSEDTAGLENGTH];
  unsigned int aftervertexindex[MOLECULESIZE + 3 * COMPRESSEDTAGLENGTH];

  /* Allocate the first molecule of the new link ring. */
  newlinkring = proxipoolnew(pool, allocindex, (void **) &cule);
  moleculetag = newlinkring;

  /* Compress the first tag (if there is any) into `atombuffer'. */
  vertexatomcount = tagarraylen <= 0 ? 0 :
                    linkringtagcompress(groundtag, tagarray[0], atombuffer);
  /* Compressed tags are written backward by linkringtagcompress(), so the */
  /*   compressed tag will be copied from index `vertexcopyindex' back to  */
  /*   index `vertexleftindex' (presently zero).                           */
  vertexleftindex = 0;
  vertexcopyindex = vertexatomcount - 1;
  /* Keep track of where this compressed vertex ends, and the second vertex */
  /*   will begin.                                                          */
  aftervertexindex[0] = vertexatomcount;
  vertexwriteindex = vertexatomcount;
  /* The second vertex has index one. */
  tagarrayindex = 1;
  nextcule = (molecule) NULL;
  nextculetag = 0;

  /* The outer loop fills one molecule per iteration. */
  do {
    /* The next loop compresses enough tags to yield MOLECULESIZE atoms */
    /*   (unless we run out of tags).                                   */
    while ((tagarrayindex < tagarraylen) && (vertexatomcount < MOLECULESIZE)) {
      /* Compress the next tag into the buffer. */
      tagsize = linkringtagcompress(groundtag, tagarray[tagarrayindex],
                                    &atombuffer[vertexwriteindex]);
      tagarrayindex++;
      /* Keep track of the number of atoms waiting to be put into link ring. */
      vertexatomcount += tagsize;
      /* Keep track of where this compressed vertex ends. */
      aftervertexindex[vertexwriteindex] = vertexwriteindex + tagsize;
      vertexwriteindex += tagsize;
      /* If too close to the end of the buffer, return to the beginning */
      /*   for the next vertex.                                         */
      if (vertexwriteindex > MOLECULESIZE + 2 * COMPRESSEDTAGLENGTH) {
        vertexwriteindex = 0;
      }
    }

    /* Check if this is the last molecule in the link ring. */
    if (vertexatomcount >= MOLECULESIZE) {
      /* Too many atoms left to fit in one molecule.  Allocate another one. */
      nextculetag = proxipoolnew(pool, allocindex, (void **) &nextcule);
      /* Compress the "next molecule" tag and figure where to put it. */
      nextculeindex = MOLECULESIZE -
                      linkringtagcompress(moleculetag, nextculetag,
                                          (char *) cule);
      /* Shift the compressed tag from the left to the right side of `cule'. */
      for (i = nextculeindex; i < MOLECULESIZE; i++) {
        cule[i] = cule[i - nextculeindex];
      }
    } else {
      /* Write a STOP atom after the last vertex. */
      cule[vertexatomcount] = (char) STOP;
      /* Write a STOP atom for the "next molecule" tag. */
      cule[MOLECULESIZE - 1] = (char) STOP;
      /* Set up `nextculeindex' for the upcoming loop. */
      nextculeindex = vertexatomcount;
    }

    /* Fill the rest of the molecule with atoms. */
    for (i = 0; i < nextculeindex; i++) {
      cule[i] = atombuffer[vertexcopyindex];
      /* Tags are compressed backward, so we reverse them back to forward. */
      vertexcopyindex--;
      /* Have we just written the last atom of a vertex? */
      if (vertexcopyindex < vertexleftindex) {
        /* Yes.  Jump to the next vertex. */
        vertexleftindex = aftervertexindex[vertexleftindex];
        /* If too close to the end of the buffer, return to the beginning */
        /*   (following the process that wrote the buffer).               */
        if (vertexleftindex > MOLECULESIZE + 2 * COMPRESSEDTAGLENGTH) {
          vertexleftindex = 0;
        }
        /* Find the rightmost atom of this compressed vertex. */
        vertexcopyindex = aftervertexindex[vertexleftindex] - 1;
      }
    }
    /* How many atoms in the buffer still await copying? */
    vertexatomcount -= nextculeindex;

    /* Go on to the next molecule in the next iteration. */
    cule = nextcule;
    moleculetag = nextculetag;
  } while (vertexatomcount > 0);

  return newlinkring;
}

/*****************************************************************************/
/*                                                                           */
/*  linkringreadtag()   Read the "next molecule" tag from a molecule.        */
/*                                                                           */
/*  This macro is a sequence of operations repeated in a bunch of the        */
/*  linkring procedures.  It's repeated often enough that it's worth         */
/*  inlining; hence, I've made it a macro.                                   */
/*                                                                           */
/*  moleculetag:  Used as a ground tag for decompressing `nextmoleculetag'   */
/*    (input, not changed).                                                  */
/*  cule:  The molecule to read (input, not changed).                        */
/*  atomindex:  Output only, whereupon it is the last index devoted to       */
/*    compressed vertices (i.e. one before the first index devoted to the    */
/*    compressed "next molecule" tag) in `cule'.                             */
/*  nextmoleculetag:  Output only, whereupon it is the tag for the next      */
/*    molecule after `cule' in the linked list.                              */
/*  nowatom:  A work variable; neither input nor output.                     */
/*  errstring:  A string to print if there's an unrecoverable error.         */
/*                                                                           */
/*****************************************************************************/

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

/*****************************************************************************/
/*                                                                           */
/*  linkringrestart()   Reset a link ring to empty.                          */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from.              */
/*  linkring:  Tag for the first molecule in the link ring.                  */
/*                                                                           */
/*****************************************************************************/

void linkringrestart(struct proxipool *pool,
                     tag linkring)
{
  molecule cule;
  tag nextmoleculetag;
  tag moleculetag;
  int atomindex;
  char nowatom;

  if (linkring >= STOP) {
    return;
  }

  moleculetag = linkring;

  /* Loop through the molecules and free all but the first. */
  do {
    /* Convert the tag to a molecule. */
    cule = (molecule) proxipooltag2object(pool, moleculetag);
    /* Read the "next molecule" tag. */
    linkringreadtag(moleculetag, cule, atomindex, nextmoleculetag, nowatom,
                    "Internal error in linkringrestart():\n");
    if (moleculetag == linkring) {
      /* There are no vertices in the first link ring. */
      cule[0] = (char) STOP;
      /* There is no next molecule. */
      cule[MOLECULESIZE - 1] = (char) STOP;
    } else {
      /* Free the molecule. */
      proxipoolfree(pool, moleculetag);
    }
    moleculetag = nextmoleculetag;
  } while (nextmoleculetag != STOP);
}

/*****************************************************************************/
/*                                                                           */
/*  linkringdelete()   Free all the molecules in a link ring to the pool.    */
/*                                                                           */
/*  pool:  The proxipool that the molecules were allocated from.             */
/*  linkring:  Tag for the first molecule in the link ring.                  */
/*                                                                           */
/*****************************************************************************/

void linkringdelete(struct proxipool *pool,
                    tag linkring)
{
  molecule cule;
  tag nextmoleculetag;
  tag moleculetag;
  int atomindex;
  char nowatom;

  if (linkring >= STOP) {
    return;
  }

  moleculetag = linkring;

  /* Loop through the molecules and free them. */
  do {
    /* Convert the tag to a molecule. */
    cule = (molecule) proxipooltag2object(pool, moleculetag);
    /* Read the "next molecule" tag. */
    linkringreadtag(moleculetag, cule, atomindex, nextmoleculetag, nowatom,
                    "Internal error in linkringdelete():\n");
    /* Free the molecule. */
    proxipoolfree(pool, moleculetag);
    moleculetag = nextmoleculetag;
  } while (nextmoleculetag != STOP);
}

/*****************************************************************************/
/*                                                                           */
/*  linkringadjacencies()    Return the two neighboring vertices adjoining a */
/*  linkringadjacencies2()   vertex in a link ring.                          */
/*                                                                           */
/*  These procedures are interchangeable.  linkringadjacencies2() is a       */
/*  complete implementation.  linkringadjacencies() is an optimized wrapper  */
/*  that handles the special case where the entire link ring fits in a       */
/*  single molecule faster, and calls linkringadjacencies2() otherwise.      */
/*  Profiling suggests that when MOLECULESIZE is 20, linkringadjacencies()   */
/*  resorts to calling linkringadjacencies2() only 2.5% of the time.         */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from.              */
/*  linkring:  Tag for the first molecule in the link ring.                  */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  searchvertex:  The tag of the vertex to search for.                      */
/*  adjacencies[2]:  Array in which the procedure returns the tags of the    */
/*    vertices adjacent to `searchvertex'.  adjacencies[0] is just before    */
/*    (clockwise from) `searchvertex', and adjacencies[1] is just after      */
/*    (counterclockwise from) `searchvertex'.  One or both of these may be   */
/*    GHOSTVERTEX, if `searchvertex' does not participate in two edges of    */
/*    the link ring.  The contents of this array do not need to be           */
/*    initialized prior to calling this procedure.                           */
/*                                                                           */
/*  Returns 1 if `searchvertex' is in the link ring; 0 otherwise.            */
/*                                                                           */
/*****************************************************************************/

int linkringadjacencies2(struct proxipool *pool,
                         tag linkring,
                         tag groundtag,
                         tag searchvertex,
                         tag adjacencies[2])
{
  molecule cule;
  tag vertextag;
  tag firstvertextag;
  tag prevvertextag;
  tag nextmoleculetag;
  tag moleculetag;
  long vertexcount;
  int vertexatoms;
  int atomindex1, atomindex2;
  char nowatom;

  if (linkring >= STOP) {
    /* Not a link ring. */
    adjacencies[0] = GHOSTVERTEX;
    adjacencies[1] = GHOSTVERTEX;
    return 0;
  }

  /* There is no "previous" vertex we've visited yet. */
  firstvertextag = STOP;
  prevvertextag = STOP;
  vertexcount = 0;
  vertextag = 0;
  vertexatoms = 0;
  /* Start at the first molecule. */
  nextmoleculetag = linkring;

  /* Loop through the linked list of molecules. */
  do {
    /* Convert the molecule's tag to a pointer. */
    cule = (molecule) proxipooltag2object(pool, nextmoleculetag);
    moleculetag = nextmoleculetag;
    /* Read the "next molecule" tag. */
    linkringreadtag(moleculetag, cule, atomindex2, nextmoleculetag, nowatom,
                    "Internal error in linkringadjacencies():\n");

    atomindex1 = 0;
    /* Loop through the atoms, stopping at the "next molecule" tag. */
    while (atomindex1 <= atomindex2) {
      nowatom = cule[atomindex1];
      /* Append the atom to the vertex tag. */
      vertextag = (vertextag << 7) + (nowatom & (char) 127);
      vertexatoms++;
      atomindex1++;

      /* Is this the last atom in the compressed tag? */
      if (nowatom < (char) 0) {
        /* Yes; we have an uncompressed tag now.  What tag is it? */
        if ((nowatom == (char) STOP) && (vertexatoms == 1)) {
          /* STOP tag.  Exit the inner loop early; go on to next molecule. */
          break;
        } else if ((nowatom == (char) GHOSTVERTEX) && (vertexatoms == 1)) {
          /* Ghost vertex. */
          vertextag = GHOSTVERTEX;
        } else {
          /* Use the ground tag to supply the high-order bits. */
          vertextag += (groundtag >> (7 * vertexatoms)) << (7 * vertexatoms);
        }

        vertexcount++;
        /* Is this the vertex we're searching for? */
        if (vertextag == searchvertex) {
          /* Yes.  Output the previous vertex.  (If we're at the start of  */
          /*   the list, `prevvertextag' is STOP, but we'll fix it later.) */
          adjacencies[0] = prevvertextag;
        }

        if (vertexcount == 1) {
          /* This is the first vertex.  Remember it. */
          firstvertextag = vertextag;
        } else if (prevvertextag == searchvertex) {
          /* The previous vertex was `searchvertex', so output this vertex. */
          adjacencies[1] = vertextag;
          /* Return early...unless `searchvertex' was at the start of the  */
          /*   list, in which case one neighbor is at the end of the list. */
          if (vertexcount > 2) {
            return 1;
          }
        }

        /* Remember this tag during the next iteration. */
        prevvertextag = vertextag;
        /* Prepare to read another tag. */
        vertextag = 0;
        vertexatoms = 0;
      }
    }
  } while (nextmoleculetag != STOP);

  if (nowatom >= (char) 0) {
    /* Oops.  The end of the last vertex is missing from the last molecule. */
    printf("Internal error in linkringadjacencies():\n");
    printf("  Vertex at end of link ring not properly terminated.\n");
    internalerror();
  }

  /* The link ring is circular, so check the first and last vertices. */
  if (prevvertextag == searchvertex) {
    /* The last vertex is `searchvertex', so output the first vertex. */
    adjacencies[1] = firstvertextag;
    if (vertexcount == 1) {
      /* There is only one vertex in the ring. */
      adjacencies[0] = firstvertextag;
    }
    return 1;
  } else if (firstvertextag == searchvertex) {
    /* The first vertex is `searchvertex', so output the last vertex. */
    adjacencies[0] = prevvertextag;
    return 1;
  } else {
    /* Execution can only get this far if `searchvertex' is not in the ring. */
    adjacencies[0] = GHOSTVERTEX;
    adjacencies[1] = GHOSTVERTEX;
    return 0;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  linkringadjacencies()   Return the two neighboring vertices adjoining a  */
/*                          vertex in a link ring.                           */
/*                                                                           */
/*  See header above linkringadjacencies2().                                 */
/*                                                                           */
/*****************************************************************************/

int linkringadjacencies(struct proxipool *pool,
                        tag linkring,
                        tag groundtag,
                        tag searchvertex,
                        tag adjacencies[2])
{
  molecule cule;
  tag vertextag;
  tag firstvertextag;
  tag prevvertextag;
  long vertexcount;
  int vertexatoms;
  int atomindex;
  char nowatom;

  if (linkring >= STOP) {
    /* Not a link ring. */
    adjacencies[0] = GHOSTVERTEX;
    adjacencies[1] = GHOSTVERTEX;
    return 0;
  }

  /* Convert the molecule's tag to a pointer. */
  cule = (molecule) proxipooltag2object(pool, linkring);
  /* Is the list just one molecule? */
  if (cule[MOLECULESIZE - 1] == (char) STOP) {
    /* Yes.  Run a faster lookup. */

    /* There is no "previous" vertex we've visited yet. */
    firstvertextag = STOP;
    prevvertextag = STOP;
    vertexcount = 0;
    vertextag = 0;
    vertexatoms = 0;

    atomindex = 0;
    /* Loop through the atoms, stopping at the "next molecule" tag. */
    while (1) {
      nowatom = cule[atomindex];
      /* Append the atom to the vertex tag. */
      vertextag = (vertextag << 7) + (nowatom & (char) 127);
      vertexatoms++;
      atomindex++;

      /* Is this the last atom in the compressed tag? */
      if (nowatom < (char) 0) {
        /* Yes; we have an uncompressed tag now.  What tag is it? */
        if (((nowatom == (char) STOP) && (vertexatoms == 1)) ||
            (atomindex >= MOLECULESIZE)) {
          /* STOP tag.  End of the list.  The link ring is circular, so */
          /*   check the first and last vertices.                       */
          if (prevvertextag == searchvertex) {
            /* The last vertex is `searchvertex', so output the first */
            /*   vertex.                                              */
            adjacencies[1] = firstvertextag;
            if (vertexcount == 1) {
              /* There is only one vertex in the ring. */
              adjacencies[0] = firstvertextag;
            }
            return 1;
          } else if (firstvertextag == searchvertex) {
            /* The first vertex is `searchvertex', so output the last */
            /*   vertex.                                              */
            adjacencies[0] = prevvertextag;
            return 1;
          }
          /* `searchvertex' is not in the ring. */
          adjacencies[0] = GHOSTVERTEX;
          adjacencies[1] = GHOSTVERTEX;
          return 0;
        } else if ((nowatom == (char) GHOSTVERTEX) && (vertexatoms == 1)) {
          /* Ghost vertex. */
          vertextag = GHOSTVERTEX;
        } else {
          /* Use the ground tag to supply the high-order bits. */
          vertextag += (groundtag >> (7 * vertexatoms)) << (7 * vertexatoms);
        }

        vertexcount++;
        /* Is this the vertex we're searching for? */
        if (vertextag == searchvertex) {
          /* Yes.  Output the previous vertex.  (If we're at the start of  */
          /*   the list, `prevvertextag' is STOP, but we'll fix it later.) */
          adjacencies[0] = prevvertextag;
        }

        if (vertexcount == 1) {
          /* This is the first vertex.  Remember it. */
          firstvertextag = vertextag;
        } else if (prevvertextag == searchvertex) {
          /* The previous vertex was `searchvertex', so output this vertex. */
          adjacencies[1] = vertextag;
          /* Return early...unless `searchvertex' was at the start of the  */
          /*   list, in which case one neighbor is at the end of the list. */
          if (vertexcount > 2) {
            return 1;
          }
        }

        /* Remember this tag during the next iteration. */
        prevvertextag = vertextag;
        /* Prepare to read another tag. */
        vertextag = 0;
        vertexatoms = 0;
      }
    }
  } else {
    /* The list does not fit in one molecule.  Do it the general (slow) way. */
    return linkringadjacencies2(pool, linkring, groundtag, searchvertex,
                                adjacencies);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  linkringiteratorinit()   Initialize an iterator that traverses all the   */
/*                           vertices in a link ring (including ghost        */
/*                           vertices) one by one.                           */
/*                                                                           */
/*  The iterator is a variable `pos', whose internals should not be          */
/*  examined or modified by the client.  The iterator's job is to keep       */
/*  track of where it is in the link ring.  This procedure sets the iterator */
/*  to reference the first vertex in the link ring.                          */
/*                                                                           */
/*  After a link ring is modified, any iterators on that link ring may be    */
/*  corrupted and should not be used without being initialized (by this      */
/*  procedure) again.                                                        */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from.              */
/*  linkring:  The link ring to traverse.                                    */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  pos:  The iterator.  Its contents do not need to be initialized prior to */
/*    calling this procedure.                                                */
/*                                                                           */
/*****************************************************************************/

void linkringiteratorinit(struct proxipool *pool,
                          tag linkring,
                          tag groundtag,
                          struct linkposition *pos)
{
  char nowatom;

  pos->pool = pool;
  pos->groundtag = groundtag;

  if (linkring >= STOP) {
    pos->cule = (molecule) NULL;
    pos->moleculetag = STOP;
    pos->nextmoleculetag = STOP;
    pos->textindex = MOLECULESIZE;
    pos->lasttextindex = -1;
    return;
  }

  pos->moleculetag = linkring;
  /* Find the molecule identified by the tag `linkring'. */
  pos->cule = (molecule) proxipooltag2object(pool, linkring);
  /* Read the molecule's "next molecule" tag. */
  linkringreadtag(linkring, pos->cule, pos->lasttextindex,
                  pos->nextmoleculetag, nowatom,
                  "Internal error in linkringiteratorinit():\n");
  /* Start the iterations from the beginning of the molecule. */
  pos->textindex = 0;
}

/*****************************************************************************/
/*                                                                           */
/*  linkringiterate()   Return the tag that a link ring iterator references, */
/*                      and advance the iterator so it will return the next  */
/*                      vertex next time.                                    */
/*                                                                           */
/*  The iterator is a variable `pos', whose internals should not be          */
/*  examined or modified by the client.                                      */
/*                                                                           */
/*  After a link ring is modified, any iterators on that link ring created   */
/*  before the modification may be corrupted and should not be passed to     */
/*  this procedure again until they are initialized again.                   */
/*                                                                           */
/*  pos:  The iterator.                                                      */
/*                                                                           */
/*  Returns the tag of a vertex in the link ring (possibly GHOSTVERTEX), or  */
/*    STOP if the iterator is at the end of the link ring.                   */
/*                                                                           */
/*****************************************************************************/

tag linkringiterate(struct linkposition *pos)
{
  tag vertextag;
  int vertexatoms;
  char nowatom;

  vertextag = 0;
  vertexatoms = 0;

  /* Loop through atoms to build up one vertex tag. */
  do {
    /* Have we read the last vertex atom in this molecule? */
    while ((pos->textindex > pos->lasttextindex) ||
           ((pos->cule[pos->textindex] == (char) STOP) &&
            (vertexatoms == 0))) {
      /* Yes, we have.  Are there any more molecules after this one? */
      if (pos->nextmoleculetag == STOP) {
        /* No, there aren't.  The iterator is finished. */
        return STOP;
      }

      /* Find the next molecule in the linked list. */
      pos->cule = (molecule) proxipooltag2object(pos->pool,
                                                 pos->nextmoleculetag);
      pos->moleculetag = pos->nextmoleculetag;
      /* Read the next molecule's "next molecule" tag. */
      linkringreadtag(pos->moleculetag, pos->cule, pos->lasttextindex,
                      pos->nextmoleculetag, nowatom,
                      "Internal error in linkringiterate():\n");
      /* Start from the beginning of this molecule. */
      pos->textindex = 0;
    }

    /* Read the next atom. */
    nowatom = pos->cule[pos->textindex];
    /* Append it to the tag. */
    vertextag = (vertextag << 7) + (nowatom & (char) 127);
    vertexatoms++;
    pos->textindex++;
    /* End the loop when `nowatom' has a stop bit (i.e. is negative). */
  } while (nowatom >= (char) 0);

  if ((nowatom == (char) GHOSTVERTEX) && (vertexatoms == 1)) {
    return GHOSTVERTEX;
  } else {
    /* Get the high-order bits from the ground tag. */
    vertextag += (pos->groundtag >> (7 * vertexatoms)) << (7 * vertexatoms);
    return vertextag;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  linkringprint()   Print the contents of a link ring.                     */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from.              */
/*  linkring:  Tag for the first molecule in the link ring.                  */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*                                                                           */
/*****************************************************************************/

void linkringprint(struct proxipool *pool,
                   tag linkring,
                   tag groundtag)
{
  struct linkposition pos;
  tag nexttag;

  printf("Ring %lu: ", (unsigned long) linkring);
  linkringiteratorinit(pool, linkring, groundtag, &pos);
  nexttag = linkringiterate(&pos);
  if (nexttag == STOP) {
    printf("EMPTY");
  } else do {
    if (nexttag == GHOSTVERTEX) {
      printf("GHOST ");
    } else {
      printf("%lu ", (unsigned long) nexttag);
    }
    nexttag = linkringiterate(&pos);
  } while (nexttag != STOP);
  printf("\n");
}

/*****************************************************************************/
/*                                                                           */
/*  linkringinsertatoms()    Insert atoms at a specified position in a link  */
/*  linkringinsertatoms2()   ring (moving the other atoms back to make       */
/*                           room).                                          */
/*                                                                           */
/*  These procedures are interchangeable.  linkringinsertatoms2() is a       */
/*  complete implementation.  linkringinsertatoms() is an optimized wrapper  */
/*  that handles the special case where the entire link ring fits in a       */
/*  single molecule faster, and calls linkringinsertatoms2() otherwise.      */
/*  Profiling suggests that when MOLECULESIZE is 20, linkringinsertatoms()   */
/*  resorts to calling linkringinsertatoms2() only 5.5% of the time.         */
/*                                                                           */
/*  These procedures insert atoms in the REVERSE order they occur in the     */
/*  buffer (to compensate for the fact that linkringtagcompress() writes the */
/*  atoms in reverse order).                                                 */
/*                                                                           */
/*  WARNING:  These procedures uses the input `newatombuffer' not only as a  */
/*  source of atoms to insert, but also as a buffer to shift the following   */
/*  atoms back.  Therefore, the contents of the buffer are trashed.          */
/*                                                                           */
/*  These procedures are meant for internal use by other linkring            */
/*  procedures.                                                              */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from, and that new */
/*    molecules may be allocated from.                                       */
/*  insertposition:  References the position in the link ring to insert the  */
/*    atoms.                                                                 */
/*  insertposition->cule:  Pointer to the molecule.                          */
/*  insertposition->textindex:  The index within the molecule.               */
/*  insertposition->lasttextindex:  Index of the last atom in the molecule   */
/*    thatrepresents a vertex (rather than the "next molecule" tag).         */
/*  insertposition->moleculetag:  Tag for the molecule `position->cule'.     */
/*  insertposition->nextmoleculetag:  Tag for the next molecule in the       */
/*    linked list.  (This field and position->lasttextindex are included so  */
/*    they won't need to be computed a second time.)                         */
/*  numatoms:  The number of atoms to insert.  Must be greater than zero.    */
/*  newatombuffer:  Array of atoms to insert, in reverse order.  This        */
/*    procedure trashes the contents of this buffer.                         */
/*  allocindex:  The allocation index associated with the link ring.         */
/*                                                                           */
/*  Returns the tag of the last molecule in the modified linked list.        */
/*                                                                           */
/*****************************************************************************/

tag linkringinsertatoms2(struct proxipool *pool,
                         struct linkpossmall *insertposition,
                         int numatoms,
                         char *newatombuffer,
                         proxipoolulong allocindex)
{
  molecule cule;
  molecule nextmolecule;
  tag moleculetag;
  tag nextmoleculetag;
  int atomindex;
  int newatomindex;
  int lasttextindex;
  int nextculeindex;
  int bufferatoms;
  int bufferindex;
  int i;
  char nextculeatoms[COMPRESSEDTAGLENGTH];
  char swapatom;
  char nowatom;

  cule = insertposition->cule;
  atomindex = insertposition->textindex;
  lasttextindex = insertposition->lasttextindex;
  moleculetag = insertposition->moleculetag;
  nextmoleculetag = insertposition->nextmoleculetag;
  /* Start at the end of `newatombuffer' and work backward. */
  bufferindex = numatoms - 1;
  /* In the link ring, we are at the start of a compressed tag. */
  swapatom = (char) STOP;

  /* Loop through the linked list of molecules. */
  while (1) {
    /* Loop through the vertex atoms in this molecule. */
    while ((atomindex <= lasttextindex) &&
           ((cule[atomindex] != (char) STOP) || (swapatom >= (char) 0))) {
      /* Swap an atom in the link ring with one in the buffer. */
      swapatom = cule[atomindex];
      cule[atomindex] = newatombuffer[bufferindex];
      newatombuffer[bufferindex] = swapatom;
      atomindex++;
      /* Advance backward through the circular buffer.  This reverses the  */
      /*   order of the input atoms.  The circularity makes it possible to */
      /*   shift the link ring atoms forward through a linked list.        */
      bufferindex = ((bufferindex == 0) ? numatoms : bufferindex) - 1;
    }

    if (nextmoleculetag == STOP) {
      /* We've swapped the last vertex atom in the list into the buffer. */
      break;
    } else {
      /* Find the next molecule. */
      cule = (molecule) proxipooltag2object(pool, nextmoleculetag);
      moleculetag = nextmoleculetag;
      /* Read the tag for the next molecule after that. */
      linkringreadtag(moleculetag, cule, lasttextindex, nextmoleculetag,
                      nowatom, "Internal error in linkringinsertatoms():\n");
      atomindex = 0;
    }
  }

  if (swapatom >= (char) 0) {
    /* The end of the last vertex is missing from the last molecule. */
    printf("Internal error in linkringinsertatoms():\n");
    printf("  Vertex at end of link ring not properly terminated.\n");
    internalerror();
  }

  /* Initialize counter of atoms remaining to append. */
  bufferatoms = numatoms;
  /* While there are too many atoms to fit in one molecule, make molecules. */
  while (atomindex + bufferatoms >= MOLECULESIZE) {
    /* Allocate a new molecule. */
    nextmoleculetag = proxipoolnew(pool, allocindex, (void **) &nextmolecule);
    /* Compress its tag and figure the index to store it at. */
    nextculeindex = MOLECULESIZE -
                    linkringtagcompress(moleculetag, nextmoleculetag,
                                        nextculeatoms);

    /* Is there room for the "next molecule" tag in `cule'? */
    if (atomindex > nextculeindex) {
      /* No; move atoms to the next molecule to make room for the tag. */
      for (newatomindex = 0; newatomindex < atomindex - nextculeindex;
           newatomindex++) {
        nextmolecule[newatomindex] = cule[nextculeindex + newatomindex];
      }
      /* We'll continue writing atoms at this index on the next iteration. */
      atomindex = newatomindex;
    } else {
      /* Fill in the remaining space with atoms from the buffer. */
      while (atomindex < nextculeindex) {
        cule[atomindex] = newatombuffer[bufferindex];
        bufferindex = ((bufferindex == 0) ? numatoms : bufferindex) - 1;
        atomindex++;
        bufferatoms--;
      }
      atomindex = 0;
    }

    /* Copy the "next molecule" tag into the end of `cule'. */
    for (i = nextculeindex; i < MOLECULESIZE; i++) {
      cule[i] = nextculeatoms[i - nextculeindex];
    }
    /* Iterate on the next molecule. */
    moleculetag = nextmoleculetag;
    cule = nextmolecule;
  }

  /* Copy the remaining atoms into the last molecule. */
  while (bufferatoms > 0) {
    cule[atomindex] = newatombuffer[bufferindex];
    bufferindex = ((bufferindex == 0) ? numatoms : bufferindex) - 1;
    bufferatoms--;
    atomindex++;
  }

  /* Write a STOP atom after the last vertex. */
  cule[atomindex] = (char) STOP;
  /* Write a STOP atom for the "next molecule" tag. */
  cule[MOLECULESIZE - 1] = (char) STOP;

  /* Return the tail molecule of the link ring. */
  return moleculetag;
}

/*****************************************************************************/
/*                                                                           */
/*  linkringinsertatoms()   Insert atoms at a specified position in a link   */
/*                          ring (moving the other atoms back to make room). */
/*                                                                           */
/*  See header above linkringinsertatoms2().                                 */
/*                                                                           */
/*****************************************************************************/

tag linkringinsertatoms(struct proxipool *pool,
                        struct linkpossmall *insertposition,
                        int numatoms,
                        char *newatombuffer,
                        proxipoolulong allocindex)
{
  int atomindex;
  char checkatom;

  /* Is the list just one molecule? */
  if (insertposition->nextmoleculetag == STOP) {
    /* Yes.  Find the end of the vertex tags. */
    /* In the link ring, we are at the start of a compressed tag. */
    checkatom = (char) STOP;
    atomindex = insertposition->textindex;
    while ((atomindex <= MOLECULESIZE - 2) &&
           ((insertposition->cule[atomindex] != (char) STOP) ||
            (checkatom >= (char) 0))) {
      checkatom = insertposition->cule[atomindex];
      atomindex++;
    }
    /* Can we fit the new atoms in this molecule? */
    if (atomindex + numatoms < MOLECULESIZE) {
      /* Yes.  Shift atoms to the right to make room for new ones.  Note     */
      /*   that this loop might overwrite the molecule's final atom with the */
      /*   STOP tag, but that's okay, because it's already a STOP tag.       */
      while (atomindex >= insertposition->textindex) {
        insertposition->cule[atomindex + numatoms] =
          insertposition->cule[atomindex];
        atomindex--;
      }
      /* Fill in the new atoms. */
      while (numatoms > 0) {
        atomindex++;
        numatoms--;
        insertposition->cule[atomindex] = newatombuffer[numatoms];
      }
      /* Return the lone molecule's tag. */
      return insertposition->moleculetag;
    }
  }

  /* The updated list does not fit in one molecule.  Do it the general */
  /*   (slow) way.                                                     */
  return linkringinsertatoms2(pool, insertposition, numatoms, newatombuffer,
                              allocindex);
}

/*****************************************************************************/
/*                                                                           */
/*  linkringdeleteatoms()    Delete atoms at a specified position in a link  */
/*  linkringdeleteatoms2()   ring (shifting the following atoms forward).    */
/*                                                                           */
/*  These procedures are interchangeable.  linkringdeleteatoms2() is a       */
/*  complete implementation.  linkringdeleteatoms() is an optimized wrapper  */
/*  that handles the special case where the entire link ring fits in a       */
/*  single molecule faster, and calls linkringdeleteatoms2() otherwise.      */
/*  Profiling suggests that when MOLECULESIZE is 20, linkringdeleteatoms()   */
/*  resorts to calling linkringdeleteatoms2() only 5.0% of the time.         */
/*                                                                           */
/*  Usually, the parameter `position' signifies the first atom to delete.    */
/*  However, if the first atom to delete is at the beginning of a molecule,  */
/*  and that molecule is not the first in the link ring, it's better for     */
/*  `position' to be at the _end_ of the previous molecule, with `textindex' */
/*  past the last vertex atoms in the molecule.  Why?  Because               */
/*  linkringdeleteatoms2() might be able to free the molecule containing the */
/*  first deleted atom--either because all the remaining atoms in the list   */
/*  are deleted, or because the remaining atoms can fit in the previous      */
/*  molecule, overwriting the "next molecule" tag.                           */
/*                                                                           */
/*  linkringdeleteatoms2() deletes atoms from a link ring by having two      */
/*  references traverse through the list, one at a distance behind the       */
/*  other, and by copying atoms from the lead reference to the one following */
/*  it.  To avoid having to decode the "next molecule" tags twice, the lead  */
/*  reference maintains a queue of molecules for use by the following        */
/*  reference.  The queue is an array whose members are reused in circular   */
/*  order, so the link ring can be arbitrarily long.  The size of the queue  */
/*  is fixed, but these procedures are never used to delete more than a      */
/*  small number of atoms, so the lead reference doesn't get too far ahead   */
/*  of the follower.                                                         */
/*                                                                           */
/*  These procedures are meant for internal use by other linkring            */
/*  procedures.  Any external caller needs to be aware that there is a limit */
/*  on the number of atoms that this procedure can delete at once (which can */
/*  be increased by increasing MOLECULEQUEUESIZE).                           */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from.              */
/*  deleteposition:  References the position of the deleted atoms in the     */
/*    link ring.                                                             */
/*  deleteposition->cule:  Pointer to the molecule.                          */
/*  deleteposition->moleculetag:  Tag for the molecule `cule'.  Only needs   */
/*    to be initialized if you are using the return result.  If this is not  */
/*    initialized, the return result might be wrong.                         */
/*  deleteposition->textindex:  The index within the molecule to start       */
/*    deleting atoms from.                                                   */
/*  deleteposition->lasttextindex:  Index of the last atom in the molecule   */
/*    `cule' that represents a vertex (rather than the "next molecule" tag). */
/*  deleteposition->nextmoleculetag:  Tag for the next molecule in the       */
/*    linked list.  (This field and position->lasttextindex are included so  */
/*    they won't need to be computed a second time.)                         */
/*  numatoms:  The number of atoms to delete.                                */
/*                                                                           */
/*  Returns the tag of the last molecule in the modified linked list.        */
/*                                                                           */
/*****************************************************************************/

tag linkringdeleteatoms2(struct proxipool *pool,
                         struct linkpossmall *deleteposition,
                         int numatoms)
{
  struct {
    molecule cule;
    tag culetag;
    int lasttextindex;
  } molequeue[MOLECULEQUEUESIZE];
  molecule leadmolecule;
  molecule followmolecule;
  molecule prevmolecule;
  tag leadnexttag;
  tag leadtag;
  tag returntag;
  int leadindex;
  int followindex;
  int leadlastindex;
  int followlastindex;
  int previndex;
  int leadnextqindex;
  int followcurrentqindex;
  int followprevqindex;
  int followadvanceflag;
  int vertexstartflag;
  int i;
  char nowatom = (char) 0;

  /* Remember the starting position. */
  molequeue[0].cule = deleteposition->cule;
  molequeue[0].culetag = deleteposition->moleculetag;
  molequeue[0].lasttextindex = deleteposition->lasttextindex;
  leadnexttag = deleteposition->nextmoleculetag;

  /* Put the leading and following references at the starting position. */
  leadmolecule = deleteposition->cule;
  followmolecule = deleteposition->cule;
  leadindex = deleteposition->textindex;
  followindex = deleteposition->textindex;
  leadlastindex = deleteposition->lasttextindex;
  followlastindex = deleteposition->lasttextindex;
  /* Keep track of the queue index the lead reference will use next, and the */
  /*   index the follower is using now ('cause that's what's convenient).    */
  leadnextqindex = 1;
  followcurrentqindex = 0;
  /* A flag that indicates whether followcurrentqindex has ever advanced. */
  followadvanceflag = 0;
  /* A flag that indicates we're reading the first atom of a tag. */
  vertexstartflag = 1;

  /* Are we past the last atom in the linked list? */
  if ((leadnexttag == STOP) && (leadindex > leadlastindex)) {
    /* There's nothing here to delete. */
    return deleteposition->moleculetag;
  }

  /* The front reference loops through the molecules of the linked list. */
  do {
    /* Skip this block on the first iteration...unless `deleteposition' is */
    /*   already past all the vertex atoms in its molecule.  On subsequent */
    /*   iterations, this block is always executed.                        */
    if (leadindex > leadlastindex) {
      /* Look up the next molecule (for the lead reference). */
      leadmolecule = (molecule) proxipooltag2object(pool, leadnexttag);
      /* Save it for the follower. */
      molequeue[leadnextqindex].culetag = leadnexttag;
      molequeue[leadnextqindex].cule = leadmolecule;
      leadtag = leadnexttag;
      /* Read the "next molecule" tag. */
      linkringreadtag(leadtag, leadmolecule, leadlastindex, leadnexttag,
                      nowatom, "Internal error in linkringdeleteatoms():\n");
      /* Save the index where the vertex atoms end, for the follower. */
      molequeue[leadnextqindex].lasttextindex = leadlastindex;

      leadindex = 0;
      /* Advance (circularly) the queue index for the next iteration. */
      leadnextqindex = (leadnextqindex + 1 == MOLECULEQUEUESIZE) ?
                       0 : leadnextqindex + 1;
      if (leadnextqindex == followcurrentqindex) {
        /* The leader has just stepped on the follower's data.  The queue */
        /*   isn't large enough to delete this many atoms.                */
        printf("Internal error in linkringdeleteatoms():\n");
        printf("  Asked to delete too many atoms; queue too small.\n");
        printf("  (Increase MOLECULEQUEUESIZE and recompile.)\n");
        internalerror();
      }
    }

    /* Skip up to `numatoms' atoms in this molecule. */
    while ((numatoms > 0) && (leadindex <= leadlastindex)) {
      nowatom = leadmolecule[leadindex];
      if (vertexstartflag && (nowatom == (char) STOP)) {
        /* We've reached the end of the link ring. */
        leadindex = MOLECULESIZE;
#ifdef SELF_CHECK
        if (leadnexttag != STOP) {
          /* Oops.  The link ring ends before the last molecule. */
          printf("Internal error in linkringdeleteatoms():\n");
          printf("  Link ring terminator encountered before last molecule.\n");
          internalerror();
        }
#endif /* SELF_CHECK */
      } else {
        vertexstartflag = nowatom < (char) 0;
        leadindex++;
        numatoms--;
      }
    }

    /* Loop through the atoms in this molecule and copy them from the lead */
    /*   to the follower.  If numatoms > 0, this loop is skipped.          */
    while (leadindex <= leadlastindex) {
      nowatom = leadmolecule[leadindex];
      if (vertexstartflag && (nowatom == (char) STOP)) {
        /* We've reached the end of the link ring. */
        leadindex = MOLECULESIZE;
#ifdef SELF_CHECK
        if (leadnexttag != STOP) {
          /* Oops.  The link ring ends before the last molecule. */
          printf("Internal error in linkringdeleteatoms():\n");
          printf("  Link ring terminator encountered before last molecule.\n");
          internalerror();
        }
#endif /* SELF_CHECK */
      } else {
        /* Is the follower at the end of its molecule? */
        if (followindex > followlastindex) {
          /* Yes.  Advance (circularly) the queue index and molecule. */
          followcurrentqindex =
            (followcurrentqindex + 1 == MOLECULEQUEUESIZE) ?
            0 : followcurrentqindex + 1;
          followmolecule = molequeue[followcurrentqindex].cule;
          followindex = 0;
          followlastindex = molequeue[followcurrentqindex].lasttextindex;
          followadvanceflag = 1;
        }

        /* Copy an atom from the leader. */
        followmolecule[followindex] = nowatom;
        followindex++;

        /* Check the atom for a stop bit. */
        vertexstartflag = nowatom < (char) 0;
        leadindex++;
      }
    }
  } while (leadnexttag != STOP);

  if (nowatom >= (char) 0) {
    /* Oops.  The end of the last vertex is missing from the last molecule. */
    printf("Internal error in linkringdeleteatoms():\n");
    printf("  Vertex at end of link ring not properly terminated.\n");
    internalerror();
  }

  /* The leader has reached the end of the linked list.  The follower might */
  /*   be able to get rid of the molecule it's currently in by moving atoms */
  /*   into the previous molecule, writing over atoms taken by the "next    */
  /*   molecule tag".  Check whether it's possible.  It's only possible if  */
  /*   there is a previous molecule, and that molecule has enough space     */
  /*   currently taken by the "next molecule" tag.                          */
  followprevqindex = (followcurrentqindex == 0) ?
                     MOLECULEQUEUESIZE - 1 : followcurrentqindex - 1;
  if (followadvanceflag &&
      (molequeue[followprevqindex].cule != (molecule) NULL) &&
      (followindex <=
       MOLECULESIZE - 2 - molequeue[followprevqindex].lasttextindex)) {
    prevmolecule = molequeue[followprevqindex].cule;
    previndex = molequeue[followprevqindex].lasttextindex + 1;
    /* Prepare to return the tail molecule. */
    returntag = molequeue[followprevqindex].culetag;
    /* Copy atoms from the follower's molecule to its previous molecule. */
    for (i = 0; i < followindex; i++) {
      prevmolecule[previndex] = followmolecule[i];
      previndex++;
    }
    /* Write a STOP atom after the last vertex. */
    prevmolecule[previndex] = (char) STOP;
    /* Write a STOP atom for the "next molecule" tag. */
    prevmolecule[MOLECULESIZE - 1] = (char) STOP;
  } else {
    /* Prepare to return the tail molecule. */
    returntag = molequeue[followcurrentqindex].culetag;
    /* Write a STOP atom after the last vertex. */
    followmolecule[followindex] = (char) STOP;
    /* Write a STOP atom for the "next molecule" tag. */
    followmolecule[MOLECULESIZE - 1] = (char) STOP;
    /* Start freeing molecules with the next one. */
    followcurrentqindex = (followcurrentqindex + 1 == MOLECULEQUEUESIZE) ?
                          0 : followcurrentqindex + 1;
  }

  /* Loop through the remaining molecules and free them. */
  while (followcurrentqindex != leadnextqindex) {
    proxipoolfree(pool, molequeue[followcurrentqindex].culetag);
    followcurrentqindex = (followcurrentqindex + 1 == MOLECULEQUEUESIZE) ?
                          0 : followcurrentqindex + 1;
  }

  /* Return the tail molecule of the link ring. */
  return returntag;
}

/*****************************************************************************/
/*                                                                           */
/*  linkringdeleteatoms()   Delete atoms at a specified position in a link   */
/*                          ring (shifting the following atoms forward).     */
/*                                                                           */
/*  See header above linkringdeleteatoms2().                                 */
/*                                                                           */
/*****************************************************************************/

tag linkringdeleteatoms(struct proxipool *pool,
                        struct linkpossmall *deleteposition,
                        int numatoms)
{
  int i;

  /* Is the list just one molecule? */
  if (deleteposition->nextmoleculetag == STOP) {
    /* Yes.  Shift atoms to the left to delete `numatoms' atoms. */
    for (i = deleteposition->textindex + numatoms; i < MOLECULESIZE - 1; i++) {
      deleteposition->cule[i - numatoms] = deleteposition->cule[i];
    }
    deleteposition->cule[i - numatoms] = (char) STOP;
    /* Return the lone molecule's tag. */
    return deleteposition->moleculetag;
  } else {
    /* The list does not fit in one molecule.  Do it the general (slow) way. */
    return linkringdeleteatoms2(pool, deleteposition, numatoms);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  linkringrotateatoms()   Swap two contiguous sets of atoms.               */
/*                                                                           */
/*  This is a special-purpose procedure used solely to handle one special    */
/*  case in linkringinsertedge().  If a link ring consists of three or more  */
/*  distinct chains of edges (with three or more ghost vertices filling the  */
/*  gaps), a newly inserted edge might glue together two of those chains--   */
/*  and the chains might not be listed in the right order.  So some of the   */
/*  chains need to be swapped.                                               */
/*                                                                           */
/*  Imagine dividing a link ring into three segments of vertices (including  */
/*  ghost vertices).  One segment begins at position `pos1' and ends with    */
/*  the atom right before position `pos2'.  Another segment begins at `pos2' */
/*  and ends with the atom right before `pos3'.  A third segment begins at   */
/*  `pos3' and (circularly) ends right before `pos1'.  This procedure swaps  */
/*  the first two segments (beginning at `pos1' and `pos2').                 */
/*                                                                           */
/*  WARNING:  It's dangerous to swap two arbitrary segments, because the     */
/*  beginning of the linked list must coincide with the beginning of a       */
/*  compressed vertex tag.  Therefore, the positions `pos1', `pos2', and     */
/*  `pos3' should occur in that order in the linked list, or one of those    */
/*  three should be the very start of the linked list.  Calling this         */
/*  procedure in other circumstances might misalign a compressed tag at the  */
/*  start of the list (though it will swap the atoms as advertised).         */
/*                                                                           */
/*  On return, `pos2' references the first atom of the _moved_ copy of the   */
/*  segment that was originally (but no longer) at `pos1'.  The contents of  */
/*  `pos1' may be trashed, so copy it before calling if you need it.  `pos3' */
/*  is unchanged.                                                            */
/*                                                                           */
/*  It's nearly impossible to do this efficiently without extra buffer       */
/*  space (since the link ring is only singly-linked), and this procedure    */
/*  might temporarily allocate as large an array as it needs.                */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from.              */
/*  linkring:  Tag for the first molecule in the link ring.                  */
/*  pos1:  References the first atom in the first segment to swap.  Contents */
/*    are trashed on return.                                                 */
/*  pos2:  References the first atom in the second segment to swap.  Also    */
/*    demarcates the end of the first segment.  On return, this position     */
/*    references the first atom in the _moved_ segment that was originally   */
/*    the first segment (but now comes second).                              */
/*  pos3:  References the first atom in a third segment, which doesn't move. */
/*    Therefore, `pos3' demarcates the end of the second segment.            */
/*                                                                           */
/*****************************************************************************/

void linkringrotateatoms(struct proxipool *pool,
                         tag linkring,
                         struct linkpossmall *pos1,
                         struct linkpossmall *pos2,
                         struct linkpossmall *pos3)
{
#define STARTBUFFERSIZE 1024

  starlong bufferindex;
  starlong buffersize;
  starlong i;
  int atomindex1, atomindex2, atomindex3;
  int starttag1flag, starttag2flag;
  char firstbuffer[STARTBUFFERSIZE];
  char *buffer;
  char *newbuffer;
  char nowatom;

  /* Start with a buffer in static memory.  We'll allocate a larger one */
  /*   from dynamic memory only if necessary.                           */
  buffer = firstbuffer;
  buffersize = STARTBUFFERSIZE;
  bufferindex = 0;
  /* Move the indices into local variables (hopefully registers). */
  atomindex1 = pos1->textindex;
  atomindex2 = pos2->textindex;
  atomindex3 = pos3->textindex;
  /* Indicate that each position, 1 and 2, is at the start of a compressed */
  /*   vertex tag.                                                         */
  starttag1flag = 1;
  starttag2flag = 1;

  /* Each iteration of this loop advances both positions by one atom. */
  do {
    /* Have we read the last vertex atom in the molecule at position 1? */
    if ((atomindex1 > pos1->lasttextindex) ||
        (starttag1flag && (pos1->cule[atomindex1] == (char) STOP))) {
      /* Yes, we have.  Are there any more molecules after this one? */
      if (pos1->nextmoleculetag == STOP) {
        /* No, there aren't.  Loop back to the start of the linked list. */
        pos1->nextmoleculetag = linkring;
      }
      /* Find the next molecule in the linked list. */
      pos1->cule = (molecule) proxipooltag2object(pool, pos1->nextmoleculetag);
      pos1->moleculetag = pos1->nextmoleculetag;
      /* Read the next molecule's "next molecule" tag. */
      linkringreadtag(pos1->moleculetag, pos1->cule, pos1->lasttextindex,
                      pos1->nextmoleculetag, nowatom,
                      "Internal error in linkringrotateatoms():\n");
      /* Start from the beginning of this molecule. */
      atomindex1 = 0;
    }

    /* Have we read the last vertex atom in the molecule at position 2? */
    if ((atomindex2 > pos2->lasttextindex) ||
        (starttag2flag && (pos2->cule[atomindex2] == (char) STOP))) {
      /* Yes, we have.  Are there any more molecules after this one? */
      if (pos2->nextmoleculetag == STOP) {
        /* No, there aren't.  Loop back to the start of the linked list. */
        pos2->nextmoleculetag = linkring;
      }
      /* Find the next molecule in the linked list. */
      pos2->cule = (molecule) proxipooltag2object(pool, pos2->nextmoleculetag);
      pos2->moleculetag = pos2->nextmoleculetag;
      /* Read the next molecule's "next molecule" tag. */
      linkringreadtag(pos2->moleculetag, pos2->cule, pos2->lasttextindex,
                      pos2->nextmoleculetag, nowatom,
                      "Internal error in linkringrotateatoms():\n");
      /* Start from the beginning of this molecule. */
      atomindex2 = 0;

      /* Exit the loop when position 2 reaches position 3. */
      if ((pos2->cule == pos3->cule) && (atomindex2 == atomindex3)) {
        break;
      }
    }

    /* There's no telling at the start how large a buffer we need, so we */
    /*   may need to resize it.                                          */
    if (bufferindex >= buffersize) {
      newbuffer = (char *) starmalloc((size_t) (buffersize * 3));
      for (i = 0; i < buffersize; i++) {
        newbuffer[i] = buffer[i];
      }
      /* The first buffer is in static memory; only free subsequent buffers. */
      if (buffer != firstbuffer) {
        starfree(buffer);
      }
      buffer = newbuffer;
      buffersize *= 3;
    }

    /* Copy atoms from position 1 to the buffer. */
    nowatom = pos1->cule[atomindex1];
    buffer[bufferindex] = nowatom;
    /* Will we be starting a new tag at position 1?  Check for a stop bit. */
    starttag1flag = nowatom < (char) 0;
    /* Copy atoms from position 2 to position 1. */
    nowatom = pos2->cule[atomindex2];
    pos1->cule[atomindex1] = nowatom;
    /* Will we be starting a new tag at position 2? */
    starttag2flag = nowatom < (char) 0;
    /* Advance all the positions. */
    bufferindex++;
    atomindex1++;
    atomindex2++;

    /* Exit the loop when position 2 reaches position 3. */
  } while ((pos2->cule != pos3->cule) || (atomindex2 != atomindex3));

  /* Now we know how many atoms are in the second segment.  Set up the */
  /*   buffer to be a circular buffer of that length.                  */
  buffersize = bufferindex;
  bufferindex = 0;

  /* Store the position of the (now) second of the two swapped segments, to */
  /*   return to the caller.                                                */
  *pos2 = *pos1;
  pos2->textindex = atomindex1;

  /* Position 1 continues to loop through atoms, swapping them with atoms   */
  /*   in the circular buffer, until it reaches position 3.  The circular   */
  /*   buffer makes it possible to move all the atoms in the first segment  */
  /*   back by a number of atoms equal to the length of the second segment, */
  /*   without knowing how long the first segment is.                       */
  do {
    /* Have we read the last vertex atom in the molecule at position 1? */
    if ((atomindex1 > pos1->lasttextindex) ||
        (starttag1flag && (pos1->cule[atomindex1] == (char) STOP))) {
      /* Yes, we have.  Are there any more molecules after this one? */
      if (pos1->nextmoleculetag == STOP) {
        /* No, there aren't.  Loop back to the start of the linked list. */
        pos1->nextmoleculetag = linkring;
      }
      /* Find the next molecule in the linked list. */
      pos1->cule = (molecule) proxipooltag2object(pool, pos1->nextmoleculetag);
      pos1->moleculetag = pos1->nextmoleculetag;
      /* Read the next molecule's "next molecule" tag. */
      linkringreadtag(pos1->moleculetag, pos1->cule, pos1->lasttextindex,
                      pos1->nextmoleculetag, nowatom,
                      "Internal error in linkringrotateatoms():\n");
      /* Start from the beginning of this molecule. */
      atomindex1 = 0;

      /* Exit the loop when position 1 reaches position 3. */
      if ((pos1->cule == pos3->cule) && (atomindex1 == atomindex3)) {
        break;
      }
    }

    /* Swap atoms between position 1 and the buffer. */
    nowatom = pos1->cule[atomindex1];
    pos1->cule[atomindex1] = buffer[bufferindex];
    buffer[bufferindex] = nowatom;
    /* Will we be starting a new tag?  Check for a stop bit. */
    starttag1flag = nowatom < (char) 0;
    /* Advance the positions. */
    bufferindex = (bufferindex + 1 >= buffersize) ? 0 : bufferindex + 1;
    atomindex1++;
    /* Exit the loop when position 1 reaches position 3. */
  } while ((pos1->cule != pos3->cule) || (atomindex1 != atomindex3));

  /* If the buffer was allocated dynamically, free it. */
  if (buffer != firstbuffer) {
    starfree(buffer);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  linkringinsertedge()   Insert an edge into a link ring.                  */
/*                                                                           */
/*  If this link ring is the link of an edge, you are actually inserting a   */
/*  tetrahedron (from the edge's point of view--you may need to inform the   */
/*  tetrahedron's other edges as well.)  If this link ring is the link of a  */
/*  vertex, you are actually inserting a triangle (from the vertex's point   */
/*  of view).                                                                */
/*                                                                           */
/*  The new edge is `endpoint1'-`endpoint2', with `endpoint2' immediately    */
/*  following (counterclockwise from--unless you're using the reverse        */
/*  orientation everywhere, as these procedures can't tell the difference)   */
/*  `endpoint1'.  The order in which the two endpoints are given matters--it */
/*  must match the desired orientation of the link ring.  It is possible to  */
/*  have a link ring with just two edges--say, 1-2 and 2-1.                  */
/*                                                                           */
/*  If both vertices of the new edge are already present, this procedure     */
/*  can glue two chains together, or close a single chain (making the link   */
/*  into a true ring), if it's appropriate to do so.                         */
/*                                                                           */
/*  This procedure will return 0 and leave the link ring unchanged if any of */
/*  the following is true.                                                   */
/*                                                                           */
/*  - There is already an edge immediately following `endpoint1'.  (That is, */
/*    `endpoint1' is in the list and is not followed by a ghost vertex.)     */
/*  - There is already an edge immediately preceding `endpoint2'.  (That is, */
/*    `endpoint2' is in the list and is not preceded by a ghost vertex.)     */
/*  - The link ring is truly a ring, with no space to insert a new edge      */
/*    (i.e. no ghost vertex).                                                */
/*  - The link ring currently consists of two or more chains, and the        */
/*    introduction of the new edge will glue one of them into a ring.  It's  */
/*    not topologically possible to have a link with a ring AND a chain.     */
/*                                                                           */
/*  WARNING:  This procedure will not usually work if there is more than one */
/*  copy of `endpoint1', or more than one copy of `endpoint2', in the ring.  */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from, and that new */
/*    molecules may be allocated from.                                       */
/*  linkring:  Tag for the first molecule in the link ring.                  */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  endpoint1:  The tag of the new edge's first vertex.                      */
/*  endpoint2:  The tag of the new edge's second vertex.                     */
/*                                                                           */
/*  Returns one of the following values.                                     */
/*    0:  If the edge cannot be inserted, and the link ring is unchanged.    */
/*    1:  If `endpoint1' was inserted, and `endpoint2' was already present.  */
/*    2:  If `endpoint2' was inserted, and `endpoint1' was already present.  */
/*    3:  If neither vertex was present, and both were inserted.  (In this   */
/*        case, a ghost vertex is inserted as well to prevent the new edge   */
/*        from being connected to any other vertex.)                         */
/*    4:  If both `endpoint1' and `endpoint2' were already present in the    */
/*        link, and merely needed to be connected by an edge (which is       */
/*        accomplished by deleting the ghost vertex between the two          */
/*        endpoints).                                                        */
/*                                                                           */
/*****************************************************************************/

int linkringinsertedge(struct proxipool *pool,
                       tag linkring,
                       tag groundtag,
                       tag endpoint1,
                       tag endpoint2)
{
  struct linkpossmall position;
  struct linkpossmall pt2position;
  struct linkpossmall ghostposition;
  struct linkpossmall ghostafter1position;
  struct linkpossmall afterghostafter2position;
  molecule cule;
  tag nextmoleculetag;
  tag moleculetag;
  int atomindex;
  int lasttextindex;
  int numatoms;
  char newvertexatoms[2 * COMPRESSEDTAGLENGTH + 1];
  char nowatom;

  /* There is no "previous" vertex we've visited yet. */
  tag firstvertextag = STOP;
  tag prevvertextag = STOP;
  tag prevprevvertextag = STOP;
  tag vertextag = 0;
  int vertexatoms = 0;
  /* Set to 1 during the first iteration of the "do" loop. */
  int firstflag = 1;
  /* The following flags indicate whether `endpoint1', `endpoint2', or */
  /*   a ghost vertex have been visited in the linked list yet.        */
  int oneflag = 0;
  int twoflag = 0;
  int ghostflag = 0;
  /* Set if `endpoint2' is one of the first two vertices in the linked list. */
  int early2flag = 0;
  /* Set if `endpoint2' has been visited, but not `endpoint1' after it. */
  int twonot1flag = 0;
  /* Set if a ghost vertex has been visited, but not `endpoint2' after it. */
  int ghostnot2flag = 0;
  /* Set if at least one ghost vertex comes after `endpoint2' but before */
  /*   `endpoint1' in the ring (taking into account that it loops).      */
  int ghostbetween21flag = 0;
  /* Set if `endpoint1' appears after `endpoint2' in the linked list. */
  int oneafter2flag = 0;
  /* Set if a ghost vertex appears after `endpoint2' in the linked list. */
  int ghostafter2flag = 0;

  if ((linkring >= STOP) || (endpoint1 >= STOP) || (endpoint2 >= STOP)) {
    /* Invalid parameters. */
    return 0;
  }

  /* Start at the first molecule. */
  nextmoleculetag = linkring;

  /* Loop through the linked list of molecules. */
  do {
    /* Convert the molecule's tag to a pointer. */
    cule = (molecule) proxipooltag2object(pool, nextmoleculetag);
    moleculetag = nextmoleculetag;
    /* Read the "next molecule" tag. */
    linkringreadtag(moleculetag, cule, lasttextindex, nextmoleculetag, nowatom,
                    "Internal error in linkringinsertedge():\n");
    if (firstflag) {
      /* Position to insert or delete atoms at the beginning of the list. */
      position.cule = cule;
      position.moleculetag = moleculetag;
      position.textindex = 0;
      position.lasttextindex = lasttextindex;
      position.nextmoleculetag = nextmoleculetag;
      firstflag = 0;
    }

    atomindex = 0;
    /* Loop through the atoms, stopping at the "next molecule" tag. */
    while (atomindex <= lasttextindex) {
      nowatom = cule[atomindex];
      /* Append the atom to the vertex tag. */
      vertextag = (vertextag << 7) + (nowatom & (char) 127);
      vertexatoms++;
      atomindex++;

      /* Is this the last atom in the compressed tag? */
      if (nowatom < (char) 0) {
        /* Yes; we have an uncompressed tag now.  What tag is it? */
        if ((nowatom == (char) STOP) && (vertexatoms == 1)) {
          /* STOP tag.  Exit the inner loop early; go on to next molecule. */
          break;
        } else if ((nowatom == (char) GHOSTVERTEX) && (vertexatoms == 1)) {
          /* Ghost vertex. */
          vertextag = GHOSTVERTEX;
          ghostflag = 1;
          if (twoflag) {
            /* The beginning/end of the linked list does not fall between */
            /*   `endpoint2' and the following ghost.                     */
            ghostafter2flag = 1;
          }
          if (twonot1flag) {
            /* Because `endpoint2' has been visited, and `endpoint1' didn't */
            /*   follow it, this ghost vertex appears after `endpoint2' and */
            /*   before `endpoint1' in the link ring...IF `endpoint1' is in */
            /*   the link at all.  (If not, this won't hurt.)               */
            ghostbetween21flag = 1;
          }
          /* Remember the ghost's position in case we need to insert a */
          /*   vertex here or delete the ghost.                        */
          ghostposition = position;
          if (prevvertextag == endpoint1) {
            ghostafter1position = position;
          }
          if (!ghostnot2flag) {
            /* Remember the position after the ghost, in case we need to */
            /*   rotate the atoms in the link ring.  Note that if        */
            /*   `endpoint2' isn't in the link ring, this branch might   */
            /*   execute, but it won't matter.                           */
            afterghostafter2position.cule = cule;
            afterghostafter2position.textindex = atomindex;
            afterghostafter2position.lasttextindex = lasttextindex;
            afterghostafter2position.moleculetag = moleculetag;
            afterghostafter2position.nextmoleculetag = nextmoleculetag;
          }
          ghostnot2flag = 1;
        } else {
          /* The tag is neither a STOP tag nor a ghost vertex. */
          if (prevvertextag == endpoint1) {
            /* `endpoint1' does not have a ghost vertex after it, so it is */
            /*   not possible to insert an (ordered) edge 1-2.             */
            return 0;
          }

          /* Use the ground tag to supply the high-order bits. */
          vertextag += (groundtag >> (7 * vertexatoms)) << (7 * vertexatoms);

          if (vertextag == endpoint1) {
            oneflag = 1;
            twonot1flag = 0;
            if (ghostnot2flag) {
              /* Because a ghost vertex has been visited, and `endpoint2' */
              /*   didn't follow it, that ghost vertex appears after      */
              /*   `endpoint2' and before `endpoint1' in the link ring... */
              /*   IF `endpoint2' is in the link at all.  (If not, this   */
              /*   won't hurt.)                                           */
              ghostbetween21flag = 1;
            }
            if (twoflag) {
              oneafter2flag = 1;
            }
          } else if (vertextag == endpoint2) {
            if ((prevvertextag != GHOSTVERTEX) && (prevvertextag != STOP)) {
              /* `endpoint2' does not have a ghost vertex before it, so it */
              /*   is not possible to insert an (ordered) edge 1-2.        */
              return 0;
            }
            if (prevprevvertextag == endpoint1) {
              /* `endpoint1' is immediately followed by a ghost vertex, then */
              /*   `endpoint2', so just delete the ghost from the middle.    */
              linkringdeleteatoms(pool, &ghostposition, 1);
              return 4;
            }

            if (prevprevvertextag == STOP) {
              /* `endpoint2' is one of the first two vertices in the list. */
              /*   Remember this in case `endpoint1' is the last.          */
              early2flag = 1;
            }
            twoflag = 1;
            twonot1flag = 1;
            ghostnot2flag = 0;
            /* Remember `endpoint2's position in case we need to insert */
            /*   `endpoint1' in front of it.                            */
            pt2position = position;
          }
        }

        if (prevvertextag == STOP) {
          /* This is the first vertex.  Remember it. */
          firstvertextag = vertextag;
        }

        /* Remember `vertextag' for the next two iterations. */
        prevprevvertextag = prevvertextag;
        prevvertextag = vertextag;
        /* Prepare to read another tag. */
        vertextag = 0;
        vertexatoms = 0;

        /* Store the position of the beginning of the next compressed tag,   */
        /*   in case we realize we need it once we reach the end of the tag. */
        position.cule = cule;
        position.textindex = atomindex;
        position.lasttextindex = lasttextindex;
        position.moleculetag = moleculetag;
        position.nextmoleculetag = nextmoleculetag;
      }
    }
  } while (nextmoleculetag != STOP);

  if (nowatom >= (char) 0) {
    /* Oops.  The end of the last vertex is missing from the last molecule. */
    printf("Internal error in linkringinsertedge():\n");
    printf("  Vertex at end of link ring not properly terminated.\n");
    internalerror();
  }

  if (prevvertextag == endpoint1) {
    /* The last vertex in the linked list is `endpoint1'. */
    if (firstvertextag != GHOSTVERTEX) {
      /* `endpoint1' does not have a ghost vertex after it, so it is */
      /*   not possible to insert an (ordered) edge 1-2.             */
      return 0;
    } else if (!twoflag) {
      /* `endpoint2' is not in the list, so insert it right after */
      /*   `endpoint1' (at the very end of the linked list).      */
      numatoms = linkringtagcompress(groundtag, endpoint2, newvertexatoms);
      linkringinsertatoms(pool, &position, numatoms, newvertexatoms,
                          proxipooltag2allocindex(pool, linkring));
      return 2;
    } else {
      /* The ghost following `endpoint1` is at the beginning of the list. */
      ghostafter1position.moleculetag = linkring;
      ghostafter1position.cule = (molecule) proxipooltag2object(pool,
                                                                linkring);
      ghostafter1position.textindex = 0;
      /* Read the "next molecule" tag. */
      linkringreadtag(linkring, ghostafter1position.cule,
                      ghostafter1position.lasttextindex,
                      ghostafter1position.nextmoleculetag,
                      nowatom, "Internal error in linkringinsertedge():\n");
      if (early2flag) {
        /* `endpoint2' must occur second in the linked list, with a ghost   */
        /*   vertex before it, and `endpoint1' at the end of the list (thus */
        /*   before the ghost in the link ring).  Delete the ghost vertex.  */
        linkringdeleteatoms(pool, &ghostafter1position, 1);
        return 4;
      }
      /* If execution reaches this point, we either need to rotate a portion */
      /*   of the link ring, or decide that inserting the edge is            */
      /*   impossible.  Code for doing these appears further down.           */
    }
  } else if (firstvertextag == endpoint2) {
    /* The first vertex in the linked list is `endpoint2'. */
    if (prevvertextag != GHOSTVERTEX) {
      /* `endpoint2' (at the beginning of the linked list) does not have a   */
      /*   ghost vertex before it in the link ring (at the end of the list), */
      /*   so it is not possible to insert an (ordered) edge 1-2.            */
      return 0;
    } else if (prevprevvertextag == endpoint1) {
      /* The linked list ends with `endpoint1' (second to last) and a ghost */
      /*   vertex (last), and begins with `endpoint2'.  Delete the ghost.   */
      linkringdeleteatoms(pool, &ghostposition, 1);
      return 4;
    } else if (!oneflag) {
      /* `endpoint1' is not in the list, so insert it at the very end of the */
      /*   linked list, putting it right before `endpoint2' in the ring.     */
      numatoms = linkringtagcompress(groundtag, endpoint1, newvertexatoms);
      linkringinsertatoms(pool, &position, numatoms, newvertexatoms,
                          proxipooltag2allocindex(pool, linkring));
      return 1;
    }
    /* If execution reaches this point, we either need to rotate a portion */
    /*   of the link ring, or decide that inserting the edge is            */
    /*   impossible.  Code for doing these appears soon.                   */
  }

  if (oneflag) {
    if (twoflag) {
      if (ghostbetween21flag) {
        /* Here is the yuckiest situation.  There is a ghost vertex after    */
        /*   `endpoint1', and a ghost vertex before `endpoint2', but they    */
        /*   are not the same ghost vertex.  There is a third ghost vertex   */
        /*   following `endpoint2' and preceding `endpoint1'.  Therefore,    */
        /*   the link ring is composed of three or more connected chains of  */
        /*   edges, and we need to reorder these chains to get `endpoint1'   */
        /*   and `endpoint2' connected to each other.  There's a procedure   */
        /*   written just for this purpose.                                  */
        /*                                                                   */
        /* Imagine dividing the link ring into three segments of vertices    */
        /*   (including ghost vertices).  Segment 2 begins with `endpoint2'  */
        /*   and ends with the first ghost vertex following it.  Segment 1   */
        /*   begins right after that ghost vertex, and ends at `endpoint1'.  */
        /*   Segment 0 begins with the next vertex, a ghost vertex, and ends */
        /*   with the vertex right before `endpoint2', also a ghost vertex.  */
        /*   We want to swap two of these segments, thereby creating the     */
        /*   edge 1-2.  The swap also puts two ghost vertices together, at   */
        /*   the start of Segment 0 and the end of Segment 2, so we then     */
        /*   delete one of those two ghost vertices.                         */
        /*                                                                   */
        /* However, it's dangerous to swap two arbitrary segments, because   */
        /*   the beginning of the link ring must coincide with the beginning */
        /*   of a vertex.  Therefore, we identify two segments that are safe */
        /*   to swap (don't have the beginning/end of the link ring inside   */
        /*   them), and swap those.                                          */
        if (oneafter2flag) {
          /* It is safe to swap Segments 2 and 1. */
          position = ghostafter1position;
          linkringrotateatoms(pool, linkring, &pt2position,
                              &afterghostafter2position, &ghostafter1position);
        } else if (ghostafter2flag) {
          /* `endpoint1' precedes `endpoint2' in the linked list, and a */
          /*    ghost vertex follows `endpoint2', so it is safe to swap */
          /*    Segments 0 and 2.                                       */
          linkringrotateatoms(pool, linkring, &ghostafter1position,
                              &pt2position, &afterghostafter2position);
          /* When the above procedure returns, `pt2position' is the new */
          /*   position of Segment 0.                                   */
          position = pt2position;
        } else {
          /* Segment 2 overlaps the beginning/end of the link ring, so it is */
          /*   safe to swap Segments 1 and 0.                                */
          position = afterghostafter2position;
          linkringrotateatoms(pool, linkring, &afterghostafter2position,
                              &ghostafter1position, &pt2position);
        }

        /*   The swap puts two ghost vertices together, at the end of      */
        /*   Segment 2 and the start of Segment 0.  Delete the second one. */
        linkringdeleteatoms(pool, &position, 1);
        return 4;
      } else {
        /* There is a ghost vertex after `endpoint1', and a ghost vertex     */
        /*   before `endpoint2', but they are not the same ghost vertex.     */
        /*   There is no ghost vertex following `endpoint2' and preceding    */
        /*   `endpoint1', so the two endpoints are ends of the same chain.   */
        /*   If we create an edge 1-2, it will close that chain into a ring. */
        /*   However, there's at least one other chain, and it's not         */
        /*   topologically possible to have a link with a ring AND a chain.  */
        return 0;
      }
    } else {
      /* `endpoint2' is not in the list, so insert it right after */
      /*   `endpoint1'.                                           */
      numatoms = linkringtagcompress(groundtag, endpoint2, newvertexatoms);
      linkringinsertatoms(pool, &ghostafter1position, numatoms, newvertexatoms,
                          proxipooltag2allocindex(pool, linkring));
      return 2;
    }
  } else {
    if (twoflag) {
      /* `endpoint1' is not in the list, so insert it right before */
      /*   `endpoint2'.                                            */
      numatoms = linkringtagcompress(groundtag, endpoint1, newvertexatoms);
      linkringinsertatoms(pool, &pt2position, numatoms, newvertexatoms,
                          proxipooltag2allocindex(pool, linkring));
      return 1;
    } else {
      /* Neither `endpoint1' nor `endpoint2' are in the list. */
      if (ghostflag || (prevvertextag == STOP)) {
        /* Insert a ghost vertex, followed by `endpoint1', followed by    */
        /*   `endpoint2'.  Because linkringtagcompress() writes the atoms */
        /*   backward, and linkringinsertatoms() reverses them back to    */
        /*   forward, write the sequence 2-1-ghost into the buffer.       */
        numatoms = linkringtagcompress(groundtag, endpoint2,
                                       newvertexatoms);
        numatoms += linkringtagcompress(groundtag, endpoint1,
                                        &newvertexatoms[numatoms]);
        newvertexatoms[numatoms] = (char) GHOSTVERTEX;
        if (ghostflag) {
          /* Insert them in front of a ghost vertex. */
          linkringinsertatoms(pool, &ghostposition, numatoms + 1,
                              newvertexatoms,
                              proxipooltag2allocindex(pool, linkring));
        } else {
          /* The link ring is empty.  Insert the three vertices. */
          linkringinsertatoms(pool, &position, numatoms + 1, newvertexatoms,
                              proxipooltag2allocindex(pool, linkring));
        }
        return 3;
      } else {
        /* The link ring is really a ring, so there's no gap to put a new */
        /*   edge into.                                                   */
        return 0;
      }
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  linkringdeleteedge()   Delete an edge from a link ring.                  */
/*                                                                           */
/*  This procedure also deletes any endpoint of the edge that is not an      */
/*  endpoint of another edge.  Therefore, it cannot be used to create a      */
/*  single dangling vertex with no edge.  (You can accomplish that by using  */
/*  linkringdeletevertex() to whittle a chain down to one vertex, or by      */
/*  using linkringinsertvertex() to insert a ghost vertex.)                  */
/*                                                                           */
/*  The order in which the two endpoints are given matters--it must match    */
/*  the edge's orientation in the link ring.  It is possible to have a link  */
/*  ring with just two edges--say, 1-2 and 2-1.  In this case, the order of  */
/*  the endpoints determines which of the two edges is deleted.              */
/*                                                                           */
/*  WARNING:  This procedure will not usually work if there is more than one */
/*  copy of `endpoint1', or more than one copy of `endpoint2', in the ring.  */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from, and that new */
/*    molecules may be allocated from.                                       */
/*  linkring:  Tag for the first molecule in the link ring.                  */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  endpoint1:  The tag of the deleted edge's first vertex.                  */
/*  endpoint2:  The tag of the deleted edge's second vertex.                 */
/*                                                                           */
/*  Returns one of the following values.                                     */
/*    -1:  If the link ring is empty on entry (so it is not changed).        */
/*     0:  If the edge is not present, and the link ring is unchanged (but   */
/*         not empty).                                                       */
/*     1:  If `endpoint1' was deleted, and `endpoint2' survives because it   */
/*         is an endpoint of another edge.                                   */
/*     2:  If `endpoint2' was deleted, and `endpoint1' survives.             */
/*     3:  If both vertices are deleted, but some other vertex survives.     */
/*         (In this case, a ghost vertex is deleted as well, to prevent      */
/*         having two adjacent ghost vertices.)                              */
/*     7:  If both vertices are deleted, and the link ring is now empty.     */
/*         (A ghost vertex is deleted as well.)                              */
/*     8:  If the edge is deleted, but both vertices survive.  (In this      */
/*         case, a ghost vertex is inserted between them.)                   */
/*                                                                           */
/*    Hence, a positive return value implies that the edge was deleted, and  */
/*    the 4's bit signifies whether the link ring is empty on return.  If    */
/*    the result is not zero, the 1's bit signifies that `endpoint1' is no   */
/*    longer in the link ring (or never was), and the 2's bit signifies that */
/*    `endpoint2' is not in the link ring on return.  (If the result is      */
/*    zero, this procedure doesn't check whether the endpoints are present.) */
/*                                                                           */
/*****************************************************************************/

int linkringdeleteedge(struct proxipool *pool,
                       tag linkring,
                       tag groundtag,
                       tag endpoint1,
                       tag endpoint2)
{
  struct linkpossmall position;
  struct linkpossmall pt1position;
  struct linkpossmall pt2position;
  struct linkpossmall ghostposition;
  molecule cule;
  tag nextmoleculetag;
  tag moleculetag;
  int atomindex;
  int lasttextindex;
  char ghostatom[1];
  char nowatom;

  /* There is no "previous" vertex we've visited yet. */
  tag firstvertextag = STOP;
  tag prevvertextag = STOP;
  tag vertextag = 0;
  long vertexcount = 0;
  int vertexatoms = 0;
  int end1atoms = 0;
  int end2atoms = 0;
  /* Set to 1 during the first iteration of the "do" loop. */
  int firstflag = 1;
  /* Set if there is a ghost vertex immediately before `endpoint1'. */
  int ghostbefore1flag = 0;
  /* Set if there is a ghost vertex immediately after `endpoint2'. */
  int ghostafter2flag = 0;
  int earlystopflag = 0;

  if ((linkring >= STOP) || (endpoint1 >= STOP) || (endpoint2 >= STOP)) {
    /* Invalid parameters. */
    return 0;
  }

  /* Start at the first molecule. */
  nextmoleculetag = linkring;

  /* Loop through the linked list of molecules. */
  do {
    /* Convert the molecule's tag to a pointer. */
    cule = (molecule) proxipooltag2object(pool, nextmoleculetag);
    moleculetag = nextmoleculetag;
    /* Read the "next molecule" tag. */
    linkringreadtag(moleculetag, cule, lasttextindex, nextmoleculetag, nowatom,
                    "Internal error in linkringdeleteedge():\n");
    if (firstflag) {
      /* Position to insert or delete atoms at the beginning of the list. */
      position.cule = cule;
      position.moleculetag = moleculetag;
      position.textindex = 0;
      position.lasttextindex = lasttextindex;
      position.nextmoleculetag = nextmoleculetag;
      firstflag = 0;
    }

    atomindex = 0;
    /* Loop through the atoms, stopping at the "next molecule" tag. */
    while (atomindex <= lasttextindex) {
      nowatom = cule[atomindex];
      /* Append the atom to the vertex tag. */
      vertextag = (vertextag << 7) + (nowatom & (char) 127);
      vertexatoms++;
      atomindex++;

      /* Is this the last atom in the compressed tag? */
      if (nowatom < (char) 0) {
        /* Yes; we have an uncompressed tag now.  What tag is it? */
        if ((nowatom == (char) STOP) && (vertexatoms == 1)) {
          /* STOP tag.  Exit the inner loop early; go on to next molecule. */
          break;
        } else if ((nowatom == (char) GHOSTVERTEX) && (vertexatoms == 1)) {
          vertextag = GHOSTVERTEX;
          /* Remember this position in case we need to delete vertices here. */
          ghostposition = position;
          if (prevvertextag == endpoint2) {
            /* There's a ghost vertex immediately after `endpoint2'. */
            ghostafter2flag = 1;
          }
        } else {
          /* Use the ground tag to supply the high-order bits. */
          vertextag += (groundtag >> (7 * vertexatoms)) << (7 * vertexatoms);

          if (vertextag == endpoint2) {
            /* Remember this position in case we need to insert a ghost */
            /*   vertex here or delete `endpoint2'.                     */
            pt2position = position;
            end2atoms = vertexatoms;
          } else if (vertextag == endpoint1) {
            /* Remember this position in case we need to delete vertices */
            /*   here.                                                   */
            pt1position = position;
            end1atoms = vertexatoms;
            if (prevvertextag == GHOSTVERTEX) {
              /* There's a ghost vertex immediately before `endpoint1'. */
              ghostbefore1flag = 1;
            }
          }
        }

        vertexcount++;
        if (vertexcount == 1) {
          /* This is the first vertex.  Remember it. */
          firstvertextag = vertextag;
        } else if ((prevvertextag == endpoint1) ^ (vertextag == endpoint2)) {
          /* Either `endpoint1' occurs without `endpoint2' following it, or */
          /*   `endpoint2' occurs without `endpoint1' preceding it, so the  */
          /*   edge we seek to delete doesn't exist.                        */
          return 0;
        } else if ((prevvertextag == endpoint2) && (vertexcount > 3)) {
          /* If execution reaches here, the edge 1-2 does appear in the link */
          /*   ring, and we've collected enough information to determine     */
          /*   whether thre are ghost vertices immediately preceding and     */
          /*   following it.  Exit both loops early.                         */
          earlystopflag = 1;
          nextmoleculetag = STOP;
          break;
        }

        /* Remember this tag during the next iteration. */
        prevvertextag = vertextag;
        /* Prepare to read another tag. */
        vertextag = 0;
        vertexatoms = 0;

        /* Store the position of the beginning of the next compressed tag,   */
        /*   in case we realize we need it once we reach the end of the tag. */
        position.cule = cule;
        position.textindex = atomindex;
        position.lasttextindex = lasttextindex;
        position.moleculetag = moleculetag;
        position.nextmoleculetag = nextmoleculetag;
      }
    }
  } while (nextmoleculetag != STOP);

  if (nowatom >= (char) 0) {
    /* Oops.  The end of the last vertex is missing from the last molecule. */
    printf("Internal error in linkringdeleteedge():\n");
    printf("  Vertex at end of link ring not properly terminated.\n");
    internalerror();
  }

  if (!earlystopflag) {
    if (vertexcount < 2) {
      /* There are fewer than two vertices in the link ring, hence no edge. */
      return (vertexcount == 0) ? -1 : 0;
    } else if (firstvertextag == endpoint1) {
      /* Check if the last vertex in the linked list is a ghost vertex, as */
      /*   it immediately precedes `endpoint1' in the link ring.           */
      ghostbefore1flag = prevvertextag == GHOSTVERTEX;
    } else if (prevvertextag == endpoint2) {
      /* Check if the first vertex in the linked list is a ghost vertex, as */
      /*   it immediately follows `endpoint2' in the link ring.             */
      ghostafter2flag = firstvertextag == GHOSTVERTEX;
    } else if ((prevvertextag != endpoint1) || (firstvertextag != endpoint2)) {
      /* The edge 1-2 is not in the link ring. */
      return 0;
    }
  }

  /* If execution reaches this point, the edge 1-2 is in the link ring. */
  if (ghostafter2flag) {
    if (ghostbefore1flag) {
      /* There are ghost vertices immediately before and after the edge 1-2, */
      /*   and you can't have two ghosts in a row, so one of the ghosts must */
      /*   be deleted along with `endpoint1' and `endpoint2'.                */
      if (vertexcount <= 3) {
        /* Just edge 1-2 and a ghost in the ring.  Empty the link ring. */
        linkringrestart(pool, linkring);
        return 7;
      } else if (earlystopflag) {
        /* Delete the contiguous sequence 1-2-ghost. */
        linkringdeleteatoms(pool, &pt1position, end1atoms + end2atoms + 1);
      } else if (prevvertextag == endpoint1) {
        /* Delete the sequence ghost-1 from the end of the linked list. */
        linkringdeleteatoms(pool, &ghostposition, end1atoms + 1);
        /* The deletion above may have changed the "next molecule" tag */
        /*   stored in the molecule at `pt2position' to STOP.          */
        if (pt2position.cule[MOLECULESIZE - 1] == (char) STOP) {
          pt2position.nextmoleculetag = STOP;
          pt2position.lasttextindex = MOLECULESIZE - 2;
        }
        /* Delete `endpoint2' from the beginning of the list. */
        linkringdeleteatoms(pool, &pt2position, end2atoms);
      } else if (prevvertextag == endpoint2) {
        /* Delete the sequence ghost-1-2 from the end of the linked list. */
        linkringdeleteatoms(pool, &ghostposition, end1atoms + end2atoms + 1);
      } else {
        /* Delete a ghost vertex from the end of the linked list. */
        linkringdeleteatoms(pool, &ghostposition, 1);
        /* The deletion above may have changed the "next molecule" tag */
        /*   stored in the molecule at `pt1position' to STOP.          */
        if (pt1position.cule[MOLECULESIZE - 1] == (char) STOP) {
          pt1position.nextmoleculetag = STOP;
          pt1position.lasttextindex = MOLECULESIZE - 2;
        }
        /* Delete the sequence 1-2 from the beginning of the list. */
        linkringdeleteatoms(pool, &pt1position, end1atoms + end2atoms);
      }
      return 3;
    } else {
      /* There is a ghost vertex immediately after `endpoint2', but not   */
      /*   before `endpoint1', so `endpoint1' is shared with another edge */
      /*   and will survive.  Delete only `endpoint2'.                    */
      linkringdeleteatoms(pool, &pt2position, end2atoms);
      return 2;
    }
  } else {
    if (ghostbefore1flag) {
      /* There is a ghost vertex immediately before `endpoint1', but not */
      /*   after `endpoint2', so `endpoint2' is shared with another edge */
      /*   and will survive.  Delete only `endpoint1'.                   */
      linkringdeleteatoms(pool, &pt1position, end1atoms);
      return 1;
    } else {
      /* There is no ghost vertex immediately before or after edge 1-2, so   */
      /*   both vertices survive.  Insert a ghost vertex between `endpoint1' */
      /*   and `endpoint2'.                                                  */
      ghostatom[0] = (char) GHOSTVERTEX;
      if (prevvertextag == endpoint1) {
        /* Put the ghost vertex at the end of the linked list, immediately */
        /*   following `endpoint1'.                                        */
        linkringinsertatoms(pool, &position, 1, ghostatom,
                            proxipooltag2allocindex(pool, linkring));
      } else {
        /* Put the ghost vertex immediately before `endpoint2'. */
        linkringinsertatoms(pool, &pt2position, 1, ghostatom,
                            proxipooltag2allocindex(pool, linkring));
      }
      return 8;
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  linkringinsertvertex()   Insert a vertex into a link ring, immediately   */
/*                           following a specified vertex.                   */
/*                                                                           */
/*  The main purpose of this procedure is to provide a way to swap one edge  */
/*  for two.  For instance, if the link ring contains an edge 1-3, and you   */
/*  request that the vertex 2 be inserted after the vertex 1, then the edge  */
/*  1-3 is replaced by the edges 1-2 and 2-3.  (You can accomplish the same  */
/*  thing by calling linkringdeleteedge() and linkringinsertedge(), but it's */
/*  much slower.)                                                            */
/*                                                                           */
/*  If you're feeling sneaky, you can insert GHOSTVERTEX after vertex 1,     */
/*  thereby deleting the edge 1-3.  (But it's safer to do that with          */
/*  linkringdeleteedge().  Or you can insert a specified vertex after        */
/*  GHOSTVERTEX.  However, if there's more than one ghost vertex in the link */
/*  ring, you cannot control which ghost vertex the new vertex is inserted   */
/*  after.                                                                   */
/*                                                                           */
/*  WARNING:  For speed, this procedure does little error checking.  You can */
/*  inadvertently corrupt a link ring by inserting two copies of the same    */
/*  (non-ghost) vertex, or inserting a ghost vertex next to another ghost    */
/*  vertex.  You should never do the latter.  Inserting two copies of the    */
/*  same non-ghost vertex is useful for implementing the Bowyer-Watson       */
/*  algorithm, but if you do so, you cannot use linkringinsertedge() or      */
/*  linkringdeleteedge() to affect the duplicated vertex, and you must use   */
/*  linkringdelete2vertices() carefully to finish digging the cavity, so     */
/*  that the duplicated vertex will merge back into one copy in the end.     */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from, and that new */
/*    molecules may be allocated from.                                       */
/*  linkring:  Tag for the first molecule in the link ring.                  */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  searchvertex:  The tag of the vertex to search for.                      */
/*  newvertex:  The tag to insert after `searchvertex'.                      */
/*                                                                           */
/*  Returns 1 if `searchvertex' is in the link ring; 0 otherwise.  In the    */
/*    latter case, the link ring is not changed.                             */
/*                                                                           */
/*****************************************************************************/

int linkringinsertvertex(struct proxipool *pool,
                         tag linkring,
                         tag groundtag,
                         tag searchvertex,
                         tag newvertex)
{
  struct linkpossmall position;
  tag vertextag;
  int vertexatoms;
  int numatoms;
  char newvertexatoms[COMPRESSEDTAGLENGTH];
  char nowatom;

  if ((linkring >= STOP) ||
      (newvertex == searchvertex) || (newvertex == STOP)) {
    /* Invalid parameters. */
    return 0;
  }

  vertextag = 0;
  vertexatoms = 0;
  /* Start at the first molecule. */
  position.nextmoleculetag = linkring;

  /* Loop through the linked list of molecules. */
  do {
    /* Convert the molecule's tag to a pointer. */
    position.cule = (molecule) proxipooltag2object(pool, 
                                                   position.nextmoleculetag);
    position.moleculetag = position.nextmoleculetag;
    /* Read the "next molecule" tag. */
    linkringreadtag(position.moleculetag, position.cule,
                    position.lasttextindex, position.nextmoleculetag, nowatom,
                    "Internal error in linkringinsertvertex():\n");

    position.textindex = 0;
    /* Loop through the atoms, stopping at the "next molecule" tag. */
    while (position.textindex <= position.lasttextindex) {
      nowatom = position.cule[position.textindex];
      /* Append the atom to the vertex tag. */
      vertextag = (vertextag << 7) + (nowatom & (char) 127);
      vertexatoms++;
      position.textindex++;

      /* Is this the last atom in the compressed tag? */
      if (nowatom < (char) 0) {
        /* Yes; we have an uncompressed tag now.  What tag is it? */
        if ((nowatom == (char) STOP) && (vertexatoms == 1)) {
          /* STOP tag.  Exit the inner loop early; go on to next molecule. */
          break;
        } else if ((nowatom == (char) GHOSTVERTEX) && (vertexatoms == 1)) {
          /* Ghost vertex. */
          vertextag = GHOSTVERTEX;
        } else {
          /* Use the ground tag to supply the high-order bits. */
          vertextag += (groundtag >> (7 * vertexatoms)) << (7 * vertexatoms);
        }

        /* Is this the vertex we're searching for? */
        if (vertextag == searchvertex) {
          /* Insert the new vertex here and return. */
          numatoms = linkringtagcompress(groundtag, newvertex, newvertexatoms);
          linkringinsertatoms(pool, &position, numatoms, newvertexatoms,
                              proxipooltag2allocindex(pool, linkring));
          return 1;
        }

        /* Prepare to read another tag. */
        vertextag = 0;
        vertexatoms = 0;
      }
    }
  } while (position.nextmoleculetag != STOP);

  if (nowatom >= (char) 0) {
    /* Oops.  The end of the last vertex is missing from the last molecule. */
    printf("Internal error in linkringinsertvertex():\n");
    printf("  Link ring not properly terminated.\n");
    internalerror();
  }

  /* `searchvertex' is not in the link ring. */
  return 0;
}

/*****************************************************************************/
/*                                                                           */
/*  linkringdeletevertex()   Delete a vertex from a link ring.               */
/*                                                                           */
/*  The main purpose of this procedure is to provide a way to swap two edges */
/*  for one.  For instance, if the link ring contains the edges 1-2 and 2-3, */
/*  and you delete the vertex 2, then the two edges are replaced by the edge */
/*  1-3.  (You can accomplish the same thing by calling linkringdeleteedge() */
/*  and linkringinsertedge(), but it's much slower.)                         */
/*                                                                           */
/*  You can also use it to delete an edge from an end of a chain.  For       */
/*  example, if the edge 1-2 is the beginning of a chain (i.e. there is a    */
/*  ghost vertex before the vertex 1), then deleting the vertex 1 has the    */
/*  effect of deleting the edge 1-2.  If the edge 1-2 was the entire chain,  */
/*  then the vertex 2 survives as a lone vertex in the link ring, with ghost */
/*  vertices on both sides of it.  If you subsequently delete the vertex 2,  */
/*  this procedure will do the right thing and delete one of the ghost       */
/*  vertices as well.                                                        */
/*                                                                           */
/*  If you're feeling sneaky, you can delete GHOSTVERTEX, thereby creating   */
/*  an edge.  For example, deleting a ghost vertex between vertices 1 and 2  */
/*  creates the edge 1-2.  However, if there's more than one ghost vertex in */
/*  the link ring, you cannot control which ghost vertex is deleted.         */
/*                                                                           */
/*  WARNING:  If there is are multiple copies of `deletevertex' in the link  */
/*  ring (be it a ghost vertex or not), you cannot control which one gets    */
/*  deleted.                                                                 */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from.              */
/*  linkring:  Tag for the first molecule in the link ring.                  */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  deletevertex:  The tag to delete.                                        */
/*                                                                           */
/*  Returns one of the following values.                                     */
/*    -1:  If the link ring is empty on entry (so it is not changed).        */
/*     0:  If `deletevertex' is not present, and the link ring is unchanged  */
/*         (but not empty).                                                  */
/*     1:  If `deletevertex' was in the link ring on entry, and thus was     */
/*         deleted; but some other vertex survives.                          */
/*     3:  If `deletevertex' was the sole (non-ghost) vertex on entry, and   */
/*         thus was deleted, leaving the link ring empty.                    */
/*                                                                           */
/*    Hence, a positive return value implies that the vertex was deleted,    */
/*    and the 2's bit signifies whether the link ring is empty on return.    */
/*                                                                           */
/*****************************************************************************/

int linkringdeletevertex(struct proxipool *pool,
                         tag linkring,
                         tag groundtag,
                         tag deletevertex)
{
  struct linkpossmall position;
  struct linkpossmall deleteposition;
  struct linkpossmall ghostposition;
  molecule cule;
  tag nextmoleculetag;
  tag moleculetag;
  int atomindex;
  int lasttextindex;
  char nowatom;

  /* There is no "previous" vertex we've visited yet. */
  tag firstvertextag = STOP;
  tag prevvertextag = STOP;
  tag vertextag = 0;
  long vertexcount = 0;
  int vertexatoms = 0;
  int deleteatoms = 0;
  /* Set to 1 during the first iteration of the "do" loop. */
  int firstflag = 1;

  if (linkring >= STOP) {
    /* Not a link ring. */
    return 0;
  }

  /* Start at the first molecule. */
  nextmoleculetag = linkring;

  /* Loop through the linked list of molecules. */
  do {
    /* Convert the molecule's tag to a pointer. */
    cule = (molecule) proxipooltag2object(pool, nextmoleculetag);
    moleculetag = nextmoleculetag;
    /* Read the "next molecule" tag. */
    linkringreadtag(moleculetag, cule, lasttextindex, nextmoleculetag, nowatom,
                    "Internal error in linkringdeletevertex():\n");
    if (firstflag) {
      /* Position to delete atoms from the beginning of the linked list. */
      position.cule = cule;
      position.textindex = 0;
      position.lasttextindex = lasttextindex;
      position.nextmoleculetag = nextmoleculetag;
      firstflag = 0;
    }

    atomindex = 0;
    /* Loop through the atoms, stopping at the "next molecule" tag. */
    while (atomindex <= lasttextindex) {
      nowatom = cule[atomindex];
      /* Append the atom to the vertex tag. */
      vertextag = (vertextag << 7) + (nowatom & (char) 127);
      vertexatoms++;
      atomindex++;

      /* Is this the last atom in the compressed tag? */
      if (nowatom < (char) 0) {
        /* Yes; we have an uncompressed tag now.  What tag is it? */
        if ((nowatom == (char) STOP) && (vertexatoms == 1)) {
          /* STOP tag.  Exit the inner loop early; go on to next molecule. */
          break;
        } else if ((nowatom == (char) GHOSTVERTEX) && (vertexatoms == 1)) {
          vertextag = GHOSTVERTEX;
          /* Remember this position in case we need to delete the ghost. */
          ghostposition = position;
        } else {
          /* Use the ground tag to supply the high-order bits. */
          vertextag += (groundtag >> (7 * vertexatoms)) << (7 * vertexatoms);
        }

        vertexcount++;
        if (vertexcount == 1) {
          /* This is the first vertex.  Remember it. */
          firstvertextag = vertextag;
        }

        /* Is this the vertex we're searching for? */
        if (vertextag == deletevertex) {
          /* Yes.  If there are ghost vertices immediately before and after */
          /*   this vertex, we must delete one of them as well.  If this    */
          /*   vertex is at the beginning or end of the linked list, it     */
          /*   takes a bit of effort to check for this.                     */

          if ((prevvertextag != STOP) && (prevvertextag != GHOSTVERTEX)) {
            /* No ghost vertex before.  Just delete this vertex and return. */
            linkringdeleteatoms(pool, &position, vertexatoms);
            return 1;
          }

          /* Look ahead one atom to see if a ghost vertex is next. */
          if (atomindex <= lasttextindex) {
            nowatom = cule[atomindex];
          } else if (nextmoleculetag == STOP) {
            nowatom = (char) STOP;
          } else {
            /* The next atom is in the next molecule. */
            nowatom = ((molecule)
                       proxipooltag2object(pool, nextmoleculetag))[0];
          }

          if ((nowatom != (char) GHOSTVERTEX) &&
              ((nowatom != (char) STOP) ||
               (firstvertextag != GHOSTVERTEX))) {
            /* No ghost vertex after.  Just delete this vertex and return. */
            if ((vertexcount == 1) && (nowatom == (char) STOP)) {
              /* Empty the link ring. */
              linkringrestart(pool, linkring);
              return 3;
            } else {
              linkringdeleteatoms(pool, &position, vertexatoms);
              return 1;
            }
          }

          if (prevvertextag == GHOSTVERTEX) {
            /* Ghost vertex before and after.  Delete this vertex and the */
            /*   ghost vertex preceding it.  (Note that this also handles */
            /*   the case where there are only two vertices in the link   */
            /*   ring, so the same ghost vertex is before and after.)     */
            if ((vertexcount == 2) && (nowatom == (char) STOP)) {
              /* Empty the link ring. */
              linkringrestart(pool, linkring);
              return 3;
            } else {
              linkringdeleteatoms(pool, &ghostposition, vertexatoms + 1);
              return 1;
            }
          }

          /* If execution reaches this point, there is a ghost vertex after */
          /*   the vertex we want to delete; but `deletevertex' is at the   */
          /*   beginning of the linked list, so we won't know if there's a  */
          /*   ghost vertex before until we reach the end of the list.      */
          /*   Save this vertex's position for later deletion.              */
          deleteposition = position;
          deleteatoms = vertexatoms;
        }

        /* Remember this tag during the next iteration. */
        prevvertextag = vertextag;
        /* Prepare to read another tag. */
        vertextag = 0;
        vertexatoms = 0;

        /* Store the position of the beginning of the next compressed tag,   */
        /*   in case we realize we need it once we reach the end of the tag. */
        position.cule = cule;
        position.textindex = atomindex;
        position.lasttextindex = lasttextindex;
        position.nextmoleculetag = nextmoleculetag;
      }
    }
  } while (nextmoleculetag != STOP);

  if (nowatom >= (char) 0) {
    /* Oops.  The end of the last vertex is missing from the last molecule. */
    printf("Internal error in linkringdeletevertex():\n");
    printf("  Vertex at end of link ring not properly terminated.\n");
    internalerror();
  }

  /* If execution reaches here, either `deletevertex' is absent from the */
  /*   link ring, or the linked list begins with `deletevertex' followed */
  /*   by a ghost vertex.                                                */

  if (firstvertextag == deletevertex) {
    /* The linked list begins with `deletevertex' and a ghost vertex */
    /*   immediately following it.                                   */
    if (vertexcount <= 2) {
      /* Empty the link ring. */
      linkringrestart(pool, linkring);
      return 3;
    } else if (prevvertextag == GHOSTVERTEX) {
      /* The linked list ends with a ghost vertex, which immediately      */
      /*   precedes `deletevertex' in the link ring.  Delete the ghost    */
      /*   vertex at the end.  (It's better to leave a ghost vertex at    */
      /*   the beginning, to reduce the frequency of having to go through */
      /*   the whole list just to see if there's a ghost at the end.)     */
      linkringdeleteatoms(pool, &ghostposition, 1);
      /* The deletion above may have changed the "next molecule" tag stored */
      /*   in the molecule at `deleteposition' to STOP.                     */
      if (deleteposition.cule[MOLECULESIZE - 1] == (char) STOP) {
        deleteposition.nextmoleculetag = STOP;
        deleteposition.lasttextindex = MOLECULESIZE - 2;
      }
    }
    /* Delete the target vertex. */
    linkringdeleteatoms(pool, &deleteposition, deleteatoms);
    return 1;
  }

  /* `deletevertex' is not in the link ring. */
  if (vertexcount == 0) {
    return -1;
  } else {
    return 0;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  linkringdelete2vertices()   Delete a vertex and the vertex that follows  */
/*                              it from a link ring.                         */
/*                                                                           */
/*  This procedure differs from linkringdeletevertex() in that it does not   */
/*  check whether the deletion puts two ghost vertices next to each other;   */
/*  nor does it check whether the resulting link ring contains just one      */
/*  vertex (ghost or not).  It should not be used in circumstances where it  */
/*  will cause one of these problems.                                        */
/*                                                                           */
/*  The procedure's purpose is to help implement the Bowyer-Watson algorithm */
/*  for incremental insertion of a vertex into a Delaunay triangulation.     */
/*  It is used in the circumstance where the link ring contains two (or      */
/*  more) copies of the new vertex being inserted, with `deletevertex'       */
/*  sandwiched between them, which the Bowyer-Watson algorithm wants to      */
/*  delete.  Deleting just `deletevertex' would put two copies of the new    */
/*  vertex next to each other, but they should be merged into a single copy. */
/*  Therefore, this procedure deletes `deletevertex' and the vertex          */
/*  following it (which should be a copy of the new vertex) at once, leaving */
/*  behind one copy of the new vertex.  (That's why there are no checks for  */
/*  adjacent ghost vertices, nor for a one-vertex link ring.)  This          */
/*  procedure also works correctly in the case where there are only two      */
/*  vertices in the link ring, so the copies of the new vertex (before and   */
/*  after `deletevertex') are really the same copy.                          */
/*                                                                           */
/*  The procedure could be used for other purposes as well, so long as there */
/*  is no possibility of putting two ghost vertices together, or leaving     */
/*  behind just one ghost vertex and no other vertex.                        */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from.              */
/*  linkring:  Tag for the first molecule in the link ring.                  */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  deletevertex:  The tag to delete, along with its successor.              */
/*                                                                           */
/*  Returns one of the following values.                                     */
/*    -1:  If the link ring is empty on entry (so it is not changed).        */
/*     0:  If `deletevertex' is not present, and the link ring is unchanged  */
/*         (but not empty).                                                  */
/*     1:  If `deletevertex' was in the link ring on entry, and thus was     */
/*         deleted (with its successor); but some other vertex survives.     */
/*     3:  If `deletevertex' was deleted, and the link ring is now empty.    */
/*                                                                           */
/*    Hence, a positive return value implies that the vertex was deleted,    */
/*    and the 2's bit signifies whether the link ring is empty on return.    */
/*                                                                           */
/*****************************************************************************/

int linkringdelete2vertices(struct proxipool *pool,
                            tag linkring,
                            tag groundtag,
                            tag deletevertex)
{
  struct linkpossmall position;
  struct linkpossmall deleteposition;
  molecule cule;
  tag nextmoleculetag;
  tag moleculetag;
  int atomindex;
  int lasttextindex;
  char nowatom;

  /* There is no "previous" vertex we've visited yet. */
  tag prevvertextag = STOP;
  tag vertextag = 0;
  long vertexcount = 0;
  int vertexatoms = 0;
  int deleteatoms = 0;
  int firstatoms = 0;
  /* Set to 1 during the first iteration of the "do" loop. */
  int firstflag = 1;

  if (linkring >= STOP) {
    /* Not a link ring. */
    return 0;
  }

  /* Start at the first molecule. */
  nextmoleculetag = linkring;

  /* Loop through the linked list of molecules. */
  do {
    /* Convert the molecule's tag to a pointer. */
    cule = (molecule) proxipooltag2object(pool, nextmoleculetag);
    moleculetag = nextmoleculetag;
    /* Read the "next molecule" tag. */
    linkringreadtag(moleculetag, cule, lasttextindex, nextmoleculetag, nowatom,
                    "Internal error in linkringdelete2vertices():\n");
    if (firstflag) {
      /* Position to delete atoms from the beginning of the linked list. */
      position.cule = cule;
      position.textindex = 0;
      position.lasttextindex = lasttextindex;
      position.nextmoleculetag = nextmoleculetag;
      firstflag = 0;
    }

    atomindex = 0;
    /* Loop through the atoms, stopping at the "next molecule" tag. */
    while (atomindex <= lasttextindex) {
      nowatom = cule[atomindex];
      /* Append the atom to the vertex tag. */
      vertextag = (vertextag << 7) + (nowatom & (char) 127);
      vertexatoms++;
      atomindex++;

      /* Is this the last atom in the compressed tag? */
      if (nowatom < (char) 0) {
        /* Yes; we have an uncompressed tag now.  What tag is it? */
        if ((nowatom == (char) STOP) && (vertexatoms == 1)) {
          /* STOP tag.  Exit the inner loop early; go on to next molecule. */
          break;
        } else if ((nowatom == (char) GHOSTVERTEX) && (vertexatoms == 1)) {
          vertextag = GHOSTVERTEX;
        } else {
          /* Use the ground tag to supply the high-order bits. */
          vertextag += (groundtag >> (7 * vertexatoms)) << (7 * vertexatoms);
        }

        vertexcount++;
        if (vertexcount == 1) {
          /* This is the first vertex.  Remember its length. */
          firstatoms = vertexatoms;
        }

        /* Is the previous vertex the one we want to delete? */
        if (prevvertextag == deletevertex) {
          /* Yes; delete `deletevertex' and the following vertex. */
          linkringdeleteatoms(pool, &deleteposition,
                              deleteatoms + vertexatoms);
          /* Is the link ring empty now? */
          if ((vertexcount == 2) &&
              (((molecule) proxipooltag2object(pool, linkring))[0] ==
               (char) STOP)) {
            /* Yes. */
            return 3;
          } else {
            return 1;
          }
        } else if (vertextag == deletevertex) {
          /* This is the vertex we want to delete.  Remember its position. */
          deleteposition = position;
          deleteatoms = vertexatoms;
        }

        /* Remember this tag during the next iteration. */
        prevvertextag = vertextag;
        /* Prepare to read another tag. */
        vertextag = 0;
        vertexatoms = 0;

        /* Store the position of the beginning of the next compressed tag,   */
        /*   in case we realize we need it once we reach the end of the tag. */
        position.cule = cule;
        position.textindex = atomindex;
        position.lasttextindex = lasttextindex;
        position.nextmoleculetag = nextmoleculetag;
      }
    }
  } while (nextmoleculetag != STOP);

  if (nowatom >= (char) 0) {
    /* Oops.  The end of the last vertex is missing from the last molecule. */
    printf("Internal error in linkringdelete2vertices():\n");
    printf("  Vertex at end of link ring not properly terminated.\n");
    internalerror();
  }

  /* If execution reaches here, either `deletevertex' is absent from the */
  /*   link ring, or the linked list ends with `deletevertex'.           */

  if (prevvertextag == deletevertex) {
    if (vertexcount <= 2) {
      /* Empty the link ring. */
      linkringrestart(pool, linkring);
      return 3;
    } else {
      /* Delete `deletevertex' from the end first. */
      linkringdeleteatoms(pool, &deleteposition, deleteatoms);
      /* Delete one of the vertex copies from the beginning.  Note that the */
      /*   deletion above may have changed the first molecule in the list,  */
      /*   so we must re-read it.                                           */
      position.cule = (molecule) proxipooltag2object(pool, linkring);
      position.textindex = 0;
      linkringreadtag(linkring, position.cule, position.lasttextindex,
                      position.nextmoleculetag, nowatom,
                      "Internal error in linkringdelete2vertices():\n");
      linkringdeleteatoms(pool, &position, firstatoms);
      return 1;
    }
  }

  /* `deletevertex' is not in the link ring. */
  if (vertexcount == 0) {
    return -1;
  } else {
    return 0;
  }
}

/**                                                                         **/
/**                                                                         **/
/********* Link ring routines end here                               *********/


/********* 2D link/triangulation representation routines begin here  *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  Compressed 2D links                                                      */
/*                                                                           */
/*  The link of a vertex in a 3D triangulation is a topologically two-       */
/*  dimensional triangulation, embedded in 3D space.  If the vertex is in    */
/*  the interior of the triangulation, then its link is a triangulation of a */
/*  topological sphere.  (This data structure can store the link of any      */
/*  vertex in a 3D triangulation, though.)                                   */
/*                                                                           */
/*  The triangles in a 2D link should be thought of as sharing a fixed       */
/*  "orientation".  I choose the orientation that has the vertices of each   */
/*  triangle in counterclockwise order, as viewed by the vertex whose link   */
/*  it is.  Here I'm assuming that there are no geometrically "inverted"     */
/*  tetrahedra, but this code is deliberately oblivious to geometry--it      */
/*  never looks at a coordinate.  You could use all-clockwise if you prefer. */
/*  However, the topological orientations of the triangles should all be     */
/*  mutually consistent.  You could not, for example, represent an           */
/*  unoriented manifold (e.g. a Mobius strip or Klein bottle).               */
/*                                                                           */
/*  2D links are stored in a compressed format adapted from Blandford,       */
/*  Blelloch, Cardoze, and Kadow.  See the header for a full citation.       */
/*                                                                           */
/*  Each link ring is stored in a linked list of list nodes of type          */
/*  `molecule'.  Molecules are described in the comments for "Compressed     */
/*  link rings."  2D links use essentially the same representation, and have */
/*  mostly the same invariants, as link rings.  The molecules for 2D links   */
/*  and link rings are allocated from the same proxipool.                    */
/*                                                                           */
/*  Here are the invariants of 2D links that differ from link rings:         */
/*                                                                           */
/*  - The compressed tags in the linked list alternate between vertex tags   */
/*    and link ring tags.  Each vertex in the list is followed by the tag    */
/*    for its link ring.                                                     */
/*                                                                           */
/*  - Vertices in a 2D link may appear in any order.  Their order is not     */
/*    relevant.                                                              */
/*                                                                           */
/*  - A 2D link can contain a GHOSTVERTEX, but only one.  In a 2D link, the  */
/*    link ring of a ghost vertex represents ghost triangles and ghost       */
/*    tetrahedra.                                                            */
/*                                                                           */
/*  - A 2D link cannot contain an empty link ring.  When a link ring empties */
/*    out, it must be removed from any 2D link that contains it.             */
/*                                                                           */
/*  - Whereas each vertex tag is compressed against a "ground tag" (just     */
/*    like in link rings), each link ring tag is compressed against the tag  */
/*    for the 2D link.                                                       */
/*                                                                           */
/*  Public interface:                                                        */
/*  GHOSTVERTEX   Tag representing a ghost vertex; borrowed from linkring.   */
/*  STOP   Tag representing an unsuccessful query; borrowed from linkring.   */
/*  link2dcache   Type for a cache used to speed up link2dfindring().        */
/*  struct link2dposition   Represents a position in a 2D link.              */
/*  tag link2dnew(pool, allocindex)   Allocate a new, empty 2D link.         */
/*  void link2ddelete(pool, link)   Free a 2D link to the pool.              */
/*  void link2disempty(pool, link)   Determine if a 2D link is empty.        */
/*  void link2dcacheinit(cache)   Initialize/reset a 2D link cache to empty. */
/*  tag link2dfindring(pool, cache, link, groundtag, searchvertex)   Return  */
/*    the link ring associated with a vertex in a 2D link.                   */
/*  tag link2dfindinsert(pool, cache, linkhead, linktail, groundtag,         */
/*    searchvertex)   Return or create a vertex/link ring in a 2D link.      */
/*  void link2dinsertvertex(pool, cache, linkhead, linktail, groundtag,      */
/*    newvertex, linkring)   Insert a vertex/link ring in a 2D link.         */
/*  tag link2ddeletevertex(pool, cache, linkhead, linktail, groundtag,       */
/*    deletevertex)   Delete a vertex from a 2D link; return link ring.      */
/*  void link2diteratorinit(pool, link, groundtag, pos)   Initialize an      */
/*    iterator that traverses all the vertices in a link ring one by one.    */
/*  void link2diterate(pos, vertexandlink[2])   Read two tags (for a vertex  */
/*    and its link ring) and advance the iterator.                           */
/*  void link2dprint(pool, link, grountag)   Print the contents of a link.   */
/*                                                                           */
/*  For internal use only:                                                   */
/*  tag link2dfindringnocache(pool, link, groundtag, searchvertex)   Return  */
/*    the link ring associated with a vertex in a 2D link.                   */
/*  void link2dinsertvertexnocache(pool, linkhead, linktail, groundtag,      */
/*    newvertex, linkring)   Insert a vertex/link ring in a 2D link.         */
/*  tag link2ddeletevertexnocache(pool, linkhead, linktail, groundtag,       */
/*    deletevertex)   Delete a vertex from a 2D link; return link ring.      */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  The 2D link cache                                                        */
/*                                                                           */
/*  A user of the link2d procedures may optionally use a cache of type       */
/*  "link2dcache" to speed up the link2dfindring() procedure.  In my         */
/*  Delaunay triangulation implementation, this improves link2dfindring()    */
/*  from taking about 30% of the running time to taking about 3.5% of the    */
/*  running time.                                                            */
/*                                                                           */
/*  A cache should be initialized with link2dcacheinit() before use.  Once   */
/*  used, the cache must be passed to all subsequent calls to                */
/*  link2dinsertvertex(), link2dfindinsert(), and link2ddeletevertex() on    */
/*  the same proxipool.  Otherwise, the cache will be out of date, and       */
/*  link2dfindring() may subsequently return out-of-date information.  The   */
/*  cache is passed to link2dfindring() to improve its speed.                */
/*                                                                           */
/*  A cache cannot be used for more than one link proxipool simultaneously.  */
/*  The cache simply maps a 2D link tag and a vertex tag to a link ring tag, */
/*  with no knowledge of what pools the tags are drawn from.                 */
/*                                                                           */
/*  Any procedure that takes a cache as a parameter will accept a NULL       */
/*  pointer (if you don't want to bother with cacheing).                     */
/*                                                                           */
/*****************************************************************************/

/*  LINK2DCACHESIZE is the total number of entries in the cache that speeds  */
/*  up lookup of vertices in 2D links.  Because the cache is two-way         */
/*  associative, the number of buckets is half this number.  LINK2DCACHESIZE */
/*  _must_ be a power of two.                                                */

#define LINK2DCACHESIZE 16384

/*  LINK2DPRIME is a prime number used in a (not very good) hash function.   */

#define LINK2DPRIME 16908799u


/*  A link2dcache is an array that caches the link rings for 2D link/vertex  */
/*  pairs that have been recently looked up or updated.  A link cache        */
/*  greatly speeds up the link2dfindring() procedure.                        */

typedef struct {
  tag mylink2d;
  tag myvertex;
  tag mylinkring;
} link2dcache[LINK2DCACHESIZE];

/*  A link2dposition represents an atom in a 2D link.  It is the data        */
/*  structure used as an iterator that walks through a 2D link.              */

struct link2dposition {
  struct linkposition innerpos;                  /* Position in the 2D link. */
  tag linktag;     /* Tag for the link's first molecule, used to decompress. */
};


/*****************************************************************************/
/*                                                                           */
/*  link2dnew()   Allocate a new, empty 2D link triangulation.               */
/*                                                                           */
/*  The parameters include an allocation index, used to determine where the  */
/*  new link will be stored in memory.  Links with the same allocation index */
/*  go into common areas in memory.  The idea is to create spatial           */
/*  coherence:  links that are geometrically close to each other are near    */
/*  each other in memory, too.                                               */
/*                                                                           */
/*  pool:  The proxipool to allocate the link from.                          */
/*  allocindex:  An allocation index associated with the link.               */
/*                                                                           */
/*  Returns the tag of the first (and only) molecule of the new link.        */
/*                                                                           */
/*****************************************************************************/

tag link2dnew(struct proxipool *pool,
              proxipoolulong allocindex)
{
  molecule cule;
  tag newlink;

  /* Allocate a molecule to hold the (empty) link triangulation. */
  newlink = proxipoolnew(pool, allocindex, (void **) &cule);
  /* There are no vertices in this link. */
  cule[0] = (char) STOP;
  /* There is no next molecule. */
  cule[MOLECULESIZE - 1] = (char) STOP;

  return newlink;
}

/*****************************************************************************/
/*                                                                           */
/*  link2ddelete()   Free all the molecules in a 2D link to the pool.        */
/*                                                                           */
/*  Does not free the link rings referenced by the link.                     */
/*                                                                           */
/*  WARNING:  If a "link2dcache" is in use, this procedure does not clear    */
/*  entries in this 2D link from the cache.  If `link' is not empty, then    */
/*  then you should clear the cache with link2dcacheinit().                  */
/*                                                                           */
/*  pool:  The proxipool that the molecules were allocated from.             */
/*  link:  Tag for the first molecule in the link.                           */
/*                                                                           */
/*****************************************************************************/

void link2ddelete(struct proxipool *pool,
                  tag link)
{
  molecule cule;
  tag nextmoleculetag;
  tag moleculetag;
  int atomindex;
  char nowatom;

  if (link >= STOP) {
    return;
  }

  nextmoleculetag = link;

  /* Loop through the molecules and free them. */
  while (nextmoleculetag != STOP) {
    /* Convert the tag to a molecule. */
    cule = (molecule) proxipooltag2object(pool, nextmoleculetag);
#ifdef SELF_CHECK
    if (cule == (molecule) NULL) {
      printf("Internal error in link2ddelete():\n");
      printf("  Molecule tag indexes a NULL pointer.\n");
      internalerror();
    }
#endif /* SELF_CHECK */
    moleculetag = nextmoleculetag;
    /* Read the "next molecule tag". */
    linkringreadtag(moleculetag, cule, atomindex, nextmoleculetag, nowatom,
                    "Internal error in link2ddelete():\n");
    /* Free the molecule. */
    proxipoolfree(pool, moleculetag);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  link2disempty()   Determine if a 2D link is empty (or invalid).          */
/*                                                                           */
/*  pool:  The proxipool that the link was allocated from.                   */
/*  link:  Tag for the first molecule in the link.                           */
/*                                                                           */
/*  Returns 1 if the 2D link is empty (or the tag is invalid); 0 otherwise.  */
/*                                                                           */
/*****************************************************************************/

int link2disempty(struct proxipool *pool,
                  tag link)
{
  molecule cule;

  if (link >= STOP) {
    /* Not a valid link tag. */
    return 1;
  }

  /* Convert the tag to a molecule. */
  cule = (molecule) proxipooltag2object(pool, link);
  if ((cule == (molecule) NULL) || (cule[0] == (char) STOP)) {
    /* Missing or empty link. */
    return 1;
  } else {
    /* Not empty. */
    return 0;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  link2diteratorinit()   Initialize an iterator that traverses all the     */
/*                         vertices in a link one by one.                    */
/*                                                                           */
/*  The iterator is a variable `pos', whose internals should not be          */
/*  examined or modified by the client.  The iterator's job is to keep       */
/*  track of where it is in the link.  This procedure sets the iterator to   */
/*  reference the first vertex in the link.                                  */
/*                                                                           */
/*  When a link is modified, any iterators on that link may be corrupted and */
/*  should not be used without being initialized (by this procedure) again.  */
/*                                                                           */
/*  pool:  The proxipool that the link was allocated from.                   */
/*  link:  Tag for the first molecule in the link to traverse.               */
/*  groundtag:  The ground tag relative to which the vertices in the link    */
/*    will be decompressed.                                                  */
/*  pos:  The iterator.  Its contents do not need to be initialized prior to */
/*    calling this procedure.                                                */
/*                                                                           */
/*****************************************************************************/

void link2diteratorinit(struct proxipool *pool,
                        tag link,
                        tag groundtag,
                        struct link2dposition *pos)
{
  char nowatom;

  pos->linktag = link;
  pos->innerpos.groundtag = groundtag;
  pos->innerpos.pool = pool;
  if (link >= STOP) {
    /* Not a link. */
    pos->innerpos.cule = (molecule) NULL;
    pos->innerpos.moleculetag = STOP;
    pos->innerpos.nextmoleculetag = STOP;
    pos->innerpos.textindex = MOLECULESIZE;
    pos->innerpos.lasttextindex = -1;
  } else {
    /* Find the molecule identified by the tag `link'. */
    pos->innerpos.cule = (molecule) proxipooltag2object(pool, link);
#ifdef SELF_CHECK
    if (pos->innerpos.cule == (molecule) NULL) {
      printf("Internal error in link2diteratorinit():\n");
      printf("  Molecule tag indexes a NULL pointer.\n");
      internalerror();
    }
#endif /* SELF_CHECK */
    pos->innerpos.moleculetag = link;
    /* Read the molecule's "next molecule tag". */
    linkringreadtag(link, pos->innerpos.cule, pos->innerpos.lasttextindex,
                    pos->innerpos.nextmoleculetag, nowatom,
                    "Internal error in link2diteratorinit():\n");
    /* Start the iterations from the beginning of the molecule. */
    pos->innerpos.textindex = 0;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  link2diterate()   Return the two tags (for a vertex and its link ring)   */
/*                    that a 2D link iterator references, and advance the    */
/*                    iterator so it will return the next vertex next time.  */
/*                                                                           */
/*  The iterator is a variable `pos', whose internals should not be          */
/*  examined or modified by the client.                                      */
/*                                                                           */
/*  After a link is modified, any iterators on that link created before the  */
/*  modification may be corrupted and should not be passed to this procedure */
/*  again until they are re-initialized.                                     */
/*                                                                           */
/*  pos:  The iterator.                                                      */
/*  vertexandlink:  An array used to return the tags for a vertex (in        */
/*    vertexandlink[0]) and the link ring associated with it (in             */
/*    vertexandlink[1]).  If the iterator has reached the end of the list,   */
/*    both values will be STOP on return.  Does not need to be initialized   */
/*    before the call.                                                       */
/*                                                                           */
/*****************************************************************************/

void link2diterate(struct link2dposition *pos,
                   tag vertexandlink[2])
{
  tag texttag;
  int textatoms;
  int vertextagflag;
  char nowatom;

  texttag = 0;
  textatoms = 0;
  /* Set to 1 while reading a vertex tag; 0 when reading a link ring tag. */
  vertextagflag = 1;

  /* Loop through atoms to build up two tags. */
  while (1) {
    /* Have we read the last text atom in this molecule? */
    if ((pos->innerpos.textindex > pos->innerpos.lasttextindex) ||
        ((pos->innerpos.cule[pos->innerpos.textindex] == (char) STOP) &&
         (textatoms == 0))) {
      /* Yes, we have.  Are there any more molecules after this one? */
      if (pos->innerpos.nextmoleculetag == STOP) {
        /* No, there aren't.  The iterator is finished. */
#ifdef SELF_CHECK
        if (textatoms > 0) {
          /* The end of the last tag is missing from the last molecule. */
          printf("Internal error in link2diterate():\n");
          printf("  Tag at end of 2D link not properly terminated.\n");
          internalerror();
        }
        if (!vertextagflag) {
          /* There is a vertex tag without a link ring tag following it. */
          printf("Internal error in link2diterate():\n");
          printf("  Vertex tag not followed by link ring tag.\n");
          internalerror();
        }
#endif /* SELF_CHECK */

        vertexandlink[0] = STOP;
        vertexandlink[1] = STOP;
        return;
      }

      /* Find the next molecule in the linked list. */
      pos->innerpos.cule =
        (molecule) proxipooltag2object(pos->innerpos.pool,
                                       pos->innerpos.nextmoleculetag);
#ifdef SELF_CHECK
      if (pos->innerpos.cule == (molecule) NULL) {
        printf("Internal error in link2diterate():\n");
        printf("  Molecule tag indexes a NULL pointer.\n");
        internalerror();
      }
#endif /* SELF_CHECK */
      pos->innerpos.moleculetag = pos->innerpos.nextmoleculetag;
      /* Find the next molecule's "next molecule tag". */
      linkringreadtag(pos->innerpos.moleculetag, pos->innerpos.cule,
                      pos->innerpos.lasttextindex,
                      pos->innerpos.nextmoleculetag, nowatom,
                      "Internal error in link2diterate():\n");
      /* Start from the beginning of this molecule. */
      pos->innerpos.textindex = 0;
    }

    /* Read the next atom. */
    nowatom = pos->innerpos.cule[pos->innerpos.textindex];
    /* Append it to the tag. */
    texttag = (texttag << 7) + (nowatom & (char) 127);
    textatoms++;
    pos->innerpos.textindex++;

    /* If this is the end of a tag, store it for the caller. */
    if (nowatom < (char) 0) {
      if (vertextagflag) {
        if ((nowatom == (char) GHOSTVERTEX) && (textatoms == 1)) {
          vertexandlink[0] = GHOSTVERTEX;
        } else {
          /* Get the high-order bits from the ground vertex's tag. */
          vertexandlink[0] = texttag +
            ((pos->innerpos.groundtag >> (7 * textatoms)) << (7 * textatoms));
        }
        /* Prepare to read another tag. */
        texttag = 0;
        textatoms = 0;
        vertextagflag = 0;
      } else {
        /* Get the high-order bits from the first molecule's tag. */
        vertexandlink[1] = texttag +
          ((pos->linktag >> (7 * textatoms)) << (7 * textatoms));
        return;
      }
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  link2dprint()   Print the contents of a 2D link.                         */
/*                                                                           */
/*  pool:  The proxipool that the link was allocated from.                   */
/*  link:  Tag for the first molecule in the link.                           */
/*  groundtag:  The ground tag relative to which the vertices in the link    */
/*    will be decompressed.                                                  */
/*                                                                           */
/*****************************************************************************/

void link2dprint(struct proxipool *pool,
                 tag link,
                 tag groundtag)
{
  struct link2dposition pos;
  tag vertexandlink[2];

  printf("2D link %lu:\n", (unsigned long) link);
  link2diteratorinit(pool, link, groundtag, &pos);
  link2diterate(&pos, vertexandlink);
  if (vertexandlink[0] == STOP) {
    printf("  EMPTY\n");
  } else do {
    if (vertexandlink[0] == GHOSTVERTEX) {
      printf("  Vertex tag GHOST, ");
    } else {
      printf("  Vertex tag %lu, ", (unsigned long) vertexandlink[0]);
    }
    linkringprint(pool, vertexandlink[1], groundtag);
    link2diterate(&pos, vertexandlink);
  } while (vertexandlink[0] != STOP);
}

/*****************************************************************************/
/*                                                                           */
/*  link2dcacheinit()   Reset a 2D link cache to empty.                      */
/*                                                                           */
/*  A "link2dcache" should always be initialized before use.  It should also */
/*  be cleared by this procedure if you use link2ddelete() to free a 2D link */
/*  that is not empty.                                                       */
/*                                                                           */
/*  cache:  The cache to initialize.                                         */
/*                                                                           */
/*****************************************************************************/

void link2dcacheinit(link2dcache cache) {
  int i;

  for (i = 0; i < LINK2DCACHESIZE; i++) {
    cache[i].mylink2d = STOP;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  link2dfindringnocache()   Return the link ring associated with a vertex  */
/*                            in a 2D link.                                  */
/*                                                                           */
/*  pool:  The proxipool that the link was allocated from.                   */
/*  link:  Tag for the first molecule in the link.                           */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  searchvertex:  The tag of the vertex to search for.                      */
/*  linktail:  If (and only if) `searchvertex' is missing from the link AND  */
/*    `linktail' is not NULL, then on return, `*linktail' contains the tag   */
/*    for the last molecule in the 2D link.  Output only; the value of       */
/*    `*linktail' on entry is irrelevant.  Set `linktail' to NULL if you     */
/*    don't want this information.                                           */
/*                                                                           */
/*  Returns the tag of the link ring associated with `searchvertex' if       */
/*    `searchvertex' is in the link; STOP otherwise.                         */
/*                                                                           */
/*****************************************************************************/

tag link2dfindringnocache(struct proxipool *pool,
                          tag link,
                          tag groundtag,
                          tag searchvertex,
                          tag *linktail)
{
  molecule cule;
  tag nextmoleculetag;
  tag moleculetag;
  tag texttag;
  int textatoms;
  int atomindex;
  int lasttextindex;
  int vertextagflag;
  int matchflag;
  char nowatom;

  if (link >= STOP) {
    /* Not a link. */
    return STOP;
  }

  texttag = 0;
  textatoms = 0;
  /* Start at the first molecule. */
  nextmoleculetag = link;
  /* Set to 1 while reading a vertex tag; 0 when reading a link ring tag. */
  vertextagflag = 1;
  matchflag = 0;

  /* Loop through the linked list of molecules. */
  do {
    /* Convert the molecule's tag to a pointer. */
    cule = (molecule) proxipooltag2object(pool, nextmoleculetag);
#ifdef SELF_CHECK
    if (cule == (molecule) NULL) {
      printf("Internal error in link2dfindringnocache():\n");
      printf("  Molecule tag indexes a NULL pointer.\n");
      internalerror();
    }
#endif /* SELF_CHECK */
    moleculetag = nextmoleculetag;
    /* Read the "next molecule tag". */
    linkringreadtag(moleculetag, cule, lasttextindex, nextmoleculetag, nowatom,
                    "Internal error in link2dfindringnocache():\n");

    atomindex = 0;
    /* Loop through the atoms, stopping at the "next molecule tag". */
    while (atomindex <= lasttextindex) {
      nowatom = cule[atomindex];
      /* Append the atom to the text tag. */
      texttag = (texttag << 7) + (nowatom & (char) 127);
      textatoms++;
      atomindex++;

      /* Is this the last atom in the compressed tag? */
      if (nowatom < (char) 0) {
        /* Yes; we have an uncompressed tag now.  What tag is it? */
        if ((nowatom == (char) STOP) && (textatoms == 1)) {
          /* STOP tag.  Exit the inner loop early; go on to next molecule. */
          break;
        } else if (vertextagflag) {
          /* Is this the vertex we're searching for? */
          if ((nowatom == (char) GHOSTVERTEX) && (textatoms == 1)) {
            /* Ghost vertex. */
            matchflag = searchvertex == GHOSTVERTEX;
          } else {
            /* Use the ground tag to supply the high-order bits of the */
            /*   vertex tag.                                           */
            matchflag =
              searchvertex ==
              texttag + ((groundtag >> (7 * textatoms)) << (7 * textatoms));
          }
        } else if (matchflag) {
          /* Use the link tag to supply the high-order bits of the */
          /*   link ring.  Return the latter.                      */
          return texttag + ((link >> (7 * textatoms)) << (7 * textatoms));
        }

        /* Prepare to read another tag. */
        texttag = 0;
        textatoms = 0;
        /* Alternate between reading vertex tags and link ring tags. */
        vertextagflag = !vertextagflag;
      }
    }
  } while (nextmoleculetag != STOP);

#ifdef SELF_CHECK
  if (nowatom >= (char) 0) {
    /* Oops.  The end of the last tag is missing from the last molecule. */
    printf("Internal error in link2dfindringnocache():\n");
    printf("  Tag at end of 2D link not properly terminated.\n");
    internalerror();
  }
  if (!vertextagflag) {
    /* There is a vertex tag without a link ring tag following it. */
    printf("Internal error in link2dfindringnocache():\n");
    printf("  Vertex tag not followed by link ring tag.\n");
    internalerror();
  }
#endif /* SELF_CHECK */

  /* `searchvertex' is not in the link. */
  if (linktail != (tag *) NULL) {
    *linktail = moleculetag;
  }
  return STOP;
}

/*****************************************************************************/
/*                                                                           */
/*  link2dfindring()   Return the link ring associated with a vertex in a 2D */
/*                     link.                                                 */
/*                                                                           */
/*  Resorts to calling link2dfindringnocache() if the answer is not in the   */
/*  cache.                                                                   */
/*                                                                           */
/*  pool:  The proxipool that the link was allocated from.                   */
/*  cache:  An (optional) cache that may speed up the lookup.                */
/*  link:  Tag for the first molecule in the link.                           */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  searchvertex:  The tag of the vertex to search for.                      */
/*                                                                           */
/*  Returns the tag of the link ring associated with `searchvertex' if       */
/*    `searchvertex' is in the link; STOP otherwise.                         */
/*                                                                           */
/*****************************************************************************/

tag link2dfindring(struct proxipool *pool,
                   link2dcache cache,
                   tag link,
                   tag groundtag,
                   tag searchvertex)
{
  int hash;
  tag linkring;

  if (link >= STOP) {
    /* Not a link. */
    return STOP;
  } else if (cache == NULL) {
    return link2dfindringnocache(pool, link, groundtag, searchvertex,
                                 (tag *) NULL);
  }

  /* Hash the tags `link' and `searchvertex' to an even location in the */
  /*   cache.                                                           */
  hash = (LINK2DPRIME * link * link + 2u * searchvertex) &
         (LINK2DCACHESIZE - 2);
  /* Is cache entry at the even location a hit? */
  if ((cache[hash].mylink2d == link) &&
      (cache[hash].myvertex == searchvertex)) {
    /* Cache hit; we're done. */
    return cache[hash].mylinkring;
  }
  /* Is cache entry at the odd location (adding one) a hit? */
  if ((cache[hash + 1].mylink2d == link) &&
      (cache[hash + 1].myvertex == searchvertex)) {
    /* Cache hit. */
    linkring = cache[hash + 1].mylinkring;
  } else {
    /* Cache miss.  Find the link ring the slow way. */
    linkring = link2dfindringnocache(pool, link, groundtag, searchvertex,
                                     (tag *) NULL);
  }

  /* Make the sought link ring be the first (even) entry at this cache      */
  /*   location, and move whatever is currently there into the second (odd) */
  /*   entry.  The former second entry will no longer be in the cache.      */
  cache[hash + 1].mylink2d = cache[hash].mylink2d;
  cache[hash + 1].myvertex = cache[hash].myvertex;
  cache[hash + 1].mylinkring = cache[hash].mylinkring;
  cache[hash].mylink2d = link;
  cache[hash].myvertex = searchvertex;
  cache[hash].mylinkring = linkring;

  return linkring;
}

/*****************************************************************************/
/*                                                                           */
/*  link2dinsertvertexnocache()   Insert a vertex into a 2D link, associated */
/*                                with a link ring.                          */
/*                                                                           */
/*  This procedure does not check whether the vertex `newvertex' is already  */
/*  in the link.  It's the client's responsibility not to call this          */
/*  procedure if the vertex is already there.  If it might be there or might */
/*  not, call link2dfindring() first, or use link2dfindinsert() instead.     */
/*                                                                           */
/*  pool:  The proxipool that the link was allocated from, and that new      */
/*    molecules will be allocated from.                                      */
/*  linkhead:  Tag for the first molecule in the link.                       */
/*  linktail:  Tag for the last molecule in the link.  Passed by pointer,    */
/*    and may be modified on return.                                         */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  newvertex:  The new vertex to insert into the link.                      */
/*  linkring:  The link ring associated with the vertex `newvertex'.         */
/*                                                                           */
/*****************************************************************************/

void link2dinsertvertexnocache(struct proxipool *pool,
                               tag linkhead,
                               tag *linktail,
                               tag groundtag,
                               tag newvertex,
                               tag linkring)
{
  struct linkpossmall position;
  int numatoms;
  int starttagflag;
  char newtextatoms[2 * COMPRESSEDTAGLENGTH];

  if ((linkhead >= STOP) || (*linktail >= STOP) ||
      (newvertex == STOP) || (linkring >= STOP)) {
    /* Invalid parameters. */
    return;
  }

  /* Start at the last molecule. */
  position.moleculetag = *linktail;
  position.nextmoleculetag = STOP;
  /* Convert the molecule's tag to a pointer. */
  position.cule = (molecule) proxipooltag2object(pool, position.moleculetag);
#ifdef SELF_CHECK
  if (position.cule == (molecule) NULL) {
    printf("Internal error in link2dinsertvertexnocache():\n");
    printf("  Molecule tag indexes a NULL pointer.\n");
    internalerror();
  }
#endif /* SELF_CHECK */
  if (position.cule[MOLECULESIZE - 1] != (char) STOP) {
    /* Oops.  `linktail' isn't really a tail molecule. */
    printf("Internal error in link2dinsertvertexnocache():\n");
    printf("  Parameter `linktail' is not the tail of a list.\n");
    internalerror();
  }
  position.lasttextindex = MOLECULESIZE - 2;

  /* Search for the end of the atoms in the link.  If the head and tail   */
  /*   molecules are the same, the list might be empty, in which case the */
  /*   very first atom is STOP.  Otherwise, the tail molecule must begin  */
  /*   with at least one non-STOP atom (this is an invariant of the link  */
  /*   data structure), so we check the stop bit of the first atom to see */
  /*   whether the second atom might be a STOP atom.                      */
  if (linkhead == *linktail) {
    starttagflag = 1;
    position.textindex = 0;
  } else {
    starttagflag = position.cule[0] < (char) 0;
    position.textindex = 1;
  }
  /* Loop through the atoms, looking for a STOP tag. */
  while ((position.cule[position.textindex] != (char) STOP) ||
         !starttagflag) {
    if (position.textindex >= MOLECULESIZE - 1) {
      /* Oops.  The end of the last tag is missing from the last molecule. */
      printf("Internal error in link2dinsertvertexnocache():\n");
      printf("  Link ring not properly terminated.\n");
      internalerror();
    }
    starttagflag = position.cule[position.textindex] < (char) 0;
    position.textindex++;
  }

  numatoms = linkringtagcompress(linkhead, linkring, newtextatoms);
  numatoms += linkringtagcompress(groundtag, newvertex,
                                  &newtextatoms[numatoms]);
  *linktail = linkringinsertatoms(pool, &position, numatoms, newtextatoms,
                                  proxipooltag2allocindex(pool, linkhead));
}

/*****************************************************************************/
/*                                                                           */
/*  link2dinsertvertex()   Insert a vertex into a 2D link, associated with   */
/*                         a link ring.                                      */
/*                                                                           */
/*  This procedure does not check whether the vertex `newvertex' is already  */
/*  in the link.  It's the client's responsibility not to call this          */
/*  procedure if the vertex is already there.  If you're not sure it's not   */
/*  already in the link, use link2dfindinsert() instead, or call             */
/*  link2dfindring() first.                                                  */
/*                                                                           */
/*  This procedure updates the cache to reflect the insertion, then calls    */
/*  link2dinsertvertexnocache() to perform the insertion.                    */
/*                                                                           */
/*  pool:  The proxipool that the link was allocated from, and that new      */
/*    molecules will be allocated from.                                      */
/*  cache:  An (optional) cache that will be updated by the insertion.       */
/*  linkhead:  Tag for the first molecule in the link.                       */
/*  linktail:  Tag for the last molecule in the link.  Passed by pointer,    */
/*    and modified on return.                                                */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  newvertex:  The new vertex to insert into the link.                      */
/*  linkring:  The link ring associated with the vertex `newvertex'.         */
/*                                                                           */
/*****************************************************************************/

void link2dinsertvertex(struct proxipool *pool,
                        link2dcache cache,
                        tag linkhead,
                        tag *linktail,
                        tag groundtag,
                        tag newvertex,
                        tag linkring)
{
  int hash;

  if ((linkhead >= STOP) || (*linktail >= STOP) ||
      (newvertex == STOP) || (linkring >= STOP)) {
    /* Invalid parameters. */
    return;
  }

  if (cache != NULL) {
    hash = (LINK2DPRIME * linkhead * linkhead + 2u * newvertex) &
           (LINK2DCACHESIZE - 2);
    if ((cache[hash].mylink2d != linkhead) ||
        (cache[hash].myvertex != newvertex)) {
      cache[hash + 1].mylink2d = cache[hash].mylink2d;
      cache[hash + 1].myvertex = cache[hash].myvertex;
      cache[hash + 1].mylinkring = cache[hash].mylinkring;
      cache[hash].mylink2d = linkhead;
      cache[hash].myvertex = newvertex;
    }
    cache[hash].mylinkring = linkring;
  }

  return link2dinsertvertexnocache(pool, linkhead, linktail, groundtag,
                                   newvertex, linkring);
}

/*****************************************************************************/
/*                                                                           */
/*  link2dfindinsert()   Return the link ring associated with a vertex in a  */
/*                       2D link.  If the vertex is not in the link, create  */
/*                       an empty link ring for it and add it to the link.   */
/*                                                                           */
/*  pool:  The proxipool that the link was allocated from, and that new      */
/*    molecules will be allocated from.                                      */
/*  cache:  An (optional) cache that may speed up the lookup, and will be    */
/*    updated if the vertex is inserted.                                     */
/*  linkhead:  Tag for the first molecule in the link.                       */
/*  linktail:  Tag for the last molecule in the link.  Passed by pointer,    */
/*    and may be modified on return.                                         */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  searchvertex:  The tag of the vertex to search for, and possibly insert. */
/*                                                                           */
/*  Returns the link ring associated with `searchvertex' (or STOP if         */
/*    `linkhead' isn't a legal tag--namely, if it's STOP or GHOSTVERTEX).    */
/*                                                                           */
/*****************************************************************************/

tag link2dfindinsert(struct proxipool *pool,
                     link2dcache cache,
                     tag linkhead,
                     tag *linktail,
                     tag groundtag,
                     tag searchvertex)
{
  tag linkring;

  if (linkhead >= STOP) {
    /* Not a link. */
    return STOP;
  }

  linkring = link2dfindring(pool, cache, linkhead, groundtag, searchvertex);
  if (linkring == STOP) {
    linkring = linkringnew(pool, proxipooltag2allocindex(pool, linkhead));
    link2dinsertvertex(pool, cache, linkhead, linktail, groundtag,
                       searchvertex, linkring);
  }

  return linkring;
}

/*****************************************************************************/
/*                                                                           */
/*  link2ddeletevertexnocache()   Delete a vertex from a 2D link.  Also      */
/*                                returns the link ring associated with the  */
/*                                vertex.                                    */
/*                                                                           */
/*  This procedure does not free the link ring's molecules.                  */
/*                                                                           */
/*  pool:  The proxipool that the link was allocated from.                   */
/*  linkhead:  Tag for the first molecule in the link.                       */
/*  linktail:  Tag for the last molecule in the link.  Passed by pointer,    */
/*    and may be modified on return.                                         */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  deletevertex:  The tag of the vertex to delete.                          */
/*                                                                           */
/*  Returns the tag of the link ring associated with `deletevertex' if       */
/*    `deletevertex' is in the link (in which case it is deleted from the    */
/*    link); STOP otherwise.                                                 */
/*                                                                           */
/*****************************************************************************/

tag link2ddeletevertexnocache(struct proxipool *pool,
                              tag linkhead,
                              tag *linktail,
                              tag groundtag,
                              tag deletevertex)
{
  struct linkpossmall position;
  molecule cule;
  tag nextmoleculetag;
  tag moleculetag;
  tag texttag;
  int textatoms;
  int vertexatoms;
  int atomindex;
  int lasttextindex;
  int vertextagflag;
  int firstflag;
  int matchflag;
  char nowatom;

  if (linkhead >= STOP) {
    /* Not a link. */
    return STOP;
  }

  texttag = 0;
  textatoms = 0;
  vertexatoms = 0;
  /* Start at the first molecule. */
  nextmoleculetag = linkhead;
  /* Set to 1 while reading a vertex tag; 0 when reading a link ring tag. */
  vertextagflag = 1;
  /* Set to 1 during the first iteration of the "do" loop. */
  firstflag = 1;
  matchflag = 0;

  /* Loop through the linked list of molecules. */
  do {
    /* Convert the molecule's tag to a pointer. */
    cule = (molecule) proxipooltag2object(pool, nextmoleculetag);
#ifdef SELF_CHECK
    if (cule == (molecule) NULL) {
      printf("Internal error in link2ddeletevertexnocache():\n");
      printf("  Molecule tag indexes a NULL pointer.\n");
      internalerror();
    }
#endif /* SELF_CHECK */
    moleculetag = nextmoleculetag;
    /* Read the "next molecule tag". */
    linkringreadtag(moleculetag, cule, lasttextindex, nextmoleculetag, nowatom,
                    "Internal error in link2ddeletevertexnocache():\n");
    if (firstflag) {
      /* Position to delete atoms from the beginning of the linked list. */
      position.cule = cule;
      position.moleculetag = moleculetag;
      position.textindex = 0;
      position.lasttextindex = lasttextindex;
      position.nextmoleculetag = nextmoleculetag;
      firstflag = 0;
    }

    atomindex = 0;
    /* Loop through the atoms, stopping at the "next molecule tag". */
    while (atomindex <= lasttextindex) {
      nowatom = cule[atomindex];
      /* Append the atom to the text tag. */
      texttag = (texttag << 7) + (nowatom & (char) 127);
      textatoms++;
      atomindex++;

      /* Is this the last atom in the compressed tag? */
      if (nowatom < (char) 0) {
        /* Yes; we have an uncompressed tag now.  What tag is it? */
        if ((nowatom == (char) STOP) && (textatoms == 1)) {
          /* STOP tag.  Exit the inner loop early; go on to next molecule. */
          break;
        } else if (vertextagflag) {
          /* Is this the vertex we're searching for? */
          if ((nowatom == (char) GHOSTVERTEX) && (textatoms == 1)) {
            /* Ghost vertex. */
            matchflag = deletevertex == GHOSTVERTEX;
          } else {
            /* Use the ground tag to supply the high-order bits of the */
            /*   vertex tag.                                           */
            matchflag =
              deletevertex ==
              texttag + ((groundtag >> (7 * textatoms)) << (7 * textatoms));
          }
          vertexatoms = textatoms;
        } else if (matchflag) {
          /* Delete the vertex and its link ring. */
          *linktail = linkringdeleteatoms(pool, &position,
                                          vertexatoms + textatoms);
          /* Use the link tag to supply the high-order bits of the */
          /*   link ring.  Return the latter.                      */
          return texttag + ((linkhead >> (7 * textatoms)) << (7 * textatoms));
        } else {
          /* Store the position of the beginning of the next compressed tag, */
          /*   in case the vertex starting here proves to be `deletevertex'. */
          position.cule = cule;
          position.moleculetag = moleculetag;
          position.textindex = atomindex;
          position.lasttextindex = lasttextindex;
          position.nextmoleculetag = nextmoleculetag;
        }

        /* Prepare to read another tag. */
        texttag = 0;
        textatoms = 0;
        /* Alternate between reading vertex tags and link ring tags. */
        vertextagflag = !vertextagflag;
      }
    }
  } while (nextmoleculetag != STOP);

  if (nowatom >= (char) 0) {
    /* Oops.  The end of the last tag is missing from the last molecule. */
    printf("Internal error in link2ddeletevertexnocache():\n");
    printf("  Tag at end of 2D link not properly terminated.\n");
    internalerror();
  }

  /* `deletevertex' is not in the link. */
  return STOP;
}

/*****************************************************************************/
/*                                                                           */
/*  link2ddeletevertex()   Delete a vertex from a 2D link.  Also returns the */
/*                         link ring associated with the vertex.             */
/*                                                                           */
/*  This procedure does not free the link ring's molecules.                  */
/*                                                                           */
/*  This procedure updates the cache to reflect the deletion, then calls     */
/*  link2ddeletevertexnocache() to perform the deletion (unless the first    */
/*  cache entry says that the vertex is not present).                        */
/*                                                                           */
/*  pool:  The proxipool that the link was allocated from.                   */
/*  cache:  An (optional) cache that will be updated by the deletion.        */
/*  linkhead:  Tag for the first molecule in the link.                       */
/*  linktail:  Tag for the last molecule in the link.  Passed by pointer,    */
/*    and may be modified on return.                                         */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  deletevertex:  The tag of the vertex to delete.                          */
/*                                                                           */
/*  Returns the tag of the link ring associated with `deletevertex' if       */
/*    `deletevertex' is in the link (in which case it is deleted from the    */
/*    link); STOP otherwise.                                                 */
/*                                                                           */
/*****************************************************************************/

tag link2ddeletevertex(struct proxipool *pool,
                       link2dcache cache,
                       tag linkhead,
                       tag *linktail,
                       tag groundtag,
                       tag deletevertex)
{
  int hash;

  if (linkhead >= STOP) {
    /* Not a link. */
    return STOP;
  }

  if (cache != NULL) {
    hash = (LINK2DPRIME * linkhead * linkhead + 2u * deletevertex) &
           (LINK2DCACHESIZE - 2);
    if ((cache[hash].mylink2d == linkhead) &&
        (cache[hash].myvertex == deletevertex)) {
      if (cache[hash].mylinkring == STOP) {
        return STOP;
      }
    } else {
      cache[hash + 1].mylink2d = cache[hash].mylink2d;
      cache[hash + 1].myvertex = cache[hash].myvertex;
      cache[hash + 1].mylinkring = cache[hash].mylinkring;
      cache[hash].mylink2d = linkhead;
      cache[hash].myvertex = deletevertex;
    }
    cache[hash].mylinkring = STOP;
  }

  return link2ddeletevertexnocache(pool, linkhead, linktail, groundtag,
                                   deletevertex);
}

/**                                                                         **/
/**                                                                         **/
/********* 2D link/triangulation representation routines end here    *********/


/********* Geometric primitives begin here                           *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  Robust geometric predicates using adaptive precision floating-point      */
/*  arithmetic                                                               */
/*                                                                           */
/*  The adaptive exact arithmetic geometric predicates implemented herein    */
/*  are described in detail in my paper, "Adaptive Precision Floating-Point  */
/*  Arithmetic and Fast Robust Geometric Predicates."  See the header for a  */
/*  full citation.                                                           */
/*                                                                           */
/*****************************************************************************/

/*  Which of the following two methods of finding the absolute values is     */
/*  fastest is compiler-dependent.  A few compilers can inline and optimize  */
/*  the fabs() call; but most will incur the overhead of a function call,    */
/*  which is disastrously slow.  A faster way on IEEE machines might be to   */
/*  mask the appropriate bit.                                                */

#define Absolute(a)  ((a) >= 0.0 ? (a) : -(a))
/* #define Absolute(a)  fabs(a) */

/*  Many of the operations are broken up into two pieces--a main part that   */
/*  performs an approximate operation, and a "tail" that computes the        */
/*  roundoff error of that operation.                                        */
/*                                                                           */
/*  The operations Fast_Two_Sum(), Fast_Two_Diff(), Two_Sum(), Two_Diff(),   */
/*  Split(), and Two_Product() are all implemented as described in the       */
/*  reference.  Each of these macros requires certain variables to be        */
/*  defined in the calling routine.  The variables `bvirt', `c', `abig',     */
/*  `_i', `_j', `_k', `_l', `_m', and `_n' are declared `INEXACT' because    */
/*  they store the result of an operation that may incur roundoff error.     */
/*  The input parameter `x' (or the highest numbered `x_' parameter) must    */
/*  also be declared `INEXACT'.                                              */

#define Fast_Two_Sum_Tail(a, b, x, y) \
  bvirt = x - a; \
  y = b - bvirt

#define Fast_Two_Sum(a, b, x, y) \
  x = (starreal) (a + b); \
  Fast_Two_Sum_Tail(a, b, x, y)

#define Two_Sum_Tail(a, b, x, y) \
  bvirt = (starreal) (x - a); \
  avirt = x - bvirt; \
  bround = b - bvirt; \
  around = a - avirt; \
  y = around + bround

#define Two_Sum(a, b, x, y) \
  x = (starreal) (a + b); \
  Two_Sum_Tail(a, b, x, y)

#define Two_Diff_Tail(a, b, x, y) \
  bvirt = (starreal) (a - x); \
  avirt = x + bvirt; \
  bround = bvirt - b; \
  around = a - avirt; \
  y = around + bround

#define Two_Diff(a, b, x, y) \
  x = (starreal) (a - b); \
  Two_Diff_Tail(a, b, x, y)

#define Split(a, ahi, alo) \
  c = (starreal) (splitter * a); \
  abig = (starreal) (c - a); \
  ahi = c - abig; \
  alo = a - ahi

#define Two_Product_Tail(a, b, x, y) \
  Split(a, ahi, alo); \
  Split(b, bhi, blo); \
  err1 = x - (ahi * bhi); \
  err2 = err1 - (alo * bhi); \
  err3 = err2 - (ahi * blo); \
  y = (alo * blo) - err3

#define Two_Product(a, b, x, y) \
  x = (starreal) (a * b); \
  Two_Product_Tail(a, b, x, y)

/*  Two_Product_Presplit() is Two_Product() where one of the inputs has      */
/*  already been split.  Avoids redundant splitting.                         */

#define Two_Product_Presplit(a, b, bhi, blo, x, y) \
  x = (starreal) (a * b); \
  Split(a, ahi, alo); \
  err1 = x - (ahi * bhi); \
  err2 = err1 - (alo * bhi); \
  err3 = err2 - (ahi * blo); \
  y = (alo * blo) - err3

/*  Square() can be done more quickly than Two_Product().                    */

#define Square_Tail(a, x, y) \
  Split(a, ahi, alo); \
  err1 = x - (ahi * ahi); \
  err3 = err1 - ((ahi + ahi) * alo); \
  y = (alo * alo) - err3

#define Square(a, x, y) \
  x = (starreal) (a * a); \
  Square_Tail(a, x, y)

/*  Macros for summing expansions of various fixed lengths.  These are all   */
/*  unrolled versions of Expansion_Sum().                                    */

#define Two_One_Sum(a1, a0, b, x2, x1, x0) \
  Two_Sum(a0, b , _i, x0); \
  Two_Sum(a1, _i, x2, x1)

#define Two_One_Diff(a1, a0, b, x2, x1, x0) \
  Two_Diff(a0, b , _i, x0); \
  Two_Sum( a1, _i, x2, x1)

#define Two_Two_Sum(a1, a0, b1, b0, x3, x2, x1, x0) \
  Two_One_Sum(a1, a0, b0, _j, _0, x0); \
  Two_One_Sum(_j, _0, b1, x3, x2, x1)

#define Two_Two_Diff(a1, a0, b1, b0, x3, x2, x1, x0) \
  Two_One_Diff(a1, a0, b0, _j, _0, x0); \
  Two_One_Diff(_j, _0, b1, x3, x2, x1)

#define Four_One_Sum(a3, a2, a1, a0, b, x4, x3, x2, x1, x0) \
  Two_One_Sum(a1, a0, b , _j, x1, x0); \
  Two_One_Sum(a3, a2, _j, x4, x3, x2)

#define Four_Two_Sum(a3, a2, a1, a0, b1, b0, x5, x4, x3, x2, x1, x0) \
  Four_One_Sum(a3, a2, a1, a0, b0, _k, _2, _1, _0, x0); \
  Four_One_Sum(_k, _2, _1, _0, b1, x5, x4, x3, x2, x1)

#define Four_Four_Sum(a3, a2, a1, a0, b4, b3, b1, b0, x7, x6, x5, x4, x3, x2, \
                      x1, x0) \
  Four_Two_Sum(a3, a2, a1, a0, b1, b0, _l, _2, _1, _0, x1, x0); \
  Four_Two_Sum(_l, _2, _1, _0, b4, b3, x7, x6, x5, x4, x3, x2)

#define Eight_One_Sum(a7, a6, a5, a4, a3, a2, a1, a0, b, x8, x7, x6, x5, x4, \
                      x3, x2, x1, x0) \
  Four_One_Sum(a3, a2, a1, a0, b , _j, x3, x2, x1, x0); \
  Four_One_Sum(a7, a6, a5, a4, _j, x8, x7, x6, x5, x4)

#define Eight_Two_Sum(a7, a6, a5, a4, a3, a2, a1, a0, b1, b0, x9, x8, x7, \
                      x6, x5, x4, x3, x2, x1, x0) \
  Eight_One_Sum(a7, a6, a5, a4, a3, a2, a1, a0, b0, _k, _6, _5, _4, _3, _2, \
                _1, _0, x0); \
  Eight_One_Sum(_k, _6, _5, _4, _3, _2, _1, _0, b1, x9, x8, x7, x6, x5, x4, \
                x3, x2, x1)

#define Eight_Four_Sum(a7, a6, a5, a4, a3, a2, a1, a0, b4, b3, b1, b0, x11, \
                       x10, x9, x8, x7, x6, x5, x4, x3, x2, x1, x0) \
  Eight_Two_Sum(a7, a6, a5, a4, a3, a2, a1, a0, b1, b0, _l, _6, _5, _4, _3, \
                _2, _1, _0, x1, x0); \
  Eight_Two_Sum(_l, _6, _5, _4, _3, _2, _1, _0, b4, b3, x11, x10, x9, x8, \
                x7, x6, x5, x4, x3, x2)

/*  Macro for multiplying a two-component expansion by a single component.   */

#define Two_One_Product(a1, a0, b, x3, x2, x1, x0) \
  Split(b, bhi, blo); \
  Two_Product_Presplit(a0, b, bhi, blo, _i, x0); \
  Two_Product_Presplit(a1, b, bhi, blo, _j, _0); \
  Two_Sum(_i, _0, _k, x1); \
  Fast_Two_Sum(_j, _k, x3, x2)

/*****************************************************************************/
/*                                                                           */
/*  primitivesinit()   Initialize the variables used for exact arithmetic.   */
/*                                                                           */
/*  `epsilon' is the largest power of two such that 1.0 + epsilon = 1.0 in   */
/*  floating-point arithmetic.  `epsilon' bounds the relative roundoff       */
/*  error.  It is used for floating-point error analysis.                    */
/*                                                                           */
/*  `splitter' is used to split floating-point numbers into two half-        */
/*  length significands for exact multiplication.                            */
/*                                                                           */
/*  I imagine that a highly optimizing compiler might be too smart for its   */
/*  own good, and somehow cause this routine to fail, if it pretends that    */
/*  floating-point arithmetic is too much like real arithmetic.              */
/*                                                                           */
/*  Don't change this routine unless you really understand it.               */
/*                                                                           */
/*****************************************************************************/

void primitivesinit(void)
{
  starreal half;
  starreal check, lastcheck;
  int every_other;
#ifdef LINUX
  int cword;
#endif /* LINUX */

#ifdef CPU86
#ifdef SINGLE
  _control87(_PC_24, _MCW_PC); /* Set FPU control word for single precision. */
#else /* not SINGLE */
  _control87(_PC_53, _MCW_PC); /* Set FPU control word for double precision. */
#endif /* not SINGLE */
#endif /* CPU86 */
#ifdef LINUX
#ifdef SINGLE
  /*  cword = 4223; */
  cword = 4210;                 /* set FPU control word for single precision */
#else /* not SINGLE */
  /*  cword = 4735; */
  cword = 4722;                 /* set FPU control word for double precision */
#endif /* not SINGLE */
  _FPU_SETCW(cword);
#endif /* LINUX */

  every_other = 1;
  half = 0.5;
  epsilon = 1.0;
  splitter = 1.0;
  check = 1.0;
  /* Repeatedly divide `epsilon' by two until it is too small to add to      */
  /*   one without causing roundoff.  (Also check if the sum is equal to     */
  /*   the previous sum, for machines that round up instead of using exact   */
  /*   rounding.  Not that these routines will work on such machines.)       */
  do {
    lastcheck = check;
    epsilon *= half;
    if (every_other) {
      splitter *= 2.0;
    }
    every_other = !every_other;
    check = 1.0 + epsilon;
  } while ((check != 1.0) && (check != lastcheck));
  splitter += 1.0;
  /* Error bounds for orientation and insphere tests. */
  resulterrbound = (3.0 + 8.0 * epsilon) * epsilon;
  o2derrboundA = (3.0 + 16.0 * epsilon) * epsilon;
  o2derrboundB = (2.0 + 12.0 * epsilon) * epsilon;
  o2derrboundC = (9.0 + 64.0 * epsilon) * epsilon * epsilon;
  o3derrboundA = (7.0 + 56.0 * epsilon) * epsilon;
  o3derrboundB = (3.0 + 28.0 * epsilon) * epsilon;
  o3derrboundC = (26.0 + 288.0 * epsilon) * epsilon * epsilon;
  isperrboundA = (16.0 + 224.0 * epsilon) * epsilon;
  isperrboundB = (5.0 + 72.0 * epsilon) * epsilon;
  isperrboundC = (71.0 + 1408.0 * epsilon) * epsilon * epsilon;
}

/*****************************************************************************/
/*                                                                           */
/*  fast_expansion_sum_zeroelim()   Sum two expansions, eliminating zero     */
/*                                  components from the output expansion.    */
/*                                                                           */
/*  Sets h = e + f.  See my Robust Predicates paper for details.             */
/*                                                                           */
/*  h cannot be e or f.                                                      */
/*                                                                           */
/*  If round-to-even is used (as with IEEE 754), maintains the strongly      */
/*  nonoverlapping property.  (That is, if e is strongly nonoverlapping, h   */
/*  will be also.)  Does NOT maintain the nonoverlapping or nonadjacent      */
/*  properties.                                                              */
/*                                                                           */
/*****************************************************************************/

int fast_expansion_sum_zeroelim(int elen,
                                starreal *e,
                                int flen,
                                starreal *f,
                                starreal *h)
{
  starreal Q;
  INEXACT starreal Qnew;
  INEXACT starreal hh;
  INEXACT starreal bvirt;
  starreal avirt, bround, around;
  int eindex, findex, hindex;
  starreal enow, fnow;

  enow = e[0];
  fnow = f[0];
  eindex = findex = 0;
  if ((fnow > enow) == (fnow > -enow)) {
    Q = enow;
    enow = e[++eindex];
  } else {
    Q = fnow;
    fnow = f[++findex];
  }
  hindex = 0;
  if ((eindex < elen) && (findex < flen)) {
    if ((fnow > enow) == (fnow > -enow)) {
      Fast_Two_Sum(enow, Q, Qnew, hh);
      enow = e[++eindex];
    } else {
      Fast_Two_Sum(fnow, Q, Qnew, hh);
      fnow = f[++findex];
    }
    Q = Qnew;
    if (hh != 0.0) {
      h[hindex++] = hh;
    }
    while ((eindex < elen) && (findex < flen)) {
      if ((fnow > enow) == (fnow > -enow)) {
        Two_Sum(Q, enow, Qnew, hh);
        enow = e[++eindex];
      } else {
        Two_Sum(Q, fnow, Qnew, hh);
        fnow = f[++findex];
      }
      Q = Qnew;
      if (hh != 0.0) {
        h[hindex++] = hh;
      }
    }
  }
  while (eindex < elen) {
    Two_Sum(Q, enow, Qnew, hh);
    enow = e[++eindex];
    Q = Qnew;
    if (hh != 0.0) {
      h[hindex++] = hh;
    }
  }
  while (findex < flen) {
    Two_Sum(Q, fnow, Qnew, hh);
    fnow = f[++findex];
    Q = Qnew;
    if (hh != 0.0) {
      h[hindex++] = hh;
    }
  }
  if ((Q != 0.0) || (hindex == 0)) {
    h[hindex++] = Q;
  }
  return hindex;
}

/*****************************************************************************/
/*                                                                           */
/*  scale_expansion_zeroelim()   Multiply an expansion by a scalar,          */
/*                               eliminating zero components from the        */
/*                               output expansion.                           */
/*                                                                           */
/*  Sets h = be.  See my Robust Predicates paper for details.                */
/*                                                                           */
/*  e and h cannot be the same.                                              */
/*                                                                           */
/*  Maintains the nonoverlapping property.  If round-to-even is used (as     */
/*  with IEEE 754), maintains the strongly nonoverlapping and nonadjacent    */
/*  properties as well.  (That is, if e has one of these properties, so      */
/*  will h.)                                                                 */
/*                                                                           */
/*****************************************************************************/

int scale_expansion_zeroelim(int elen,
                             starreal *e,
                             starreal b,
                             starreal *h)
{
  INEXACT starreal Q, sum;
  starreal hh;
  INEXACT starreal product1;
  starreal product0;
  int eindex, hindex;
  starreal enow;
  INEXACT starreal bvirt;
  starreal avirt, bround, around;
  INEXACT starreal c;
  INEXACT starreal abig;
  starreal ahi, alo, bhi, blo;
  starreal err1, err2, err3;

  Split(b, bhi, blo);
  Two_Product_Presplit(e[0], b, bhi, blo, Q, hh);
  hindex = 0;
  if (hh != 0) {
    h[hindex++] = hh;
  }
  for (eindex = 1; eindex < elen; eindex++) {
    enow = e[eindex];
    Two_Product_Presplit(enow, b, bhi, blo, product1, product0);
    Two_Sum(Q, product0, sum, hh);
    if (hh != 0) {
      h[hindex++] = hh;
    }
    Fast_Two_Sum(product1, sum, Q, hh);
    if (hh != 0) {
      h[hindex++] = hh;
    }
  }
  if ((Q != 0.0) || (hindex == 0)) {
    h[hindex++] = Q;
  }
  return hindex;
}

/*****************************************************************************/
/*                                                                           */
/*  estimate()   Produce a one-word estimate of an expansion's value.        */
/*                                                                           */
/*  See my Robust Predicates paper for details.                              */
/*                                                                           */
/*****************************************************************************/

starreal estimate(int elen,
                  starreal *e)
{
  starreal Q;
  int eindex;

  Q = e[0];
  for (eindex = 1; eindex < elen; eindex++) {
    Q += e[eindex];
  }
  return Q;
}

/*****************************************************************************/
/*                                                                           */
/*  orient2d()   Calculate the determinant | ax ay 1 |                       */
/*                                         | bx by 1 |                       */
/*                                         | cx cy 1 |                       */
/*                                                                           */
/*  Uses exact arithmetic if necessary to ensure a correct answer.  The      */
/*  determinant is computed adaptively, in the sense that exact arithmetic   */
/*  is used only to the degree it is needed to ensure that the returned      */
/*  value has the correct sign.  Hence, this function is usually quite fast, */
/*  but will run more slowly when the input points are collinear or nearly   */
/*  so.                                                                      */
/*                                                                           */
/*  See my Robust Predicates paper for details.                              */
/*                                                                           */
/*****************************************************************************/

starreal orient2dadapt(starreal ax,
                       starreal ay,
                       starreal bx,
                       starreal by,
                       starreal cx,
                       starreal cy,
                       starreal detsum)
{
  INEXACT starreal acx, acy, bcx, bcy;
  starreal acxtail, acytail, bcxtail, bcytail;
  INEXACT starreal detleft, detright;
  starreal detlefttail, detrighttail;
  starreal det, errbound;
  starreal B[4], C1[8], C2[12], D[16];
  INEXACT starreal B3;
  int C1length, C2length, Dlength;
  starreal u[4];
  INEXACT starreal u3;
  INEXACT starreal s1, t1;
  starreal s0, t0;

  INEXACT starreal bvirt;
  starreal avirt, bround, around;
  INEXACT starreal c;
  INEXACT starreal abig;
  starreal ahi, alo, bhi, blo;
  starreal err1, err2, err3;
  INEXACT starreal _i, _j;
  starreal _0;

  acx = (starreal) (ax - cx);
  bcx = (starreal) (bx - cx);
  acy = (starreal) (ay - cy);
  bcy = (starreal) (by - cy);

  Two_Product(acx, bcy, detleft, detlefttail);
  Two_Product(acy, bcx, detright, detrighttail);

  Two_Two_Diff(detleft, detlefttail, detright, detrighttail,
               B3, B[2], B[1], B[0]);
  B[3] = B3;

  det = estimate(4, B);
  errbound = o2derrboundB * detsum;
  if ((det >= errbound) || (-det >= errbound)) {
    return det;
  }

  Two_Diff_Tail(ax, cx, acx, acxtail);
  Two_Diff_Tail(bx, cx, bcx, bcxtail);
  Two_Diff_Tail(ay, cy, acy, acytail);
  Two_Diff_Tail(by, cy, bcy, bcytail);

  if ((acxtail == 0.0) && (acytail == 0.0) &&
      (bcxtail == 0.0) && (bcytail == 0.0)) {
    return det;
  }

  errbound = o2derrboundC * detsum + resulterrbound * Absolute(det);
  det += (acx * bcytail + bcy * acxtail) -
         (acy * bcxtail + bcx * acytail);
  if ((det >= errbound) || (-det >= errbound)) {
    return det;
  }

  Two_Product(acxtail, bcy, s1, s0);
  Two_Product(acytail, bcx, t1, t0);
  Two_Two_Diff(s1, s0, t1, t0, u3, u[2], u[1], u[0]);
  u[3] = u3;
  C1length = fast_expansion_sum_zeroelim(4, B, 4, u, C1);

  Two_Product(acx, bcytail, s1, s0);
  Two_Product(acy, bcxtail, t1, t0);
  Two_Two_Diff(s1, s0, t1, t0, u3, u[2], u[1], u[0]);
  u[3] = u3;
  C2length = fast_expansion_sum_zeroelim(C1length, C1, 4, u, C2);

  Two_Product(acxtail, bcytail, s1, s0);
  Two_Product(acytail, bcxtail, t1, t0);
  Two_Two_Diff(s1, s0, t1, t0, u3, u[2], u[1], u[0]);
  u[3] = u3;
  Dlength = fast_expansion_sum_zeroelim(C2length, C2, 4, u, D);

  return(D[Dlength - 1]);
}

starreal orient2d(struct behavior *b,
                  starreal ax,
                  starreal ay,
                  starreal bx,
                  starreal by,
                  starreal cx,
                  starreal cy)
{
  starreal detleft, detright, det;
  starreal detsum, errbound;

  detleft = (ax - cx) * (by - cy);
  detright = (ay - cy) * (bx - cx);
  det = detleft - detright;

  if (b->noexact) {
    return det;
  }

  if (detleft > 0.0) {
    if (detright <= 0.0) {
      return det;
    } else {
      detsum = detleft + detright;
    }
  } else if (detleft < 0.0) {
    if (detright >= 0.0) {
      return det;
    } else {
      detsum = -detleft - detright;
    }
  } else {
    return det;
  }

  errbound = o2derrboundA * detsum;
  if ((det >= errbound) || (-det >= errbound)) {
    return det;
  }

  return orient2dadapt(ax, ay, bx, by, cx, cy, detsum);
}

/*****************************************************************************/
/*                                                                           */
/*  orient3d()   Return a positive value if the point pd lies below the      */
/*               plane passing through pa, pb, and pc; "below" is defined so */
/*               that pa, pb, and pc appear in counterclockwise order when   */
/*               viewed from above the plane.  Returns a negative value if   */
/*               pd lies above the plane.  Returns zero if the points are    */
/*               coplanar.  The result is also a rough approximation of six  */
/*               times the signed volume of the tetrahedron defined by the   */
/*               four points.                                                */
/*                                                                           */
/*  Uses exact arithmetic if necessary to ensure a correct answer.  The      */
/*  result returned is the determinant of a matrix.  This determinant is     */
/*  computed adaptively, in the sense that exact arithmetic is used only to  */
/*  the degree it is needed to ensure that the returned value has the        */
/*  correct sign.  Hence, this function is usually quite fast, but will run  */
/*  more slowly when the input points are coplanar or nearly so.             */
/*                                                                           */
/*  See my Robust Predicates paper for details.                              */
/*                                                                           */
/*****************************************************************************/

starreal orient3dadapt(starreal *pa,
                       starreal *pb,
                       starreal *pc,
                       starreal *pd,
                       starreal permanent)
{
  INEXACT starreal adx, bdx, cdx, ady, bdy, cdy, adz, bdz, cdz;
  starreal det, errbound;

  INEXACT starreal bdxcdy1, cdxbdy1, cdxady1, adxcdy1, adxbdy1, bdxady1;
  starreal bdxcdy0, cdxbdy0, cdxady0, adxcdy0, adxbdy0, bdxady0;
  starreal bc[4], ca[4], ab[4];
  INEXACT starreal bc3, ca3, ab3;
  starreal adet[8], bdet[8], cdet[8];
  int alen, blen, clen;
  starreal abdet[16];
  int ablen;
  starreal *finnow, *finother, *finswap;
  starreal fin1[192], fin2[192];
  int finlength;

  starreal adxtail, bdxtail, cdxtail;
  starreal adytail, bdytail, cdytail;
  starreal adztail, bdztail, cdztail;
  INEXACT starreal at_blarge, at_clarge;
  INEXACT starreal bt_clarge, bt_alarge;
  INEXACT starreal ct_alarge, ct_blarge;
  starreal at_b[4], at_c[4], bt_c[4], bt_a[4], ct_a[4], ct_b[4];
  int at_blen, at_clen, bt_clen, bt_alen, ct_alen, ct_blen;
  INEXACT starreal bdxt_cdy1, cdxt_bdy1, cdxt_ady1;
  INEXACT starreal adxt_cdy1, adxt_bdy1, bdxt_ady1;
  starreal bdxt_cdy0, cdxt_bdy0, cdxt_ady0;
  starreal adxt_cdy0, adxt_bdy0, bdxt_ady0;
  INEXACT starreal bdyt_cdx1, cdyt_bdx1, cdyt_adx1;
  INEXACT starreal adyt_cdx1, adyt_bdx1, bdyt_adx1;
  starreal bdyt_cdx0, cdyt_bdx0, cdyt_adx0;
  starreal adyt_cdx0, adyt_bdx0, bdyt_adx0;
  starreal bct[8], cat[8], abt[8];
  int bctlen, catlen, abtlen;
  INEXACT starreal bdxt_cdyt1, cdxt_bdyt1, cdxt_adyt1;
  INEXACT starreal adxt_cdyt1, adxt_bdyt1, bdxt_adyt1;
  starreal bdxt_cdyt0, cdxt_bdyt0, cdxt_adyt0;
  starreal adxt_cdyt0, adxt_bdyt0, bdxt_adyt0;
  starreal u[4], v[12], w[16];
  INEXACT starreal u3;
  int vlength, wlength;
  starreal negate;

  INEXACT starreal bvirt;
  starreal avirt, bround, around;
  INEXACT starreal c;
  INEXACT starreal abig;
  starreal ahi, alo, bhi, blo;
  starreal err1, err2, err3;
  INEXACT starreal _i, _j, _k;
  starreal _0;

  adx = (starreal) (pa[0] - pd[0]);
  bdx = (starreal) (pb[0] - pd[0]);
  cdx = (starreal) (pc[0] - pd[0]);
  ady = (starreal) (pa[1] - pd[1]);
  bdy = (starreal) (pb[1] - pd[1]);
  cdy = (starreal) (pc[1] - pd[1]);
  adz = (starreal) (pa[2] - pd[2]);
  bdz = (starreal) (pb[2] - pd[2]);
  cdz = (starreal) (pc[2] - pd[2]);

  Two_Product(bdx, cdy, bdxcdy1, bdxcdy0);
  Two_Product(cdx, bdy, cdxbdy1, cdxbdy0);
  Two_Two_Diff(bdxcdy1, bdxcdy0, cdxbdy1, cdxbdy0, bc3, bc[2], bc[1], bc[0]);
  bc[3] = bc3;
  alen = scale_expansion_zeroelim(4, bc, adz, adet);

  Two_Product(cdx, ady, cdxady1, cdxady0);
  Two_Product(adx, cdy, adxcdy1, adxcdy0);
  Two_Two_Diff(cdxady1, cdxady0, adxcdy1, adxcdy0, ca3, ca[2], ca[1], ca[0]);
  ca[3] = ca3;
  blen = scale_expansion_zeroelim(4, ca, bdz, bdet);

  Two_Product(adx, bdy, adxbdy1, adxbdy0);
  Two_Product(bdx, ady, bdxady1, bdxady0);
  Two_Two_Diff(adxbdy1, adxbdy0, bdxady1, bdxady0, ab3, ab[2], ab[1], ab[0]);
  ab[3] = ab3;
  clen = scale_expansion_zeroelim(4, ab, cdz, cdet);

  ablen = fast_expansion_sum_zeroelim(alen, adet, blen, bdet, abdet);
  finlength = fast_expansion_sum_zeroelim(ablen, abdet, clen, cdet, fin1);

  det = estimate(finlength, fin1);
  errbound = o3derrboundB * permanent;
  if ((det >= errbound) || (-det >= errbound)) {
    return det;
  }

  Two_Diff_Tail(pa[0], pd[0], adx, adxtail);
  Two_Diff_Tail(pb[0], pd[0], bdx, bdxtail);
  Two_Diff_Tail(pc[0], pd[0], cdx, cdxtail);
  Two_Diff_Tail(pa[1], pd[1], ady, adytail);
  Two_Diff_Tail(pb[1], pd[1], bdy, bdytail);
  Two_Diff_Tail(pc[1], pd[1], cdy, cdytail);
  Two_Diff_Tail(pa[2], pd[2], adz, adztail);
  Two_Diff_Tail(pb[2], pd[2], bdz, bdztail);
  Two_Diff_Tail(pc[2], pd[2], cdz, cdztail);

  if ((adxtail == 0.0) && (bdxtail == 0.0) && (cdxtail == 0.0) &&
      (adytail == 0.0) && (bdytail == 0.0) && (cdytail == 0.0) &&
      (adztail == 0.0) && (bdztail == 0.0) && (cdztail == 0.0)) {
    return det;
  }

  errbound = o3derrboundC * permanent + resulterrbound * Absolute(det);
  det += (adz * ((bdx * cdytail + cdy * bdxtail)
                 - (bdy * cdxtail + cdx * bdytail))
          + adztail * (bdx * cdy - bdy * cdx))
       + (bdz * ((cdx * adytail + ady * cdxtail)
                 - (cdy * adxtail + adx * cdytail))
          + bdztail * (cdx * ady - cdy * adx))
       + (cdz * ((adx * bdytail + bdy * adxtail)
                 - (ady * bdxtail + bdx * adytail))
          + cdztail * (adx * bdy - ady * bdx));
  if ((det >= errbound) || (-det >= errbound)) {
    return det;
  }

  finnow = fin1;
  finother = fin2;

  if (adxtail == 0.0) {
    if (adytail == 0.0) {
      at_b[0] = 0.0;
      at_blen = 1;
      at_c[0] = 0.0;
      at_clen = 1;
    } else {
      negate = -adytail;
      Two_Product(negate, bdx, at_blarge, at_b[0]);
      at_b[1] = at_blarge;
      at_blen = 2;
      Two_Product(adytail, cdx, at_clarge, at_c[0]);
      at_c[1] = at_clarge;
      at_clen = 2;
    }
  } else {
    if (adytail == 0.0) {
      Two_Product(adxtail, bdy, at_blarge, at_b[0]);
      at_b[1] = at_blarge;
      at_blen = 2;
      negate = -adxtail;
      Two_Product(negate, cdy, at_clarge, at_c[0]);
      at_c[1] = at_clarge;
      at_clen = 2;
    } else {
      Two_Product(adxtail, bdy, adxt_bdy1, adxt_bdy0);
      Two_Product(adytail, bdx, adyt_bdx1, adyt_bdx0);
      Two_Two_Diff(adxt_bdy1, adxt_bdy0, adyt_bdx1, adyt_bdx0,
                   at_blarge, at_b[2], at_b[1], at_b[0]);
      at_b[3] = at_blarge;
      at_blen = 4;
      Two_Product(adytail, cdx, adyt_cdx1, adyt_cdx0);
      Two_Product(adxtail, cdy, adxt_cdy1, adxt_cdy0);
      Two_Two_Diff(adyt_cdx1, adyt_cdx0, adxt_cdy1, adxt_cdy0,
                   at_clarge, at_c[2], at_c[1], at_c[0]);
      at_c[3] = at_clarge;
      at_clen = 4;
    }
  }
  if (bdxtail == 0.0) {
    if (bdytail == 0.0) {
      bt_c[0] = 0.0;
      bt_clen = 1;
      bt_a[0] = 0.0;
      bt_alen = 1;
    } else {
      negate = -bdytail;
      Two_Product(negate, cdx, bt_clarge, bt_c[0]);
      bt_c[1] = bt_clarge;
      bt_clen = 2;
      Two_Product(bdytail, adx, bt_alarge, bt_a[0]);
      bt_a[1] = bt_alarge;
      bt_alen = 2;
    }
  } else {
    if (bdytail == 0.0) {
      Two_Product(bdxtail, cdy, bt_clarge, bt_c[0]);
      bt_c[1] = bt_clarge;
      bt_clen = 2;
      negate = -bdxtail;
      Two_Product(negate, ady, bt_alarge, bt_a[0]);
      bt_a[1] = bt_alarge;
      bt_alen = 2;
    } else {
      Two_Product(bdxtail, cdy, bdxt_cdy1, bdxt_cdy0);
      Two_Product(bdytail, cdx, bdyt_cdx1, bdyt_cdx0);
      Two_Two_Diff(bdxt_cdy1, bdxt_cdy0, bdyt_cdx1, bdyt_cdx0,
                   bt_clarge, bt_c[2], bt_c[1], bt_c[0]);
      bt_c[3] = bt_clarge;
      bt_clen = 4;
      Two_Product(bdytail, adx, bdyt_adx1, bdyt_adx0);
      Two_Product(bdxtail, ady, bdxt_ady1, bdxt_ady0);
      Two_Two_Diff(bdyt_adx1, bdyt_adx0, bdxt_ady1, bdxt_ady0,
                  bt_alarge, bt_a[2], bt_a[1], bt_a[0]);
      bt_a[3] = bt_alarge;
      bt_alen = 4;
    }
  }
  if (cdxtail == 0.0) {
    if (cdytail == 0.0) {
      ct_a[0] = 0.0;
      ct_alen = 1;
      ct_b[0] = 0.0;
      ct_blen = 1;
    } else {
      negate = -cdytail;
      Two_Product(negate, adx, ct_alarge, ct_a[0]);
      ct_a[1] = ct_alarge;
      ct_alen = 2;
      Two_Product(cdytail, bdx, ct_blarge, ct_b[0]);
      ct_b[1] = ct_blarge;
      ct_blen = 2;
    }
  } else {
    if (cdytail == 0.0) {
      Two_Product(cdxtail, ady, ct_alarge, ct_a[0]);
      ct_a[1] = ct_alarge;
      ct_alen = 2;
      negate = -cdxtail;
      Two_Product(negate, bdy, ct_blarge, ct_b[0]);
      ct_b[1] = ct_blarge;
      ct_blen = 2;
    } else {
      Two_Product(cdxtail, ady, cdxt_ady1, cdxt_ady0);
      Two_Product(cdytail, adx, cdyt_adx1, cdyt_adx0);
      Two_Two_Diff(cdxt_ady1, cdxt_ady0, cdyt_adx1, cdyt_adx0,
                   ct_alarge, ct_a[2], ct_a[1], ct_a[0]);
      ct_a[3] = ct_alarge;
      ct_alen = 4;
      Two_Product(cdytail, bdx, cdyt_bdx1, cdyt_bdx0);
      Two_Product(cdxtail, bdy, cdxt_bdy1, cdxt_bdy0);
      Two_Two_Diff(cdyt_bdx1, cdyt_bdx0, cdxt_bdy1, cdxt_bdy0,
                   ct_blarge, ct_b[2], ct_b[1], ct_b[0]);
      ct_b[3] = ct_blarge;
      ct_blen = 4;
    }
  }

  bctlen = fast_expansion_sum_zeroelim(bt_clen, bt_c, ct_blen, ct_b, bct);
  wlength = scale_expansion_zeroelim(bctlen, bct, adz, w);
  finlength = fast_expansion_sum_zeroelim(finlength, finnow, wlength, w,
                                          finother);
  finswap = finnow; finnow = finother; finother = finswap;

  catlen = fast_expansion_sum_zeroelim(ct_alen, ct_a, at_clen, at_c, cat);
  wlength = scale_expansion_zeroelim(catlen, cat, bdz, w);
  finlength = fast_expansion_sum_zeroelim(finlength, finnow, wlength, w,
                                          finother);
  finswap = finnow; finnow = finother; finother = finswap;

  abtlen = fast_expansion_sum_zeroelim(at_blen, at_b, bt_alen, bt_a, abt);
  wlength = scale_expansion_zeroelim(abtlen, abt, cdz, w);
  finlength = fast_expansion_sum_zeroelim(finlength, finnow, wlength, w,
                                          finother);
  finswap = finnow; finnow = finother; finother = finswap;

  if (adztail != 0.0) {
    vlength = scale_expansion_zeroelim(4, bc, adztail, v);
    finlength = fast_expansion_sum_zeroelim(finlength, finnow, vlength, v,
                                            finother);
    finswap = finnow; finnow = finother; finother = finswap;
  }
  if (bdztail != 0.0) {
    vlength = scale_expansion_zeroelim(4, ca, bdztail, v);
    finlength = fast_expansion_sum_zeroelim(finlength, finnow, vlength, v,
                                            finother);
    finswap = finnow; finnow = finother; finother = finswap;
  }
  if (cdztail != 0.0) {
    vlength = scale_expansion_zeroelim(4, ab, cdztail, v);
    finlength = fast_expansion_sum_zeroelim(finlength, finnow, vlength, v,
                                            finother);
    finswap = finnow; finnow = finother; finother = finswap;
  }

  if (adxtail != 0.0) {
    if (bdytail != 0.0) {
      Two_Product(adxtail, bdytail, adxt_bdyt1, adxt_bdyt0);
      Two_One_Product(adxt_bdyt1, adxt_bdyt0, cdz, u3, u[2], u[1], u[0]);
      u[3] = u3;
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                              finother);
      finswap = finnow; finnow = finother; finother = finswap;
      if (cdztail != 0.0) {
        Two_One_Product(adxt_bdyt1, adxt_bdyt0, cdztail, u3, u[2], u[1], u[0]);
        u[3] = u3;
        finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                                finother);
        finswap = finnow; finnow = finother; finother = finswap;
      }
    }
    if (cdytail != 0.0) {
      negate = -adxtail;
      Two_Product(negate, cdytail, adxt_cdyt1, adxt_cdyt0);
      Two_One_Product(adxt_cdyt1, adxt_cdyt0, bdz, u3, u[2], u[1], u[0]);
      u[3] = u3;
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                              finother);
      finswap = finnow; finnow = finother; finother = finswap;
      if (bdztail != 0.0) {
        Two_One_Product(adxt_cdyt1, adxt_cdyt0, bdztail, u3, u[2], u[1], u[0]);
        u[3] = u3;
        finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                                finother);
        finswap = finnow; finnow = finother; finother = finswap;
      }
    }
  }
  if (bdxtail != 0.0) {
    if (cdytail != 0.0) {
      Two_Product(bdxtail, cdytail, bdxt_cdyt1, bdxt_cdyt0);
      Two_One_Product(bdxt_cdyt1, bdxt_cdyt0, adz, u3, u[2], u[1], u[0]);
      u[3] = u3;
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                              finother);
      finswap = finnow; finnow = finother; finother = finswap;
      if (adztail != 0.0) {
        Two_One_Product(bdxt_cdyt1, bdxt_cdyt0, adztail, u3, u[2], u[1], u[0]);
        u[3] = u3;
        finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                                finother);
        finswap = finnow; finnow = finother; finother = finswap;
      }
    }
    if (adytail != 0.0) {
      negate = -bdxtail;
      Two_Product(negate, adytail, bdxt_adyt1, bdxt_adyt0);
      Two_One_Product(bdxt_adyt1, bdxt_adyt0, cdz, u3, u[2], u[1], u[0]);
      u[3] = u3;
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                              finother);
      finswap = finnow; finnow = finother; finother = finswap;
      if (cdztail != 0.0) {
        Two_One_Product(bdxt_adyt1, bdxt_adyt0, cdztail, u3, u[2], u[1], u[0]);
        u[3] = u3;
        finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                                finother);
        finswap = finnow; finnow = finother; finother = finswap;
      }
    }
  }
  if (cdxtail != 0.0) {
    if (adytail != 0.0) {
      Two_Product(cdxtail, adytail, cdxt_adyt1, cdxt_adyt0);
      Two_One_Product(cdxt_adyt1, cdxt_adyt0, bdz, u3, u[2], u[1], u[0]);
      u[3] = u3;
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                              finother);
      finswap = finnow; finnow = finother; finother = finswap;
      if (bdztail != 0.0) {
        Two_One_Product(cdxt_adyt1, cdxt_adyt0, bdztail, u3, u[2], u[1], u[0]);
        u[3] = u3;
        finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                                finother);
        finswap = finnow; finnow = finother; finother = finswap;
      }
    }
    if (bdytail != 0.0) {
      negate = -cdxtail;
      Two_Product(negate, bdytail, cdxt_bdyt1, cdxt_bdyt0);
      Two_One_Product(cdxt_bdyt1, cdxt_bdyt0, adz, u3, u[2], u[1], u[0]);
      u[3] = u3;
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                              finother);
      finswap = finnow; finnow = finother; finother = finswap;
      if (adztail != 0.0) {
        Two_One_Product(cdxt_bdyt1, cdxt_bdyt0, adztail, u3, u[2], u[1], u[0]);
        u[3] = u3;
        finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                                finother);
        finswap = finnow; finnow = finother; finother = finswap;
      }
    }
  }

  if (adztail != 0.0) {
    wlength = scale_expansion_zeroelim(bctlen, bct, adztail, w);
    finlength = fast_expansion_sum_zeroelim(finlength, finnow, wlength, w,
                                            finother);
    finswap = finnow; finnow = finother; finother = finswap;
  }
  if (bdztail != 0.0) {
    wlength = scale_expansion_zeroelim(catlen, cat, bdztail, w);
    finlength = fast_expansion_sum_zeroelim(finlength, finnow, wlength, w,
                                            finother);
    finswap = finnow; finnow = finother; finother = finswap;
  }
  if (cdztail != 0.0) {
    wlength = scale_expansion_zeroelim(abtlen, abt, cdztail, w);
    finlength = fast_expansion_sum_zeroelim(finlength, finnow, wlength, w,
                                            finother);
    finswap = finnow; finnow = finother; finother = finswap;
  }

  return finnow[finlength - 1];
}

starreal orient3d(struct behavior *b,
                  starreal *pa,
                  starreal *pb,
                  starreal *pc,
                  starreal *pd)
{
  starreal adx, bdx, cdx, ady, bdy, cdy, adz, bdz, cdz;
  starreal bdxcdy, cdxbdy, cdxady, adxcdy, adxbdy, bdxady;
  starreal det;
  starreal permanent, errbound;

  b->orientcount++;

  adx = pa[0] - pd[0];
  bdx = pb[0] - pd[0];
  cdx = pc[0] - pd[0];
  ady = pa[1] - pd[1];
  bdy = pb[1] - pd[1];
  cdy = pc[1] - pd[1];
  adz = pa[2] - pd[2];
  bdz = pb[2] - pd[2];
  cdz = pc[2] - pd[2];

  bdxcdy = bdx * cdy;
  cdxbdy = cdx * bdy;

  cdxady = cdx * ady;
  adxcdy = adx * cdy;

  adxbdy = adx * bdy;
  bdxady = bdx * ady;

  det = adz * (bdxcdy - cdxbdy) 
      + bdz * (cdxady - adxcdy)
      + cdz * (adxbdy - bdxady);

  if (b->noexact) {
    return det;
  }

  permanent = (Absolute(bdxcdy) + Absolute(cdxbdy)) * Absolute(adz)
            + (Absolute(cdxady) + Absolute(adxcdy)) * Absolute(bdz)
            + (Absolute(adxbdy) + Absolute(bdxady)) * Absolute(cdz);
  errbound = o3derrboundA * permanent;
  if ((det > errbound) || (-det > errbound)) {
    return det;
  }

  return orient3dadapt(pa, pb, pc, pd, permanent);
}

/*****************************************************************************/
/*                                                                           */
/*  insphere()   Return a positive value if the point pe lies inside the     */
/*               sphere passing through pa, pb, pc, and pd; a negative value */
/*               if it lies outside; and zero if the five points are         */
/*               cospherical.  The points pa, pb, pc, and pd must be ordered */
/*               so that they have a positive orientation (as defined by     */
/*               orient3d()), or the sign of the result will be reversed.    */
/*                                                                           */
/*  Uses exact arithmetic if necessary to ensure a correct answer.  The      */
/*  result returned is the determinant of a matrix.  This determinant is     */
/*  computed adaptively, in the sense that exact arithmetic is used only to  */
/*  the degree it is needed to ensure that the returned value has the        */
/*  correct sign.  Hence, this function is usually quite fast, but will run  */
/*  more slowly when the input points are cospherical or nearly so.          */
/*                                                                           */
/*  See my Robust Predicates paper for details.                              */
/*                                                                           */
/*****************************************************************************/

starreal insphereexact(starreal *pa,
                       starreal *pb,
                       starreal *pc,
                       starreal *pd,
                       starreal *pe)
{
  INEXACT starreal axby1, bxcy1, cxdy1, dxey1, exay1;
  INEXACT starreal bxay1, cxby1, dxcy1, exdy1, axey1;
  INEXACT starreal axcy1, bxdy1, cxey1, dxay1, exby1;
  INEXACT starreal cxay1, dxby1, excy1, axdy1, bxey1;
  starreal axby0, bxcy0, cxdy0, dxey0, exay0;
  starreal bxay0, cxby0, dxcy0, exdy0, axey0;
  starreal axcy0, bxdy0, cxey0, dxay0, exby0;
  starreal cxay0, dxby0, excy0, axdy0, bxey0;
  starreal ab[4], bc[4], cd[4], de[4], ea[4];
  starreal ac[4], bd[4], ce[4], da[4], eb[4];
  starreal temp8a[8], temp8b[8], temp16[16];
  int temp8alen, temp8blen, temp16len;
  starreal abc[24], bcd[24], cde[24], dea[24], eab[24];
  starreal abd[24], bce[24], cda[24], deb[24], eac[24];
  int abclen, bcdlen, cdelen, dealen, eablen;
  int abdlen, bcelen, cdalen, deblen, eaclen;
  starreal temp48a[48], temp48b[48];
  int temp48alen, temp48blen;
  starreal abcd[96], bcde[96], cdea[96], deab[96], eabc[96];
  int abcdlen, bcdelen, cdealen, deablen, eabclen;
  starreal temp192[192];
  starreal det384x[384], det384y[384], det384z[384];
  int xlen, ylen, zlen;
  starreal detxy[768];
  int xylen;
  starreal adet[1152], bdet[1152], cdet[1152], ddet[1152], edet[1152];
  int alen, blen, clen, dlen, elen;
  starreal abdet[2304], cddet[2304], cdedet[3456];
  int ablen, cdlen;
  starreal deter[5760];
  int deterlen;
  int i;

  INEXACT starreal bvirt;
  starreal avirt, bround, around;
  INEXACT starreal c;
  INEXACT starreal abig;
  starreal ahi, alo, bhi, blo;
  starreal err1, err2, err3;
  INEXACT starreal _i, _j;
  starreal _0;

  Two_Product(pa[0], pb[1], axby1, axby0);
  Two_Product(pb[0], pa[1], bxay1, bxay0);
  Two_Two_Diff(axby1, axby0, bxay1, bxay0, ab[3], ab[2], ab[1], ab[0]);

  Two_Product(pb[0], pc[1], bxcy1, bxcy0);
  Two_Product(pc[0], pb[1], cxby1, cxby0);
  Two_Two_Diff(bxcy1, bxcy0, cxby1, cxby0, bc[3], bc[2], bc[1], bc[0]);

  Two_Product(pc[0], pd[1], cxdy1, cxdy0);
  Two_Product(pd[0], pc[1], dxcy1, dxcy0);
  Two_Two_Diff(cxdy1, cxdy0, dxcy1, dxcy0, cd[3], cd[2], cd[1], cd[0]);

  Two_Product(pd[0], pe[1], dxey1, dxey0);
  Two_Product(pe[0], pd[1], exdy1, exdy0);
  Two_Two_Diff(dxey1, dxey0, exdy1, exdy0, de[3], de[2], de[1], de[0]);

  Two_Product(pe[0], pa[1], exay1, exay0);
  Two_Product(pa[0], pe[1], axey1, axey0);
  Two_Two_Diff(exay1, exay0, axey1, axey0, ea[3], ea[2], ea[1], ea[0]);

  Two_Product(pa[0], pc[1], axcy1, axcy0);
  Two_Product(pc[0], pa[1], cxay1, cxay0);
  Two_Two_Diff(axcy1, axcy0, cxay1, cxay0, ac[3], ac[2], ac[1], ac[0]);

  Two_Product(pb[0], pd[1], bxdy1, bxdy0);
  Two_Product(pd[0], pb[1], dxby1, dxby0);
  Two_Two_Diff(bxdy1, bxdy0, dxby1, dxby0, bd[3], bd[2], bd[1], bd[0]);

  Two_Product(pc[0], pe[1], cxey1, cxey0);
  Two_Product(pe[0], pc[1], excy1, excy0);
  Two_Two_Diff(cxey1, cxey0, excy1, excy0, ce[3], ce[2], ce[1], ce[0]);

  Two_Product(pd[0], pa[1], dxay1, dxay0);
  Two_Product(pa[0], pd[1], axdy1, axdy0);
  Two_Two_Diff(dxay1, dxay0, axdy1, axdy0, da[3], da[2], da[1], da[0]);

  Two_Product(pe[0], pb[1], exby1, exby0);
  Two_Product(pb[0], pe[1], bxey1, bxey0);
  Two_Two_Diff(exby1, exby0, bxey1, bxey0, eb[3], eb[2], eb[1], eb[0]);

  temp8alen = scale_expansion_zeroelim(4, bc, pa[2], temp8a);
  temp8blen = scale_expansion_zeroelim(4, ac, -pb[2], temp8b);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp8blen, temp8b,
                                          temp16);
  temp8alen = scale_expansion_zeroelim(4, ab, pc[2], temp8a);
  abclen = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp16len, temp16,
                                       abc);

  temp8alen = scale_expansion_zeroelim(4, cd, pb[2], temp8a);
  temp8blen = scale_expansion_zeroelim(4, bd, -pc[2], temp8b);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp8blen, temp8b,
                                          temp16);
  temp8alen = scale_expansion_zeroelim(4, bc, pd[2], temp8a);
  bcdlen = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp16len, temp16,
                                       bcd);

  temp8alen = scale_expansion_zeroelim(4, de, pc[2], temp8a);
  temp8blen = scale_expansion_zeroelim(4, ce, -pd[2], temp8b);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp8blen, temp8b,
                                          temp16);
  temp8alen = scale_expansion_zeroelim(4, cd, pe[2], temp8a);
  cdelen = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp16len, temp16,
                                       cde);

  temp8alen = scale_expansion_zeroelim(4, ea, pd[2], temp8a);
  temp8blen = scale_expansion_zeroelim(4, da, -pe[2], temp8b);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp8blen, temp8b,
                                          temp16);
  temp8alen = scale_expansion_zeroelim(4, de, pa[2], temp8a);
  dealen = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp16len, temp16,
                                       dea);

  temp8alen = scale_expansion_zeroelim(4, ab, pe[2], temp8a);
  temp8blen = scale_expansion_zeroelim(4, eb, -pa[2], temp8b);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp8blen, temp8b,
                                          temp16);
  temp8alen = scale_expansion_zeroelim(4, ea, pb[2], temp8a);
  eablen = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp16len, temp16,
                                       eab);

  temp8alen = scale_expansion_zeroelim(4, bd, pa[2], temp8a);
  temp8blen = scale_expansion_zeroelim(4, da, pb[2], temp8b);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp8blen, temp8b,
                                          temp16);
  temp8alen = scale_expansion_zeroelim(4, ab, pd[2], temp8a);
  abdlen = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp16len, temp16,
                                       abd);

  temp8alen = scale_expansion_zeroelim(4, ce, pb[2], temp8a);
  temp8blen = scale_expansion_zeroelim(4, eb, pc[2], temp8b);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp8blen, temp8b,
                                          temp16);
  temp8alen = scale_expansion_zeroelim(4, bc, pe[2], temp8a);
  bcelen = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp16len, temp16,
                                       bce);

  temp8alen = scale_expansion_zeroelim(4, da, pc[2], temp8a);
  temp8blen = scale_expansion_zeroelim(4, ac, pd[2], temp8b);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp8blen, temp8b,
                                          temp16);
  temp8alen = scale_expansion_zeroelim(4, cd, pa[2], temp8a);
  cdalen = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp16len, temp16,
                                       cda);

  temp8alen = scale_expansion_zeroelim(4, eb, pd[2], temp8a);
  temp8blen = scale_expansion_zeroelim(4, bd, pe[2], temp8b);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp8blen, temp8b,
                                          temp16);
  temp8alen = scale_expansion_zeroelim(4, de, pb[2], temp8a);
  deblen = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp16len, temp16,
                                       deb);

  temp8alen = scale_expansion_zeroelim(4, ac, pe[2], temp8a);
  temp8blen = scale_expansion_zeroelim(4, ce, pa[2], temp8b);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp8blen, temp8b,
                                          temp16);
  temp8alen = scale_expansion_zeroelim(4, ea, pc[2], temp8a);
  eaclen = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp16len, temp16,
                                       eac);

  temp48alen = fast_expansion_sum_zeroelim(cdelen, cde, bcelen, bce, temp48a);
  temp48blen = fast_expansion_sum_zeroelim(deblen, deb, bcdlen, bcd, temp48b);
  for (i = 0; i < temp48blen; i++) {
    temp48b[i] = -temp48b[i];
  }
  bcdelen = fast_expansion_sum_zeroelim(temp48alen, temp48a,
                                        temp48blen, temp48b, bcde);
  xlen = scale_expansion_zeroelim(bcdelen, bcde, pa[0], temp192);
  xlen = scale_expansion_zeroelim(xlen, temp192, pa[0], det384x);
  ylen = scale_expansion_zeroelim(bcdelen, bcde, pa[1], temp192);
  ylen = scale_expansion_zeroelim(ylen, temp192, pa[1], det384y);
  zlen = scale_expansion_zeroelim(bcdelen, bcde, pa[2], temp192);
  zlen = scale_expansion_zeroelim(zlen, temp192, pa[2], det384z);
  xylen = fast_expansion_sum_zeroelim(xlen, det384x, ylen, det384y, detxy);
  alen = fast_expansion_sum_zeroelim(xylen, detxy, zlen, det384z, adet);

  temp48alen = fast_expansion_sum_zeroelim(dealen, dea, cdalen, cda, temp48a);
  temp48blen = fast_expansion_sum_zeroelim(eaclen, eac, cdelen, cde, temp48b);
  for (i = 0; i < temp48blen; i++) {
    temp48b[i] = -temp48b[i];
  }
  cdealen = fast_expansion_sum_zeroelim(temp48alen, temp48a,
                                        temp48blen, temp48b, cdea);
  xlen = scale_expansion_zeroelim(cdealen, cdea, pb[0], temp192);
  xlen = scale_expansion_zeroelim(xlen, temp192, pb[0], det384x);
  ylen = scale_expansion_zeroelim(cdealen, cdea, pb[1], temp192);
  ylen = scale_expansion_zeroelim(ylen, temp192, pb[1], det384y);
  zlen = scale_expansion_zeroelim(cdealen, cdea, pb[2], temp192);
  zlen = scale_expansion_zeroelim(zlen, temp192, pb[2], det384z);
  xylen = fast_expansion_sum_zeroelim(xlen, det384x, ylen, det384y, detxy);
  blen = fast_expansion_sum_zeroelim(xylen, detxy, zlen, det384z, bdet);

  temp48alen = fast_expansion_sum_zeroelim(eablen, eab, deblen, deb, temp48a);
  temp48blen = fast_expansion_sum_zeroelim(abdlen, abd, dealen, dea, temp48b);
  for (i = 0; i < temp48blen; i++) {
    temp48b[i] = -temp48b[i];
  }
  deablen = fast_expansion_sum_zeroelim(temp48alen, temp48a,
                                        temp48blen, temp48b, deab);
  xlen = scale_expansion_zeroelim(deablen, deab, pc[0], temp192);
  xlen = scale_expansion_zeroelim(xlen, temp192, pc[0], det384x);
  ylen = scale_expansion_zeroelim(deablen, deab, pc[1], temp192);
  ylen = scale_expansion_zeroelim(ylen, temp192, pc[1], det384y);
  zlen = scale_expansion_zeroelim(deablen, deab, pc[2], temp192);
  zlen = scale_expansion_zeroelim(zlen, temp192, pc[2], det384z);
  xylen = fast_expansion_sum_zeroelim(xlen, det384x, ylen, det384y, detxy);
  clen = fast_expansion_sum_zeroelim(xylen, detxy, zlen, det384z, cdet);

  temp48alen = fast_expansion_sum_zeroelim(abclen, abc, eaclen, eac, temp48a);
  temp48blen = fast_expansion_sum_zeroelim(bcelen, bce, eablen, eab, temp48b);
  for (i = 0; i < temp48blen; i++) {
    temp48b[i] = -temp48b[i];
  }
  eabclen = fast_expansion_sum_zeroelim(temp48alen, temp48a,
                                        temp48blen, temp48b, eabc);
  xlen = scale_expansion_zeroelim(eabclen, eabc, pd[0], temp192);
  xlen = scale_expansion_zeroelim(xlen, temp192, pd[0], det384x);
  ylen = scale_expansion_zeroelim(eabclen, eabc, pd[1], temp192);
  ylen = scale_expansion_zeroelim(ylen, temp192, pd[1], det384y);
  zlen = scale_expansion_zeroelim(eabclen, eabc, pd[2], temp192);
  zlen = scale_expansion_zeroelim(zlen, temp192, pd[2], det384z);
  xylen = fast_expansion_sum_zeroelim(xlen, det384x, ylen, det384y, detxy);
  dlen = fast_expansion_sum_zeroelim(xylen, detxy, zlen, det384z, ddet);

  temp48alen = fast_expansion_sum_zeroelim(bcdlen, bcd, abdlen, abd, temp48a);
  temp48blen = fast_expansion_sum_zeroelim(cdalen, cda, abclen, abc, temp48b);
  for (i = 0; i < temp48blen; i++) {
    temp48b[i] = -temp48b[i];
  }
  abcdlen = fast_expansion_sum_zeroelim(temp48alen, temp48a,
                                        temp48blen, temp48b, abcd);
  xlen = scale_expansion_zeroelim(abcdlen, abcd, pe[0], temp192);
  xlen = scale_expansion_zeroelim(xlen, temp192, pe[0], det384x);
  ylen = scale_expansion_zeroelim(abcdlen, abcd, pe[1], temp192);
  ylen = scale_expansion_zeroelim(ylen, temp192, pe[1], det384y);
  zlen = scale_expansion_zeroelim(abcdlen, abcd, pe[2], temp192);
  zlen = scale_expansion_zeroelim(zlen, temp192, pe[2], det384z);
  xylen = fast_expansion_sum_zeroelim(xlen, det384x, ylen, det384y, detxy);
  elen = fast_expansion_sum_zeroelim(xylen, detxy, zlen, det384z, edet);

  ablen = fast_expansion_sum_zeroelim(alen, adet, blen, bdet, abdet);
  cdlen = fast_expansion_sum_zeroelim(clen, cdet, dlen, ddet, cddet);
  cdelen = fast_expansion_sum_zeroelim(cdlen, cddet, elen, edet, cdedet);
  deterlen = fast_expansion_sum_zeroelim(ablen, abdet, cdelen, cdedet, deter);

  return deter[deterlen - 1];
}

starreal insphereadapt(starreal *pa,
                       starreal *pb,
                       starreal *pc,
                       starreal *pd,
                       starreal *pe,
                       starreal permanent)
{
  INEXACT starreal aex, bex, cex, dex, aey, bey, cey, dey, aez, bez, cez, dez;
  starreal det, errbound;

  INEXACT starreal aexbey1, bexaey1, bexcey1, cexbey1;
  INEXACT starreal cexdey1, dexcey1, dexaey1, aexdey1;
  INEXACT starreal aexcey1, cexaey1, bexdey1, dexbey1;
  starreal aexbey0, bexaey0, bexcey0, cexbey0;
  starreal cexdey0, dexcey0, dexaey0, aexdey0;
  starreal aexcey0, cexaey0, bexdey0, dexbey0;
  starreal ab[4], bc[4], cd[4], da[4], ac[4], bd[4];
  INEXACT starreal ab3, bc3, cd3, da3, ac3, bd3;
  starreal abeps, bceps, cdeps, daeps, aceps, bdeps;
  starreal temp8a[8], temp8b[8], temp8c[8], temp16[16], temp24[24], temp48[48];
  int temp8alen, temp8blen, temp8clen, temp16len, temp24len, temp48len;
  starreal xdet[96], ydet[96], zdet[96], xydet[192];
  int xlen, ylen, zlen, xylen;
  starreal adet[288], bdet[288], cdet[288], ddet[288];
  int alen, blen, clen, dlen;
  starreal abdet[576], cddet[576];
  int ablen, cdlen;
  starreal fin1[1152];
  int finlength;

  starreal aextail, bextail, cextail, dextail;
  starreal aeytail, beytail, ceytail, deytail;
  starreal aeztail, beztail, ceztail, deztail;

  INEXACT starreal bvirt;
  starreal avirt, bround, around;
  INEXACT starreal c;
  INEXACT starreal abig;
  starreal ahi, alo, bhi, blo;
  starreal err1, err2, err3;
  INEXACT starreal _i, _j;
  starreal _0;

  aex = (starreal) (pa[0] - pe[0]);
  bex = (starreal) (pb[0] - pe[0]);
  cex = (starreal) (pc[0] - pe[0]);
  dex = (starreal) (pd[0] - pe[0]);
  aey = (starreal) (pa[1] - pe[1]);
  bey = (starreal) (pb[1] - pe[1]);
  cey = (starreal) (pc[1] - pe[1]);
  dey = (starreal) (pd[1] - pe[1]);
  aez = (starreal) (pa[2] - pe[2]);
  bez = (starreal) (pb[2] - pe[2]);
  cez = (starreal) (pc[2] - pe[2]);
  dez = (starreal) (pd[2] - pe[2]);

  Two_Product(aex, bey, aexbey1, aexbey0);
  Two_Product(bex, aey, bexaey1, bexaey0);
  Two_Two_Diff(aexbey1, aexbey0, bexaey1, bexaey0, ab3, ab[2], ab[1], ab[0]);
  ab[3] = ab3;

  Two_Product(bex, cey, bexcey1, bexcey0);
  Two_Product(cex, bey, cexbey1, cexbey0);
  Two_Two_Diff(bexcey1, bexcey0, cexbey1, cexbey0, bc3, bc[2], bc[1], bc[0]);
  bc[3] = bc3;

  Two_Product(cex, dey, cexdey1, cexdey0);
  Two_Product(dex, cey, dexcey1, dexcey0);
  Two_Two_Diff(cexdey1, cexdey0, dexcey1, dexcey0, cd3, cd[2], cd[1], cd[0]);
  cd[3] = cd3;

  Two_Product(dex, aey, dexaey1, dexaey0);
  Two_Product(aex, dey, aexdey1, aexdey0);
  Two_Two_Diff(dexaey1, dexaey0, aexdey1, aexdey0, da3, da[2], da[1], da[0]);
  da[3] = da3;

  Two_Product(aex, cey, aexcey1, aexcey0);
  Two_Product(cex, aey, cexaey1, cexaey0);
  Two_Two_Diff(aexcey1, aexcey0, cexaey1, cexaey0, ac3, ac[2], ac[1], ac[0]);
  ac[3] = ac3;

  Two_Product(bex, dey, bexdey1, bexdey0);
  Two_Product(dex, bey, dexbey1, dexbey0);
  Two_Two_Diff(bexdey1, bexdey0, dexbey1, dexbey0, bd3, bd[2], bd[1], bd[0]);
  bd[3] = bd3;

  temp8alen = scale_expansion_zeroelim(4, cd, bez, temp8a);
  temp8blen = scale_expansion_zeroelim(4, bd, -cez, temp8b);
  temp8clen = scale_expansion_zeroelim(4, bc, dez, temp8c);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a,
                                          temp8blen, temp8b, temp16);
  temp24len = fast_expansion_sum_zeroelim(temp8clen, temp8c,
                                          temp16len, temp16, temp24);
  temp48len = scale_expansion_zeroelim(temp24len, temp24, aex, temp48);
  xlen = scale_expansion_zeroelim(temp48len, temp48, -aex, xdet);
  temp48len = scale_expansion_zeroelim(temp24len, temp24, aey, temp48);
  ylen = scale_expansion_zeroelim(temp48len, temp48, -aey, ydet);
  temp48len = scale_expansion_zeroelim(temp24len, temp24, aez, temp48);
  zlen = scale_expansion_zeroelim(temp48len, temp48, -aez, zdet);
  xylen = fast_expansion_sum_zeroelim(xlen, xdet, ylen, ydet, xydet);
  alen = fast_expansion_sum_zeroelim(xylen, xydet, zlen, zdet, adet);

  temp8alen = scale_expansion_zeroelim(4, da, cez, temp8a);
  temp8blen = scale_expansion_zeroelim(4, ac, dez, temp8b);
  temp8clen = scale_expansion_zeroelim(4, cd, aez, temp8c);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a,
                                          temp8blen, temp8b, temp16);
  temp24len = fast_expansion_sum_zeroelim(temp8clen, temp8c,
                                          temp16len, temp16, temp24);
  temp48len = scale_expansion_zeroelim(temp24len, temp24, bex, temp48);
  xlen = scale_expansion_zeroelim(temp48len, temp48, bex, xdet);
  temp48len = scale_expansion_zeroelim(temp24len, temp24, bey, temp48);
  ylen = scale_expansion_zeroelim(temp48len, temp48, bey, ydet);
  temp48len = scale_expansion_zeroelim(temp24len, temp24, bez, temp48);
  zlen = scale_expansion_zeroelim(temp48len, temp48, bez, zdet);
  xylen = fast_expansion_sum_zeroelim(xlen, xdet, ylen, ydet, xydet);
  blen = fast_expansion_sum_zeroelim(xylen, xydet, zlen, zdet, bdet);

  temp8alen = scale_expansion_zeroelim(4, ab, dez, temp8a);
  temp8blen = scale_expansion_zeroelim(4, bd, aez, temp8b);
  temp8clen = scale_expansion_zeroelim(4, da, bez, temp8c);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a,
                                          temp8blen, temp8b, temp16);
  temp24len = fast_expansion_sum_zeroelim(temp8clen, temp8c,
                                          temp16len, temp16, temp24);
  temp48len = scale_expansion_zeroelim(temp24len, temp24, cex, temp48);
  xlen = scale_expansion_zeroelim(temp48len, temp48, -cex, xdet);
  temp48len = scale_expansion_zeroelim(temp24len, temp24, cey, temp48);
  ylen = scale_expansion_zeroelim(temp48len, temp48, -cey, ydet);
  temp48len = scale_expansion_zeroelim(temp24len, temp24, cez, temp48);
  zlen = scale_expansion_zeroelim(temp48len, temp48, -cez, zdet);
  xylen = fast_expansion_sum_zeroelim(xlen, xdet, ylen, ydet, xydet);
  clen = fast_expansion_sum_zeroelim(xylen, xydet, zlen, zdet, cdet);

  temp8alen = scale_expansion_zeroelim(4, bc, aez, temp8a);
  temp8blen = scale_expansion_zeroelim(4, ac, -bez, temp8b);
  temp8clen = scale_expansion_zeroelim(4, ab, cez, temp8c);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a,
                                          temp8blen, temp8b, temp16);
  temp24len = fast_expansion_sum_zeroelim(temp8clen, temp8c,
                                          temp16len, temp16, temp24);
  temp48len = scale_expansion_zeroelim(temp24len, temp24, dex, temp48);
  xlen = scale_expansion_zeroelim(temp48len, temp48, dex, xdet);
  temp48len = scale_expansion_zeroelim(temp24len, temp24, dey, temp48);
  ylen = scale_expansion_zeroelim(temp48len, temp48, dey, ydet);
  temp48len = scale_expansion_zeroelim(temp24len, temp24, dez, temp48);
  zlen = scale_expansion_zeroelim(temp48len, temp48, dez, zdet);
  xylen = fast_expansion_sum_zeroelim(xlen, xdet, ylen, ydet, xydet);
  dlen = fast_expansion_sum_zeroelim(xylen, xydet, zlen, zdet, ddet);

  ablen = fast_expansion_sum_zeroelim(alen, adet, blen, bdet, abdet);
  cdlen = fast_expansion_sum_zeroelim(clen, cdet, dlen, ddet, cddet);
  finlength = fast_expansion_sum_zeroelim(ablen, abdet, cdlen, cddet, fin1);

  det = estimate(finlength, fin1);
  errbound = isperrboundB * permanent;
  if ((det >= errbound) || (-det >= errbound)) {
    return det;
  }

  Two_Diff_Tail(pa[0], pe[0], aex, aextail);
  Two_Diff_Tail(pa[1], pe[1], aey, aeytail);
  Two_Diff_Tail(pa[2], pe[2], aez, aeztail);
  Two_Diff_Tail(pb[0], pe[0], bex, bextail);
  Two_Diff_Tail(pb[1], pe[1], bey, beytail);
  Two_Diff_Tail(pb[2], pe[2], bez, beztail);
  Two_Diff_Tail(pc[0], pe[0], cex, cextail);
  Two_Diff_Tail(pc[1], pe[1], cey, ceytail);
  Two_Diff_Tail(pc[2], pe[2], cez, ceztail);
  Two_Diff_Tail(pd[0], pe[0], dex, dextail);
  Two_Diff_Tail(pd[1], pe[1], dey, deytail);
  Two_Diff_Tail(pd[2], pe[2], dez, deztail);
  if ((aextail == 0.0) && (aeytail == 0.0) && (aeztail == 0.0) &&
      (bextail == 0.0) && (beytail == 0.0) && (beztail == 0.0) &&
      (cextail == 0.0) && (ceytail == 0.0) && (ceztail == 0.0) &&
      (dextail == 0.0) && (deytail == 0.0) && (deztail == 0.0)) {
    return det;
  }

  errbound = isperrboundC * permanent + resulterrbound * Absolute(det);
  abeps = (aex * beytail + bey * aextail)
        - (aey * bextail + bex * aeytail);
  bceps = (bex * ceytail + cey * bextail)
        - (bey * cextail + cex * beytail);
  cdeps = (cex * deytail + dey * cextail)
        - (cey * dextail + dex * ceytail);
  daeps = (dex * aeytail + aey * dextail)
        - (dey * aextail + aex * deytail);
  aceps = (aex * ceytail + cey * aextail)
        - (aey * cextail + cex * aeytail);
  bdeps = (bex * deytail + dey * bextail)
        - (bey * dextail + dex * beytail);
  det += (((bex * bex + bey * bey + bez * bez)
           * ((cez * daeps + dez * aceps + aez * cdeps)
              + (ceztail * da3 + deztail * ac3 + aeztail * cd3))
           + (dex * dex + dey * dey + dez * dez)
           * ((aez * bceps - bez * aceps + cez * abeps)
              + (aeztail * bc3 - beztail * ac3 + ceztail * ab3)))
          - ((aex * aex + aey * aey + aez * aez)
           * ((bez * cdeps - cez * bdeps + dez * bceps)
              + (beztail * cd3 - ceztail * bd3 + deztail * bc3))
           + (cex * cex + cey * cey + cez * cez)
           * ((dez * abeps + aez * bdeps + bez * daeps)
              + (deztail * ab3 + aeztail * bd3 + beztail * da3))))
       + 2.0 * (((bex * bextail + bey * beytail + bez * beztail)
                 * (cez * da3 + dez * ac3 + aez * cd3)
                 + (dex * dextail + dey * deytail + dez * deztail)
                 * (aez * bc3 - bez * ac3 + cez * ab3))
                - ((aex * aextail + aey * aeytail + aez * aeztail)
                 * (bez * cd3 - cez * bd3 + dez * bc3)
                 + (cex * cextail + cey * ceytail + cez * ceztail)
                 * (dez * ab3 + aez * bd3 + bez * da3)));
  if ((det >= errbound) || (-det >= errbound)) {
    return det;
  }

  return insphereexact(pa, pb, pc, pd, pe);
}

starreal insphere(struct behavior *b,
                  starreal *pa,
                  starreal *pb,
                  starreal *pc,
                  starreal *pd,
                  starreal *pe)
{
  starreal aex, bex, cex, dex;
  starreal aey, bey, cey, dey;
  starreal aez, bez, cez, dez;
  starreal aexbey, bexaey, bexcey, cexbey, cexdey, dexcey, dexaey, aexdey;
  starreal aexcey, cexaey, bexdey, dexbey;
  starreal alift, blift, clift, dlift;
  starreal ab, bc, cd, da, ac, bd;
  starreal abc, bcd, cda, dab;
  starreal aezplus, bezplus, cezplus, dezplus;
  starreal aexbeyplus, bexaeyplus, bexceyplus, cexbeyplus;
  starreal cexdeyplus, dexceyplus, dexaeyplus, aexdeyplus;
  starreal aexceyplus, cexaeyplus, bexdeyplus, dexbeyplus;
  starreal det;
  starreal permanent, errbound;

  b->inspherecount++;

  aex = pa[0] - pe[0];
  bex = pb[0] - pe[0];
  cex = pc[0] - pe[0];
  dex = pd[0] - pe[0];
  aey = pa[1] - pe[1];
  bey = pb[1] - pe[1];
  cey = pc[1] - pe[1];
  dey = pd[1] - pe[1];
  aez = pa[2] - pe[2];
  bez = pb[2] - pe[2];
  cez = pc[2] - pe[2];
  dez = pd[2] - pe[2];

  aexbey = aex * bey;
  bexaey = bex * aey;
  ab = aexbey - bexaey;
  bexcey = bex * cey;
  cexbey = cex * bey;
  bc = bexcey - cexbey;
  cexdey = cex * dey;
  dexcey = dex * cey;
  cd = cexdey - dexcey;
  dexaey = dex * aey;
  aexdey = aex * dey;
  da = dexaey - aexdey;

  aexcey = aex * cey;
  cexaey = cex * aey;
  ac = aexcey - cexaey;
  bexdey = bex * dey;
  dexbey = dex * bey;
  bd = bexdey - dexbey;

  abc = aez * bc - bez * ac + cez * ab;
  bcd = bez * cd - cez * bd + dez * bc;
  cda = cez * da + dez * ac + aez * cd;
  dab = dez * ab + aez * bd + bez * da;

  alift = aex * aex + aey * aey + aez * aez;
  blift = bex * bex + bey * bey + bez * bez;
  clift = cex * cex + cey * cey + cez * cez;
  dlift = dex * dex + dey * dey + dez * dez;

  det = (dlift * abc - clift * dab) + (blift * cda - alift * bcd);

  if (b->noexact) {
    return det;
  }

  aezplus = Absolute(aez);
  bezplus = Absolute(bez);
  cezplus = Absolute(cez);
  dezplus = Absolute(dez);
  aexbeyplus = Absolute(aexbey);
  bexaeyplus = Absolute(bexaey);
  bexceyplus = Absolute(bexcey);
  cexbeyplus = Absolute(cexbey);
  cexdeyplus = Absolute(cexdey);
  dexceyplus = Absolute(dexcey);
  dexaeyplus = Absolute(dexaey);
  aexdeyplus = Absolute(aexdey);
  aexceyplus = Absolute(aexcey);
  cexaeyplus = Absolute(cexaey);
  bexdeyplus = Absolute(bexdey);
  dexbeyplus = Absolute(dexbey);
  permanent = ((cexdeyplus + dexceyplus) * bezplus
               + (dexbeyplus + bexdeyplus) * cezplus
               + (bexceyplus + cexbeyplus) * dezplus)
            * alift
            + ((dexaeyplus + aexdeyplus) * cezplus
               + (aexceyplus + cexaeyplus) * dezplus
               + (cexdeyplus + dexceyplus) * aezplus)
            * blift
            + ((aexbeyplus + bexaeyplus) * dezplus
               + (bexdeyplus + dexbeyplus) * aezplus
               + (dexaeyplus + aexdeyplus) * bezplus)
            * clift
            + ((bexceyplus + cexbeyplus) * aezplus
               + (cexaeyplus + aexceyplus) * bezplus
               + (aexbeyplus + bexaeyplus) * cezplus)
            * dlift;
  errbound = isperrboundA * permanent;
  if ((det > errbound) || (-det > errbound)) {
    return det;
  }

  return insphereadapt(pa, pb, pc, pd, pe, permanent);
}

/*****************************************************************************/
/*                                                                           */
/*  orient4d()   Return a positive value if the point pe lies above the      */
/*               hyperplane passing through pa, pb, pc, and pd; "above" is   */
/*               defined in a manner best found by trial-and-error.  Returns */
/*               a negative value if pe lies below the hyperplane.  Returns  */
/*               zero if the points are co-hyperplanar (not affinely         */
/*               independent).  The result is also a rough approximation of  */
/*               24 times the signed volume of the 4-simplex defined by the  */
/*               five points.                                                */
/*                                                                           */
/*  Uses exact arithmetic if necessary to ensure a correct answer.  The      */
/*  result returned is the determinant of a matrix.  This determinant is     */
/*  computed adaptively, in the sense that exact arithmetic is used only to  */
/*  the degree it is needed to ensure that the returned value has the        */
/*  correct sign.  Hence, orient4d() is usually quite fast, but will run     */
/*  more slowly when the input points are hyper-coplanar or nearly so.       */
/*                                                                           */
/*  See my Robust Predicates paper for details.                              */
/*                                                                           */
/*****************************************************************************/

starreal orient4dexact(starreal *pa,
                       starreal *pb,
                       starreal *pc,
                       starreal *pd,
                       starreal *pe,
                       starreal aheight,
                       starreal bheight,
                       starreal cheight,
                       starreal dheight,
                       starreal eheight)
{
  INEXACT starreal axby1, bxcy1, cxdy1, dxey1, exay1;
  INEXACT starreal bxay1, cxby1, dxcy1, exdy1, axey1;
  INEXACT starreal axcy1, bxdy1, cxey1, dxay1, exby1;
  INEXACT starreal cxay1, dxby1, excy1, axdy1, bxey1;
  starreal axby0, bxcy0, cxdy0, dxey0, exay0;
  starreal bxay0, cxby0, dxcy0, exdy0, axey0;
  starreal axcy0, bxdy0, cxey0, dxay0, exby0;
  starreal cxay0, dxby0, excy0, axdy0, bxey0;
  starreal ab[4], bc[4], cd[4], de[4], ea[4];
  starreal ac[4], bd[4], ce[4], da[4], eb[4];
  starreal temp8a[8], temp8b[8], temp16[16];
  int temp8alen, temp8blen, temp16len;
  starreal abc[24], bcd[24], cde[24], dea[24], eab[24];
  starreal abd[24], bce[24], cda[24], deb[24], eac[24];
  int abclen, bcdlen, cdelen, dealen, eablen;
  int abdlen, bcelen, cdalen, deblen, eaclen;
  starreal temp48a[48], temp48b[48];
  int temp48alen, temp48blen;
  starreal abcd[96], bcde[96], cdea[96], deab[96], eabc[96];
  int abcdlen, bcdelen, cdealen, deablen, eabclen;
  starreal adet[192], bdet[192], cdet[192], ddet[192], edet[192];
  int alen, blen, clen, dlen, elen;
  starreal abdet[384], cddet[384], cdedet[576];
  int ablen, cdlen;
  starreal deter[960];
  int deterlen;
  int i;

  INEXACT starreal bvirt;
  starreal avirt, bround, around;
  INEXACT starreal c;
  INEXACT starreal abig;
  starreal ahi, alo, bhi, blo;
  starreal err1, err2, err3;
  INEXACT starreal _i, _j;
  starreal _0;

  Two_Product(pa[0], pb[1], axby1, axby0);
  Two_Product(pb[0], pa[1], bxay1, bxay0);
  Two_Two_Diff(axby1, axby0, bxay1, bxay0, ab[3], ab[2], ab[1], ab[0]);

  Two_Product(pb[0], pc[1], bxcy1, bxcy0);
  Two_Product(pc[0], pb[1], cxby1, cxby0);
  Two_Two_Diff(bxcy1, bxcy0, cxby1, cxby0, bc[3], bc[2], bc[1], bc[0]);

  Two_Product(pc[0], pd[1], cxdy1, cxdy0);
  Two_Product(pd[0], pc[1], dxcy1, dxcy0);
  Two_Two_Diff(cxdy1, cxdy0, dxcy1, dxcy0, cd[3], cd[2], cd[1], cd[0]);

  Two_Product(pd[0], pe[1], dxey1, dxey0);
  Two_Product(pe[0], pd[1], exdy1, exdy0);
  Two_Two_Diff(dxey1, dxey0, exdy1, exdy0, de[3], de[2], de[1], de[0]);

  Two_Product(pe[0], pa[1], exay1, exay0);
  Two_Product(pa[0], pe[1], axey1, axey0);
  Two_Two_Diff(exay1, exay0, axey1, axey0, ea[3], ea[2], ea[1], ea[0]);

  Two_Product(pa[0], pc[1], axcy1, axcy0);
  Two_Product(pc[0], pa[1], cxay1, cxay0);
  Two_Two_Diff(axcy1, axcy0, cxay1, cxay0, ac[3], ac[2], ac[1], ac[0]);

  Two_Product(pb[0], pd[1], bxdy1, bxdy0);
  Two_Product(pd[0], pb[1], dxby1, dxby0);
  Two_Two_Diff(bxdy1, bxdy0, dxby1, dxby0, bd[3], bd[2], bd[1], bd[0]);

  Two_Product(pc[0], pe[1], cxey1, cxey0);
  Two_Product(pe[0], pc[1], excy1, excy0);
  Two_Two_Diff(cxey1, cxey0, excy1, excy0, ce[3], ce[2], ce[1], ce[0]);

  Two_Product(pd[0], pa[1], dxay1, dxay0);
  Two_Product(pa[0], pd[1], axdy1, axdy0);
  Two_Two_Diff(dxay1, dxay0, axdy1, axdy0, da[3], da[2], da[1], da[0]);

  Two_Product(pe[0], pb[1], exby1, exby0);
  Two_Product(pb[0], pe[1], bxey1, bxey0);
  Two_Two_Diff(exby1, exby0, bxey1, bxey0, eb[3], eb[2], eb[1], eb[0]);

  temp8alen = scale_expansion_zeroelim(4, bc, pa[2], temp8a);
  temp8blen = scale_expansion_zeroelim(4, ac, -pb[2], temp8b);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp8blen, temp8b,
                                          temp16);
  temp8alen = scale_expansion_zeroelim(4, ab, pc[2], temp8a);
  abclen = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp16len, temp16,
                                       abc);

  temp8alen = scale_expansion_zeroelim(4, cd, pb[2], temp8a);
  temp8blen = scale_expansion_zeroelim(4, bd, -pc[2], temp8b);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp8blen, temp8b,
                                          temp16);
  temp8alen = scale_expansion_zeroelim(4, bc, pd[2], temp8a);
  bcdlen = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp16len, temp16,
                                       bcd);

  temp8alen = scale_expansion_zeroelim(4, de, pc[2], temp8a);
  temp8blen = scale_expansion_zeroelim(4, ce, -pd[2], temp8b);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp8blen, temp8b,
                                          temp16);
  temp8alen = scale_expansion_zeroelim(4, cd, pe[2], temp8a);
  cdelen = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp16len, temp16,
                                       cde);

  temp8alen = scale_expansion_zeroelim(4, ea, pd[2], temp8a);
  temp8blen = scale_expansion_zeroelim(4, da, -pe[2], temp8b);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp8blen, temp8b,
                                          temp16);
  temp8alen = scale_expansion_zeroelim(4, de, pa[2], temp8a);
  dealen = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp16len, temp16,
                                       dea);

  temp8alen = scale_expansion_zeroelim(4, ab, pe[2], temp8a);
  temp8blen = scale_expansion_zeroelim(4, eb, -pa[2], temp8b);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp8blen, temp8b,
                                          temp16);
  temp8alen = scale_expansion_zeroelim(4, ea, pb[2], temp8a);
  eablen = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp16len, temp16,
                                       eab);

  temp8alen = scale_expansion_zeroelim(4, bd, pa[2], temp8a);
  temp8blen = scale_expansion_zeroelim(4, da, pb[2], temp8b);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp8blen, temp8b,
                                          temp16);
  temp8alen = scale_expansion_zeroelim(4, ab, pd[2], temp8a);
  abdlen = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp16len, temp16,
                                       abd);

  temp8alen = scale_expansion_zeroelim(4, ce, pb[2], temp8a);
  temp8blen = scale_expansion_zeroelim(4, eb, pc[2], temp8b);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp8blen, temp8b,
                                          temp16);
  temp8alen = scale_expansion_zeroelim(4, bc, pe[2], temp8a);
  bcelen = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp16len, temp16,
                                       bce);

  temp8alen = scale_expansion_zeroelim(4, da, pc[2], temp8a);
  temp8blen = scale_expansion_zeroelim(4, ac, pd[2], temp8b);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp8blen, temp8b,
                                          temp16);
  temp8alen = scale_expansion_zeroelim(4, cd, pa[2], temp8a);
  cdalen = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp16len, temp16,
                                       cda);

  temp8alen = scale_expansion_zeroelim(4, eb, pd[2], temp8a);
  temp8blen = scale_expansion_zeroelim(4, bd, pe[2], temp8b);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp8blen, temp8b,
                                          temp16);
  temp8alen = scale_expansion_zeroelim(4, de, pb[2], temp8a);
  deblen = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp16len, temp16,
                                       deb);

  temp8alen = scale_expansion_zeroelim(4, ac, pe[2], temp8a);
  temp8blen = scale_expansion_zeroelim(4, ce, pa[2], temp8b);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp8blen, temp8b,
                                          temp16);
  temp8alen = scale_expansion_zeroelim(4, ea, pc[2], temp8a);
  eaclen = fast_expansion_sum_zeroelim(temp8alen, temp8a, temp16len, temp16,
                                       eac);

  temp48alen = fast_expansion_sum_zeroelim(cdelen, cde, bcelen, bce, temp48a);
  temp48blen = fast_expansion_sum_zeroelim(deblen, deb, bcdlen, bcd, temp48b);
  for (i = 0; i < temp48blen; i++) {
    temp48b[i] = -temp48b[i];
  }
  bcdelen = fast_expansion_sum_zeroelim(temp48alen, temp48a,
                                        temp48blen, temp48b, bcde);
  alen = scale_expansion_zeroelim(bcdelen, bcde, aheight, adet);

  temp48alen = fast_expansion_sum_zeroelim(dealen, dea, cdalen, cda, temp48a);
  temp48blen = fast_expansion_sum_zeroelim(eaclen, eac, cdelen, cde, temp48b);
  for (i = 0; i < temp48blen; i++) {
    temp48b[i] = -temp48b[i];
  }
  cdealen = fast_expansion_sum_zeroelim(temp48alen, temp48a,
                                        temp48blen, temp48b, cdea);
  blen = scale_expansion_zeroelim(cdealen, cdea, bheight, bdet);

  temp48alen = fast_expansion_sum_zeroelim(eablen, eab, deblen, deb, temp48a);
  temp48blen = fast_expansion_sum_zeroelim(abdlen, abd, dealen, dea, temp48b);
  for (i = 0; i < temp48blen; i++) {
    temp48b[i] = -temp48b[i];
  }
  deablen = fast_expansion_sum_zeroelim(temp48alen, temp48a,
                                        temp48blen, temp48b, deab);
  clen = scale_expansion_zeroelim(deablen, deab, cheight, cdet);

  temp48alen = fast_expansion_sum_zeroelim(abclen, abc, eaclen, eac, temp48a);
  temp48blen = fast_expansion_sum_zeroelim(bcelen, bce, eablen, eab, temp48b);
  for (i = 0; i < temp48blen; i++) {
    temp48b[i] = -temp48b[i];
  }
  eabclen = fast_expansion_sum_zeroelim(temp48alen, temp48a,
                                        temp48blen, temp48b, eabc);
  dlen = scale_expansion_zeroelim(eabclen, eabc, dheight, ddet);

  temp48alen = fast_expansion_sum_zeroelim(bcdlen, bcd, abdlen, abd, temp48a);
  temp48blen = fast_expansion_sum_zeroelim(cdalen, cda, abclen, abc, temp48b);
  for (i = 0; i < temp48blen; i++) {
    temp48b[i] = -temp48b[i];
  }
  abcdlen = fast_expansion_sum_zeroelim(temp48alen, temp48a,
                                        temp48blen, temp48b, abcd);
  elen = scale_expansion_zeroelim(abcdlen, abcd, eheight, edet);

  ablen = fast_expansion_sum_zeroelim(alen, adet, blen, bdet, abdet);
  cdlen = fast_expansion_sum_zeroelim(clen, cdet, dlen, ddet, cddet);
  cdelen = fast_expansion_sum_zeroelim(cdlen, cddet, elen, edet, cdedet);
  deterlen = fast_expansion_sum_zeroelim(ablen, abdet, cdelen, cdedet, deter);

  return deter[deterlen - 1];
}

starreal orient4dadapt(starreal *pa,
                       starreal *pb,
                       starreal *pc,
                       starreal *pd,
                       starreal *pe,
                       starreal aheight,
                       starreal bheight,
                       starreal cheight,
                       starreal dheight,
                       starreal eheight,
                       starreal permanent)
{
  INEXACT starreal aex, bex, cex, dex, aey, bey, cey, dey, aez, bez, cez, dez;
  INEXACT starreal aeheight, beheight, ceheight, deheight;
  starreal det, errbound;

  INEXACT starreal aexbey1, bexaey1, bexcey1, cexbey1;
  INEXACT starreal cexdey1, dexcey1, dexaey1, aexdey1;
  INEXACT starreal aexcey1, cexaey1, bexdey1, dexbey1;
  starreal aexbey0, bexaey0, bexcey0, cexbey0;
  starreal cexdey0, dexcey0, dexaey0, aexdey0;
  starreal aexcey0, cexaey0, bexdey0, dexbey0;
  starreal ab[4], bc[4], cd[4], da[4], ac[4], bd[4];
  INEXACT starreal ab3, bc3, cd3, da3, ac3, bd3;
  starreal abeps, bceps, cdeps, daeps, aceps, bdeps;
  starreal temp8a[8], temp8b[8], temp8c[8], temp16[16], temp24[24];
  int temp8alen, temp8blen, temp8clen, temp16len, temp24len;
  starreal adet[48], bdet[48], cdet[48], ddet[48];
  int alen, blen, clen, dlen;
  starreal abdet[96], cddet[96];
  int ablen, cdlen;
  starreal fin1[192];
  int finlength;

  starreal aextail, bextail, cextail, dextail;
  starreal aeytail, beytail, ceytail, deytail;
  starreal aeztail, beztail, ceztail, deztail;
  starreal aeheighttail, beheighttail, ceheighttail, deheighttail;

  INEXACT starreal bvirt;
  starreal avirt, bround, around;
  INEXACT starreal c;
  INEXACT starreal abig;
  starreal ahi, alo, bhi, blo;
  starreal err1, err2, err3;
  INEXACT starreal _i, _j;
  starreal _0;

  aex = (starreal) (pa[0] - pe[0]);
  bex = (starreal) (pb[0] - pe[0]);
  cex = (starreal) (pc[0] - pe[0]);
  dex = (starreal) (pd[0] - pe[0]);
  aey = (starreal) (pa[1] - pe[1]);
  bey = (starreal) (pb[1] - pe[1]);
  cey = (starreal) (pc[1] - pe[1]);
  dey = (starreal) (pd[1] - pe[1]);
  aez = (starreal) (pa[2] - pe[2]);
  bez = (starreal) (pb[2] - pe[2]);
  cez = (starreal) (pc[2] - pe[2]);
  dez = (starreal) (pd[2] - pe[2]);
  aeheight = (starreal) (aheight - eheight);
  beheight = (starreal) (bheight - eheight);
  ceheight = (starreal) (cheight - eheight);
  deheight = (starreal) (dheight - eheight);

  Two_Product(aex, bey, aexbey1, aexbey0);
  Two_Product(bex, aey, bexaey1, bexaey0);
  Two_Two_Diff(aexbey1, aexbey0, bexaey1, bexaey0, ab3, ab[2], ab[1], ab[0]);
  ab[3] = ab3;

  Two_Product(bex, cey, bexcey1, bexcey0);
  Two_Product(cex, bey, cexbey1, cexbey0);
  Two_Two_Diff(bexcey1, bexcey0, cexbey1, cexbey0, bc3, bc[2], bc[1], bc[0]);
  bc[3] = bc3;

  Two_Product(cex, dey, cexdey1, cexdey0);
  Two_Product(dex, cey, dexcey1, dexcey0);
  Two_Two_Diff(cexdey1, cexdey0, dexcey1, dexcey0, cd3, cd[2], cd[1], cd[0]);
  cd[3] = cd3;

  Two_Product(dex, aey, dexaey1, dexaey0);
  Two_Product(aex, dey, aexdey1, aexdey0);
  Two_Two_Diff(dexaey1, dexaey0, aexdey1, aexdey0, da3, da[2], da[1], da[0]);
  da[3] = da3;

  Two_Product(aex, cey, aexcey1, aexcey0);
  Two_Product(cex, aey, cexaey1, cexaey0);
  Two_Two_Diff(aexcey1, aexcey0, cexaey1, cexaey0, ac3, ac[2], ac[1], ac[0]);
  ac[3] = ac3;

  Two_Product(bex, dey, bexdey1, bexdey0);
  Two_Product(dex, bey, dexbey1, dexbey0);
  Two_Two_Diff(bexdey1, bexdey0, dexbey1, dexbey0, bd3, bd[2], bd[1], bd[0]);
  bd[3] = bd3;

  temp8alen = scale_expansion_zeroelim(4, cd, bez, temp8a);
  temp8blen = scale_expansion_zeroelim(4, bd, -cez, temp8b);
  temp8clen = scale_expansion_zeroelim(4, bc, dez, temp8c);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a,
                                          temp8blen, temp8b, temp16);
  temp24len = fast_expansion_sum_zeroelim(temp8clen, temp8c,
                                          temp16len, temp16, temp24);
  alen = scale_expansion_zeroelim(temp24len, temp24, -aeheight, adet);

  temp8alen = scale_expansion_zeroelim(4, da, cez, temp8a);
  temp8blen = scale_expansion_zeroelim(4, ac, dez, temp8b);
  temp8clen = scale_expansion_zeroelim(4, cd, aez, temp8c);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a,
                                          temp8blen, temp8b, temp16);
  temp24len = fast_expansion_sum_zeroelim(temp8clen, temp8c,
                                          temp16len, temp16, temp24);
  blen = scale_expansion_zeroelim(temp24len, temp24, beheight, bdet);

  temp8alen = scale_expansion_zeroelim(4, ab, dez, temp8a);
  temp8blen = scale_expansion_zeroelim(4, bd, aez, temp8b);
  temp8clen = scale_expansion_zeroelim(4, da, bez, temp8c);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a,
                                          temp8blen, temp8b, temp16);
  temp24len = fast_expansion_sum_zeroelim(temp8clen, temp8c,
                                          temp16len, temp16, temp24);
  clen = scale_expansion_zeroelim(temp24len, temp24, -ceheight, cdet);

  temp8alen = scale_expansion_zeroelim(4, bc, aez, temp8a);
  temp8blen = scale_expansion_zeroelim(4, ac, -bez, temp8b);
  temp8clen = scale_expansion_zeroelim(4, ab, cez, temp8c);
  temp16len = fast_expansion_sum_zeroelim(temp8alen, temp8a,
                                          temp8blen, temp8b, temp16);
  temp24len = fast_expansion_sum_zeroelim(temp8clen, temp8c,
                                          temp16len, temp16, temp24);
  dlen = scale_expansion_zeroelim(temp24len, temp24, deheight, ddet);

  ablen = fast_expansion_sum_zeroelim(alen, adet, blen, bdet, abdet);
  cdlen = fast_expansion_sum_zeroelim(clen, cdet, dlen, ddet, cddet);
  finlength = fast_expansion_sum_zeroelim(ablen, abdet, cdlen, cddet, fin1);

  det = estimate(finlength, fin1);
  errbound = isperrboundB * permanent;
  if ((det >= errbound) || (-det >= errbound)) {
    return det;
  }

  Two_Diff_Tail(pa[0], pe[0], aex, aextail);
  Two_Diff_Tail(pa[1], pe[1], aey, aeytail);
  Two_Diff_Tail(pa[2], pe[2], aez, aeztail);
  Two_Diff_Tail(aheight, eheight, aeheight, aeheighttail);
  Two_Diff_Tail(pb[0], pe[0], bex, bextail);
  Two_Diff_Tail(pb[1], pe[1], bey, beytail);
  Two_Diff_Tail(pb[2], pe[2], bez, beztail);
  Two_Diff_Tail(bheight, eheight, beheight, beheighttail);
  Two_Diff_Tail(pc[0], pe[0], cex, cextail);
  Two_Diff_Tail(pc[1], pe[1], cey, ceytail);
  Two_Diff_Tail(pc[2], pe[2], cez, ceztail);
  Two_Diff_Tail(cheight, eheight, ceheight, ceheighttail);
  Two_Diff_Tail(pd[0], pe[0], dex, dextail);
  Two_Diff_Tail(pd[1], pe[1], dey, deytail);
  Two_Diff_Tail(pd[2], pe[2], dez, deztail);
  Two_Diff_Tail(dheight, eheight, deheight, deheighttail);
  if ((aextail == 0.0) && (aeytail == 0.0) && (aeztail == 0.0)
      && (bextail == 0.0) && (beytail == 0.0) && (beztail == 0.0)
      && (cextail == 0.0) && (ceytail == 0.0) && (ceztail == 0.0)
      && (dextail == 0.0) && (deytail == 0.0) && (deztail == 0.0)
      && (aeheighttail == 0.0) && (beheighttail == 0.0)
      && (ceheighttail == 0.0) && (deheighttail == 0.0)) {
    return det;
  }

  errbound = isperrboundC * permanent + resulterrbound * Absolute(det);
  abeps = (aex * beytail + bey * aextail)
        - (aey * bextail + bex * aeytail);
  bceps = (bex * ceytail + cey * bextail)
        - (bey * cextail + cex * beytail);
  cdeps = (cex * deytail + dey * cextail)
        - (cey * dextail + dex * ceytail);
  daeps = (dex * aeytail + aey * dextail)
        - (dey * aextail + aex * deytail);
  aceps = (aex * ceytail + cey * aextail)
        - (aey * cextail + cex * aeytail);
  bdeps = (bex * deytail + dey * bextail)
        - (bey * dextail + dex * beytail);
  det += ((beheight
           * ((cez * daeps + dez * aceps + aez * cdeps)
              + (ceztail * da3 + deztail * ac3 + aeztail * cd3))
           + deheight
           * ((aez * bceps - bez * aceps + cez * abeps)
              + (aeztail * bc3 - beztail * ac3 + ceztail * ab3)))
          - (aeheight
           * ((bez * cdeps - cez * bdeps + dez * bceps)
              + (beztail * cd3 - ceztail * bd3 + deztail * bc3))
           + ceheight
           * ((dez * abeps + aez * bdeps + bez * daeps)
              + (deztail * ab3 + aeztail * bd3 + beztail * da3))))
       + ((beheighttail * (cez * da3 + dez * ac3 + aez * cd3)
           + deheighttail * (aez * bc3 - bez * ac3 + cez * ab3))
          - (aeheighttail * (bez * cd3 - cez * bd3 + dez * bc3)
           + ceheighttail * (dez * ab3 + aez * bd3 + bez * da3)));
  if ((det >= errbound) || (-det >= errbound)) {
    return det;
  }

  return orient4dexact(pa, pb, pc, pd, pe,
                       aheight, bheight, cheight, dheight, eheight);
}

starreal orient4d(struct behavior *b,
                  starreal *pa,
                  starreal *pb,
                  starreal *pc,
                  starreal *pd,
                  starreal *pe,
                  starreal aheight,
                  starreal bheight,
                  starreal cheight,
                  starreal dheight,
                  starreal eheight)
{
  starreal aex, bex, cex, dex;
  starreal aey, bey, cey, dey;
  starreal aez, bez, cez, dez;
  starreal aexbey, bexaey, bexcey, cexbey, cexdey, dexcey, dexaey, aexdey;
  starreal aexcey, cexaey, bexdey, dexbey;
  starreal aeheight, beheight, ceheight, deheight;
  starreal ab, bc, cd, da, ac, bd;
  starreal abc, bcd, cda, dab;
  starreal aezplus, bezplus, cezplus, dezplus;
  starreal aexbeyplus, bexaeyplus, bexceyplus, cexbeyplus;
  starreal cexdeyplus, dexceyplus, dexaeyplus, aexdeyplus;
  starreal aexceyplus, cexaeyplus, bexdeyplus, dexbeyplus;
  starreal det;
  starreal permanent, errbound;

  b->orient4dcount++;

  aex = pa[0] - pe[0];
  bex = pb[0] - pe[0];
  cex = pc[0] - pe[0];
  dex = pd[0] - pe[0];
  aey = pa[1] - pe[1];
  bey = pb[1] - pe[1];
  cey = pc[1] - pe[1];
  dey = pd[1] - pe[1];
  aez = pa[2] - pe[2];
  bez = pb[2] - pe[2];
  cez = pc[2] - pe[2];
  dez = pd[2] - pe[2];
  aeheight = aheight - eheight;
  beheight = bheight - eheight;
  ceheight = cheight - eheight;
  deheight = dheight - eheight;

  aexbey = aex * bey;
  bexaey = bex * aey;
  ab = aexbey - bexaey;
  bexcey = bex * cey;
  cexbey = cex * bey;
  bc = bexcey - cexbey;
  cexdey = cex * dey;
  dexcey = dex * cey;
  cd = cexdey - dexcey;
  dexaey = dex * aey;
  aexdey = aex * dey;
  da = dexaey - aexdey;

  aexcey = aex * cey;
  cexaey = cex * aey;
  ac = aexcey - cexaey;
  bexdey = bex * dey;
  dexbey = dex * bey;
  bd = bexdey - dexbey;

  abc = aez * bc - bez * ac + cez * ab;
  bcd = bez * cd - cez * bd + dez * bc;
  cda = cez * da + dez * ac + aez * cd;
  dab = dez * ab + aez * bd + bez * da;

  det = (deheight * abc - ceheight * dab) + (beheight * cda - aeheight * bcd);

  if (b->noexact) {
    return det;
  }

  aezplus = Absolute(aez);
  bezplus = Absolute(bez);
  cezplus = Absolute(cez);
  dezplus = Absolute(dez);
  aexbeyplus = Absolute(aexbey);
  bexaeyplus = Absolute(bexaey);
  bexceyplus = Absolute(bexcey);
  cexbeyplus = Absolute(cexbey);
  cexdeyplus = Absolute(cexdey);
  dexceyplus = Absolute(dexcey);
  dexaeyplus = Absolute(dexaey);
  aexdeyplus = Absolute(aexdey);
  aexceyplus = Absolute(aexcey);
  cexaeyplus = Absolute(cexaey);
  bexdeyplus = Absolute(bexdey);
  dexbeyplus = Absolute(dexbey);
  permanent = ((cexdeyplus + dexceyplus) * bezplus
               + (dexbeyplus + bexdeyplus) * cezplus
               + (bexceyplus + cexbeyplus) * dezplus)
            * aeheight
            + ((dexaeyplus + aexdeyplus) * cezplus
               + (aexceyplus + cexaeyplus) * dezplus
               + (cexdeyplus + dexceyplus) * aezplus)
            * beheight
            + ((aexbeyplus + bexaeyplus) * dezplus
               + (bexdeyplus + dexbeyplus) * aezplus
               + (dexaeyplus + aexdeyplus) * bezplus)
            * ceheight
            + ((bexceyplus + cexbeyplus) * aezplus
               + (cexaeyplus + aexceyplus) * bezplus
               + (aexbeyplus + bexaeyplus) * cezplus)
            * deheight;
  errbound = isperrboundA * permanent;
  if ((det > errbound) || (-det > errbound)) {
    return det;
  }

  return orient4dadapt(pa, pb, pc, pd, pe,
                       aheight, bheight, cheight, dheight, eheight, permanent);
}

/*****************************************************************************/
/*                                                                           */
/*  nonregular()   Return a positive value if the point pe is incompatible   */
/*                 with the sphere or hyperplane passing through pa, pb, pc, */
/*                 and pd (meaning that pe is inside the sphere or below the */
/*                 hyperplane); a negative value if it is compatible; and    */
/*                 zero if the five points are cospherical/cohyperplanar.    */
/*                 The points pa, pb, pc, and pd must be ordered so that     */
/*                 they have a positive orientation (as defined by           */
/*                 orient3d()), or the sign of the result will be reversed.  */
/*                                                                           */
/*  If the -w switch is used, the points are lifted onto the parabolic       */
/*  lifting map, then they are adjusted according to their weights, then the */
/*  4D orientation test is applied.  If the -W switch is used, the points'   */
/*  heights are already provided, so the 4D orientation test is applied      */
/*  directly.  If neither switch is used, the insphere test is applied.      */
/*                                                                           */
/*****************************************************************************/

starreal nonregular(struct behavior *b,
                    starreal *pa,
                    starreal *pb,
                    starreal *pc,
                    starreal *pd,
                    starreal *pe)
{
  if (b->weighted == 0) {
    return insphere(b, pa, pb, pc, pd, pe);
  } else if (b->weighted == 1) {
    return orient4d(b, pa, pb, pc, pd, pe,
                    pa[0] * pa[0] + pa[1] * pa[1] + pa[2] * pa[2] - pa[3],
                    pb[0] * pb[0] + pb[1] * pb[1] + pb[2] * pb[2] - pb[3],
                    pc[0] * pc[0] + pc[1] * pc[1] + pc[2] * pc[2] - pc[3],
                    pd[0] * pd[0] + pd[1] * pd[1] + pd[2] * pd[2] - pd[3],
                    pe[0] * pe[0] + pe[1] * pe[1] + pe[2] * pe[2] - pe[3]);
  } else {
    return orient4d(b, pa, pb, pc, pd, pe, pa[3], pb[3], pc[3], pd[3], pe[3]);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcircumcenter()   Find the circumcenter of a tetrahedron.              */
/*                                                                           */
/*  The result is returned both in terms of xyz coordinates and xi-eta-zeta  */
/*  coordinates, relative to the tetrahedron's apex (that is, the apex is    */
/*  the origin of both coordinate systems).  Hence, the xyz coordinates      */
/*  returned are NOT absolute; one must add the coordinates of the           */
/*  tetrahedron apex to find the absolute coordinates of the circumcircle.   */
/*  However, this means that the result is frequently more accurate than     */
/*  would be possible if absolute coordinates were returned, due to limited  */
/*  floating-point precision.                                                */
/*                                                                           */
/*  The xi-eta-zeta coordinate system is defined in terms of the             */
/*  tetrahedron:  the apex of the tetrahedron is the origin of the           */
/*  coordinate system.  The length from the apex to the origin of the        */
/*  tetrahedron is one unit along the xi axis.  The eta and zeta axes are    */
/*  defined similarly by the destination and face apex.                      */
/*                                                                           */
/*  If `xi' is NULL on input, the xi-eta-zeta coordinates will not be        */
/*  computed.                                                                */
/*                                                                           */
/*****************************************************************************/

void tetcircumcenter(struct behavior *b,
                     starreal *tetorg,
                     starreal *tetdest,
                     starreal *tetfapex,
                     starreal *tettapex,
                     starreal *circumcenter,
                     starreal *xi,
                     starreal *eta,
                     starreal *zeta)
{
  starreal xot, yot, zot, xdt, ydt, zdt, xft, yft, zft;
  starreal otlength, dtlength, ftlength;
  starreal xcrossdf, ycrossdf, zcrossdf;
  starreal xcrossfo, ycrossfo, zcrossfo;
  starreal xcrossod, ycrossod, zcrossod;
  starreal denominator;
  starreal xct, yct, zct;

  b->tetcircumcentercount++;

  /* Use coordinates relative to the apex of the tetrahedron. */
  xot = tetorg[0] - tettapex[0];
  yot = tetorg[1] - tettapex[1];
  zot = tetorg[2] - tettapex[2];
  xdt = tetdest[0] - tettapex[0];
  ydt = tetdest[1] - tettapex[1];
  zdt = tetdest[2] - tettapex[2];
  xft = tetfapex[0] - tettapex[0];
  yft = tetfapex[1] - tettapex[1];
  zft = tetfapex[2] - tettapex[2];
  /* Squares of lengths of the origin, destination, and face apex edges. */
  otlength = xot * xot + yot * yot + zot * zot;
  dtlength = xdt * xdt + ydt * ydt + zdt * zdt;
  ftlength = xft * xft + yft * yft + zft * zft;
  /* Cross products of the origin, destination, and face apex vectors. */
  xcrossdf = ydt * zft - yft * zdt;
  ycrossdf = zdt * xft - zft * xdt;
  zcrossdf = xdt * yft - xft * ydt;
  xcrossfo = yft * zot - yot * zft;
  ycrossfo = zft * xot - zot * xft;
  zcrossfo = xft * yot - xot * yft;
  xcrossod = yot * zdt - ydt * zot;
  ycrossod = zot * xdt - zdt * xot;
  zcrossod = xot * ydt - xdt * yot;

  /* Calculate the denominator of all the formulae. */
  if (b->noexact) {
    denominator = 0.5 / (xot * xcrossdf + yot * ycrossdf + zot * zcrossdf);
  } else {
    /* Use the orient3d() routine to ensure a positive (and    */
    /*   reasonably accurate) result, avoiding any possibility */
    /*   of division by zero.                                  */
    denominator = 0.5 / orient3d(b, tetorg, tetdest, tetfapex, tettapex);
    /* Don't count the above as an orientation test. */
    b->orientcount--;
  }

  /* Calculate offset (from apex) of circumcenter. */
  xct = (otlength * xcrossdf + dtlength * xcrossfo + ftlength * xcrossod) *
        denominator;
  yct = (otlength * ycrossdf + dtlength * ycrossfo + ftlength * ycrossod) *
        denominator;
  zct = (otlength * zcrossdf + dtlength * zcrossfo + ftlength * zcrossod) *
        denominator;
  circumcenter[0] = xct;
  circumcenter[1] = yct;
  circumcenter[2] = zct;

  if (xi != (starreal *) NULL) {
    /* To interpolate vertex attributes for the new vertex inserted at */
    /*   the circumcenter, define a coordinate system with a xi-axis,  */
    /*   directed from the tetrahedron's apex to its origin, an        */
    /*   eta-axis, directed from its apex to its destination, and a    */
    /*   zeta-axis, directed from its apex to its face apex.   The     */
    /*   values for xi, eta, and zeta are computed by Cramer's Rule    */
    /*   for solving systems of linear equations.                      */
    *xi = (xct * xcrossdf + yct * ycrossdf + zct * zcrossdf) *
          (2.0 * denominator);
    *eta = (xct * xcrossfo + yct * ycrossfo + zct * zcrossfo) *
           (2.0 * denominator);
    *zeta = (xct * xcrossod + yct * ycrossod + zct * zcrossod) *
            (2.0 * denominator);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tricircumcenter3d()   Find the circumcenter of a triangle in 3D.         */
/*                                                                           */
/*  The result is returned both in terms of xyz coordinates and xi-eta       */
/*  coordinates, relative to the triangle's apex (that is, the apex is the   */
/*  origin of both coordinate systems).  Hence, the xyz coordinates returned */
/*  are NOT absolute; one must add the coordinates of the apex to find the   */
/*  absolute coordinates of the circumcircle.  However, this means that the  */
/*  result is frequently more accurate than would be possible if absolute    */
/*  coordinates were returned, due to limited floating-point precision.  In  */
/*  particular, the result can be used for a more accurate calculation of    */
/*  the radius of the triangle's circumcirle.                                */
/*                                                                           */
/*  The xi-eta coordinate system is defined in terms of the triangle:  the   */
/*  apex of the triangle is the origin of the coordinate system.  The length */
/*  from the apex to the origin of the triangle is one unit along the xi     */
/*  axis.  The length from the apex to the destination is one unit along the */
/*  eta axis.                                                                */
/*                                                                           */
/*  A normal vector to the triangle is returned in `normal'.  This vector is */
/*  a cross product of two of the triangle's edge, so its length is twice    */
/*  the triangle's area.  It points up from the face (assuming the triangle  */
/*  lies with its origin, destination, and apex in counterclockwise order).  */
/*                                                                           */
/*  If `normal' is NULL on input, the normal will not be returned.           */
/*  If `xi' is NULL on input, the xi-eta coordinates will not be computed.   */
/*                                                                           */
/*****************************************************************************/

void tricircumcenter3d(struct behavior *b,
                       starreal *triorg,
                       starreal *tridest,
                       starreal *triapex,
                       starreal *circumcenter,
                       starreal *normal,
                       starreal *xi,
                       starreal *eta)
{
  starreal xoa, yoa, zoa, xda, yda, zda;
  starreal oalength, dalength;
  starreal xcrossod, ycrossod, zcrossod;
  starreal denominator;
  starreal xca, yca, zca;

  b->tricircumcentercount++;

  /* Use coordinates relative to the apex of the triangle. */
  xoa = triorg[0] - triapex[0];
  yoa = triorg[1] - triapex[1];
  zoa = triorg[2] - triapex[2];
  xda = tridest[0] - triapex[0];
  yda = tridest[1] - triapex[1];
  zda = tridest[2] - triapex[2];
  /* Squares of lengths of the origin and destination edges. */
  oalength = xoa * xoa + yoa * yoa + zoa * zoa;
  dalength = xda * xda + yda * yda + zda * zda;
  
  /* Cross product of the origin and destination vectors. */
  if (b->noexact) {
    xcrossod = yoa * zda - yda * zoa;
    ycrossod = zoa * xda - zda * xoa;
    zcrossod = xoa * yda - xda * yoa;
  } else {
    /* Use the orient2d() routine to ensure a nonzero (and     */
    /*   reasonably accurate) result, avoiding any possibility */
    /*   of division by zero.                                  */
    xcrossod = orient2d(b, triorg[1], triorg[2], tridest[1], tridest[2],
                        triapex[1], triapex[2]);
    ycrossod = orient2d(b, triorg[2], triorg[0], tridest[2], tridest[0],
                        triapex[2], triapex[0]);
    zcrossod = orient2d(b, triorg[0], triorg[1], tridest[0], tridest[1],
                        triapex[0], triapex[1]);
  }
  /* Calculate the denominator of the circumcenter. */
  denominator = 0.5 / (xcrossod * xcrossod + ycrossod * ycrossod +
                       zcrossod * zcrossod);

  /* Calculate offset (from apex) of circumcenter. */
  xca = ((oalength * yda - dalength * yoa) * zcrossod -
         (oalength * zda - dalength * zoa) * ycrossod) * denominator;
  yca = ((oalength * zda - dalength * zoa) * xcrossod -
         (oalength * xda - dalength * xoa) * zcrossod) * denominator;
  zca = ((oalength * xda - dalength * xoa) * ycrossod -
         (oalength * yda - dalength * yoa) * xcrossod) * denominator;
  circumcenter[0] = xca;
  circumcenter[1] = yca;
  circumcenter[2] = zca;

  if (normal != (starreal *) NULL) {
    normal[0] = xcrossod;
    normal[1] = ycrossod;
    normal[2] = zcrossod;
  }

  if (xi != (starreal *) NULL) {
    /* To interpolate vertex attributes for the new vertex inserted at */
    /*   the circumcenter, define a coordinate system with a xi-axis,  */
    /*   directed from the tetrahedron's apex to its origin, and an    */
    /*   eta-axis, directed from its apex to its destination.  The     */
    /*   values for xi and eta are computed by Cramer's Rule for       */
    /*   solving systems of linear equations.                          */

    /* There are three ways to do this calculation - using xcrossod, */
    /*   ycrossod, or zcrossod.  Choose whichever has the largest    */
    /*   magnitude, to improve stability and avoid division by zero. */
    if (((xcrossod >= ycrossod) ^ (-xcrossod > ycrossod)) &&
        ((xcrossod >= zcrossod) ^ (-xcrossod > zcrossod))) {
      *xi = (yca * zda - yda * zca) / xcrossod;
      *eta = (yoa * zca - yca * zoa) / xcrossod;
    } else if ((ycrossod >= zcrossod) ^ (-ycrossod > zcrossod)) {
      *xi = (zca * xda - zda * xca) / ycrossod;
      *eta = (zoa * xca - zca * xoa) / ycrossod;
    } else {
      *xi = (xca * yda - xda * yca) / zcrossod;
      *eta = (xoa * yca - xca * yoa) / zcrossod;
    }
  }
}

/**                                                                         **/
/**                                                                         **/
/********* Geometric primitives end here                             *********/


/********* Tetrahedral complex routines begin here                   *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  Space-efficient tetrahedral complexes (struct tetcomplex)                */
/*                                                                           */
/*  Roughly speaking, a tetrahedral complex (as it is implemented here) is   */
/*  a set of tetrahedra that follow the rules of simplicial complexes.  It   */
/*  cannot represent a three-dimensional simplicial complex with full        */
/*  generality, though, because it does not (fully) support the presence of  */
/*  edges or triangles that are not faces of any tetrahedron.                */
/*                                                                           */
/*  The tetcomplex procedures are purely topological, which means that none  */
/*  of them ever looks at the coordinates of a vertex (or even knows how     */
/*  vertices are represented).  A tetrahedron is denoted by the labels of    */
/*  its four vertices, which are integer tags (that is, the `tag' type       */
/*  defined for proxipools).  The tetcomplex procedures take and return      */
/*  vertex tags (not data structures that represent vertices).               */
/*                                                                           */
/*  Every tetrahedron has an orientation, implied by the order of its        */
/*  vertices.  A tetrahedron can be oriented two ways.  All even             */
/*  permutations of a tetrahedron's vertices are considered equivalent       */
/*  (having the same orientation), and all odd permutations of a             */
/*  tetrahedron's vertices have the opposite orientation (but are equivalent */
/*  to each other).  So (1, 2, 3, 4) = (2, 1, 4, 3) = (1, 4, 2, 3), but      */
/*  (1, 2, 3, 4) is opposite to (1, 2, 4, 3).  Typically, topological        */
/*  orientation echoes geometrical orientation according to a right-hand     */
/*  rule:  the orientation (1, 2, 3, 4) implies that the vertices 2, 3, 4    */
/*  occur in counterclockwise order as seen from vertex 1.  If you curl the  */
/*  fingers of your right hand to follow the vertices 2, 3, 4, then your     */
/*  thumb points toward vertex 1.                                            */
/*                                                                           */
/*  You could use the left-hand rule (a mirror image) instead, and this data */
/*  structure won't know the difference.  However, all the tetrahedron       */
/*  orientations must agree!  This data structure does not support complexes */
/*  in which the tetrahedron orientations are mixed.                         */
/*                                                                           */
/*  The data structure is designed to support tetrahedral complexes in       */
/*  three-dimensional space.  One rule is that each triangle may be a face   */
/*  of at most two tetrahedra.  In other words, for any triple of vertices,  */
/*  only two tetrahedra can have all three of those vertices.  (This rules   */
/*  out general tetrahedral complexes in four-dimensional space.)            */
/*                                                                           */
/*  Another rule is that if two tetrahedra share a triangular face, their    */
/*  orientations must be such that the tetrahedra are on opposite sides of   */
/*  the face.  For example, the tetrahedra (1, 2, 3, 4) and (1, 3, 2, 5) can */
/*  coexist in a tetcomplex, but (1, 2, 3, 4) and (1, 2, 3, 5) cannot.       */
/*                                                                           */
/*  The data structure supports the insertion of tetrahedra, so long as they */
/*  obey these rules.  It supports the deletion of any tetrahedra in the     */
/*  complex.  There are also procedures for performing common bistellar      */
/*  flips.                                                                   */
/*                                                                           */
/*  The main query is a "triangle query", which takes three vertices         */
/*  (representing a triangle) as input, and returns the two tetrahedra       */
/*  adjoining the triangle, represented by the fourth vertex of each         */
/*  tetrahedron.  The absence of one (or both) of these tetrahedra is        */
/*  indicated by a value of GHOSTVERTEX as the fourth vertex.  If the        */
/*  triangle itself is missing from the complex, the return value indicates  */
/*  that.                                                                    */
/*                                                                           */
/*  There are also specialized procedures designed to help the Bowyer-Watson */
/*  algorithm for incremental insertion in Delaunay triangulations run fast. */
/*                                                                           */
/*  Public interface:                                                        */
/*  struct tetcomplex   Represents a tetrahedral complex.                    */
/*  struct tetcomplexstar   Holds information about one vertex's 2D link.    */
/*  struct tetcomplexposition   Represents a position for iterating through  */
/*    the tetrahedra in a complex.                                           */
/*  tetcomplexinit(plex, vertexpool, verbose)   Initialize a tetcomplex.     */
/*  tetcomplexrestart(plex)   Erase all tetrahedra.                          */
/*  tetcomplexdeinit(plex)   Free a tetcomplex's memory to the OS.           */
/*  tetcomplextag2vertex(plex, searchtag)   Map a tag to a pointer that      */
/*    points at a vertex data structure.  Unsafe; fast macro.                */
/*  tetcomplextag2attributes(plex, searchtag)   Map a tag to a pointer at    */
/*    the attributes associated with a vertex.  Unsafe; fast macro.          */
/*  tetcomplexadjacencies(plex, vtx1, vtx2, vtx3, adjacencies[2])   Find the */
/*    tetrahedra adjoining the triangle (vtx1, vtx2, vtx3).                  */
/*  tetcomplexiteratorinit(plex, pos)   Initialize an iterator over the      */
/*    tetrahedra of the complex.                                             */
/*  tetcomplexiterate(pos, nexttet[4])   Return the next tetrahedron,        */
/*    excluding duplicates.  Certain ghosts are included.                    */
/*  tetcomplexiteratenoghosts(pos, nexttet[4])   Return the next             */
/*    tetrahedron, excluding duplicates and ghosts.                          */
/*  tetcomplexprint(plex)   Print the tetrahedra in a tetcomplex.            */
/*  tetcomplexprintstars(plex)   Print the stars in a tetcomplex.            */
/*  tetcomplexconsistency(plex)   Check internal consistency of tetcomplex.  */
/*  tetcomplexdeletetet(plex, vtx1, vtx2, vtx3, vtx4)   Delete a             */
/*    tetrahedron.                                                           */
/*  tetcomplexinserttet(plex, vtx1, vtx2, vtx3, vtx4)   Insert a             */
/*    tetrahedron.                                                           */
/*  tetcomplex14flip(plex, vtx1, vtx2, vtx3, vtx4, newvertex)   Replace one  */
/*    tetrahedron with four by lazily inserting `newvertex'.                 */
/*  tetcomplex23flip(plex, vtxtop, vtx1, vtx2, vtx3, vtxbot)   Replace two   */
/*    tetrahedra with three (bistellar flip).                                */
/*  tetcomplex32flip(plex, vtxtop, vtx1, vtx2, vtx3, vtxbot)   Replace three */
/*    tetrahedra with two (bistellar flip).  Inverse of tetcomplex23flip.    */
/*  tetcomplex41flip(plex, vtx1, vtx2, vtx3, vtx4, deletevertex)   Replace   */
/*    four tetrahedra with one.  Inverse of tetcomplex14flip.                */
/*  tetcomplexremoveghosttets(plex)   Remove all the ghost tetrahedra.       */
/*  tetcomplextetcount(plex)   Return number of tetrahedra (no ghosts).      */
/*  tetcomplexghosttetcount(plex)   Return number of explicity inserted      */
/*    ghost tetrahedra.                                                      */
/*  tetcomplexbytes(plex)   Return bytes used by the complex.                */
/*                                                                           */
/*  For algorithms that use inconsistent stars (e.g. star splaying):         */
/*  tetcomplexringadjacencies(plex, vtx1, vtx2, vtx3, adjacencies[2])        */
/*    Search the star of (vtx1, vtx2) to try to find the tetrahedra          */
/*    adjoining the triangle (vtx1, vtx2, vtx3).                             */
/*  tetcomplexiterateall(pos, nexttet[4])   Return the next tetrahedron,     */
/*    with duplicate copies included.  Certain ghosts are included.          */
/*  tetcomplexdeleteorderedtet(plex, vtx1, vtx2, vtx3, vtx4)   Delete an     */
/*    ordered tetrahedron.                                                   */
/*  tetcomplexinsertorderedtet(plex, vtx1, vtx2, vtx3, vtx4)   Insert an     */
/*    ordered tetrahedron.                                                   */
/*  tetcomplex12fliponedge(plex, vtx1, vtx2, vtx3, vtx4, newvertex)          */
/*    Replace (vtx1, vtx2, vtx3, vtx4) with (vtx1, vtx2, vtx3, newvertex)    */
/*    and (vtx1, vtx2, newvertex, vtx4) in the star of (vtx1, vtx2).         */
/*  tetcomplex21fliponedge(plex, vtx1, vtx2, vtx3)   Replace (vtx1,          */
/*    vtx2, ?, vtx3) and (vtx1, vtx2, vtx3, ??) with (vtx1, vtx2, ?, ??) in  */
/*    the star of (vtx1, vtx2).                                              */
/*                                                                           */
/*  Specialized for the Bowyer-Watson algorithm:                             */
/*  tetcomplexinserttetontripod(plex, vtx1, vtx2, vtx3, vtx4)   Insert a     */
/*    tetrahedron into the stars of 3 edges that meet at `vtx1'.             */
/*  tetcomplexdeletetriangle(plex, vtx1, vtx2, vtx3)   Delete a triangle,    */
/*    gluing two tetrahedra into a polyhedron.                               */
/*  tetcomplexsqueezetriangle(plex, vtx1, vtx2, vtx3)   Delete a triangle    */
/*    that has the new vertex on both sides of it.                           */
/*  tetcomplexbuild3dstar(plex, newvertex, trianglelist, firstindex,         */
/*    trianglecount)   Convert a 2D link triangulation made up of `struct    */
/*    tetcomplexlinktriangle' triangles to the native data structure.        */
/*                                                                           */
/*  For internal use only:                                                   */
/*  tetcomplexlookup3dstar(plex, vtx)                                        */
/*  tetcomplexmissingtet(plex, vtx1, vtx2, vtx3, vtx4)                       */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  Ghost tetrahedra                                                         */
/*                                                                           */
/*  A ghost tetrahedron is a tetrahedron that has the GHOSTVERTEX for one of */
/*  its vertices.  A solid tetrahedron is a tetrahedron whose vertices are   */
/*  all not GHOSTVERTEX.  Except in one special case, tetrahedra with two    */
/*  ghost vertices are prohibited.                                           */
/*                                                                           */
/*  Ghost tetrahedra are implicitly returned by the face adjacency query.    */
/*  For example, if you query the identity of the two tetrahedra adjoining   */
/*  the triangle (1, 2, 3), and the tetrahedron (1, 2, 3, 4) is the sole     */
/*  tetrahedron in the entire complex, the query will return GHOSTVERTEX and */
/*  4, indicating that the triangle adjoins the tetrahedra (1, 2, 3, 4) and  */
/*  (1, 3, 2, GHOSTVERTEX).  However, this does not mean that the latter     */
/*  tetrahedron was ever inserted into the tetcomplex.                       */
/*                                                                           */
/*  However, it is permissible to insert ghost tetrahedra into a tetcomplex, */
/*  and they will (usually) be stored therein, if they obey the same rules   */
/*  that solid tetrahedra obey about sharing faces.  Why would you want to   */
/*  do this?  Because they make it easier to navigate along the boundary of  */
/*  a complex.  This takes a bit of explanation...                           */
/*                                                                           */
/*  A ghost triangle is a triangle that has the GHOSTVERTEX for one of its   */
/*  vertices.  Suppose you know that the edge (1, 2) is on the boundary of   */
/*  the mesh, and you want to find the identity of the boundary triangles    */
/*  that have it for an edge.  Suppose these triangles are (1, 2, 3) and     */
/*  (2, 1, 4).  Because they are boundary triangles, you might think that    */
/*  you could query the identity of the tetrahedra that adjoin the ghost     */
/*  triangle (1, 2, GHOSTVERTEX).  Ideally, the query would return the       */
/*  tetrahedra (1, 2, GHOSTVERTEX, 3) and (1, GHOSTVERTEX, 2, 4), thereby    */
/*  answering the question of which boundary triangles adjoin (1, 2).        */
/*                                                                           */
/*  One of the disadvantages of the space-efficient data structure is that   */
/*  this query doesn't always find the desired ghost tetrahedra.  (Sometimes */
/*  it does; sometimes it doesn't.)  However, if the ghost tetrahedra are    */
/*  _explicitly_ inserted into the tetcomplex (at the cost of taking up      */
/*  extra memory), adjacency queries on ghost triangles are answered         */
/*  reliably.  For this to work, you must insert one ghost tetrahedron for   */
/*  every triangular face on the boundary of the tetrahedralization.  This   */
/*  makes it possible, for example, to efficiently "walk" along the boundary */
/*  of the tetrahedralization.                                               */
/*                                                                           */
/*  There are some oddities to explicitly stored ghost tetrahedra, though.   */
/*  First, they are independent of the solid tetrahedra stored in the        */
/*  tetcomplex.  The tetcomplex won't notice if the solid tetrahedra and     */
/*  ghost tetrahedra are completely inconsistent with each other.  It's the  */
/*  caller's responsibility to keep them consistent, because the tetcomplex  */
/*  doesn't check.                                                           */
/*                                                                           */
/*  All adjacency queries on solid triangles are answered using the solid    */
/*  tetrahedra; stored ghost tetrahedra have no influence on these queries.  */
/*  Adjacency queries on ghost triangles are answered using a combination    */
/*  of ghost and solid tetrahedra.  If the ghost tetrahedra are not          */
/*  consistent with the solid tetrahedra, these queries may be answered      */
/*  inconsistently.                                                          */
/*                                                                           */
/*  Second, imagine that two solid tetrahedra (1, 2, 3, 4) and (1, 2, 5, 6)  */
/*  share a boundary edges (1, 2), but no other tetrahedra do.  So there are */
/*  actually four boundary triangles sharing the edge.  (The boundary of the */
/*  tetrahedral complex is nonmanifold.)  If you try to explicitly store a   */
/*  ghost tetrahedron for each of these boundary faces, then you have four   */
/*  ghost tetrahedra sharing the face (1, 2, GHOSTVERTEX).  This is not      */
/*  allowed; only two ghost tetrahedra may share a ghost triangle.  So       */
/*  explicitly stored ghost tetrahedra only really make sense for a          */
/*  tetrahedralization covering a (nondegenerate) convex polyhedron.  They   */
/*  don't make sense for a general-purpose mesh.                             */
/*                                                                           */
/*  Third, it isn't really possible to determine whether a tetcomplex        */
/*  contains a particular ghost tetrahedron or not.  For example, if a solid */
/*  tetrahedron (1, 2, 3, 4) is present, then queries on the faces (1, 2, 3) */
/*  or (1, 2, GHOSTVERTEX) might both indicate the presence of a tetrahedron */
/*  (1, 3, 2, GHOSTVERTEX), but that doesn't tell you whether that           */
/*  tetrahedron is explicitly stored or not.                                 */
/*                                                                           */
/*  Fourth, sometimes the insertion (or deletion) of a ghost tetrahedron     */
/*  doesn't actually change the data structure at all--except for the count  */
/*  of ghost tetrahedra.  This is because of the unusual nature of the       */
/*  space-efficient tetrahedral complex data structure:  it might not store  */
/*  the link of any of the tetrahedron's edges adjoining the GHOSTVERTEX.    */
/*  With these tetrahedra, an application might be able to corrupt the count */
/*  of ghost tetrahedra by inserting (or deleting) the same ghost            */
/*  tetrahedron twice.                                                       */
/*                                                                           */
/*  Despite all these quirks, explicitly stored ghost tetrahedra can be very */
/*  useful.  My implementation of the Bowyer-Watson algorithm for            */
/*  incremental vertex insertion in Delaunay triangulations uses them        */
/*  because it simplifies the algorithm in several ways:  by making it       */
/*  possible to treat inserting a vertex outside the convex hull much the    */
/*  same as inserting a vertex inside the tetrahedralization, and by making  */
/*  it easy to walk along the boundary of the tetrahedralization (which is   */
/*  important when a vertex is inserted outside the convex hull).            */
/*  Nevertheless, it's simpler to use the tetcomplex data structure if you   */
/*  don't explicitly store ghost tetrahedra.                                 */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  The memory-efficient data structure                                      */
/*  (Blandford, Blelloch, Cardoze, and Kadow)                                */
/*                                                                           */
/*  The tetcomplex structure represents a tetrahedral complex as a set of    */
/*  _stars_.  The star of a vertex is the set of simplices that adjoin the   */
/*  vertex.  The star of an edge is the set of simplices that have the edge  */
/*  for a face (i.e. simplices that share both of the edge's vertices).      */
/*                                                                           */
/*  The _link_ of a vertex is the set of simplices that don't adjoin the     */
/*  vertex, but are faces of simplices in the vertex's star.  For example,   */
/*  if vertex 1 adjoins just two tetrahedra, (1, 2, 3, 4) and (1, 3, 2, 5),  */
/*  then its link is the set { (2), (3), (4), (5), (2, 3), (2, 4), (3, 4),   */
/*  (2, 5), (3, 5), (2, 3, 4), (3, 2, 5) }.  Because the tetcomplex data     */
/*  structure does not support edges or triangles that aren't faces of       */
/*  tetrahedra, you can specify the link of a vertex simply by specifying    */
/*  the triangles--in this case, (2, 3, 4) and (3, 2, 5).                    */
/*                                                                           */
/*  The link of an edge is the set of simplices that don't adjoin either of  */
/*  the edge's vertices, but are faces of simplices in the edge's star.  In  */
/*  the previous example, the link of (1, 2) is { (3), (4), (5), (3, 4),     */
/*  (3, 5) }.  You can specify the link of an edge simply by specifying the  */
/*  edges--in this case, (3, 4) and (3, 5).  The link of an edge is either a */
/*  ring of edges, or a (possibly empty) sequence of chains of edges.        */
/*                                                                           */
/*  A data structure that represents the link of a vertex implicitly         */
/*  represents the star of the vertex as well, and vice versa.  (The star is */
/*  three-dimensional, because it includes tetrahedra, and the link is two-  */
/*  dimensional, because its highest-dimensional simplices are triangles.)   */
/*  Likewise, an edge's link implies its star, and vice versa.  (The star is */
/*  three-dimensional, but the link is only one-dimensional, because its     */
/*  highest-dimensional simplices are edges.)                                */
/*                                                                           */
/*  The tetcomplex data structure uses a `tetcomplexstar' to represent the   */
/*  star of a vertex.  The tetcomplexstar references (using a tag) a link2d  */
/*  data structure, which simultaneously represents the 2D link and the 3D   */
/*  star.  Each link2d contains a list of edges and their stars, represented */
/*  by the linkring data structure, which simultaneously represents the 1D   */
/*  link and the 3D star.  (Both the link2d and linkring data structures are */
/*  described in separate headers like this one.)  The link2d and linkring   */
/*  data structures store tags in a compressed form, helping to make the     */
/*  tetcomplex data structure occupy less space.                             */
/*                                                                           */
/*  However, that's not the only trick Blandford et al. use to save space.   */
/*  At first sight, the star representation of a tetrahedral complex might   */
/*  seem to store a lot of redundant information, because each tetrahedron   */
/*  is in the star of four different vertices; and within one vertex's star, */
/*  each tetrahedron is in the star of three different edges.  So is each    */
/*  tetrahedron stored twelve times?                                         */
/*                                                                           */
/*  No.  On average, each tetrahedron is stored just three times, because    */
/*  of the following two optimizations of Blandford et al.                   */
/*                                                                           */
/*  (1)  If a tetcomplex contains an edge (a, b), the link ring for that     */
/*       edge is stored only if a and b have the same parity--in other       */
/*       words, if both are odd or both are even.  (If a and b have opposite */
/*       parity, the edge's link must be deduced by looking at other edges'  */
/*       links.)  This cuts the memory occupied by link rings in half.       */
/*       Note that the GHOSTVERTEX has odd parity.                           */
/*                                                                           */
/*  (2)  If a tetcomplex contains an edge (a, b), where a and b have the     */
/*       same parity, the link ring for edge (a, b) is stored in one         */
/*       vertex's star--either the star for a or the star for b, but not     */
/*       both.  If one of the vertices is the GHOSTVERTEX, then the edge's   */
/*       link ring is stored in the other vertex's star.  (The GHOSTVERTEX   */
/*       does not get a star.)  Otherwise, if both vertices have the same    */
/*       two's bit, the edge's link ring is stored in the star of the vertex */
/*       with the greater index.  If the vertices have opposite two's bits,  */
/*       the edge's link is stored with the vertex with the lesser index.    */
/*       This cuts the memory occupied by link rings in half.                */
/*                                                                           */
/*       Why such a complicated rule?  I could have simply stored every      */
/*       edge's link with the vertex with the lesser index (which is what    */
/*       Blandford et al. do).  But then vertices with small indices would   */
/*       tend to have a lot of link rings stored in their stars, and         */
/*       vertices with large indices would tend to have few or none.  But    */
/*       the overall amount of time a program spends searching through a     */
/*       vertex's star is probably quadratic in the number of link rings it  */
/*       stores, so it's better to distribute link rings among vertices in a */
/*       pseudorandom way.  Moreover, fewer vertices will have empty stars,  */
/*       so the operation of finding some tetrahedron that adjoins a vertex  */
/*       is usually faster.                                                  */
/*                                                                           */
/*  Even with these optimizations, the tetcomplex data structure supports    */
/*  fast triangle queries.  Suppose you query the identity of the two        */
/*  tetrahedra adjoining the triangle (a, b, c).  At least two of those      */
/*  three vertices must have the same parity.  Suppose a and c have the same */
/*  parity.  The query is answered by a search through the link ring of the  */
/*  edge (a, c), which is stored either in a's star or in c's star.          */
/*                                                                           */
/*  The disadvantage of these optimizations is that it is not always         */
/*  efficient to answer the queries "find a tetrahedron that adjoins vertex  */
/*  a" or "find a tetrahedron that has the edge (a, b)".  In the worst case, */
/*  these queries might require an exhaustive search through the entire      */
/*  tetcomplex.  Therefore, these queries should be used as little as        */
/*  possible.  Fortunately, incremental Delaunay triangulation and many      */
/*  other algorithms can be implemented without these queries.               */
/*                                                                           */
/*  Ghost tetrahedra inserted into a tetcomplex are stored in the link rings */
/*  of ghost edges.  Inserting or deleting a ghost tetrahedron does not      */
/*  modify any solid edges' link rings.  However, triangle queries on ghost  */
/*  triangles sometimes use ghost edges' link rings, and sometimes solid     */
/*  edges' link rings, to answer those queries.  For example, a query on the */
/*  triangle (2, 4, GHOSTVERTEX) will consult the edge (2, 4), because the   */
/*  GHOSTVERTEX has odd parity, and no link ring is stored for               */
/*  (2, GHOSTVERTEX) or (4, GHOSTVERTEX).  That's why these queries may be   */
/*  answered incorrectly if the ghost tetrahedra are not consistent with the */
/*  solid tetrahedra.                                                        */
/*                                                                           */
/*  Solid tetrahedra inserted into a tetcomplex are stored in the link rings */
/*  of solid edges.  Inserting or deleting a solid tetrahedron does not      */
/*  modify any ghost edges' link rings.  Triangle queries on solid triangles */
/*  never consult ghost edges' link rings.                                   */
/*                                                                           */
/*****************************************************************************/


/*  A tetcomplex is a tetrahedral complex data structure.  It is composed of */
/*  a set of 3D stars, one for each vertex.  These stars have type `struct   */
/*  tetcomplexstar', and are stored (referenced by vertex tag) through the   */
/*  array `stars'.  The molecules that the stars are composed of are stored  */
/*  in the memory pool `moleculepool'.  All of the stars in `stars' having   */
/*  index less than `nextinitindex' have been initialized, and all the stars */
/*  having index `nextinitindex' or greater have not.                        */
/*                                                                           */
/*  Each tetcomplex has a pointer `vertexpool' to the memory pool in which   */
/*  the complex's vertices are stored.  One set of vertices can be shared by */
/*  any number of tetcomplexes.  Recall that no tetcomplex procedure ever    */
/*  looks at the coordinates of a vertex.  So what is `vertexpool' for?      */
/*  First, it is used to look up allocation indices; the molecules that make */
/*  up a vertex's star are allocated using the same allocation index that    */
/*  was used to allocate the vertex itself.  Second, the procedures          */
/*  `tetcomplextag2vertex' and `tetcomplextag2attributes' are provided to    */
/*  clients, so that clients can look up vertices without having to remember */
/*  which vertices go with which tetcomplex (and perhaps getting it wrong!). */
/*                                                                           */
/*  `tetcount' is the number of solid tetrahedra in the complex, and         */
/*  `ghosttetcount' is the number of ghost tetrahedra.  These counts are not */
/*  guaranteed to be preserved correctly by all operations.  In particular,  */
/*  if the stars become mutually inconsistent during star splaying, these    */
/*  counts will become wrong.  (When the stars are inconsistent with each    */
/*  other, you can't meaningfully say how many tetrahedra there are.)  Also, */
/*  sometimes inserting or removing a ghost tetrahedron doesn't change the   */
/*  data structure at all (except the count of ghost tetrahedra), so         */
/*  `ghosttetcount' is maintained primarily by counting the tetrahedra       */
/*  inserted or removed by the calling application, and can be fooled if the */
/*  application adds or removes a ghost tetrahedron whose absence or         */
/*  presence the tetcomplex cannot verify.                                   */
/*                                                                           */
/*  `consistentflag' indicates whether the stars are (believed to be)        */
/*  mutually consistent.                                                     */
/*                                                                           */
/*  The number `verbosity' indicates how much debugging information to       */
/*  print, from none (0) to lots (4+).                                       */

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

/*  A tetcomplexstar addresses a vertex's 3D star, represented by a 2D link, */
/*  stored as a link2d data structure.  `linkhead' and `linktail' are tags   */
/*  referencing the first and last molecules, respectively, of the link2d    */
/*  linked list.  The tail is stored to make `link2dinsertvertex' fast.      */

struct tetcomplexstar {
  tag linkhead;                 /* First molecule of the link2d linked list. */
  tag linktail;                  /* Last molecule of the link2d linked list. */
};

/*  A tetcomplexposition represents a position in a tetrahedral complex, and */
/*  is used to iterate over all the tetrahedra in the complex.               */

struct tetcomplexposition {
  struct tetcomplex *mycomplex;          /* The complex this position is in. */
  tag starvertex;     /* The vertex whose star is currently being traversed. */
  struct link2dposition link2dposition;            /* That vertex's 2D link. */
  tag link2dvertex;                  /* Vertex in 2D link, denoting an edge. */
  struct linkposition link1dposition;           /* That edge's 1D link ring. */
  tag link1dfirstvertex;    /* First vertex in ring (needed at end of ring). */
  tag link1dprevvertex;                 /* Vertex visited on last iteration. */
};

/*  The tetcomplexlinktriangle struct is part of the interface of            */
/*  tetcomplexbuild3dstar().  The procedure takes as input a complete 2D     */
/*  link made up of tetcomplexlinktriangles linked together, and converts    */
/*  them into link rings and a 2D link.                                      */

struct tetcomplexlinktriangle {
  tag vtx[3];
  arraypoolulong neighbor[3];
};


/*****************************************************************************/
/*                                                                           */
/*  tetcomplexinit()   Initialize an empty tetrahedral complex.              */
/*                                                                           */
/*  Before a tetcomplex may be used, it must be initialized by this          */
/*  procedure.                                                               */
/*                                                                           */
/*  The optional `vertexpool' parameter has two purposes.  First, it allows  */
/*  the tetcomplex to look up the allocation indices of the vertices, and    */
/*  use the same allocation indices to allocate the data structures used to  */
/*  store the vertices' links.  Thus, if vertices that are geometrically     */
/*  close to each other tend to have the same allocation indices, then the   */
/*  same will be true for the link data structures.  Second, it enables the  */
/*  fast macros tetcomplextag2vertex() and tetcomplextag2attributes(), which */
/*  directly map a vertex tag to a vertex data structure.  These save the    */
/*  programmer from having to remember which vertexpool goes with the        */
/*  tetcomplex.                                                              */
/*                                                                           */
/*  If you are not storing vertex data structures in a proxipool (or if you  */
/*  don't have vertex data structures at all), set `vertexpool' to NULL.     */
/*  This will cause all link data structures to be allocated from a common   */
/*  pool, thereby sacrificing some of the memory locality and compression    */
/*  of the data structure.  It will also disable the macros (as they will    */
/*  dereference a NULL pointer).                                             */
/*                                                                           */
/*  Don't call this procedure on a tetcomplex that has already been          */
/*  initialized (by this procedure), unless you call tetcomplexdeinit() on   */
/*  it first.  You will leak memory.  Also see tetcomplexrestart().          */
/*                                                                           */
/*  plex:  The tetcomplex to initialize.                                     */
/*  vertexpool:  The proxipool of vertices associated with this tetcomplex.  */
/*    The tags stored in the tetcomplex reference these vertices.  This      */
/*    parameter may be set to NULL, but if it is, you cannot subsequently    */
/*    use tetcomplextag2vertex() or tetcomplextag2attributes().              */
/*  verbose:  How much debugging information tetcomplex procedures should    */
/*    print, from none (0) to lots (4+).                                     */
/*                                                                           */
/*****************************************************************************/

void tetcomplexinit(struct tetcomplex *plex,
                    struct proxipool *vertexpool,
                    int verbose)
{
  /* Initialize a pool to store the molecules in the 1D and 2D links. */
  proxipoolinit(&plex->moleculepool, MOLECULESIZE, 0, verbose);
  /* Initialize a cache for fast 2D link lookups. */
  link2dcacheinit(plex->cache);
  /* Initialize a pool to store the 3D stars. */
  arraypoolinit(&plex->stars, sizeof(struct tetcomplexstar), 10, 0);
  /* Remember the vertices in the tetcomplex. */
  plex->vertexpool = vertexpool;
  /* No tag has a star yet. */
  plex->nextinitindex = 0;
  /* No tetrahedra yet. */
  plex->tetcount = 0;
  plex->ghosttetcount = 0;
  plex->consistentflag = 1;
  plex->verbosity = verbose;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexrestart()   Empty a tetrahedral complex.                       */
/*                                                                           */
/*  The complex is returned to its starting state, except that no memory is  */
/*  freed to the operating system.  Rather, the previously allocated         */
/*  proxipool (of molecules) and arraypool (of stars) are ready to be        */
/*  reused.                                                                  */
/*                                                                           */
/*  plex:  The tetcomplex to restart.                                        */
/*                                                                           */
/*****************************************************************************/

void tetcomplexrestart(struct tetcomplex *plex)
{
  /* Restart the memory pools. */
  proxipoolrestart(&plex->moleculepool);
  arraypoolrestart(&plex->stars);
  /* Empty the cache for fast 2D link lookups. */
  link2dcacheinit(plex->cache);
  /* No tag has a star yet. */
  plex->nextinitindex = 0;
  /* No tetrahedra yet. */
  plex->tetcount = 0;
  plex->ghosttetcount = 0;
  plex->consistentflag = 1;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexdeinit()   Free to the operating system all memory taken by a  */
/*                       tetcomplex.                                         */
/*                                                                           */
/*  plex:  The tetcomplex to free.                                           */
/*                                                                           */
/*****************************************************************************/

void tetcomplexdeinit(struct tetcomplex *plex)
{
  proxipooldeinit(&plex->moleculepool);
  arraypooldeinit(&plex->stars);
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplextag2vertex()   Map a tag to the vertex it indexes.             */
/*                                                                           */
/*  This procedure is much like proxipooltag2object(), but it saves callers  */
/*  from needing to remember which proxipool holds the vertices associated   */
/*  with a tetrahedral complex (and possibly getting it wrong).              */
/*                                                                           */
/*  WARNING:  fails catastrophically, probably with a segmentation fault, if */
/*  the tag's minipool doesn't yet exist.                                    */
/*                                                                           */
/*  Implemented as a macro to meet your need for speed.                      */
/*                                                                           */
/*  plex:  The tetcomplex in which to find a vertex.                         */
/*         Type (struct tetcomplex *).                                       */
/*  searchtag:  The tag whose vertex is sought.                              */
/*              Type (tag).                                                  */
/*                                                                           */
/*  Returns a pointer to the vertex.  Might crash if the tag has not been    */
/*    allocated yet.                                                         */
/*    Type (void *).                                                         */
/*                                                                           */
/*****************************************************************************/

#define tetcomplextag2vertex(plex, searchtag)  \
  proxipooltag2object(plex->vertexpool, searchtag)

/*****************************************************************************/
/*                                                                           */
/*  tetcomplextag2attributes()   Map a tag to the vertex attributes (stored  */
/*                               in a supplementary object) it indexes.      */
/*                                                                           */
/*  This procedure is much like proxipooltag2object2(), but it saves callers */
/*  from needing to remember which proxipool holds the vertices associated   */
/*  with a tetrahedral complex (and possibly getting it wrong).              */
/*                                                                           */
/*  WARNING:  fails catastrophically, probably with a segmentation fault, if */
/*  the tag's minipool doesn't yet exist.                                    */
/*                                                                           */
/*  Implemented as a macro to meet your need for speed.                      */
/*                                                                           */
/*  plex:  The tetcomplex in which to find a vertex's attributes.            */
/*         Type (struct tetcomplex *).                                       */
/*  searchtag:  The tag whose attributes (supplementary object) are sought.  */
/*              Type (tag).                                                  */
/*                                                                           */
/*  Returns a pointer to the vertex attributes.  Might crash if the tag has  */
/*    not been allocated yet.                                                */
/*    Type (void *).                                                         */
/*                                                                           */
/*****************************************************************************/

#define tetcomplextag2attributes(plex, searchtag)  \
  proxipooltag2object2(plex->vertexpool, searchtag)

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexringadjacencies()   Search the link of a specified edge for    */
/*                                the apex vertices of the two tetrahedra    */
/*                                having a specified triangle for a face.    */
/*                                                                           */
/*  This procedure is a helper procedure for tetcomplexadjacencies().  It    */
/*  searches for the tetrahedra adjoining triangle (vtx1, vtx2, vtx3), but   */
/*  it only looks in the star of the (undirected) edge (vtx1, vtx2), which   */
/*  is appropriate if `vtx1' and `vtx2' have the same parity.                */
/*                                                                           */
/*  Do not use this procedure if the stars of `vtx1' and `vtx2' might be     */
/*  inconsistent with each other and disagree about the star/link of the     */
/*  edge (vtx1, vtx2).                                                       */
/*                                                                           */
/*  plex:  The tetcomplex that contains the triangle.                        */
/*  vtx1, vtx2, vtx3:  The vertices of the query triangle.  (vtx1, vtx2) is  */
/*    the (only) edge whose link ring will be searched.                      */
/*  adjacencies[2]:  Array in which the procedure returns the tags of the    */
/*    apex vertices of the two tetrahedra adjoining the query triangle.      */
/*    The tetrahedra have orientations (vtx1, vtx2, vtx3, adjacencies[1])    */
/*    and (vtx1, vtx2, adjacencies[0], vtx3).  One or both elements of this  */
/*    arrary may be GHOSTVERTEX, if there are no such adjoining tetrahedra.  */
/*    The contents of this array do not need to be initialized prior to      */
/*    calling this procedure.                                                */
/*                                                                           */
/*  Returns 1 if the triangle is in the tetcomplex; 0 otherwise.             */
/*                                                                           */
/*****************************************************************************/

int tetcomplexringadjacencies(struct tetcomplex *plex,
                              tag vtx1,
                              tag vtx2,
                              tag vtx3,
                              tag adjacencies[2])
{
  struct tetcomplexstar *star;
  tag ring;
  tag swap;

  /* The link ring for the edge (vtx1, vtx2) is stored in the star for one   */
  /*   of those two vertices.  If one of the vertices is GHOSTVERTEX, it is  */
  /*   stored with the other vertex (GHOSTVERTEX never stores its star).     */
  /*   Otherwise, if both vertices have the same two's bit, the edge's link  */
  /*   ring is stored in the star of the vertex with the greater index.  If  */
  /*   the vertices have opposite two's bits, the edge's link is stored with */
  /*   the vertex with the lesser index.  See the tetcomplex header for an   */
  /*   explanation.                                                          */
  if ((vtx2 == GHOSTVERTEX) ||
      ((vtx1 != GHOSTVERTEX) &&
       ((vtx1 < vtx2) ^ ((vtx1 & 2) == (vtx2 & 2))))) {
    /* Does vertex `vtx1' even have a star yet? */
    if (vtx1 >= plex->nextinitindex) {
      /* No. */
      adjacencies[0] = STOP;
      adjacencies[1] = STOP;
      return 0;
    }
    /* Look up the star of `vtx1'. */
    star = (struct tetcomplexstar *)
           arraypoolfastlookup(&plex->stars, (arraypoolulong) vtx1);
    /* Search it for the link ring of (vtx1, vtx2). */
    ring = link2dfindring(&plex->moleculepool, plex->cache, star->linkhead,
                          vtx1, vtx2);
    /* Find the neighbors of `vtx3' in the link ring.  Note that if the edge */
    /*   (vtx1, vtx2) is not present, `ring' is STOP, but the following line */
    /*   will do the right thing (return 0) anyway.                          */
    return linkringadjacencies(&plex->moleculepool, ring, vtx1, vtx3,
                               adjacencies);
  } else {
    /* Does vertex `vtx2' even have a star yet? */
    if (vtx2 >= plex->nextinitindex) {
      /* No. */
      adjacencies[0] = STOP;
      adjacencies[1] = STOP;
      return 0;
    }
    /* Look up the star of `vtx2'. */
    star = (struct tetcomplexstar *)
           arraypoolfastlookup(&plex->stars, (arraypoolulong) vtx2);
    /* Search it for the link ring of (vtx2, vtx1). */
    ring = link2dfindring(&plex->moleculepool, plex->cache, star->linkhead,
                          vtx2, vtx1);
    /* Find the neighbors of `vtx3' in the link ring. */
    if (linkringadjacencies(&plex->moleculepool, ring, vtx2, vtx3,
                            adjacencies)) {
      /* The orientation of the edge and the link ring is opposite to the */
      /*   orientation of the query, so swap the tetrahedron apices to    */
      /*   compensate.                                                    */
      swap = adjacencies[0];
      adjacencies[0] = adjacencies[1];
      adjacencies[1] = swap;
      return 1;
    }
    return 0;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexadjacencies()   Find the apex vertices of the two tetrahedra   */
/*                            having a specified triangle for a face.  Known */
/*                            as a "triangle query" for short.               */
/*                                                                           */
/*  This procedure only works correctly if all the vertex and edge stars     */
/*  (i.e. link2d and linkring data structures) in the tetcomplex are         */
/*  consistent with each other.                                              */
/*                                                                           */
/*  plex:  The tetcomplex that contains the triangle.                        */
/*  vtx1, vtx2, vtx3:  The vertices of the query triangle.                   */
/*  adjacencies[2]:  Array in which the procedure returns the tags of the    */
/*    apex vertices of the two tetrahedra adjoining the query triangle.      */
/*    The tetrahedra have orientations (vtx1, vtx2, vtx3, adjacencies[1])    */
/*    and (vtx1, vtx2, adjacencies[0], vtx3).  One or both elements of this  */
/*    array may be GHOSTVERTEX, if there are no such adjoining tetrahedra.   */
/*    The contents of this array do not need to be initialized prior to      */
/*    calling this procedure.                                                */
/*                                                                           */
/*  Returns 1 if the triangle is in the tetcomplex; 0 otherwise.             */
/*                                                                           */
/*****************************************************************************/

int tetcomplexadjacencies(struct tetcomplex *plex,
                          tag vtx1,
                          tag vtx2,
                          tag vtx3,
                          tag adjacencies[2])
{
  /* Reject a query triangle with two identical vertices. */
  if ((vtx1 == vtx2) || (vtx2 == vtx3) || (vtx3 == vtx1)) {
    return 0;
  }

  /* Do `vtx1' and `vtx2' have the same parity? */
  if ((vtx1 & 1) == (vtx2 & 1)) {
    /* Yes; check the link ring of edge (vtx1, vtx2). */
    return tetcomplexringadjacencies(plex, vtx1, vtx2, vtx3, adjacencies);
  } else if ((vtx2 & 1) == (vtx3 & 1)) {
    /* Check the link ring of edge (vtx2, vtx3). */
    return tetcomplexringadjacencies(plex, vtx2, vtx3, vtx1, adjacencies);
  } else {
    /* `vtx3' and `vtx1' must have the same parity. */
    return tetcomplexringadjacencies(plex, vtx3, vtx1, vtx2, adjacencies);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexedge2tet()   Find one tetrahedron adjoining a specified edge.  */
/*                         Known as an "edge query" for short.               */
/*                                                                           */
/*  Never returns a ghost tetrahedron.  If there is no solid tetrahedron     */
/*  adjoining the query vertex, this procedure returns 0, and all four       */
/*  entries of adjtet are set to STOP.                                       */
/*                                                                           */
/*  This procedure only works correctly if all the vertex and edge stars     */
/*  (i.e. link2d and linkring data structures) in the tetcomplex are         */
/*  consistent with each other.                                              */
/*                                                                           */
/*  plex:  The tetcomplex that contains the edge.                            */
/*  vtx1, vtx2:  The vertices of the query edge.                             */
/*  adjtet[4]:  Array in which the procedure returns the tags of the         */
/*    vertices of a tetrahedron adjoining the query vertex.  If there is no  */
/*    such tetrahedron, all four vertices will be GHOSTVERTEX.  The contents */
/*    of this array do not need to be initialized prior to calling this      */
/*    procedure.                                                             */
/*                                                                           */
/*  Returns 1 if the tetcomplex contains a solid tetrahedron that has the    */
/*  query edge; 0 otherwise.                                                 */
/*                                                                           */
/*****************************************************************************/

int tetcomplexedge2tet(struct tetcomplex *plex,
                       tag vtx1,
                       tag vtx2,
                       tag adjtet[4])
{
  struct link2dposition pos2d;
  struct linkposition pos1d;
  struct tetcomplexstar *star;
  tag vertexandlink[2];
  tag endpt1, endpt2, firstendpt;
  tag vtxloop;

  /* If a tetrahedron is found, it will be returned with the query vertices */
  /*   listed first.                                                        */
  adjtet[0] = vtx1;
  adjtet[1] = vtx2;

  /* Loop through the stars of all the vertices, searching for a tetrahedron */
  /*   with edge (vtx1, vtx2).  Search the star of `vtx1' first, then the    */
  /*   star of `vtx2', then all the other stars starting from the star after */
  /*   `vtx2' (wrapping around from the highest numbered star to zero).      */
  vtxloop = STOP;
  while (1) {
    /* The following code advances `vtxloop' to the next star.  Because   */
    /*   `vtx1' and `vtx2' might not have been allocated stars, a loop is */
    /*   needed here to skip indices that haven't been allocated yet.     */
    do {
      if (vtxloop == STOP) {
        /* Search the star of `vtx1' first. */
        vtxloop = vtx1;
      } else if (vtxloop == vtx1) {
        /* Search the star of `vtx2' second. */
        vtxloop = vtx2;
      } else {
        /* Advance to the next star, wrapping around if necessary. */
        if (vtxloop >= plex->nextinitindex) {
          vtxloop = 0;
        } else {
          vtxloop++;
        }
        /* If we reach `vtx1', advance again so we don't do `vtx1' twice. */
        if (vtxloop == vtx1) {
          vtxloop++;
        }
        /* If we reach `vtx2', we've gone through the whole list. */
        if ((vtxloop == vtx2) ||
            ((vtxloop >= plex->nextinitindex) &&
             (vtx2 >= plex->nextinitindex))) {
          /* No tetrahedron adjoins (vtx1, vtx2). */
          adjtet[0] = STOP;
          adjtet[1] = STOP;
          adjtet[2] = STOP;
          adjtet[3] = STOP;
          return 0;
        }
      }
      /* If `vtxloop' is not an allocated index, loop. */
    } while (vtxloop >= plex->nextinitindex);

    /* Look up the star of `vtxloop'. */
    star = (struct tetcomplexstar *)
           arraypoolfastlookup(&plex->stars, (arraypoolulong) vtxloop);

    /* Loop through the vertices in the 2D link of vertex `vtxloop'.  Skip  */
    /*   ghost vertices.  We could search every link ring in the whole      */
    /*   tetcomplex, but that's unnecessarily slow.  If there exists a      */
    /*   tetrahedron with edge (vtx1, vtx2), at least one of the two        */
    /*   vertices has the same parity as some other vertex of the           */
    /*   tetrahedron.  Therefore, if we only look at the link rings of the  */
    /*   edges that adjoin at least one of `vtx1' or `vtx2', we'll find the */
    /*   tetrahedron.                                                       */
    link2diteratorinit(&plex->moleculepool, star->linkhead, vtxloop, &pos2d);
    link2diterate(&pos2d, vertexandlink);
    while (vertexandlink[0] != STOP) {
      /* Skip ghost vertices and edges not adjoining `vtx1' nor `vtx2'. */
      if ((vertexandlink[0] != GHOSTVERTEX) &&
          ((vtxloop == vtx1) || (vtxloop == vtx2) ||
           (vertexandlink[0] == vtx1) || (vertexandlink[0] == vtx2))) {
        /* Find the first tetrahedron in the star of edge */
        /*   (vtxloop, vertexandlink[0]).                 */
        linkringiteratorinit(&plex->moleculepool, vertexandlink[1], vtxloop,
                             &pos1d);
        endpt1 = linkringiterate(&pos1d);
        endpt2 = linkringiterate(&pos1d);
        /* `endpt1' comes around again at the end of the ring, if it's not */
        /*   a ghost vertex.                                               */
        firstendpt = (endpt1 == GHOSTVERTEX) ? STOP : endpt1;

        /* Loop through the tetrahedra in the star of edge */
        /*   (vtxloop, vertexandlink[0]).                  */
        do {
          /* If this is a ghost tetrahedron, don't check for the edge. */
          if ((endpt1 != GHOSTVERTEX) && (endpt2 != GHOSTVERTEX)) {
            /* Not a ghost tetrahedron.  Check for the edge (vtx1, vtx2). */
            if (vtxloop == vtx1) {
              if (vertexandlink[0] == vtx2) {
                adjtet[2] = endpt1;
                adjtet[3] = endpt2;
                return 1;
              } else if (endpt1 == vtx2) {
                adjtet[2] = endpt2;
                adjtet[3] = vertexandlink[0];
                return 1;
              } else if (endpt2 == vtx2) {
                adjtet[2] = vertexandlink[0];
                adjtet[3] = endpt1;
                return 1;
              }
            } else if (vtxloop == vtx2) {
              if (vertexandlink[0] == vtx1) {
                adjtet[2] = endpt2;
                adjtet[3] = endpt1;
                return 1;
              } else if (endpt1 == vtx1) {
                adjtet[2] = vertexandlink[0];
                adjtet[3] = endpt2;
                return 1;
              } else if (endpt2 == vtx1) {
                adjtet[2] = endpt1;
                adjtet[3] = vertexandlink[0];
                return 1;
              }
            } else if (vertexandlink[0] == vtx1) {
              if (endpt1 == vtx2) {
                adjtet[2] = vtxloop;
                adjtet[3] = endpt2;
                return 1;
              } else if (endpt2 == vtx2) {
                adjtet[2] = endpt1;
                adjtet[3] = vtxloop;
                return 1;
              }
            } else if (vertexandlink[0] == vtx2) {
              if (endpt1 == vtx1) {
                adjtet[2] = endpt2;
                adjtet[3] = vtxloop;
                return 1;
              } else if (endpt2 == vtx1) {
                adjtet[2] = vtxloop;
                adjtet[3] = endpt1;
                return 1;
              }
            }
          }

          /* Iterate over the next tetrahedron in the star. */
          if (endpt2 == firstendpt) {
            /* We've wrapped around the end of the link ring, so we're done. */
            endpt2 = STOP;
          } else {
            endpt1 = endpt2;
            endpt2 = linkringiterate(&pos1d);
            /* If we've reached the end of the link ring, and the first      */
            /*   vertex in the ring was not a ghost vertex, consider the     */
            /*   tetrahedron having the first and last vertices in the ring. */
            if (endpt2 == STOP) {
              endpt2 = firstendpt;
            }
          }
        } while (endpt2 != STOP);
      }

      /* Iterate over the next vertex in the star of vertex `vtxloop'. */
      link2diterate(&pos2d, vertexandlink);
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexvertex2tet()   Find one tetrahedron adjoining a specified      */
/*                           vertex.  Known as a "vertex query" for short.   */
/*                                                                           */
/*  Never returns a ghost tetrahedron.  If there is no solid tetrahedron     */
/*  adjoining the query vertex, this procedure returns 0, and all four       */
/*  entries of adjtet are set to STOP.                                       */
/*                                                                           */
/*  plex:  The tetcomplex that contains the vertex.                          */
/*  vtx:  The query vertex.                                                  */
/*  adjtet[4]:  Array in which the procedure returns the tags of the         */
/*    vertices of a tetrahedron adjoining the query vertex.  If there is no  */
/*    such tetrahedron, all four vertices will be GHOSTVERTEX.  The contents */
/*    of this array do not need to be initialized prior to calling this      */
/*    procedure.                                                             */
/*                                                                           */
/*  Returns 1 if the tetcomplex contains a solid tetrahedron that adjoins    */
/*  the query vertex; 0 otherwise.                                           */
/*                                                                           */
/*****************************************************************************/

int tetcomplexvertex2tet(struct tetcomplex *plex,
                         tag vtx,
                         tag adjtet[4])
{
  struct link2dposition pos2d;
  struct linkposition pos1d;
  struct tetcomplexstar *star;
  tag vertexandlink[2];
  tag endpt1, endpt2, firstendpt;
  tag vtxloop;

  /* If a tetrahedron is found, it will be returned with the query vertex */
  /*   listed first.                                                      */
  adjtet[0] = vtx;

  /* Does vertex `vtx' even have a star yet? */
  if (vtx < plex->nextinitindex) {
    /* Maybe.  Start by searching its star. */
    vtxloop = vtx;
  } else {
    /* No.  Start searching the star of vertex 0. */
    vtxloop = 0;
  }

  /* Loop through the stars of all the vertices, searching for a tetrahedron */
  /*   adjoining vertex `vtx'.                                               */
  while (1) {
    /* Look up the star of `vtxloop'. */
    star = (struct tetcomplexstar *)
           arraypoolfastlookup(&plex->stars, (arraypoolulong) vtxloop);
    /* Loop through the vertices in the 2D link of vertex `vtxloop'. */
    link2diteratorinit(&plex->moleculepool, star->linkhead, vtxloop, &pos2d);
    link2diterate(&pos2d, vertexandlink);
    while (vertexandlink[0] != STOP) {
      /* Skip ghost vertices. */
      if (vertexandlink[0] != GHOSTVERTEX) {
        /* Find the first tetrahedron in the star of edge */
        /*   (vtxloop, vertexandlink[0]).                 */
        linkringiteratorinit(&plex->moleculepool, vertexandlink[1], vtxloop,
                             &pos1d);
        endpt1 = linkringiterate(&pos1d);
        endpt2 = linkringiterate(&pos1d);
        /* `endpt1' comes around again at the end of the ring, if it's not */
        /*   a ghost vertex.                                               */
        firstendpt = (endpt1 == GHOSTVERTEX) ? STOP : endpt1;

        /* Loop through the tetrahedra in the star of vertex `vtxloop'. */
        do {
          /* If this is a ghost tetrahedron, don't check for the vertex. */
          if ((endpt1 != GHOSTVERTEX) && (endpt2 != GHOSTVERTEX)) {
            /* Not a ghost tetrahedron.  Check for vertex `vtx'. */
            if (vtxloop == vtx) {
              adjtet[1] = vertexandlink[0];
              adjtet[2] = endpt1;
              adjtet[3] = endpt2;
              return 1;
            } else if (vertexandlink[0] == vtx) {
              adjtet[1] = vtxloop;
              adjtet[2] = endpt2;
              adjtet[3] = endpt1;
              return 1;
            } else if (endpt1 == vtx) {
              adjtet[1] = endpt2;
              adjtet[2] = vtxloop;
              adjtet[3] = vertexandlink[0];
              return 1;
            } else if (endpt2 == vtx) {
              adjtet[1] = endpt1;
              adjtet[2] = vertexandlink[0];
              adjtet[3] = vtxloop;
              return 1;
            }
          }

          /* Iterate over the next tetrahedron in the star. */
          if (endpt2 == firstendpt) {
            /* We've wrapped around the end of the link ring, so we're done. */
            endpt2 = STOP;
          } else {
            endpt1 = endpt2;
            endpt2 = linkringiterate(&pos1d);
            /* If we've reached the end of the link ring, and the first      */
            /*   vertex in the ring was not a ghost vertex, consider the     */
            /*   tetrahedron having the first and last vertices in the ring. */
            if (endpt2 == STOP) {
              endpt2 = firstendpt;
            }
          }
        } while (endpt2 != STOP);
      }

      /* Iterate over the next vertex in the star of vertex `vtxloop'. */
      link2diterate(&pos2d, vertexandlink);
    }

    /* Iterate over the next vertex star. */
    vtxloop++;
    /* If we reach `vtx', we've gone through the whole list. */
    if ((vtxloop == vtx) ||
        ((vtxloop >= plex->nextinitindex) && (vtx >= plex->nextinitindex))) {
      /* No tetrahedron adjoins `vtx'. */
      adjtet[0] = STOP;
      adjtet[1] = STOP;
      adjtet[2] = STOP;
      adjtet[3] = STOP;
      return 0;
    }
    if (vtxloop >= plex->nextinitindex) {
      /* Wrap around to the star of vertex 0. */
      vtxloop = 0;
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexiteratorinit()   Initialize an iterator that traverses all the */
/*                             tetrahedra in a tetcomplex one by one.        */
/*                                                                           */
/*  The iterator is a variable `pos', whose internals should not be          */
/*  examined or modified by the client.  The iterator's job is to keep       */
/*  track of where it is in the lists of tetrahedra.  This procedure sets    */
/*  the iterator so that the first call to tetcomplexiterateall() will find  */
/*  the first tetrahedron in the first link ring in the first nonempty       */
/*  link2d.                                                                  */
/*                                                                           */
/*  When a tetcomplex is modified, any iterators on that tetcomplex may be   */
/*  corrupted and should not be used without being initialized (by this      */
/*  procedure) again.                                                        */
/*                                                                           */
/*  plex:  The tetcomplex whose tetrahedra you want to iterate over.         */
/*  pos:  The iterator.  Its contents do not need to be initialized prior to */
/*    calling this procedure.                                                */
/*                                                                           */
/*****************************************************************************/

void tetcomplexiteratorinit(struct tetcomplex *plex,
                            struct tetcomplexposition *pos)
{
  pos->mycomplex = plex;
  /* Start with the star of the vertex having tag zero. */
  pos->starvertex = 0;
  /* The iterator is not in the middle of a link. */
  pos->link2dvertex = STOP;
  pos->link1dprevvertex = STOP;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexiterateall()   Advance a tetcomplexposition iterator to the    */
/*                           next tetrahedron and return it (with            */
/*                           duplicates).                                    */
/*                                                                           */
/*  This procedure returns every _copy_ of every tetrahedron in the data     */
/*  structure, so it exposes the underlying implementation, and the fact     */
/*  that each solid tetrahedron is stored in the link rings of two, three,   */
/*  or six edges (depending on the parities of the tetrahedron's vertices).  */
/*  In most cases, it's better to use tetcomplexiterate() or                 */
/*  tetcomplexiteratenoghosts() instead of this procedure, because they      */
/*  return each tetrahedron only once.  (They call this procedure and filter */
/*  out duplicate tetrahedra.)                                               */
/*                                                                           */
/*  However, when the stars are not consistent with each other (as in the    */
/*  star splaying algorithm), this procedure can be useful because it        */
/*  returns "ordered" tetrahedra.  Each tetrahedron returned is in the star  */
/*  of the vertex nexttet[0], and in the star of the edge (nexttet[0],       */
/*  nexttet[1]).                                                             */
/*                                                                           */
/*  This procedure also returns ghost tetrahedra.  Even if no ghost          */
/*  tetrahedron has been explicitly inserted into the complex, there will    */
/*  still be ghost tetrahedra returned for triangles that are faces of       */
/*  exactly one solid tetrahedron.  If ghost tetrahedra have been inserted,  */
/*  more copies of them will be returned.  Any vertex of the returned        */
/*  tetrahedron can be GHOSTVERTEX except the first one (nexttet[0]).        */
/*                                                                           */
/*  The iterator is a variable `pos', whose internals should not be          */
/*  examined or modified by the client.                                      */
/*                                                                           */
/*  When a tetcomplex is modified, any iterators on that tetcomplex may be   */
/*  corrupted and should not be passed to this procedure again until they    */
/*  are re-initialized.                                                      */
/*                                                                           */
/*  pos:  The iterator.                                                      */
/*  nexttet:  An array used to return the tags for the vertices of the       */
/*    next tetrahedron.  The tetrahedron is always returned with positive    */
/*    orientation.  If the iterator has enumerated all the tetrahedra in the */
/*    complex, all four vertices will be STOP on return.  Does not need to   */
/*    be initialized before the call.                                        */
/*                                                                           */
/*****************************************************************************/

void tetcomplexiterateall(struct tetcomplexposition *pos,
                          tag nexttet[4])
{
  struct tetcomplexstar *star;
  tag vertexandlink[2];
  int iteratorflag;

  if (pos->link1dprevvertex != STOP) {
    /* We're in the middle of traversing a link ring, so find the next edge */
    /*   in the ring.                                                       */
    nexttet[0] = pos->starvertex;
    nexttet[1] = pos->link2dvertex;
    /* The previous vertex is one endpoint of the edge. */
    nexttet[2] = pos->link1dprevvertex;
    /* The next vertex in the link ring is the other endpoint. */
    nexttet[3] = linkringiterate(&pos->link1dposition);
    pos->link1dprevvertex = nexttet[3];
    if (nexttet[3] == STOP) {
      /* We've reach the end of the list, so "close" the link ring by using */
      /*   the first vertex in the list as the last endpoint.               */
      nexttet[3] = pos->link1dfirstvertex;
    }
    return;
  }

  /* We need to find the next link ring to traverse.  Are there any vertex */
  /*   stars left to traverse?                                             */
  while (pos->starvertex < pos->mycomplex->nextinitindex) {
    /* Presume we'll find a vertex's 2D link until proven otherwise. */
    iteratorflag = 1;
    /* Are we in the middle of traversing a vertex's 2D link? */
    if (pos->link2dvertex == STOP) {
      /* No.  Find the next vertex star to traverse. */
      star = (struct tetcomplexstar *)
             arraypoolfastlookup(&pos->mycomplex->stars,
                                 (arraypoolulong) pos->starvertex);
      if (star->linkhead == STOP) {
        /* Vertex `pos->starvertex' doesn't have a star.  Try the */
        /*   next vertex.                                         */
        iteratorflag = 0;
        pos->starvertex++;
      } else {
        /* Prepare to iterate through the vertex's 2D link. */
        link2diteratorinit(&pos->mycomplex->moleculepool, star->linkhead,
                           pos->starvertex, &pos->link2dposition);
      }
    }

    /* Are we traversing a 2D link? */
    if (iteratorflag) {
      /* Yes.  Find the next vertex and link ring in the 2D link. */
      link2diterate(&pos->link2dposition, vertexandlink);
      pos->link2dvertex = vertexandlink[0];
      /* Have we reached the end of the list? */
      if (vertexandlink[0] != STOP) {
        /* No.  Return a tetrahedron adjoining this edge. */
        nexttet[0] = pos->starvertex;
        nexttet[1] = vertexandlink[0];
        /* Find the first edge in the new link ring. */
        linkringiteratorinit(&pos->mycomplex->moleculepool, vertexandlink[1],
                             pos->starvertex, &pos->link1dposition);
        nexttet[2] = linkringiterate(&pos->link1dposition);
        pos->link1dfirstvertex = nexttet[2];
        nexttet[3] = linkringiterate(&pos->link1dposition);
        pos->link1dprevvertex = nexttet[3];
        return;
      }
      pos->starvertex++;
    }
  }

  /* There are no more vertex stars to traverse or tetrahedra to return. */
  nexttet[0] = STOP;
  nexttet[1] = STOP;
  nexttet[2] = STOP;
  nexttet[3] = STOP;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexiterate()   Advance a tetcomplexposition iterator to a new     */
/*                        tetrahedron and return it (skipping duplicates).   */
/*                                                                           */
/*  This procedure returns every tetrahedron in the data structure once.     */
/*  It returns ghost tetrahedra as well as solid tetrahedra (even if no      */
/*  ghost tetrahedron has been explicitly inserted into the complex):  for   */
/*  each triangle that is a face of exactly one solid tetrahedron, a ghost   */
/*  tetrahedron adjoining that face is returned.                             */
/*                                                                           */
/*  Ghost tetrahedra explicitly inserted into the tetcomplex have no effect  */
/*  on the tetrahedra this procedure returns!  The ghost tetrahedra returned */
/*  depend solely on the solid tetrahedra.  (You will notice this only if    */
/*  the inserted ghost tetrahedra are inconsistent with the solid            */
/*  tetrahedra.)                                                             */
/*                                                                           */
/*  The first two vertices (nexttet[0] and nexttet[1]) of the returned       */
/*  tetrahedron are never GHOSTVERTEX.  A ghost tetrahedron has its ghost    */
/*  vertex in one of the last two places (nexttet[2] or nexttet[3]).         */
/*                                                                           */
/*  The iterator is a variable `pos', whose internals should not be          */
/*  examined or modified by the client.                                      */
/*                                                                           */
/*  When a tetcomplex is modified, any iterators on that tetcomplex may be   */
/*  corrupted and should not be passed to this procedure again until they    */
/*  are re-initialized.                                                      */
/*                                                                           */
/*  pos:  The iterator.                                                      */
/*  nexttet:  An array used to return the tags for the vertices of the       */
/*    next tetrahedron.  The tetrahedron is always returned with positive    */
/*    orientation.  If the iterator has enumerated all the tetrahedra in the */
/*    complex, all four vertices will be STOP on return.  Does not need to   */
/*    be initialized before the call.                                        */
/*                                                                           */
/*****************************************************************************/

void tetcomplexiterate(struct tetcomplexposition *pos,
                       tag nexttet[4])
{
  /* To ensure that no tetrahedron is returned twice (with the vertices in   */
  /*   a different order, we use the following rules.  (It's helpful to      */
  /*   remember that nexttet[0] and nexttet[1] always have the same parity.) */
  /*                                                                         */
  /* (1)  Do not return the tetrahedron if nexttet[1] is GHOSTVERTEX.  This  */
  /*      ensures that no tetrahedron in the star of a ghost edge is ever    */
  /*      returned; so ghost tetrahedra explicitly inserted into the         */
  /*      tetcomplex have no effect on the tetrahedra this procedure         */
  /*      returns.                                                           */
  /* (2)  Do not return the tetrahedron if nexttet[2] has the same parity as */
  /*      nexttet[0] and nexttet[1], and a smaller index than at least one   */
  /*      of those two.  (The tetrahedron is in at least three link rings,   */
  /*      one for each of the edges of the triangle nexttet[0, 1, 2], so     */
  /*      return it only when nexttet[2] has the largest index.)             */
  /* (3)  Same thing for nexttet[3] (replacing nexttet[2]).                  */
  /* (4)  Do not return the tetrahedron if nexttet[2] has the same parity as */
  /*      nexttet[3], and either nexttet[0] or nexttet[1] has the largest    */
  /*      index of all four tetrahedron vertices.  (The tetrahedron is in    */
  /*      the link rings of two edges, nexttet[0, 1] and nexttet[2, 3], and  */
  /*      the edge containing the vertex with the largest index defers to    */
  /*      the other edge.)                                                   */
  /*                                                                         */
  /* These rules ensure that only one copy of each tetrahedron is returned.  */
  do {
    tetcomplexiterateall(pos, nexttet);
  } while ((nexttet[1] == GHOSTVERTEX) ||
           (((nexttet[2] & 1) == (nexttet[0] & 1)) &&
            ((nexttet[2] < nexttet[0]) || (nexttet[2] < nexttet[1]))) ||
           (((nexttet[3] & 1) == (nexttet[0] & 1)) &&
            ((nexttet[3] < nexttet[0]) || (nexttet[3] < nexttet[1]))) ||
           (((nexttet[3] & 1) == (nexttet[2] & 1)) &&
            (((nexttet[2] < nexttet[0]) && (nexttet[3] < nexttet[0])) ||
             ((nexttet[2] < nexttet[1]) && (nexttet[3] < nexttet[1])))));
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexiteratenoghosts()   Advance a tetcomplexposition iterator to a */
/*                                new solid tetrahedron and return it        */
/*                                (skipping duplicates).                     */
/*                                                                           */
/*  This procedure returns every solid tetrahedron in the data structure     */
/*  once.  Ghost tetrahedra are skipped.                                     */
/*                                                                           */
/*  The iterator is a variable `pos', whose internals should not be          */
/*  examined or modified by the client.                                      */
/*                                                                           */
/*  When a tetcomplex is modified, any iterators on that tetcomplex may be   */
/*  corrupted and should not be passed to this procedure again until they    */
/*  are re-initialized.                                                      */
/*                                                                           */
/*  pos:  The iterator.                                                      */
/*  nexttet:  An array used to return the tags for the vertices of the       */
/*    next tetrahedron.  The tetrahedron is always returned with positive    */
/*    orientation.  If the iterator has enumerated all the tetrahedra in the */
/*    complex, all four vertices will be STOP on return.  Does not need to   */
/*    be initialized before the call.                                        */
/*                                                                           */
/*****************************************************************************/

void tetcomplexiteratenoghosts(struct tetcomplexposition *pos,
                               tag nexttet[4])
{
  /* This loop ensures that no ghost tetrahedron is returned (note that   */
  /*   nexttet[0] is never a GHOSTVERTEX), and no tetrahedron is returned */
  /*   twice.  For an explanation of how the latter is accomplished, see  */
  /*   the comments in tetcomplexiterate().  The first copy of each       */
  /*   tetrahedron is returned, and later copies are not.                 */
  do {
    tetcomplexiterateall(pos, nexttet);
  } while ((nexttet[1] == GHOSTVERTEX) || (nexttet[2] == GHOSTVERTEX) ||
           (nexttet[3] == GHOSTVERTEX) ||
           (((nexttet[2] & 1) == (nexttet[0] & 1)) &&
            ((nexttet[2] < nexttet[0]) || (nexttet[2] < nexttet[1]))) ||
           (((nexttet[3] & 1) == (nexttet[0] & 1)) &&
            ((nexttet[3] < nexttet[0]) || (nexttet[3] < nexttet[1]))) ||
           (((nexttet[3] & 1) == (nexttet[2] & 1)) &&
            (((nexttet[2] < nexttet[0]) && (nexttet[3] < nexttet[0])) ||
             ((nexttet[2] < nexttet[1]) && (nexttet[3] < nexttet[1])))));
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexprint()   Print the contents of a tetcomplex.                  */
/*                                                                           */
/*  This procedure prints every tetrahedron in a tetcomplex.  It only works  */
/*  correctly if the stars are consistent with each other; for instance, it  */
/*  will not print meaningful information half way through star splaying.    */
/*                                                                           */
/*  plex:  The tetcomplex to print.                                          */
/*                                                                           */
/*****************************************************************************/

void tetcomplexprint(struct tetcomplex *plex) {
  struct tetcomplexposition pos;
  tag nexttet[4];
  int i;

  printf("List of all tetrahedra in the tetcomplex:\n");
  tetcomplexiteratorinit(plex, &pos);
  tetcomplexiterate(&pos, nexttet);
  while (nexttet[0] != STOP) {
    printf("  Vertex tags");
    for (i = 0; i < 4; i++) {
      if (nexttet[i] == GHOSTVERTEX) {
        printf(" GHOST");
      } else {
        printf(" %lu", (unsigned long) nexttet[i]);
      }
    }
    printf("\n");
    tetcomplexiterate(&pos, nexttet);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexprintstars()   Print the stars in a tetcomplex.                */
/*                                                                           */
/*  This procedure prints every _copy_ of every tetrahedron in the data      */
/*  structure, so it exposes the underlying implementation, and the fact     */
/*  that each solid tetrahedron is stored in the link rings of two, three,   */
/*  or six edges (depending on the parities of the tetrahedron's vertices).  */
/*                                                                           */
/*  plex:  The tetcomplex to print.                                          */
/*                                                                           */
/*****************************************************************************/

void tetcomplexprintstars(struct tetcomplex *plex) {
  struct tetcomplexstar *star;
  tag tagloop;

  printf("List of all stars in the tetcomplex:\n");
  for (tagloop = 0; tagloop < plex->nextinitindex; tagloop++) {
    star = (struct tetcomplexstar *)
           arraypoolfastlookup(&plex->stars, (arraypoolulong) tagloop);
    if (star->linkhead != STOP) {
      printf("Vertex tag %lu, ", (unsigned long) tagloop);
      link2dprint(&plex->moleculepool, star->linkhead, tagloop);
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexmissingtet()   Check whether a tetrahedron is missing from a   */
/*                           (particular) link ring where it should be.      */
/*                                                                           */
/*  This procedure checks whether the edge (vtx1, vtx2) stores a link ring,  */
/*  and if so, whether the star of (vtx1, vtx2) includes the tetrahedron     */
/*  (vtx1, vtx2, vtx3, vtx4).  If not, it prints an error message and        */
/*  returns 1.                                                               */
/*                                                                           */
/*  Ghost edges are not guaranteed to have explicitly stored link rings,     */
/*  so if one of `vtx1' or `vtx2' is a ghost vertex, this procedure does     */
/*  nothing but return 0.                                                    */
/*                                                                           */
/*  plex:  The tetcomplex to check for the presence of the tetrahedron.      */
/*  vtx1, vtx2, vtx3, vtx4:  The tags for the vertices of the tetrahedron.   */
/*    (vtx1, vtx2) is the edge whose link ring should include (vtx3, vtx4).  */
/*                                                                           */
/*  Returns 1 if the tetrahedron should be present but is missing.  Returns  */
/*  0 if the tetrahedron is present, if one of `vtx1' or `vtx2' is a ghost   */
/*  vertex, or if (vtx1, vtx2) does not store a link ring (because the two   */
/*  vertices have opposite parity).                                          */
/*                                                                           */
/*****************************************************************************/

unsigned int tetcomplexmissingtet(struct tetcomplex *plex,
                                  tag vtx1,
                                  tag vtx2,
                                  tag vtx3,
                                  tag vtx4) {
  tag adjacencies[2];
  int existflag;

  /* Are `vtx1' and `vtx2' non-ghost vertices with the same parity? */
  if (((vtx1 & 1) == (vtx2 & 1)) &&
      (vtx1 != GHOSTVERTEX) && (vtx2 != GHOSTVERTEX) &&
      ((vtx3 != GHOSTVERTEX) || (vtx4 != GHOSTVERTEX))) {
    /* Yes.  The tetrahedron (vtx1, vtx2, vtx3, vtx4) should exist in the */
    /*   link ring of (vtx1, vtx2).                                       */
    if (vtx3 != GHOSTVERTEX) {
      existflag = tetcomplexringadjacencies(plex, vtx1, vtx2, vtx3,
                                            adjacencies);
      existflag = existflag && (adjacencies[1] == vtx4);
    } else {
      existflag = tetcomplexringadjacencies(plex, vtx1, vtx2, vtx4,
                                            adjacencies);
      existflag = existflag && (adjacencies[0] == vtx3);
    }
    if (!existflag) {
      printf("  !! !! Tet x=%lu y=%lu %lu %lu is in star of edge (x, y),\n",
             (unsigned long) vtx1, (unsigned long) vtx4,
             (unsigned long) vtx2, (unsigned long) vtx3);
      printf("    but tet v=%lu w=%lu %lu %lu is not in star of (v, w).\n",
             (unsigned long) vtx1, (unsigned long) vtx2,
             (unsigned long) vtx3, (unsigned long) vtx4);
      return 1;
    }
  }

  return 0;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexconsistency()   Check whether a tetcomplex is consistent, in   */
/*                            the sense that any tetrahedron stored in the   */
/*                            complex is duplicated in the stars of all its  */
/*                            represented edges.                             */
/*                                                                           */
/*  Prints error messages if inconsistencies are found, or a reassuring      */
/*  message otherwise.                                                       */
/*                                                                           */
/*  plex:  The tetcomplex to check for internal consistency.                 */
/*                                                                           */
/*****************************************************************************/

void tetcomplexconsistency(struct tetcomplex *plex) {
  struct tetcomplexposition pos;
  tag nexttet[4];
  arraypoolulong horrors;

  printf("Checking consistency of tetrahedral complex...\n");

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

  if (horrors == 0) {
      printf("  Tremble before my vast wisdom, with which "
             "I find the mesh to be consistent.\n");
  } else if (horrors == 1) {
    printf("  !! !! !! !! Precisely one oozing cyst sighted.\n");
  } else {
    printf("  !! !! !! !! %lu monstrosities witnessed.\n",
           (unsigned long) horrors);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexdeleteorderedtet()   Delete an ordered tetrahedron from a      */
/*                                 tetcomplex.                               */
/*                                                                           */
/*  "Ordered tetrahedron" implies that the tetrahedron is deleted only from  */
/*  the star of vertex `vtx1', and therein, only from the star of edge       */
/*  (vtx1, vtx2).  This is a helper function for other tetrahedron deletion  */
/*  procedures, and is directly useful for algorithms like star splaying     */
/*  where the stars are not always mutually consistent.  If you want to keep */
/*  your tetcomplex internally consistent, you probably should not call this */
/*  procedure directly.                                                      */
/*                                                                           */
/*  Any face of the tetrahedron that is not shared by another tetrahedron    */
/*  in the star of (vtx1, vtx2) also vanishes from that star.                */
/*                                                                           */
/*  The orientation of the tetrahedron matters.  (The orientation can be     */
/*  reversed by swapping `vtx3' and `vtx4'.)  You cannot delete (1, 2, 3, 4) */
/*  by asking to delete (1, 2, 4, 3).                                        */
/*                                                                           */
/*  WARNING:  This procedure will not usually work if there is more than one */
/*  copy of `vtx3', or more than one copy of `vtx4', in the link of (vtx1,   */
/*  vtx2).  (This circumstance is not easy to create, but it's possible.)    */
/*                                                                           */
/*  plex:  The tetcomplex to delete a tetrahedron from.                      */
/*  vtx1, vtx2, vtx3, vtx4:  The tags for the vertices of the deleted        */
/*    tetrahedron, ordered so that only the star of `vtx1' changes, and      */
/*    within that star, only the star of edge (vtx1, vtx2) changes.          */
/*                                                                           */
/*  Returns one of the following values.                                     */
/*     0:  If the tetrahedron is not present in the star of the ordered edge */
/*         (vtx1, vtx2), and the tetcomplex is unchanged.                    */
/*     1:  If the triangle (vtx1, vtx2, vtx3) was deleted, and (vtx1, vtx2,  */
/*         vtx4) survives because it is a face of another tetrahedron in the */
/*         star.                                                             */
/*     2:  If (vtx1, vtx2, vtx4) was deleted, and (vtx1, vtx2, vtx3)         */
/*         survives.                                                         */
/*     3:  If both faces are deleted, but some other tetrahedron survives in */
/*         the edge's star.                                                  */
/*     7:  If both faces are deleted, leaving the ordered edge's star empty, */
/*         so the ordered edge is deleted as well.                           */
/*     8:  If the tetrahedron is deleted, but both faces survive.            */
/*                                                                           */
/*    Hence, a positive return value implies that the tetrahedron was        */
/*    deleted.  If the result is not zero, the 1's bit signifies that (vtx1, */
/*    vtx2, vtx3) is no longer in the edge's star, and the 2's bit signifies */
/*    that (vtx1, vtx2, vtx4) is no longer in the edge's star.  (If the      */
/*    result is zero, this procedure doesn't check whether the faces are     */
/*    present.)                                                              */
/*                                                                           */
/*****************************************************************************/

int tetcomplexdeleteorderedtet(struct tetcomplex *plex,
                               tag vtx1,
                               tag vtx2,
                               tag vtx3,
                               tag vtx4)
{
  struct tetcomplexstar *star;
  tag ring;
  int result;

  if (vtx1 >= plex->nextinitindex) {
    /* `vtx1' has never had a star, so the tetrahedron can't exist. */
    return 0;
  }
  /* Look up the star of vertex `vtx1'. */
  star = (struct tetcomplexstar *)
         arraypoolfastlookup(&plex->stars, (arraypoolulong) vtx1);

  /* Find the link ring for edge (vtx1, vtx2) in the star of `vtx1'. */
  ring = link2dfindring(&plex->moleculepool, plex->cache, star->linkhead,
                        vtx1, vtx2);
  if (ring == STOP) {
    /* `vtx2' is not in the link of `vtx1', so the tetrahedron can't exist. */
    return 0;
  }

  /* Attempt to delete the tetrahedron from the star of (vtx1, vtx2). */
  result = linkringdeleteedge(&plex->moleculepool, ring, vtx1, vtx3, vtx4);
  if ((result & 4) != 0) {
    /* The link ring is now empty, so deallocate it and remove `vtx2' from */
    /*   the 2D link of `vtx1'.                                            */
    linkringdelete(&plex->moleculepool, ring);
    link2ddeletevertex(&plex->moleculepool, plex->cache, star->linkhead,
                       &star->linktail, vtx1, vtx2);
  }

  /* If linkringdeleteedge() returned -1, change it to 0. */
  if (result <= 0) {
    return 0;
  }
  return result;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexdeletetet()   Delete a tetrahedron from a tetcomplex.          */
/*                                                                           */
/*  Any face or edge of the tetrahedron that is not shared by another        */
/*  tetrahedron also vanishes.                                               */
/*                                                                           */
/*  The orientation of the tetrahedron matters.  (The orientation can be     */
/*  reversed by swapping any two vertices.)  You cannot delete (1, 2, 3, 4)  */
/*  by asking to delete (1, 2, 4, 3).  However, all even permutations of the */
/*  vertices are equivalent; deleting (1, 2, 3, 4) is the same as deleting   */
/*  (2, 1, 4, 3).                                                            */
/*                                                                           */
/*  If the tetrahedral complex is not internally consistent, this procedure  */
/*  will delete whatever copies of the tetrahedron it happens to find (which */
/*  might not be all of them).  It it finds some but not all of the copies   */
/*  it expects to find, it returns -1.  This differs from                    */
/*  tetcomplexinserttet(), which rolls back the changes when it finds an     */
/*  inconsistency.                                                           */
/*                                                                           */
/*  plex:  The tetcomplex to delete a tetrahedron from.                      */
/*  vtx1, vtx2, vtx3, vtx4:  The tags for the vertices of the deleted        */
/*    tetrahedron.                                                           */
/*                                                                           */
/*  Returns one of the following values.                                     */
/*   -1:  If the tetrahedron was deleted from some, but not all, of the edge */
/*        stars in which it should have resided.  This usually means that    */
/*        the tetcomplex is (or was) not internally consistent.              */
/*    0:  If the tetrahedron cannot be deleted, and the tetcomplex is        */
/*        unchanged.                                                         */
/*    1:  If the tetrahedron was deleted successfully.                       */
/*                                                                           */
/*****************************************************************************/

int tetcomplexdeletetet(struct tetcomplex *plex,
                        tag vtx1,
                        tag vtx2,
                        tag vtx3,
                        tag vtx4)
{
  tag swaptag;
  int result;
  int deletionflag;
  int failflag;
  int i;

  if (plex->verbosity > 3) {
    printf("        Deleting tet w/tags %lu %lu %lu %lu.\n",
           (unsigned long) vtx1, (unsigned long) vtx2,
           (unsigned long) vtx3, (unsigned long) vtx4);
  }

#ifdef SELF_CHECK
  if ((vtx1 == vtx2) || (vtx1 == vtx3) || (vtx1 == vtx4) ||
      (vtx2 == vtx3) || (vtx2 == vtx4) || (vtx3 == vtx4)) {
    printf("Internal error in tetcomplexdeletetet():\n");
    printf("  Asked to delete tetrahedron with two vertices alike.\n");
    internalerror();
  }
#endif /* SELF_CHECK */

  /* No changes have been made to the tetcomplex yet. */
  deletionflag = 0;
  /* Assume we won't fail, until proven otherwise. */
  failflag = 0;

  if ((vtx1 == GHOSTVERTEX) || (vtx2 == GHOSTVERTEX) ||
      (vtx3 == GHOSTVERTEX) || (vtx4 == GHOSTVERTEX)) {
    /* Make `vtx1', `vtx2', and `vtx3' the non-ghost vertices (preserving */
    /*   the tetrahedron's orientation).                                  */
    if (vtx1 == GHOSTVERTEX) {
      vtx1 = vtx2;
      vtx2 = vtx4;
    } else if (vtx2 == GHOSTVERTEX) {
      vtx2 = vtx3;
      vtx3 = vtx4;
    } else if (vtx3 == GHOSTVERTEX) {
      vtx3 = vtx1;
      vtx1 = vtx4;
    }

    /* For each vertex with the same parity as the ghost vertex, there is  */
    /*   a ghost edge from which the tetrahedron should be deleted.  Ghost */
    /*   tetrahedra are not explicitly stored in the link rings of solid   */
    /*   edges, so ignore the solid edges.                                 */
    if ((vtx1 & 1) == (GHOSTVERTEX & 1)) {
      if (tetcomplexdeleteorderedtet(plex, vtx1, GHOSTVERTEX, vtx2, vtx3) >
          0) {
        deletionflag = 1;
      } else {
        failflag = 1;
      }
    }
    if ((vtx2 & 1) == (GHOSTVERTEX & 1)) {
      if (tetcomplexdeleteorderedtet(plex, vtx2, GHOSTVERTEX, vtx3, vtx1) >
          0) {
        deletionflag = 1;
      } else {
        failflag = 1;
      }
    }
    if ((vtx3 & 1) == (GHOSTVERTEX & 1)) {
      if (tetcomplexdeleteorderedtet(plex, vtx3, GHOSTVERTEX, vtx1, vtx2) >
          0) {
        deletionflag = 1;
      } else {
        failflag = 1;
      }
    }

    if (!failflag) {
      /* Note that this counter may be decremented even if no change was     */
      /*   made to the tetcomplex at all, simply because the GHOSTVERTEX has */
      /*   a parity opposite to that of the other three vertices.            */
      plex->ghosttetcount--;

      /*^^^ Assertion:  The tetrahedral complex is now consistent. */
#ifdef PARANOID
      tetcomplexconsistency(plex);
#endif /* PARANOID */

      return 1;
    }
  } else {
    /* Loop over all six edges of the tetrahedron. */
    for (i = 0; i < 6; i++) {
      /* If the endpoints of the edge (vtx1, vtx2) have the same parity, the */
      /*   edge's link ring is stored, so delete the tetrahedron from the    */
      /*   edge's star.                                                      */
      if ((vtx1 & 1) == (vtx2 & 1)) {
        /* Determine which vertex star stores the link for (vtx1, vtx2). */
        if ((vtx1 < vtx2) ^ ((vtx1 & 2) == (vtx2 & 2))) {
          result = tetcomplexdeleteorderedtet(plex, vtx1, vtx2, vtx3, vtx4);
        } else {
          result = tetcomplexdeleteorderedtet(plex, vtx2, vtx1, vtx4, vtx3);
        }
        if (result > 0) {
          deletionflag = 1;
        } else {
          failflag = 1;
        }
      }

      /* The following shift cycles (vtx1, vtx2) through all the edges while */
      /*   maintaining the tetrahedron's orientation.  The schedule is       */
      /*   i = 0:  1 2 3 4 => 2 3 1 4                                        */
      /*   i = 1:  2 3 1 4 => 2 4 3 1                                        */
      /*   i = 2:  2 4 3 1 => 4 3 2 1                                        */
      /*   i = 3:  4 3 2 1 => 4 1 3 2                                        */
      /*   i = 4:  4 1 3 2 => 1 3 4 2                                        */
      /*   i = 5:  1 3 4 2 => 1 2 3 4 (which isn't used).                    */
      if ((i & 1) == 0) {
        swaptag = vtx1;
        vtx1 = vtx2;
        vtx2 = vtx3;
        vtx3 = swaptag;
      } else {
        swaptag = vtx4;
        vtx4 = vtx3;
        vtx3 = vtx2;
        vtx2 = swaptag;
      }
    }

    if (!failflag) {
      /* The tetrahedron has been successfully deleted. */
      plex->tetcount--;

      /*^^^ Assertion:  The tetrahedral complex is now consistent. */
#ifdef PARANOID
      tetcomplexconsistency(plex);
#endif /* PARANOID */

      return 1;
    }
  }

  /* The tetrahedron was missing from some edge's star where it should have */
  /*   appeared, so the deletion has failed.                                */
  if (deletionflag) {
    /* Nevertheless, a partial deletion occurred and the tetcomplex changed. */
    return -1;
  }

  /* The tetcomplex didn't change. */
  return 0;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexlookup3dstar()   Return the `tetcomplexstar' record for a      */
/*                             vertex, creating one if necessary.            */
/*                                                                           */
/*  This procedure allocates memory for, and initializes, `tetcomplexstar'   */
/*  records for all uninitialized vertex stars up to and including the tag   */
/*  `vtx'.  It also allocates a 2D link for `vtx' if it doesn't already have */
/*  one; but it doesn't allocate 2D links for tags prior to `vtx'.           */
/*                                                                           */
/*  plex:  The tetcomplex in which the vertex needs a `tetcomplexstar'.      */
/*  vtx:  The tag for the vertex whose `tetcomplexstar' record is returned   */
/*        (and possibly created).                                            */
/*                                                                           */
/*  Returns a pointer to the `tetcomplexstar' record for `vtx'.              */
/*                                                                           */
/*****************************************************************************/

struct tetcomplexstar *tetcomplexlookup3dstar(struct tetcomplex *plex,
                                              tag vtx)
{
  struct tetcomplexstar *star;
  proxipoolulong allocindex;
  tag i;

  for (i = plex->nextinitindex; i <= vtx; i++) {
    /* Make sure memory is allocated for star `i'. */
    star = (struct tetcomplexstar *)
           arraypoolforcelookup(&plex->stars, (arraypoolulong) i);
    /* Initially, the 2D link is empty. */
    star->linkhead = STOP;
    star->linktail = STOP;
  }
  /* Remember where to continue next time. */
  plex->nextinitindex = i;

  /* Look up the star of vertex `vtx'. */
  star = (struct tetcomplexstar *)
         arraypoolfastlookup(&plex->stars, (arraypoolulong) vtx);
  if (star->linkhead == STOP) {
    /* Vertex `vtx' doesn't have a 2D link yet.  Allocate one, using the */
    /*   same allocation index that was used for the vertex `vtx'.       */
    if (plex->vertexpool == (struct proxipool *) NULL) {
      allocindex = 0;
    } else {
      allocindex = proxipooltag2allocindex(plex->vertexpool, vtx);
    }
    star->linkhead = link2dnew(&plex->moleculepool, allocindex);
    star->linktail = star->linkhead;
  }

  return star;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexinsertorderedtet()   Insert an ordered tetrahedron into a      */
/*                                 tetcomplex.                               */
/*                                                                           */
/*  "Ordered tetrahedron" implies that the tetrahedron is inserted only into */
/*  the star of vertex `vtx1', and therein, it is inserted only into the     */
/*  star of edge (vtx1, vtx2).  Therefore, even permutations of the vertices */
/*  are NOT equivalent in this procedure.  This is a helper procedure for    */
/*  other tetrahedron insertion procedures, and is also directly useful for  */
/*  algorithms like star splaying where the stars are not always mutually    */
/*  consistent.  If you want to keep your tetcomplex internally consistent,  */
/*  you should not call this procedure directly.                             */
/*                                                                           */
/*  The orientation of the tetrahedron matters.  (The orientation can be     */
/*  reversed by swapping `vtx3' and `vtx4'.)  Inserting (1, 2, 3, 4) is not  */
/*  the same as inserting (1, 2, 4, 3).                                      */
/*                                                                           */
/*  This procedure will return 0 and leave the tetcomplex unchanged if any   */
/*  of the following is true.                                                */
/*                                                                           */
/*  - There is already a tetrahedron (vtx1, vtx2, vtx3, X) for some X in the */
/*    star of the ordered edge (vtx1, vtx2).                                 */
/*  - There is already a tetrahedron (vtx1, vtx2, X, vtx4) for some X in the */
/*    star of the ordered edge (vtx1, vtx2).                                 */
/*  - The link of the ordered edge (vtx1, vtx2) is truly a ring, with no     */
/*    place to insert a new tetrahedron.                                     */
/*  - The link of the ordered edge (vtx1, vtx2) currently consists of two or */
/*    more chains, and the introduction of edge (vtx3, vtx4) into the link   */
/*    will glue one of the chains into a ring.  It's not topologically       */
/*    possible (in a tetrahedral complex embeddable in 3D space) for an edge */
/*    to have a link with a ring AND a chain.                                */
/*                                                                           */
/*  WARNING:  This procedure will not usually work if there is more than one */
/*  copy of `vtx3', or more than one copy of `vtx4', in the link of (vtx1,   */
/*  vtx2).  (This circumstance is not easy to create, but it's possible.)    */
/*                                                                           */
/*  plex:  The tetcomplex to insert a tetrahedron into.                      */
/*  vtx1, vtx2, vtx3, vtx4:  The tags for the vertices of the new            */
/*    tetrahedron, ordered so that only the star of `vtx1' changes, and      */
/*    within that star, only the star of edge (vtx1, vtx2) changes.          */
/*                                                                           */
/*  Returns one of the following values.                                     */
/*    0:  If the tetrahedron cannot be inserted, and the tetcomplex is       */
/*        unchanged.                                                         */
/*    1:  If the triangle (vtx1, vtx2, vtx3) is new in the star of the       */
/*        ordered edge (vtx1, vtx2), and (vtx1, vtx2, vtx4) was already      */
/*        present.                                                           */
/*    2:  If the triangle (vtx1, vtx2, vtx4) is new in the star of the       */
/*        ordered edge (vtx1, vtx2), and (vtx1, vtx2, vtx3) was already      */
/*        present.                                                           */
/*    3:  If neither triangle was present, and both are new.                 */
/*    4:  If both triangles (vtx1, vtx2, vtx3) and (vtx1, vtx2, vtx4) were   */
/*        already present in the star of (vtx1, vtx2), and the new           */
/*        tetrahedron slid nicely between them.                              */
/*                                                                           */
/*  Hence, the 1's bit signifies that (vtx1, vtx2, vtx3) was created anew,   */
/*  and the 2's bit signifies that (vtx1, vtx2, vtx4) was created anew.      */
/*                                                                           */
/*****************************************************************************/

int tetcomplexinsertorderedtet(struct tetcomplex *plex,
                               tag vtx1,
                               tag vtx2,
                               tag vtx3,
                               tag vtx4)
{
  struct tetcomplexstar *star;
  tag ring;

  /* Make sure `vtx1' has a 2D star, and find it. */
  star = tetcomplexlookup3dstar(plex, vtx1);
  /* Find the link ring for edge (vtx1, vtx2) in the star of `vtx1'. */
  ring = link2dfindinsert(&plex->moleculepool, plex->cache, star->linkhead,
                          &star->linktail, vtx1, vtx2);
  /* Insert the tetrahedron into the star of the ordered edge (vtx1, vtx2). */
  return linkringinsertedge(&plex->moleculepool, ring, vtx1, vtx3, vtx4);
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexinserttet()   Insert a tetrahedron into a tetcomplex.          */
/*                                                                           */
/*  The orientation of the tetrahedron matters.  (The orientation can be     */
/*  reversed by swapping any two vertices.)  Inserting (1, 2, 3, 4) is not   */
/*  the same as inserting (1, 2, 4, 3).  However, all even permutations of   */
/*  the vertices are equivalent; (1, 2, 3, 4) is the same as (2, 1, 4, 3).   */
/*                                                                           */
/*  This procedure will return 0 and leave the link ring unchanged if any of */
/*  the following is true.                                                   */
/*                                                                           */
/*  - Some tetrahedron already in the complex has a face in common with the  */
/*    new tetrahedron, and both tetrahedra would be on the same side of that */
/*    face.  For example, there is already a tetrahedron (vtx1, vtx2, X,     */
/*    vtx4) for some vertex X.                                               */
/*  - One of the edges of the new tetrahedron has a full complement of       */
/*    tetrahedra encircling it.  In other words, the edge's link is a ring.  */
/*  - One of the edges of the new tetrahedron has a link consisting of two   */
/*    or more chains, and the introduction of the new tetrahedron will glue  */
/*    one of the chains into a ring.  It's not topologically possible to     */
/*    have a link with a ring AND a chain.                                   */
/*                                                                           */
/*  plex:  The tetcomplex to insert a tetrahedron into.                      */
/*  vtx1, vtx2, vtx3, vtx4:  The tags for the vertices of the new            */
/*    tetrahedron.                                                           */
/*                                                                           */
/*  Returns one of the following values.                                     */
/*    0:   If the tetrahedron cannot be inserted, and the tetcomplex is      */
/*         unchanged.                                                        */
/*    1:   If a ghost tetrahedron was inserted successfully.                 */
/*    2:   If a solid tetrahedron was inserted successfully, and the number  */
/*         of boundary triangles decreased by 4.                             */
/*    4:   If a solid tetrahedron was inserted successfully, and the number  */
/*         of boundary triangles decreased by 2.                             */
/*    6:   If a solid tetrahedron was inserted successfully, and the number  */
/*         of boundary triangles remained constant.                          */
/*    8:   If a solid tetrahedron was inserted successfully, and the number  */
/*         of boundary triangles increased by 2.                             */
/*    10:  If a solid tetrahedron was inserted successfully, and the number  */
/*         of boundary triangles increased by 4.                             */
/*                                                                           */
/*****************************************************************************/

int tetcomplexinserttet(struct tetcomplex *plex,
                        tag vtx1,
                        tag vtx2,
                        tag vtx3,
                        tag vtx4)
{
  tag lasthead = 0;
  tag lasttail = 0;
  tag swaptag;
  int result = 0;
  int linkringchanges;
  int deltafaces;
  int i;

  if (plex->verbosity > 3) {
    printf("        Inserting tet w/tags %lu %lu %lu %lu.\n",
           (unsigned long) vtx1, (unsigned long) vtx2,
           (unsigned long) vtx3, (unsigned long) vtx4);
  }

#ifdef SELF_CHECK
  if ((vtx1 == vtx2) || (vtx1 == vtx3) || (vtx1 == vtx4) ||
      (vtx2 == vtx3) || (vtx2 == vtx4) || (vtx3 == vtx4)) {
    printf("Internal error in tetcomplexinserttet():\n");
    printf("  Asked to insert tetrahedron with two vertices alike.\n");
    internalerror();
  }
#endif /* SELF_CHECK */

  /* No changes have been made to the tetcomplex yet. */
  linkringchanges = 0;

  if ((vtx1 == GHOSTVERTEX) || (vtx2 == GHOSTVERTEX) ||
      (vtx3 == GHOSTVERTEX) || (vtx4 == GHOSTVERTEX)) {
    /* Make `vtx1', `vtx2', and `vtx3' the non-ghost vertices (preserving */
    /*   the tetrahedron's orientation).                                  */
    if (vtx1 == GHOSTVERTEX) {
      vtx1 = vtx2;
      vtx2 = vtx4;
    } else if (vtx2 == GHOSTVERTEX) {
      vtx2 = vtx3;
      vtx3 = vtx4;
    } else if (vtx3 == GHOSTVERTEX) {
      vtx3 = vtx1;
      vtx1 = vtx4;
    }

    /* For each vertex with the same parity as the ghost vertex, there is */
    /*   (or will be) a ghost edge.  Store the ghost tetrahedron in the   */
    /*   star of the ghost edge.  Note that ghost tetrahedra are not      */
    /*   explicitly stored in the link rings of solid edges, but they are */
    /*   implicitly already there if the ghost tetrahedra are consistent  */
    /*   with the solid tetrahedra.                                       */
    if ((vtx1 & 1) == (GHOSTVERTEX & 1)) {
      if (tetcomplexinsertorderedtet(plex, vtx1, GHOSTVERTEX, vtx2, vtx3) <=
          0) {
        return 0;
      }
      linkringchanges = 1;
    }
    if ((vtx2 & 1) == (GHOSTVERTEX & 1)) {
      if (tetcomplexinsertorderedtet(plex, vtx2, GHOSTVERTEX, vtx3, vtx1) <=
          0) {
        if (linkringchanges) {
          /* Roll back the change that has been made. */
          tetcomplexdeleteorderedtet(plex, vtx1, GHOSTVERTEX, vtx2, vtx3);
        }
        return 0;
      }
      linkringchanges = linkringchanges | 2;
    }
    if ((vtx3 & 1) == (GHOSTVERTEX & 1)) {
      if (tetcomplexinsertorderedtet(plex, vtx3, GHOSTVERTEX, vtx1, vtx2) <=
          0) {
        if ((linkringchanges & 1) != 0) {
          /* Roll back the change. */
          tetcomplexdeleteorderedtet(plex, vtx1, GHOSTVERTEX, vtx2, vtx3);
        }
        if ((linkringchanges & 2) != 0) {
          /* Roll back the change. */
          tetcomplexdeleteorderedtet(plex, vtx2, GHOSTVERTEX, vtx3, vtx1);
        }
        return 0;
      }
    }

    /* Note that this counter may be incremented even if no change was made */
    /*   to the tetcomplex at all, simply because the GHOSTVERTEX has a     */
    /*   parity opposite to that of the other three vertices.               */
    plex->ghosttetcount++;

    /*^^^ Assertion:  The tetrahedral complex is now consistent. */
#ifdef PARANOID
    tetcomplexconsistency(plex);
#endif /* PARANOID */

    return 1;
  }

  /* No triangular boundary faces have been created/covered yet. */
  deltafaces = 0;

  /* Loop over all six edges of the tetrahedron. */
  for (i = 0; i < 6; i++) {
    /* If the endpoints of the edge (vtx1, vtx2) have the same parity, the */
    /*   edge's link ring is stored, so insert the new tetrahedron into    */
    /*   the edge's star.                                                  */
    if ((vtx1 & 1) == (vtx2 & 1)) {
      /* Determine which vertex star stores the link for (vtx1, vtx2). */
      if ((vtx1 < vtx2) ^ ((vtx1 & 2) == (vtx2 & 2))) {
        result = tetcomplexinsertorderedtet(plex, vtx1, vtx2, vtx3, vtx4);
        lasthead = vtx1;
        lasttail = vtx4;
      } else {
        result = tetcomplexinsertorderedtet(plex, vtx2, vtx1, vtx4, vtx3);
        lasthead = vtx2;
        lasttail = vtx3;
      }
      if (result == 3) {
        /* Two new triangular faces are on the boundary. */
        deltafaces += 2;
      } else if (result == 4) {
        /* Two triangular faces are covered, and no longer on the boundary. */
        deltafaces -= 2;
      } else if (result <= 0) {
        /* The insertion failed.  If any previous insertions were */
        /*   successful, roll back the changes that were made.    */
        if (linkringchanges != 0) {
          tetcomplexdeletetet(plex, vtx1, vtx2, vtx3, vtx4);
        }
        return 0;
      }
      /* Count the number of edges whose link rings have changed. */
      linkringchanges++;
    }

    /* The following rotation cycles (vtx1, vtx2) through all the edges    */
    /*   while maintaining the tetrahedron's orientation.  The schedule is */
    /*   i = 0:  1 2 3 4 => 2 3 1 4                                        */
    /*   i = 1:  2 3 1 4 => 2 4 3 1                                        */
    /*   i = 2:  2 4 3 1 => 4 3 2 1                                        */
    /*   i = 3:  4 3 2 1 => 4 1 3 2                                        */
    /*   i = 4:  4 1 3 2 => 1 3 4 2                                        */
    /*   i = 5:  1 3 4 2 => 1 2 3 4 (which isn't used).                    */
    if ((i & 1) == 0) {
      swaptag = vtx1;
      vtx1 = vtx2;
      vtx2 = vtx3;
      vtx3 = swaptag;
    } else {
      swaptag = vtx4;
      vtx4 = vtx3;
      vtx3 = vtx2;
      vtx2 = swaptag;
    }
  }

  /* The tetrahedron has been successfully inserted. */
  plex->tetcount++;

  /*^^^ Assertion:  The tetrahedral complex is now consistent. */
#ifdef PARANOID
  tetcomplexconsistency(plex);
#endif /* PARANOID */

  /* `linkringchanges' can be 2, 3, or 6, depending on the parities of       */
  /*   the vertices.  If it's 2, then only two (opposing) edges of the       */
  /*   tetrahedron have link rings, so the change `deltafaces' in the number */
  /*   of boundary faces was counted correctly.  Otherwise, it needs to be   */
  /*   adjusted.                                                             */
  if (linkringchanges == 3) {
    /* One of the face's changes was counted three times (that face being  */
    /*   the one opposite the vertex whose parity disagrees with the other */
    /*   three vertices), whereas the other three faces' changes were each */
    /*   counted once.                                                     */
    if (deltafaces > 0) {
      /* A newly created boundary face was counted thrice. */
      deltafaces -= 2;
    } else if (deltafaces < 0) {
      /* A covered boundary face was counted thrice. */
      deltafaces += 2;
    } else {
      /* The face that was counted three times cancels out the other three  */
      /*   faces.  Therefore, `deltafaces' is zero, and `result' is either  */
      /*   1 or 2.  Was a new boundary face counted three times, or was a   */
      /*   covered boundary face counted thrice?  If `lasttail' has parity  */
      /*   opposite to `lasthead', then `lasttail' is the parity-mismatched */
      /*   vertex.                                                          */
      if (((lasttail & 1) != (lasthead & 1)) ^ (result == 1)) {
        /* Either `lasttail' is the parity-mismatched vertex and             */
        /*   result == 2, in which case a boundary face opposite `lasttail'  */
        /*   got covered (is no longer on the boundary) and counted thrice;  */
        /*   or `lasttail' matches two other vertices' parity and            */
        /*   result == 1, in which case a boundary face adjoining `lasttail' */
        /*   got covered and counted thrice.                                 */
        deltafaces += 2;
      } else {
        /* Reversing the reasoning above, a new boundary face got counted */
        /*   thrice.                                                      */
        deltafaces -= 2;
      }
    }
  } else if (linkringchanges == 6) {
    /* All four vertices have the same parity, so all six edges of the */
    /*   tetrahedron have link rings, so every face change was counted */
    /*   thrice.                                                       */
    deltafaces /= 3;
  }
  return 6 + deltafaces;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplex12fliponedge()   Replace a tetrahedron with two in an edge's   */
/*                             star.                                         */
/*                                                                           */
/*  Replaces the tetrahedron (vtx1, vtx2, vtx3, vtx4) with (vtx1, vtx2,      */
/*  vtx3, newvertex) and (vtx1, vtx2, newvertex, vtx4) in the star of the    */
/*  edge (vtx1, vtx2).  If `vtx1' and `vtx2' have opposite parities, this    */
/*  procedure assumes that the edge is not stored, and does nothing.         */
/*                                                                           */
/*  Equivalently, this procedure replace one edge with two in the link       */
/*  ring of the edge (vtx1, vtx2).  This is accomplished by inserting a      */
/*  vertex `newvertex' between `vxt3' and `vtx4' in the link ring.           */
/*                                                                           */
/*  This is a helper procedure for other procedures.  Used in isolation, it  */
/*  changes the complex so the stars are not mutually consistent.  If you    */
/*  want to keep your tetcomplex internally consistent, you probably should  */
/*  not call this procedure directly.                                        */
/*                                                                           */
/*  WARNING:  This procedure does not check whether the tetrahedron (vtx1,   */
/*  vtx2, vtx3, vtx4) is in the vertex's star.  If the tetrahedron is not    */
/*  there, `newvertex' may be inserted in the link ring, next to `vtx3' or   */
/*  `vtx4', or the program may halt with an internal error.  Don't call this */
/*  procedure unless you're sure the tetrahedron is in the edge's star (if   */
/*  the edge is stored at all).                                              */
/*                                                                           */
/*  plex:  The tetcomplex in which the flip occurs.                          */
/*  vtx1, vtx2, vtx3, vtx4:  The tags for the vertices of the tetrahedron    */
/*    being replaced.  (vtx1, vtx2) is the edge whose star is changed.       */
/*  newvertex:  Tag for the vertex to be inserted between `vtx3' and `vtx4'  */
/*    in the link ring.                                                      */
/*                                                                           */
/*****************************************************************************/

void tetcomplex12fliponedge(struct tetcomplex *plex,
                            tag vtx1,
                            tag vtx2,
                            tag vtx3,
                            tag vtx4,
                            tag newvertex)
{
  struct tetcomplexstar *star;
  tag ring;
  tag swaptag;
  int result;

  /* The edge (vtx1, vtx2) is stored only if vtx1 and vtx2 have the */
  /*   same parity.                                                 */
  if ((vtx1 & 1) != (vtx2 & 1)) {
    return;
  }

  /* Determine which vertex's star stores the edge's link ring. */
  if (!((vtx2 == GHOSTVERTEX) ||
        ((vtx1 != GHOSTVERTEX) &&
         ((vtx1 < vtx2) ^ ((vtx1 & 2) == (vtx2 & 2)))))) {
    /* `vtx2' stores the link ring, so swap it with `vtx1'.  To preserve the */
    /*   tetrahedron orientation, `vtx4' replaces `vtx3'.                    */
    swaptag = vtx1;
    vtx1 = vtx2;
    vtx2 = swaptag;
    vtx3 = vtx4;
  }

  /* Has a star ever been allocated for vtx1? */
  if (vtx1 < plex->nextinitindex) {
    /* Insert `newvertex' after `vtx3' in the link ring for (vtx1, vtx2). */
    star = (struct tetcomplexstar *)
           arraypoolfastlookup(&plex->stars, (arraypoolulong) vtx1);
    ring = link2dfindring(&plex->moleculepool, plex->cache, star->linkhead,
                          vtx1, vtx2);
    result = linkringinsertvertex(&plex->moleculepool, ring,
                                  vtx1, vtx3, newvertex);
    if (result <= 0) {
      printf("Internal error in tetcomplex12fliponedge():\n");
      printf("  Tetrahedron is missing from the edge's star.\n");
      internalerror();
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexinserttetontripod()   Insert a tetrahedron into the link rings */
/*                                  of three edges that meet at `vtx1',      */
/*                                  forming a "tripod".                      */
/*                                                                           */
/*  This is a special-purpose procedure designed to help ease and speed up   */
/*  the Bowyer-Watson algorithm.  It should not normally be used by other    */
/*  algorithms.                                                              */
/*                                                                           */
/*  This procedure is similar to tetcomplexinserttet(), but it only modifies */
/*  the link rings of three of the tetrahedron's six edges--the three edges  */
/*  that meet at `vtx1'.  (Note that this is NOT the same as changing only   */
/*  the star of `vtx1'; the three edges might be stored with the other       */
/*  vertices, and both endpoints of each edge are presumed to have had       */
/*  their stars changed.)  This procedure can introduce (or repair)          */
/*  inconsistencies _within_ the stars of `vtx2', `vtx3', and `vtx4', which  */
/*  the Bowyer-Watson implementation eventually repairs.                     */
/*                                                                           */
/*  This procedure will return 0 and leave the link ring under roughly the   */
/*  same circumstances as tetcomplexinserttet(), although this procedure     */
/*  does not check the edges it doesn't modify.                              */
/*                                                                           */
/*  plex:  The tetcomplex to insert a tetrahedron into.                      */
/*  vtx1, vtx2, vtx3, vtx4:  The tags for the vertices of the new            */
/*    tetrahedron.  Only edges adjoining `vtx1' have their stars modified.   */
/*                                                                           */
/*  Returns one of the following values.                                     */
/*    0:  If the tetrahedron cannot be inserted, and the tetcomplex is       */
/*        unchanged.                                                         */
/*    1:  If the tetrahedron was inserted successfully.                      */
/*                                                                           */
/*****************************************************************************/

int tetcomplexinserttetontripod(struct tetcomplex *plex,
                                tag vtx1,
                                tag vtx2,
                                tag vtx3,
                                tag vtx4)
{
  tag swaptag;
  int result;
  int insertionflag;
  int i;

  if (plex->verbosity > 3) {
    printf("        Inserting tet w/tags %lu %lu %lu %lu\n"
           "          in stars of edges adjoining vertex tag %lu.\n",
           (unsigned long) vtx1, (unsigned long) vtx2,
           (unsigned long) vtx3, (unsigned long) vtx4, (unsigned long) vtx1);
  }

#ifdef SELF_CHECK
  if (vtx1 >= STOP) {
    printf("Internal error in tetcomplexinserttetontripod():\n");
    printf("  First parameter is not a valid vertex tag.\n");
    internalerror();
  }
  if ((vtx1 == vtx2) || (vtx1 == vtx3) || (vtx1 == vtx4) ||
      (vtx2 == vtx3) || (vtx2 == vtx4) || (vtx3 == vtx4)) {
    printf("Internal error in tetcomplexinserttetontripod():\n");
    printf("  Asked to insert tetrahedron with two vertices alike.\n");
    internalerror();
  }
#endif /* SELF_CHECK */

  /* No changes have been made to the tetcomplex yet. */
  insertionflag = 0;

  result = 1;
  if ((vtx2 == GHOSTVERTEX) || (vtx3 == GHOSTVERTEX) ||
      (vtx4 == GHOSTVERTEX)) {
    /* A ghost tetrahedron changes only a ghost edge's link ring.  `vtx1'   */
    /*   stores a ghost edge only if it has the same parity as GHOSTVERTEX. */
    if ((vtx1 & 1) == (GHOSTVERTEX & 1)) {
      /* Insert the ghost tetrahedron into a ghost edge's star. */
      if (vtx2 == GHOSTVERTEX) {
        if (tetcomplexinsertorderedtet(plex,
                                       vtx1, GHOSTVERTEX, vtx3, vtx4) == 0) {
          return 0;
        }
      } else if (vtx3 == GHOSTVERTEX) {
        if (tetcomplexinsertorderedtet(plex,
                                       vtx1, GHOSTVERTEX, vtx4, vtx2) == 0) {
          return 0;
        }
      } else {
        /* vtx4 == GHOSTVERTEX */
        if (tetcomplexinsertorderedtet(plex,
                                       vtx1, GHOSTVERTEX, vtx2, vtx3) == 0) {
          return 0;
        }
      }
    }

    /* Note that this counter may be incremented even if no change was */
    /*   made to the tetcomplex at all, simply because `vtx1' and the  */
    /*   GHOSTVERTEX have opposite parities.                           */
    plex->ghosttetcount++;
    return 1;
  } else {
    /* Loop over the three edges of the tetrahedron adjoining `vtx1'. */
    for (i = 0; i < 3; i++) {
      /* If the endpoints of the edge (vtx1, vtx2) have the same parity, the */
      /*   edge's link ring is stored, so insert the new tetrahedron into    */
      /*   the edge's star.                                                  */
      if ((vtx1 & 1) == (vtx2 & 1)) {
        /* Determine which vertex star stores the link for (vtx1, vtx2). */
        if ((vtx1 < vtx2) ^ ((vtx1 & 2) == (vtx2 & 2))) {
          result = tetcomplexinsertorderedtet(plex, vtx1, vtx2, vtx3, vtx4);
        } else {
          result = tetcomplexinsertorderedtet(plex, vtx2, vtx1, vtx4, vtx3);
        }
        if (result <= 0) {
          /* The insertion failed.  If any previous insertions were */
          /*   successful, roll back the changes that were made.    */
          if (insertionflag) {
            tetcomplexdeleteorderedtet(plex, vtx1, vtx2, vtx3, vtx4);
            tetcomplexdeleteorderedtet(plex, vtx2, vtx1, vtx4, vtx3);
            tetcomplexdeleteorderedtet(plex, vtx1, vtx3, vtx4, vtx2);
            tetcomplexdeleteorderedtet(plex, vtx3, vtx1, vtx2, vtx4);
          }
          return 0;
        }
        /* Remember this change in case it needs to be rolled back later. */
        insertionflag = 1;
      }

      /* The following shift cycles (vtx1, vtx2) through the edges adjoining */
      /*   `vtx1' while maintaining the tetrahedron's orientation.           */
      swaptag = vtx2;
      vtx2 = vtx3;
      vtx3 = vtx4;
      vtx4 = swaptag;
    }

    /* Note that this counter may be incremented even if no change was */
    /*   made to the tetcomplex at all, simply because `vtx1' has a    */
    /*   parity opposite to that of the other three vertices.          */
    plex->tetcount++;
    return 1;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplex21fliponedge()   Replace two adjacent tetrahedra with one in   */
/*                             an edge's star.                               */
/*                                                                           */
/*  Replaces the tetrahedra (vtx1, vtx2, X, vtx3) and (vtx1, vtx2, vtx3, Y)  */
/*  with (vtx1, vtx2, X, Y) in the star of the edge (vtx1, vtx2).  If `vtx1' */
/*  and `vtx2' have opposite parities, this procedure assumes that the edge  */
/*  is not stored, and does nothing.                                         */
/*                                                                           */
/*  Equivalently, this procedure replace two edges with one in the link      */
/*  ring of the edge (vtx1, vtx2).  This is accomplished by deleting a       */
/*  vertex `vtx3' from the link ring.                                        */
/*                                                                           */
/*  This is a helper procedure for other procedures.  Used in isolation, it  */
/*  changes the complex so the stars are not mutually consistent.  If you    */
/*  want to keep your tetcomplex internally consistent, you probably should  */
/*  not call this procedure directly.                                        */
/*                                                                           */
/*  WARNING:  don't call this procedure unless you're sure `vtx3' is in the  */
/*  edge's link ring (if the edge is stored at all).                         */
/*                                                                           */
/*  plex:  The tetcomplex in which the flip occurs.                          */
/*  vtx1, vtx2, vtx3:  The tags for the vertices of the triangle being       */
/*    deleted.  (vtx1, vtx2) is the edge whose star is changed.              */
/*                                                                           */
/*****************************************************************************/

void tetcomplex21fliponedge(struct tetcomplex *plex,
                            tag vtx1,
                            tag vtx2,
                            tag vtx3)
{
  struct tetcomplexstar *star;
  tag ring;
  tag swaptag;
  int result;

  /* The edge (vtx1, vtx2) is stored only if vtx1 and vtx2 have the */
  /*   same parity.                                                 */
  if ((vtx1 & 1) != (vtx2 & 1)) {
    return;
  }

  /* Determine which vertex's star stores the edge's link ring. */
  if (!((vtx2 == GHOSTVERTEX) ||
        ((vtx1 != GHOSTVERTEX) &&
         ((vtx1 < vtx2) ^ ((vtx1 & 2) == (vtx2 & 2)))))) {
    /* `vtx2' stores the link ring, so swap it with `vtx1'. */
    swaptag = vtx1;
    vtx1 = vtx2;
    vtx2 = swaptag;
  }

  /* Has a star ever been allocated for vtx1? */
  if (vtx1 < plex->nextinitindex) {
    /* Delete `vtx3' from the link ring for (vtx1, vtx2). */
    star = (struct tetcomplexstar *)
           arraypoolfastlookup(&plex->stars, (arraypoolulong) vtx1);
    ring = link2dfindring(&plex->moleculepool, plex->cache, star->linkhead,
                          vtx1, vtx2);
    result = linkringdeletevertex(&plex->moleculepool, ring, vtx1, vtx3);
    if ((result & 2) != 0) {
      /* Link ring now empty; free it and remove vtx2 from vtx1's 2D link. */
      linkringdelete(&plex->moleculepool, ring);
      link2ddeletevertex(&plex->moleculepool, plex->cache, star->linkhead,
                         &star->linktail, vtx1, vtx2);
    }
    if (result <= 0) {
      printf("Internal error in tetcomplex21fliponedge():\n");
      printf("  Triangle is missing from the edge's star.\n");
      internalerror();
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexdeletetriangle()   Delete a triangle from a tetcomplex, gluing */
/*                               the two adjoining tetrahedra (or one        */
/*                               tetrahedron and one polyhedron) together    */
/*                               into a single polyhedron.                   */
/*                                                                           */
/*  This is a special-purpose procedure designed to help ease and speed up   */
/*  the Bowyer-Watson algorithm.  It should not normally be used by other    */
/*  algorithms.                                                              */
/*                                                                           */
/*  To understand this procedure, it's useful to think of each edge's link   */
/*  ring not as a bunch of tetrahedra encircling an edge, but as a bunch of  */
/*  triangles encircling an edge, acting as boundaries in a polyhedral       */
/*  complex.  The Blandford et al. data structure can represent arbitrary    */
/*  polyhedral complexes (with all faces triangulated)--although to support  */
/*  efficient adjacency queries, you would need to store all the edges (not  */
/*  just half the edges, as Blandford et al. do).                            */
/*                                                                           */
/*  This procedure deletes a triangle (vtx1, vtx2, vtx3) from the complex,   */
/*  thereby uniting the two polyhedra adjoining the face into a single       */
/*  polyhedron.  It is used by the Bowyer-Watson implementation to unite all */
/*  the tetrahedra that are no longer Delaunay into one polyhedral cavity,   */
/*  which is subsequently filled with new tetrahedra.                        */
/*                                                                           */
/*  plex:  The tetcomplex to delete a triangle from.                         */
/*  vtx1, vtx2, vtx3:  The tags for the vertices of the triangle to delete.  */
/*                                                                           */
/*****************************************************************************/

void tetcomplexdeletetriangle(struct tetcomplex *plex,
                              tag vtx1,
                              tag vtx2,
                              tag vtx3)
{
  if (plex->verbosity > 3) {
    printf("        Deleting triangle w/tags %lu %lu %lu.\n",
           (unsigned long) vtx1, (unsigned long) vtx2, (unsigned long) vtx3);
  }

  tetcomplex21fliponedge(plex, vtx1, vtx2, vtx3);
  tetcomplex21fliponedge(plex, vtx2, vtx3, vtx1);
  tetcomplex21fliponedge(plex, vtx3, vtx1, vtx2);
}

void tetcomplexsqueezeonhalfedge(struct tetcomplex *plex,
                                 tag vtx1,
                                 tag vtx2,
                                 tag vtx3)
{
  struct tetcomplexstar *star;
  tag ring;
  int result;

  if (vtx1 < plex->nextinitindex) {
    star = (struct tetcomplexstar *)
           arraypoolfastlookup(&plex->stars, (arraypoolulong) vtx1);
    ring = link2dfindring(&plex->moleculepool, plex->cache, star->linkhead,
                          vtx1, vtx2);
    result = linkringdelete2vertices(&plex->moleculepool, ring, vtx1, vtx3);
    if ((result & 2) != 0) {
      linkringdelete(&plex->moleculepool, ring);
      link2ddeletevertex(&plex->moleculepool, plex->cache, star->linkhead,
                         &star->linktail, vtx1, vtx2);
    }
    if (result < 1) {
      printf("Oops.\n");
    }
  } else {
    printf("Oops.\n");
  }
}

void tetcomplexsqueezeonedge(struct tetcomplex *plex,
                             tag vtx1,
                             tag vtx2,
                             tag vtx3)
{
  if ((vtx1 & 1) == (vtx2 & 1)) {
    if ((vtx2 == GHOSTVERTEX) ||
        ((vtx1 != GHOSTVERTEX) &&
         ((vtx1 < vtx2) ^ ((vtx1 & 2) == (vtx2 & 2))))) {
      tetcomplexsqueezeonhalfedge(plex, vtx1, vtx2, vtx3);
    } else {
      tetcomplexsqueezeonhalfedge(plex, vtx2, vtx1, vtx3);
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexsqueezetriangle()   Delete a triangle from a tetcomplex, in    */
/*                                the case where both tetrahedra adjoining   */
/*                                the triangle have the same apex.           */
/*                                                                           */
/*  This is a special-purpose procedure designed to help ease and speed up   */
/*  the Bowyer-Watson algorithm.  It should not normally be used by other    */
/*  algorithms.                                                              */
/*                                                                           */
/*  To understand this procedure, it's useful to think of each edge's link   */
/*  ring not as a bunch of tetrahedra encircling an edge, but as a bunch of  */
/*  triangles encircling an edge, acting as boundaries in a polyhedral       */
/*  complex.  The Blandford et al. data structure can represent arbitrary    */
/*  polyhedral complexes (with all faces triangulated)--although to support  */
/*  efficient adjacency queries, you would need to store all the edges (not  */
/*  just half the edges, as Blandford et al. do).                            */
/*                                                                           */
/*  This procedure deletes a triangle (vtx1, vtx2, vtx3) from the complex,   */
/*  in the special case where both tetrahedra, on each side of it, have      */
/*  already been "carved out" from the Bowyer-Watson cavity.  In this case,  */
/*  the triangle itself is not Delaunay and must be deleted also.  However,  */
/*  it is in a transient state where the newly inserted vertex appears on    */
/*  both sides of the triangle.  So when the triangle is deleted, one of     */
/*  the duplicate copies of the new vertex must also be deleted.             */
/*                                                                           */
/*  One or more edges of the triangle may be deleted (with their link        */
/*  rings).  This happens when the new vertex is the only vertex that would  */
/*  be left in the link ring.                                                */
/*                                                                           */
/*  plex:  The tetcomplex to delete a triangle from.                         */
/*  vtx1, vtx2, vtx3:  The tags for the vertices of the triangle to delete.  */
/*                                                                           */
/*****************************************************************************/

void tetcomplexsqueezetriangle(struct tetcomplex *plex,
                               tag vtx1,
                               tag vtx2,
                               tag vtx3)
{
  if (plex->verbosity > 3) {
    printf("        Squeezing triangle w/tags %lu %lu %lu.\n",
           (unsigned long) vtx1, (unsigned long) vtx2, (unsigned long) vtx3);
  }

  tetcomplexsqueezeonedge(plex, vtx1, vtx2, vtx3);
  tetcomplexsqueezeonedge(plex, vtx2, vtx3, vtx1);
  tetcomplexsqueezeonedge(plex, vtx3, vtx1, vtx2);
}

void tetcomplex12flipon3edges(struct tetcomplex *plex,
                              tag vtx1,
                              tag vtx2,
                              tag vtx3,
                              tag vtx4,
                              tag newvertex)
{
  if (plex->verbosity > 3) {
    printf("        Inserting vertex into stars of 3 edges:\n");
    printf("          Tags %lu %lu %lu %lu; new vertex %lu.\n",
           (unsigned long) vtx1, (unsigned long) vtx2, (unsigned long) vtx3,
           (unsigned long) vtx4, (unsigned long) newvertex);
  }

  tetcomplex12fliponedge(plex, vtx1, vtx2, vtx3, vtx4, newvertex);
  tetcomplex12fliponedge(plex, vtx1, vtx3, vtx4, vtx2, newvertex);
  tetcomplex12fliponedge(plex, vtx1, vtx4, vtx2, vtx3, newvertex);
  if ((vtx1 == GHOSTVERTEX) || (vtx2 == GHOSTVERTEX) ||
      (vtx3 == GHOSTVERTEX) || (vtx4 == GHOSTVERTEX)) {
    /* A ghost tetrahedron was absorbed into the cavity. */
    plex->ghosttetcount--;
  } else {
    /* A solid tetrahedron was absorbed into the cavity. */
    plex->tetcount--;
  }
}

void tetcomplex12flipon6edges(struct tetcomplex *plex,
                              tag vtx1,
                              tag vtx2,
                              tag vtx3,
                              tag vtx4,
                              tag newvertex)
{
  if (plex->verbosity > 3) {
    printf("        Inserting vertex into stars of 6 edges (tetrahedron):\n");
    printf("          Tags %lu %lu %lu %lu; new vertex %lu.\n",
           (unsigned long) vtx1, (unsigned long) vtx2, (unsigned long) vtx3,
           (unsigned long) vtx4, (unsigned long) newvertex);
  }

  tetcomplex12fliponedge(plex, vtx1, vtx2, vtx3, vtx4, newvertex);
  tetcomplex12fliponedge(plex, vtx1, vtx3, vtx4, vtx2, newvertex);
  tetcomplex12fliponedge(plex, vtx1, vtx4, vtx2, vtx3, newvertex);
  tetcomplex12fliponedge(plex, vtx2, vtx3, vtx1, vtx4, newvertex);
  tetcomplex12fliponedge(plex, vtx2, vtx4, vtx3, vtx1, newvertex);
  tetcomplex12fliponedge(plex, vtx3, vtx4, vtx1, vtx2, newvertex);
  if ((vtx4 == GHOSTVERTEX) || (vtx3 == GHOSTVERTEX) ||
      (vtx2 == GHOSTVERTEX) || (vtx1 == GHOSTVERTEX)) {
    /* The cavity start with the deletion of a ghost tetrahedron. */
    plex->ghosttetcount--;
  } else {
    /* The cavity start with the deletion of a solid tetrahedron. */
    plex->tetcount--;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplex14flip()   Replace one tetrahedron with four (bistellar flip). */
/*                                                                           */
/*  The tetrahedron (vtx1, vtx2, vtx3, vtx4) is replaced by the tetrahedra   */
/*  (newvertex, vtx2, vtx3, vtx4), (vtx1, newvertex, vtx3, vtx4),            */
/*  (vtx1, vtx2, newvertex, vtx4), and (vtx1, vtx2, vtx3, newvertex).        */
/*  Equivalently, `newvertex' is lazily inserted in the tetrahedron.         */
/*                                                                           */
/*  This procedure is equivalent to                                          */
/*                                                                           */
/*    tetcomplexdeletetet(plex, vtx1, vtx2, vtx3, vtx4);                     */
/*    tetcomplexinserttet(plex, newvertex, vtx2, vtx3, vtx4);                */
/*    tetcomplexinserttet(plex, vtx1, newvertex, vtx3, vtx4);                */
/*    tetcomplexinserttet(plex, vtx1, vtx2, newvertex, vtx4);                */
/*    tetcomplexinserttet(plex, vtx1, vtx2, vtx3, newvertex);                */
/*                                                                           */
/*  only faster, and with less error checking.                               */
/*                                                                           */
/*  This procedure is the inverse of tetcomplex41flip().                     */
/*                                                                           */
/*  WARNING:  The tetcomplex `plex' must not contain any tetrahedra that     */
/*  have `newvertex' for a vertex.  (In other words, the star of `newvertex' */
/*  must be empty before the call.)  This procedure does not check whether   */
/*  the complex actually contains the tetrahedron (vtx1, vtx2, vtx3, vtx4),  */
/*  and may fail catastrophically if it does not.                            */
/*                                                                           */
/*  plex:  The tetcomplex containing the original tetrahedron.               */
/*  vtx1, vtx2, vtx3, vtx4:  The tags for the vertices of the tetrahedron to */
/*    flip.                                                                  */
/*  newvertex:  The tag for the new vertex to insert.                        */
/*                                                                           */
/*****************************************************************************/

void tetcomplex14flip(struct tetcomplex *plex,
                      tag vtx1,
                      tag vtx2,
                      tag vtx3,
                      tag vtx4,
                      tag newvertex)
{
  tag vtx[4];
  tag tagarray[3];
  struct tetcomplexstar *star;
  proxipoolulong allocindex;
  tag vtxowner, vtxowned;
  tag ring;
  int i;

  if (plex->verbosity > 2) {
    printf("      Transforming one tetrahedron to four (vertex insertion).\n");
    if (plex->verbosity > 3) {
      printf("        Tags %lu %lu %lu %lu; new vertex tag %lu.\n",
             (unsigned long) vtx1, (unsigned long) vtx2, (unsigned long) vtx3,
             (unsigned long) vtx4, (unsigned long) newvertex);
    }
  }

#ifdef SELF_CHECK
  if ((newvertex == vtx1) || (newvertex == vtx2) ||
      (newvertex == vtx3) || (newvertex == vtx4) ||
      (vtx1 == vtx2) || (vtx1 == vtx3) || (vtx1 == vtx4) ||
      (vtx2 == vtx3) || (vtx2 == vtx4) || (vtx3 == vtx4)) {
    printf("Internal error in tetcomplex14flip():\n");
    printf("  Asked to perform a flip with two identical vertices.\n");
    internalerror();
  }

  if (newvertex < plex->nextinitindex) {
    /* Look up the star of vertex `newvertex'. */
    star = tetcomplexlookup3dstar(plex, newvertex);
    if (!link2disempty(&plex->moleculepool, star->linkhead)) {
      printf("Internal error in tetcomplex14flip():\n");
      printf("  New vertex's star is not empty before the flip.\n");
      internalerror();
    }
  }
#endif /* SELF_CHECK */

  /* Insert six new triangles in the stars of the six edges of the original */
  /*   tetrahedron.                                                         */
  tetcomplex12fliponedge(plex, vtx1, vtx2, vtx3, vtx4, newvertex);
  tetcomplex12fliponedge(plex, vtx1, vtx3, vtx4, vtx2, newvertex);
  tetcomplex12fliponedge(plex, vtx1, vtx4, vtx2, vtx3, newvertex);
  tetcomplex12fliponedge(plex, vtx2, vtx3, vtx1, vtx4, newvertex);
  tetcomplex12fliponedge(plex, vtx2, vtx4, vtx3, vtx1, newvertex);
  tetcomplex12fliponedge(plex, vtx3, vtx4, vtx1, vtx2, newvertex);

  /* Create the four new edges and their link rings. */
  vtx[0] = vtx1;
  vtx[1] = vtx2;
  vtx[2] = vtx3;
  vtx[3] = vtx4;
  for (i = 0; i < 4; i++) {
    /* Does the edge (newvertex, vtx[i]) store a link ring?  (Parity check.) */
    if ((newvertex & 1) == (vtx[i] & 1)) {
      /* Yes.  Which of the two vertices is responsible for storing it? */
      if ((vtx[i] == GHOSTVERTEX) ||
          ((newvertex != GHOSTVERTEX) &&
           ((newvertex < vtx[i]) ^ ((newvertex & 2) == (vtx[i] & 2))))) {
        /* `newvertex' stores a new link ring containing three tetrahedra. */
        vtxowner = newvertex;
        vtxowned = vtx[i];
        tagarray[0] = vtx[i ^ 3];
        tagarray[1] = vtx[i ^ 2];
        tagarray[2] = vtx[i ^ 1];
      } else {
        /* `vtx[i]' stores a new link ring containing three tetrahedra.     */
        vtxowner = vtx[i];
        vtxowned = newvertex;
        tagarray[0] = vtx[i ^ 1];
        tagarray[1] = vtx[i ^ 2];
        tagarray[2] = vtx[i ^ 3];
      }

      /* Create the new link ring for edge (vtxowner, vtxowned), using the */
      /*   same allocation index that was used to allocate `vtxowner'.     */
      if (plex->vertexpool == (struct proxipool *) NULL) {
        allocindex = 0;
      } else {
        allocindex = proxipooltag2allocindex(plex->vertexpool, vtxowner);
      }
      ring = linkringnewfill(&plex->moleculepool, vtxowner, tagarray,
                             (proxipoolulong) 3, allocindex);
      /* Make sure `vtxowner' has a 2D link, and find it. */
      star = tetcomplexlookup3dstar(plex, vtxowner);
      /* Store the link ring in 'vtxowner's 2D link. */
      link2dinsertvertex(&plex->moleculepool, plex->cache, star->linkhead,
                         &star->linktail, vtxowner, vtxowned, ring);
    }
  }

  if (newvertex == GHOSTVERTEX) {
    /* The flip replaces one solid tetrahedron with four ghost tetrahedra. */
    plex->tetcount--;
    plex->ghosttetcount += 4;
  } else if ((vtx1 == GHOSTVERTEX) || (vtx2 == GHOSTVERTEX) ||
             (vtx3 == GHOSTVERTEX) || (vtx4 == GHOSTVERTEX)) {
    /* The flip replaces one ghost tetrahedron with one solid tetrahedron */
    /*   and three ghost tetrahedra.                                      */
    plex->tetcount++;
    plex->ghosttetcount += 2;
  } else {
    /* The flip replaces one solid tetrahedron with four. */
    plex->tetcount += 3;
  }

  /*^^^ Assertion:  The tetrahedral complex is now consistent. */
#ifdef PARANOID
  tetcomplexconsistency(plex);
#endif /* PARANOID */
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplex23flip()   Replace two tetrahedra with three (bistellar flip). */
/*                                                                           */
/*  The tetrahedra (vtxtop, vtx1, vtx2, vtx3) and (vtx1, vtx2, vtx3, vtxbot) */
/*  are replaced by the tetrahedra (vtx1, vtx2, vtxtop, vtxbot),             */
/*  (vtx2, vtx3, vtxtop, vtxbot), and (vtx3, vtx1, vtxtop, vtxbot).  This    */
/*  creates one new edge, (vtxtop, vtxbot).  It also deletes one triangle,   */
/*  (vtx1, vtx2, vtx3), and replaces it with three new triangles, all        */
/*  adjoining the new edge.                                                  */
/*                                                                           */
/*  This procedure is equivalent to                                          */
/*                                                                           */
/*    tetcomplexdeletetet(plex, vtxtop, vtx1, vtx2, vtx3);                   */
/*    tetcomplexdeletetet(plex, vtx1, vtx2, vtx3, vtxbot);                   */
/*    tetcomplexinserttet(plex, vtxtop, vtx1, vtx2, vtxbot);                 */
/*    tetcomplexinserttet(plex, vtxtop, vtx2, vtx3, vtxbot);                 */
/*    tetcomplexinserttet(plex, vtxtop, vtx3, vtx1, vtxbot);                 */
/*                                                                           */
/*  only faster, and with less error checking.                               */
/*                                                                           */
/*  This procedure is the inverse of tetcomplex32flip().                     */
/*                                                                           */
/*  WARNING:  This procedure does not check whether the complex actually     */
/*  contains either of the tetrahedra to be deleted, and may fail            */
/*  catastrophically if it does not.                                         */
/*                                                                           */
/*  plex:  The tetcomplex containing the original tetrahedra.                */
/*  vtx1, vtx2, vtx3:  The tags for the vertices of the deleted triangle.    */
/*  vtxbot, vtxtop:  The tags for the vertices of the newly created edge.    */
/*                                                                           */
/*****************************************************************************/

void tetcomplex23flip(struct tetcomplex *plex,
                      tag vtxtop,
                      tag vtx1,
                      tag vtx2,
                      tag vtx3,
                      tag vtxbot)
{
  tag tagarray[3];
  struct tetcomplexstar *star;
  proxipoolulong allocindex;
  tag vtxowner, vtxowned;
  tag ring;

  if (plex->verbosity > 2) {
    printf("      Transforming two tetrahedra to three.\n");
    if (plex->verbosity > 3) {
      printf("        Tags %lu %lu %lu / %lu %lu.\n",
             (unsigned long) vtx1, (unsigned long) vtx2, (unsigned long) vtx3,
             (unsigned long) vtxbot, (unsigned long) vtxtop);
    }
  }

#ifdef SELF_CHECK
  if ((vtx1 == vtx2) || (vtx2 == vtx3) || (vtx3 == vtx1) ||
      (vtx1 == vtxbot) || (vtx2 == vtxbot) || (vtx3 == vtxbot) ||
      (vtx1 == vtxtop) || (vtx2 == vtxtop) || (vtx3 == vtxtop) ||
      (vtxbot == vtxtop)) {
    printf("Internal error in tetcomplex23flip():\n");
    printf("  Asked to perform a flip with two identical vertices.\n");
    internalerror();
  }
#endif /* SELF_CHECK */

  /* Delete the triangle (vtx1, vtx2, vtx3) from the stars of all three */
  /*   of its edges.                                                    */
  tetcomplex21fliponedge(plex, vtx1, vtx2, vtx3);
  tetcomplex21fliponedge(plex, vtx2, vtx3, vtx1);
  tetcomplex21fliponedge(plex, vtx3, vtx1, vtx2);

  /* Insert the triangles (vtxtop, vtx1, vtxbot), (vtxtop, vtx2, vtxbot),  */
  /*   (vtxtop, vtx3, vtxbot) into the stars of their edges, excepting the */
  /*   edge (vtxtop, vtxbot), which we must create anew.                   */
  tetcomplex12fliponedge(plex, vtxtop, vtx1, vtx2, vtx3, vtxbot);
  tetcomplex12fliponedge(plex, vtxtop, vtx2, vtx3, vtx1, vtxbot);
  tetcomplex12fliponedge(plex, vtxtop, vtx3, vtx1, vtx2, vtxbot);
  tetcomplex12fliponedge(plex, vtxbot, vtx1, vtx3, vtx2, vtxtop);
  tetcomplex12fliponedge(plex, vtxbot, vtx2, vtx1, vtx3, vtxtop);
  tetcomplex12fliponedge(plex, vtxbot, vtx3, vtx2, vtx1, vtxtop);

  /* Does the edge (vtxtop, vtxbot) store a link ring?  (Parity check.) */
  if ((vtxtop & 1) == (vtxbot & 1)) {
    /* Yes.  Which of the two vertices is responsible for storing it? */
    if ((vtxbot == GHOSTVERTEX) ||
        ((vtxtop != GHOSTVERTEX) &&
         ((vtxtop < vtxbot) ^ ((vtxtop & 2) == (vtxbot & 2))))) {
      /* `vtxtop' stores a new link ring containing three new tetrahedra. */
      vtxowner = vtxtop;
      vtxowned = vtxbot;
      tagarray[0] = vtx1;
      tagarray[1] = vtx2;
      tagarray[2] = vtx3;
    } else {
      /* `vtxbot' stores a new link ring containing three new tetrahedra. */
      vtxowner = vtxbot;
      vtxowned = vtxtop;
      tagarray[0] = vtx3;
      tagarray[1] = vtx2;
      tagarray[2] = vtx1;
    }

    /* Create the new link ring for edge (vtxowner, vtxowned), using the */
    /*   same allocation index that was used to allocate `vtxowner'.     */
    if (plex->vertexpool == (struct proxipool *) NULL) {
      allocindex = 0;
    } else {
      allocindex = proxipooltag2allocindex(plex->vertexpool, vtxowner);
    }
    ring = linkringnewfill(&plex->moleculepool, vtxowner, tagarray,
                           (proxipoolulong) 3, allocindex);
    /* Make sure `vtxowner' has a 2D link, and find it. */
    star = tetcomplexlookup3dstar(plex, vtxowner);
    /* Store the link ring in 'vtxowner's 2D link. */
    link2dinsertvertex(&plex->moleculepool, plex->cache, star->linkhead,
                       &star->linktail, vtxowner, vtxowned, ring);
  }

  if ((vtxtop == GHOSTVERTEX) || (vtxbot == GHOSTVERTEX)) {
    /* The flip replaces one solid tetrahedron and one ghost tetrahedron */
    /*   with three ghost tetrahedra.                                    */
    plex->tetcount--;
    plex->ghosttetcount += 2;
  } else {
    /* Either the flip replaces two solid tetrahedra with three, or it     */
    /*   replaces two ghost tetrahedra with two different ghost tetrahedra */
    /*   and one solid tetrahedron.                                        */
    plex->tetcount++;
  }

  /*^^^ Assertion:  The tetrahedral complex is now consistent. */
#ifdef PARANOID
  tetcomplexconsistency(plex);
#endif /* PARANOID */
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplex32flip()   Replace three tetrahedra with two (bistellar flip). */
/*                                                                           */
/*  The tetrahedra (vtx1, vtx2, vtxtop, vtxbot), (vtx2, vtx3, vtxtop,        */
/*  vtxbot), and (vtx3, vtx1, vtxtop, vtxbot) are replaced by the tetrahedra */
/*  (vtxtop, vtx1, vtx2, vtx3) and (vtx1, vtx2, vtx3, vtxbot).  This deletes */
/*  one edge, (vtxtop, vtxbot).  It also deletes three triangles adjoining   */
/*  that edge, and replaces them with a new triangle, (vtx1, vtx2, vtx3).    */
/*                                                                           */
/*  This procedure is equivalent to                                          */
/*                                                                           */
/*    tetcomplexdeletetet(plex, vtxtop, vtx1, vtx2, vtxbot);                 */
/*    tetcomplexdeletetet(plex, vtxtop, vtx2, vtx3, vtxbot);                 */
/*    tetcomplexdeletetet(plex, vtxtop, vtx3, vtx1, vtxbot);                 */
/*    tetcomplexinserttet(plex, vtxtop, vtx1, vtx2, vtx3);                   */
/*    tetcomplexinserttet(plex, vtx1, vtx2, vtx3, vtxbot);                   */
/*                                                                           */
/*  only faster, and with less error checking.                               */
/*                                                                           */
/*  This procedure is the inverse of tetcomplex23flip().                     */
/*                                                                           */
/*  WARNING:  This procedure does not check whether the complex actually     */
/*  contains any of the tetrahedra to be deleted, and may fail               */
/*  catastrophically if it does not.                                         */
/*                                                                           */
/*  plex:  The tetcomplex containing the original tetrahedra.                */
/*  vtx1, vtx2, vtx3:  The tags for the vertices of the new triangle.        */
/*  vtxbot, vtxtop:  The tags for the vertices of the deleted edge.          */
/*                                                                           */
/*****************************************************************************/

void tetcomplex32flip(struct tetcomplex *plex,
                      tag vtxtop,
                      tag vtx1,
                      tag vtx2,
                      tag vtx3,
                      tag vtxbot)
{
  struct tetcomplexstar *star;
  tag ring;

  if (plex->verbosity > 2) {
    printf("      Transforming three tetrahedra to two.\n");
    if (plex->verbosity > 3) {
      printf("        Tags %lu %lu %lu / %lu %lu.\n",
             (unsigned long) vtx1, (unsigned long) vtx2, (unsigned long) vtx3,
             (unsigned long) vtxbot, (unsigned long) vtxtop);
    }
  }

#ifdef SELF_CHECK
  if ((vtx1 == vtx2) || (vtx2 == vtx3) || (vtx3 == vtx1) ||
      (vtx1 == vtxbot) || (vtx2 == vtxbot) || (vtx3 == vtxbot) ||
      (vtx1 == vtxtop) || (vtx2 == vtxtop) || (vtx3 == vtxtop) ||
      (vtxbot == vtxtop)) {
    printf("Internal error in tetcomplex32flip():\n");
    printf("  Asked to perform a flip with two identical vertices.\n");
    internalerror();
  }
#endif /* SELF_CHECK */

  /* Delete the triangles (vtxtop, vtx1, vtxbot), (vtxtop, vtx2, vtxbot),  */
  /*   (vtxtop, vtx3, vtxbot) from the stars of their edges, excepting the */
  /*   edge (vtxtop, vtxbot), which we shall delete in one blow.           */
  tetcomplex21fliponedge(plex, vtxtop, vtx1, vtxbot);
  tetcomplex21fliponedge(plex, vtxtop, vtx2, vtxbot);
  tetcomplex21fliponedge(plex, vtxtop, vtx3, vtxbot);
  tetcomplex21fliponedge(plex, vtxbot, vtx1, vtxtop);
  tetcomplex21fliponedge(plex, vtxbot, vtx2, vtxtop);
  tetcomplex21fliponedge(plex, vtxbot, vtx3, vtxtop);

  /* Insert the triangle (vtx1, vtx2, vtx3) into the stars of all three */
  /*   of its edges.                                                    */
  tetcomplex12fliponedge(plex, vtx1, vtx2, vtxtop, vtxbot, vtx3);
  tetcomplex12fliponedge(plex, vtx2, vtx3, vtxtop, vtxbot, vtx1);
  tetcomplex12fliponedge(plex, vtx3, vtx1, vtxtop, vtxbot, vtx2);

  /* Does the edge (vtxtop, vtxbot) store a link ring?  (Parity check.) */
  if ((vtxtop & 1) == (vtxbot & 1)) {
    /* Yes.  Which of the two vertices is responsible for storing it? */
    if ((vtxbot == GHOSTVERTEX) ||
        ((vtxtop != GHOSTVERTEX) &&
         ((vtxtop < vtxbot) ^ ((vtxtop & 2) == (vtxbot & 2))))) {
      if (vtxtop >= plex->nextinitindex) {
        printf("Internal error in tetcomplex32flip():\n");
        printf("  Central edge (to be deleted by a 3-2 flip) is missing.\n");
        internalerror();
      }

      /* Delete edge (vtxtop, vtxbot) and its link ring from `vtxtop's star. */
      star = (struct tetcomplexstar *)
             arraypoolfastlookup(&plex->stars, (arraypoolulong) vtxtop);
      ring = link2ddeletevertex(&plex->moleculepool, plex->cache,
                                star->linkhead, &star->linktail,
                                vtxtop, vtxbot);
    } else {
      if (vtxbot >= plex->nextinitindex) {
        printf("Internal error in tetcomplex32flip():\n");
        printf("  Central edge (to be deleted by a 3-2 flip) is missing.\n");
        internalerror();
      }

      /* Delete edge (vtxtop, vtxbot) and its link ring from `vtxbot's star. */
      star = (struct tetcomplexstar *)
             arraypoolfastlookup(&plex->stars, (arraypoolulong) vtxbot);
      ring = link2ddeletevertex(&plex->moleculepool, plex->cache,
                                star->linkhead, &star->linktail,
                                vtxbot, vtxtop);
    }
    if (ring == STOP) {
      printf("Internal error in tetcomplex32flip():\n");
      printf("  Central edge (to be deleted by a 3-2 flip) is missing.\n");
      internalerror();
    } else {
      linkringdelete(&plex->moleculepool, ring);
    }
  }

  if ((vtxtop == GHOSTVERTEX) || (vtxbot == GHOSTVERTEX)) {
    /* The flip replaces three ghost tetrahedra with one solid tetrahedron */
    /*   and one ghost tetrahedron.                                        */
    plex->tetcount++;
    plex->ghosttetcount -= 2;
  } else {
    /* Either the flip replaces three solid tetrahedra with two, or it    */
    /*   replaces two ghost tetrahedra and one solid tetrahedron with two */
    /*   different ghost tetrahedra.                                      */
    plex->tetcount--;
  }

  /*^^^ Assertion:  The tetrahedral complex is now consistent. */
#ifdef PARANOID
  tetcomplexconsistency(plex);
#endif /* PARANOID */
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplex41flip()   Replace four tetrahedra with one (bistellar flip).  */
/*                                                                           */
/*  The tetrahedra (newvertex, vtx2, vtx3, vtx4), (vtx1, newvertex, vtx3,    */
/*  vtx4), (vtx1, vtx2, newvertex, vtx4), and (vtx1, vtx2, vtx3, newvertex)  */
/*  are replaced by the tetrahedron (vtx1, vtx2, vtx3, vtx4).  Equivalently, */
/*  `deletevertex' is deleted from inside a tetrahedron.                     */
/*                                                                           */
/*  This procedure is equivalent to                                          */
/*                                                                           */
/*    tetcomplexinserttet(plex, deletevertex, vtx2, vtx3, vtx4);             */
/*    tetcomplexinserttet(plex, vtx1, deletevertex, vtx3, vtx4);             */
/*    tetcomplexinserttet(plex, vtx1, vtx2, deletevertex, vtx4);             */
/*    tetcomplexinserttet(plex, vtx1, vtx2, vtx3, deletevertex);             */
/*    tetcomplexdeletetet(plex, vtx1, vtx2, vtx3, vtx4);                     */
/*                                                                           */
/*  only faster, and with less error checking.                               */
/*                                                                           */
/*  This procedure is the inverse of tetcomplex14flip().                     */
/*                                                                           */
/*  WARNING:  This procedure does not check whether the complex actually     */
/*  contains any of the tetrahedra to be deleted, and may fail               */
/*  catastrophically if it does not.                                         */
/*                                                                           */
/*  plex:  The tetcomplex containing the original tetrahedra.                */
/*  vtx1, vtx2, vtx3, vtx4:  The tags for the vertices of the tetrahedron    */
/*    created by the flip.                                                   */
/*  deletevertex:  The tag for the vertex to delete.                         */
/*                                                                           */
/*****************************************************************************/

void tetcomplex41flip(struct tetcomplex *plex,
                      tag vtx1,
                      tag vtx2,
                      tag vtx3,
                      tag vtx4,
                      tag deletevertex)
{
  struct tetcomplexstar *star;
  tag ring;
  tag vtxi;
  int i;

  if (plex->verbosity > 2) {
    printf("      Transforming four tetrahedra to one (vertex deletion).\n");
    if (plex->verbosity > 3) {
      printf("        Tags %lu %lu %lu %lu; deleted vertex tag %lu.\n",
             (unsigned long) vtx1, (unsigned long) vtx2, (unsigned long) vtx3,
             (unsigned long) vtx4, (unsigned long) deletevertex);
    }
  }

#ifdef SELF_CHECK
  if ((deletevertex == vtx1) || (deletevertex == vtx2) ||
      (deletevertex == vtx3) || (deletevertex == vtx4) ||
      (vtx1 == vtx2) || (vtx1 == vtx3) || (vtx1 == vtx4) ||
      (vtx2 == vtx3) || (vtx2 == vtx4) || (vtx3 == vtx4)) {
    printf("Internal error in tetcomplex41flip():\n");
    printf("  Asked to perform a flip with two identical vertices.\n");
    internalerror();
  }
#endif /* SELF_CHECK */

  /* Delete the triangles adjoining `deletevertex' from the stars of the */
  /*   surviving edges.                                                  */
  tetcomplex21fliponedge(plex, vtx1, vtx2, deletevertex);
  tetcomplex21fliponedge(plex, vtx1, vtx3, deletevertex);
  tetcomplex21fliponedge(plex, vtx1, vtx4, deletevertex);
  tetcomplex21fliponedge(plex, vtx2, vtx3, deletevertex);
  tetcomplex21fliponedge(plex, vtx2, vtx4, deletevertex);
  tetcomplex21fliponedge(plex, vtx3, vtx4, deletevertex);

  /* Delete the four edges adjoining `deletevertex' and their link rings. */
  for (i = 1; i <= 4; i++) {
    vtxi = (i <= 2) ? ((i == 1) ? vtx1 : vtx2) : ((i == 3) ? vtx3 : vtx4);
    /* Does the edge (newvertex, vtxi) store a link ring?  (Parity check.) */
    if ((deletevertex & 1) == (vtxi & 1)) {
      /* Yes.  Which of the two vertices is responsible for storing it? */
      ring = STOP;
      if ((vtxi == GHOSTVERTEX) ||
          ((deletevertex != GHOSTVERTEX) &&
           ((deletevertex < vtxi) ^ ((deletevertex & 2) == (vtxi & 2))))) {
        /* `deletevertex' stores the link ring.  Delete the ring. */
        if (deletevertex < plex->nextinitindex) {
          star = (struct tetcomplexstar *)
                 arraypoolfastlookup(&plex->stars,
                                     (arraypoolulong) deletevertex);
          ring = link2ddeletevertex(&plex->moleculepool, plex->cache,
                                    star->linkhead, &star->linktail,
                                    deletevertex, vtxi);
        }
      } else {
        /* `vtxi' stores the link ring.  Delete the ring. */
        if (vtxi < plex->nextinitindex) {
          star = (struct tetcomplexstar *)
                 arraypoolfastlookup(&plex->stars, (arraypoolulong) vtxi);
          ring = link2ddeletevertex(&plex->moleculepool, plex->cache,
                                    star->linkhead, &star->linktail,
                                    vtxi, deletevertex);
        }
      }

      if (ring == STOP) {
        printf("Internal error in tetcomplex41flip():\n");
        printf("  An edge to be deleted by a 4-1 flip is missing.\n");
        internalerror();
      } else {
        linkringdelete(&plex->moleculepool, ring);
      }
    }
  }

  if (deletevertex == GHOSTVERTEX) {
    /* The flip replaces four ghost tetrahedra with one solid tetrahedron. */
    plex->tetcount++;
    plex->ghosttetcount -= 4;
  } else if ((vtx1 == GHOSTVERTEX) || (vtx2 == GHOSTVERTEX) ||
             (vtx3 == GHOSTVERTEX) || (vtx4 == GHOSTVERTEX)) {
    /* The flip replaces one solid tetrahedron and three ghost tetrahedra */
    /*   with one ghost tetrahedron.                                      */
    plex->tetcount--;
    plex->ghosttetcount -= 2;
  } else {
    /* The flip replaces four solid tetrahedra with one. */
    plex->tetcount -= 3;
  }

  /*^^^ Assertion:  The tetrahedral complex is now consistent. */
#ifdef PARANOID
  tetcomplexconsistency(plex);
#endif /* PARANOID */
}

void tetcomplexbuild3dstar(struct tetcomplex *plex,
                           tag newvertex,
                           struct arraypool *trianglelist,
                           arraypoolulong firstindex,
                           arraypoolulong trianglecount)
{
  struct tetcomplexstar *star;
  struct tetcomplexlinktriangle *triangle, *traveltri;
  tag tagarray[512];
  tag origin;
  tag vtx1, vtx2;
  tag ring;
  proxipoolulong allocindex;
  proxipoolulong tagindex;
  proxipoolulong i;
  arraypoolulong triindex;
  arraypoolulong travelindex;
  arraypoolulong orgneighbor;
  int edge;
  int traveledge;

  for (triindex = firstindex; triindex < firstindex + trianglecount;
       triindex++) {
    triangle = (struct tetcomplexlinktriangle *)
               arraypoolfastlookup(trianglelist, triindex);
    for (edge = 0; edge < 3; edge++) {
      origin = triangle->vtx[(edge == 2) ? 0 : edge + 1];
      if ((origin & 1) == (newvertex & 1)) {
        orgneighbor = triangle->neighbor[edge];
        if (orgneighbor < ~orgneighbor) {
          if ((origin == GHOSTVERTEX) ||
              ((newvertex != GHOSTVERTEX) &&
               ((newvertex < origin) ^ ((newvertex & 2) == (origin & 2))))) {
            vtx1 = newvertex;
            vtx2 = origin;
          } else {
            vtx1 = origin;
            vtx2 = newvertex;
          }

          tagindex = 0;
          traveledge = edge;
          traveltri = triangle;
          do {
            tagarray[tagindex] = traveltri->vtx[traveledge];
            tagindex++;
            if (vtx1 == newvertex) {
              traveltri->neighbor[traveledge] =
                ~traveltri->neighbor[traveledge];
              travelindex = traveltri->neighbor[(traveledge == 0) ?
                                                2 : traveledge - 1];
              travelindex = (travelindex < ~travelindex) ?
                            travelindex : ~travelindex;
              traveledge = travelindex & 3;
            } else {
              travelindex = traveltri->neighbor[traveledge];
              traveltri->neighbor[traveledge] = ~travelindex;
              traveledge = travelindex & 3;
              traveledge = (traveledge == 2) ? 0 : traveledge + 1;
            }
            traveltri = (struct tetcomplexlinktriangle *)
                        arraypoolfastlookup(trianglelist, travelindex >> 2);
          } while (traveltri != triangle);

          if (plex->verbosity > 3) {
            printf("        Creating link ring for edge w/tags %lu %lu:\n"
                   "         ",
                   (unsigned long) vtx1, (unsigned long) vtx2);
            for (i = 0; i < tagindex; i++) {
              printf(" %lu", (unsigned long) tagarray[i]);
            }
            printf("\n");
          }

          /* Create the new link ring for edge (vtx1, vtx2), using the */
          /*   same allocation index that was used to allocate `vtx1'. */
          if (plex->vertexpool == (struct proxipool *) NULL) {
            allocindex = 0;
          } else {
            allocindex = proxipooltag2allocindex(plex->vertexpool, vtx1);
          }
          ring = linkringnewfill(&plex->moleculepool, vtx1, tagarray,
                                 tagindex, allocindex);
          /* Make sure `vtx1' has a 2D star, and find it. */
          star = tetcomplexlookup3dstar(plex, vtx1);
          /* Store the link ring in 'vtx1's 2D link. */
          link2dinsertvertex(&plex->moleculepool, plex->cache, star->linkhead,
                             &star->linktail, vtx1, vtx2, ring);
        }

        triangle->neighbor[edge] = ~orgneighbor;
      }
    }

    if ((triangle->vtx[0] == GHOSTVERTEX) ||
        (triangle->vtx[1] == GHOSTVERTEX) ||
        (triangle->vtx[2] == GHOSTVERTEX)) {
      plex->ghosttetcount++;
    } else {
      plex->tetcount++;
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexremoveghosttets()   Deletes all ghost edges, and their stars,  */
/*                                from a tetcomplex, thereby deleting all    */
/*                                the explicitly inserted ghost tetrahedra.  */
/*                                                                           */
/*  This procedure searches the star of every vertex, rather than trying to  */
/*  do something more clever like finding one ghost tetrahedron by           */
/*  exhaustive search, then the others by walking through all the ghost      */
/*  triangles.  The advantage is that it will work correctly no matter how   */
/*  inconsistent the stars are with each other.  The disadvantage is that    */
/*  it's not as fast as it would be if implemented the clever way.           */
/*                                                                           */
/*  plex:  The tetcomplex in question.                                       */
/*                                                                           */
/*****************************************************************************/

void tetcomplexremoveghosttets(struct tetcomplex *plex)
{
  struct tetcomplexstar *newstar;
  tag starvertex;
  tag ghostring;

  if (plex->verbosity) {
    printf("  Removing ghost tetrahedra.\n");
  }

  /* Loop through all the vertices that might exist. */
  for (starvertex = 0; starvertex < plex->nextinitindex; starvertex++) {
    /* Is there a star for a vertex with tag `starvertex'? */
    newstar = (struct tetcomplexstar *)
      arraypoolfastlookup(&plex->stars, (arraypoolulong) starvertex);
    if (newstar->linkhead != STOP) {
      /* Remove the GHOSTVERTEX (and its link ring), if it is present, */
      /*   from the star of `starvertex'.                              */
      ghostring = link2ddeletevertex(&plex->moleculepool, plex->cache, 
                                     newstar->linkhead, &newstar->linktail,
                                     starvertex, GHOSTVERTEX);
      if (ghostring != STOP) {
        /* Deallocate the link ring. */
        linkringdelete(&plex->moleculepool, ghostring);
      }
    }
  }

  plex->ghosttetcount = 0;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplextetcount()   Returns the number of solid tetrahedra a          */
/*                         tetcomplex believes it contains.                  */
/*                                                                           */
/*  Ghost tetrahedra have no effect on the return value.                     */
/*                                                                           */
/*  The return value is meaningless if the tetcomplex is not internally      */
/*  consistent (as during a run of the star splaying algorithm).             */
/*                                                                           */
/*  The return value may be wrong if the tetcomplex passed through a state   */
/*  where it was not internally consistent, and the count has not been       */
/*  refreshed since the tetcomplex became consistent again.                  */
/*                                                                           */
/*  plex:  The tetcomplex in question.                                       */
/*                                                                           */
/*  Returns the number of solid tetrahedra believed to be in the complex.    */
/*                                                                           */
/*****************************************************************************/

arraypoolulong tetcomplextetcount(struct tetcomplex *plex)
{
  return plex->tetcount;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexghosttetcount()   Returns the number of _explicitly_inserted_  */
/*                              ghost tetrahedra a tetcomplex believes it    */
/*                              "contains".                                  */
/*                                                                           */
/*  Returns the value of an internal counter that is incremented with every  */
/*  successful explicit insertion of a ghost tetrahedron, and decremented    */
/*  with every successful explicit deletion of a ghost tetrahedron.  The     */
/*  explicit insertion or deletion of a ghost tetrahedron does not always    */
/*  change the tetcomplex data structure at all (because the GHOSTVERTEX     */
/*  might have a parity opposite to that of the other three vertices), so    */
/*  this number doesn't reflect any measurable property of the data          */
/*  structure.  The count is easily fooled, because you can repeatedly and   */
/*  "successfully" insert or delete a ghost tetrahedron that doesn't change  */
/*  the data structure.  Nevertheless, if ghost tetrahedra are inserted and  */
/*  deleted in a disciplined way, this count is the number of triangles on   */
/*  the boundary of the triangulation, and that is how the Bowyer-Watson     */
/*  implementation uses it.                                                  */
/*                                                                           */
/*  The return value is meaningless if the tetcomplex is not internally      */
/*  consistent (as it is not during a run of the star splaying algorithm).   */
/*                                                                           */
/*  The return value may be wrong if the tetcomplex passed through a state   */
/*  where it was not internally consistent, and the count has not been       */
/*  refreshed since the tetcomplex became consistent again.                  */
/*                                                                           */
/*  plex:  The tetcomplex in question.                                       */
/*                                                                           */
/*  Returns the number of ghost tetrahedra believed to be in the complex.    */
/*                                                                           */
/*****************************************************************************/

arraypoolulong tetcomplexghosttetcount(struct tetcomplex *plex)
{
  return plex->ghosttetcount;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexbytes()   Returns the number of bytes of dynamic memory used   */
/*                      by the tetrahedral complex.                          */
/*                                                                           */
/*  Does not include the memory for the `struct tetcomplex' record itself.   */
/*                                                                           */
/*  plex:  The tetcomplex in question.                                       */
/*                                                                           */
/*  Returns the number of dynamically allocated bytes in `plex'.             */
/*                                                                           */
/*****************************************************************************/

arraypoolulong tetcomplexbytes(struct tetcomplex *plex)
{
  return proxipoolbytes(&plex->moleculepool) +
         arraypoolbytes(&plex->stars);
}

/**                                                                         **/
/**                                                                         **/
/********* Tetrahedral complex routines end here                     *********/


/********* Input routines begin here                                 *********/
/**                                                                         **/
/**                                                                         **/

/*  A vertex is assigned the `number' DEADVERTEX when it is deallocated, to  */
/*  indicate that the vertex record should not be consulted (and its memory  */
/*  might be reclaimed by another vertex any time).  A vertex is most likely */
/*  to become dead if it is a non-corner node of an input mesh (such as the  */
/*  side-edge nodes of a quadratic element), or if it was generated (not an  */
/*  input vertex) then subsequently deleted from a mesh.                     */
/*                                                                           */
/*  A vertex is assigned the `number' ACTIVEVERTEX temporarily when the      */
/*  `jettison' (-j) switch is used, to indicate that some tetrahedron has it */
/*  for a vertex and it should not be discarded.                             */

#define DEADVERTEX ((arraypoolulong) ~0)
#define ACTIVEVERTEX ((arraypoolulong) ~1)

/*  `vertexmarktype' is the type of int for the user-specified marker        */
/*  associated with each vertex.  These markers are typically used to keep   */
/*  track of which vertices lie on which segments or facets, and which       */
/*  boundary conditions should be applied to which vertices.                 */

typedef int vertexmarktype;


/*  A `vertex' is a structure that represents a vertex in the mesh.  Each    */
/*  vertex has three coordinates (x, y, and z) in `coord', a vertex mark     */
/*  stored in `mark', and a `number' by which the user indexes the vertex.   */
/*  Note that a vertex may have a different number in the output files than  */
/*  it had in the input files.  Note that each vertex may have auxiliary     */
/*  user-provided floating-point attributes, but these are not stored        */
/*  contiguously with the vertex structures.                                 */

struct vertex {
  starreal coord[3];
  vertexmarktype mark;
  arraypoolulong number;
};

/*  A `vertexshort' is a structure that represents a vertex in the mesh.     */
/*  It is like a `vertex', but the vertex number is omitted to save space.   */
/*  The `coord' array comes last because it is treated as a variable-length  */
/*  array, with indices 3 and greater representing user-provided floating-   */
/*  point attributes.  This structure is typically used for vertex sorting   */
/*  prior to triangulation.                                                  */

struct vertexshort {
  vertexmarktype mark;
  starreal coord[3];
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

struct outputs {
  arraypoolulong vertexcount;
  arraypoolulong tetcount;
  arraypoolulong facecount;
  arraypoolulong boundaryfacecount;
  arraypoolulong edgecount;
};


void vertexcheckorientation(struct behavior *behave,
                            struct tetcomplex *plex,
                            tag vtx1,
                            tag vtx2,
                            tag vtx3,
                            tag vtx4)
{
  starreal ori;
  struct vertex *point1;
  struct vertex *point2;
  struct vertex *point3;
  struct vertex *point4;

  point1 = (struct vertex *) tetcomplextag2vertex(plex, vtx1);
  point2 = (struct vertex *) tetcomplextag2vertex(plex, vtx2);
  point3 = (struct vertex *) tetcomplextag2vertex(plex, vtx3);
  point4 = (struct vertex *) tetcomplextag2vertex(plex, vtx4);

  ori = orient3d(behave, point1->coord, point2->coord, point3->coord,
                 point4->coord);
  if (ori <= 0.0) {
    if (ori < 0.0) {
      printf("  !! !! Creating inverted tet\n");
    } else {
      printf("  !! !! Creating degenerate tet\n");
    }
    printf("        # %lu tag %lu (%.12g, %.12g, %.12g)"
           " # %lu tag %lu (%.12g, %.12g, %.12g)\n",
           (unsigned long) point1->number, (unsigned long) vtx1,
           (double) point1->coord[0], (double) point1->coord[1],
           (double) point1->coord[2],
           (unsigned long) point2->number, (unsigned long) vtx2,
           (double) point2->coord[0], (double) point2->coord[1],
           (double) point2->coord[2]);
    printf("        # %lu tag %lu (%.12g, %.12g, %.12g)"
           " # %lu tag %lu (%.12g, %.12g, %.12g)\n",
           (unsigned long) point3->number, (unsigned long) vtx3,
           (double) point3->coord[0], (double) point3->coord[1],
           (double) point3->coord[2],
           (unsigned long) point4->number, (unsigned long) vtx4,
           (double) point4->coord[0], (double) point4->coord[1],
           (double) point4->coord[2]);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  inputtextline()   Read a nonempty line from a file.                      */
/*                                                                           */
/*  A line is considered "nonempty" if it contains something that looks like */
/*  a number.  Comments (prefaced by `#') are ignored.                       */
/*                                                                           */
/*****************************************************************************/

#ifndef STARLIBRARY

char *inputtextline(char *string,
                    FILE *infile,
                    char *infilename)
{
  char *result;

  /* Search for something that looks like a number. */
  do {
    result = fgets(string, INPUTLINESIZE, infile);
    if (result == (char *) NULL) {
      printf("  Error:  Unexpected end of file in %s.\n", infilename);
      starexit(1);
    }

    /* Skip anything that doesn't look like a number, a comment, */
    /*   or the end of a line.                                   */
    while ((*result != '\0') && (*result != '#') &&
           (*result != '.') && (*result != '+') && (*result != '-') &&
           ((*result < '0') || (*result > '9'))) {
      result++;
    }
  /* If it's a comment or end of line, read another line and try again. */
  } while ((*result == '#') || (*result == '\0'));

  return result;
}

#endif /* not STARLIBRARY */

/*****************************************************************************/
/*                                                                           */
/*  inputfindfield()   Find the next field of a string.                      */
/*                                                                           */
/*  Jumps past the current field by searching for whitespace, then jumps     */
/*  past the whitespace to find the next field.                              */
/*                                                                           */
/*****************************************************************************/

#ifndef STARLIBRARY

char *inputfindfield(char *string)
{
  char *result;

  result = string;
  /* Skip the current field.  Stop upon reaching whitespace. */
  while ((*result != '\0') && (*result != '#') &&
         (*result != ' ') && (*result != '\t')) {
    result++;
  }

  /* Now skip the whitespace and anything else that doesn't look like a */
  /*   number, a comment, or the end of a line.                         */
  while ((*result != '\0') && (*result != '#') &&
         (*result != '.') && (*result != '+') && (*result != '-') &&
         ((*result < '0') || (*result > '9'))) {
    result++;
  }

  /* Check for a comment (prefixed with `#'). */
  if (*result == '#') {
    *result = '\0';
  }

  return result;
}

#endif /* not STARLIBRARY */

/*****************************************************************************/
/*                                                                           */
/*  inputvertices()   Read the vertices from a file into a newly created     */
/*                    array, in `struct vertexshort' format.                 */
/*                                                                           */
/*****************************************************************************/

#ifndef STARLIBRARY

char *inputvertices(FILE *vertexfile,
                    char *vertexfilename,
                    struct inputs *in,
                    int markflag)
{
  char inputline[INPUTLINESIZE];
  char *stringptr;
  char *vertices;
  struct vertexshort *vertexptr;
  size_t vertexbytes;
  long firstnode;
  arraypoolulong i;
  unsigned int j;

  vertexbytes = sizeof(struct vertexshort) +
                (size_t) in->attribcount * sizeof(starreal);
  vertices = (char *) starmalloc((size_t) (in->vertexcount * vertexbytes));

  /* Read the vertices. */
  for (i = 0; i < in->vertexcount; i++) {
    vertexptr = (struct vertexshort *) &vertices[i * vertexbytes];

    stringptr = inputtextline(inputline, vertexfile, vertexfilename);
    if (i == 0) {
      firstnode = (long) strtol(stringptr, &stringptr, 0);
      if ((firstnode == 0) || (firstnode == 1)) {
        in->firstnumber = (arraypoolulong) firstnode;
      }
    }

    /* Read the vertex coordinates and attributes. */
    for (j = 0; j < 3 + in->attribcount; j++) {
      stringptr = inputfindfield(stringptr);
      if (*stringptr == '\0') {
        if (j >= 3) {
          vertexptr->coord[j] = 0.0;
        } else {
          printf("Error:  Vertex %lu has no %c coordinate.\n",
                 (unsigned long) (in->firstnumber + i),
                 (j == 0) ? 'x' : (j == 1) ? 'y' : 'z');
          starexit(1);
        }
      } else {
        vertexptr->coord[j] = (starreal) strtod(stringptr, &stringptr);
      }
    }

    if (markflag) {
      /* Read a vertex marker. */
      stringptr = inputfindfield(stringptr);
      if (*stringptr == '\0') {
        vertexptr->mark = 0;
      } else {
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
        vertexptr->mark = (vertexmarktype) strtoll(stringptr, &stringptr, 0);
#else /* no long long type */
        vertexptr->mark = (vertexmarktype) strtol(stringptr, &stringptr, 0);
#endif
      }
    }
  }

  return vertices;
}

#endif /* not STARLIBRARY */

char *inputvertexfile(struct behavior *behave,
                      struct inputs *in,
                      FILE **polyfile)
{
  char inputline[INPUTLINESIZE];
  char *stringptr;
  FILE *infile = (FILE *) NULL;
  char *infilename;
  char *vertices;
  int mesh_dim = 3;
  int markflag = 0;

  if (behave->poly) {
    /* Read the vertices from a .poly file. */
    if (!behave->quiet) {
      printf("Opening %s.\n", behave->inpolyfilename);
    }

    *polyfile = fopen(behave->inpolyfilename, "r");
    if (*polyfile == (FILE *) NULL) {
      printf("  Error:  Cannot access file %s.\n", behave->inpolyfilename);
      starexit(1);
    }

    /* Read number of vertices, number of dimensions, number of vertex */
    /*   attributes, and number of boundary markers.                   */
    stringptr = inputtextline(inputline, *polyfile,
                              behave->inpolyfilename);
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
    in->vertexcount = (arraypoolulong) strtoull(stringptr, &stringptr, 0);
#else /* no long long type */
    in->vertexcount = (arraypoolulong) strtoul(stringptr, &stringptr, 0);
#endif
    stringptr = inputfindfield(stringptr);
    if (*stringptr == '\0') {
      mesh_dim = 3;
    } else {
      mesh_dim = (int) strtol(stringptr, &stringptr, 0);
    }
    stringptr = inputfindfield(stringptr);
    if (*stringptr == '\0') {
      in->attribcount = 0;
    } else {
      in->attribcount = (unsigned int) strtoul(stringptr, &stringptr, 0);
    }
    stringptr = inputfindfield(stringptr);
    if (*stringptr == '\0') {
      markflag = 0;
    } else {
      markflag = (int) strtol(stringptr, &stringptr, 0);
    }

    if (in->vertexcount > 0) {
      infile = *polyfile;
      infilename = behave->inpolyfilename;
      behave->readnodefileflag = 0;
    } else {
      /* If the .poly file claims there are zero vertices, that means that */
      /*   the vertices should be read from a separate .node file.         */
      behave->readnodefileflag = 1;
      infilename = behave->innodefilename;
    }
  } else {
    behave->readnodefileflag = 1;
    infilename = behave->innodefilename;
    *polyfile = (FILE *) NULL;
  }
  in->deadvertexcount = 0;

  if (behave->readnodefileflag) {
    /* Read the vertices from a .node file. */
    if (!behave->quiet) {
      printf("Opening %s.\n", behave->innodefilename);
    }

    infile = fopen(behave->innodefilename, "r");
    if (infile == (FILE *) NULL) {
      printf("  Error:  Cannot access file %s.\n", behave->innodefilename);
      starexit(1);
    }

    /* Read number of vertices, number of dimensions, number of vertex */
    /*   attributes, and number of boundary markers.                   */
    stringptr = inputtextline(inputline, infile, behave->innodefilename);
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
    in->vertexcount = (arraypoolulong) strtoull(stringptr, &stringptr, 0);
#else /* no long long type */
    in->vertexcount = (arraypoolulong) strtoul(stringptr, &stringptr, 0);
#endif
    stringptr = inputfindfield(stringptr);
    if (*stringptr == '\0') {
      mesh_dim = 3;
    } else {
      mesh_dim = (int) strtol(stringptr, &stringptr, 0);
    }
    stringptr = inputfindfield(stringptr);
    if (*stringptr == '\0') {
      in->attribcount = 0;
    } else {
      in->attribcount = (unsigned int) strtoul(stringptr, &stringptr, 0);
    }
    stringptr = inputfindfield(stringptr);
    if (*stringptr == '\0') {
      markflag = 0;
    } else {
      markflag = (int) strtol(stringptr, &stringptr, 0);
    }
  }

  if (in->vertexcount < 4) {
    printf("Error:  Input must have at least four input vertices.\n");
    starexit(1);
  }
  if (mesh_dim != 3) {
    printf("Error:  Star only works with three-dimensional meshes.\n");
    starexit(1);
  }

  in->firstnumber = behave->firstnumber;
  vertices = inputvertices(infile, infilename, in, markflag);

  if (behave->readnodefileflag) {
    fclose(infile);
  }

  behave->firstnumber = in->firstnumber;
  if (in->attribcount == 0) {
    behave->weighted = 0;
  }

  return vertices;
}

/*****************************************************************************/
/*                                                                           */
/*  inputvertexsort()   Sort an array of vertices (in `struct vertexshort'   */
/*                      format) along a z-order space-filling curve.         */
/*                                                                           */
/*  Uses quicksort.  Randomized O(n log n) time.                             */
/*                                                                           */
/*  Sorts whole `struct vertexshort' records, rather than pointers to        */
/*  records, because I think it will be faster for very large vertex sets,   */
/*  where quicksort's linear walk through the array favors the memory        */
/*  hierarchy better than pseudo-random record lookups.                      */
/*                                                                           */
/*****************************************************************************/

void inputvertexsort(char *vertices,
                     tag *vertextags,
                     arraypoolulong vertexcount,
                     size_t vertexbytes,
                     unsigned int attribcount)
{
  struct vertexshort tempvertex;
  struct vertexshort pivotvertex;
  struct vertexshort *leftptr, *rightptr, *pivotptr;
  arraypoolulong leftindex, rightindex, pivotindex;
  starreal pivotx, pivoty, pivotz;
  starreal tempreal;
  tag pivottag;
  tag temptag;
  unsigned int j;

  if (vertexcount == 2) {
    /* Base case of the recursion. */
    leftptr = (struct vertexshort *) vertices;
    rightptr = (struct vertexshort *) &vertices[vertexbytes];
    if (zorderbefore(rightptr->coord[0], rightptr->coord[1],
                     rightptr->coord[2], leftptr->coord[0],
                     leftptr->coord[1], leftptr->coord[2])) {
      temptag = vertextags[1];
      vertextags[1] = vertextags[0];
      vertextags[0] = temptag;
      tempvertex = *rightptr;
      *rightptr = *leftptr;
      *leftptr = tempvertex;
      for (j = 3; j < 3 + attribcount; j++) {
        tempreal = rightptr->coord[j];
        rightptr->coord[j] = leftptr->coord[j];
        leftptr->coord[j] = tempreal;
      }
    }
    return;
  }

  if (vertexcount < 5) {
    pivotindex = 0;
    pivotptr = (struct vertexshort *) vertices;
  } else {
    /* Choose a random pivot to split the array. */
    pivotindex = (arraypoolulong) randomnation((unsigned int) vertexcount);
    pivotptr = (struct vertexshort *) &vertices[pivotindex * vertexbytes];
  }
  pivottag = vertextags[pivotindex];
  pivotvertex = *pivotptr;
  pivotx = pivotvertex.coord[0];
  pivoty = pivotvertex.coord[1];
  pivotz = pivotvertex.coord[2];
  if (pivotindex > 0) {
    /* Move the first vertex of the array to take the pivot's place. */
    vertextags[pivotindex] = vertextags[0];
    *pivotptr = * (struct vertexshort *) vertices;
    for (j = 3; j < 3 + attribcount; j++) {
      /* Swap the pivot's attributes with the first vertex's. */
      tempreal = pivotptr->coord[j];
      pivotptr->coord[j] = ((struct vertexshort *) vertices)->coord[j];
      ((struct vertexshort *) vertices)->coord[j] = tempreal;
    }
  }

  /* Partition the array. */
  leftindex = 0;
  leftptr = (struct vertexshort *) vertices;
  rightindex = vertexcount;
  rightptr = (struct vertexshort *) &vertices[vertexcount * vertexbytes];
  do {
    /* Find a vertex whose z-order position is too large for the left. */
    do {
      leftindex++;
      leftptr = (struct vertexshort *) (((char *) leftptr) + vertexbytes);
    } while ((leftindex < rightindex) &&
             zorderbefore(leftptr->coord[0], leftptr->coord[1],
                          leftptr->coord[2], pivotx, pivoty, pivotz));
    /* Find a vertex whose z-order position is too small for the right. */
    do {
      rightindex--;
      rightptr = (struct vertexshort *) (((char *) rightptr) - vertexbytes);
    } while ((leftindex <= rightindex) &&
             zorderbefore(pivotx, pivoty, pivotz, rightptr->coord[0],
                          rightptr->coord[1], rightptr->coord[2]));

    if (leftindex < rightindex) {
      /* Swap the left and right vertices. */
      temptag = vertextags[rightindex];
      vertextags[rightindex] = vertextags[leftindex];
      vertextags[leftindex] = temptag;
      tempvertex = *rightptr;
      *rightptr = *leftptr;
      *leftptr = tempvertex;
      for (j = 3; j < 3 + attribcount; j++) {
        tempreal = rightptr->coord[j];
        rightptr->coord[j] = leftptr->coord[j];
        leftptr->coord[j] = tempreal;
      }
    }
  } while (leftindex < rightindex);

  /* Place the pivot in the middle of the partition by swapping with the */
  /*   right vertex.                                                     */
  vertextags[0] = vertextags[rightindex];
  vertextags[rightindex] = pivottag;
  * (struct vertexshort *) vertices = *rightptr;
  *rightptr = pivotvertex;
  for (j = 3; j < 3 + attribcount; j++) {
    /* Swap the right vertex's attributes with the pivot's. */
    tempreal = rightptr->coord[j];
    rightptr->coord[j] = ((struct vertexshort *) vertices)->coord[j];
    ((struct vertexshort *) vertices)->coord[j] = tempreal;
  }

  if (rightindex > 1) {
    /* Recursively sort the left subset. */
    inputvertexsort(vertices, vertextags, rightindex, vertexbytes,
                    attribcount);
  }
  if (rightindex < vertexcount - 2) {
    /* Recursively sort the right subset. */
    inputvertexsort(&vertices[(rightindex + 1) * vertexbytes],
                    &vertextags[rightindex + 1], vertexcount - rightindex - 1,
                    vertexbytes, attribcount);
  }
}

void inputverticesintopool(char *vertices,
                           struct inputs *in,
                           struct proxipool *pool)
{
  struct vertex *vertexptr;
  struct vertexshort *vertexshortptr;
  starreal *attributes;
  size_t vertexbytes;
  tag newtag;
  arraypoolulong i;
  unsigned int j;

  vertexshortptr = (struct vertexshort *) vertices;
  vertexbytes = sizeof(struct vertexshort) +
                (size_t) in->attribcount * sizeof(starreal);
  for (i = 0; i < in->vertexcount; i++) {
    newtag = proxipoolnew(pool, 0, (void **) &vertexptr);
    vertexptr->coord[0] = vertexshortptr->coord[0];
    vertexptr->coord[1] = vertexshortptr->coord[1];
    vertexptr->coord[2] = vertexshortptr->coord[2];
    vertexptr->mark = vertexshortptr->mark;
    vertexptr->number = in->vertextags[i];
    in->vertextags[i] = newtag;
    if (in->attribcount > 0) {
      attributes = (starreal *) proxipooltag2object2(pool, newtag);
      for (j = 0; j < in->attribcount; j++) {
        attributes[j] = vertexshortptr->coord[3 + j];
      }
    }

    vertexshortptr = (struct vertexshort *) (((char *) vertexshortptr) +
                                             vertexbytes);
  }
}

void inputverticessortstore(char *vertices,
                            struct inputs *in,
                            struct proxipool *pool)
{
  arraypoolulong i;
  tag *tags;

  tags = (tag *) starmalloc((size_t) (in->vertexcount * sizeof(tag)));
  for (i = 0; i < in->vertexcount; i++) {
    tags[i] = (tag) (in->firstnumber + i);
  }

  inputvertexsort(vertices, tags, in->vertexcount,
                  sizeof(struct vertexshort) +
                  (size_t) in->attribcount * sizeof(starreal),
                  in->attribcount);

  in->vertextags = tags;
  inputverticesintopool(vertices, in, pool);
}

FILE *inputverticesreadsortstore(struct behavior *behave,
                                 struct inputs *in,
                                 struct proxipool *pool)
{
  char *vertices;
  FILE *polyfile;

  vertices = inputvertexfile(behave, in, &polyfile);
  proxipoolinit(pool, sizeof(struct vertex),
                (size_t) (in->attribcount * sizeof(starreal)),
                behave->verbose);
  inputverticessortstore(vertices, in, pool);
  starfree(vertices);

  return polyfile;
}

void inputmaketagmap(struct proxipool *vertexpool,
                     arraypoolulong firstnumber,
                     tag *vertextags)
{
  struct vertex *vertexptr;
  tag iterator;

  iterator = proxipooliterate(vertexpool, NOTATAG);
  while (iterator != NOTATAG) {
    vertexptr = proxipooltag2object(vertexpool, iterator);
    vertextags[vertexptr->number - firstnumber] = iterator;
    iterator = proxipooliterate(vertexpool, iterator);
  }
}

void inputtetrahedra(struct behavior *behave,
                     struct inputs *in,
                     struct proxipool *vertexpool,
                     struct outputs *out,
                     struct tetcomplex *plex)
{
  arraypoolulong corner[4];
  tag cornertag[4];
  char inputline[INPUTLINESIZE];
  char *stringptr;
  FILE *elefile;
  struct vertex *killvertex;
  tag killtag;
  arraypoolulong killnode;
  arraypoolulong elementnumber;
  arraypoolulong boundaryfacecount;
  int elemnodes;
  int result;
  int i;

  /* Read the tetrahedra from an .ele file. */
  if (!behave->quiet) {
    printf("Opening %s.\n", behave->inelefilename);
  }

  elefile = fopen(behave->inelefilename, "r");
  if (elefile == (FILE *) NULL) {
    printf("  Error:  Cannot access file %s.\n", behave->inelefilename);
    starexit(1);
  }

  /* Read number of tetrahedra, number of vertices per tetrahedron, and */
  /*   number of tetrahedron attributes from .ele file.                 */
  stringptr = inputtextline(inputline, elefile, behave->inelefilename);
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
  in->tetcount = (arraypoolulong) strtoull(stringptr, &stringptr, 0);
#else /* no long long type */
  in->tetcount = (arraypoolulong) strtoul(stringptr, &stringptr, 0);
#endif
  stringptr = inputfindfield(stringptr);
  if (*stringptr == '\0') {
    elemnodes = 4;
  } else {
    elemnodes = (int) strtol(stringptr, &stringptr, 0);
    if (elemnodes < 4) {
      printf("Error:  Tetrahedra in %s must have at least 4 vertices.\n",
             behave->inelefilename);
      starexit(1);
    }
  }
  stringptr = inputfindfield(stringptr);
  if (*stringptr == '\0') {
    in->tetattribcount = 0;
  } else {
    in->tetattribcount = (unsigned int) strtoul(stringptr, &stringptr, 0);
  }

  tetcomplexinit(plex, vertexpool, behave->verbose);

  if (!behave->quiet) {
    printf("Reconstructing mesh.\n");
  }

  boundaryfacecount = 0;
  for (elementnumber = behave->firstnumber;
       elementnumber < behave->firstnumber + in->tetcount; elementnumber++) {
    /* Read the tetrahedron's four vertices. */
    stringptr = inputtextline(inputline, elefile, behave->inelefilename);
    for (i = 0; i < 4; i++) {
      stringptr = inputfindfield(stringptr);
      if (*stringptr == '\0') {
        printf("Error:  Tetrahedron %lu is missing vertex %d in %s.\n",
               (unsigned long) elementnumber, i + 1, behave->inelefilename);
        starexit(1);
      } else {
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
        corner[i] = (arraypoolulong) strtoull(stringptr, &stringptr, 0);
#else /* no long long type */
        corner[i] = (arraypoolulong) strtoul(stringptr, &stringptr, 0);
#endif
        if ((corner[i] < behave->firstnumber) ||
            (corner[i] >= behave->firstnumber + in->vertexcount)) {
          printf("Error:  Tetrahedron %lu has an invalid vertex index.\n",
                 (unsigned long) elementnumber);
          starexit(1);
        }
        cornertag[i] = in->vertextags[corner[i] - behave->firstnumber];
      }
    }

    /* Find out about (and throw away) extra nodes. */
    for (i = 4; i < elemnodes; i++) {
      stringptr = inputfindfield(stringptr);
      if (*stringptr != '\0') {
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
        killnode = (arraypoolulong) strtoull(stringptr, &stringptr, 0);
#else /* no long long type */
        killnode = (arraypoolulong) strtoul(stringptr, &stringptr, 0);
#endif
        if ((killnode >= behave->firstnumber) &&
            (killnode < behave->firstnumber + in->vertexcount)) {
          /* Delete the non-corner node if it's not already deleted. */
          killtag = in->vertextags[killnode - behave->firstnumber];
          killvertex = (struct vertex *) proxipooltag2object(vertexpool,
                                                             killtag);
          if (killvertex->number != DEADVERTEX) {
            killvertex->number = DEADVERTEX;
            proxipoolfree(vertexpool, killtag);
            in->deadvertexcount++;
          }
        }
      }
    }

#ifdef SELF_CHECK
    vertexcheckorientation(behave, plex, cornertag[0], cornertag[1],
                           cornertag[2], cornertag[3]);
#endif /* SELF_CHECK */
    result = tetcomplexinserttet(plex, cornertag[0], cornertag[1],
                                 cornertag[2], cornertag[3]);
    if (result > 0) {
      boundaryfacecount += (result - 6);
    }
  }

  fclose(elefile);

  out->vertexcount = in->vertexcount - in->deadvertexcount;
  out->tetcount = tetcomplextetcount(plex);
  out->boundaryfacecount = boundaryfacecount;
  out->facecount = 2 * out->tetcount + (out->boundaryfacecount / 2);
  out->edgecount = out->vertexcount + out->facecount - out->tetcount - 1;
}

/**                                                                         **/
/**                                                                         **/
/********* Input routines end here                                   *********/


/********* Output routines begin here                                *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  outputfilefinish()   Write the command line to the output file so the    */
/*                       user can remember how the file was generated.       */
/*                       Close the file.                                     */
/*                                                                           */
/*****************************************************************************/

#ifndef STARLIBRARY

void outputfilefinish(FILE *outfile,
                      int argc,
                      char **argv)
{
  int i;

  fputs("# Generated by", outfile);
  for (i = 0; i < argc; i++) {
    putc(' ', outfile);
    fputs(argv[i], outfile);
  }
  putc('\n', outfile);
  fclose(outfile);
}

#endif /* not PYRLIBRARY */

/*****************************************************************************/
/*                                                                           */
/*  outputpreparevertices()   Determines which vertices are in the mesh, and */
/*                            marks the vertices on the hull boundary.       */
/*                                                                           */
/*  If the -j switch is selected (behave->jettison), the vertices in the     */
/*  mesh are identified and their "number" fields are set to ACTIVEVERTEX.   */
/*                                                                           */
/*  If the -B switch is not selected (behave->nobound), every vertex on the  */
/*  boundary of the convex hull whose mark is zero has its mark set to 1.    */
/*                                                                           */
/*  behave:  Command line switches, operation counts, etc.                   */
/*  vertexpool:  The proxipool of vertices associated with 'plex'.           */
/*  plex:  The tetcomplex in question.                                       */
/*                                                                           */
/*****************************************************************************/

void outputpreparevertices(struct behavior *behave,
                           struct tetcomplex *plex)
{
  struct tetcomplexposition position;
  struct proxipool *pool;
  struct vertex *vertexptr0;
  struct vertex *vertexptr1;
  struct vertex *vertexptr2;
  struct vertex *vertexptr3;
  tag tet[4];

  if (behave->jettison) {
    if (behave->verbose) {
      if (behave->nobound) {
        printf("Identifying vertices in mesh.\n");
      } else {
        printf("Identifying vertices in mesh and marking boundary vertices.\n"
               );
      }
    }
  } else {
    if (behave->nobound) {
      return;
    } else if (behave->verbose) {
      printf("Marking boundary vertices.\n");
    }
  }

  pool = plex->vertexpool;
  /* Iterate through all the tetrahedra in the complex. */
  tetcomplexiteratorinit(plex, &position);
  tetcomplexiterate(&position, tet);
  while (tet[0] != STOP) {
    if ((tet[2] == GHOSTVERTEX) || (tet[3] == GHOSTVERTEX)) {
      if (!behave->nobound) {
        vertexptr0 = (struct vertex *) proxipooltag2object(pool, tet[0]);
        vertexptr1 = (struct vertex *) proxipooltag2object(pool, tet[1]);
        vertexptr2 = (struct vertex *)
          proxipooltag2object(pool, tet[(tet[2] == GHOSTVERTEX) ? 3 : 2]);
        /* These vertices are on the boundary of the triangulation.  If any */
        /*   of them has a vertex mark of zero, change it to one.           */
        if (vertexptr0->mark == 0) {
          vertexptr0->mark = 1;
        }
        if (vertexptr1->mark == 0) {
          vertexptr1->mark = 1;
        }
        if (vertexptr2->mark == 0) {
          vertexptr2->mark = 1;
        }
      }
    } else if (behave->jettison) {
      vertexptr0 = (struct vertex *) proxipooltag2object(pool, tet[0]);
      vertexptr1 = (struct vertex *) proxipooltag2object(pool, tet[1]);
      vertexptr2 = (struct vertex *) proxipooltag2object(pool, tet[2]);
      vertexptr3 = (struct vertex *) proxipooltag2object(pool, tet[3]);
      vertexptr0->number = ACTIVEVERTEX;
      vertexptr1->number = ACTIVEVERTEX;
      vertexptr2->number = ACTIVEVERTEX;
      vertexptr3->number = ACTIVEVERTEX;
    }

    tetcomplexiterate(&position, tet);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  outputvertices()   Write the vertices to a .node file.                   */
/*                                                                           */
/*  in:  Properties of the input geometry.                                   */
/*                                                                           */
/*****************************************************************************/

void outputvertices(struct behavior *behave,
                    struct inputs *in,
                    struct proxipool *pool,
                    arraypoolulong vertexcount,
                    int argc,
                    char **argv)
{
  struct vertex *vertexptr;
  starreal *attributes;
  FILE *outfile;
  tag vertextag;
  arraypoolulong vertexnumber;
  arraypoolulong i;
  unsigned int j;

  if (!behave->quiet) {
    printf("Writing %s.\n", behave->outnodefilename);
  }

  outfile = fopen(behave->outnodefilename, "w");
  if (outfile == (FILE *) NULL) {
    printf("  Error:  Cannot create file %s.\n", behave->outnodefilename);
    starexit(1);
  }

  if (!behave->jettison) {
    vertexcount = proxipoolobjects(pool);
  }

  /* Number of vertices, number of dimensions, number of vertex attributes, */
  /*   and number of boundary markers (zero or one).                        */
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
  if (sizeof(arraypoolulong) > sizeof(long)) {
    fprintf(outfile, "%llu  3  %u  %d\n", (unsigned long long) vertexcount,
            in->attribcount, 1 - behave->nobound);
  } else {
    fprintf(outfile, "%lu  3  %u  %d\n", (unsigned long) vertexcount,
            in->attribcount, 1 - behave->nobound);
  }
#else
  fprintf(outfile, "%lu  3  %u  %d\n", (unsigned long) vertexcount,
          in->attribcount, 1 - behave->nobound);
#endif

  vertexnumber = (arraypoolulong) behave->firstnumber;
  vertextag = proxipooliterate(pool, NOTATAG);
  i = 0;
  while ((behave->jettison && (vertextag != NOTATAG)) ||
         (!behave->jettison && (i < in->vertexcount))) {
    if (!behave->jettison) {
      vertextag = in->vertextags[i];
    }
    vertexptr = (struct vertex *) proxipooltag2object(pool, vertextag);

    if ((vertexptr->number != DEADVERTEX) &&
        (!behave->jettison || (vertexptr->number == ACTIVEVERTEX))) {
      /* Node number, x, y, and z coordinates. */
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
      if (sizeof(arraypoolulong) > sizeof(long)) {
        fprintf(outfile, "%4llu    %.17g  %.17g  %.17g",
                (unsigned long long) vertexnumber,
                (double) vertexptr->coord[0], (double) vertexptr->coord[1],
                (double) vertexptr->coord[2]);
      } else {
        fprintf(outfile, "%4lu    %.17g  %.17g  %.17g",
                (unsigned long) vertexnumber,
                (double) vertexptr->coord[0], (double) vertexptr->coord[1],
                (double) vertexptr->coord[2]);
      }
#else
      fprintf(outfile, "%4lu    %.17g  %.17g  %.17g",
              (unsigned long) vertexnumber,
              (double) vertexptr->coord[0], (double) vertexptr->coord[1],
              (double) vertexptr->coord[2]);
#endif
      if (in->attribcount > 0) {
        attributes = (starreal *) proxipooltag2object2(pool,
                                                       in->vertextags[i]);
        for (j = 0; j < in->attribcount; j++) {
          /* Write an attribute. */
          fprintf(outfile, "  %.17g", (double) attributes[j]);
        }
      }
      if (behave->nobound) {
        putc('\n', outfile);
      } else {
        /* Write the boundary marker. */
        fprintf(outfile, "    %ld\n", (long) vertexptr->mark);
      }

      vertexptr->number = vertexnumber;
      vertexnumber++;
    }

    if (behave->jettison) {
      vertextag = proxipooliterate(pool, vertextag);
    } else {
      i++;
    }
  }

  outputfilefinish(outfile, argc, argv);
}

void outputnumbervertices(struct behavior *behave,
                          struct inputs *in,
                          struct proxipool *pool)
{
  struct vertex *vertexptr;
  tag vertextag;
  arraypoolulong vertexnumber;
  arraypoolulong i;

  vertexnumber = (arraypoolulong) behave->firstnumber;
  if (behave->jettison) {
    vertextag = proxipooliterate(pool, NOTATAG);
    while (vertextag != NOTATAG) {
      vertexptr = (struct vertex *) proxipooltag2object(pool, vertextag);
      if (vertexptr->number == ACTIVEVERTEX) {
        vertexptr->number = vertexnumber;
        vertexnumber++;
      }
      vertextag = proxipooliterate(pool, vertextag);
    }
  } else {
    for (i = 0; i < in->vertexcount; i++) {
      vertexptr = (struct vertex *) proxipooltag2object(pool,
                                                        in->vertextags[i]);
      if (vertexptr->number != DEADVERTEX) {
        vertexptr->number = vertexnumber;
        vertexnumber++;
      }
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  outputtetrahedra()   Write the tetrahedra to an .ele file.               */
/*                                                                           */
/*****************************************************************************/

void outputtetrahedra(struct behavior *behave,
                      struct inputs *in,
                      struct tetcomplex *plex,
                      int argc,
                      char **argv)
{
  struct tetcomplexposition position;
  struct proxipool *pool;
  struct vertex *vertexptr0;
  struct vertex *vertexptr1;
  struct vertex *vertexptr2;
  struct vertex *vertexptr3;
  tag tet[4];
  FILE *outfile;
  arraypoolulong tetnumber;

  if (!behave->quiet) {
    printf("Writing %s.\n", behave->outelefilename);
  }

  outfile = fopen(behave->outelefilename, "w");
  if (outfile == (FILE *) NULL) {
    printf("  Error:  Cannot create file %s.\n", behave->outelefilename);
    starexit(1);
  }

  /* Number of tetrahedra, vertices per tetrahedron, attributes */
  /*   per tetrahedron.                                         */
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
  if (sizeof(arraypoolulong) > sizeof(long)) {
    fprintf(outfile, "%llu  %d  0\n",
            (unsigned long long) tetcomplextetcount(plex),
            (behave->order + 1) * (behave->order + 2) * (behave->order + 3) /
            6);
  } else {
    fprintf(outfile, "%lu  %d  0\n",
            (unsigned long) tetcomplextetcount(plex),
            (behave->order + 1) * (behave->order + 2) * (behave->order + 3) /
            6);
  }
#else
  fprintf(outfile, "%lu  %d  0\n", (unsigned long) tetcomplextetcount(plex),
          (behave->order + 1) * (behave->order + 2) * (behave->order + 3) / 6);
#endif

  pool = plex->vertexpool;
  tetnumber = (arraypoolulong) behave->firstnumber;

  /* Iterate through all the (non-ghost) tetrahedra in the complex. */
  tetcomplexiteratorinit(plex, &position);
  tetcomplexiteratenoghosts(&position, tet);
  while (tet[0] != STOP) {
    vertexptr0 = (struct vertex *) proxipooltag2object(pool, tet[0]);
    vertexptr1 = (struct vertex *) proxipooltag2object(pool, tet[1]);
    vertexptr2 = (struct vertex *) proxipooltag2object(pool, tet[2]);
    vertexptr3 = (struct vertex *) proxipooltag2object(pool, tet[3]);

    /* Tetrahedron number and indices for four vertices. */
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
    if (sizeof(arraypoolulong) > sizeof(long)) {
      fprintf(outfile, "%4llu    %4llu  %4llu  %4llu  %4llu\n",
              (unsigned long long) tetnumber,
              (unsigned long long) vertexptr0->number,
              (unsigned long long) vertexptr1->number,
              (unsigned long long) vertexptr2->number,
              (unsigned long long) vertexptr3->number);
    } else {
      fprintf(outfile, "%4lu    %4lu  %4lu  %4lu  %4lu\n",
              (unsigned long) tetnumber,
              (unsigned long) vertexptr0->number,
              (unsigned long) vertexptr1->number,
              (unsigned long) vertexptr2->number,
              (unsigned long) vertexptr3->number);
    }
#else
    fprintf(outfile, "%4lu    %4lu  %4lu  %4lu  %4lu\n",
            (unsigned long) tetnumber,
            (unsigned long) vertexptr0->number,
            (unsigned long) vertexptr1->number,
            (unsigned long) vertexptr2->number,
            (unsigned long) vertexptr3->number);
#endif

    tetcomplexiteratenoghosts(&position, tet);
    tetnumber++;
  }

  outputfilefinish(outfile, argc, argv);
}

/*****************************************************************************/
/*                                                                           */
/*  outputedges()   Write the edges to an .edge file.                        */
/*                                                                           */
/*****************************************************************************/

void outputedges(struct behavior *behave,
                 struct inputs *in,
                 struct tetcomplex *plex,
                 arraypoolulong edgecount,
                 int argc,
                 char **argv)
{
  struct tetcomplexposition position;
  struct proxipool *pool;
  struct vertex *originptr;
  struct vertex *destinptr;
  tag tet[4];
  tag adjacencies[2];
  FILE *outfile;
  tag origin;
  tag destin;
  tag apex;
  tag stopvtx;
  tag searchvtx;
  tag swaptag;
  arraypoolulong edgenumber;
  int writeflag;
  int i;

  if (!behave->quiet) {
    printf("Writing %s.\n", behave->edgefilename);
  }

  outfile = fopen(behave->edgefilename, "w");
  if (outfile == (FILE *) NULL) {
    printf("  Error:  Cannot create file %s.\n", behave->edgefilename);
    starexit(1);
  }

  /* Number of edges, number of boundary markers (zero or one). */
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
  if (sizeof(arraypoolulong) > sizeof(long)) {
    fprintf(outfile, "%llu  %d\n", (unsigned long long) edgecount,
            1 - behave->nobound);
  } else {
    fprintf(outfile, "%lu  %d\n", (unsigned long) edgecount,
            1 - behave->nobound);
  }
#else
  fprintf(outfile, "%lu  %d\n", (unsigned long) edgecount,
          1 - behave->nobound);
#endif

  pool = plex->vertexpool;
  edgenumber = (arraypoolulong) behave->firstnumber;

  /* Iterate through all the (non-ghost) tetrahedra in the complex. */
  tetcomplexiteratorinit(plex, &position);
  tetcomplexiteratenoghosts(&position, tet);
  while (tet[0] != STOP) {
    /* Look at all six edges of the tetrahedron. */
    for (i = 0; i < 6; i++) {
      if (tet[0] < tet[1]) {
        origin = tet[0];
        destin = tet[1];
        apex = tet[2];
        stopvtx = tet[3];
      } else {
        origin = tet[1];
        destin = tet[0];
        apex = tet[3];
        stopvtx = tet[2];
      }

      searchvtx = apex;
      writeflag = 1;
      do {
        if (!tetcomplexadjacencies(plex, origin, destin, searchvtx,
                                   adjacencies)) {
          printf("Internal error in outputedges():\n");
          printf("  Complex returned tetrahedron that can't be queried.\n");
          internalerror();
        }
        if (adjacencies[0] == GHOSTVERTEX) {
          writeflag = searchvtx == apex;
        }
        searchvtx = adjacencies[0];
        if (searchvtx < apex) {
          writeflag = 0;
        }
      } while (writeflag &&
               (searchvtx != stopvtx) && (searchvtx != GHOSTVERTEX));

      if (writeflag) {
        originptr = (struct vertex *) proxipooltag2object(pool, origin);
        destinptr = (struct vertex *) proxipooltag2object(pool, destin);

        /* Edge number and indices for two vertices. */
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
        if (sizeof(arraypoolulong) > sizeof(long)) {
          fprintf(outfile, "%4llu    %4llu  %4llu",
                  (unsigned long long) edgenumber,
                  (unsigned long long) originptr->number,
                  (unsigned long long) destinptr->number);
        } else {
          fprintf(outfile, "%4lu    %4lu  %4lu",
                  (unsigned long) edgenumber,
                  (unsigned long) originptr->number,
                  (unsigned long) destinptr->number);
        }
#else
        fprintf(outfile, "%4lu    %4lu  %4lu",
                (unsigned long) edgenumber,
                (unsigned long) originptr->number,
                (unsigned long) destinptr->number);
#endif
        if (behave->nobound) {
          putc('\n', outfile);
        } else if (searchvtx == GHOSTVERTEX) {
          fputs("  1\n", outfile);
        } else {
          fputs("  0\n", outfile);
        }

        edgenumber++;
      }

      /* The following shift cycles (tet[0], tet[1]) through all the edges   */
      /*   while maintaining the tetrahedron's orientation.  The schedule is */
      /*   i = 0:  0 1 2 3 => 1 2 0 3                                        */
      /*   i = 1:  1 2 0 3 => 1 3 2 0                                        */
      /*   i = 2:  1 3 2 0 => 3 2 1 0                                        */
      /*   i = 3:  3 2 1 0 => 3 0 2 1                                        */
      /*   i = 4:  3 0 2 1 => 0 2 3 1                                        */
      /*   i = 5:  0 2 3 1 => 0 1 2 3 (which isn't used).                    */
      if ((i & 1) == 0) {
        swaptag = tet[0];
        tet[0] = tet[1];
        tet[1] = tet[2];
        tet[2] = swaptag;
      } else {
        swaptag = tet[3];
        tet[3] = tet[2];
        tet[2] = tet[1];
        tet[1] = swaptag;
      }
    }

    tetcomplexiteratenoghosts(&position, tet);
  }

  outputfilefinish(outfile, argc, argv);
}

/*****************************************************************************/
/*                                                                           */
/*  outputfaces()   Write the triangular faces to a .face file.              */
/*                                                                           */
/*****************************************************************************/

void outputfaces(struct behavior *behave,
                 struct inputs *in,
                 struct tetcomplex *plex,
                 arraypoolulong facecount,
                 int argc,
                 char **argv)
{
  struct tetcomplexposition position;
  struct proxipool *pool;
  struct vertex *vertexptr0;
  struct vertex *vertexptr1;
  struct vertex *vertexptr2;
  tag tet[4];
  tag adjacencies[2];
  FILE *outfile;
  arraypoolulong facenumber;
  int i;

  if (!behave->quiet) {
    printf("Writing %s.\n", behave->facefilename);
  }

  outfile = fopen(behave->facefilename, "w");
  if (outfile == (FILE *) NULL) {
    printf("  Error:  Cannot create file %s.\n", behave->facefilename);
    starexit(1);
  }

  /* Number of triangular faces, number of boundary markers (zero or one). */
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
  if (sizeof(arraypoolulong) > sizeof(long)) {
    fprintf(outfile, "%llu  %d\n", (unsigned long long) facecount,
            1 - behave->nobound);
  } else {
    fprintf(outfile, "%lu  %d\n", (unsigned long) facecount,
            1 - behave->nobound);
  }
#else
  fprintf(outfile, "%lu  %d\n", (unsigned long) facecount,
          1 - behave->nobound);
#endif

  pool = plex->vertexpool;
  facenumber = (arraypoolulong) behave->firstnumber;

  /* Iterate through all the (non-ghost) tetrahedra in the complex. */
  tetcomplexiteratorinit(plex, &position);
  tetcomplexiteratenoghosts(&position, tet);
  while (tet[0] != STOP) {
    /* Look at all four faces of the tetrahedron. */
    for (i = 0; i < 4; i++) {
      if (!tetcomplexadjacencies(plex, tet[i ^ 1], tet[i ^ 2], tet[i ^ 3],
                                 adjacencies)) {
        printf("Internal error in outputfaces():\n");
        printf("  Iterator returned tetrahedron that can't be queried.\n");
        internalerror();
      }

      /* `adjacencies[1]' is the apex of the tetrahedron adjoining this   */
      /*   tetrahedron on the face (tet[i ^ 1], tet[i ^ 2], tet[i ^ 3]).  */
      /*   So that each face is written only once, write the face if this */
      /*   tetrahedron's apex tag is smaller than the neighbor's.  (Note  */
      /*   that the ghost tetrahedron has the largest tag of all.)        */
      if (tet[i] < adjacencies[1]) {
        /* The vertices of the face are written in counterclockwise order */
        /*   as viewed from _outside_ this tetrahedron.                   */
        vertexptr0 = (struct vertex *) proxipooltag2object(pool, tet[i ^ 3]);
        vertexptr1 = (struct vertex *) proxipooltag2object(pool, tet[i ^ 2]);
        vertexptr2 = (struct vertex *) proxipooltag2object(pool, tet[i ^ 1]);

        /* Face number and indices for three vertices. */
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
        if (sizeof(arraypoolulong) > sizeof(long)) {
          fprintf(outfile, "%4llu    %4llu  %4llu  %4llu",
                  (unsigned long long) facenumber,
                  (unsigned long long) vertexptr0->number,
                  (unsigned long long) vertexptr1->number,
                  (unsigned long long) vertexptr2->number);
        } else {
          fprintf(outfile, "%4lu    %4lu  %4lu  %4lu",
                  (unsigned long) facenumber,
                  (unsigned long) vertexptr0->number,
                  (unsigned long) vertexptr1->number,
                  (unsigned long) vertexptr2->number);
        }
#else
        fprintf(outfile, "%4lu    %4lu  %4lu  %4lu",
                (unsigned long) facenumber,
                (unsigned long) vertexptr0->number,
                (unsigned long) vertexptr1->number,
                (unsigned long) vertexptr2->number);
#endif
        if (behave->nobound) {
          putc('\n', outfile);
        } else if (adjacencies[1] == GHOSTVERTEX) {
          fputs("  1\n", outfile);
        } else {
          fputs("  0\n", outfile);
        }

        facenumber++;
      }
    }

    tetcomplexiteratenoghosts(&position, tet);
  }

  outputfilefinish(outfile, argc, argv);
}

/**                                                                         **/
/**                                                                         **/
/********* Output routines end here                                  *********/
