/****************************************************************************/
/*                                lastest.c                                 */
/****************************************************************************/
/*                                                                          */
/* LASpack - a TEST program                                                 */
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

#define MAX_TEST_NR 20

static void GenMatr(QMatrix *Q);
static Vector *SimpleMul_QV(QMatrix *Q, Vector *V);
static void PrintHelp(void);

int main(int argc, char **argv)
{
    char Descr[MAX_TEST_NR + 1][32];
    double Factor, Clock[MAX_TEST_NR + 1], InlineClock[MAX_TEST_NR + 1];
    double s, si;
    int TestNo, NoCycles, Test, Cycle;
    clock_t BegClock, EndClock;
    size_t Dim, Ind;
    Boolean Help;
    QMatrix L;
    Real *aCmp, *bCmp, *cCmp;
    Vector a, b, c;

    OptTabType OptTab;

    /* initial message */
    fprintf(stderr, "lastest             Version %s\n", LASPACK_VERSION);
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
            /* construction of vectors and matrix */
            V_Constr(&a, "a", Dim, Normal, True);
            V_Constr(&b, "b", Dim, Normal, True);
            V_Constr(&c, "c", Dim, Normal, True);
            Q_Constr(&L, "L", Dim, True, Rowws, Normal, True);

            printf("\n");
            printf("This program tests efficiency of basic operations implemented in LASPack.\n");
	    fprintf(stderr, "\n");

            Factor = 1.0 / CLOCKS_PER_SEC / NoCycles;
            
            /* initialization random-number generator */
            srand(1);

            Test = 0;

            /* Test: vector constructor & dectructor */

            Test++;
            strncpy(Descr[Test], "vector constr. & destr.", 32);
            if (LASResult() == LASOK) {
	        fprintf(stderr, ".");

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++) {
                    V_Destr(&a);
                    V_Constr(&a, "a", Dim, Normal, True);
                }
                EndClock = clock();
                Clock[Test] = (double)(EndClock - BegClock) * Factor;

                InlineClock[Test] = 0.0;
            }

            /* Test: generation of a vector */

            Test++;
            strncpy(Descr[Test], "vector generation", 32);
            if (LASResult() == LASOK) {
	        fprintf(stderr, ".");

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++)
                    V_SetAllCmp(&a, 1.0);
                EndClock = clock();
                Clock[Test] = (double)(EndClock - BegClock) * Factor;

                InlineClock[Test] = 0.0;
            }

            /* Test: matrix constructor & dectructor */

            Test++;
            strncpy(Descr[Test], "matrix constr. & destr.", 32);
            if (LASResult() == LASOK) {
	        fprintf(stderr, ".");
    
                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++) {
                    Q_Destr(&L);
                    Q_Constr(&L, "L", Dim, True, Rowws, Normal, True);
                }
                EndClock = clock();
                Clock[Test] = (double)(EndClock - BegClock) * Factor;

                InlineClock[Test] = 0.0;
            }

            /* Test: generating of L as a symmetric seven diagonal matrix */

            Test++;
            strncpy(Descr[Test], "matrix generation", 32);
            if (LASResult() == LASOK) {
	        fprintf(stderr, ".");

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++)
                    GenMatr(&L);
                EndClock = clock();
                Clock[Test] = (double)(EndClock - BegClock) * Factor;

                InlineClock[Test] = 0.0;
            }

            /* Test: a = b */

            Test++;
            strncpy(Descr[Test], "a = b", 32);
            if (LASResult() == LASOK) {
	        fprintf(stderr, ".");
                V_SetRndCmp(&b);

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++)
                    Asgn_VV(&a, &b);
                EndClock = clock();
                Clock[Test] = (double)(EndClock - BegClock) * Factor;

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++) {
                    aCmp = a.Cmp;
                    bCmp = b.Cmp;
                    for(Ind = 1; Ind <= Dim; Ind++) 
                        aCmp[Ind] = bCmp[Ind];
                }
                EndClock = clock();
                InlineClock[Test] = (double)(EndClock - BegClock) * Factor;
            }

            /* Test: a = b + c */

            Test++;
            strncpy(Descr[Test], "a = b + c", 32);
            if (LASResult() == LASOK) {
	        fprintf(stderr, ".");
                V_SetRndCmp(&b);
                V_SetRndCmp(&c);

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++)
                    Asgn_VV(&a, Add_VV(&b, &c));
                EndClock = clock();
                Clock[Test] = (double)(EndClock - BegClock) * Factor;

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++) {
                    aCmp = a.Cmp;
                    bCmp = b.Cmp;
                    cCmp = c.Cmp;
                    for(Ind = 1; Ind <= Dim; Ind++)
                        aCmp[Ind] = bCmp[Ind] + cCmp[Ind];
                }
                EndClock = clock();
                InlineClock[Test] = (double)(EndClock - BegClock) * Factor;
            }

            /* Test: a = s * b */

            Test++;
            strncpy(Descr[Test], "a = s * b", 32);
            if (LASResult() == LASOK) {
	        fprintf(stderr, ".");
                V_SetRndCmp(&b);
                s = 3.141592;

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++)
                    Asgn_VV(&a, Mul_SV(s, &b));
                EndClock = clock();
                Clock[Test] = (double)(EndClock - BegClock) * Factor;

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++) {
                    aCmp = a.Cmp;
                    bCmp = b.Cmp;
                    for(Ind = 1; Ind <= Dim; Ind++)
                        aCmp[Ind] = s * bCmp[Ind];
                }    
                EndClock = clock();
                InlineClock[Test] = (double)(EndClock - BegClock) * Factor;
            }

            /* Test: s = b * c */

            Test++;
            strncpy(Descr[Test], "s = b * c", 32);
            if (LASResult() == LASOK) {
	        fprintf(stderr, ".");
                V_SetRndCmp(&b);
                V_SetRndCmp(&c);

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++) 
                    s = Mul_VV(&b, &c);
                EndClock = clock();
                Clock[Test] = (double)(EndClock - BegClock) * Factor;

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++) {
                    bCmp = b.Cmp;
                    cCmp = c.Cmp;
                    s = 0.0;
                    for(Ind = 1; Ind <= Dim; Ind++)
                        s += bCmp[Ind] * cCmp[Ind];
                    /* to avoid undesirable optimization */
                    sqrt(s);
                }
                EndClock = clock();
                InlineClock[Test] = (double)(EndClock - BegClock) * Factor;
            }

            /* Test: a = L * b */

            Test++;
            strncpy(Descr[Test], "a = L * b", 32);
            if (LASResult() == LASOK) {
	        fprintf(stderr, ".");
                V_SetRndCmp(&b);
                /* first operation of this type causes the allocation
                   of diagonal elements */
                Asgn_VV(&a, Mul_QV(&L, &b)); 

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++)
                    Asgn_VV(&a, Mul_QV(&L, &b));
                EndClock = clock();
                Clock[Test] = (double)(EndClock - BegClock) * Factor;

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++) 
                    Asgn_VV(&a, SimpleMul_QV(&L, &b));
                EndClock = clock();
                InlineClock[Test] = (double)(EndClock - BegClock) * Factor;
            }

            /* Test: a += b */

            Test++;
            strncpy(Descr[Test], "a = a + b", 32);
            if (LASResult() == LASOK) {
	        fprintf(stderr, ".");
                V_SetRndCmp(&a);
                V_SetRndCmp(&b);

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++)
                    AddAsgn_VV(&a, &b);
                EndClock = clock();
                Clock[Test] = (double)(EndClock - BegClock) * Factor;

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++) {
                    aCmp = a.Cmp;
                    bCmp = b.Cmp;
                    for(Ind = 1; Ind <= Dim; Ind++)
                         aCmp[Ind] += bCmp[Ind];
                }
                EndClock = clock();
                InlineClock[Test] = (double)(EndClock - BegClock) * Factor;
            }

            /* Test: a *= s */

            Test++;
            strncpy(Descr[Test], "a = s * a", 32);
            if (LASResult() == LASOK) {
	        fprintf(stderr, ".");
                V_SetRndCmp(&a);
                s = 3.141592;
                si = 1.0 / s;

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++)
		    if (Cycle % 2 == 0)
                        MulAsgn_VS(&a, s);
		    else
                        MulAsgn_VS(&a, si);
                EndClock = clock();
                Clock[Test] = (double)(EndClock - BegClock) * Factor;

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++) {
                    aCmp = a.Cmp;
		    if (Cycle % 2 == 0)
                        for(Ind = 1; Ind <= Dim; Ind++)
                            aCmp[Ind] *= s;
		    else
                        for(Ind = 1; Ind <= Dim; Ind++)
                            aCmp[Ind] *= si;
                }
                EndClock = clock();
                InlineClock[Test] = (double)(EndClock - BegClock) * Factor;
            }

            /* Test: a = b + s * c */
    
            Test++;
            strncpy(Descr[Test], "a = b + s * c", 32);
            if (LASResult() == LASOK) {
	        fprintf(stderr, ".");
                V_SetRndCmp(&a);
                V_SetRndCmp(&b);
                V_SetRndCmp(&c);
                s = 3.141592;

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++)
                    Asgn_VV(&a, Add_VV(&b, Mul_SV(s, &c)));
                EndClock = clock();
                Clock[Test] = (double)(EndClock - BegClock) * Factor;

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++) {
                    aCmp = a.Cmp;
                    bCmp = b.Cmp;
                    cCmp = c.Cmp;
                    for(Ind = 1; Ind <= Dim; Ind++)
                         aCmp[Ind] = bCmp[Ind] + s * cCmp[Ind];
                }
                EndClock = clock();
                InlineClock[Test] = (double)(EndClock - BegClock) * Factor;
            }

            /* Test: a += s * b */

            Test++;
            strncpy(Descr[Test], "a = a + s * b", 32);
            if (LASResult() == LASOK) {
	        fprintf(stderr, ".");
                V_SetRndCmp(&a);
                V_SetRndCmp(&b);
                s = 3.141592;

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++)
                    AddAsgn_VV(&a, Mul_SV(s, &b));
                EndClock = clock();
                Clock[Test] = (double)(EndClock - BegClock) * Factor;

                BegClock = clock();
                for(Cycle = 1; Cycle <= NoCycles; Cycle++) {
                    aCmp = a.Cmp;
                    bCmp = b.Cmp;
                    for(Ind = 1; Ind <= Dim; Ind++)
                        aCmp[Ind] += s * bCmp[Ind];
                }
                EndClock = clock();
                InlineClock[Test] = (double)(EndClock - BegClock) * Factor;
            }

            fprintf(stderr, "\n");
            TestNo = Test;

            /* output of results */

            if (LASResult() == LASOK) {
                printf("\n");
                printf("Results:    (for dimension %lu, cycles %d)\n",
                    (unsigned long)Dim, NoCycles);
                printf("--------\n\n");
                printf("   #    with LASPack  in-line code  efficiency     ");
                printf("description\n");
                printf("---------------------------------------------------");
                printf("-----------------------\n");
                for (Test = 1; Test <= TestNo; Test++) {
                    if (Clock[Test] > 1e-5 && InlineClock[Test] > 1e-5)
                        printf("  %2d  %10.5f s   %10.5f s    %6.2f %%    ",
                               Test, Clock[Test], InlineClock[Test],
                               100.0 * InlineClock[Test] / Clock[Test]);
                    if (Clock[Test] <= 1e-5 && InlineClock[Test] > 1e-5)
                        printf("  %2d       ---       %10.5f s      ---       ",
                               Test, InlineClock[Test]);
                    if (Clock[Test] > 1e-5 && InlineClock[Test] <= 1e-5)
                        printf("  %2d  %10.5f s        ---          ---       ",
                               Test, Clock[Test]);
                    if (Clock[Test] <= 1e-5 && InlineClock[Test] <= 1e-5)
                        printf("  %2d       ---            ---          ---       ",
                               Test);
                    printf("%s\n", Descr[Test]);
                }
                printf("---------------------------------------------------");
                printf("-----------------------\n\n");
                printf("where s is a scalar, a, b, c are vectors, and L is a matrix.\n");
		printf("The inline routines are simplified in comparison with LASPack and\n");
		printf("can therefore be a little faster.\n");
            }

            /* destruction of vectors and matrix */
            V_Destr(&a);
            V_Destr(&b);
            V_Destr(&c);
            Q_Destr(&L);

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

