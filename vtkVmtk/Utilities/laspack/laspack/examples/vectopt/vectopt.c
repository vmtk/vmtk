/****************************************************************************/
/*                                vectopt.c                                 */
/****************************************************************************/
/*                                                                          */
/* VECTor operation OPTimization for laspack                                */
/*                                                                          */
/* Copyright (C) 1992-1996 Tomas Skalicky. All rights reserved.             */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*        ANY USE OF THIS CODE CONSTITUTES ACCEPTANCE OF THE TERMS          */
/*              OF THE COPYRIGHT NOTICE (SEE FILE COPYRGHT.H)               */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <laspack/errhandl.h>
#include <laspack/vector.h>
#include <laspack/qmatrix.h>
#include <laspack/operats.h>
#include <laspack/version.h>
#include <laspack/copyrght.h>

#include <xc/getopts.h>

#include "testproc.h"

#define MAX_TEST_NR 10

static void PrintHelp(void);

int main(int argc, char **argv)
{
    double Factor, ClockLASPack = 0.0, Clock[MAX_TEST_NR + 1];
    int TestNo, NoCycles, Test, Cycle;
    clock_t BegClock, EndClock;
    size_t Dim;
    Boolean Help;
    Vector x, y;

    OptTabType OptTab;

    /* initial message */
    fprintf(stderr, "vectopt             Version %s\n", LASPACK_VERSION);
    fprintf(stderr, "                    (C) 1992-1996 Tomas Skalicky\n");
    fprintf(stderr, "                    Use option -h for help.\n");

    /* generate options table */
    OptTab.No = 3;
    OptTab.Descr = (OptDescrType *)malloc(OptTab.No * sizeof(OptDescrType));
    if (OptTab.Descr != NULL) {
        /* boolean option */
        OptTab.Descr[0].KeyChar = 'd';
        OptTab.Descr[0].Type = SizeOptType;
        OptTab.Descr[0].Variable = (void *)&Dim;

        /* int option */
        OptTab.Descr[1].KeyChar = 'c';
        OptTab.Descr[1].Type = IntOptType;
        OptTab.Descr[1].Variable = (void *)&NoCycles;

        /* option for help */
        OptTab.Descr[2].KeyChar = 'h';
        OptTab.Descr[2].Type = BoolOptType;
        OptTab.Descr[2].Variable = (void *)&Help;

        /* initialize variables with default values */
#if defined(__MSDOS__) || defined(MSDOS)
        /* the most MS-DOS compilers use size_t of 16 bits size, which rescricts
	   arrays to maximal 65536 / 8 = 8192 double elements */
	Dim = 1000;
#else
        Dim = 10000;
#endif	    
        NoCycles = 10;
        Help = False;

        /* analyse options in the command line */
        GetOpts(&argc, argv, &OptTab);
        if (OptResult() == OptOK && !Help) {
            /* construction of test vectors */
            V_Constr(&x, "x", Dim, Normal, True);
            V_Constr(&y, "y", Dim, Normal, True);

            printf("\n");
            printf("This program tests efficiency of several implementations\n");
            printf("of the vector operation:\n\n");
            printf("    <Vector 1> += <Vector 2>.\n");
	    fprintf(stderr, "\n");

            Factor = 1.0 / CLOCKS_PER_SEC / NoCycles;

            Test = 0;

            /* implementation as in the current laspack version */

            if (LASResult() == LASOK) {
	        fprintf(stderr, ".");
                V_SetAllCmp(&x, 0.0);
                V_SetAllCmp(&y, 0.0);

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++) {
                    AddAsgn_VV(&x, &y);
                }
                EndClock = clock();
                ClockLASPack = (double)(EndClock - BegClock) * Factor;
            }

            /* implementation like laspack version 0.06 */

            Test++;
            if (LASResult() == LASOK) {
	        fprintf(stderr, ".");
                V_SetAllCmp(&x, 0.0);
                V_SetAllCmp(&y, 0.0);

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++) {
                    Test1_VV(&x, &y);
                }
                EndClock = clock();
                Clock[Test] = (double)(EndClock - BegClock) * Factor;
            }


            /* implementation using local variables, ascended counting */

            Test++;
            if (LASResult() == LASOK) {
	        fprintf(stderr, ".");
                V_SetAllCmp(&x, 0.0);
                V_SetAllCmp(&y, 0.0);

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++) {
                    Test2_VV(&x, &y);
                }
                EndClock = clock();
                Clock[Test] = (double)(EndClock - BegClock) * Factor;
            }

            /* implementation using local variables, descended counting */

            Test++;
            if (LASResult() == LASOK) {
	        fprintf(stderr, ".");
                V_SetAllCmp(&x, 0.0);
                V_SetAllCmp(&y, 0.0);

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++) {
                    Test3_VV(&x, &y);
                }
                EndClock = clock();
                Clock[Test] = (double)(EndClock - BegClock) * Factor;
            }

            /* implementation using pointers, ascended counting */

            Test++;
            if (LASResult() == LASOK) {
	        fprintf(stderr, ".");
                V_SetAllCmp(&x, 0.0);
                V_SetAllCmp(&y, 0.0);

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++) {
                    Test4_VV(&x, &y);
                }
                EndClock = clock();
                Clock[Test] = (double)(EndClock - BegClock) * Factor;
            }

            /* implementation using pointers, descended counting */

            Test++;
            if (LASResult() == LASOK) {
	        fprintf(stderr, ".");
                V_SetAllCmp(&x, 0.0);
                V_SetAllCmp(&y, 0.0);

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++) {
                    Test5_VV(&x, &y);
                }
                EndClock = clock();
                Clock[Test] = (double)(EndClock - BegClock) * Factor;
            }

            fprintf(stderr, "\n");
            TestNo = Test;

            /* output of results */

            if (LASResult() == LASOK) {
                printf("\n");
                printf("Results:    (for dimension %lu, cycles %d)\n",
                    (unsigned long)Dim, NoCycles);
                printf("--------\n\n");
                printf(" implementation    CPU time\n");
                printf("-----------------------------------------\n");
                if (Clock[Test] > 1e-5)
                    printf("    LASPack    %12.3e s = 100.0 %%\n", ClockLASPack);
                for (Test = 1; Test <= TestNo; Test++) {
                    if (Clock[Test] > 1e-5)
                        printf("      %2d       %12.3e s = %5.1f %%\n",
                            Test, Clock[Test], 100.0 * Clock[Test] / ClockLASPack);
                }
                printf("-----------------------------------------\n\n");
                printf("For details of the implementations look at the file\n");
		printf("laspack/examples/vectopt/testproc.c.\n");
		printf("The current LASPack version corresponds to the implementation %d.\n",
#if !defined(__hppa) && !defined(sparc)
		     2);
#else
		     3);
