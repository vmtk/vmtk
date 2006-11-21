/****************************************************************************/
/*                                testproc.c                                */
/****************************************************************************/
/*                                                                          */
/* TEST PROCedures for vector operation optimization for laspack            */
/*                                                                          */
/* Copyright (C) 1992-1996 Tomas Skalicky. All rights reserved.             */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*        ANY USE OF THIS CODE CONSTITUTES ACCEPTANCE OF THE TERMS          */
/*              OF THE COPYRIGHT NOTICE (SEE FILE COPYRGHT.H)               */
/*                                                                          */
/****************************************************************************/

#include <stddef.h>

#include <laspack/errhandl.h>
#include <laspack/copyrght.h>

#include "testproc.h"

Vector *Test1_VV(Vector *V1, Vector *V2)
/* VRes = V1 += V2 ... implementation like laspack version 0.06 */
{
    Vector *VRes;
    
    size_t Dim, Ind;

    if (LASResult() == LASOK) {
        if (V1->Instance == Normal && V1->Dim == V2->Dim) {
            Dim = V1->Dim;
            for(Ind = 1; Ind <= Dim; Ind++)
                V1->Cmp[Ind] += V2->Cmp[Ind];
            VRes = V1;
        } else {
            LASBreak();
            VRes = NULL;
        }
    } else {
        VRes = NULL;
    }
    return(VRes);
}

Vector *Test2_VV(Vector *V1, Vector *V2)
/* VRes = V1 += V2 ... implementation using local variables,
                       ascended counting of vector components */
{
    Vector *VRes;
    
    Real *V1Cmp, *V2Cmp;
    size_t Dim, Ind;

    if (LASResult() == LASOK) {
        if (V1->Instance == Normal && V1->Dim == V2->Dim) {
            Dim = V1->Dim;
            V1Cmp = V1->Cmp;
            V2Cmp = V2->Cmp;
            for(Ind = 1; Ind <= Dim; Ind++)
                V1Cmp[Ind] += V2Cmp[Ind];
            VRes = V1;
        } else {
            LASBreak();
            VRes = NULL;
        }
    } else {
        VRes = NULL;
    }
    return(VRes);
}

Vector *Test3_VV(Vector *V1, Vector *V2)
/* VRes = V1 += V2 ... implementation using local variables,
                       descended counting of vector components */
{
    Vector *VRes;
    
    Real *V1Cmp, *V2Cmp;
    size_t Dim, Ind;

    if (LASResult() == LASOK) {
        if (V1->Instance == Normal && V1->Dim == V2->Dim) {
            Dim = V1->Dim;
            V1Cmp = V1->Cmp;
            V2Cmp = V2->Cmp;
            for(Ind = Dim; Ind > 0; Ind--)
                V1Cmp[Ind] += V2Cmp[Ind];
            VRes = V1;
        } else {
            LASBreak();
            VRes = NULL;
        }
    } else {
        VRes = NULL;
    }
    return(VRes);
}

Vector *Test4_VV(Vector *V1, Vector *V2)
/* VRes = V1 += V2 ... implementation using pointers,
                       ascended counting of vector components */
{
    Vector *VRes;
    
    Real *PtrV1Cmp, *PtrV2Cmp;
    size_t Dim, Ind;

    if (LASResult() == LASOK) {
        if (V1->Instance == Normal && V1->Dim == V2->Dim) {
            Dim = V1->Dim;
            PtrV1Cmp = V1->Cmp + 1;
            PtrV2Cmp = V2->Cmp + 1;
            for(Ind = 1; Ind <= Dim; Ind++) {
                *PtrV1Cmp += *PtrV2Cmp;
                PtrV1Cmp++;
                PtrV2Cmp++;
	    }
            VRes = V1;
        } else {
            LASBreak();
            VRes = NULL;
        }
    } else {
        VRes = NULL;
    }
    return(VRes);
}

Vector *Test5_VV(Vector *V1, Vector *V2)
/* VRes = V1 += V2 ... implementation using pointers,
                       descended counting of vector components */
{
    Vector *VRes;
    
    Real *PtrV1Cmp, *PtrV2Cmp;
    size_t Dim, Ind;

    if (LASResult() == LASOK) {
        if (V1->Instance == Normal && V1->Dim == V2->Dim) {
            Dim = V1->Dim;
            PtrV1Cmp = V1->Cmp + 1;
            PtrV2Cmp = V2->Cmp + 1;
            for(Ind = Dim; Ind > 0; Ind--) {
                *PtrV1Cmp += *PtrV2Cmp;
		PtrV1Cmp++;
                PtrV2Cmp++;
	    }
            VRes = V1;
        } else {
            LASBreak();
            VRes = NULL;
        }
    } else {
        VRes = NULL;
    }
    return(VRes);
}

