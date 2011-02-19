/*****************************************************************************/
/*                                                                           */
/*    ,d88""\ 888                                   o    o                   */
/*    8888    888o888,  o88""o Y88b    o    /      d8b  d8b      o88888o     */
/*    "Y88b   888  888 d888   b Y88b  d8b  /      d888bdY88b    d888  88b    */
/*     "Y88b, 888  888 8888   8  Y888/Y88b/      / Y88Y Y888b   8888oo888    */
/*       8888 888  888 q888   p   Y8/  Y8/      /   YY   Y888b  q888         */
/*    \_o88P' 888  888  "88oo"     Y    Y      /          Y888b  "88oooo"    */
/*                                                                           */
/*  A Display Program for Meshes and More.                                   */
/*  (showme.c)                                                               */
/*                                                                           */
/*  Version 1.61                                                             */
/*  5 July 2009                                                              */
/*                                                                           */
/*  Portions of Show Me written prior to June 30, 1998 are                   */
/*  Copyright 1995, 1996, 1998                                               */
/*  Jonathan Richard Shewchuk                                                */
/*  965 Sutter Street #815                                                   */
/*  San Francisco, California  94109-6082                                    */
/*  jrs@cs.berkeley.edu                                                      */
/*                                                                           */
/*  Portions of Show Me written after June 30, 1998 are in the public        */
/*  domain, but Show Me as a whole is not.  All rights reserved.             */
/*                                                                           */
/*  This version of Show Me is provided as part of Stellar, a program for    */
/*  improving tetrahedral meshes.  Stellar and this version of Show Me are   */
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
/*  Hypertext instructions for Show Me will some day be available at         */
/*                                                                           */
/*      http://www.cs.cmu.edu/~quake/showme.html                             */
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
/*  For double precision (which will allow you to display finer meshes),     */
/*  leave SINGLE undefined.                                                  */

/* #define SINGLE */

#ifdef SINGLE
typedef float showmereal;
#else /* not SINGLE */
typedef double showmereal;
#endif /* not SINGLE */

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

/* On some machines, the exact arithmetic routines might be defeated by the  */
/*   use of internal extended precision floating-point registers.  Sometimes */
/*   this problem can be fixed by defining certain values to be volatile,    */
/*   thus forcing them to be stored to memory and rounded off.  This isn't   */
/*   a good solution, though, as it slows Pyramid down.                      */
/*                                                                           */
/* To try this out, write "#define INEXACT volatile" below.  Normally,       */
/*   however, INEXACT should be defined to be nothing.  ("#define INEXACT".) */

#define INEXACT /* Nothing */
/* #define INEXACT volatile */

/*  Maximum number of characters in a file name (including the null).        */

#define FILENAMESIZE 2048

/*  Maximum number of characters in a line read from a file (including the   */
/*  null).                                                                   */

#define INPUTLINESIZE 1024

#define STARTWIDTH 414
#define STARTHEIGHT 414
#define MINWIDTH 50
#define MINHEIGHT 50
#define BUTTONHEIGHT 21
#define BUTTONROWS 4
#define PANELHEIGHT (BUTTONHEIGHT * BUTTONROWS)
#define MAXCOLORS 64
#define MAXGRAYS 64

#define IMAGE_TYPES 8
#define NOTHING -1
#define NODE 0
#define POLY 1
#define ELE 2
#define EDGE 3
#define PART 4
#define ADJ 5
#define VORO 6
#define DATA 7

#define TRIMLEFT 0
#define TRIMRIGHT 1
#define TRIMUP 2
#define TRIMDOWN 3

#define STARTEXPLOSION 0.5

#define DEG2RAD 0.0174532925199433
#define ONETHIRD 0.333333333333333333333333333333333333333333333333333333333333

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#ifdef CPU86
#include <float.h>
#endif /* CPU86 */
#ifdef LINUX
#include <fpu_control.h>
#endif /* LINUX */


/*  `showmelong' and `showmeulong' are the types of integer (signed and      */
/*  unsigned, respectively) of most of the indices used internally and       */
/*  externally by ShowMe, including vertex and tetrahedron numbers.  They    */
/*  determine the number of internal data structures that can be allocated,  */
/*  so long choices (e.g. ptrdiff_t and size_t, defined in stddef.h) are     */
/*  recommended.  If the number of tetrahedra might be around 2^28 or more,  */
/*  use 64-bit integers.  On a machine with 32-bit pointers (memory          */
/*  addresses), though, there's no point using integers bigger than 32 bits. */
/*  On a machine with limited memory, smaller integers might allow you to    */
/*  create larger meshes.                                                    */

typedef ptrdiff_t showmelong;
typedef size_t showmeulong;


struct tetra {
  /* Which tetrahedron is opposite vertex 0-3?  Zero means none.  A number */
  /* less than zero means it's -neighbor[i], but the neighbor has been     */
  /* marked invisible.                                                     */
  showmelong neighbor[4];
  showmelong nexttet;        /* Links list of tetrahedra with exposed faces. */
  int invisible;         /* True if this tetrahedron should not be rendered. */
};

/*  A necessary forward declaration.                                         */

int load_image();

/*  Global constants.                                                        */

showmereal splitter; /* Used to split real factors for exact multiplication. */
showmereal epsilon;                       /* Floating-point machine epsilon. */
showmereal o3derrboundA, o3derrboundB, o3derrboundC, resulterrbound;


struct tetra *tetraptr;

Display *display;
int screen;
Window rootwindow;
Window mainwindow;
Window quitwin;
Window leftwin;
Window rightwin;
Window upwin;
Window downwin;
Window resetwin;
Window pswin;
Window epswin;
Window expwin;
Window exppluswin;
Window expminuswin;
Window widthpluswin;
Window widthminuswin;
Window versionpluswin;
Window versionminuswin;
Window fillwin;
Window motionwin;
Window rotatewin[6];
Window rotateamtwin;
Window wireframeoptionwin;
Window perspectivewin;
Window perspluswin;
Window persminuswin;
Window cutleftwin;
Window cutrightwin;
Window cutupwin;
Window cutdownwin;
Window qualitywin;
Window nodewin[2];
Window polywin[2];
Window elewin[2];
Window edgewin[2];
Window partwin[2];
Window adjwin[2];
Window voronoiwin[2];
Window datawin[2];

int windowdepth;
XEvent event;
Colormap rootmap;
XFontStruct *myfont;
unsigned int width, height;
int black, white;
unsigned int showme_foreground;
GC fontgc;
GC blackfontgc;
GC linegc;
GC trianglegc;
int bitreverse[MAXCOLORS];
unsigned int colors[MAXCOLORS];
XColor rgb[MAXCOLORS];
unsigned int grays[MAXGRAYS];
XColor gray[MAXGRAYS];
int colordisplay;
int wireframe = 0;
int perspective = 0;
int motion = 0;

int start_image, current_image;
int start_inc, current_inc;
int loweriteration;
unsigned int line_width;
int loaded[2][IMAGE_TYPES];
showmereal xlo[2][IMAGE_TYPES], ylo[2][IMAGE_TYPES], zlo[2][IMAGE_TYPES];
showmereal xhi[2][IMAGE_TYPES], yhi[2][IMAGE_TYPES], zhi[2][IMAGE_TYPES];
showmereal xscale, yscale;
showmereal xoffset, yoffset;
int zoom;

showmelong nodes[2];
int nodedim[2];
showmereal *nodeptr[2];
showmelong polynodes[2];
int polydim[2];
showmelong polyedges[2], polyholes[2];
showmereal *polynodeptr[2], *polyholeptr[2];
showmelong *polysegptr[2];
showmelong elems[2];
int elecorners[2];
showmelong *eleptr[2];
showmelong edges[2];
showmelong *edgeptr[2];
showmereal *normptr[2];
int subdomains[2];
int *partition[2];
showmereal *subdomcenter[2], *subdomshift[2];
int adjsubdomains[2];
int *adjptr[2];
showmelong vnodes[2];
int vnodedim[2];
showmereal *vnodeptr[2];
showmelong vedges[2];
showmelong *vedgeptr[2];
showmereal *vnormptr[2];
showmelong datavalues[2];
int datadim[2];
showmereal *dataptr[2];
showmereal datahist[2][2]; /* Hi & Lo for adaptive scaling of 2D height info */
int firstnumber[2];

int quiet, fillelem, bw_ps, explode;
showmereal explosion;

unsigned long randomseed;                     /* Current random number seed. */

int dlist_drawn = 0;
showmereal amtLeft = 0.0, amtUp = 0.0;
showmereal lightsourcex = 0.1;
showmereal lightsourcey = 0.7937;
showmereal lightsourcez = 0.6;

char filename[FILENAMESIZE];
char nodefilename[2][FILENAMESIZE];
char polyfilename[2][FILENAMESIZE];
char elefilename[2][FILENAMESIZE];
char edgefilename[2][FILENAMESIZE];
char partfilename[2][FILENAMESIZE];
char adjfilename[2][FILENAMESIZE];
char vnodefilename[2][FILENAMESIZE];
char vedgefilename[2][FILENAMESIZE];
char datafilename[2][FILENAMESIZE];

/* Original colors */
/*
char *colorname[] = {"aquamarine", "red", "green yellow", "magenta",
                     "yellow", "green", "orange", "blue",
                     "white", "sandy brown", "cyan", "moccasin",
                     "cadet blue", "coral", "cornflower blue", "sky blue",
                     "firebrick", "forest green", "gold", "goldenrod",
                     "gray", "hot pink", "chartreuse", "pale violet red",
                     "indian red", "khaki", "lavender", "light blue",
                     "light gray", "light steel blue", "lime green", "azure",
                     "maroon", "medium aquamarine", "dodger blue", "honeydew",
                     "medium orchid", "medium sea green", "moccasin",
                     "medium slate blue", "medium spring green",
                     "medium turquoise", "medium violet red",
                     "orange red", "chocolate", "light goldenrod",
                     "orchid", "pale green", "pink", "plum",
                     "purple", "salmon", "sea green",
                     "sienna", "slate blue", "spring green",
                     "steel blue", "tan", "thistle", "turquoise",
                     "violet", "violet red", "wheat",
                     "yellow green"};
*/

char *colorname[] = {"green4", "green3", "green2", "chartreuse2",
                     "OliveDrab2", "DarkOliveGreen2",
                     "SpringGreen3", "SpringGreen2",
                     "SeaGreen2", "DarkSeaGreen2", "aquamarine2",
                     "SlateGray2", "LightSteelBlue2",
                     "LightSkyBlue2", "LightBlue2",
                     "LightCyan2", "PaleTurquoise2", "CadetBlue2",
                     "turquoise2", "cyan2", "cyan3",
                     "SkyBlue2", "DeepSkyBlue2", "SteelBlue2",
                     "DodgerBlue2", "RoyalBlue2",
                     "RoyalBlue1", "royal blue",
                     "blue1", "blue2",
                     "medium blue", "midnight blue", "navy", "blue4",
                     "dark slate blue", "purple3", "purple2", "purple1",
                     "MediumPurple2", "MediumPurple3",
                     "DarkOrchid2", "DarkOrchid3",
                     "red4", "red3", "DeepPink3",
                     "maroon3", "magenta3", "maroon2", "magenta2",
                     "DeepPink2", "OrangeRed3", "red2", "red", "orange red",
                     "DarkOrange2", "chocolate2", "tan2", "DarkGoldenRod2",
                     "goldenrod2", "gold2", "yellow3", "yellow2",
                     "LightGoldenrod2", "khaki2"};



/* This section is matrix support for 3D graphics. */

struct vec_t {
  showmereal x, y, z;
};

struct int_t {
  int x, y, z;
};

typedef showmereal matrix_t[3][3];
typedef showmereal vector_t[3];

matrix_t viewmatrix; /* The current viewing transformation matrix */
showmereal xcenter, ycenter, zcenter;

void three_D_rot_matrix_x(matrix_t matrix,
                          showmereal theta)
{
  matrix[0][0] = 1.0;  matrix[0][1] = 0.0;         matrix[0][2] = 0.0;
  matrix[1][0] = 0.0;  matrix[1][1] = cos(theta);  matrix[1][2] = - sin(theta);
  matrix[2][0] = 0.0;  matrix[2][1] = sin(theta);  matrix[2][2] = cos(theta);
}

void three_D_rot_matrix_y(matrix_t matrix,
                          showmereal theta)
{
  matrix[0][0] = cos(theta);    matrix[0][1] = 0.0;  matrix[0][2] = sin(theta);
  matrix[1][0] = 0.0;           matrix[1][1] = 1.0;  matrix[1][2] = 0.0;
  matrix[2][0] = - sin(theta);  matrix[2][1] = 0.0;  matrix[2][2] = cos(theta);
}


void three_D_rot_matrix_z(matrix_t matrix,
                          showmereal theta)
{
  matrix[0][0] = cos(theta);  matrix[0][1] = - sin(theta);  matrix[0][2] = 0.0;
  matrix[1][0] = sin(theta);  matrix[1][1] = cos(theta);    matrix[1][2] = 0.0;
  matrix[2][0] = 0.0;         matrix[2][1] = 0.0;           matrix[2][2] = 1.0;
}

void mult_matmat(matrix_t inmatrix1,
                 matrix_t inmatrix2,
                 matrix_t outmatrix)
{
  showmereal
    a1 = inmatrix1[0][0], a2 = inmatrix1[0][1], a3 = inmatrix1[0][2],
    a4 = inmatrix1[1][0], a5 = inmatrix1[1][1], a6 = inmatrix1[1][2],
    a7 = inmatrix1[2][0], a8 = inmatrix1[2][1], a9 = inmatrix1[2][2];
  showmereal
    b1 = inmatrix2[0][0], b2 = inmatrix2[0][1], b3 = inmatrix2[0][2],
    b4 = inmatrix2[1][0], b5 = inmatrix2[1][1], b6 = inmatrix2[1][2],
    b7 = inmatrix2[2][0], b8 = inmatrix2[2][1], b9 = inmatrix2[2][2];

  outmatrix[0][0] = b1 * a1 + b4 * a2 + b7 * a3;
  outmatrix[1][0] = b1 * a4 + b4 * a5 + b7 * a6;
  outmatrix[2][0] = b1 * a7 + b4 * a8 + b7 * a9;

  outmatrix[0][1] = b2 * a1 + b5 * a2 + b8 * a3;
  outmatrix[1][1] = b2 * a4 + b5 * a5 + b8 * a6;
  outmatrix[2][1] = b2 * a7 + b5 * a8 + b8 * a9;

  outmatrix[0][2] = b3 * a1 + b6 * a2 + b9 * a3;
  outmatrix[1][2] = b3 * a4 + b6 * a5 + b9 * a6;
  outmatrix[2][2] = b3 * a7 + b6 * a8 + b9 * a9;
}

void mult_matvec(matrix_t inmatrix,
                 vector_t invector,
                 vector_t outvector)
{
  showmereal
    a1 = inmatrix[0][0], a2 = inmatrix[0][1], a3 = inmatrix[0][2],
    a4 = inmatrix[1][0], a5 = inmatrix[1][1], a6 = inmatrix[1][2],
    a7 = inmatrix[2][0], a8 = inmatrix[2][1], a9 = inmatrix[2][2];
  showmereal b1 = invector[0], b2 = invector[1], b3 = invector[2];

  outvector[0] = a1 * b1 + a2 * b2 + a3 * b3;
  outvector[1] = a4 * b1 + a5 * b2 + a6 * b3;
  outvector[2] = a7 * b1 + a8 * b2 + a9 * b3;
}

void identitymatrix(matrix_t matrix)
{
  matrix[0][0] = 1.0;  matrix[0][1] = 0.0;  matrix[0][2] = 0.0;
  matrix[1][0] = 0.0;  matrix[1][1] = 1.0;  matrix[1][2] = 0.0;
  matrix[2][0] = 0.0;  matrix[2][1] = 0.0;  matrix[2][2] = 1.0;
}

/* This section supports 3D perspective projection. */

showmereal Camera_Twist = 0.0;    /* Amount to twist about the viewing axis. */

/*  Calculate the distance according to the field of view */
/*#define FOV (130.0 * DEG2RAD)*/
/*static showmereal d = 1.0/(2.0*tan(FOV/2.0));*/

showmereal perspdistance;
showmereal perspfactor = 1.5;

/*
 *  PerspectiveProj:
 *     Take a point in (the camera's) 3-space and map it to (World) x-y space
 *     using LookFrom, LookAt, FOV,  and Oroll(about the viewing axis).
 */

void perspectiveproj(vector_t CAMcoord,
                     struct vec_t *World)
{
  showmereal viewz;
  /* Translate the Camera Point to the Origin first. */
  viewz = CAMcoord[2] + perspdistance;

  if (perspective) {
    World->x = CAMcoord[0] / viewz;
    World->y = CAMcoord[1] / viewz;
    World->z = 0.0;
  } else {
    World->x = CAMcoord[0];
    World->y = CAMcoord[1];
    World->z = CAMcoord[2];
  }
}

/* End graphics projection support */

/*****************************************************************************/
/*                                                                           */
/*  syntax()   Print a list of command line switches.                        */
/*                                                                           */
/*****************************************************************************/

void syntax()
{
  printf("showme [-bfw_Qh] input_file\n");
  printf("    -b  Black and white PostScript (default is color).\n");
  printf("    -f  Fill triangles of partitioned mesh with color.\n");
  printf("    -w  Set line width to some specified number.\n");
  printf("    -Q  Quiet:  No terminal output except errors.\n");
  printf("    -h  Help:  Detailed instructions for Show Me.\n");
  exit(0);
}

/*****************************************************************************/
/*                                                                           */
/*  info()   Print out complete instructions.                                */
/*                                                                           */
/*****************************************************************************/

void info()
{
  printf("Show Me\n");
  printf("A Display Program for Meshes and More.\n");
  printf("Version 1.61\n\n");
  printf("Copyright 1996, 1998 Jonathan Richard Shewchuk\n");
  printf("965 Sutter Street #815 / San Francisco, California  94109-6082\n");
  printf("Bugs/comments to jrs@cs.berkeley.edu\n");
  printf(
"Created as part of the Quake project (tools for earthquake simulation).\n");
  printf(
"Supported in part by NSF Grant CMS-9318163 and an NSERC 1967 Scholarship.\n");
  printf("There is no warranty whatsoever.  Use at your own risk.\n");
#ifdef SINGLE
  printf("This executable is compiled for single precision arithmetic.\n\n\n");
#else
  printf("This executable is compiled for double precision arithmetic.\n\n\n");
#endif
  printf(
"Show Me graphically displays the contents of geometric files, especially\n");
  printf(
"those generated by Triangle, my two-dimensional quality mesh generator and\n"
);
  printf(
"Delaunay triangulator.  Show Me can also write images in PostScript form.\n");
  printf(
"Show Me is also useful for checking the consistency of the files you create\n"
);
  printf(
"as input to Triangle; Show Me does these checks more thoroughly than\n");
  printf("Triangle does.  The command syntax is:\n\n");
  printf("showme [-bfw_Qh] input_file\n\n");
  printf(
"The underscore indicates that a number should follow the -w switch.\n");
  printf(
"input_file may be one of several types of file.  It must have extension\n");
  printf(
".node, .poly, .ele, .edge, .part, or .adj.  If no extension is provided,\n");
  printf(
"Show Me will assume the extension .ele.  A .node file represents a set of\n");
  printf(
"points; a .poly file represents a Planar Straight Line Graph; an .ele file\n"
);
  printf(
"(coupled with a .node file) represents the elements of a mesh or the\n");
  printf(
"triangles of a triangulation; an .edge file (coupled with a .node file)\n");
  printf(
"represents a set of edges; a .part file specifies a partition of a mesh;\n");
  printf(
"and a .adj file represents the adjacency graph defined by a partition.\n");
  printf("\n");
  printf("Command Line Switches:\n");
  printf("\n");
  printf(
"    -b  Makes all PostScript output black and white.  If this switch is not\n"
);
  printf(
"        selected, color PostScript is used for partitioned meshes and\n");
  printf("        adjacency graphs (.part and .adj files).\n");
  printf(
"    -f  On color displays and in color PostScript, displays partitioned\n");
  printf(
"        meshes by filling triangles with color, rather than by coloring the\n"
);
  printf(
"        edges.  This switch will result in a clearer picture if all\n");
  printf(
"        triangles are reasonably large, and a less clear picture if small\n");
  printf(
"        triangles are present.  (There is also a button to toggle this\n");
  printf("        behavior.)\n");
  printf(
"    -w  Followed by an integer, specifies the line width used in all\n");
  printf(
"        images.  (There are also buttons to change the line width.)\n");
  printf(
"    -Q  Quiet:  Suppresses all explanation of what Show Me is doing, unless\n"
);
  printf("        an error occurs.\n");
  printf("    -h  Help:  Displays these instructions.\n");
  printf("\n");
  printf("Controls:\n");
  printf("\n");
  printf(
"  To zoom in on an image, point at the location where you want a closer\n");
  printf(
"  look, and click the left mouse button.  To zoom out, click the right\n");
  printf(
"  mouse button.  In either case, the point you click on will be centered in\n"
);
  printf(
"  the window.  If you want to know the coordinates of a point, click the\n");
  printf(
"  middle mouse button; the coordinates will be printed on the terminal you\n"
);
  printf("  invoked Show Me from.\n\n");
  printf(
"  If you resize the window, the image will grow or shrink to match.\n");
  printf("\n");
  printf(
"  There is a panel of control buttons at the bottom of the Show Me window:\n"
);
  printf("\n");
  printf("  Quit:  Shuts down Show Me.\n");
  printf("  <, >, ^, v:  Moves the image in the indicated direction.\n");
  printf(
"  Reset: Unzooms and centers the image in the window.  When you switch from\n"
);
  printf(
"    one image to another, the viewing region does not change, so you may\n");
  printf(
"    need to reset the new image to make it fully visible.  This often is\n");
  printf(
"    the case when switching between Delaunay triangulations and their\n");
  printf(
"    corresponding Voronoi diagrams, as Voronoi vertices can be far from the\n"
);
  printf("    initial point set.\n");
  printf(
"  Width+, -:  Increases or decreases the width of all lines and points.\n");
  printf(
"  Exp, +, -:  These buttons appear only when you are viewing a partitioned\n"
);
  printf(
"    mesh (.part file).  `Exp' toggles between an exploded and non-exploded\n"
);
  printf(
"    image of the mesh.  The non-exploded image will not show the partition\n"
);
  printf(
"    on a black and white monitor.  `+' and `-' allow you to adjust the\n");
  printf(
"    spacing between pieces of the mesh to better distinguish them.\n");
  printf(
"  Fill:  This button appears only when you are viewing a partitioned mesh\n");
  printf(
"    (.part file).  It toggles between color-filled triangles and colored\n");
  printf(
"    edges (as the -f switch does).  Filled triangles look better when all\n");
  printf(
"    triangles are reasonably large; colored edges look better when there\n");
  printf("    are very small triangles present.\n");
  printf(
"  PS:  Creates a PostScript file containing the image you are viewing.  If\n"
);
  printf(
"    the -b switch is selected, all PostScript output will be black and\n");
  printf(
"    white; otherwise, .part.ps and .adj.ps files will be color, independent\n"
);
  printf(
"    of whether you are using a color monitor.  Normally the output will\n");
  printf(
"    preserve the properties of the image you see on the screen, including\n");
  printf(
"    zoom and line width; however, if black and white output is selected (-b\n"
);
  printf(
"    switch), partitioned meshes will always be drawn exploded.  The output\n"
);
  printf(
"    file name depends on the image being viewed.  If you want several\n");
  printf(
"    different snapshots (zooming in on different parts) of the same object,\n"
);
  printf(
"    you'll have to rename each file after Show Me creates it so that it\n");
  printf("    isn't overwritten by the next snapshot.\n");
  printf(
"  EPS:  Creates an encapsulated PostScript file, suitable for inclusion in\n"
);
  printf(
"    documents.  Otherwise, this button is just like the PS button.  (The\n");
  printf(
"    only difference is that .eps files lack a `showpage' command at the\n");
  printf("    end.)\n\n");
  printf(
"  There are two nearly-identical rows of buttons that load different images\n"
);
  printf("  from disk.  Each row contains the following buttons:\n\n");
  printf("  node:  Loads a .node file.\n");
  printf(
"  poly:  Loads a .poly file (and possibly an associated .node file).\n");
  printf("  ele:  Loads an .ele file (and associated .node file).\n");
  printf("  edge:  Loads an .edge file (and associated .node file).\n");
  printf(
"  part:  Loads a .part file (and associated .node and .ele files).\n");
  printf(
"  adj:  Loads an .adj file (and associated .node, .ele, and .part files).\n");
  printf("  voro:  Loads a .v.node and .v.edge file for a Voronoi diagram.\n");
  printf("\n");
  printf(
"  Each row represents a different iteration number of the geometry files.\n");
  printf(
"  For a full explanation of iteration numbers, read the instructions for\n");
  printf(
"  Triangle.  Briefly, iteration numbers are used to allow a user to easily\n"
);
  printf(
"  represent a sequence of related triangulations.  Iteration numbers are\n");
  printf(
"  used in the names of geometry files; for instance, mymesh.3.ele is a\n");
  printf(
"  triangle file with iteration number three, and mymesh.ele has an implicit\n"
);
  printf("  iteration number of zero.\n\n");
  printf(
"  The control buttons at the right end of each row display the two\n");
  printf(
"  iterations currently under view.  These buttons can be clicked to\n");
  printf(
"  increase or decrease the iteration numbers, and thus conveniently view\n");
  printf("  a sequence of meshes.\n\n");
  printf(
"  Show Me keeps each file in memory after loading it, but you can force\n");
  printf(
"  Show Me to reread a set of files (for one iteration number) by reclicking\n"
);
  printf(
"  the button that corresponds to the current image.  This is convenient if\n"
);
  printf("  you have changed a geometry file.\n\n");
  printf("File Formats:\n\n");
  printf(
"  All files may contain comments prefixed by the character '#'.  Points,\n");
  printf(
"  segments, holes, triangles, edges, and subdomains must be numbered\n");
  printf(
"  consecutively, starting from either 1 or 0.  Whichever you choose, all\n");
  printf(
"  input files must be consistent (for any single iteration number); if the\n"
);
  printf(
"  nodes are numbered from 1, so must be all other objects.  Show Me\n");
  printf(
"  automatically detects your choice while reading a .node (or .poly) file.\n"
);
  printf("  Examples of these file formats are given below.\n\n");
  printf("  .node files:\n");
  printf(
"    First line:  <# of points> <dimension (must be 2)> <# of attributes>\n");
  printf(
"                                           <# of boundary markers (0 or 1)>\n"
);
  printf(
"    Remaining lines:  <point #> <x> <y> [attributes] [boundary marker]\n");
  printf("\n");
  printf(
"    The attributes, which are typically floating-point values of physical\n");
  printf(
"    quantities (such as mass or conductivity) associated with the nodes of\n"
);
  printf(
"    a finite element mesh, are ignored by Show Me.  Show Me also ignores\n");
  printf(
"    boundary markers.  See the instructions for Triangle to find out what\n");
  printf("    attributes and boundary markers are.\n\n");
  printf("  .poly files:\n");
  printf(
"    First line:  <# of points> <dimension (must be 2)> <# of attributes>\n");
  printf(
"                                           <# of boundary markers (0 or 1)>\n"
);
  printf(
"    Following lines:  <point #> <x> <y> [attributes] [boundary marker]\n");
  printf("    One line:  <# of segments> <# of boundary markers (0 or 1)>\n");
  printf(
"    Following lines:  <segment #> <endpoint> <endpoint> [boundary marker]\n");
  printf("    One line:  <# of holes>\n");
  printf("    Following lines:  <hole #> <x> <y>\n");
  printf("    [Optional additional lines that are ignored]\n\n");
  printf(
"    A .poly file represents a Planar Straight Line Graph (PSLG), an idea\n");
  printf(
"    familiar to computational geometers.  By definition, a PSLG is just a\n");
  printf(
"    list of points and edges.  A .poly file also contains some additional\n");
  printf("    information.\n\n");
  printf(
"    The first section lists all the points, and is identical to the format\n"
);
  printf(
"    of .node files.  <# of points> may be set to zero to indicate that the\n"
);
  printf(
"    points are listed in a separate .node file; .poly files produced by\n");
  printf(
"    Triangle always have this format.  When Show Me reads such a file, it\n");
  printf("    also reads the corresponding .node file.\n\n");
  printf(
"    The second section lists the segments.  Segments are edges whose\n");
  printf(
"    presence in a triangulation produced from the PSLG is enforced.  Each\n");
  printf(
"    segment is specified by listing the indices of its two endpoints.  This\n"
);
  printf(
"    means that its endpoints must be included in the point list.  Each\n");
  printf(
"    segment, like each point, may have a boundary marker, which is ignored\n"
);
  printf("    by Show Me.\n\n");
  printf(
"    The third section lists holes and concavities that are desired in any\n");
  printf(
"    triangulation generated from the PSLG.  Holes are specified by\n");
  printf("    identifying a point inside each hole.\n\n");
  printf("  .ele files:\n");
  printf(
"    First line:  <# of triangles> <points per triangle> <# of attributes>\n");
  printf(
"    Remaining lines:  <triangle #> <point> <point> <point> ... [attributes]\n"
);
  printf("\n");
  printf(
"    Points are indices into the corresponding .node file.  Show Me ignores\n"
);
  printf(
"    all but the first three points of each triangle; these should be the\n");
  printf(
"    corners listed in counterclockwise order around the triangle.  The\n");
  printf("    attributes are ignored by Show Me.\n\n");
  printf("  .edge files:\n");
  printf("    First line:  <# of edges> <# of boundary markers (0 or 1)>\n");
  printf(
"    Following lines:  <edge #> <endpoint> <endpoint> [boundary marker]\n");
  printf("\n");
  printf(
"    Endpoints are indices into the corresponding .node file.  The boundary\n"
);
  printf("    markers are ignored by Show Me.\n\n");
  printf(
"    In Voronoi diagrams, one also finds a special kind of edge that is an\n");
  printf(
"    infinite ray with only one endpoint.  For these edges, a different\n");
  printf("    format is used:\n\n");
  printf("        <edge #> <endpoint> -1 <direction x> <direction y>\n\n");
  printf(
"    The `direction' is a floating-point vector that indicates the direction\n"
);
  printf("    of the infinite ray.\n\n");
  printf("  .part files:\n");
  printf("    First line:  <# of triangles> <# of subdomains>\n");
  printf("    Remaining lines:  <triangle #> <subdomain #>\n\n");
  printf(
"    The set of triangles is partitioned by a .part file; each triangle is\n");
  printf("    mapped to a subdomain.\n\n");
  printf("  .adj files:\n");
  printf("    First line:  <# of subdomains>\n");
  printf("    Remaining lines:  <adjacency matrix entry>\n\n");
  printf(
"    An .adj file represents adjacencies between subdomains (presumably\n");
  printf("    computed by a partitioner).  The first line is followed by\n");
  printf(
"    (subdomains X subdomains) lines, each containing one entry of the\n");
  printf(
"    adjacency matrix.  A nonzero entry indicates that two subdomains are\n");
  printf("    adjacent (share a point).\n\n");
  printf("Example:\n\n");
  printf(
"  Here is a sample file `box.poly' describing a square with a square hole:\n"
);
  printf("\n");
  printf(
"    # A box with eight points in 2D, no attributes, no boundary marker.\n");
  printf("    8 2 0 0\n");
  printf("    # Outer box has these vertices:\n");
  printf("     1   0 0\n");
  printf("     2   0 3\n");
  printf("     3   3 0\n");
  printf("     4   3 3\n");
  printf("    # Inner square has these vertices:\n");
  printf("     5   1 1\n");
  printf("     6   1 2\n");
  printf("     7   2 1\n");
  printf("     8   2 2\n");
  printf("    # Five segments without boundary markers.\n");
  printf("    5 0\n");
  printf("     1   1 2          # Left side of outer box.\n");
  printf("     2   5 7          # Segments 2 through 5 enclose the hole.\n");
  printf("     3   7 8\n");
  printf("     4   8 6\n");
  printf("     5   6 5\n");
  printf("    # One hole in the middle of the inner square.\n");
  printf("    1\n");
  printf("     1   1.5 1.5\n\n");
  printf(
"  After this PSLG is triangulated by Triangle, the resulting triangulation\n"
);
  printf(
"  consists of a .node and .ele file.  Here is the former, `box.1.node',\n");
  printf("  which duplicates the points of the PSLG:\n\n");
  printf("    8  2  0  0\n");
  printf("       1    0  0\n");
  printf("       2    0  3\n");
  printf("       3    3  0\n");
  printf("       4    3  3\n");
  printf("       5    1  1\n");
  printf("       6    1  2\n");
  printf("       7    2  1\n");
  printf("       8    2  2\n");
  printf("    # Generated by triangle -pcBev box\n");
  printf("\n");
  printf("  Here is the triangulation file, `box.1.ele'.\n");
  printf("\n");
  printf("    8  3  0\n");
  printf("       1       1     5     6\n");
  printf("       2       5     1     3\n");
  printf("       3       2     6     8\n");
  printf("       4       6     2     1\n");
  printf("       5       7     3     4\n");
  printf("       6       3     7     5\n");
  printf("       7       8     4     2\n");
  printf("       8       4     8     7\n");
  printf("    # Generated by triangle -pcBev box\n\n");
  printf("  Here is the edge file for the triangulation, `box.1.edge'.\n\n");
  printf("    16  0\n");
  printf("       1   1  5\n");
  printf("       2   5  6\n");
  printf("       3   6  1\n");
  printf("       4   1  3\n");
  printf("       5   3  5\n");
  printf("       6   2  6\n");
  printf("       7   6  8\n");
  printf("       8   8  2\n");
  printf("       9   2  1\n");
  printf("      10   7  3\n");
  printf("      11   3  4\n");
  printf("      12   4  7\n");
  printf("      13   7  5\n");
  printf("      14   8  4\n");
  printf("      15   4  2\n");
  printf("      16   8  7\n");
  printf("    # Generated by triangle -pcBev box\n");
  printf("\n");
  printf(
"  Here's a file `box.1.part' that partitions the mesh into four subdomains.\n"
);
  printf("\n");
  printf("    8  4\n");
  printf("       1    3\n");
  printf("       2    3\n");
  printf("       3    4\n");
  printf("       4    4\n");
  printf("       5    1\n");
  printf("       6    1\n");
  printf("       7    2\n");
  printf("       8    2\n");
  printf("    # Generated by slice -s4 box.1\n\n");
  printf(
"  Here's a file `box.1.adj' that represents the resulting adjacencies.\n");
  printf("\n");
  printf("    4\n");
  printf("      9\n");
  printf("      2\n");
  printf("      2\n");
  printf("      0\n");
  printf("      2\n");
  printf("      9\n");
  printf("      0\n");
  printf("      2\n");
  printf("      2\n");
  printf("      0\n");
  printf("      9\n");
  printf("      2\n");
  printf("      0\n");
  printf("      2\n");
  printf("      2\n");
  printf("      9\n");
  printf("\n");
  printf("Display Speed:\n");
  printf("\n");
  printf(
"  It is worthwhile to note that .edge files typically plot and print twice\n"
);
  printf(
"  as quickly as .ele files, because .ele files cause each internal edge to\n"
);
  printf(
"  be drawn twice.  For the same reason, PostScript files created from edge\n"
);
  printf("  sets are smaller than those created from triangulations.\n\n");
  printf("Show Me on the Web:\n\n");
  printf(
"  To see an illustrated, updated version of these instructions, check out\n");
  printf("\n");
  printf("    http://www.cs.cmu.edu/~quake/showme.html\n");
  printf("\n");
  printf("A Brief Plea:\n");
  printf("\n");
  printf(
"  If you use Show Me (or Triangle), and especially if you use it to\n");
  printf(
"  accomplish real work, I would like very much to hear from you.  A short\n");
  printf(
"  letter or email (to jrs@cs.berkeley.edu) describing how you use Show Me\n");
  printf(
"  (and its sister programs) will mean a lot to me.  The more people I know\n"
);
  printf(
"  are using my programs, the more easily I can justify spending time on\n");
  printf(
"  improvements, which in turn will benefit you.  Also, I can put you\n");
  printf(
"  on a list to receive email whenever new versions are available.\n");
  printf("\n");
  printf(
"  If you use a PostScript file generated by Show Me in a publication,\n");
  printf("  please include an acknowledgment as well.\n\n");
  exit(0);
}