#endif
		printf("This is simplified in comparison with LASPack and can therefore be\n");
		printf("a little faster.\n");
            }

            /* destruction of test vectors */
            V_Destr(&x);
            V_Destr(&y);

            /* LASPack error messages */
            if (LASResult() != LASOK) {
                fprintf(stderr, "\n");
                fprintf(stderr, "LASPack error: ");
                WriteLASErrDescr(stderr);
            }
        } else {
            /* error messages */
            if (OptResult() == OptNotDefErr || OptResult() == OptSyntaxErr || Help) {
                fprintf(stderr, "\n");
                PrintHelp();
            }
            if (OptResult() == OptDescrErr) {
                fprintf(stderr, "\n");
                fprintf(stderr, "Description of an option faulty.\n");
            }
        }
        
        if (OptTab.Descr != NULL)
            free(OptTab.Descr);
    } else {
        /* error message */
        fprintf(stderr, "\n");
        fprintf(stderr, "Not enought memory for analysis of command line options.\n");
    }
    
    return(0);
}

static void PrintHelp(void)
/* print help for syntax and avaible options */
{
    fprintf(stderr, "Syntax:\n");
    fprintf(stderr, "  vectopt [-d<dim>] [-c<cycles>] [-h]\n\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -d  set vector dimension to <dim>, default is %lu\n",
#if defined(__MSDOS__) || defined(MSDOS)
	    (unsigned long)1000);
#else
            (unsigned long)10000);
#endif	    
    fprintf(stderr, "  -c  set number of cycles to <cycles>, default is 10\n");
    fprintf(stderr, "  -h  print this help\n");
}
