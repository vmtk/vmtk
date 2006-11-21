/****************************************************************************/
/*                                extrsolv.c                                */
/****************************************************************************/
/*                                                                          */
/* EXTeRn SOLVers for systems of linear equations                           */
/*                                                                          */
/* Copyright (C) 1992-1996 Tomas Skalicky. All rights reserved.             */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*        ANY USE OF THIS CODE CONSTITUTES ACCEPTANCE OF THE TERMS          */
/*              OF THE COPYRIGHT NOTICE (SEE FILE COPYRGHT.H)               */
/*                                                                          */
/****************************************************************************/

#include <math.h>
#include <stdio.h>

#include <laspack/errhandl.h>
#include <laspack/operats.h>
#include <laspack/precond.h>
#include <laspack/rtc.h>
#include <laspack/itersolv.h>
#include <laspack/copyrght.h>

#include "extrsolv.h"
#include "mlstest.h"

extern double RTCEps;

#define XSOR

#ifdef SYM_STOR

Vector *TestIter(QMatrix *a, Vector *x, Vector *b, int maxit,
	      PrecondProcType dummy, double rlx)
{
    LASBreak();
    fprintf(stderr, "\n");
    fprintf(stderr, "No test iterative method available for nonsymmetric matrices.\n");
    fprintf(stderr, "\n");
    
    return(x);
}

#else 

#ifdef XSOR

Vector *TestIter(QMatrix *a, Vector *x, Vector *b, int maxit,
	      PrecondProcType dummy, double rlx)
/* SOR lexicographicaly forward iteration ... implementation by Andreas Auge */
{
    int it;
    size_t i, j, col, dim;
    double x_old_i, sum_diff_2,
        sum_ax, sum_b_2 = 0.0, diag_el;

    dim = a->Dim;

    if (LASResult() == LASOK) {
	for (i = 1; i <= dim; i++)
	    sum_b_2 += b->Cmp[i] * b->Cmp[i];

	it = 0;
	do {
	    sum_diff_2 = 0.0;

	    for (i = 1; i <= dim; i++) {
		x_old_i = x->Cmp[i];
		sum_ax = 0.0;
		diag_el = 0.0;

		for (j = 0; j < a->Len[i]; j++) {
		    col = a->El[i][j].Pos;
		    if (col != i) {
			sum_ax += a->El[i][j].Val * x->Cmp[col];
		    } else {
			diag_el = a->El[i][j].Val;
		    }		/*else*/
		}		/*j*/
		x->Cmp[i] = rlx * ((b->Cmp[i] - sum_ax) / diag_el - x_old_i) + x_old_i;

	    }			/*i*/

	    for (i = 1; i <= dim; i++) {
		sum_ax = 0.0;

		for (j = 0; j < a->Len[i]; j++) {
		    sum_ax += a->El[i][j].Val * x->Cmp[a->El[i][j].Pos];
		}		/*j*/
		sum_diff_2 += (b->Cmp[i] - sum_ax) * (b->Cmp[i] - sum_ax);
	    }			/*i*/

	    it++;
	} while (!RTCResult(it, sqrt(sum_diff_2), sqrt(sum_b_2), SORForwIterId + 100)
	    && it < maxit);
    }

    return(x);
}

#endif /* XSOR */

#ifdef XSSOR

Vector *TestIter(QMatrix *a, Vector *x, Vector *b, int maxit,
	      PrecondProcType dummy, double rlx)
/* SSOR iteration ... implementation by Andreas Auge */
{
    int it;
    size_t i, j, col, dim;
    double x_old_i, sum_diff_2,
        sum_ax, sum_b_2 = 0.0, diag_el;

    dim = a->Dim;

    if (LASResult() == LASOK) {
	for (i = 1; i <= dim; i++)
	    sum_b_2 += b->Cmp[i] * b->Cmp[i];

	it = 0;
	do {
	    sum_diff_2 = 0.0;

	    for (i = 1; i <= dim; i++) {
		x_old_i = x->Cmp[i];
		sum_ax = 0.0;
		diag_el = 0.0;

		for (j = 0; j < a->Len[i]; j++) {
		    col = a->El[i][j].Pos;
		    if (col != i) {
			sum_ax += a->El[i][j].Val * x->Cmp[col];
		    } else {
			diag_el = a->El[i][j].Val;
		    }		/*else*/
		}		/*j*/
		x->Cmp[i] = rlx * ((b->Cmp[i] - sum_ax) / diag_el - x_old_i) + x_old_i;

	    }			/*i*/

	    for (i = dim; i >= 1; i--) {
		x_old_i = x->Cmp[i];
		sum_ax = 0.0;
		diag_el = 0.0;

		for (j = 0; j < a->Len[i]; j++) {
		    col = a->El[i][j].Pos;
		    if (col != i) {
			sum_ax += a->El[i][j].Val * x->Cmp[col];
		    } else {
			diag_el = a->El[i][j].Val;
		    }		/*else*/
		}		/*j*/
		x->Cmp[i] = rlx * ((b->Cmp[i] - sum_ax) / diag_el - x_old_i) + x_old_i;

	    }			/*i*/

	    for (i = 1; i <= dim; i++) {
		sum_ax = 0.0;

		for (j = 0; j < a->Len[i]; j++) {
		    sum_ax += a->El[i][j].Val * x->Cmp[a->El[i][j].Pos];
		}		/*j*/
		sum_diff_2 += (b->Cmp[i] - sum_ax) * (b->Cmp[i] - sum_ax);
	    }			/*i*/

	    it++;
	} while (!RTCResult(it, sqrt(sum_diff_2), sqrt(sum_b_2), SSORIterId + 100)
	    && it < maxit);
    }

    return(x);
}

#endif /* XSSOR */

#endif /* SYM_STOR */