void setfilenames(char *filename,
                  int lowermeshnumber)
{
  char numberstring[100];
  int i;

  for (i = 0; i < 2; i++) {
    strcpy(nodefilename[i], filename);
    strcpy(polyfilename[i], filename);
    strcpy(elefilename[i], filename);
    strcpy(edgefilename[i], filename);
    strcpy(partfilename[i], filename);
    strcpy(adjfilename[i], filename);
    strcpy(vnodefilename[i], filename);
    strcpy(vedgefilename[i], filename);
    strcpy(datafilename[i], filename);

    if (lowermeshnumber + i > 0) {
      sprintf(numberstring, ".%d", lowermeshnumber + i);
      strcat(nodefilename[i], numberstring);
      strcat(polyfilename[i], numberstring);
      strcat(elefilename[i], numberstring);
      strcat(edgefilename[i], numberstring);
      strcat(partfilename[i], numberstring);
      strcat(adjfilename[i], numberstring);
      strcat(vnodefilename[i], numberstring);
      strcat(vedgefilename[i], numberstring);
      strcat(datafilename[i], numberstring);
    }

    strcat(nodefilename[i], ".node");
    strcat(polyfilename[i], ".poly");
    strcat(elefilename[i], ".ele");
    strcat(edgefilename[i], ".edge");
    strcat(partfilename[i], ".part");
    strcat(adjfilename[i], ".adj");
    strcat(vnodefilename[i], ".v.node");
    strcat(vedgefilename[i], ".v.edge");
    strcat(datafilename[i], ".data");
  }
}

void parsecommandline(int argc,
                      char **argv)
{
  int increment;
  int meshnumber;
  int i, j;

  quiet = 0;
  fillelem = 0;
  line_width = 1;
  bw_ps = 0;
  start_image = ELE;
  randomseed = 1;
  filename[0] = '\0';
  for (i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      for (j = 1; argv[i][j] != '\0'; j++) {
        if (argv[i][j] == 'f') {
          fillelem = 1;
	}
        if (argv[i][j] == 'w') {
          if ((argv[i][j + 1] >= '1') && (argv[i][j + 1] <= '9')) {
            line_width = 0;
            while ((argv[i][j + 1] >= '0') && (argv[i][j + 1] <= '9')) {
              j++;
              line_width = line_width * 10 + (int) (argv[i][j] - '0');
            }
            if (line_width > 100) {
              printf("Error:  Line width cannot exceed 100.\n");
              line_width = 1;
	    }
	  }
	}
        if (argv[i][j] == 'b') {
          bw_ps = 1;
	}
        if (argv[i][j] == 'Q') {
          quiet = 1;
	}
        if ((argv[i][j] == 'h') || (argv[i][j] == 'H') ||
            (argv[i][j] == '?')) {
          info();
	}
      }
    } else {
      /* Any command-line parameter not starting with "-" is assumed to be */
      /*   a file name.                                                    */
      strncpy(filename, argv[i], FILENAMESIZE - 1);
      filename[FILENAMESIZE - 1] = '\0';
    }
  }

  if (filename[0] == '\0') {
    /* No file name specified; print a summary of the command line switches. */
    syntax();
  }

  /* Remove the filename extension (if any) and use it to infer what type */
  /*   of file should be displayed.                                       */
  if (!strcmp(&filename[strlen(filename) - 5], ".node")) {
    filename[strlen(filename) - 5] = '\0';
    start_image = NODE;
  }
  if (!strcmp(&filename[strlen(filename) - 5], ".poly")) {
    filename[strlen(filename) - 5] = '\0';
    start_image = POLY;
  }
  if (!strcmp(&filename[strlen(filename) - 4], ".ele")) {
    filename[strlen(filename) - 4] = '\0';
    start_image = ELE;
  }
  if (!strcmp(&filename[strlen(filename) - 5], ".edge")) {
    filename[strlen(filename) - 5] = '\0';
    start_image = EDGE;
  }
  if (!strcmp(&filename[strlen(filename) - 5], ".part")) {
    filename[strlen(filename) - 5] = '\0';
    start_image = PART;
  }
  if (!strcmp(&filename[strlen(filename) - 4], ".adj")) {
    filename[strlen(filename) - 4] = '\0';
    start_image = ADJ;
  }
  if (!strcmp(&filename[strlen(filename) - 5], ".data")) {
    filename[strlen(filename) - 5] = '\0';
    start_image = DATA;
  }

  /* Check the input filename for an iteration number. */
  increment = 0;
  /* Find the last period in the filename. */
  j = 1;
  while (filename[j] != '\0') {
    if ((filename[j] == '.') && (filename[j + 1] != '\0')) {
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
      if ((filename[j] >= '0') && (filename[j] <= '9')) {
        meshnumber = meshnumber * 10 + (int) (filename[j] - '0');
      } else {
        /* Oops, not a digit; this isn't an iteration number after all. */
        increment = 0;
        meshnumber = 0;
        break;
      }
      j++;
    } while (filename[j] != '\0');
  }
  if (increment > 0) {
    /* Lop off the iteration number. */
    filename[increment - 1] = '\0';
  }

  if (meshnumber == 0) {
    /* Initially, load the mesh with no iteration number (corresponding to */
    /*   number zero), and also provide buttons for iteration one.         */
    start_inc = 0;
    loweriteration = 0;
  } else {
    /* Initially, load the mesh with the specified iteration number, and */
    /*   also provide buttons for the previous iteration (one less).     */
    start_inc = 1;
    loweriteration = meshnumber - 1;
  }

  /* Set the filenames for the two iterations on the buttons. */
  setfilenames(filename, loweriteration);
}

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
  x = (showmereal) (a + b); \
  Fast_Two_Sum_Tail(a, b, x, y)

#define Two_Sum_Tail(a, b, x, y) \
  bvirt = (showmereal) (x - a); \
  avirt = x - bvirt; \
  bround = b - bvirt; \
  around = a - avirt; \
  y = around + bround

#define Two_Sum(a, b, x, y) \
  x = (showmereal) (a + b); \
  Two_Sum_Tail(a, b, x, y)

#define Two_Diff_Tail(a, b, x, y) \
  bvirt = (showmereal) (a - x); \
  avirt = x + bvirt; \
  bround = bvirt - b; \
  around = a - avirt; \
  y = around + bround

#define Two_Diff(a, b, x, y) \
  x = (showmereal) (a - b); \
  Two_Diff_Tail(a, b, x, y)

#define Split(a, ahi, alo) \
  c = (showmereal) (splitter * a); \
  abig = (showmereal) (c - a); \
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
  x = (showmereal) (a * b); \
  Two_Product_Tail(a, b, x, y)

#define Two_Product_Presplit(a, b, bhi, blo, x, y) \
  x = (showmereal) (a * b); \
  Split(a, ahi, alo); \
  err1 = x - (ahi * bhi); \
  err2 = err1 - (alo * bhi); \
  err3 = err2 - (ahi * blo); \
  y = (alo * blo) - err3

#define Square_Tail(a, x, y) \
  Split(a, ahi, alo); \
  err1 = x - (ahi * ahi); \
  err3 = err1 - ((ahi + ahi) * alo); \
  y = (alo * alo) - err3

#define Square(a, x, y) \
  x = (showmereal) (a * a); \
  Square_Tail(a, x, y)

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