static void GenMatr(QMatrix *Q)
/* generate matrix Q with a given number of subdiagonals 
   for both triangular parts of the matrix */
{
    size_t NoSubdiag = 3;

    size_t Dim, RoC, Len1, Len2, Entry, Subdiag, Pos;
    Boolean Symmetry;
    ElOrderType ElOrder;

    if (LASResult() == LASOK) {
        Dim = Q_GetDim(Q);
	Symmetry = Q_GetSymmetry(Q);
	ElOrder = Q_GetElOrder(Q);
        for (RoC = 1; RoC <= Dim; RoC++) {
            if (RoC + NoSubdiag <= Dim)
	        Len1 = NoSubdiag;
	    else
	        Len1 = Dim - RoC;
	    if (RoC > NoSubdiag)
	        Len2 = NoSubdiag;
	    else
	        Len2 = RoC - 1;
	    if (Symmetry) {
	        if (ElOrder == Rowws)
		    Q_SetLen(Q, RoC, Len1 + 1);
	        if (ElOrder == Clmws)
		    Q_SetLen(Q, RoC, Len2 + 1);
	    } else {
	        Q_SetLen(Q, RoC, Len1 + Len2 + 1);
	    }
            if (LASResult() == LASOK) {
	        /* main diagonal entry */
	        Entry = 0;
		Q__SetEntry(Q, RoC, Entry, RoC, 1.0);
		/* entries for subdiagonals in both triagonal parts */
	        for (Subdiag = 1; Subdiag <= NoSubdiag ; Subdiag++) {
		    Pos = RoC + Subdiag;
	            if ((!Symmetry || (ElOrder == Rowws)) && Pos <= Dim) {
		        Entry++;
  	                Q__SetEntry(Q, RoC, Entry, Pos, 1.0);
		    }
  	            if ((!Symmetry || (ElOrder == Clmws)) && RoC > Subdiag) {
		        Pos = RoC - Subdiag;
 	                Entry++;
    	                Q__SetEntry(Q, RoC, Entry, Pos, 1.0);
	            }
		}
	    }
	}
    }
}