void primitivesinit()
{
  showmereal half;
  showmereal check, lastcheck;
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

  /* Error bounds for orientation test. */
  resulterrbound = (3.0 + 8.0 * epsilon) * epsilon;
  o3derrboundA = (7.0 + 56.0 * epsilon) * epsilon;
  o3derrboundB = (3.0 + 28.0 * epsilon) * epsilon;
  o3derrboundC = (26.0 + 288.0 * epsilon) * epsilon * epsilon;
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
                                showmereal *e,
                                int flen,
                                showmereal *f,
                                showmereal *h)
{
  showmereal Q;
  INEXACT showmereal Qnew;
  INEXACT showmereal hh;
  INEXACT showmereal bvirt;
  showmereal avirt, bround, around;
  int eindex, findex, hindex;
  showmereal enow, fnow;

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
                             showmereal *e,
                             showmereal b,
                             showmereal *h)
{
  INEXACT showmereal Q, sum;
  showmereal hh;
  INEXACT showmereal product1;
  showmereal product0;
  int eindex, hindex;
  showmereal enow;
  INEXACT showmereal bvirt;
  showmereal avirt, bround, around;
  INEXACT showmereal c;
  INEXACT showmereal abig;
  showmereal ahi, alo, bhi, blo;
  showmereal err1, err2, err3;

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

showmereal estimate(int elen,
                    showmereal *e)
{
  showmereal Q;
  int eindex;

  Q = e[0];
  for (eindex = 1; eindex < elen; eindex++) {
    Q += e[eindex];
  }
  return Q;
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

showmereal orient3dadapt(showmereal *pa,
                       showmereal *pb,
                       showmereal *pc,
                       showmereal *pd,
                       showmereal permanent)
{
  INEXACT showmereal adx, bdx, cdx, ady, bdy, cdy, adz, bdz, cdz;
  showmereal det, errbound;

  INEXACT showmereal bdxcdy1, cdxbdy1, cdxady1, adxcdy1, adxbdy1, bdxady1;
  showmereal bdxcdy0, cdxbdy0, cdxady0, adxcdy0, adxbdy0, bdxady0;
  showmereal bc[4], ca[4], ab[4];
  INEXACT showmereal bc3, ca3, ab3;
  showmereal adet[8], bdet[8], cdet[8];
  int alen, blen, clen;
  showmereal abdet[16];
  int ablen;
  showmereal *finnow, *finother, *finswap;
  showmereal fin1[192], fin2[192];
  int finlength;

  showmereal adxtail, bdxtail, cdxtail;
  showmereal adytail, bdytail, cdytail;
  showmereal adztail, bdztail, cdztail;
  INEXACT showmereal at_blarge, at_clarge;
  INEXACT showmereal bt_clarge, bt_alarge;
  INEXACT showmereal ct_alarge, ct_blarge;
  showmereal at_b[4], at_c[4], bt_c[4], bt_a[4], ct_a[4], ct_b[4];
  int at_blen, at_clen, bt_clen, bt_alen, ct_alen, ct_blen;
  INEXACT showmereal bdxt_cdy1, cdxt_bdy1, cdxt_ady1;
  INEXACT showmereal adxt_cdy1, adxt_bdy1, bdxt_ady1;
  showmereal bdxt_cdy0, cdxt_bdy0, cdxt_ady0;
  showmereal adxt_cdy0, adxt_bdy0, bdxt_ady0;
  INEXACT showmereal bdyt_cdx1, cdyt_bdx1, cdyt_adx1;
  INEXACT showmereal adyt_cdx1, adyt_bdx1, bdyt_adx1;
  showmereal bdyt_cdx0, cdyt_bdx0, cdyt_adx0;
  showmereal adyt_cdx0, adyt_bdx0, bdyt_adx0;
  showmereal bct[8], cat[8], abt[8];
  int bctlen, catlen, abtlen;
  INEXACT showmereal bdxt_cdyt1, cdxt_bdyt1, cdxt_adyt1;
  INEXACT showmereal adxt_cdyt1, adxt_bdyt1, bdxt_adyt1;
  showmereal bdxt_cdyt0, cdxt_bdyt0, cdxt_adyt0;
  showmereal adxt_cdyt0, adxt_bdyt0, bdxt_adyt0;
  showmereal u[4], v[12], w[16];
  INEXACT showmereal u3;
  int vlength, wlength;
  showmereal negate;

  INEXACT showmereal bvirt;
  showmereal avirt, bround, around;
  INEXACT showmereal c;
  INEXACT showmereal abig;
  showmereal ahi, alo, bhi, blo;
  showmereal err1, err2, err3;
  INEXACT showmereal _i, _j, _k;
  showmereal _0;

  adx = (showmereal) (pa[0] - pd[0]);
  bdx = (showmereal) (pb[0] - pd[0]);
  cdx = (showmereal) (pc[0] - pd[0]);
  ady = (showmereal) (pa[1] - pd[1]);
  bdy = (showmereal) (pb[1] - pd[1]);
  cdy = (showmereal) (pc[1] - pd[1]);
  adz = (showmereal) (pa[2] - pd[2]);
  bdz = (showmereal) (pb[2] - pd[2]);
  cdz = (showmereal) (pc[2] - pd[2]);

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

showmereal orient3d(showmereal *pa,
                    showmereal *pb,
                    showmereal *pc,
                    showmereal *pd)
{
  showmereal adx, bdx, cdx, ady, bdy, cdy, adz, bdz, cdz;
  showmereal bdxcdy, cdxbdy, cdxady, adxcdy, adxbdy, bdxady;
  showmereal det;
  showmereal permanent, errbound;

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

  permanent = (Absolute(bdxcdy) + Absolute(cdxbdy)) * Absolute(adz)
            + (Absolute(cdxady) + Absolute(adxcdy)) * Absolute(bdz)
            + (Absolute(adxbdy) + Absolute(bdxady)) * Absolute(cdz);
  errbound = o3derrboundA * permanent;
  if ((det > errbound) || (-det > errbound)) {
    return det;
  }

  return orient3dadapt(pa, pb, pc, pd, permanent);
}

/**                                                                         **/
/**                                                                         **/
/********* Geometric primitives end here                             *********/

void free_inc(int inc)
{
  if (loaded[inc][NODE]) {
    free(nodeptr[inc]);
  }

  if (loaded[inc][POLY]) {
    if (polynodes[inc] > 0) {
      free(polynodeptr[inc]);
    }
    free(polysegptr[inc]);
    free(polyholeptr[inc]);
  }

  if (loaded[inc][ELE]) {
    free(eleptr[inc]);
    if (nodedim[inc] == 3) {
      free(tetraptr);
    }
  }

  if (loaded[inc][EDGE]) {
    free(edgeptr[inc]);
    free(normptr[inc]);
  }

  if (loaded[inc][PART]) {
    free(partition[inc]);
    free(subdomcenter[inc]);
    free(subdomshift[inc]);
  }

  if (loaded[inc][ADJ]) {
    free(adjptr[inc]);
  }

  if (loaded[inc][VORO]) {
    free(vnodeptr[inc]);
    free(vedgeptr[inc]);
    free(vnormptr[inc]);
  }

  if (loaded[inc][DATA]) {
    free(dataptr[inc]);
  }
}

void move_inc(int inc)
{
  int i;

  free_inc(1 - inc);

  for (i = 0; i < IMAGE_TYPES; i++) {
    loaded[1 - inc][i] = loaded[inc][i];
    loaded[inc][i] = 0;
    xlo[1 - inc][i] = xlo[inc][i];
    ylo[1 - inc][i] = ylo[inc][i];
    zlo[1 - inc][i] = zlo[inc][i];
    xhi[1 - inc][i] = xhi[inc][i];
    yhi[1 - inc][i] = yhi[inc][i];
    zhi[1 - inc][i] = zhi[inc][i];
  }

  nodes[1 - inc] = nodes[inc];
  nodedim[1 - inc] = nodedim[inc];
  nodeptr[1 - inc] = nodeptr[inc];
  polynodes[1 - inc] = polynodes[inc];
  polydim[1 - inc] = polydim[inc];
  polyedges[1 - inc] = polyedges[inc];
  polyholes[1 - inc] = polyholes[inc];
  polynodeptr[1 - inc] = polynodeptr[inc];
  polysegptr[1 - inc] = polysegptr[inc];
  polyholeptr[1 - inc] = polyholeptr[inc];
  elems[1 - inc] = elems[inc];
  elecorners[1 - inc] = elecorners[inc];
  eleptr[1 - inc] = eleptr[inc];
  edges[1 - inc] = edges[inc];
  edgeptr[1 - inc] = edgeptr[inc];
  normptr[1 - inc] = normptr[inc];
  subdomains[1 - inc] = subdomains[inc];
  partition[1 - inc] = partition[inc];
  subdomcenter[1 - inc] = subdomcenter[inc];
  subdomshift[1 - inc] = subdomshift[inc];
  adjsubdomains[1 - inc] = adjsubdomains[inc];
  adjptr[1 - inc] = adjptr[inc];
  vnodes[1 - inc] = vnodes[inc];
  vnodedim[1 - inc] = vnodedim[inc];
  vnodeptr[1 - inc] = vnodeptr[inc];
  vedges[1 - inc] = vedges[inc];
  vedgeptr[1 - inc] = vedgeptr[inc];
  vnormptr[1 - inc] = vnormptr[inc];
  datavalues[1 - inc] = datavalues[inc];
  datadim[1 - inc] = datadim[inc];
  dataptr[1 - inc] = dataptr[inc];
  firstnumber[1 - inc] = firstnumber[inc];
  firstnumber[inc] = -1;
}

void unload_inc(int inc)
{
  int i;

  current_image = NOTHING;
  for (i = 0; i < IMAGE_TYPES; i++) {
    loaded[inc][i] = 0;
    firstnumber[inc] = -1;
  }
}

void showme_init()
{
  int template, forward, reverse;
  int i;

  primitivesinit();
  current_image = NOTHING;
  current_inc = 0;
  explosion = STARTEXPLOSION;
  identitymatrix(viewmatrix);
  unload_inc(0);
  unload_inc(1);
  for (i = 0; i < MAXCOLORS; i++) {
    template = MAXCOLORS - 1;
    forward = i;
    reverse = 0;
    while (template != 0) {
      reverse = (reverse << 1) | (forward & 1);
      forward >>= 1;
      template >>= 1;
    }
    bitreverse[i] = reverse;
  }
}

void matchfaces(showmereal *nodeptr,
                showmelong numnodes,
                showmelong numelems,
                int dim,
                showmelong *eleptr,
                struct tetra **tetraptr)
{
  showmelong *node2nodelem;
  showmelong *nodelem2nodelem;
  showmelong thisnode, thisnodelem, thiselem;
  int thiscorner;
  showmelong thisnode1, thisnode2;
  int farcorner;
  showmelong matchnodelem, matchelem;
  int matchcorner;
  showmelong matchnode1, matchnode2, matchnode3;
  showmelong firsttet;

  /* Create array of tetrahedron adjacencies. */
  *tetraptr = (struct tetra *) malloc((numelems + 1) * sizeof(struct tetra));

  /* Short-lived arrays used to fill in the above array.  They maintain for */
  /*   each vertex a linked list of tetrahedra adjoining it.                */
  node2nodelem = (showmelong *) malloc((numnodes + 1) * sizeof(showmelong));
  nodelem2nodelem = (showmelong *)
                    malloc((numelems + 1) * 4 * sizeof(showmelong));

  /* All lists are initially empty. */
  for (thisnode = 1; thisnode <= numnodes; thisnode++) {
    node2nodelem[thisnode] = 0;
  }

  /* Initially, nothing is linked. */
  for (thiselem = 1; thiselem <= numelems; thiselem++)  {
    (*tetraptr)[thiselem].invisible = 0;
    for (thiscorner = 0; thiscorner < 4; thiscorner++) {
      (*tetraptr)[thiselem].neighbor[thiscorner] = 0;
    }
  }

  /* For each vertex, form a linked list of the tetrahedra that adjoin it. */
  for (thisnodelem = 4; thisnodelem < 4 * numelems + 4; thisnodelem++) {
    thisnode = eleptr[thisnodelem];
    nodelem2nodelem[thisnodelem] = node2nodelem[thisnode];
    node2nodelem[thisnode] = thisnodelem;
  }

  /* Loop through the vertices. */
  for (thisnode = 1; thisnode <= numnodes; thisnode++) {
    /* Loop through the tetrahedra adjoining this vertex. */
    thisnodelem = node2nodelem[thisnode];
    while (thisnodelem != 0) {
      /* The next tetrahedron (and joining corner) in the linked list. */
      thiselem = thisnodelem >> 2;
      thiscorner = (int) (thisnodelem & 3);
      /* Loop through the other corners (besides `thisnode') of the        */
      /*   tetrahedron for which the opposite tetrahedron isn't yet known. */
      for (farcorner = 0; farcorner < 4; farcorner++) {
        if ((farcorner != thiscorner) &&
            ((*tetraptr)[thiselem].neighbor[farcorner] == 0)) {
          /* Identify the other two corners. */
          thisnode1 = 3 - (farcorner & 1) - (thiscorner & 2);
          thisnode2 = 6 - farcorner - thiscorner - thisnode1;
          thisnode1 = eleptr[thiselem * 4 + thisnode1];
          thisnode2 = eleptr[thiselem * 4 + thisnode2];
          /* Loop through the remaining tetrahedra in the linked list,    */
          /*   looking for one that shares the face (thisnode, thisnode1, */
          /*   thisnode2).                                                */
          matchnodelem = nodelem2nodelem[thisnodelem];
          while (matchnodelem != 0) {
            /* The next tetrahedron (and joining corner) in the linked list. */
            matchelem = matchnodelem >> 2;
            matchcorner = (int) (matchnodelem & 3);
            /* Identify the other three corners. */
            matchnode1 = eleptr[matchelem * 4 + ((matchcorner + 1) & 3)];
            matchnode2 = eleptr[matchelem * 4 + ((matchcorner + 2) & 3)];
            matchnode3 = eleptr[matchelem * 4 + ((matchcorner + 3) & 3)];
            /* Check each of the three faces of the later tetrahedron that */
            /*   adjoin `thisnode' to see if any of them adjoins the face  */
            /*   opposite `farcorner' in the earlier tetrahedron.          */
            if (((thisnode1 == matchnode2) && (thisnode2 == matchnode3)) ||
                ((thisnode1 == matchnode3) && (thisnode2 == matchnode2))) {
              /* Glue the two tetrahedra together opposite `matchnode1'. */
              (*tetraptr)[thiselem].neighbor[farcorner] =
                matchelem * 4 + ((matchcorner + 1) & 3);
              (*tetraptr)[matchelem].neighbor[(matchcorner + 1) & 3] =
                thiselem * 4 + farcorner;
            }
            if (((thisnode1 == matchnode3) && (thisnode2 == matchnode1)) ||
                ((thisnode1 == matchnode1) && (thisnode2 == matchnode3))) {
              /* Glue the two tetrahedra together opposite `matchnode2'. */
              (*tetraptr)[thiselem].neighbor[farcorner] =
                matchelem * 4 + ((matchcorner + 2) & 3);
              (*tetraptr)[matchelem].neighbor[(matchcorner + 2) & 3] =
                thiselem * 4 + farcorner;
            }
            if (((thisnode1 == matchnode1) && (thisnode2 == matchnode2)) ||
                ((thisnode1 == matchnode2) && (thisnode2 == matchnode1))) {
              /* Glue the two tetrahedra together opposite `matchnode3'. */
              (*tetraptr)[thiselem].neighbor[farcorner] =
                matchelem * 4 + ((matchcorner + 3) & 3);
              (*tetraptr)[matchelem].neighbor[(matchcorner + 3) & 3] =
                thiselem * 4 + farcorner;
            }
            /* Find the next tetrahedron/corner in the linked list. */
            matchnodelem = nodelem2nodelem[matchnodelem];
          }
        }
      }
      /* Find the next tetrahedron/corner in the linked list. */
      thisnodelem = nodelem2nodelem[thisnodelem];
    }
  }

  /* Free the linked list arrays. */
  free(nodelem2nodelem);
  free(node2nodelem);

  /* Loop through the tetrahedra (in reverse order), creating a linked list */
  /*   of those that have at least one exposed (i.e. unshared) face.        */
  firsttet = 0;
  for (thiselem = numelems; thiselem > 0; thiselem--) {
    /* No tetrahedron is initially invisible. */
    (*tetraptr)[thiselem].invisible = 0;
    if (((*tetraptr)[thiselem].neighbor[0] <= 0) ||
        ((*tetraptr)[thiselem].neighbor[1] <= 0) ||
        ((*tetraptr)[thiselem].neighbor[2] <= 0) ||
        ((*tetraptr)[thiselem].neighbor[3] <= 0)) {
      /* Add this tetrahedron to the front of the linked list. */
      (*tetraptr)[thiselem].nexttet = firsttet;
      firsttet = thiselem;
    }
  }
  /* Remember the head of the linked list. */
  (*tetraptr)[0].nexttet = firsttet;
}

char *readline(char *string,
               FILE *infile,
               char *infilename)
{
  char *result;

  /* Search for something that looks like a number. */
  do {
    result = fgets(string, INPUTLINESIZE, infile);
    if (result == (char *) NULL) {
      printf("  Error:  Unexpected end of file in %s.\n", infilename);
      exit(1);
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

char *findfield(char *string)
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

int loadnodes(char *fname,
              int *firstnumber,
              showmelong *nodes,
              int *dim,
              showmereal **ptr,
              showmereal *xmin,
              showmereal *ymin,
              showmereal *zmin,
              showmereal *xmax,
              showmereal *ymax,
              showmereal *zmax)
{
  FILE *infile;
  char inputline[INPUTLINESIZE];
  char *stringptr;
  int extras;
  int nodemarks;
  showmelong nodenumber;
  showmelong i;
  int j;
  showmelong index;
  int smallerr;
  showmereal x, y, z;

  /* Set up a meaningful bounding box in case there's an error and no */
  /*   points get loaded.                                             */
  *xmin = *ymin = *zmin = 0.0;
  *xmax = *ymax = *zmax = 1.0;

  if (!quiet) {
    printf("Opening %s.\n", fname);
  }
  infile = fopen(fname, "r");
  if (infile == (FILE *) NULL) {
    printf("  Error:  Cannot access file %s.\n", fname);
    return 1;
  }

  /* Read and check number of points. */
  stringptr = readline(inputline, infile, fname);
  *nodes = (showmelong) strtol(stringptr, &stringptr, 0);
  if (*nodes < 3) {
    printf("  Error:  %s contains only %ld points.\n", fname, (long) *nodes);
    return 1;
  }

  /* Read and check dimensionality of points. */
  stringptr = findfield(stringptr);
  if (*stringptr == '\0') {
    *dim = 2;
  } else {
    *dim = (int) strtol(stringptr, &stringptr, 0);
  }
  if (*dim < 1) {
    printf("  Error:  %s has dimensionality %d.\n", fname, *dim);
    return 1;
  }
  if ((*dim < 2) || (*dim > 3)) {
    printf("  Error:  I only understand two and three-dimensional meshes.\n");
    return 1;
  }

  /* Read and check number of point attributes. */
  stringptr = findfield(stringptr);
  if (*stringptr == '\0') {
    extras = 0;
  } else {
    extras = (int) strtol(stringptr, &stringptr, 0);
  }
  if ((extras < 0) && !quiet) {
    printf("  Warning:  %s has negative value for number of attributes.\n",
           fname);
  }

  /* Read and check number of point markers. */
  stringptr = findfield(stringptr);
  if (*stringptr == '\0') {
    nodemarks = 0;
  } else {
    nodemarks = (int) strtol(stringptr, &stringptr, 0);
  }
  if (nodemarks < 0) {
    printf("  Warning:  %s has negative value for number of point markers.\n",
           fname);
  }
  if (nodemarks > 1) {
    printf(
   "  Warning:  %s has value greater than one for number of point markers.\n",
           fname);
  }

  /* Allocate space to store the points. */
  *ptr = (showmereal *) malloc((*nodes + 1) * *dim * sizeof(showmereal));
  if (*ptr == (showmereal *) NULL) {
    printf("  Error:  Out of memory.\n");
    return 1;
  }

  /* Read the points.  The coordinate index `index' starts at '*dim', */
  /*   because points are internally numbered from 1 (regardless of   */
  /*   how they're numbered in the file).                             */
  index = *dim;
  smallerr = 1;
  for (i = 0; i < *nodes; i++) {
    stringptr = readline(inputline, infile, fname);
    nodenumber = (showmelong) strtol(stringptr, &stringptr, 0);

    /* The point numbers are mostly ignored, except that the first one      */
    /*   tells us whether points are numbered from zero or one in the file. */
    if ((i == 0) && (*firstnumber == -1)) {
      if (nodenumber == 0) {
        *firstnumber = 0;
      } else {
        *firstnumber = 1;
      }
    }
    /* If points are not numbered consecutively, print a warning. */
    if ((nodenumber != *firstnumber + i) && (smallerr)) {
      printf("  Warning:  Points in %s are not numbered correctly\n", fname);
      printf("            (starting with point %ld).\n",
             (long) (*firstnumber + i));
      /* Don't print more minor errors after this. */
      smallerr = 0;
    }

    /* Read this point's coordinates. */
    for (j = 0; j < *dim; j++) {
      stringptr = findfield(stringptr);
      if (*stringptr == '\0') {
        printf("Error:  Point %ld is missing a coordinate in %s.\n",
               (long) (*firstnumber + i), fname);
        /* Free the memory for points and return an error indication. */
        free(*ptr);
        return 1;
      }
      /* Store the coordinate and advance the coordinate index. */
      (*ptr)[index++] = (showmereal) strtod(stringptr, &stringptr);
    }
  }

  fclose(infile);

  /* Use the first point's coordinates to initialize the bounding box. */
  index = *dim;
  *xmin = *xmax = (*ptr)[index];
  *ymin = *ymax = (*ptr)[index + 1];
  if (*dim == 3) {
    *zmin = *zmax = (*ptr)[index + 2];
  }
  /* Loop through the remaining points and update the bounding box. */
  for (i = 2; i <= *nodes; i++) {
    index += *dim;
    x = (*ptr)[index];
    y = (*ptr)[index + 1];
    if (x < *xmin) {
      *xmin = x;
    }
    if (y < *ymin) {
      *ymin = y;
    }
    if (x > *xmax) {
      *xmax = x;
    }
    if (y > *ymax) {
      *ymax = y;
    }
    if (*dim == 3) {
      z = (*ptr)[index + 2];
      if (z < *zmin) {
	*zmin = z;
      }
      if (z > *zmax) {
	*zmax = z;
      }
    }
  }

  /* Make sure the minima and maxima don't coincide. */
  if (*xmin == *xmax) {
    *xmin -= 0.5;
    *xmax += 0.5;
  }
  if (*ymin == *ymax) {
    *ymin -= 0.5;
    *ymax += 0.5;
  }
  if (*dim == 3) {
    if (*zmin == *zmax) {
      *zmin -= 0.5;
      *zmax += 0.5;
    }
  }

  return 0;
}

int loadpoly(int inc,
             char *fname,
             int *firstnumber,
             showmelong *pnodes,
             int *dim,
             showmelong *segments,
             showmelong *holes,
             showmereal **nodeptr,
             showmelong **segptr,
             showmereal **holeptr,
             showmereal *xmin,
             showmereal *ymin,
             showmereal *zmin,
             showmereal *xmax,
             showmereal *ymax,
             showmereal *zmax)
{
  FILE *infile;
  char inputline[INPUTLINESIZE];
  char *stringptr;
  int extras;
  int nodemarks;
  int segmentmarks;
  showmelong nodenumber, edgenumber, holenumber;
  showmelong maxnode;
  showmelong i;
  int j;
  showmelong index;
  int smallerr;
  showmereal x, y, z;

  if (!quiet) {
    printf("Opening %s.\n", fname);
  }
  infile = fopen(fname, "r");
  if (infile == (FILE *) NULL) {
    printf("  Error:  Cannot access file %s.\n", fname);
    return 1;
  }

  /* Read and check number of points. */
  stringptr = readline(inputline, infile, fname);
  *pnodes = (showmelong) strtol(stringptr, &stringptr, 0);
  if (*pnodes == 0) {
    /* The .poly file specifies zero nodes, which means that the nodes are */
    /*   stored in a separate .node file.  Make sure it's loaded.          */
    if (!loaded[inc][NODE]) {
      if (load_image(inc, NODE)) {
        /* The .node file is absent or broken; return an error indication. */
        return 1;
      }
    }

    /* Get number of nodes in the .node file. */
    maxnode = nodes[inc];
    /* This .poly file has the same bounding box as the .node file. */
    *xmin = xlo[inc][NODE];
    *ymin = ylo[inc][NODE];
    *zmin = zlo[inc][NODE];
    *xmax = xhi[inc][NODE];
    *ymax = yhi[inc][NODE];
    *zmax = zhi[inc][NODE];
  } else {
    if (*pnodes < 1) {
      printf("  Error:  %s contains %ld points.\n", fname, (long) *pnodes);
      return 1;
    }
    /* Get number of nodes in the .poly file. */
    maxnode = *pnodes;
    /* Set up a meaningful bounding box in case there's an error and no */
    /*   points get loaded.                                             */
    *xmin = *ymin = *zmin = 0.0;
    *xmax = *ymax = *zmax = 1.0;
  }

  /* Read and check dimensionality of points. */
  stringptr = findfield(stringptr);
  if (*stringptr == '\0') {
    *dim = 2;
  } else {
    *dim = (int) strtol(stringptr, &stringptr, 0);
  }
  if (*dim < 1) {
    printf("  Error:  %s has dimensionality %d.\n", fname, *dim);
    return 1;
  }
  if (*dim != 2) {
    printf("  Error:  I only understand two-dimensional .poly files.\n");
    return 1;
  }
  if ((*pnodes == 0) && (*dim != nodedim[inc])) {
    printf("  Error:  Dimensionality of .poly and .node files don't match.\n");
    return 1;
  }

  /* Read and check number of point attributes. */
  stringptr = findfield(stringptr);
  if (*stringptr == '\0') {
    extras = 0;
  } else {
    extras = (int) strtol(stringptr, &stringptr, 0);
  }
  if ((extras < 0) && !quiet) {
    printf("  Warning:  %s has negative value for number of attributes.\n",
           fname);
  }

  /* Read and check number of point markers. */
  stringptr = findfield(stringptr);
  if (*stringptr == '\0') {
    nodemarks = 0;
  } else {
    nodemarks = (int) strtol(stringptr, &stringptr, 0);
  }
  if (nodemarks < 0) {
    printf("  Warning:  %s has negative value for number of point markers.\n",
           fname);
  }
  if (nodemarks > 1) {
    printf("  Warning:  %s has value greater than one for number of point "
           "markers.\n", fname);
  }

  if (*pnodes > 0) {
    /* Allocate space to store the points. */
    *nodeptr = (showmereal *)
               malloc((*pnodes + 1) * *dim * sizeof(showmereal));
    if (*nodeptr == (showmereal *) NULL) {
      printf("  Error:  Out of memory.\n");
      return 1;
    }

    /* Read the points.  The coordinate index `index' starts at '*dim', */
    /*   because points are internally numbered from 1 (regardless of   */
    /*   how they're numbered in the file).                             */
    index = *dim;
    smallerr = 1;
    for (i = 0; i < *pnodes; i++) {
      stringptr = readline(inputline, infile, fname);
      nodenumber = (showmelong) strtol(stringptr, &stringptr, 0);

      /* The point numbers are mostly ignored, except that the first one */
      /*   tells us whether points are numbered from zero or one in the  */
      /*   file.                                                         */
      if ((i == 0) && (*firstnumber == -1)) {
        if (nodenumber == 0) {
          *firstnumber = 0;
        } else {
          *firstnumber = 1;
        }
      }
      /* If points are not numbered consecutively, print a warning. */
      if ((nodenumber != *firstnumber + i) && (smallerr)) {
        printf("  Warning:  Points in %s are not numbered correctly.\n",
               fname);
        printf("            (starting with point %ld).\n",
               (long) (*firstnumber + i));
        smallerr = 0;
      }

      /* Read this point's coordinates. */
      for (j = 0; j < *dim; j++) {
        stringptr = findfield(stringptr);
        if (*stringptr == '\0') {
          printf("Error:  Point %ld is missing a coordinate in %s.\n",
                 (long) (*firstnumber + i), fname);
          /* Free the memory for points and return an error indication. */
          free(*nodeptr);
          return 1;
        }
        /* Store the coordinate and advance the coordinate index. */
        (*nodeptr)[index++] = (showmereal) strtod(stringptr, &stringptr);
      }
    }
  }

  /* Read and check number of segments. */
  stringptr = readline(inputline, infile, fname);
  *segments = (showmelong) strtol(stringptr, &stringptr, 0);
  if (*segments < 0) {
    printf("  Error:  %s contains %ld segments.\n", fname, (long) *segments);
    /* Free the memory for points and return an error indication. */
    if (*pnodes > 0) {
      free(*nodeptr);
    }
    return 1;
  }

  /* Read and check number of segment markers. */
  stringptr = findfield(stringptr);
  if (*stringptr == '\0') {
    segmentmarks = 0;
  } else {
    segmentmarks = (int) strtol(stringptr, &stringptr, 0);
  }
  if (segmentmarks < 0) {
    printf("  Warning:  %s has negative value for number of segment "
           "markers.\n", fname);
  }
  if (segmentmarks > 1) {
    printf("  Warning:  %s has value greater than one for number of segment "
           "markers.\n", fname);
  }

  /* Allocate space to store the segments. */
  *segptr = (showmelong *) malloc(((*segments + 1) << 1) * sizeof(showmelong));
  if (*segptr == (showmelong *) NULL) {
    printf("  Error:  Out of memory.\n");
    /* Free the memory for points and return an error indication. */
    if (*pnodes > 0) {
      free(*nodeptr);
    }
    return 1;
  }

  /* Read the segments.  The corner index `index' starts at 2, because    */
  /*   segments are internally numbered from 1 (regardless of how they're */
  /*   numbered in the file).                                             */
  index = 2;
  smallerr = 1;
  for (i = *firstnumber; i < *firstnumber + *segments; i++) {
    stringptr = readline(inputline, infile, fname);
    edgenumber = (showmelong) strtol(stringptr, &stringptr, 0);

    /* If segments are not numbered as expected, print a warning. */
    if ((edgenumber != i) && (smallerr)) {
      printf("  Warning:  Segments in %s are not numbered correctly.\n",
             fname);
      printf("            (starting with segment %ld).\n", (long) i);
      /* Don't print more minor errors after this. */
      smallerr = 0;
    }

    /* Read the first segment endpoint; adjust it so the vertices are */
    /*   internally numbered from one.                                */
    stringptr = findfield(stringptr);
    if (*stringptr == '\0') {
      printf("Error:  Segment %ld is missing its endpoints in %s.\n",
             (long) i, fname);
      /* Free the memory for everything and return an error indication. */
      if (*pnodes > 0) {
        free(*nodeptr);
      }
      free(*segptr);
      return 1;
    }
    (*segptr)[index] = (showmelong) strtol(stringptr, &stringptr, 0) + 1 -
                       *firstnumber;

    /* Check if the first endpoint is valid. */
    if (((*segptr)[index] < 1) || ((*segptr)[index] > maxnode)) {
      printf("Error:  Segment %ld has invalid endpoint in %s.\n",
             (long) i, fname);
      return 1;
    }

    /* Read the second segment endpoint; adjust it so the vertices are */
    /*   internally numbered from one.                                 */
    stringptr = findfield(stringptr);
    if (*stringptr == '\0') {
      printf("Error:  Segment %ld is missing an endpoint in %s.\n",
             (long) i, fname);
      /* Free the memory for everything and return an error indication. */
      if (*pnodes > 0) {
        free(*nodeptr);
      }
      free(*segptr);
      return 1;
    }
    (*segptr)[index + 1] = (showmelong) strtol(stringptr, &stringptr, 0) + 1 -
                           *firstnumber;

    /* Check if the second endpoint is valid. */
    if (((*segptr)[index + 1] < 1) || ((*segptr)[index + 1] > maxnode)) {
      printf("Error:  Segment %ld has invalid endpoint in %s.\n",
             (long) i, fname);
      return 1;
    }

    index += 2;
  }

  /* Read and check number of holes. */
  stringptr = readline(inputline, infile, fname);
  *holes = (showmelong) strtol(stringptr, &stringptr, 0);
  if (*holes < 0) {
    printf("  Error:  %s contains %ld holes.\n", fname, (long) *holes);
    /* Free the memory for everything and return an error indication. */
    if (*pnodes > 0) {
      free(*nodeptr);
    }
    free(*segptr);
    return 1;
  }

  /* Allocate space to store the holes. */
  *holeptr = (showmereal *) malloc((*holes + 1) * *dim * sizeof(showmereal));
  if (*holeptr == (showmereal *) NULL) {
    printf("  Error:  Out of memory.\n");
    /* Free the memory for everything and return an error indication. */
    if (*pnodes > 0) {
      free(*nodeptr);
    }
    free(*segptr);
    return 1;
  }

  /* Read the holes.  The coordinate index `index' starts at '*dim', */
  /*   because holes are internally numbered from 1 (regardless of   */
  /*   how they're numbered in the file).                            */
  index = *dim;
  smallerr = 1;
  for (i = *firstnumber; i < *firstnumber + *holes; i++) {
    stringptr = readline(inputline, infile, fname);
    holenumber = (showmereal) strtol(stringptr, &stringptr, 0);

    /* If holes are not numbered as expected, print a warning. */
    if ((holenumber != i) && (smallerr)) {
      printf("  Warning:  Holes in %s are not numbered correctly.\n", fname);
      printf("            (starting with hole %ld).\n", (long) i);
      /* Don't print more minor errors after this. */
      smallerr = 0;
    }

    /* Read this hole's coordinates. */
    for (j = 0; j < *dim; j++) {
      stringptr = findfield(stringptr);
      if (*stringptr == '\0') {
        printf("Error:  Hole %ld is missing a coordinate in %s.\n",
               (long) i, fname);
        /* Free the memory for everything and return an error indication. */
        if (*pnodes > 0) {
          free(*nodeptr);
        }
        free(*segptr);
        free(*holeptr);
        return 1;
      }
        /* Store the coordinate and advance the coordinate index. */
      (*holeptr)[index++] = (showmereal) strtod(stringptr, &stringptr);
    }
  }

  fclose(infile);

  if (*pnodes > 0) {
    /* Use the first point's coordinates to initialize the bounding box. */
    index = *dim;
    *xmin = *xmax = (*nodeptr)[index];
    *ymin = *ymax = (*nodeptr)[index + 1];
    if (*dim == 2) {
      *zmin = 0.0;
      *zmax = 1.0;
    } else {
      *zmin = *zmax = (*nodeptr)[index + 2];
    }
    /* Loop through the remaining points and update the bounding box. */
    for (i = 2; i <= *pnodes; i++) {
      index += *dim;
      x = (*nodeptr)[index];
      y = (*nodeptr)[index + 1];
      if (x < *xmin) {
        *xmin = x;
      }
      if (y < *ymin) {
        *ymin = y;
      }
      if (x > *xmax) {
        *xmax = x;
      }
      if (y > *ymax) {
        *ymax = y;
      }
      if (*dim == 3) {
        z = (*nodeptr)[index + 2];
        if (z < *zmin) {
          *zmin = z;
        }
        if (z > *zmax) {
          *zmax = z;
        }
      }
    }
  }
  /* Loop through the holes and update the bounding box. */
  index = *dim;
  for (i = 1; i <= *holes; i++) {
    x = (*holeptr)[index];
    y = (*holeptr)[index + 1];
    if (x < *xmin) {
      *xmin = x;
    }
    if (y < *ymin) {
      *ymin = y;
    }
    if (x > *xmax) {
      *xmax = x;
    }
    if (y > *ymax) {
      *ymax = y;
    }
    if (*dim == 3) {
      z = (*holeptr)[index + 2];
      if (z < *zmin) {
        *zmin = z;
      }
      if (z > *zmax) {
        *zmax = z;
      }
    }
    index += *dim;
  }

  return 0;
}

int loadelems(char *fname,
              int dim,
              int firstnumber,
              showmelong nodes,
              showmereal *nodeptr,
              showmelong *elems,
              int *corners,
              showmelong **eleptr)
{
  FILE *infile;
  char inputline[INPUTLINESIZE];
  char *stringptr;
  showmelong extras;
  showmelong index;
  showmelong elemnumber;
  showmelong i, j;
  showmelong smallerr;
  showmelong swap;

  if (!quiet) {
    printf("Opening %s.\n", fname);
  }
  infile = fopen(fname, "r");
  if (infile == (FILE *) NULL) {
    printf("  Error:  Cannot access file %s.\n", fname);
    return 1;
  }

  /* Read and check number of elements. */
  stringptr = readline(inputline, infile, fname);
  *elems = (showmelong) strtol(stringptr, &stringptr, 0);
  if (*elems < 1) {
    printf("  Error:  %s contains %ld elements.\n", fname, (long) *elems);
    return 1;
  }

  /* Read and check points per element. */
  stringptr = findfield(stringptr);
  if (*stringptr == '\0') {
    *corners = dim + 1;
  } else {
    *corners = (int) strtol(stringptr, &stringptr, 0);
  }
  if (*corners <= dim) {
    printf("  Error:  Elements in %s have only %d corners.\n", fname,
           *corners);
    return 1;
  }

  /* Read and check number of element attributes. */
  stringptr = findfield(stringptr);
  if (*stringptr == '\0') {
    extras = 0;
  } else {
    extras = (showmelong) strtol(stringptr, &stringptr, 0);
  }
  if ((extras < 0) && !quiet) {
    printf("  Warning:  %s has negative value for number of attributes.\n",
           fname);
  }

  /* Allocate space to store the points. */
  *eleptr = (showmelong *)
    malloc((*elems + 1) * (dim + 1) * sizeof(showmelong));
  if (*eleptr == (showmelong *) NULL) {
    printf("  Error:  Out of memory.\n");
    return 1;
  }

  /* Read the elements.  The corner index `index' starts at 'dim + 1',     */
  /*   because elements are internally numbered from 1 (regardless of how  */
  /*   they're numbered in the file).  They are assumed to be triangles or */
  /*   tetrahedra, and the first 'dim + 1' nodes are the corners.          */
  index = dim + 1;
  smallerr = 1;
  for (i = firstnumber; i < firstnumber + *elems; i++) {
    stringptr = readline(inputline, infile, fname);
    elemnumber = (showmelong) strtol(stringptr, &stringptr, 0);

    /* If elements are not numbered as expected, print a warning. */
    if ((elemnumber != i) && smallerr) {
      printf("  Warning:  Elements in %s are not numbered correctly.\n",
             fname);
      printf("            (starting with element %ld).\n", (long) i);
      /* Don't print more minor errors after this. */
      smallerr = 0;
    }

    /* Read this element's corners (as point indices). */
    for (j = 0; j <= dim; j++) {
      stringptr = findfield(stringptr);
      if (*stringptr == '\0') {
        printf("Error:  Element %ld is missing a corner in %s.\n",
               (long) i, fname);
        /* Free the memory for points and return an error indication. */
        free(*eleptr);
        return 1;
      }
      /* Store the corner (adjusting it so the vertices are internally */
      /*   numbered from one) and advance the corner index.            */
      (*eleptr)[index] = (showmelong) strtol(stringptr, &stringptr, 0) + 1 -
                         firstnumber;

      /* Check if the corner (point index) is valid. */
      if (((*eleptr)[index] < 1) || ((*eleptr)[index] > nodes)) {
        printf("Error:  Triangle %ld has invalid corner in %s.\n",
               (long) i, fname);
        return 1;
      }

      index++;
    }
  }

  fclose(infile);

  if (dim == 3) {
    /* Make sure all the tetrahedra have the correct orientation. */
    for (i = 1; i <= *elems; i++) {
      if (orient3d(&nodeptr[(*eleptr)[4 * i] * 3],
                   &nodeptr[(*eleptr)[4 * i + 1] * 3],
                   &nodeptr[(*eleptr)[4 * i + 2] * 3],
                   &nodeptr[(*eleptr)[4 * i + 3] * 3]) < 0.0) {
        printf("Warning:  Element %ld is inverted in %s.\n",
               (long) (i + firstnumber - 1), fname);
        /* Fix its orientation. */
        swap = (*eleptr)[4 * i + 3];
        (*eleptr)[4 * i + 3] = (*eleptr)[4 * i + 2];
        (*eleptr)[4 * i + 2] = swap;
      }
    }

    /* Determine which tetrahedra adjoin which tetrahedra. */
    matchfaces(nodeptr, nodes, *elems, dim, *eleptr, &tetraptr);
  }

  return 0;
}

int loadedges(char *fname,
              int firstnumber,
              showmelong nodes,
              showmelong *edges,
              showmelong **edgeptr,
              showmereal **normptr)
{
  FILE *infile;
  char inputline[INPUTLINESIZE];
  char *stringptr;
  showmelong edgenumber;
  int edgemarks;
  showmelong i;
  showmelong index;
  int smallerr;

  if (!quiet) {
    printf("Opening %s.\n", fname);
  }
  infile = fopen(fname, "r");
  if (infile == (FILE *) NULL) {
    printf("  Error:  Cannot access file %s.\n", fname);
    return 1;
  }

  /* Read and check number of edges. */
  stringptr = readline(inputline, infile, fname);
  *edges = (showmelong) strtol(stringptr, &stringptr, 0);
  if (*edges < 1) {
    printf("  Error:  %s contains %ld edges.\n", fname, (long) *edges);
    return 1;
  }

  /* Read and check number of edge markers. */
  stringptr = findfield(stringptr);
  if (*stringptr == '\0') {
    edgemarks = 0;
  } else {
    edgemarks = (int) strtol(stringptr, &stringptr, 0);
  }
  if (edgemarks < 0) {
    printf("  Warning:  %s has negative value for number of edge markers.\n",
           fname);
  }
  if (edgemarks > 1) {
    printf("  Warning:  %s has value greater than one for number of edge "
           "markers.\n", fname);
  }

  /* Allocate space to store the edges. */
  *edgeptr = (showmelong *) malloc(((*edges + 1) << 1) * sizeof(showmelong));
  if (*edgeptr == (showmelong *) NULL) {
    printf("  Error:  Out of memory.\n");
    return 1;
  }

  /* Allocate space to store the directions of infinite rays. */
  *normptr = (showmereal *) malloc(((*edges + 1) << 1) * sizeof(showmereal));
  if (*normptr == (showmereal *) NULL) {
    printf("  Error:  Out of memory.\n");
    /* Free the memory for edges and return an error indication. */
    free(*edgeptr);
    return 1;
  }

  /* Read the edges.  The corner index `index' starts at 2, because    */
  /*   edges are internally numbered from 1 (regardless of how they're */
  /*   numbered in the file).                                          */
  index = 2;
  smallerr = 1;
  for (i = firstnumber; i < firstnumber + *edges; i++) {
    stringptr = readline(inputline, infile, fname);
    edgenumber = (showmelong) strtol(stringptr, &stringptr, 0);

    /* If edges are not numbered as expected, print a warning. */
    if ((edgenumber != i) && (smallerr)) {
      printf("  Warning:  Edges in %s are not numbered correctly.\n", fname);
      printf("            (starting with edge %ld).\n", (long) i);
      /* Don't print more minor errors after this. */
      smallerr = 0;
    }

    /* Read the first edge endpoint; adjust it so the vertices are */
    /*   internally numbered from one.                             */
    stringptr = findfield(stringptr);
    if (*stringptr == '\0') {
      printf("Error:  Edge %ld is missing its endpoints in %s.\n",
             (long) i, fname);
      /* Free the memory for everything and return an error indication. */
      free(*edgeptr);
      free(*normptr);
      return 1;
    }
    (*edgeptr)[index] = (showmelong) strtol(stringptr, &stringptr, 0) + 1 -
                        firstnumber;

    /* Check if the first endpoint is valid. */
    if (((*edgeptr)[index] < 1) || ((*edgeptr)[index] > nodes)) {
      printf("Error:  Edge %ld has invalid endpoint in %s.\n",
             (long) i, fname);
      free(*edgeptr);
      free(*normptr);
      return 1;
    }

    /* Read the second edge endpoint. */
    stringptr = findfield(stringptr);
    if (*stringptr == '\0') {
      printf("Error:  Edge %ld is missing an endpoint in %s.\n",
             (long) i, fname);
      /* Free the memory for everything and return an error indication. */
      free(*edgeptr);
      free(*normptr);
      return 1;
    }
    (*edgeptr)[index + 1] = (showmelong) strtol(stringptr, &stringptr, 0);

    /* Is this edge an infinite ray (in a Voronoi diagram)? */
    if ((*edgeptr)[index + 1] == -1) {
      /* Yes.  Read the x-coordinate of its direction. */
      stringptr = findfield(stringptr);
      if (*stringptr == '\0') {
        printf("Error:  Edge %ld is missing its direction in %s.\n",
               (long) i, fname);
        free(*edgeptr);
        free(*normptr);
        return 1;
      }
      (*normptr)[index] = (showmereal) strtod(stringptr, &stringptr);

      /* Read the y-coordinate of its direction. */
      stringptr = findfield(stringptr);
      if (*stringptr == '\0') {
        printf("Error:  Edge %ld is missing a direction coordinate in %s.\n",
               (long) i, fname);
        free(*edgeptr);
        free(*normptr);
        return 1;
      }
      (*normptr)[index + 1] = (showmereal) strtod(stringptr, &stringptr);
    } else {
      /* Adjust the second endpoint so the vertices are internally numbered */
      /*   from one.                                                        */
      (*edgeptr)[index + 1] += 1 - firstnumber;

      /* Check if the second endpoint is valid. */
      if (((*edgeptr)[index + 1] < 1) || ((*edgeptr)[index + 1] > nodes)) {
        printf("Error:  Edge %ld has invalid endpoint in %s.\n",
               (long) i, fname);
        free(*edgeptr);
        free(*normptr);
        return 1;
      }
    }

    index += 2;
  }

  fclose(infile);

  return 0;
}

int loadpart(char *fname,
             int dim,
             int firstnumber,
             showmelong elems,
             showmereal *nodeptr,
             showmelong *eleptr,
             int *subdomains,
             int **partition,
             showmereal **subdomcenter,
             showmereal **subdomshift)
{
  FILE *infile;
  char inputline[INPUTLINESIZE];
  char *stringptr;
  showmelong partelems;
  showmelong elemnumber;
  showmelong i, j;
  int k;
  showmelong index;
  int smallerr;
  showmelong *subsizes;

  if (!quiet) {
    printf("Opening %s.\n", fname);
  }
  infile = fopen(fname, "r");
  if (infile == (FILE *) NULL) {
    printf("  Error:  Cannot access file %s.\n", fname);
    return 1;
  }

  /* Read and check number of elements. */
  stringptr = readline(inputline, infile, fname);
  partelems = (showmelong) strtol(stringptr, &stringptr, 0);
  /* Does it match the number in the .ele file? */
  if (partelems != elems) {
    printf("  Error:  %s does not agree with .ele file on number of "
           "elements.\n", fname);
    return 1;
  }

  /* Read and check number of subdomains. */
  stringptr = findfield(stringptr);
  if (*stringptr == '\0') {
    *subdomains = 1;
  } else {
    *subdomains = (int) strtol(stringptr, &stringptr, 0);
  }
  if (*subdomains < 1) {
    printf("  Error:  %s specifies %d subdomains.\n", fname, *subdomains);
    return 1;
  }

  /* Allocate space to store the mapping from elements to subdomains. */
  *partition = (int *) malloc((elems + 1) * sizeof(int));
  if (*partition == (int *) NULL) {
    printf("  Error:  Out of memory.\n");
    return 1;
  }

  /* Read the mapping from elements to subdomains. */
  smallerr = 1;
  for (i = firstnumber; i < firstnumber + partelems; i++) {
    stringptr = readline(inputline, infile, fname);
    elemnumber = (showmelong) strtol(stringptr, &stringptr, 0);

    /* If elements are not numbered as expected, print a warning. */
    if ((elemnumber != i) && (smallerr)) {
      printf("  Warning:  Elements in %s are not numbered correctly.\n",
             fname);
      printf("            (starting with element %ld).\n", (long) i);
      /* Don't print more minor errors after this. */
      smallerr = 0;
    }

    /* Read the first subdomain; adjust it so the subdomains are */
    /*   internally numbered from zero.                          */
    stringptr = findfield(stringptr);
    if (*stringptr == '\0') {
      printf("Error:  Element %ld has no subdomain in %s.\n",
             (long) i, fname);
      free(*partition);
      return 1;
    }
    /* Index j is adjusted so elements are internally numbered from one. */
    j = i + 1 - firstnumber;
    (*partition)[j] = (int) strtol(stringptr, &stringptr, 0) - firstnumber;

    /* Check if the subdomain number is valid. */
    if (((*partition)[j] >= *subdomains) || ((*partition)[j] < 0)) {
      printf("  Error:  Element %ld of %s has an invalid subdomain.\n",
             (long) i, fname);
      /* Free the memory for subdomains and return an error indication. */
      free(*partition);
      return 1;
    }
  }

  fclose(infile);

  /* Allocate space to hold the center of mass of each subdomain. */
  *subdomcenter = (showmereal *)
                  malloc((*subdomains + 1) * dim * sizeof(showmereal));
  if (*subdomcenter == (showmereal *) NULL) {
    printf("Error:  Out of memory.\n");
    free(*partition);
    return 1;
  }

  /* Allocate space to hold the relative displacement of each subdomain. */
  *subdomshift = (showmereal *) malloc(*subdomains * dim * sizeof(showmereal));
  if (*subdomshift == (showmereal *) NULL) {
    printf("Error:  Out of memory.\n");
    free(*partition);
    free(*subdomcenter);
    return 1;
  }

  /* Allocate space to hold the number of element corners in each subdomain. */
  subsizes = (showmelong *) malloc((*subdomains + 1) * sizeof(showmelong));
  if (subsizes == (showmelong *) NULL) {
    printf("Error:  Out of memory.\n");
    free(*partition);
    free(*subdomcenter);
    free(*subdomshift);
    return 1;
  }

  /* Initialize subdomain size and center. */
  for (i = 0; i <= *subdomains; i++) {
    subsizes[i] = 0;
    for (j = 0; j < dim; j++) {
      (*subdomcenter)[i * dim + j] = 0.0;
    }
  }

  /* Iterate over the elements.  The corner index `index' starts at */
  /*   `dim + 1', because elements are internally numbered from 1.  */
  index = dim + 1;
  for (i = 1; i <= elems; i++) {
    /* Sum the `dim + 1' corners of this element into `subdomcenter' . */
    for (j = 0; j < dim + 1; j++) {
      for (k = 0; k < dim; k++) {
        (*subdomcenter)[(*partition)[i] * dim + k] +=
          nodeptr[eleptr[index] * dim + k];
      }
      index++;
    }
    /* Keep track of the total number of corners in the sum. */
    subsizes[(*partition)[i]] += dim + 1;
  }

  /* Compute the subdomain centers of mass. */
  for (i = 0; i < *subdomains; i++) {
    for (j = 0; j < dim; j++) {
      /* Divide total by number of subdomains. */
      (*subdomcenter)[i * dim + j] /= (showmereal) subsizes[i];
      /* Maintain a grand total of subdomain centers of mass. */
      (*subdomcenter)[*subdomains * dim + j] += (*subdomcenter)[i * dim + j];
    }
  }
  /* Compute a center of mass of the centers of mass. */
  for (j = 0; j < dim; j++) {
    (*subdomcenter)[*subdomains * dim + j] /= (showmereal) *subdomains;
  }

  /* `subsizes' is no longer needed. */
  free(subsizes);

  return 0;
}

int loadadj(char *fname,
            int *subdomains,
            int **ptr)
{
  FILE *infile;
  char inputline[INPUTLINESIZE];
  char *stringptr;
  int i, j;

  if (!quiet) {
    printf("Opening %s.\n", fname);
  }
  infile = fopen(fname, "r");
  if (infile == (FILE *) NULL) {
    printf("  Error:  Cannot access file %s.\n", fname);
    return 1;
  }

  /* Read and check number of subdomains. */
  stringptr = readline(inputline, infile, fname);
  if (*stringptr == '\0') {
    *subdomains = 1;
  } else {
    *subdomains = (int) strtol(stringptr, &stringptr, 0);
  }
  if (*subdomains < 1) {
    printf("  Error:  %s specifies %d subdomains.\n", fname, *subdomains);
    return 1;
  }

  /* Allocate space to store the adjacency graph. */
  *ptr = (int *) malloc(*subdomains * *subdomains * sizeof(int));
  if (*ptr == (int *) NULL) {
    printf("  Error:  Out of memory.\n");
    return 1;
  }

  /* Read the adjacency graph. */
  for (i = 0; i < *subdomains; i++) {
    for (j = 0; j < *subdomains; j++) {
      stringptr = readline(inputline, infile, fname);
      (*ptr)[i * *subdomains + j] = (int) strtol(stringptr, &stringptr, 0);
    }
  }
  return 0;
}

int loaddata(char *fname,
             int firstnumber,
             showmelong nodes,
             int *datadim,
             showmereal **dataptr,
             showmereal *datahist)
{
  FILE *infile;
  char inputline[INPUTLINESIZE];
  char *stringptr;
  showmelong datavalues;
  showmelong edgenumber;
  showmelong i;
  showmelong index;
  int smallerr;

  if (!quiet) {
    printf("Opening %s.\n", fname);
  }
  infile = fopen(fname, "r");
  if (infile == (FILE *) NULL) {
    printf("  Error:  Cannot access file %s.\n", fname);
    return 1;
  }

  /* Read and check number of nodes. */
  stringptr = readline(inputline, infile, fname);
  datavalues = (showmelong) strtol(stringptr, &stringptr, 0);
  if (datavalues != nodes) {
    printf("  Error:  %s does not agree with .node file on number of "
           "nodes.\n", fname);
    return 1;
  }

  /* Read and check number of data per node. */
  stringptr = findfield(stringptr);
  if (*stringptr == '\0') {
    *datadim = 1;
  } else {
    *datadim = (int) strtol(stringptr, &stringptr, 0);
  }
  if (*datadim < 1) {
    printf("  Error:  %s specifies %d data values per node.\n",
           fname, *datadim);
    return 1;
  }

  /* Allocate space to store the data. */
  *dataptr = (showmereal *) malloc((datavalues + 1) * sizeof(showmereal));
  if (*dataptr == (showmereal *) NULL) {
    printf("  Error:  Out of memory.\n");
    return 1;
  }

  /* Read the data. */
  index = 1;
  smallerr = 1;
  for (i = firstnumber; i < firstnumber + datavalues; i++) {
    stringptr = readline(inputline, infile, fname);
    edgenumber = (showmelong) strtol(stringptr, &stringptr, 0);

    /* If data are not numbered as expected, print a warning. */
    if ((edgenumber != i) && (smallerr)) {
      printf("  Warning:  Data in %s are not numbered correctly.\n", fname);
      printf("            (starting with data value %ld).\n", (long) i);
      /* Don't print more minor errors after this. */
      smallerr = 0;
    }

    stringptr = findfield(stringptr);
    if (*stringptr == '\0') {
      printf("  Error:  Datum %ld is missing from %s.\n", (long) i, fname);
      free(*dataptr);
      return 1;
    }
    (*dataptr)[index++] = (showmereal) strtod(stringptr, &stringptr);

    /* Keep track of the least and greatest data. */
    if (i == firstnumber) {
      datahist[0] = (*dataptr)[index - 1];
      datahist[1] = (*dataptr)[index - 1];
    } else {
      if (datahist[0] > (*dataptr)[index - 1]) {
        datahist[0] = (*dataptr)[index - 1];
      }
      if (datahist[1] < (*dataptr)[index - 1]) {
        datahist[1] = (*dataptr)[index - 1];
      }
    }
  }

  fclose(infile);

  return 0;
}

void findsubdomshift(int dim,
                     int subdomains,
                     showmereal explosion,
                     showmereal *subdomcenter,
                     showmereal *subdomshift)
{
  int i, j;

  /* Loop through subdomains and coordinates of each subdomain's center. */
  for (i = 0; i < subdomains; i++) {
    for (j = 0; j < dim; j++) {
      /* Each subdomain is shifted away from the center of mass of all the */
      /*   subdomains, by the factor `explosion'.                          */
      subdomshift[i * dim + j] = explosion *
        (subdomcenter[i * dim + j] - subdomcenter[subdomains * dim + j]);
    }
  }
}

int load_image(int inc,
               int image)
{
  int error;

  switch (image) {
    case NODE:
      error = loadnodes(nodefilename[inc], &firstnumber[inc], &nodes[inc],
                        &nodedim[inc], &nodeptr[inc],
                        &xlo[inc][NODE], &ylo[inc][NODE], &zlo[inc][NODE],
                        &xhi[inc][NODE], &yhi[inc][NODE], &zhi[inc][NODE]);
      break;
    case POLY:
      error = loadpoly(inc, polyfilename[inc], &firstnumber[inc],
                       &polynodes[inc], &polydim[inc], &polyedges[inc],
                       &polyholes[inc], &polynodeptr[inc], &polysegptr[inc],
                       &polyholeptr[inc],
                       &xlo[inc][POLY], &ylo[inc][POLY], &zlo[inc][POLY],
                       &xhi[inc][POLY], &yhi[inc][POLY], &zhi[inc][POLY]);
      break;
    case ELE:
      error = loadelems(elefilename[inc], nodedim[inc], firstnumber[inc],
                        nodes[inc], nodeptr[inc], &elems[inc],
                        &elecorners[inc], &eleptr[inc]);
      xlo[inc][ELE] = xlo[inc][NODE];
      ylo[inc][ELE] = ylo[inc][NODE];
      zlo[inc][ELE] = zlo[inc][NODE];
      xhi[inc][ELE] = xhi[inc][NODE];
      yhi[inc][ELE] = yhi[inc][NODE];
      zhi[inc][ELE] = zhi[inc][NODE];
      break;
    case EDGE:
      error = loadedges(edgefilename[inc], firstnumber[inc], nodes[inc],
                        &edges[inc], &edgeptr[inc], &normptr[inc]);
      xlo[inc][EDGE] = xlo[inc][NODE];
      ylo[inc][EDGE] = ylo[inc][NODE];
      zlo[inc][EDGE] = zlo[inc][NODE];
      xhi[inc][EDGE] = xhi[inc][NODE];
      yhi[inc][EDGE] = yhi[inc][NODE];
      zhi[inc][EDGE] = zhi[inc][NODE];
      break;
    case PART:
      error = loadpart(partfilename[inc], nodedim[inc], firstnumber[inc],
                       elems[inc], nodeptr[inc], eleptr[inc],
                       &subdomains[inc], &partition[inc], &subdomcenter[inc],
                       &subdomshift[inc]);
      if (!error) {
        findsubdomshift(nodedim[inc], subdomains[inc], explosion,
                        subdomcenter[inc], subdomshift[inc]);
      }
      xlo[inc][PART] = xlo[inc][NODE];
      ylo[inc][PART] = ylo[inc][NODE];
      zlo[inc][PART] = zlo[inc][NODE];
      xhi[inc][PART] = xhi[inc][NODE];
      yhi[inc][PART] = yhi[inc][NODE];
      zhi[inc][PART] = zhi[inc][NODE];
      break;
    case ADJ:
      error = loadadj(adjfilename[inc], &adjsubdomains[inc], &adjptr[inc]);
      xlo[inc][ADJ] = xlo[inc][NODE];
      ylo[inc][ADJ] = ylo[inc][NODE];
      zlo[inc][ADJ] = zlo[inc][NODE];
      xhi[inc][ADJ] = xhi[inc][NODE];
      yhi[inc][ADJ] = yhi[inc][NODE];
      zhi[inc][ADJ] = zhi[inc][NODE];
      break;
    case VORO:
      error = loadnodes(vnodefilename[inc], &firstnumber[inc], &vnodes[inc],
                        &vnodedim[inc], &vnodeptr[inc],
                        &xlo[inc][VORO], &ylo[inc][VORO], &zlo[inc][VORO],
                        &xhi[inc][VORO], &yhi[inc][VORO], &zhi[inc][VORO]);
      if (!error) {
        error = loadedges(vedgefilename[inc], firstnumber[inc], vnodes[inc],
                          &vedges[inc], &vedgeptr[inc], &vnormptr[inc]);
      }
      break;
    case DATA:
      error = loaddata(datafilename[inc], firstnumber[inc], nodes[inc],
                       &datadim[inc], &dataptr[inc], datahist[inc]);
      xlo[inc][DATA] = xlo[inc][NODE];
      ylo[inc][DATA] = ylo[inc][NODE];
      zlo[inc][DATA] = zlo[inc][NODE];
      xhi[inc][DATA] = xhi[inc][NODE];
      yhi[inc][DATA] = yhi[inc][NODE];
      zhi[inc][DATA] = zhi[inc][NODE];
      break;
    default:
      error = 1;
  }
  if (!error) {
    loaded[inc][image] = 1;
  }
  return error;
}

void choose_image(int inc,
                  int image)
{
  if (!loaded[inc][image]) {
    if ((image == ELE) || (image == EDGE) || (image == PART) || (image == ADJ)
        || (image == DATA)) {
      if (!loaded[inc][NODE]) {
        if (load_image(inc, NODE)) {
          return;
        }
      }
    }
    if ((image == PART) || (image == ADJ) || (image == DATA)) {
      if (!loaded[inc][ELE]) {
        if (load_image(inc, ELE)) {
          return;
        }
      }
    }
    if (image == ADJ) {
      if (!loaded[inc][PART]) {
        if (load_image(inc, PART)) {
          return;
        }
      }
    }
    if (load_image(inc, image)) {
      return;
    }
  }
  current_inc = inc;
  current_image = image;
}

Window make_button(char *name,
                   int x,
                   int y,
                   unsigned int width)
{
  XSetWindowAttributes attr;
  XSizeHints hints;
  Window button;

  attr.background_pixel = black;
  attr.border_pixel = white;
  attr.backing_store = NotUseful;
  attr.event_mask = ExposureMask | ButtonReleaseMask | ButtonPressMask;
  attr.bit_gravity = SouthWestGravity;
  attr.win_gravity = SouthWestGravity;
  attr.save_under = False;
  button = XCreateWindow(display, mainwindow, x, y, width, BUTTONHEIGHT - 4,
                         2, 0, InputOutput, CopyFromParent,
                         CWBackPixel | CWBorderPixel | CWEventMask |
                         CWBitGravity | CWWinGravity | CWBackingStore |
                         CWSaveUnder, &attr);
  hints.width = width;
  hints.height = BUTTONHEIGHT - 4;
  hints.min_width = 0;
  hints.min_height = BUTTONHEIGHT - 4;
  hints.max_width = width;
  hints.max_height = BUTTONHEIGHT - 4;
  hints.width_inc = 1;
  hints.height_inc = 1;
  hints.flags = PMinSize | PMaxSize | PSize | PResizeInc;
  XSetStandardProperties(display, button, name, "showme", None, (char **) NULL,
                         0, &hints);
  return button;
}

void make_buttons(int y)
{
  char rotatewinname[2];
  int i;

  for (i = 1; i >= 0; i--) {
    nodewin[i] = make_button("node", 0, y + (1 - i) * BUTTONHEIGHT, 42);
    XMapWindow(display, nodewin[i]);
    polywin[i] = make_button("poly", 44, y + (1 - i) * BUTTONHEIGHT, 42);
    XMapWindow(display, polywin[i]);
    elewin[i] = make_button("ele", 88, y + (1 - i) * BUTTONHEIGHT, 33);
    XMapWindow(display, elewin[i]);
    edgewin[i] = make_button("edge", 123, y + (1 - i) * BUTTONHEIGHT, 42);
    XMapWindow(display, edgewin[i]);
    partwin[i] = make_button("part", 167, y + (1 - i) * BUTTONHEIGHT, 42);
    XMapWindow(display, partwin[i]);
    adjwin[i] = make_button("adj", 211, y + (1 - i) * BUTTONHEIGHT, 33);
    XMapWindow(display, adjwin[i]);
    voronoiwin[i] = make_button("voro", 246, y + (1 - i) * BUTTONHEIGHT, 42);
    XMapWindow(display, voronoiwin[i]);
    datawin[i] = make_button("data", 290, y + (1 - i) * BUTTONHEIGHT, 42);
    XMapWindow(display, datawin[i]);

  }
  versionpluswin = make_button("    +", 334, y, 52);
  XMapWindow(display, versionpluswin);
  versionminuswin = make_button("    -", 334, y + BUTTONHEIGHT, 52);
  XMapWindow(display, versionminuswin);

  motionwin = make_button("Rot", 0, y + 2 * BUTTONHEIGHT, 33);
  XMapWindow(display, motionwin);
  for (i = 0; i < 6; i++) {
    sprintf(rotatewinname, "%d", i);
    rotatewin[i] = make_button(rotatewinname, 35 + i * 20,
                               y + 2 * BUTTONHEIGHT, 18);
    XMapWindow(display, rotatewin[i]);
  }
  rotateamtwin = make_button("Amt", 155, y + 2 * BUTTONHEIGHT, 33);
  XMapWindow(display, rotateamtwin);
  wireframeoptionwin = make_button("Wire", 190, y + 2 * BUTTONHEIGHT, 42);
  XMapWindow(display, wireframeoptionwin);
  cutleftwin = make_button("Cut<", 234, y + 2 * BUTTONHEIGHT, 42);
  XMapWindow(display, cutleftwin);
  cutrightwin = make_button(">", 278, y + 2 * BUTTONHEIGHT, 14);
  XMapWindow(display, cutrightwin);
  cutupwin = make_button("^", 294, y + 2 * BUTTONHEIGHT, 14);
  XMapWindow(display, cutupwin);
  cutdownwin = make_button("v", 310, y + 2 * BUTTONHEIGHT, 14);
  XMapWindow(display, cutdownwin);
  perspectivewin = make_button("Persp", 326, y + 2 * BUTTONHEIGHT, 52);
  XMapWindow(display, perspectivewin);
  perspluswin = make_button("+", 380, y + 2 * BUTTONHEIGHT, 14);
  XMapWindow(display, perspluswin);
  persminuswin = make_button("-", 396, y + 2 * BUTTONHEIGHT, 14);
  XMapWindow(display, persminuswin);
  qualitywin = make_button("Q", 396, y + BUTTONHEIGHT, 14);
  XMapWindow(display, qualitywin);

  quitwin = make_button("Quit", 0, y + 3 * BUTTONHEIGHT, 42);
  XMapWindow(display, quitwin);
  leftwin = make_button("<", 44, y + 3 * BUTTONHEIGHT, 14);
  XMapWindow(display, leftwin);
  rightwin = make_button(">", 60, y + 3 * BUTTONHEIGHT, 14);
  XMapWindow(display, rightwin);
  upwin = make_button("^", 76, y + 3 * BUTTONHEIGHT, 14);
  XMapWindow(display, upwin);
  downwin = make_button("v", 92, y + 3 * BUTTONHEIGHT, 14);
  XMapWindow(display, downwin);
  resetwin = make_button("Reset", 108, y + 3 * BUTTONHEIGHT, 52);
  XMapWindow(display, resetwin);
  widthpluswin = make_button("Width+", 162, y + 3 * BUTTONHEIGHT, 61);
  XMapWindow(display, widthpluswin);
  widthminuswin = make_button("-", 225, y + 3 * BUTTONHEIGHT, 14);
  XMapWindow(display, widthminuswin);
  expwin = make_button("Exp", 241, y + 3 * BUTTONHEIGHT, 33);
  XMapWindow(display, expwin);
  exppluswin = make_button("+", 276, y + 3 * BUTTONHEIGHT, 14);
  XMapWindow(display, exppluswin);
  expminuswin = make_button("-", 292, y + 3 * BUTTONHEIGHT, 14);
  XMapWindow(display, expminuswin);
  fillwin = make_button("Fill", 308, y + 3 * BUTTONHEIGHT, 41);
  XMapWindow(display, fillwin);
  pswin = make_button("PS", 351, y + 3 * BUTTONHEIGHT, 24);
  XMapWindow(display, pswin);
  epswin = make_button("EPS", 377, y + 3 * BUTTONHEIGHT, 33);
  XMapWindow(display, epswin);
}

void fill_button(Window button)
{
  int x, y;
  unsigned int w, h, d, b;
  Window rootw;

  XGetGeometry(display, button, &rootw, &x, &y, &w, &h, &d, &b);
  XFillRectangle(display, button, fontgc, 0, 0, w, h);
}

void draw_buttons()
{
  char numberstring[32];
  char buttonstring[6];
  int rotatenumber;
  int i;

  for (i = 1; i >= 0; i--) {
    if ((current_image == NODE) && (current_inc == i)) {
      fill_button(nodewin[i]);
      XDrawString(display, nodewin[i], blackfontgc, 2, 13, "node", 4);
    } else {
      XClearWindow(display, nodewin[i]);
      XDrawString(display, nodewin[i], fontgc, 2, 13, "node", 4);
    }
    if ((current_image == POLY) && (current_inc == i)) {
      fill_button(polywin[i]);
      XDrawString(display, polywin[i], blackfontgc, 2, 13, "poly", 4);
    } else {
      XClearWindow(display, polywin[i]);
      XDrawString(display, polywin[i], fontgc, 2, 13, "poly", 4);
    }
    if ((current_image == ELE) && (current_inc == i)) {
      fill_button(elewin[i]);
      XDrawString(display, elewin[i], blackfontgc, 2, 13, "ele", 3);
    } else {
      XClearWindow(display, elewin[i]);
      XDrawString(display, elewin[i], fontgc, 2, 13, "ele", 3);
    }
    if ((current_image == EDGE) && (current_inc == i)) {
      fill_button(edgewin[i]);
      XDrawString(display, edgewin[i], blackfontgc, 2, 13, "edge", 4);
    } else {
      XClearWindow(display, edgewin[i]);
      XDrawString(display, edgewin[i], fontgc, 2, 13, "edge", 4);
    }
    if ((current_image == PART) && (current_inc == i)) {
      fill_button(partwin[i]);
      XDrawString(display, partwin[i], blackfontgc, 2, 13, "part", 4);
    } else {
      XClearWindow(display, partwin[i]);
      XDrawString(display, partwin[i], fontgc, 2, 13, "part", 4);
    }
    if ((current_image == ADJ) && (current_inc == i)) {
      fill_button(adjwin[i]);
      XDrawString(display, adjwin[i], blackfontgc, 2, 13, "adj", 3);
    } else {
      XClearWindow(display, adjwin[i]);
      XDrawString(display, adjwin[i], fontgc, 2, 13, "adj", 3);
    }
    if ((current_image == VORO) && (current_inc == i)) {
      fill_button(voronoiwin[i]);
      XDrawString(display, voronoiwin[i], blackfontgc, 2, 13, "voro", 4);
    } else {
      XClearWindow(display, voronoiwin[i]);
      XDrawString(display, voronoiwin[i], fontgc, 2, 13, "voro", 4);
    }
    if ((current_image == DATA) && (current_inc == i)) {
      fill_button(datawin[i]);
      XDrawString(display, datawin[i], blackfontgc, 2, 13, "data", 4);
    } else {
      XClearWindow(display, datawin[i]);
      XDrawString(display, datawin[i], fontgc, 2, 13, "data", 4);
    }
  }

  XClearWindow(display, versionpluswin);
  sprintf(numberstring, "%d", loweriteration + 1);
  sprintf(buttonstring, "%-4.4s+", numberstring);
  XDrawString(display, versionpluswin, fontgc, 2, 13, buttonstring, 5);
  XClearWindow(display, versionminuswin);
  sprintf(numberstring, "%d", loweriteration);
  if (loweriteration == 0) {
    sprintf(buttonstring, "%-4.4s", numberstring);
  } else {
    sprintf(buttonstring, "%-4.4s-", numberstring);
  }
  XDrawString(display, versionminuswin, fontgc, 2, 13, buttonstring, 5);

  XClearWindow(display, motionwin);
  for (rotatenumber = 0; rotatenumber < 6; rotatenumber++) {
    XClearWindow(display, rotatewin[rotatenumber]);
  }
  XClearWindow(display, rotateamtwin);
  XClearWindow(display, wireframeoptionwin);
  XClearWindow(display, perspluswin);
  XClearWindow(display, persminuswin);
  XClearWindow(display, perspectivewin);
  XClearWindow(display, cutleftwin);
  XClearWindow(display, cutrightwin);
  XClearWindow(display, cutupwin);
  XClearWindow(display, cutdownwin);
  XClearWindow(display, qualitywin);
  if ((current_image == DATA) || (nodedim[current_inc] == 3)) {
    if (motion) {
      fill_button(motionwin);
      XDrawString(display, motionwin, blackfontgc, 2, 13, "Rot", 3);
    } else {
      XDrawString(display, motionwin, fontgc, 2, 13, "Rot", 3);
    }
    /* Draw the little pan, tilt, and twist arrow indicators */
    XDrawArc(display, rotatewin[0], fontgc, 4, 2, 8, 12, -5760, -16000);
    XDrawRectangle(display, rotatewin[0], fontgc, 9, 12, 2, 2);
    XDrawArc(display, rotatewin[1], fontgc, 4, 2, 8, 12, -5760, -16000);
    XDrawRectangle(display, rotatewin[1], fontgc, 11, 4, 2, 2);
    XDrawArc(display, rotatewin[2], fontgc, 2, 4, 12, 8, 0, -16000);
    XDrawRectangle(display, rotatewin[2], fontgc, 6, 3, 2, 2);
    XDrawArc(display, rotatewin[3], fontgc, 2, 4, 12, 8, 0, -16000);
    XDrawRectangle(display, rotatewin[3], fontgc, 13, 6, 2, 2);
    XDrawArc(display, rotatewin[4], fontgc, 2, 2, 12, 12, -1280, -21760);
    XDrawRectangle(display, rotatewin[4], fontgc, 13, 11, 2, 2);
    XDrawArc(display, rotatewin[5], fontgc, 2, 2, 12, 12, -1280, -21760);
    XDrawRectangle(display, rotatewin[5], fontgc, 13, 5, 2, 2);
    XDrawString(display, rotateamtwin, fontgc, 2, 13, "Amt", 3);
    if (wireframe) {
      fill_button(wireframeoptionwin);
      XDrawString(display, wireframeoptionwin, blackfontgc, 2, 13, "Wire", 4);
    } else {
      XDrawString(display, wireframeoptionwin, fontgc, 2, 13, "Wire", 4);
    }
    if (perspective) {
      fill_button(perspectivewin);
      XDrawString(display, perspectivewin, blackfontgc, 2, 13, "Persp", 5);
      XDrawString(display, perspluswin, fontgc, 2, 13, "+", 1);
      XDrawString(display, persminuswin, fontgc, 2, 13, "-", 1);
    } else {
      XDrawString(display, perspectivewin, fontgc, 2, 13, "Persp", 5);
    }
    if (nodedim[current_inc] == 3) {
      XDrawString(display, cutleftwin, fontgc, 2, 13, "Cut<", 4);
      XDrawString(display, cutrightwin, fontgc, 2, 13, ">", 1);
      XDrawString(display, cutupwin, fontgc, 2, 13, "^", 1);
      XDrawString(display, cutdownwin, fontgc, 2, 13, "v", 1);
      if ((current_image == ELE) || (current_image == PART)
          || (current_image == DATA)) {
        XDrawString(display, qualitywin, fontgc, 2, 13, "Q", 1);
      }
    }
  }

  XClearWindow(display, quitwin);
  XDrawString(display, quitwin, fontgc, 2, 13, "Quit", 4);
  XClearWindow(display, leftwin);
  XDrawString(display, leftwin, fontgc, 2, 13, "<", 1);
  XClearWindow(display, rightwin);
  XDrawString(display, rightwin, fontgc, 2, 13, ">", 1);
  XClearWindow(display, upwin);
  XDrawString(display, upwin, fontgc, 2, 13, "^", 1);
  XClearWindow(display, downwin);
  XDrawString(display, downwin, fontgc, 2, 13, "v", 1);
  XClearWindow(display, resetwin);
  XDrawString(display, resetwin, fontgc, 2, 13, "Reset", 6);
  XClearWindow(display, widthpluswin);
  if (line_width < 100) {
    XDrawString(display, widthpluswin, fontgc, 2, 13, "Width+", 6);
  } else {
    XDrawString(display, widthpluswin, fontgc, 2, 13, "Width ", 6);
  }
  XClearWindow(display, widthminuswin);
  if (line_width > 1) {
    XDrawString(display, widthminuswin, fontgc, 2, 13, "-", 1);
  }
  XClearWindow(display, expwin);
  XClearWindow(display, exppluswin);
  XClearWindow(display, expminuswin);
  XClearWindow(display, fillwin);
  if (current_image == PART) {
    if (explode) {
      fill_button(expwin);
      XDrawString(display, expwin, blackfontgc, 2, 13, "Exp", 3);
    } else {
      XDrawString(display, expwin, fontgc, 2, 13, "Exp", 3);
    }
    XDrawString(display, exppluswin, fontgc, 2, 13, "+", 1);
    XDrawString(display, expminuswin, fontgc, 2, 13, "-", 1);
  }
  if ((current_image == PART) || (current_image == DATA)) {
    if (fillelem) {
      fill_button(fillwin);
      XDrawString(display, fillwin, blackfontgc, 2, 13, "Fill", 4);
    } else {
      XDrawString(display, fillwin, fontgc, 2, 13, "Fill", 4);
    }
  }
  XClearWindow(display, pswin);
  XDrawString(display, pswin, fontgc, 2, 13, "PS", 2);
  XClearWindow(display, epswin);
  XDrawString(display, epswin, fontgc, 2, 13, "EPS", 3);
}

Window create_popup_window(char *name,
                           int x,
                           int y,
                           unsigned int width,
                           unsigned int height,
                           int bw,
                           int back_bw)
{
  XSetWindowAttributes attr;
  XSizeHints hints;
  Window tmpwin;

  black = BlackPixel(display, screen);
  white = WhitePixel(display, screen);
  windowdepth = DefaultDepth(display, screen);
  rootmap = DefaultColormap(display, screen);
  attr.background_pixel = white;
  attr.border_pixel = black;
  attr.backing_store = NotUseful;
  attr.event_mask = ExposureMask | ButtonReleaseMask | ButtonPressMask
                  | StructureNotifyMask;
  attr.bit_gravity = NorthWestGravity;
  attr.win_gravity = NorthWestGravity;
  attr.save_under = False;
  tmpwin=XCreateWindow(display, rootwindow, x, y, width,
                       height, 3, 0,
                       InputOutput, CopyFromParent,
                       CWBackPixel | CWBorderPixel | CWEventMask
                       | CWBitGravity | CWWinGravity | CWBackingStore
                       | CWSaveUnder, &attr);
  hints.min_width = 0;
  hints.min_height = 0;
  hints.max_width = width;
  hints.max_height = height;
  hints.width_inc = 1;
  hints.height_inc = 1;
  hints.flags = PMinSize | PSize | PResizeInc;

  XSetStandardProperties(display, tmpwin, name, "popup", None, 0, 0, &hints);
  XClearWindow(display, tmpwin);
  XMapWindow(display, tmpwin);
  XFlush(display);

  return tmpwin;
}

char answer[132]; /* A temporary global so no mallocing is done by get_line */

char *get_line(char *prompt)
{
  Window popup, root_ret;

  int x, y;
  unsigned int w_ret, h_ret, bw_ret, d_ret;
  char string[10];
  int nchar, num_letters;
  char c;

  XGetGeometry(display, rotateamtwin, &root_ret, &x, &y, &w_ret, &h_ret,
               &bw_ret, &d_ret);

  popup = create_popup_window("Dialog", x, y,
                              strlen(prompt) * 10 + 82, 30, 4, 1);
  XSelectInput(display, popup, KeyPressMask);

  XMapWindow(display, popup);
  XClearWindow(display, popup);

  XDrawString(display, popup, blackfontgc, 2, 17, prompt,
              (int) strlen(prompt));
  XFlush(display);

  num_letters = 0;

/*
  XFillRectangle(display, popup, fontgc,
                 (strlen(prompt) + num_letters) * 10 + 12, 9, 6, 10);
*/

  while (1) {
    XMaskEvent(display, KeyPressMask, &event);

    nchar = XLookupString(&event.xkey, string, 10, NULL, NULL);
    if (nchar == 0) {
      continue;
    }

    c = *string;

    if (c == '\r') {
      break;
    }

    if ((c == '\b') || (c == '\177')) { /* backspace */
      if (num_letters > 0) {
	XFillRectangle(display, popup, fontgc,
                       ((int) strlen(prompt) + num_letters) * 10 + 12,
                       9, 6, 10);
	num_letters -= 1;
	XFillRectangle(display, popup, blackfontgc,
                       ((int) strlen(prompt) + num_letters) * 10 + 12,
                       9, 6, 10);
	answer[num_letters] = '\0';
      }
    }
    else if (num_letters < 130) {
      XFillRectangle(display, popup, fontgc,
                     ((int) strlen(prompt) + num_letters) * 10 + 12, 9, 6, 10);
      ++num_letters;
      XFillRectangle(display, popup, blackfontgc,
                     ((int) strlen(prompt) + num_letters) * 10 + 12, 9, 6, 10);
      answer[num_letters - 1] = c;
      answer[num_letters] = '\0';

      XDrawString(display, popup, blackfontgc,
                  ((int) strlen(prompt) + num_letters) * 10 + 2,
                  17, string, 1);
    }
    XFlush(display);
  }
  XDestroyWindow(display,popup);
  XFlush(display);
  return answer;
}

void showme_window(int argc,
                   char **argv)
{
  XSetWindowAttributes attr;
  XSizeHints hints;
  XGCValues fontvalues, linevalues;
  XColor alloc_color, exact_color;
  char grayname[8];
  int i;

  display = XOpenDisplay((char *) NULL);
  if (!display) {
    printf("Error:  Cannot open display.\n");
    exit(1);
  }
  screen = DefaultScreen(display);
  rootwindow = DefaultRootWindow(display);
  black = BlackPixel(display, screen);
  white = WhitePixel(display, screen);
  windowdepth = DefaultDepth(display, screen);
  rootmap = DefaultColormap(display, screen);
  width = STARTWIDTH;
  height = STARTHEIGHT;
  attr.background_pixel = black;
  attr.border_pixel = white;
  attr.backing_store = NotUseful;
  attr.event_mask = ExposureMask | ButtonReleaseMask | ButtonPressMask |
                    StructureNotifyMask | PointerMotionMask;
  attr.bit_gravity = NorthWestGravity;
  attr.win_gravity = NorthWestGravity;
  attr.save_under = False;
  mainwindow = XCreateWindow(display, rootwindow, 0, 0, width,
                             height + PANELHEIGHT, 3, 0,
                             InputOutput, CopyFromParent,
                             CWBackPixel | CWBorderPixel | CWEventMask |
                             CWBitGravity | CWWinGravity | CWBackingStore |
                             CWSaveUnder, &attr);
  hints.width = width;
  hints.height = height + PANELHEIGHT;
  hints.min_width = MINWIDTH;
  hints.min_height = MINHEIGHT + PANELHEIGHT;
  hints.width_inc = 1;
  hints.height_inc = 1;
  hints.flags = PMinSize | PSize | PResizeInc;
  XSetStandardProperties(display, mainwindow, "Show Me", "showme", None,
                         argv, argc, &hints);
  XChangeProperty(display, mainwindow, XA_WM_CLASS, XA_STRING, 8,
                  PropModeReplace, (unsigned char *) "showme\0Archimedes", 18);
  XClearWindow(display, mainwindow);
  XMapWindow(display, mainwindow);
  if ((windowdepth > 1) &&
      XAllocNamedColor(display, rootmap, "yellow", &alloc_color,
                       &exact_color)) {
    colordisplay = 1;
    explode = bw_ps;
    fontvalues.foreground = alloc_color.pixel;
    linevalues.foreground = alloc_color.pixel;
    showme_foreground = alloc_color.pixel;
    for (i = 0; i < MAXCOLORS; i++) {
      if (XAllocNamedColor(display, rootmap, colorname[i], &alloc_color,
                           &rgb[i])) {
        colors[i] = alloc_color.pixel;
      } else {
        colors[i] = white;
        rgb[i].red = alloc_color.red;
        rgb[i].green = alloc_color.green;
        rgb[i].blue = alloc_color.blue;
        if (!quiet) {
          printf("Warning:  I could not allocate %s.\n", colorname[i]);
        }
      }
    }
    for (i = 0; i < MAXGRAYS; i++) {
      sprintf(grayname, "gray%d",
              25 + (int) (75.0 * (showmereal) i / (showmereal) MAXGRAYS));
      if (XAllocNamedColor(display, rootmap, grayname, &alloc_color,
                           &gray[i])) {
        grays[i] = alloc_color.pixel;
      } else {
        grays[i] = white;
        gray[i].red = alloc_color.red;
        gray[i].green = alloc_color.green;
        gray[i].blue = alloc_color.blue;
        if (!quiet) {
          printf("Warning:  I could not allocate %s.\n", grayname);
        }
      }
    }
  } else {
    colordisplay = 0;
    fillelem = 0;
    explode = 1;
    fontvalues.foreground = white;
    linevalues.foreground = white;
    showme_foreground = white;
  }
  myfont = XLoadQueryFont(display, "9x15");
  fontvalues.background = black;
  fontvalues.font = myfont->fid;
  fontvalues.fill_style = FillSolid;
  fontvalues.line_width = 2;
  fontgc = XCreateGC(display, rootwindow, GCForeground | GCBackground |
                      GCFont | GCLineWidth | GCFillStyle, &fontvalues);
  fontvalues.foreground = black;
  blackfontgc = XCreateGC(display, rootwindow, GCForeground | GCBackground |
                         GCFont | GCLineWidth | GCFillStyle, &fontvalues);
  linevalues.background = black;
  linevalues.line_width = line_width;
  linevalues.cap_style = CapRound;
  linevalues.join_style = JoinRound;
  linevalues.fill_style = FillSolid;
  linegc = XCreateGC(display, rootwindow, GCForeground | GCBackground |
                     GCLineWidth | GCCapStyle | GCJoinStyle | GCFillStyle,
                     &linevalues);
  linevalues.foreground = black;
  trianglegc = XCreateGC(display, rootwindow, GCForeground | GCBackground |
                         GCLineWidth | GCCapStyle | GCJoinStyle | GCFillStyle,
                         &linevalues);
  make_buttons((int) height);
  XFlush(display);
}

void draw_node2d(showmelong nodes,
                 showmereal *nodeptr,
                 showmereal xcenter,
                 showmereal ycenter,
                 showmereal xscale,
                 showmereal yscale,
                 showmereal xoffset,
                 showmereal yoffset)
{
  showmelong i;
  showmelong index;

  index = 2;
  for (i = 1; i <= nodes; i++) {
    XFillRectangle(display, mainwindow, linegc,
                   (int) ((nodeptr[index] - xcenter) * xscale + xoffset -
                          (line_width >> 1)),
                   (int) ((nodeptr[index + 1] - ycenter) * yscale + yoffset -
                          (line_width >> 1)), line_width, line_width);
    index += 2;
  }
}

void draw_node3d(showmelong nodes,
                 showmereal *nodeptr,
                 showmereal xcenter,
                 showmereal ycenter,
                 showmereal zcenter,
                 showmereal xscale,
                 showmereal yscale,
                 showmereal xoffset,
                 showmereal yoffset)
{
  showmelong i;
  showmelong index;
  vector_t hpt;

  index = 3;
  for (i = 1; i <= nodes; i++) {
    hpt[0] = nodeptr[index++] - xcenter;
    hpt[1] = nodeptr[index++] - ycenter;
    hpt[2] = nodeptr[index++] - zcenter;
    mult_matvec(viewmatrix, hpt, hpt);
/*    perspectiveproj(hpt, &pt1); */
    XFillRectangle(display, mainwindow, linegc,
                   (int) (hpt[0] * xscale + xoffset - (line_width >> 1)),
                   (int) (hpt[1] * yscale + yoffset - (line_width >> 1)),
                   line_width, line_width);
  }
}

void draw_poly2d(showmelong nodes,
                 showmelong edges,
                 showmelong holes,
                 showmereal *nodeptr,
                 showmelong *edgeptr,
                 showmereal *holeptr,
                 showmereal xcenter,
                 showmereal ycenter,
                 showmereal xscale,
                 showmereal yscale,
                 showmereal xoffset,
                 showmereal yoffset)
{
  showmereal *point1, *point2;
  int x1, y1, x2, y2;
  showmelong i;
  showmelong index;

  index = 2;
  for (i = 1; i <= nodes; i++) {
    XFillRectangle(display, mainwindow, linegc,
                   (int) ((nodeptr[index] - xcenter) * xscale + xoffset -
                          (line_width >> 1)),
                   (int) ((nodeptr[index + 1] - ycenter) * yscale + yoffset -
                          (line_width >> 1)), line_width, line_width);
    index += 2;
  }
  index = 2;
  for (i = 1; i <= edges; i++) {
    point1 = &nodeptr[edgeptr[index++] * 2];
    point2 = &nodeptr[edgeptr[index++] * 2];
    XDrawLine(display, mainwindow, linegc,
              (int) ((point1[0] - xcenter) * xscale + xoffset),
              (int) ((point1[1] - ycenter) * yscale + yoffset),
              (int) ((point2[0] - xcenter) * xscale + xoffset),
              (int) ((point2[1] - ycenter) * yscale + yoffset));
  }
  index = 2;
  if (colordisplay) {
    XSetForeground(display, linegc, colors[0]);
  }
  for (i = 1; i <= holes; i++) {
    x1 = (int) ((holeptr[index] - xcenter) * xscale + xoffset) - 3;
    y1 = (int) ((holeptr[index + 1] - ycenter) * yscale + yoffset) - 3;
    x2 = x1 + 6;
    y2 = y1 + 6;
    XDrawLine(display, mainwindow, linegc, x1, y1, x2, y2);
    XDrawLine(display, mainwindow, linegc, x1, y2, x2, y1);
    index += 2;
  }
  XSetForeground(display, linegc, showme_foreground);
}

unsigned long randomnation(unsigned int choices)
{
  randomseed = (randomseed * 1366l + 150889l) % 714025l;
  return randomseed / (714025l / choices + 1);
}

/*
 *  Put the depth together with an index so the pair can be sorted
 *  according to depth
 */
struct facelist {
  showmereal z;
  int whichtet;
  int whichface;
  int color;
};

void sortfaces(struct facelist *drawfaces,
               int faces)
{
  struct facelist temp;
  showmereal pivotz;
  int pivot;
  int left, right;

  pivot = (int) randomnation((unsigned int) faces);
  pivotz = drawfaces[pivot].z;
  left = -1;
  right = faces;
  while (left < right) {
    do {
      left++;
    } while (drawfaces[left].z < pivotz);
    do {
      right--;
    } while (drawfaces[right].z > pivotz);
    if (left < right) {
      temp = drawfaces[left];
      drawfaces[left] = drawfaces[right];
      drawfaces[right] = temp;
    }
  }
  if (left > 1) {
    sortfaces(drawfaces, left);
  }
  if (right < faces - 2) {
    sortfaces(&drawfaces[right + 1], faces - right - 1);
  }
}

void draw_ele2d(showmelong elems,
                showmereal *nodeptr,
                showmelong *eleptr,
                int *partition,
                showmereal *shift,
                showmereal xcenter,
                showmereal ycenter,
                showmereal xscale,
                showmereal yscale,
                showmereal xoffset,
                showmereal yoffset)
{
  showmelong i;
  int j, k;
  int index;
  showmereal xshift, yshift;
  showmereal *nowpoint;
  XPoint vertices[3];

  index = 3;
  for (i = 1; i <= elems; i++) {
    if ((partition != (int *) NULL) && explode) {
      xshift = shift[partition[i] << 1];
      yshift = shift[(partition[i] << 1) + 1];
      for (j = 0; j < 3; j++) {
        nowpoint = &nodeptr[eleptr[index++] * 2];
        vertices[j].x = (short) ((nowpoint[0] - xcenter + xshift) * xscale
                                 + xoffset);
        vertices[j].y = (short) ((nowpoint[1] - ycenter + yshift) * yscale
                                 + yoffset);
      }
    } else {
      for (j = 0; j < 3; j++) {
        nowpoint = &nodeptr[eleptr[index++] * 2];
        vertices[j].x = (short) ((nowpoint[0] - xcenter) * xscale
                                 + xoffset);
        vertices[j].y = (short) ((nowpoint[1] - ycenter) * yscale
                                 + yoffset);
      }
    }
    if (colordisplay && (partition != (int *) NULL)) {
      if (fillelem) {
        XSetForeground(display, trianglegc,
                       colors[bitreverse[partition[i] & 63]]);
        XFillPolygon(display, mainwindow, trianglegc, vertices, 3,
                     Convex, CoordModeOrigin);
      } else {
        XSetForeground(display, linegc, colors[bitreverse[partition[i] & 63]]);
      }
    }
    k = 2;
    for (j = 0; j < 3; j++) {
      XDrawLine(display, mainwindow, linegc,
                vertices[j].x, vertices[j].y, vertices[k].x, vertices[k].y);
      k = j;
    }
  }
  XSetForeground(display, linegc, showme_foreground);
}

void draw_ele3d(showmelong elems,
                showmereal *nodeptr,
                showmelong *eleptr,
                int *partition,
                showmereal *shift,
                showmereal *dataptr,
                showmereal *datahist,
                showmereal xcenter,
                showmereal ycenter,
                showmereal zcenter,
                showmereal xscale,
                showmereal yscale,
                showmereal xoffset,
                showmereal yoffset)
{
  showmelong i;
  int j, k, l, m;
  int index;
  int faces;
  int tet, face;
  int shade;
  showmereal normx, normy, normz, norm;
  showmereal xshift, yshift, zshift;
  showmereal normalizer;
  showmereal kdatum, ldatum, mdatum;
  showmereal datum0, datum1, datum2, datum3;
  /*
  showmereal lengthkl, lengthkm, lengthlm;
  */
  showmereal *nowpoint;
  XPoint vertices[4];
  struct vec_t corner[4];
  vector_t hpt;
  struct facelist *drawfaces;
  XPoint *store_vertices[4];

  if (!wireframe) {
    drawfaces = (struct facelist *)
                malloc((elems + 1) * 4 * sizeof(struct facelist));
    if (drawfaces == (struct facelist *) NULL) {
      printf("  Insufficient memory to draw object.\n");
      return;
    }
    store_vertices[0] = (XPoint *) malloc((elems + 1) * sizeof(XPoint));
    store_vertices[1] = (XPoint *) malloc((elems + 1) * sizeof(XPoint));
    store_vertices[2] = (XPoint *) malloc((elems + 1) * sizeof(XPoint));
    store_vertices[3] = (XPoint *) malloc((elems + 1) * sizeof(XPoint));
  }

  if (colordisplay && (dataptr != (showmereal *) NULL)) {
    normalizer = 0.999 * (showmereal) MAXCOLORS / (datahist[1] - datahist[0]);
  }

  faces = 0;
  if (wireframe) {
    i = elems;
    while (tetraptr[i].invisible && (i > 0)) {
      i--;
    }
  } else {
    i = tetraptr[0].nexttet;
  }
  while (i != 0) {
    if ((partition != (int *) NULL) && explode) {
      xshift = shift[partition[i] * 3];
      yshift = shift[partition[i] * 3 + 1];
      zshift = shift[partition[i] * 3 + 2];
    }

    index = 4 * i;
    for (j = 0; j < 4; j++) {
      nowpoint = &nodeptr[eleptr[index++] * 3];
      hpt[0] = nowpoint[0] - xcenter;
      hpt[1] = nowpoint[1] - ycenter;
      hpt[2] = nowpoint[2] - zcenter;
      if ((partition != (int *) NULL) && explode) {
        hpt[0] += xshift;
        hpt[1] += yshift;
        hpt[2] += zshift;
      }
      mult_matvec(viewmatrix, hpt, hpt);
/*      perspectiveproj(hpt, &pt1); */

      if (!wireframe) {
        corner[j].x = hpt[0];
        corner[j].y = hpt[1];
        corner[j].z = hpt[2];
        store_vertices[j][i].x = (short) (hpt[0] * xscale + xoffset);
        store_vertices[j][i].y = (short) (hpt[1] * yscale + yoffset);
      } else {
        vertices[j].x = (short) (hpt[0] * xscale + xoffset);
        vertices[j].y = (short) (hpt[1] * yscale + yoffset);
      }
    }

    if (wireframe) {
      if (colordisplay && !fillelem) {
        if (partition != (int *) NULL) {
          XSetForeground(display, linegc,
                         colors[bitreverse[partition[i] & 63]]);
        } else if (dataptr != (showmereal *) NULL) {
          datum0 = dataptr[eleptr[i * 4]];
          datum1 = dataptr[eleptr[i * 4 + 1]];
          datum2 = dataptr[eleptr[i * 4 + 2]];
          datum3 = dataptr[eleptr[i * 4 + 3]];
          XSetForeground(display, linegc,
                         colors[(int) ((0.5 * (datum0 + datum1) - datahist[0])
                                       * normalizer)]);
        }
      }
      XDrawLine(display, mainwindow, linegc, vertices[0].x, vertices[0].y,
                vertices[1].x, vertices[1].y);
      if (colordisplay && !fillelem && (dataptr != (showmereal *) NULL)) {
        XSetForeground(display, linegc,
                       colors[(int) ((0.5 * (datum1 + datum2) - datahist[0])
                                     * normalizer)]);
      }
      XDrawLine(display, mainwindow, linegc, vertices[1].x, vertices[1].y,
                vertices[2].x, vertices[2].y);
      if (colordisplay && !fillelem && (dataptr != (showmereal *) NULL)) {
        XSetForeground(display, linegc,
                       colors[(int) ((0.5 * (datum2 + datum0) - datahist[0])
                                     * normalizer)]);
      }
      XDrawLine(display, mainwindow, linegc, vertices[2].x, vertices[2].y,
                vertices[0].x, vertices[0].y);
      if (colordisplay && !fillelem && (dataptr != (showmereal *) NULL)) {
        XSetForeground(display, linegc,
                       colors[(int) ((0.5 * (datum0 + datum3) - datahist[0])
                                     * normalizer)]);
      }
      XDrawLine(display, mainwindow, linegc, vertices[0].x, vertices[0].y,
                vertices[3].x, vertices[3].y);
      if (colordisplay && !fillelem && (dataptr != (showmereal *) NULL)) {
        XSetForeground(display, linegc,
                       colors[(int) ((0.5 * (datum1 + datum3) - datahist[0])
                                     * normalizer)]);
      }
      XDrawLine(display, mainwindow, linegc, vertices[1].x, vertices[1].y,
                vertices[3].x, vertices[3].y);
      if (colordisplay && !fillelem && (dataptr != (showmereal *) NULL)) {
        XSetForeground(display, linegc,
                       colors[(int) ((0.5 * (datum2 + datum3) - datahist[0])
                                     * normalizer)]);
      }
      XDrawLine(display, mainwindow, linegc, vertices[2].x, vertices[2].y,
                vertices[3].x, vertices[3].y);
      do {
        i--;
      } while (tetraptr[i].invisible && (i > 0));
    } else {
      for (face = 0; face < 4; face++) {
        if (tetraptr[i].neighbor[face] <= 0) {
          k = (face + 1) & 3;
          l = (face + 2) & 3;
          m = (face + 3) & 3;
          normz = (corner[l].x - corner[k].x) * (corner[m].y - corner[k].y) -
                  (corner[l].y - corner[k].y) * (corner[m].x - corner[k].x);
          if ((face == 0) || (face == 2)) {
            normz = - normz;
          }
          if (normz > 0.0) {
            /*
            lengthkl = sqrt((corner[k].x - corner[l].x) *
                            (corner[k].x - corner[l].x) +
                            (corner[k].y - corner[l].y) *
                            (corner[k].y - corner[l].y) +
                            (corner[k].z - corner[l].z) *
                            (corner[k].z - corner[l].z));
            lengthkm = sqrt((corner[k].x - corner[m].x) *
                            (corner[k].x - corner[m].x) +
                            (corner[k].y - corner[m].y) *
                            (corner[k].y - corner[m].y) +
                            (corner[k].z - corner[m].z) *
                            (corner[k].z - corner[m].z));
            lengthlm = sqrt((corner[l].x - corner[m].x) *
                            (corner[l].x - corner[m].x) +
                            (corner[l].y - corner[m].y) *
                            (corner[l].y - corner[m].y) +
                            (corner[l].z - corner[m].z) *
                            (corner[l].z - corner[m].z));
            drawfaces[faces].z = (lengthlm * corner[k].z +
                                  lengthkm * corner[l].z +
                                  lengthkl * corner[m].z) /
                                 (lengthlm + lengthkm +lengthkl);
            */
            drawfaces[faces].z = corner[k].z + corner[l].z + corner[m].z;
            drawfaces[faces].whichtet = i;
            drawfaces[faces].whichface = face;
            normx = (corner[l].y - corner[k].y) * (corner[m].z - corner[k].z) -
                    (corner[l].z - corner[k].z) * (corner[m].y - corner[k].y);
            normy = (corner[l].z - corner[k].z) * (corner[m].x - corner[k].x) -
                    (corner[l].x - corner[k].x) * (corner[m].z - corner[k].z);
            if ((face == 0) || (face == 2)) {
              normx = - normx;
              normy = - normy;
            }
            norm = normx * normx + normy * normy + normz * normz;
            if (norm == 0.0) {
              normx = 0.0;
              normy = 0.0;
              normz = 1.0;
              norm = 1.0;
            } else {
              norm = 1.0 / sqrt(norm);
            }
            shade = (int) ((normx * lightsourcex +
                            normy * lightsourcey +
                            normz * lightsourcez) *
                           norm * (showmereal) MAXGRAYS);
            if (shade < 0) {
              shade = 0;
            } else if (shade >= MAXGRAYS) {
              /* This should never happen. */
              shade = MAXGRAYS - 1;
            }
            drawfaces[faces].color = shade;
            faces++;
          }
        }
      }
      i = tetraptr[i].nexttet;
    }
  }

  if (!wireframe) {
    if (faces > 1) {
      sortfaces(drawfaces, faces);
    }
    for (i = 0; i < faces; i++) {
      tet = drawfaces[i].whichtet;
      face = drawfaces[i].whichface;
      k = (face + 1) & 3;
      l = (face + 2) & 3;
      m = (face + 3) & 3;

      vertices[0].x = store_vertices[k][tet].x;
      vertices[0].y = store_vertices[k][tet].y;
      vertices[1].x = store_vertices[l][tet].x;
      vertices[1].y = store_vertices[l][tet].y;
      vertices[2].x = store_vertices[m][tet].x;
      vertices[2].y = store_vertices[m][tet].y;

      if (colordisplay) {
        if (partition != (int *) NULL) {
          if (fillelem) {
            XSetForeground(display, trianglegc,
                           colors[bitreverse[partition[tet] & 63]]);
          } else {
            XSetForeground(display, linegc,
                           colors[bitreverse[partition[tet] & 63]]);
            XSetForeground(display, trianglegc, grays[drawfaces[i].color]);
          }
        } else if (dataptr != (showmereal *) NULL) {
          kdatum = dataptr[eleptr[tet * 4 + k]];
          ldatum = dataptr[eleptr[tet * 4 + l]];
          mdatum = dataptr[eleptr[tet * 4 + m]];
          if (fillelem) {
            XSetForeground(display, trianglegc,
                           colors[(int) ((ONETHIRD * (kdatum + ldatum + mdatum)
                                          - datahist[0]) * normalizer)]);
            XSetForeground(display, linegc, showme_foreground);
          } else {
            XSetForeground(display, trianglegc, grays[drawfaces[i].color]);
            XSetForeground(display, linegc,
                           colors[(int) ((0.5 * (kdatum + ldatum)
                                          - datahist[0]) * normalizer)]);
          }
        } else {
          XSetForeground(display, trianglegc, grays[drawfaces[i].color]);
        }
      }

      XFillPolygon(display, mainwindow, trianglegc, vertices, 3,
                   Convex, CoordModeOrigin);
      XDrawLine(display, mainwindow, linegc,
                vertices[0].x, vertices[0].y, vertices[1].x, vertices[1].y);
      if (colordisplay && !fillelem && (dataptr != (showmereal *) NULL)) {
        XSetForeground(display, linegc,
                       colors[(int) ((0.5 * (ldatum + mdatum) - datahist[0])
                                     * normalizer)]);
      }
      XDrawLine(display, mainwindow, linegc,
                vertices[1].x, vertices[1].y, vertices[2].x, vertices[2].y);
      if (colordisplay && !fillelem && (dataptr != (showmereal *) NULL)) {
        XSetForeground(display, linegc,
                       colors[(int) ((0.5 * (mdatum + kdatum) - datahist[0])
                                     * normalizer)]);
      }
      XDrawLine(display, mainwindow, linegc,
                vertices[2].x, vertices[2].y, vertices[0].x, vertices[0].y);
      if (colordisplay && (dataptr != (showmereal *) NULL)) {
        XSetForeground(display, linegc,
                       colors[(int) ((kdatum - datahist[0]) * normalizer)]);
        XFillArc(display, mainwindow, linegc,
                 vertices[0].x - 2 - (int) (line_width >> 1),
                 vertices[0].y - 2 - (int) (line_width >> 1),
                 line_width + 4, line_width + 4, 0, 23040);
        XSetForeground(display, linegc,
                       colors[(int) ((ldatum - datahist[0]) * normalizer)]);
        XFillArc(display, mainwindow, linegc,
                 vertices[1].x - 2 - (int) (line_width >> 1),
                 vertices[1].y - 2 - (int) (line_width >> 1),
                 line_width + 4, line_width + 4, 0, 23040);
        XSetForeground(display, linegc,
                       colors[(int) ((mdatum - datahist[0]) * normalizer)]);
        XFillArc(display, mainwindow, linegc,
                 vertices[2].x - 2 - (int) (line_width >> 1),
                 vertices[2].y - 2 - (int) (line_width >> 1),
                 line_width + 4, line_width + 4, 0, 23040);
      }
    }
    free(drawfaces);
    for (i = 0; i < 4; i++) {
      free(store_vertices[i]);
    }
  }
  XSetForeground(display, linegc, showme_foreground);
}

void draw_edge2d(showmelong nodes,
                 showmelong edges,
                 showmereal *nodeptr,
                 showmelong *edgeptr,
                 showmereal *normptr,
                 showmereal xcenter,
                 showmereal ycenter,
                 showmereal xscale,
                 showmereal yscale,
                 showmereal xoffset,
                 showmereal yoffset)
{
  showmelong i;
  showmelong index;
  showmereal *point1, *point2;
  showmereal normx, normy;
  showmereal normmult, normmultx, normmulty;
  showmereal windowxmin, windowymin, windowxmax, windowymax;

  index = 2;
  for (i = 1; i <= edges; i++) {
    point1 = &nodeptr[edgeptr[index++] * 2];
    if (edgeptr[index] == -1) {
      normx = normptr[index - 1];
      normy = normptr[index++];
      normmultx = 0.0;
      if (normx > 0) {
        windowxmax = xcenter + (0.5 * (showmereal) width / xscale);
        normmultx = (windowxmax - point1[0]) / normx;
      } else if (normx < 0) {
        windowxmin = xcenter - (0.5 * (showmereal) width / xscale);
        normmultx = (windowxmin - point1[0]) / normx;
      }
      normmulty = 0.0;
      if (normy > 0) {
        windowymax = ycenter - (0.5 * (showmereal) height / yscale);
        normmulty = (windowymax - point1[1]) / normy;
      } else if (normy < 0) {
        windowymin = ycenter + (0.5 * (showmereal) height / yscale);
        normmulty = (windowymin - point1[1]) / normy;
      }
      if (normmultx == 0.0) {
        normmult = normmulty;
      } else if (normmulty == 0.0) {
        normmult = normmultx;
      } else if (normmultx < normmulty) {
        normmult = normmultx;
      } else {
        normmult = normmulty;
      }
      if (normmult > 0.0) {
        XDrawLine(display, mainwindow, linegc,
                  (int) ((point1[0] - xcenter) * xscale + xoffset),
                  (int) ((point1[1] - ycenter) * yscale + yoffset),
                  (int) ((point1[0] - xcenter + normmult * normx)
                         * xscale + xoffset),
                  (int) ((point1[1] - ycenter + normmult * normy)
                         * yscale + yoffset));
      }
    } else {
      point2 = &nodeptr[edgeptr[index++] * 2];
      XDrawLine(display, mainwindow, linegc,
                (int) ((point1[0] - xcenter) * xscale + xoffset),
                (int) ((point1[1] - ycenter) * yscale + yoffset),
                (int) ((point2[0] - xcenter) * xscale + xoffset),
                (int) ((point2[1] - ycenter) * yscale + yoffset));
    }
  }
}

void draw_edge3d(showmelong nodes,
                 showmelong edges,
                 showmereal *nodeptr,
                 showmelong *edgeptr,
                 showmereal xcenter,
                 showmereal ycenter,
                 showmereal zcenter,
                 showmereal xscale,
                 showmereal yscale,
                 showmereal xoffset,
                 showmereal yoffset)
{
  showmelong i;
  showmelong index;
  showmereal *point1, *point2;
  vector_t hpt1, hpt2;

  index = 2;
  for (i = 1; i <= edges; i++) {
    point1 = &nodeptr[edgeptr[index++] * 3];
    hpt1[0] = point1[0] - xcenter;
    hpt1[1] = point1[1] - ycenter;
    hpt1[2] = point1[2] - zcenter;
    mult_matvec(viewmatrix, hpt1, hpt1);
/*    perspectiveproj(hpt, &pt1); */
    point2 = &nodeptr[edgeptr[index++] * 3];
    hpt2[0] = point2[0] - xcenter;
    hpt2[1] = point2[1] - ycenter;
    hpt2[2] = point2[2] - zcenter;
    mult_matvec(viewmatrix, hpt2, hpt2);
/*    perspectiveproj(hpt, &pt2); */
    XDrawLine(display, mainwindow, linegc,
              (int) (hpt1[0] * xscale + xoffset),
              (int) (hpt1[1] * yscale + yoffset),
              (int) (hpt2[0] * xscale + xoffset),
              (int) (hpt2[1] * yscale + yoffset));
  }
}

void draw_adj2d(int subdomains,
                int *adjptr,
                showmereal *center,
                showmereal xcenter,
                showmereal ycenter,
                showmereal xscale,
                showmereal yscale,
                showmereal xoffset,
                showmereal yoffset)
{
  int i, j;
  showmereal *point1, *point2;

  for (i = 0; i < subdomains; i++) {
    for (j = i + 1; j < subdomains; j++) {
      if (adjptr[i * subdomains + j]) {
        point1 = &center[i * 2];
        point2 = &center[j * 2];
        XDrawLine(display, mainwindow, linegc,
                  (int) ((point1[0] - xcenter) * xscale + xoffset),
                  (int) ((point1[1] - ycenter) * yscale + yoffset),
                  (int) ((point2[0] - xcenter) * xscale + xoffset),
                  (int) ((point2[1] - ycenter) * yscale + yoffset));
      }
    }
  }
  for (i = 0; i < subdomains; i++) {
    point1 = &center[i * 2];
    if (colordisplay) {
      XSetForeground(display, linegc, colors[bitreverse[i & 63]]);
    }
    XFillArc(display, mainwindow, linegc,
             (int) ((point1[0] - xcenter) * xscale + xoffset - 5 -
                    (line_width >> 1)),
             (int) ((point1[1] - ycenter) * yscale + yoffset - 5 -
                    (line_width >> 1)),
             line_width + 10, line_width + 10, 0, 23040);
  }
  XSetForeground(display, linegc, showme_foreground);
}

void draw_adj3d(int subdomains,
                int *adjptr,
                showmereal *center,
                showmereal xcenter,
                showmereal ycenter,
                showmereal zcenter,
                showmereal xscale,
                showmereal yscale,
                showmereal xoffset,
                showmereal yoffset)
{
  int i, j;
  showmereal *point1, *point2;
  vector_t hpt1, hpt2;

  for (i = 0; i < subdomains; i++) {
    for (j = i + 1; j < subdomains; j++) {
      if (adjptr[i * subdomains + j]) {
        point1 = &center[i * 3];
        hpt1[0] = point1[0] - xcenter;
        hpt1[1] = point1[1] - ycenter;
        hpt1[2] = point1[2] - zcenter;
        mult_matvec(viewmatrix, hpt1, hpt1);
/*        perspectiveproj(hpt1, &pt1); */
        point2 = &center[j * 3];
        hpt2[0] = point2[0] - xcenter;
        hpt2[1] = point2[1] - ycenter;
        hpt2[2] = point2[2] - zcenter;
        mult_matvec(viewmatrix, hpt2, hpt2);
/*        perspectiveproj(hpt2, &pt2); */
        XDrawLine(display, mainwindow, linegc,
                  (int) (hpt1[0] * xscale + xoffset),
                  (int) (hpt1[1] * yscale + yoffset),
                  (int) (hpt2[0] * xscale + xoffset),
                  (int) (hpt2[1] * yscale + yoffset));
      }
    }
  }
  for (i = 0; i < subdomains; i++) {
    point1 = &center[i * 3];
    if (colordisplay) {
      XSetForeground(display, linegc, colors[bitreverse[i & 63]]);
    }
    hpt1[0] = point1[0] - xcenter;
    hpt1[1] = point1[1] - ycenter;
    hpt1[2] = point1[2] - zcenter;
    mult_matvec(viewmatrix, hpt1, hpt1);
/*    perspectiveproj(hpt1, &pt1); */
    XFillArc(display, mainwindow, linegc,
             (int) (hpt1[0] * xscale + xoffset - 5 - (line_width >> 1)),
             (int) (hpt1[1] * yscale + yoffset - 5 - (line_width >> 1)),
             line_width + 10, line_width + 10, 0, 23040);
  }
  XSetForeground(display, linegc, showme_foreground);
}

void draw_data2d(showmelong nodes,
                 showmelong elems,
                 showmelong datavalues,
                 showmereal *nodeptr,
                 showmelong *eleptr,
                 showmereal *dataptr,
                 showmereal *datahist,
                 showmereal xcenter,
                 showmereal ycenter,
                 showmereal xscale,
                 showmereal yscale,
                 showmereal xoffset,
                 showmereal yoffset)
{
  showmereal normalizer;
  showmereal datum1, datum2, datum3;
  int shade;
  showmelong i;
  int index;

  showmereal datafac = 20.0 / (datahist[1] - datahist[0]);
  showmereal dataoffset = 0.5 * (datahist[0] + datahist[1]) * datafac;

  showmereal *point1, *point2, *point3;
  vector_t hpt1, hpt2, hpt3;
  showmereal normx, normy, normz, norm;

  struct facelist *drawfaces = (struct facelist *)
                               malloc(elems * sizeof(struct facelist));
  XPoint *vertices = (XPoint *) malloc(sizeof(XPoint) * (elems + 1) * 3);
  XPoint *final;

  if (colordisplay) {
    normalizer = 0.999 * (showmereal) MAXCOLORS / (datahist[1] - datahist[0]);
  }

  index = 3;
  for (i = 1; i <= elems; i++) {
    point1 = &nodeptr[eleptr[index++] * 2];
    point2 = &nodeptr[eleptr[index++] * 2];
    point3 = &nodeptr[eleptr[index++] * 2];

    hpt1[0] = point1[0] - xcenter;
    hpt1[1] = point1[1] - ycenter;
    hpt1[2] = dataptr[eleptr[3 * i]] * datafac - dataoffset;
    mult_matvec(viewmatrix, hpt1, hpt1);
/*    perspectiveproj(hpt1, &pt1); */
    hpt2[0] = point2[0] - xcenter;
    hpt2[1] = point2[1] - ycenter;
    hpt2[2] = dataptr[eleptr[3 * i + 1]] * datafac - dataoffset;
    mult_matvec(viewmatrix, hpt2, hpt2);
/*    perspectiveproj(hpt2, &pt2); */
    hpt3[0] = point3[0] - xcenter;
    hpt3[1] = point3[1] - ycenter;
    hpt3[2] = dataptr[eleptr[3 * i + 2]] * datafac - dataoffset;
    mult_matvec(viewmatrix, hpt3, hpt3);
/*    perspectiveproj(hpt3, &pt3); */

    if (wireframe) {
      if (colordisplay && !fillelem) {
        datum1 = dataptr[eleptr[i * 3]];
        datum2 = dataptr[eleptr[i * 3 + 1]];
        datum3 = dataptr[eleptr[i * 3 + 2]];
        XSetForeground(display, linegc,
                       colors[(int) ((0.5 * (datum1 + datum2) - datahist[0])
                                     * normalizer)]);
      }
      XDrawLine(display, mainwindow, linegc,
                (int) (hpt1[0] * xscale + xoffset),
                (int) (hpt1[1] * yscale + yoffset),
                (int) (hpt2[0] * xscale + xoffset),
                (int) (hpt2[1] * yscale + yoffset));
      if (colordisplay && !fillelem) {
        XSetForeground(display, linegc,
                       colors[(int) ((0.5 * (datum2 + datum3) - datahist[0])
                                     * normalizer)]);
      }
      XDrawLine(display, mainwindow, linegc,
                (int) (hpt2[0] * xscale + xoffset),
                (int) (hpt2[1] * yscale + yoffset),
                (int) (hpt3[0] * xscale + xoffset),
                (int) (hpt3[1] * yscale + yoffset));
      if (colordisplay && !fillelem) {
        XSetForeground(display, linegc,
                       colors[(int) ((0.5 * (datum3 + datum1) - datahist[0])
                                     * normalizer)]);
      }
      XDrawLine(display, mainwindow, linegc,
                (int) (hpt3[0] * xscale + xoffset),
                (int) (hpt3[1] * yscale + yoffset),
                (int) (hpt1[0] * xscale + xoffset),
                (int) (hpt1[1] * yscale + yoffset));
    } else {
      vertices[3 * i].x = (short) (hpt1[0] * xscale + xoffset);
      vertices[3 * i].y = (short) (hpt1[1] * yscale + yoffset);
      vertices[3 * i + 1].x = (short) (hpt2[0] * xscale + xoffset);
      vertices[3 * i + 1].y = (short) (hpt2[1] * yscale + yoffset);
      vertices[3 * i + 2].x = (short) (hpt3[0] * xscale + xoffset);
      vertices[3 * i + 2].y = (short) (hpt3[1] * yscale + yoffset);

      drawfaces[i - 1].whichtet = i;
      drawfaces[i - 1].z = hpt1[2] + hpt2[2] + hpt3[2];
      normx = (hpt2[1] - hpt1[1]) * (hpt3[2] - hpt1[2])
            - (hpt2[2] - hpt1[2]) * (hpt3[1] - hpt1[1]);
      normy = (hpt2[2] - hpt1[2]) * (hpt3[0] - hpt1[0])
            - (hpt2[0] - hpt1[0]) * (hpt3[2] - hpt1[2]);
      normz = (hpt2[0] - hpt1[0]) * (hpt3[1] - hpt1[1])
            - (hpt2[1] - hpt1[1]) * (hpt3[0] - hpt1[0]);
      norm = normx * normx + normy * normy + normz * normz;
      if (norm == 0.0) {
        normx = 0.0;
        normy = 0.0;
        normz = 1.0;
        norm = 1.0;
      } else {
        norm = 1.0 / sqrt(norm);
      }
      if (normz < 0.0) {
        normx = -normx;
        normy = -normy;
        normz = -normz;
      }
      shade = (int) ((normx * lightsourcex +
                      normy * lightsourcey +
                      normz * lightsourcez) * norm * (showmereal) MAXGRAYS);
      if (shade < 0) {
        shade = 0;
      } else if (shade >= MAXGRAYS) {
        /* This should never happen. */
        shade = MAXGRAYS - 1;
      }
      drawfaces[i - 1].color = shade;
    }
  }

  if (!wireframe) {
    if (elems > 1) {
      sortfaces(drawfaces, elems);
    }
    for (i = 1; i <= elems; i++) {
      final = &vertices[3 * drawfaces[i - 1].whichtet];

      if (colordisplay) {
        datum1 = dataptr[eleptr[drawfaces[i - 1].whichtet * 3]];
        datum2 = dataptr[eleptr[drawfaces[i - 1].whichtet * 3 + 1]];
        datum3 = dataptr[eleptr[drawfaces[i - 1].whichtet * 3 + 2]];
        if (fillelem) {
          XSetForeground(display, trianglegc,
                         colors[(int) ((ONETHIRD * (datum1 + datum2 + datum3)
                                        - datahist[0]) * normalizer)]);
          XSetForeground(display, linegc, showme_foreground);	
        } else {
          XSetForeground(display, trianglegc, grays[drawfaces[i - 1].color]);
          XSetForeground(display, linegc,
                         colors[(int) ((0.5 * (datum1 + datum2) - datahist[0])
                                       * normalizer)]);
        }
      }
      XFillPolygon(display, mainwindow, trianglegc, final, 3,
                   Convex, CoordModeOrigin);

      /* outline the solid polygons */
      XDrawLine(display, mainwindow, linegc,
                final[0].x, final[0].y, final[1].x, final[1].y);
      if (colordisplay && !fillelem) {
        XSetForeground(display, linegc,
                       colors[(int) ((0.5 * (datum2 + datum3) - datahist[0])
                                     * normalizer)]);
      }
      XDrawLine(display, mainwindow, linegc,
                final[1].x, final[1].y, final[2].x, final[2].y);
      if (colordisplay && !fillelem) {
        XSetForeground(display, linegc,
                       colors[(int) ((0.5 * (datum3 + datum1) - datahist[0])
                                     * normalizer)]);
      }
      XDrawLine(display, mainwindow, linegc,
                final[2].x, final[2].y, final[0].x, final[0].y);
      if (colordisplay) {
        XSetForeground(display, linegc,
                       colors[(int) ((datum1 - datahist[0]) * normalizer)]);
        XFillArc(display, mainwindow, linegc,
                 final[0].x - 2 - (int) (line_width >> 1),
                 final[0].y - 2 - (int) (line_width >> 1),
                 line_width + 4, line_width + 4, 0, 23040);
        XSetForeground(display, linegc,
                       colors[(int) ((datum2 - datahist[0]) * normalizer)]);
        XFillArc(display, mainwindow, linegc,
                 final[1].x - 2 - (int) (line_width >> 1),
                 final[1].y - 2 - (int) (line_width >> 1),
                 line_width + 4, line_width + 4, 0, 23040);
        XSetForeground(display, linegc,
                       colors[(int) ((datum3 - datahist[0]) * normalizer)]);
        XFillArc(display, mainwindow, linegc,
                 final[2].x - 2 - (int) (line_width >> 1),
                 final[2].y - 2 - (int) (line_width >> 1),
                 line_width + 4, line_width + 4, 0, 23040);
      }
    }
  }
  XSetForeground(display, linegc, showme_foreground);
}

void draw_image(int inc,
                int image,
                showmereal xcenter,
                showmereal ycenter,
                showmereal zcenter,
                showmereal xwidth,
                showmereal ywidth,
                showmereal xtrans,
                showmereal ytrans)
{
  XClearWindow(display, mainwindow);
  if (image == NOTHING) {
    return;
  }
  if (!loaded[inc][image]) {
    return;
  }
  if ((image == PART) && explode) {
    xwidth *= (1.0 + explosion);
    ywidth *= (1.0 + explosion);
  }
  xscale = (showmereal) (width - line_width - 4) / xwidth;
  yscale = (showmereal) (height - line_width - 4) / ywidth;
  if (xscale > yscale) {
    xscale = yscale;
  } else {
    yscale = xscale;
  }
  yscale = -yscale;

  xoffset = 0.5 * (showmereal) width + xtrans * xscale;
  yoffset = 0.5 * (showmereal) height + ytrans * yscale;
  perspdistance = xwidth * perspfactor;

  switch (image) {
    case NODE:
      if (nodedim[inc] == 2) {
        draw_node2d(nodes[inc], nodeptr[inc], xcenter, ycenter,
                    xscale, yscale, xoffset, yoffset);
      } else {
        draw_node3d(nodes[inc], nodeptr[inc], xcenter, ycenter, zcenter,
                    xscale, yscale, xoffset, yoffset);
      }
      break;
    case POLY:
      if (polynodes[inc] > 0) {
        if (polydim[inc] == 2) {
          draw_poly2d(polynodes[inc], polyedges[inc], polyholes[inc],
                      polynodeptr[inc], polysegptr[inc], polyholeptr[inc],
                      xcenter, ycenter, xscale, yscale, xoffset, yoffset);
        }
      } else {
        if (nodedim[inc] == 2) {
          draw_poly2d(nodes[inc], polyedges[inc], polyholes[inc],
                      nodeptr[inc], polysegptr[inc], polyholeptr[inc],
                      xcenter, ycenter, xscale, yscale, xoffset, yoffset);
        }
      }
      break;
    case ELE:
      if (nodedim[inc] == 2) {
        draw_ele2d(elems[inc], nodeptr[inc], eleptr[inc],
                   (int *) NULL, (showmereal *) NULL, xcenter, ycenter,
                   xscale, yscale, xoffset, yoffset);
      } else {
        draw_ele3d(elems[inc], nodeptr[inc], eleptr[inc],
                   (int *) NULL, (showmereal *) NULL, (showmereal *) NULL, (showmereal *) NULL,
                   xcenter, ycenter, zcenter, xscale, yscale,
                   xoffset, yoffset);
      }
      break;
    case EDGE:
      if (nodedim[inc] == 2) {
        draw_edge2d(nodes[inc], edges[inc], nodeptr[inc],
                    edgeptr[inc], normptr[inc], xcenter, ycenter,
                    xscale, yscale, xoffset, yoffset);
      } else {
        draw_edge3d(nodes[inc], edges[inc], nodeptr[inc],
                    edgeptr[inc], xcenter, ycenter, zcenter,
                    xscale, yscale, xoffset, yoffset);
      }
      break;
    case PART:
      if (nodedim[inc] == 2) {
        draw_ele2d(elems[inc], nodeptr[inc], eleptr[inc],
                   partition[inc], subdomshift[inc], xcenter, ycenter,
                   xscale, yscale, xoffset, yoffset);
      } else {
        draw_ele3d(elems[inc], nodeptr[inc], eleptr[inc],
                   partition[inc], subdomshift[inc],
                   (showmereal *) NULL, (showmereal *) NULL,
                   xcenter, ycenter, zcenter, xscale, yscale,
                   xoffset, yoffset);
      }
      break;
    case ADJ:
      if (nodedim[inc] == 2) {
        draw_adj2d(adjsubdomains[inc], adjptr[inc], subdomcenter[inc],
                   xcenter, ycenter, xscale, yscale, xoffset, yoffset);
      } else {
        draw_adj3d(adjsubdomains[inc], adjptr[inc], subdomcenter[inc],
                   xcenter, ycenter, zcenter, xscale, yscale,
                   xoffset, yoffset);
      }
      break;
    case VORO:
      if (vnodedim[inc] == 2) {
        if (loaded[inc][NODE]) {
          if (nodedim[inc] == 2) {
            draw_node2d(nodes[inc], nodeptr[inc], xcenter, ycenter,
                        xscale, yscale, xoffset, yoffset);
          }
        }
        draw_edge2d(vnodes[inc], vedges[inc], vnodeptr[inc],
                    vedgeptr[inc], vnormptr[inc],
                    xcenter, ycenter, xscale, yscale, xoffset, yoffset);
      }
      break;
    case DATA:
      if (nodedim[inc] == 2) {
        draw_data2d(nodes[inc], elems[inc], datavalues[inc],
                    nodeptr[inc], eleptr[inc], dataptr[inc], datahist[inc],
                    xcenter, ycenter, xscale, yscale, xoffset, yoffset);
      } else {
        draw_ele3d(elems[inc], nodeptr[inc], eleptr[inc],
                   (int *) NULL, (showmereal *) NULL,
                   dataptr[inc], datahist[inc],
                   xcenter, ycenter, zcenter, xscale, yscale,
                   xoffset, yoffset);
      }
      break;
    default:
      break;
  }
}

void addps(char *instring,
           char *outstring,
           int eps)
{
  strcpy(outstring, instring);
  if (eps) {
    strcat(outstring, ".eps");
  } else {
    strcat(outstring, ".ps");
  }
}

int print_head(char *fname,
               FILE **file,
               int llcornerx,
               int llcornery,
               int eps)
{
  if (!quiet) {
    printf("Writing %s\n", fname);
  }
  *file = fopen(fname, "w");
  if (*file == (FILE *) NULL) {
    printf("  Error:  Could not open %s\n", fname);
    return 1;
  }
  if (eps) {
    fprintf(*file, "%%!PS-Adobe-2.0 EPSF-2.0\n");
  } else {
    fprintf(*file, "%%!PS-Adobe-2.0\n");
  }
  fprintf(*file, "%%%%BoundingBox: %d %d %d %d\n", llcornerx, llcornery,
          612 - llcornerx, 792 - llcornery);
  fprintf(*file, "%%%%Creator: Show Me\n");
  fprintf(*file, "%%%%EndComments\n\n");
  fprintf(*file, "/m {moveto} bind def\n");
  fprintf(*file, "/l {lineto} bind def\n");
  fprintf(*file, "/s {setrgbcolor} bind def\n");
  fprintf(*file, "/g {gsave fill grestore} bind def\n");
  fprintf(*file, "/k {stroke} bind def\n\n");
  fprintf(*file, "1 setlinecap\n");
  fprintf(*file, "1 setlinejoin\n");
  fprintf(*file, "%f setlinewidth\n", 0.3 * (double) line_width);
  fprintf(*file, "%d %d m\n", llcornerx, llcornery);
  fprintf(*file, "%d %d l\n", 612 - llcornerx, llcornery);
  fprintf(*file, "%d %d l\n", 612 - llcornerx, 792 - llcornery);
  fprintf(*file, "%d %d l\n", llcornerx, 792 - llcornery);
  fprintf(*file, "closepath\nclip\nnewpath\n");
  return 0;
}

void print_node2d(FILE *nodefile,
                  showmelong nodes,
                  showmereal *nodeptr,
                  showmereal xcenter,
                  showmereal ycenter,
                  showmereal xscale,
                  showmereal yscale,
                  showmereal xoffset,
                  showmereal yoffset)
{
  showmelong i;
  showmelong index;

  index = 2;
  for (i = 1; i <= nodes; i++) {
    fprintf(nodefile, "%f %f %d 0 360 arc\nfill\n",
            (nodeptr[index] - xcenter) * xscale + xoffset,
            (nodeptr[index + 1] - ycenter) * yscale + yoffset,
            1 + (line_width >> 1));
    index += 2;
  }
}

void print_node3d(FILE *nodefile,
                  showmelong nodes,
                  showmereal *nodeptr,
                  showmereal xcenter,
                  showmereal ycenter,
                  showmereal zcenter,
                  showmereal xscale,
                  showmereal yscale,
                  showmereal xoffset,
                  showmereal yoffset)
{
  showmelong i;
  showmelong index;
  vector_t hpt;

  index = 3;
  for (i = 1; i <= nodes; i++) {
    hpt[0] = nodeptr[index++] - xcenter;
    hpt[1] = nodeptr[index++] - ycenter;
    hpt[2] = nodeptr[index++] - zcenter;
    mult_matvec(viewmatrix, hpt, hpt);
/*    perspectiveproj(hpt, &pt1); */
    fprintf(nodefile, "%f %f %d 0 360 arc\nfill\n",
            hpt[0] * xscale + xoffset, hpt[1] * yscale + yoffset,
            1 + (line_width >> 1));
  }
}

void print_poly2d(FILE *polyfile,
                  showmelong nodes,
                  showmelong edges,
                  showmereal *nodeptr,
                  showmelong *edgeptr,
                  showmereal xcenter,
                  showmereal ycenter,
                  showmereal xscale,
                  showmereal yscale,
                  showmereal xoffset,
                  showmereal yoffset)
{
  showmereal *point1, *point2;
  showmelong i;
  showmelong index;

  index = 2;
  for (i = 1; i <= nodes; i++) {
    fprintf(polyfile, "%f %f %d 0 360 arc\nfill\n",
            (nodeptr[index] - xcenter) * xscale + xoffset,
            (nodeptr[index + 1] - ycenter) * yscale + yoffset,
            1 + (line_width >> 1));
    index += 2;
  }
  index = 2;
  for (i = 1; i <= edges; i++) {
    point1 = &nodeptr[edgeptr[index++] * 2];
    point2 = &nodeptr[edgeptr[index++] * 2];
    fprintf(polyfile, "%f %f m\n",
            (point1[0] - xcenter) * xscale + xoffset,
            (point1[1] - ycenter) * yscale + yoffset);
    fprintf(polyfile, "%f %f l\nk\n",
            (point2[0] - xcenter) * xscale + xoffset,
            (point2[1] - ycenter) * yscale + yoffset);
  }
  index = 2;
}

void print_ele2d(FILE *elefile,
                 showmelong elems,
                 showmereal *nodeptr,
                 showmelong *eleptr,
                 int *partition,
                 showmereal *shift,
                 showmereal xcenter,
                 showmereal ycenter,
                 showmereal xscale,
                 showmereal yscale,
                 showmereal xoffset,
                 showmereal yoffset,
                 int llcornerx,
                 int llcornery)
{
  showmereal xshift, yshift;
  showmereal *nowpoint;
  int colorindex;
  int index;
  showmelong i;
  int j;

  if ((partition != (int *) NULL) && !bw_ps) {
    fprintf(elefile, "0 0 0 s\n");
    fprintf(elefile, "%d %d m\n", llcornerx, llcornery);
    fprintf(elefile, "%d %d l\n", 612 - llcornerx, llcornery);
    fprintf(elefile, "%d %d l\n", 612 - llcornerx, 792 - llcornery);
    fprintf(elefile, "%d %d l\n", llcornerx, 792 - llcornery);
    fprintf(elefile, "fill\n");
  }
  index = 3;
  for (i = 1; i <= elems; i++) {
    if ((partition != (int *) NULL) && !bw_ps) {
      colorindex = bitreverse[partition[i] & 63];
      fprintf(elefile, "%6.3f %6.3f %6.3f s\n",
              (showmereal) rgb[colorindex].red / 65535.0,
              (showmereal) rgb[colorindex].green / 65535.0,
              (showmereal) rgb[colorindex].blue / 65535.0);
    }
    nowpoint = &nodeptr[eleptr[index + 2] * 2];
    if ((partition != (int *) NULL) && (explode || bw_ps)) {
      xshift = shift[partition[i] << 1];
      yshift = shift[(partition[i] << 1) + 1];
      fprintf(elefile, "%f %f m\n",
              (nowpoint[0] - xcenter + xshift) * xscale + xoffset,
              (nowpoint[1] - ycenter + yshift) * yscale + yoffset);
      for (j = 0; j < 3; j++) {
        nowpoint = &nodeptr[eleptr[index++] * 2];
        fprintf(elefile, "%f %f l\n",
                (nowpoint[0] - xcenter + xshift) * xscale + xoffset,
                (nowpoint[1] - ycenter + yshift) * yscale + yoffset);
      }
    } else {
      fprintf(elefile, "%f %f m\n",
              (nowpoint[0] - xcenter) * xscale + xoffset,
              (nowpoint[1] - ycenter) * yscale + yoffset);
      for (j = 0; j < 3; j++) {
        nowpoint = &nodeptr[eleptr[index++] * 2];
        fprintf(elefile, "%f %f l\n",
                (nowpoint[0] - xcenter) * xscale + xoffset,
                (nowpoint[1] - ycenter) * yscale + yoffset);
      }
    }
    if (fillelem && !bw_ps && (partition != (int *) NULL)) {
      fprintf(elefile, "g\n1 1 0 s\n");
    }
    fprintf(elefile, "k\n");
  }
}

void print_ele3d(FILE *elefile,
                 showmelong elems,
                 showmereal *nodeptr,
                 showmelong *eleptr,
                 int *partition,
                 showmereal *shift,
                 showmereal xcenter,
                 showmereal ycenter,
                 showmereal zcenter,
                 showmereal xscale,
                 showmereal yscale,
                 showmereal xoffset,
                 showmereal yoffset,
                 int llcornerx,
                 int llcornery)
{
  showmelong i;
  int j, k, l, m;
  int index;
  int faces;
  int tet, face;
  int shade;
  int colorindex;
  showmereal normx, normy, normz, norm;
  showmereal xshift, yshift, zshift;
  showmereal *nowpoint;
  struct vec_t vertices[4];
  struct vec_t corner[4];
  vector_t hpt;
  struct facelist *drawfaces;

  struct vec_t *store_vertices[4];

  if (!wireframe) {
    drawfaces = (struct facelist *)
                malloc((elems + 1) * 4 * sizeof(struct facelist));
    if (drawfaces == (struct facelist *) NULL) {
      printf("  Insufficient memory to draw object.\n");
      return;
    }
    store_vertices[0] = (struct vec_t *) malloc((elems + 1)
                                                * sizeof(struct vec_t));
    store_vertices[1] = (struct vec_t *) malloc((elems + 1)
                                                * sizeof(struct vec_t));
    store_vertices[2] = (struct vec_t *) malloc((elems + 1)
                                                * sizeof(struct vec_t));
    store_vertices[3] = (struct vec_t *) malloc((elems + 1)
                                                * sizeof(struct vec_t));
  }

  faces = 0;
  if (wireframe) {
    i = elems;
    while (tetraptr[i].invisible && (i > 0)) {
      i--;
    }
  } else {
    i = tetraptr[0].nexttet;
  }
  while (i != 0) {
    if ((partition != (int *) NULL) && explode) {
      xshift = shift[partition[i] * 3];
      yshift = shift[partition[i] * 3 + 1];
      zshift = shift[partition[i] * 3 + 2];
    }

    index = 4 * i;
    for (j = 0; j < 4; j++) {
      nowpoint = &nodeptr[eleptr[index++] * 3];
      hpt[0] = nowpoint[0] - xcenter;
      hpt[1] = nowpoint[1] - ycenter;
      hpt[2] = nowpoint[2] - zcenter;
      if ((partition != (int *) NULL) && explode) {
        hpt[0] += xshift;
        hpt[1] += yshift;
        hpt[2] += zshift;
      }
      mult_matvec(viewmatrix, hpt, hpt);
/*      perspectiveproj(hpt, &pt1); */

      if (wireframe) {
        vertices[j].x = hpt[0] * xscale + xoffset;
        vertices[j].y = hpt[1] * yscale + yoffset;
      } else {
        corner[j].x = hpt[0];
        corner[j].y = hpt[1];
        corner[j].z = hpt[2];
        store_vertices[j][i].x = hpt[0] * xscale + xoffset;
        store_vertices[j][i].y = hpt[1] * yscale + yoffset;
      }
    }

    if (wireframe) {
      if (!fillelem && !bw_ps && (partition != (int *) NULL)) {
        colorindex = partition[i] & 63;
        fprintf(elefile, "%6.3f %6.3f %6.3f s\n",
                (showmereal) rgb[colorindex].red / 65535.0,
                (showmereal) rgb[colorindex].green / 65535.0,
                (showmereal) rgb[colorindex].blue / 65535.0);
      }
      fprintf(elefile, "%f %f m\n", vertices[0].x, vertices[0].y);
      fprintf(elefile, "%f %f l\n", vertices[1].x, vertices[1].y);
      fprintf(elefile, "%f %f l\n", vertices[2].x, vertices[2].y);
      fprintf(elefile, "%f %f l\n", vertices[0].x, vertices[0].y);
      fprintf(elefile, "%f %f l\n", vertices[3].x, vertices[3].y);
      fprintf(elefile, "%f %f l\nk\n", vertices[1].x, vertices[1].y);
      fprintf(elefile, "%f %f m\n", vertices[2].x, vertices[2].y);
      fprintf(elefile, "%f %f l\nk\n", vertices[3].x, vertices[3].y);
      do {
        i--;
      } while (tetraptr[i].invisible && (i > 0));
    } else {
      for (face = 0; face < 4; face++) {
        if (tetraptr[i].neighbor[face] <= 0) {
          k = (face + 1) & 3;
          l = (face + 2) & 3;
          m = (face + 3) & 3;
          normz = (corner[l].x - corner[k].x) * (corner[m].y - corner[k].y) -
                  (corner[l].y - corner[k].y) * (corner[m].x - corner[k].x);
          if ((face == 0) || (face == 2)) {
            normz = - normz;
          }
          if (normz > 0.0) {
            drawfaces[faces].z = corner[k].z + corner[l].z + corner[m].z;
            drawfaces[faces].whichtet = i;
            drawfaces[faces].whichface = face;
            normx = (corner[l].y - corner[k].y) * (corner[m].z - corner[k].z) -
                    (corner[l].z - corner[k].z) * (corner[m].y - corner[k].y);
            normy = (corner[l].z - corner[k].z) * (corner[m].x - corner[k].x) -
                    (corner[l].x - corner[k].x) * (corner[m].z - corner[k].z);
            if ((face == 0) || (face == 2)) {
              normx = - normx;
              normy = - normy;
            }
            norm = normx * normx + normy * normy + normz * normz;
            if (norm == 0.0) {
              normx = 0.0;
              normy = 0.0;
              normz = 1.0;
              norm = 1.0;
            } else {
              norm = 1.0 / sqrt(norm);
            }
            shade = 16384 + (int) ((normx * lightsourcex +
                                    normy * lightsourcey +
                                    normz * lightsourcez) * norm * 49152.0);
            if (shade < 16384) {
              shade = 16384;
            } else if (shade >= 65536) {
              /* This should never happen. */
              shade = 65535;
            }
            drawfaces[faces].color = shade;
            faces++;
          }
        }
      }
      i = tetraptr[i].nexttet;
    }
  }

  if (!wireframe) {
    if (faces > 1) {
      sortfaces(drawfaces, faces);
    }
    for (i = 0; i < faces; i++) {
      tet = drawfaces[i].whichtet;
      face = drawfaces[i].whichface;
      k = (face + 1) & 3;
      l = (face + 2) & 3;
      m = (face + 3) & 3;

      if (fillelem && !bw_ps && (partition != (int *) NULL)) {
        colorindex = partition[tet] & 63;
        fprintf(elefile, "%6.3f %6.3f %6.3f s\n",
                (showmereal) rgb[colorindex].red / 65535.0,
                (showmereal) rgb[colorindex].green / 65535.0,
                (showmereal) rgb[colorindex].blue / 65535.0);
      } else if (tetraptr[tet].neighbor[face] < 0) {
        fprintf(elefile, "%6.3f %6.3f %6.3f s\n",
                (showmereal) drawfaces[i].color / 65535.0,
                (showmereal) drawfaces[i].color / 65535.0,
                (showmereal) drawfaces[i].color / 65535.0 / 1.5);
        /*
        fprintf(elefile, "%6.3f %6.3f %6.3f s\n",
                1.0,
                1.0,
                (showmereal) drawfaces[i].color / 65535.0);
        */
      } else {
        fprintf(elefile, "%6.3f %6.3f %6.3f s\n",
                (showmereal) drawfaces[i].color / 65535.0 / 1.35,
                (showmereal) drawfaces[i].color / 65535.0,
                (showmereal) drawfaces[i].color / 65535.0 / 1.35);
        /*
        fprintf(elefile, "%6.3f %6.3f %6.3f s\n",
                (showmereal) drawfaces[i].color / 65535.0,
                1.0,
                (showmereal) drawfaces[i].color / 65535.0);
        */
      }
      fprintf(elefile, "%f %f m\n",
              store_vertices[k][tet].x, store_vertices[k][tet].y);
      fprintf(elefile, "%f %f l\n",
              store_vertices[l][tet].x, store_vertices[l][tet].y);
      fprintf(elefile, "%f %f l\n",
              store_vertices[m][tet].x, store_vertices[m][tet].y);
      fprintf(elefile, "%f %f l\n",
              store_vertices[k][tet].x, store_vertices[k][tet].y);
      fprintf(elefile, "g\n\n");
      if (bw_ps) {
        fprintf(elefile, "0 0 0 s\n");
      } else if (!fillelem && (partition != (int *) NULL)) {
        colorindex = partition[tet] & 63;
        fprintf(elefile, "%6.3f %6.3f %6.3f s\n",
                (showmereal) rgb[colorindex].red / 65535.0,
                (showmereal) rgb[colorindex].green / 65535.0,
                (showmereal) rgb[colorindex].blue / 65535.0);
      } else {
        /* Was light green
           fprintf(elefile, "0.3 1 0.3 s\n"); */
        fprintf(elefile, "0 0 0 s\n");
      }
      fprintf(elefile, "k\n");
    }
    free(drawfaces);
    for (i = 0; i < 4; i++) {
      free(store_vertices[i]);
    }
  }
}

void print_edge2d(FILE *edgefile,
                  showmelong nodes,
                  showmelong edges,
                  showmereal *nodeptr,
                  showmelong *edgeptr,
                  showmereal *normptr,
                  showmereal xcenter,
                  showmereal ycenter,
                  showmereal xscale,
                  showmereal yscale,
                  showmereal xoffset,
                  showmereal yoffset,
                  int llcornerx,
                  int llcornery)
{
  showmelong i;
  showmelong index;
  showmereal *point1, *point2;
  showmereal normx, normy;
  showmereal normmult, normmultx, normmulty;
  showmereal windowxmin, windowymin, windowxmax, windowymax;

  index = 2;
  for (i = 1; i <= edges; i++) {
    point1 = &nodeptr[edgeptr[index++] * 2];
    if (edgeptr[index] == -1) {
      normx = normptr[index - 1];
      normy = normptr[index++];
      normmultx = 0.0;
      if (normx > 0) {
        windowxmax = xcenter + ((showmereal) (306 - llcornerx) / xscale);
        normmultx = (windowxmax - point1[0]) / normx;
      } else if (normx < 0) {
        windowxmin = xcenter - ((showmereal) (306 - llcornerx) / xscale);
        normmultx = (windowxmin - point1[0]) / normx;
      }
      normmulty = 0.0;
      if (normy > 0) {
        windowymax = ycenter + ((showmereal) (396 - llcornery) / yscale);
        normmulty = (windowymax - point1[1]) / normy;
      } else if (normy < 0) {
        windowymin = ycenter - ((showmereal) (396 - llcornery) / yscale);
        normmulty = (windowymin - point1[1]) / normy;
      }
      if (normmultx == 0.0) {
        normmult = normmulty;
      } else if (normmulty == 0.0) {
        normmult = normmultx;
      } else if (normmultx < normmulty) {
        normmult = normmultx;
      } else {
        normmult = normmulty;
      }
      if (normmult > 0.0) {
        fprintf(edgefile, "%f %f m\n",
                (point1[0] - xcenter) * xscale + xoffset,
                (point1[1] - ycenter) * yscale + yoffset);
        fprintf(edgefile, "%f %f l\nk\n",
                (point1[0] - xcenter + normmult * normx) * xscale + xoffset,
                (point1[1] - ycenter + normmult * normy) * yscale + yoffset);
      }
    } else {
      point2 = &nodeptr[edgeptr[index++] * 2];
      fprintf(edgefile, "%f %f m\n",
              (point1[0] - xcenter) * xscale + xoffset,
              (point1[1] - ycenter) * yscale + yoffset);
      fprintf(edgefile, "%f %f l\nk\n",
              (point2[0] - xcenter) * xscale + xoffset,
              (point2[1] - ycenter) * yscale + yoffset);
    }
  }
}

void print_edge3d(FILE *edgefile,
                  showmelong nodes,
                  showmelong edges,
                  showmereal *nodeptr,
                  showmelong *edgeptr,
                  showmereal xcenter,
                  showmereal ycenter,
                  showmereal zcenter,
                  showmereal xscale,
                  showmereal yscale,
                  showmereal xoffset,
                  showmereal yoffset,
                  int llcornerx,
                  int llcornery)
{
  showmelong i;
  showmelong index;
  showmereal *point1, *point2;
  vector_t hpt1, hpt2;

  index = 2;
  for (i = 1; i <= edges; i++) {
    point1 = &nodeptr[edgeptr[index++] * 3];
    hpt1[0] = point1[0] - xcenter;
    hpt1[1] = point1[1] - ycenter;
    hpt1[2] = point1[2] - zcenter;
    mult_matvec(viewmatrix, hpt1, hpt1);
/*    perspectiveproj(hpt1, &pt1); */
    point2 = &nodeptr[edgeptr[index++] * 3];
    hpt2[0] = point2[0] - xcenter;
    hpt2[1] = point2[1] - ycenter;
    hpt2[2] = point2[2] - zcenter;
    mult_matvec(viewmatrix, hpt2, hpt2);
/*     perspectiveproj(hpt2, &pt2); */
    fprintf(edgefile, "%f %f m\n",
            hpt1[0] * xscale + xoffset, hpt1[1] * yscale + yoffset);
    fprintf(edgefile, "%f %f l\nk\n",
            hpt2[0] * xscale + xoffset, hpt2[1] * yscale + yoffset);
  }
}

void print_adj2d(FILE *adjfile,
                 int subdomains,
                 int *adjptr,
                 showmereal *center,
                 showmereal xcenter,
                 showmereal ycenter,
                 showmereal xscale,
                 showmereal yscale,
                 showmereal xoffset,
                 showmereal yoffset,
                 int llcornerx,
                 int llcornery)
{
  int i, j;
  showmereal *point1, *point2;
  int colorindex;

  if (!bw_ps) {
    fprintf(adjfile, "0 0 0 s\n");
    fprintf(adjfile, "%d %d m\n", llcornerx, llcornery);
    fprintf(adjfile, "%d %d l\n", 612 - llcornerx, llcornery);
    fprintf(adjfile, "%d %d l\n", 612 - llcornerx, 792 - llcornery);
    fprintf(adjfile, "%d %d l\n", llcornerx, 792 - llcornery);
    fprintf(adjfile, "fill\n");
    fprintf(adjfile, "1 1 0 s\n");
  }
  for (i = 0; i < subdomains; i++) {
    for (j = i + 1; j < subdomains; j++) {
      if (adjptr[i * subdomains + j]) {
        point1 = &center[i * 2];
        point2 = &center[j * 2];
        fprintf(adjfile, "%f %f m\n",
                (point1[0] - xcenter) * xscale + xoffset,
                (point1[1] - ycenter) * yscale + yoffset);
        fprintf(adjfile, "%f %f l\nk\n",
                (point2[0] - xcenter) * xscale + xoffset,
                (point2[1] - ycenter) * yscale + yoffset);
      }
    }
  }
  for (i = 0; i < subdomains; i++) {
    point1 = &center[i * 2];
    if (!bw_ps) {
      colorindex = i & 63;
      fprintf(adjfile, "%6.3f %6.3f %6.3f s\n",
              (showmereal) rgb[colorindex].red / 65535.0,
              (showmereal) rgb[colorindex].green / 65535.0,
              (showmereal) rgb[colorindex].blue / 65535.0);
      fprintf(adjfile, "%f %f %d 0 360 arc\nfill\n",
              (point1[0] - xcenter) * xscale + xoffset,
              (point1[1] - ycenter) * yscale + yoffset,
              5 + (line_width >> 1));
    } else {
      fprintf(adjfile, "%f %f %d 0 360 arc\nfill\n",
              (point1[0] - xcenter) * xscale + xoffset,
              (point1[1] - ycenter) * yscale + yoffset,
              3 + (line_width >> 1));
    }
  }
}

void print_adj3d(FILE *adjfile,
                 int subdomains,
                 int *adjptr,
                 showmereal *center,
                 showmereal xcenter,
                 showmereal ycenter,
                 showmereal zcenter,
                 showmereal xscale,
                 showmereal yscale,
                 showmereal xoffset,
                 showmereal yoffset,
                 int llcornerx,
                 int llcornery)
{
  int i, j;
  showmereal *point1, *point2;
  int colorindex;
  vector_t hpt1, hpt2;

  if (!bw_ps) {
    fprintf(adjfile, "0 0 0 s\n");
    fprintf(adjfile, "%d %d m\n", llcornerx, llcornery);
    fprintf(adjfile, "%d %d l\n", 612 - llcornerx, llcornery);
    fprintf(adjfile, "%d %d l\n", 612 - llcornerx, 792 - llcornery);
    fprintf(adjfile, "%d %d l\n", llcornerx, 792 - llcornery);
    fprintf(adjfile, "fill\n");
    fprintf(adjfile, "1 1 0 s\n");
  }
  for (i = 0; i < subdomains; i++) {
    for (j = i + 1; j < subdomains; j++) {
      if (adjptr[i * subdomains + j]) {
        point1 = &center[i * 3];
        hpt1[0] = point1[0] - xcenter;
        hpt1[1] = point1[1] - ycenter;
        hpt1[2] = point1[2] - zcenter;
        mult_matvec(viewmatrix, hpt1, hpt1);
/*        perspectiveproj(hpt1, &pt1); */
        point2 = &center[j * 3];
        hpt2[0] = point2[0] - xcenter;
        hpt2[1] = point2[1] - ycenter;
        hpt2[2] = point2[2] - zcenter;
        mult_matvec(viewmatrix, hpt2, hpt2);
/*        perspectiveproj(hpt2, &pt2); */
        fprintf(adjfile, "%f %f m\n",
                hpt1[0] * xscale + xoffset,
                hpt1[1] * yscale + yoffset);
        fprintf(adjfile, "%f %f l\nk\n",
                hpt2[0] * xscale + xoffset,
                hpt2[1] * yscale + yoffset);
      }
    }
  }
  for (i = 0; i < subdomains; i++) {
    point1 = &center[i * 3];
    hpt1[0] = point1[0] - xcenter;
    hpt1[1] = point1[1] - ycenter;
    hpt1[2] = point1[2] - zcenter;
    mult_matvec(viewmatrix, hpt1, hpt1);
/*    perspectiveproj(hpt1, &pt1); */
    if (!bw_ps) {
      colorindex = i & 63;
      fprintf(adjfile, "%6.3f %6.3f %6.3f s\n",
              (showmereal) rgb[colorindex].red / 65535.0,
              (showmereal) rgb[colorindex].green / 65535.0,
              (showmereal) rgb[colorindex].blue / 65535.0);
      fprintf(adjfile, "%f %f %d 0 360 arc\nfill\n",
              hpt1[0] * xscale + xoffset, hpt1[1] * yscale + yoffset,
              5 + (line_width >> 1));
    } else {
      fprintf(adjfile, "%f %f %d 0 360 arc\nfill\n",
              hpt1[0] * xscale + xoffset, hpt1[1] * yscale + yoffset,
              3 + (line_width >> 1));
    }
  }
}

void print_data2d(FILE *datafile,
                  showmelong nodes,
                  showmelong elems,
                  showmelong datavalues,
                  showmereal *nodeptr,
                  showmelong *eleptr,
                  showmereal *dataptr,
                  showmereal *datahist,
                  showmereal xcenter,
                  showmereal ycenter,
                  showmereal xscale,
                  showmereal yscale,
                  showmereal xoffset,
                  showmereal yoffset,
                  int llcornerx,
                  int llcornery)
{
  showmelong i, j;
  int index;

  showmereal *point1, *point2, backz;
  vector_t hpt1, hpt2;
  showmereal normx, normy, normz, norm;
  int shade;

  showmereal datafac = 5.0 / (datahist[1] - datahist[0]);
  showmereal dataoffset = 0.5 * (datahist[0] + datahist[1]) * datafac;

  XPoint *final;

  struct facelist *drawfaces = (struct facelist *)
                               malloc(elems * sizeof(struct facelist));
  XPoint *vertices = (XPoint *) malloc(sizeof(XPoint) * elems * 3 + 4);
  struct vec_t corner[4];

  if (!bw_ps) {
    fprintf(datafile, "0 0 0 s\n");
    fprintf(datafile, "%d %d m\n", llcornerx, llcornery);
    fprintf(datafile, "%d %d l\n", 612 - llcornerx, llcornery);
    fprintf(datafile, "%d %d l\n", 612 - llcornerx, 792 - llcornery);
    fprintf(datafile, "%d %d l\n", llcornerx, 792 - llcornery);
    fprintf(datafile, "fill\n");
  }

  index = 3;
  for (i = 1; i <=elems * 3; i++) {
    if (eleptr[index] >= 0) {
      point1 = &nodeptr[eleptr[index] * 2];
      if (index % 3 == 0) {
        point2 = &nodeptr[eleptr[index + 2] * 2];
      } else {
        point2 = &nodeptr[eleptr[index - 1] * 2];
      }

      hpt1[0] = point1[0] - xcenter;
      hpt1[1] = point1[1] - ycenter;
      hpt1[2] = dataptr[eleptr[index] / 2 + 1] * datafac - dataoffset;
      mult_matvec(viewmatrix, hpt1, hpt1);
      backz = hpt1[2];
/*      perspectiveproj(hpt1, &pt1); */

      if (wireframe) {
        hpt2[0] = point2[0] - xcenter;
        hpt2[1] = point2[1] - ycenter;
        if (index % 3 == 0) {
          hpt2[2] = dataptr[eleptr[index + 2] / 2 + 1] * datafac - dataoffset;
        } else {
          hpt2[2] = dataptr[eleptr[index - 1] / 2 + 1] * datafac - dataoffset;
        }
        mult_matvec(viewmatrix, hpt2, hpt2);
/*        perspectiveproj(hpt2, &pt2); */
      }

      if (wireframe) {
        fprintf(datafile, "%f %f m\n",
                (hpt1[0] - xcenter) * xscale + xoffset,
                (hpt1[1] - ycenter) * yscale + yoffset);
        fprintf(datafile, "%f %f l\n",
                (hpt2[0] - xcenter) * xscale + xoffset,
                (hpt2[1] - ycenter) * yscale + yoffset);
      } else {
        vertices[i - 1].x = (int) ((hpt1[0] - xcenter) * xscale + xoffset);
        vertices[i - 1].y = (int) ((hpt1[1] - ycenter) * yscale + yoffset);

	j = (i - 1) / 3;

        if ((i - 1) % 3 == 0) {
          drawfaces[j].whichtet = i - 1;
          drawfaces[j].z = 0.0;
        }
        drawfaces[j].z += backz;

        corner[(i - 1) % 3].x = hpt1[0] * xscale + xoffset;
        corner[(i - 1) % 3].y = hpt1[1] * yscale + yoffset;
        corner[(i - 1) % 3].z = backz * 100.0;

        if ((i - 1) % 3 == 2) {
          normx = (corner[1].y - corner[0].y) * (corner[2].z - corner[0].z) -
                  (corner[1].z - corner[0].z) * (corner[2].y - corner[0].y);
          normy = (corner[1].z - corner[0].z) * (corner[2].x - corner[0].x) -
                  (corner[1].x - corner[0].x) * (corner[2].z - corner[0].z);
          normz = (corner[1].x - corner[0].x) * (corner[2].y - corner[0].y) -
                  (corner[1].y - corner[0].y) * (corner[2].x - corner[0].x);

          norm = normx * normx + normy * normy + normz * normz;
          if (norm == 0.0) {
            normx = 0.0;
            normy = 0.0;
            normz = 1.0;
            norm = 1.0;
          } else {
            norm = 1.0 / sqrt(norm);
          }
          if (normz < 0.0) {
            normx = - normx;
            normy = - normy;
            normz = - normz;
          }
          shade = (int) ((normx * lightsourcex +
                          normy * lightsourcey +
                          normz * lightsourcez) * norm * 65536.0);
          if (shade < 0) {
            shade = 0;
          } else if (shade >= 65536) {
            /* This should never happen. */
            shade = 65535;
          }
          drawfaces[j - 1].color = shade;
        }
      }
      index++;
    }
  }

  if (!wireframe) {
    if (elems > 1) {
      sortfaces(drawfaces, elems);
    }
    for (j = 0; j < elems; j++) {
      final = vertices + drawfaces[j].whichtet;

      fprintf(datafile, "%6.3f %6.3f %6.3f s\n",
              (showmereal) drawfaces[j].color / 65535.0,
              (showmereal) drawfaces[j].color / 65535.0,
              (showmereal) drawfaces[j].color / 65535.0);

      fprintf(datafile, "%d %d m\n", final[0].x, final[0].y);
      fprintf(datafile, "%d %d l\n", final[1].x, final[1].y);
      fprintf(datafile, "%d %d l\n", final[2].x, final[2].y);
      fprintf(datafile, "%d %d l\n", final[0].x, final[0].y);

      fprintf(datafile, "gsave\nclosepath\nclip\nfill\ngrestore\n");

      fprintf(datafile, "1 1 0 s\n");

      /* outline the solid polygons */
      fprintf(datafile, "%d %d m\n", final[0].x, final[0].y);
      fprintf(datafile, "%d %d l\n", final[1].x, final[1].y);
      fprintf(datafile, "%d %d l\n", final[2].x, final[2].y);
      fprintf(datafile, "%d %d l\n", final[0].x, final[0].y);
      fprintf(datafile, "gsave\ngrestore\nk\n");
    }
  }
  fprintf(datafile, "k\n");
}

void print_image(int inc,
                 int image,
                 showmereal xcenter,
                 showmereal ycenter,
                 showmereal zcenter,
                 showmereal xwidth,
                 showmereal ywidth,
                 showmereal xtrans,
                 showmereal ytrans,
                 int eps)
{
  FILE *psfile;
  showmereal xxscale, yyscale, xxoffset, yyoffset;
  char psfilename[FILENAMESIZE];
  int llcornerx, llcornery;

  if (image == NOTHING) {
    return;
  }
  if (!loaded[inc][image]) {
    return;
  }
  if ((image == PART) && (explode || bw_ps)) {
    xwidth *= (1.0 + explosion);
    ywidth *= (1.0 + explosion);
  }
  xxscale = (460.0 - (showmereal) line_width) / xwidth;
  yyscale = (640.0 - (showmereal) line_width) / ywidth;
  if (xxscale > yyscale) {
    xxscale = yyscale;
    llcornerx = (604 - (int) (yyscale * xwidth) - line_width) >> 1;
    llcornery = 72;
  } else {
    yyscale = xxscale;
    llcornerx = 72;
    llcornery = (784 - (int) (xxscale * ywidth) - line_width) >> 1;
  }
  xxoffset = 306.0 + xtrans * xxscale;
  yyoffset = 396.0 + ytrans * yyscale;
  switch(image) {
    case NODE:
      addps(nodefilename[inc], psfilename, eps);
      break;
    case POLY:
      addps(polyfilename[inc], psfilename, eps);
      break;
    case ELE:
      addps(elefilename[inc], psfilename, eps);
      break;
    case EDGE:
      addps(edgefilename[inc], psfilename, eps);
      break;
    case PART:
      addps(partfilename[inc], psfilename, eps);
      break;
    case ADJ:
      addps(adjfilename[inc], psfilename, eps);
      break;
    case VORO:
      addps(vedgefilename[inc], psfilename, eps);
      break;
    case DATA:
      addps(datafilename[inc], psfilename, eps);
      break;
    default:
      break;
  }
  if (print_head(psfilename, &psfile, llcornerx, llcornery, eps)) {
    return;
  }
  switch(image) {
    case NODE:
      if (nodedim[inc] == 2) {
        print_node2d(psfile, nodes[inc], nodeptr[inc], xcenter, ycenter,
                     xxscale, yyscale, xxoffset, yyoffset);
      } else {
        print_node3d(psfile, nodes[inc], nodeptr[inc],
                     xcenter, ycenter, zcenter, xxscale, yyscale,
                     xxoffset, yyoffset);
      }
      break;
    case POLY:
      if (polynodes[inc] > 0) {
        if (polydim[inc] == 2) {
          print_poly2d(psfile, polynodes[inc], polyedges[inc],
                       polynodeptr[inc], polysegptr[inc], xcenter, ycenter,
                       xxscale, yyscale, xxoffset, yyoffset);
        }
      } else {
        if (nodedim[inc] == 2) {
          print_poly2d(psfile, nodes[inc], polyedges[inc], nodeptr[inc],
                       polysegptr[inc], xcenter, ycenter,  xxscale, yyscale,
                       xxoffset, yyoffset);
        }
      }
      break;
    case ELE:
      if (nodedim[inc] == 2) {
        print_ele2d(psfile, elems[inc], nodeptr[inc], eleptr[inc],
                    (int *) NULL, (showmereal *) NULL, xcenter, ycenter,
                    xxscale, yyscale, xxoffset, yyoffset,
                    llcornerx, llcornery);
      } else {
        print_ele3d(psfile, elems[inc], nodeptr[inc], eleptr[inc],
                    (int *) NULL, (showmereal *) NULL,
                    xcenter, ycenter, zcenter,
                    xxscale, yyscale, xxoffset, yyoffset,
                    llcornerx, llcornery);
      }
      break;
    case EDGE:
      if (nodedim[inc] == 2) {
        print_edge2d(psfile, nodes[inc], edges[inc], nodeptr[inc],
                     edgeptr[inc], normptr[inc], xcenter, ycenter,
                     xxscale, yyscale, xxoffset, yyoffset,
                     llcornerx, llcornery);
      } else {
        print_edge3d(psfile, nodes[inc], edges[inc], nodeptr[inc],
                     edgeptr[inc], xcenter, ycenter, zcenter,
                     xxscale, yyscale, xxoffset, yyoffset,
                     llcornerx, llcornery);
      }
      break;
    case PART:
      if (nodedim[inc] == 2) {
        print_ele2d(psfile, elems[inc], nodeptr[inc], eleptr[inc],
                    partition[inc], subdomshift[inc], xcenter, ycenter,
                    xxscale, yyscale, xxoffset, yyoffset,
                    llcornerx, llcornery);
      } else {
        print_ele3d(psfile, elems[inc], nodeptr[inc], eleptr[inc],
                    partition[inc], subdomshift[inc],
                    xcenter, ycenter, zcenter,
                    xxscale, yyscale, xxoffset, yyoffset,
                    llcornerx, llcornery);
      }
      break;
    case ADJ:
      if (nodedim[inc] == 2) {
        print_adj2d(psfile, adjsubdomains[inc], adjptr[inc], subdomcenter[inc],
                    xcenter, ycenter, xxscale, yyscale, xxoffset, yyoffset,
                    llcornerx, llcornery);
      } else {
        print_adj3d(psfile, adjsubdomains[inc], adjptr[inc], subdomcenter[inc],
                    xcenter, ycenter, zcenter, xxscale, yyscale,
                    xxoffset, yyoffset, llcornerx, llcornery);
      }
      break;
    case VORO:
      if (vnodedim[inc] == 2) {
        if (loaded[inc][NODE]) {
          if (nodedim[inc] == 2) {
            print_node2d(psfile, nodes[inc], nodeptr[inc], xcenter, ycenter,
                         xxscale, yyscale, xxoffset, yyoffset);
          }
        }
        print_edge2d(psfile, vnodes[inc], vedges[inc], vnodeptr[inc],
                     vedgeptr[inc], vnormptr[inc], xcenter, ycenter,
                     xxscale, yyscale, xxoffset, yyoffset,
                     llcornerx, llcornery);
      }
      break;
    case DATA:
      if (nodedim[inc] == 2) {
        print_data2d(psfile, nodes[inc], elems[inc], datavalues[inc],
                     nodeptr[inc], eleptr[inc], dataptr[inc], datahist[inc],
                     xcenter, ycenter, xxscale, yyscale, xxoffset, yyoffset,
                     llcornerx, llcornery);
      }
      break;
    default:
      break;
  }
  if (!eps) {
    fprintf(psfile, "showpage\n");
  }
  fclose(psfile);
}

/*
 *  The next section allows the user to slice away a portion of a 3D mesh.
 *  The mechanism is to project the mesh points to the screen and set a flag
 *  to 0 (no plotting) to all those tetrahedra whose projected centers are
 *  to the left, right, above, or below, depending on the cut.
 *
 *  Also, those tetrahedra that are internal (have all 4 faces shared with
 *  other tetrahedra) and are on the cutting plane will be plotted.  Multiple
 *  cuts are also possible with trim_tetrahedron, as previously cutout tetra
 *  will not be displayed.
 */

void trim_tetrahedra(int trim_direction,
                     showmelong elems,
                     showmereal *nodeptr,
                     showmelong *eleptr)
{
  showmelong i;
  int j;
  vector_t hpt;
  showmereal xptr, yptr;
  showmelong firsttet;
  int adjacent;

  do {
    XNextEvent(display, &event);
  } while (event.type != ButtonRelease);
  xptr = ((showmereal) event.xbutton.x - xoffset) / xscale;
  yptr = ((showmereal) event.xbutton.y - yoffset) / yscale;
  draw_buttons();

  for (i = 1; i <= elems; i++) {
    if (!tetraptr[i].invisible) {
      hpt[0] = 0.0;
      hpt[1] = 0.0;
      hpt[2] = 0.0;
      for (j = 0; j < 4; j++) {
        hpt[0] += nodeptr[eleptr[i * 4 + j] * 3];
        hpt[1] += nodeptr[eleptr[i * 4 + j] * 3 + 1];
        hpt[2] += nodeptr[eleptr[i * 4 + j] * 3 + 2];
      }
      hpt[0] = 0.25 * hpt[0] - xcenter;
      hpt[1] = 0.25 * hpt[1] - ycenter;
      hpt[2] = 0.25 * hpt[2] - zcenter;
      mult_matvec(viewmatrix, hpt, hpt);
/*      perspectiveproj(hpt, &centroid); */

      if (((trim_direction == TRIMLEFT) && (hpt[0] < xptr)) ||
          ((trim_direction == TRIMRIGHT) && (hpt[0] > xptr)) ||
          ((trim_direction == TRIMDOWN) && (hpt[1] < yptr)) ||
          ((trim_direction == TRIMUP) && (hpt[1] > yptr))) {
        tetraptr[i].invisible = 1;
        for (j = 0; j < 4; j++) {
          adjacent = tetraptr[i].neighbor[j];
          if (adjacent != 0) {
            if (adjacent < 0) {
              adjacent = -adjacent;
            }
            tetraptr[adjacent >> 2].neighbor[adjacent & 3] =
                    -tetraptr[adjacent >> 2].neighbor[adjacent & 3];
          }
        }
      }
    }
  }

  firsttet = 0;
  for (i = elems; i > 0; i--) {
    if (!tetraptr[i].invisible) {
      if ((tetraptr[i].neighbor[0] <= 0)
          || (tetraptr[i].neighbor[1] <= 0)
          || (tetraptr[i].neighbor[2] <= 0)
          || (tetraptr[i].neighbor[3] <= 0)) {
        tetraptr[i].nexttet = firsttet;
        firsttet = i;
      }
    }
  }
  tetraptr[0].nexttet = firsttet;
}

void quality_tetrahedra(showmereal quality,
                        showmereal *nodeptr,
                        showmelong *eleptr,
                        showmelong elems,
                        int dim)
{
  showmereal p[4][3];
  showmereal quality2;
  showmereal dx[3][4], dy[3][4], dz[3][4];
  showmereal edgelength[3][4];
  showmereal ji[3], ki[3];
  showmereal kcrossj[3];
  showmereal pyrvolume;
  showmereal facearea2;
  showmereal biggestface;
  showmereal pyrlongest2;
  showmereal pyrminaltitude2;
  showmereal pyraspect2;
  showmelong thiselem;
  int pointindex;
  showmelong firsttet;
  int adjacent;
  int i, j, k;

  quality2 = quality * quality;
  for (thiselem = 1; thiselem <= elems; thiselem++) {
    if (!tetraptr[thiselem].invisible) {
      for (j = 0; j < 4; j++) {
        pointindex = 3 * eleptr[thiselem * 4 + j];
        p[j][0] = nodeptr[pointindex];
        p[j][1] = nodeptr[pointindex + 1];
        p[j][2] = nodeptr[pointindex + 2];
      }
      pyrlongest2 = 0.0;
      biggestface = 0.0;
      for (i = 0; i < 3; i++) {
        j = (i + 1) & 3;
        k = (i + 2) & 3;
        ji[0] = p[j][0] - p[i][0];
        ji[1] = p[j][1] - p[i][1];
        ji[2] = p[j][2] - p[i][2];
        ki[0] = p[k][0] - p[i][0];
        ki[1] = p[k][1] - p[i][1];
        ki[2] = p[k][2] - p[i][2];
        kcrossj[0] = ki[1] * ji[2] - ki[2] * ji[1];
        kcrossj[1] = ki[2] * ji[0] - ki[0] * ji[2];
        kcrossj[2] = ki[0] * ji[1] - ki[1] * ji[0];
        facearea2 = kcrossj[0] * kcrossj[0] + kcrossj[1] * kcrossj[1]
                  + kcrossj[2] * kcrossj[2];
        if (facearea2 > biggestface) {
          biggestface = facearea2;
        }
        for (j = i + 1; j < 4; j++) {
          dx[i][j] = p[i][0] - p[j][0];
          dy[i][j] = p[i][1] - p[j][1];
          dz[i][j] = p[i][2] - p[j][2];
          edgelength[i][j] = dx[i][j] * dx[i][j] + dy[i][j] * dy[i][j]
                           + dz[i][j] * dz[i][j];
          if (edgelength[i][j] > pyrlongest2) {
            pyrlongest2 = edgelength[i][j];
          }
        }
      }
      pyrvolume = orient3d(p[0], p[1], p[2], p[3]);
      pyrminaltitude2 = pyrvolume * pyrvolume / biggestface;
      pyraspect2 = pyrlongest2 / pyrminaltitude2;

      if (pyraspect2 < quality2) {
        tetraptr[thiselem].invisible = 1;
        for (j = 0; j < 4; j++) {
          adjacent = tetraptr[thiselem].neighbor[j];
          if (adjacent != 0) {
            if (adjacent < 0) {
              adjacent = -adjacent;
            }
            tetraptr[adjacent >> 2].neighbor[adjacent & 3] =
                    -tetraptr[adjacent >> 2].neighbor[adjacent & 3];
          }
        }
      }
    }
  }

  firsttet = 0;
  for (i = elems; i > 0; i--) {
    if (!tetraptr[i].invisible) {
      if ((tetraptr[i].neighbor[0] <= 0)
          || (tetraptr[i].neighbor[1] <= 0)
          || (tetraptr[i].neighbor[2] <= 0)
          || (tetraptr[i].neighbor[3] <= 0)) {
        tetraptr[i].nexttet = firsttet;
        firsttet = i;
      }
    }
  }
  tetraptr[0].nexttet = firsttet;
}

int main(int argc,
         char **argv)
{
  showmereal xwidth, ywidth, zwidth;
  showmereal xtrans, ytrans;
  showmereal xptr, yptr;
  showmereal quality;
  showmereal AmtTilt = 15.0;
  matrix_t matrix;
  int past_image;
  int new_image;
  int new_inc;
  int lastPan = 0, lastTilt = 0, Pan, Tilt;
  showmelong firsttet;
  int adjacent;
  int addtolist;
  int done;
  int i, j;
  char *instring;

  parsecommandline(argc, argv);
  showme_init();
  choose_image(start_inc, start_image);

  showme_window(argc, argv);

  if (current_image != NOTHING) {
    xcenter = 0.5 * (xlo[current_inc][current_image]
                     + xhi[current_inc][current_image]);
    ycenter = 0.5 * (ylo[current_inc][current_image]
                     + yhi[current_inc][current_image]);
    zcenter = 0.5 * (zlo[current_inc][current_image]
                     + zhi[current_inc][current_image]);
    xwidth = xhi[current_inc][current_image] - xlo[current_inc][current_image];
    ywidth = yhi[current_inc][current_image] - ylo[current_inc][current_image];
    if (nodedim[current_inc] == 3) {
      zwidth = zhi[current_inc][current_image]
             - zlo[current_inc][current_image];
      if ((zwidth > xwidth) && (zwidth > ywidth)) {
        xwidth = ywidth = 1.3 * zwidth;
      } else if (ywidth > xwidth) {
        xwidth = ywidth = 1.3 * ywidth;
      } else {
        xwidth = ywidth = 1.3 * xwidth;
      }
    }
    zoom = 0;
    xtrans = 0;
    ytrans = 0;
  }

  XMaskEvent(display, ExposureMask, &event);
  while (1) {
    switch (event.type) {
      case MotionNotify:
        if (motion &&
            ((current_image == DATA) || (nodedim[current_inc] == 3))) {
          Pan = (showmereal) (event.xbutton.x - lastPan) * 0.1 * AmtTilt;
          Tilt = (showmereal) (event.xbutton.y - lastTilt) * 0.1 * AmtTilt;
	  lastPan = event.xbutton.x;
	  lastTilt = event.xbutton.y;

	  three_D_rot_matrix_y(matrix, Pan * DEG2RAD);
	  mult_matmat(matrix, viewmatrix, viewmatrix);
	  three_D_rot_matrix_x(matrix, Tilt * DEG2RAD);
	  mult_matmat(matrix, viewmatrix, viewmatrix);

          draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                     xwidth, ywidth, xtrans, ytrans);
          while (XCheckMaskEvent(display, PointerMotionMask, &event));
        }
        break;
      case ButtonRelease:
        if (event.xany.window == quitwin) {
          XDestroyWindow(display, mainwindow);
          XCloseDisplay(display);
          return 0;
        } else if (event.xany.window == leftwin) {
          xtrans -= 0.125 * xwidth;
	  amtLeft += 0.125;
          draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                     xwidth, ywidth, xtrans, ytrans);
        } else if (event.xany.window == rightwin) {
          xtrans += 0.125 * xwidth;
	  amtLeft -= 0.125;
          draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                     xwidth, ywidth, xtrans, ytrans);
        } else if (event.xany.window == upwin) {
          ytrans += 0.125 * xwidth;
	  amtUp += 0.125;
          draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                     xwidth, ywidth, xtrans, ytrans);
        } else if (event.xany.window == downwin) {
          ytrans -= 0.125 * xwidth;
	  amtUp -= 0.125;
          draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                     xwidth, ywidth, xtrans, ytrans);
        } else if (event.xany.window == resetwin) {
          done = 0;
          if ((nodedim[current_inc] == 3) &&
              ((current_image == ELE) || (current_image == PART)
               || (current_image == DATA))) {
            firsttet = 0;
	    for (i = elems[current_inc]; i > 0; i--) {
	      if (tetraptr[i].invisible) {
                tetraptr[i].invisible = 0;
                done = 1;
              }
              addtolist = 0;
              for (j = 0; j < 4; j++) {
                adjacent = tetraptr[i].neighbor[j];
                if (adjacent < 0) {
                  tetraptr[i].neighbor[j] = -adjacent;
                } else if (adjacent == 0) {
                  addtolist = 1;
                }
              }
              if (addtolist) {
                tetraptr[i].nexttet = firsttet;
                firsttet = i;
              }
	    }
            tetraptr[0].nexttet = firsttet;
	  }

          if (!done) {
            xcenter = 0.5 * (xlo[current_inc][current_image]
                             + xhi[current_inc][current_image]);
            ycenter = 0.5 * (ylo[current_inc][current_image]
                             + yhi[current_inc][current_image]);
            zcenter = 0.5 * (zlo[current_inc][current_image]
                             + zhi[current_inc][current_image]);
            xwidth = xhi[current_inc][current_image]
                   - xlo[current_inc][current_image];
            ywidth = yhi[current_inc][current_image]
                   - ylo[current_inc][current_image];
            if (nodedim[current_inc] == 3) {
              zwidth = zhi[current_inc][current_image]
                    - zlo[current_inc][current_image];
              if ((zwidth > xwidth) && (zwidth > ywidth)) {
                xwidth = 1.3 * zwidth;
                ywidth = 1.3 * zwidth;
              } else if (ywidth > xwidth) {
                xwidth = 1.3 * ywidth;
              } else {
                ywidth = 1.3 * xwidth;
              }
            }

            if ((zoom == 0) && (xtrans == 0) && (ytrans == 0)) {
              identitymatrix(viewmatrix);
              perspfactor = 1.5;
            }
            zoom = 0;
            xtrans = 0;
            ytrans = 0;
          }

          draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                     xwidth, ywidth, xtrans, ytrans);
        } else if (event.xany.window == widthpluswin) {
          if (line_width < 100) {
            line_width++;
            XSetLineAttributes(display, linegc, line_width, LineSolid,
                               CapRound, JoinRound);
            XSetLineAttributes(display, trianglegc, line_width, LineSolid,
                               CapRound, JoinRound);
            draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                       xwidth, ywidth, xtrans, ytrans);
          }
        } else if (event.xany.window == widthminuswin) {
          if (line_width > 1) {
            line_width--;
            XSetLineAttributes(display, linegc, line_width, LineSolid,
                               CapRound, JoinRound);
            XSetLineAttributes(display, trianglegc, line_width, LineSolid,
                               CapRound, JoinRound);
            draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                       xwidth, ywidth, xtrans, ytrans);
          }
        } else if (event.xany.window == expwin) {
          if ((current_image == PART) && loaded[current_inc][PART]) {
            explode = !explode;
	    dlist_drawn = 0;
            draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                       xwidth, ywidth, xtrans, ytrans);
          }
        } else if (event.xany.window == exppluswin) {
          if ((current_image == PART) && loaded[PART] && explode) {
            explosion += 0.125;
	    dlist_drawn = 0;
            findsubdomshift(nodedim[current_inc], subdomains[current_inc],
                            explosion, subdomcenter[current_inc],
                            subdomshift[current_inc]);
            draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                       xwidth, ywidth, xtrans, ytrans);
          }
        } else if (event.xany.window == expminuswin) {
          if ((current_image == PART) && loaded[PART] && explode &&
              (explosion >= 0.125)) {
            explosion -= 0.125;
            findsubdomshift(nodedim[current_inc], subdomains[current_inc],
                            explosion, subdomcenter[current_inc],
                            subdomshift[current_inc]);
            draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                       xwidth, ywidth, xtrans, ytrans);
          }
        } else if (event.xany.window == fillwin) {
          if (((current_image == PART) && loaded[PART]) ||
              ((current_image == DATA) && loaded[DATA])) {
            fillelem = !fillelem;
            draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                       xwidth, ywidth, xtrans, ytrans);
          }
        } else if (event.xany.window == pswin) {
          fill_button(pswin);
          XDrawString(display, pswin, blackfontgc, 2, 13, "PS", 2);
          XFlush(display);
          print_image(current_inc, current_image, xcenter, ycenter, zcenter,
                      xwidth, ywidth, xtrans, ytrans, 0);
          XClearWindow(display, pswin);
          XDrawString(display, pswin, fontgc, 2, 13, "PS", 2);
        } else if (event.xany.window == epswin) {
          fill_button(epswin);
          XDrawString(display, epswin, blackfontgc, 2, 13, "EPS", 3);
          XFlush(display);
          print_image(current_inc, current_image, xcenter, ycenter, zcenter,
                      xwidth, ywidth, xtrans, ytrans, 1);
          XClearWindow(display, epswin);
          XDrawString(display, epswin, fontgc, 2, 13, "EPS", 3);
        } else if (event.xany.window == versionpluswin) {
          move_inc(1);
          loweriteration++;
          setfilenames(filename, loweriteration);
          if (current_inc == 1) {
            current_inc = 0;
          } else {
            current_image = NOTHING;
            XClearWindow(display, mainwindow);
          }
        } else if (event.xany.window == versionminuswin) {
          if (loweriteration > 0) {
            move_inc(0);
            loweriteration--;
            setfilenames(filename, loweriteration);
            if (current_inc == 0) {
              current_inc = 1;
            } else {
              current_image = NOTHING;
              XClearWindow(display, mainwindow);
            }
          }
        } else if (event.xany.window == motionwin) {
          if ((current_image == DATA) || (nodedim[current_inc] == 3)) {
            motion = !motion;
            if (motion) {
              printf("Press middle button to turn off rotation.\n");
              lastPan = event.xbutton.x;
              lastTilt = event.xbutton.y;
            }
          }
	} else if (event.xany.window == rotateamtwin) {
          if ((current_image == DATA) || (nodedim[current_inc] == 3)) {
            instring = get_line("Rotation (degrees):");
            AmtTilt = atof(instring);
          }
	} else if ((event.xany.window == rotatewin[0])
                   || (event.xany.window == rotatewin[1])) {
	  if (event.xany.window == rotatewin[0]) {
            Tilt = AmtTilt;
          } else {
            Tilt = -AmtTilt;
          }
	  three_D_rot_matrix_x(matrix, Tilt * DEG2RAD);
	  mult_matmat(matrix, viewmatrix, viewmatrix);
          draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                     xwidth, ywidth, xtrans, ytrans);
        } else if ((event.xany.window == rotatewin[2])
                   || (event.xany.window == rotatewin[3])) {
	  if (event.xany.window == rotatewin[2]) {
            Pan = -AmtTilt;
          } else {
            Pan = AmtTilt;
          }
	  three_D_rot_matrix_y(matrix, Pan * DEG2RAD);
	  mult_matmat(matrix, viewmatrix, viewmatrix);
          draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                     xwidth, ywidth, xtrans, ytrans);
        } else if ((event.xany.window == rotatewin[4])
                   || (event.xany.window == rotatewin[5])) {
	  if (event.xany.window == rotatewin[4]) {
            Camera_Twist = AmtTilt;
          } else {
            Camera_Twist = -AmtTilt;
          }
	  three_D_rot_matrix_z(matrix, Camera_Twist * DEG2RAD);
	  mult_matmat(matrix, viewmatrix, viewmatrix);
          draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                     xwidth, ywidth, xtrans, ytrans);
        } else if (event.xany.window == wireframeoptionwin) {
          if ((current_image == DATA) || (nodedim[current_inc] == 3)) {
            wireframe = !wireframe;
            dlist_drawn = 0;
            draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                       xwidth, ywidth, xtrans, ytrans);
          }
        } else if (event.xany.window == perspectivewin) {
          if ((current_image == DATA) || (nodedim[current_inc] == 3)) {
            perspective = !perspective;
            draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                       xwidth, ywidth, xtrans, ytrans);
          }
        } else if (event.xany.window == perspluswin) {
          if (perspective
              && ((current_image == DATA) || (nodedim[current_inc] == 3))) {
	    perspfactor *= 0.8;
            draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                       xwidth, ywidth, xtrans, ytrans);
	  }
        } else if (event.xany.window == persminuswin) {
	  if (perspective
              && (current_image == DATA || nodedim[current_inc] == 3)) {
	    perspfactor *= 1.25;
            draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                       xwidth, ywidth, xtrans, ytrans);
	  }
	} else if (event.xany.window == cutleftwin) {
          if ((nodedim[current_inc] == 3) &&
              ((current_image == ELE) || (current_image == PART)
               || (current_image == DATA))) {
            fill_button(cutleftwin);
            XDrawString(display, cutleftwin, blackfontgc, 2, 13, "Cut<", 4);
            trim_tetrahedra(TRIMLEFT, elems[current_inc], nodeptr[current_inc],
                            eleptr[current_inc]);
            draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                       xwidth, ywidth, xtrans, ytrans);
          }
	} else if (event.xany.window == cutrightwin) {
          if ((nodedim[current_inc] == 3) &&
              ((current_image == ELE) || (current_image == PART)
               || (current_image == DATA))) {
            fill_button(cutrightwin);
            XDrawString(display, cutrightwin, blackfontgc, 2, 13, ">", 1);
            trim_tetrahedra(TRIMRIGHT, elems[current_inc],
                            nodeptr[current_inc], eleptr[current_inc]);
            draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                       xwidth, ywidth, xtrans, ytrans);
          }
	} else if (event.xany.window == cutupwin) {
          if ((nodedim[current_inc] == 3) &&
              ((current_image == ELE) || (current_image == PART)
               || (current_image == DATA))) {
            fill_button(cutupwin);
            XDrawString(display, cutupwin, blackfontgc, 2, 13, "^", 1);
            trim_tetrahedra(TRIMUP, elems[current_inc], nodeptr[current_inc],
                            eleptr[current_inc]);
            draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                       xwidth, ywidth, xtrans, ytrans);
          }
	} else if (event.xany.window == cutdownwin) {
          if ((nodedim[current_inc] == 3) &&
              ((current_image == ELE) || (current_image == PART)
               || (current_image == DATA))) {
            fill_button(cutdownwin);
            XDrawString(display, cutdownwin, blackfontgc, 2, 13, "v", 1);
            trim_tetrahedra(TRIMDOWN, elems[current_inc], nodeptr[current_inc],
                            eleptr[current_inc]);
            draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                       xwidth, ywidth, xtrans, ytrans);
          }
	} else if (event.xany.window == qualitywin) {
          if ((nodedim[current_inc] == 3) &&
              ((current_image == ELE) || (current_image == PART)
               || (current_image == DATA))) {
            instring = get_line("Aspect ratio:");
            quality = atof(instring);
            quality_tetrahedra(quality, nodeptr[current_inc],
                               eleptr[current_inc], elems[current_inc],
                               nodedim[current_inc]);
            draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                       xwidth, ywidth, xtrans, ytrans);
          }
	} else if ((event.xany.window == nodewin[0]) ||
                   (event.xany.window == polywin[0]) ||
                   (event.xany.window == elewin[0]) ||
                   (event.xany.window == edgewin[0]) ||
                   (event.xany.window == partwin[0]) ||
                   (event.xany.window == adjwin[0]) ||
                   (event.xany.window == voronoiwin[0]) ||
                   (event.xany.window == datawin[0]) ||
                   (event.xany.window == nodewin[1]) ||
                   (event.xany.window == polywin[1]) ||
                   (event.xany.window == elewin[1]) ||
                   (event.xany.window == edgewin[1]) ||
                   (event.xany.window == partwin[1]) ||
                   (event.xany.window == adjwin[1]) ||
                   (event.xany.window == voronoiwin[1]) ||
                   (event.xany.window == datawin[1])) {
          if (event.xany.window == nodewin[0]) {
            new_inc = 0;
            new_image = NODE;
          }
          if (event.xany.window == polywin[0]) {
            new_inc = 0;
            new_image = POLY;
          }
          if (event.xany.window == elewin[0]) {
            new_inc = 0;
            new_image = ELE;
          }
          if (event.xany.window == edgewin[0]) {
            new_inc = 0;
            new_image = EDGE;
          }
          if (event.xany.window == partwin[0]) {
            new_inc = 0;
            new_image = PART;
          }
          if (event.xany.window == adjwin[0]) {
            new_inc = 0;
            new_image = ADJ;
          }
          if (event.xany.window == voronoiwin[0]) {
            new_inc = 0;
            new_image = VORO;
          }
          if (event.xany.window == datawin[0]) {
            new_inc = 0;
            new_image = DATA;
          }
          if (event.xany.window == nodewin[1]) {
            new_inc = 1;
            new_image = NODE;
          }
          if (event.xany.window == polywin[1]) {
            new_inc = 1;
            new_image = POLY;
          }
          if (event.xany.window == elewin[1]) {
            new_inc = 1;
            new_image = ELE;
          }
          if (event.xany.window == edgewin[1]) {
            new_inc = 1;
            new_image = EDGE;
          }
          if (event.xany.window == partwin[1]) {
            new_inc = 1;
            new_image = PART;
          }
          if (event.xany.window == adjwin[1]) {
            new_inc = 1;
            new_image = ADJ;
          }
          if (event.xany.window == voronoiwin[1]) {
            new_inc = 1;
            new_image = VORO;
          }
          if (event.xany.window == datawin[1]) {
            new_inc = 1;
            new_image = DATA;
          }
          past_image = current_image;
          if ((current_inc == new_inc) && (current_image == new_image)) {
            free_inc(new_inc);
            unload_inc(new_inc);
          }
          choose_image(new_inc, new_image);
          if ((past_image == NOTHING) && (current_image != NOTHING)) {
            xcenter = 0.5 * (xlo[current_inc][current_image]
                             + xhi[current_inc][current_image]);
            ycenter = 0.5 * (ylo[current_inc][current_image]
                             + yhi[current_inc][current_image]);
            zcenter = 0.5 * (zlo[current_inc][current_image]
                             + zhi[current_inc][current_image]);
            xwidth = xhi[current_inc][current_image]
                   - xlo[current_inc][current_image];
            ywidth = yhi[current_inc][current_image]
                   - ylo[current_inc][current_image];
            if (nodedim[current_inc] == 3) {
              zwidth = zhi[current_inc][current_image]
                     - zlo[current_inc][current_image];
              if ((zwidth > xwidth) && (zwidth > ywidth)) {
                xwidth = 1.3 * zwidth;
                ywidth = 1.3 * zwidth;
              } else if (ywidth > xwidth) {
                xwidth = 1.3 * ywidth;
              } else {
                ywidth = 1.3 * xwidth;
              }
            }
            zoom = 0;
            xtrans = 0;
            ytrans = 0;
          }
          draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                     xwidth, ywidth, xtrans, ytrans);
        } else {
	    xptr = ((showmereal) event.xbutton.x - xoffset) / xscale;
	    yptr = ((showmereal) event.xbutton.y - yoffset) / yscale;

          if ((current_image == PART) && loaded[PART] && explode) {
            xptr = (xptr + subdomcenter[current_inc]
                                       [subdomains[current_inc] *
                                        nodedim[current_inc]] *
                    explosion) / (1.0 + explosion);
            yptr = (yptr + subdomcenter[current_inc]
                                       [subdomains[current_inc] *
                                        nodedim[current_inc] + 1] *
                    explosion) / (1.0 + explosion);
          }
          if ((event.xbutton.button == Button1)
              || (event.xbutton.button == Button3)) {
            xtrans = -xptr;
            ytrans = -yptr;
            if (event.xbutton.button == Button1) {
              xwidth /= 1.1;
              ywidth /= 1.1;
              zoom++;
            } else {
              xwidth *= 1.1;
              ywidth *= 1.1;
              zoom--;
            }
            draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                       xwidth, ywidth, xtrans, ytrans);
          } else if (event.xbutton.button == Button2) {
            if ((nodedim[current_inc] == 3) || (current_image == DATA)) {
              motion = 0;
            } else {
              printf("x = %.4g, y = %.4g\n", xptr + xcenter, yptr + ycenter);
            }
          }
        }
        draw_buttons();
        break;
      case DestroyNotify:
        XDestroyWindow(display, mainwindow);
        XCloseDisplay(display);
        return 0;
      case ConfigureNotify:
        if ((width != event.xconfigure.width) ||
            (height != event.xconfigure.height - PANELHEIGHT)) {
          width = event.xconfigure.width;
          height = event.xconfigure.height - PANELHEIGHT;
          draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                     xwidth, ywidth, xtrans, ytrans);
          while (XCheckMaskEvent(display, ExposureMask, &event));
        }
        draw_buttons();
        break;
      case Expose:
        draw_image(current_inc, current_image, xcenter, ycenter, zcenter,
                   xwidth, ywidth, xtrans, ytrans);
        draw_buttons();
        while (XCheckMaskEvent(display, ExposureMask, &event));
        break;
      default:
        break;
    }
    XNextEvent(display, &event);
  }
}