static Vector *SimpleMul_QV(QMatrix *Q, Vector *V)
/* simple multiplikation matrix by vector, multipliers are not used.
   VRes = Q * V */
{
    Vector *VRes;

    char *VResName;
    size_t Dim, Row, Clm, Len, ElCount;
    size_t *QLen;
    ElType *PtrEl, **QEl,**QDiagEl;
    Real *VCmp, *VResCmp;
    Real El;

    if (LASResult() == LASOK) {
	if (Q->Dim == V->Dim) {
	    Dim = V->Dim;
	    VRes = (Vector *)malloc(sizeof(Vector));
	    VResName = (char *)malloc((strlen(Q_GetName(Q)) + strlen(V_GetName(V)) + 10)
		           * sizeof(char));
            if (VRes != NULL && VResName != NULL) {
	        sprintf(VResName, "(%s) * (%s)", Q_GetName(Q), V_GetName(V));
		V_Constr(VRes, VResName, Dim, Tempor, True);
                
                /* allocation of diagonal elements of matrix Q */
                Q_SortEl(Q);
                Q_AllocInvDiagEl(Q);

		if (LASResult() == LASOK) {
                    /* initialisation of vector VRes */
                    V_SetAllCmp(VRes, 0.0);
                    
                    /* seting local variables */
                    VCmp = V->Cmp;
                    VResCmp = VRes->Cmp;
                    QLen = Q->Len;
                    QEl = Q->El;
                    QDiagEl = Q->DiagEl;
                    
                    /* multiplikation matrix Q by vector V */
                    if (Q->Symmetry && Q->ElOrder == Rowws) {
                        for (Row = 1; Row <= Dim; Row++) {
                            VResCmp[Row] += (*QDiagEl[Row]).Val * VCmp[Row];
                            Len = QLen[Row];
                            PtrEl = QEl[Row] + 1;
                            for (ElCount = Len - 1; ElCount > 0; ElCount--) {
                                El = (*PtrEl).Val;
                                Clm = (*PtrEl).Pos;
                                VResCmp[Row] += El * VCmp[Clm];
                                VResCmp[Clm] += El * VCmp[Row];
                                PtrEl++;
                            }
                        }
		    }
                    if (Q->Symmetry && Q->ElOrder == Clmws) {
			for (Clm = 1; Clm <= Dim; Clm++) {
                            VResCmp[Clm] += (*QDiagEl[Clm]).Val * VCmp[Clm];
                            Len = QLen[Clm];
                            PtrEl = QEl[Clm];
                            for (ElCount = Len - 1; ElCount > 0; ElCount--) {
                                El = (*PtrEl).Val;
                                Row = (*PtrEl).Pos;
                                VResCmp[Row] += El * VCmp[Clm];
                                VResCmp[Clm] += El * VCmp[Row];
                                PtrEl++;
                            }
                        }
                    }
                    if (!Q->Symmetry && Q->ElOrder == Rowws) {
			for (Row = 1; Row <= Dim; Row++) {
                            Len = QLen[Row];
                            PtrEl = QEl[Row];
                            for (ElCount = Len; ElCount > 0; ElCount--) {
                                El = (*PtrEl).Val;
                                Clm = (*PtrEl).Pos;
                                VResCmp[Row] += El * VCmp[Clm];
                                PtrEl++;
                            }
                        }
		    }
                    if (!Q->Symmetry && Q->ElOrder == Clmws) {
			for (Clm = 1; Clm <= Dim; Clm++) {
                            Len = QLen[Clm];
                            PtrEl = QEl[Clm];
                            for (ElCount = Len; ElCount > 0; ElCount--) {
                                El = (*PtrEl).Val;
                                Row = (*PtrEl).Pos;
                                VResCmp[Row] += El * VCmp[Clm];
                                PtrEl++;
                            }
			}
		    }
                }
	    } else {
		LASError(LASMemAllocErr, "SimpleMul_QV", Q_GetName(Q), V_GetName(V), NULL);
		if (VRes != NULL)
		    free(VRes);
		if (VResName != NULL)
		    free(VResName);
            }
	} else {
	    LASError(LASDimErr, "SimpleMul_QV", Q_GetName(Q), V_GetName(V), NULL);
	    VRes = NULL;
	}
    } else {
        VRes = NULL;
    }
    /* DestrFree4Tempor_Q(Q) */
    if (Q != NULL) {
	if (Q->Instance == Tempor) {
	    Q_Destr(Q);
	    free(Q);
	}
    }
    /* DestrFree4Tempor_V(V) */
    if (V != NULL) {
	if (V->Instance == Tempor) {
	    V_Destr(V);
	    free(V);
	}
    }
    return(VRes);
}

static void PrintHelp(void)
/* print help for syntax and avaible options */
{
    fprintf(stderr, "Syntax:\n");
    fprintf(stderr, "  lastest [-d<dim>] [-c<cycles>] [-h]\n\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -d  set vector and matrix dimensions to <dim>, default is %lu\n",
#if defined(__MSDOS__) || defined(MSDOS)
	    (unsigned long)1000);
#else
            (unsigned long)10000);
#endif	    
    fprintf(stderr, "  -c  set number of cycles to <cycles>, default is 10\n");
    fprintf(stderr, "  -h  print this help\n");
}
