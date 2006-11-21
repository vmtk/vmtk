/****************************************************************************/
/*                                testproc.c                                */
/****************************************************************************/
/*                                                                          */
/* TEST PROCedures for matrix operation optimization for laspack            */
/*                                                                          */
/* Copyright (C) 1992-1996 Tomas Skalicky. All rights reserved.             */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*        ANY USE OF THIS CODE CONSTITUTES ACCEPTANCE OF THE TERMS          */
/*              OF THE COPYRIGHT NOTICE (SEE FILE COPYRGHT.H)               */
/*                                                                          */
/****************************************************************************/

#include <string.h>

#include <laspack/errhandl.h>
#include <laspack/copyrght.h>

#include "testproc.h"

Vector *Test1_QV(QMatrix *Q, Vector *V)
/* VRes = Q * V ... very simple implementation */
{
    Vector *VRes;

    char *VResName;
    size_t Dim, Row, Clm, ElCount;
    ElType **QEl;

    if (LASResult() == LASOK) {
	if (Q->Dim == V->Dim) {
            Dim = Q->Dim;
            QEl = Q->El;
	    VRes = (Vector *)malloc(sizeof(Vector));
	    VResName = (char *)malloc((strlen(Q_GetName(Q)) + strlen(V_GetName(V)) + 10)
		           * sizeof(char));
            if (VRes != NULL && VResName != NULL) {
	        sprintf(VResName, "(%s) * (%s)", Q_GetName(Q), V_GetName(V));
                V_Constr(VRes, VResName, Dim, Tempor, True);
		if (LASResult() == LASOK) {
                    /* initialisation of vector VRes */
                    V_SetAllCmp(VRes, 0.0);

                    /* analysis of multipliers of matrix Q and vector V
                       is not implemented yet */

                    /* multiplication of matrix elements by vector
                       components */
                    if (!Q->Symmetry) {
       		        if (Q->ElOrder == Rowws) {
		            for (Row = 1; Row <= Dim; Row++) {
			        for (ElCount = 0; ElCount < Q->Len[Row]; ElCount++) {
				    Clm = QEl[Row][ElCount].Pos;
				    VRes->Cmp[Row] += QEl[Row][ElCount].Val * V->Cmp[Clm];
			        }
  			    }
		        }
		        if (Q->ElOrder == Clmws) {
			    for (Clm = 1; Clm <= Dim; Clm++) {
			        for (ElCount = 0; ElCount < Q->Len[Clm]; ElCount++) {
				    Row = QEl[Clm][ElCount].Pos;
				    VRes->Cmp[Row] += QEl[Clm][ElCount].Val * V->Cmp[Clm];
			        }
			    }
		        }
                    } else {
                        /* multiplication by symmetric matrix is not
                           implemented yet */
                        V_SetAllCmp(VRes, 0.0);
                    }
		}
	    } else {
		LASError(LASMemAllocErr, "Mul_QV", Q_GetName(Q), V_GetName(V), NULL);
		if (VRes != NULL)
		    free(VRes);
		if (VResName != NULL)
		    free(VResName);
            }
	} else {
	    LASError(LASDimErr, "Mul_QV", Q_GetName(Q), V_GetName(V), NULL);
	    VRes = NULL;
	}
    } else {
        VRes = NULL;
    }

    if (Q != NULL) {
	if (Q->Instance == Tempor) {
	    Q_Destr(Q);
	    free(Q);
	}
    }
    if (V != NULL) {
	if (V->Instance == Tempor) {
	    V_Destr(V);
	    free(V);
	}
    }
    return(VRes);
}

Vector *Test2_QV(QMatrix *Q, Vector *V)
/* VRes = Q * V ... implementation like laspack version 0.06 */
{
    Vector *VRes;

    char *VResName;
    size_t Dim, Row, Clm, ElCount;
    ElType *PtrEl;
    Real Val;

    if (LASResult() == LASOK) {
	if (Q->Dim == V->Dim) {
            Dim = Q->Dim;
	    VRes = (Vector *)malloc(sizeof(Vector));
	    VResName = (char *)malloc((strlen(Q_GetName(Q)) + strlen(V_GetName(V)) + 10)
		           * sizeof(char));
            if (VRes != NULL && VResName != NULL) {
	        sprintf(VResName, "(%s) * (%s)", Q_GetName(Q), V_GetName(V));
                V_Constr(VRes, VResName, Dim, Tempor, True);
		if (LASResult() == LASOK) {
                    /* initialisation of vector VRes */
                    V_SetAllCmp(VRes, 0.0);

                    /* analysis of multipliers of matrix Q and vector V
                       is not implemented yet */

                    /* multiplication of matrix elements by vector
                       components */
                    if (!Q->Symmetry) {
       		        if (Q->ElOrder == Rowws) {
		            for (Row = 1; Row <= Dim; Row++) {
                                PtrEl = Q->El[Row];
			        for (ElCount = Q->Len[Row]; ElCount > 0;
                                    ElCount--) {
                                    Val = (*PtrEl).Val;
				    Clm = (*PtrEl).Pos;
				    VRes->Cmp[Row] += Val * V->Cmp[Clm];
                                    PtrEl++;
			        }
  			    }
		        }
		        if (Q->ElOrder == Clmws) {
			    for (Clm = 1; Clm <= Dim; Clm++) {
                                PtrEl = Q->El[Clm];
			        for (ElCount = Q->Len[Clm]; ElCount > 0;
                                    ElCount--) {
                                    Val = (*PtrEl).Val;
				    Row = (*PtrEl).Pos;
				    VRes->Cmp[Row] += Val * V->Cmp[Clm];
                                    PtrEl++;
			        }
			    }
		        }
                    } else {
                        /* multiplication by symmetric matrix is not
                           implemented yet */
                        V_SetAllCmp(VRes, 0.0);
                    }
		}
	    } else {
		LASError(LASMemAllocErr, "Mul_QV", Q_GetName(Q), V_GetName(V), NULL);
		if (VRes != NULL)
		    free(VRes);
		if (VResName != NULL)
		    free(VResName);
            }
	} else {
	    LASError(LASDimErr, "Mul_QV", Q_GetName(Q), V_GetName(V), NULL);
	    VRes = NULL;
	}
    } else {
        VRes = NULL;
    }

    if (Q != NULL) {
	if (Q->Instance == Tempor) {
	    Q_Destr(Q);
	    free(Q);
	}
    }
    if (V != NULL) {
	if (V->Instance == Tempor) {
	    V_Destr(V);
	    free(V);
	}
    }
    return(VRes);
}

Vector *Test3_QV(QMatrix *Q, Vector *V)
/* VRes = Q * V ... implementation using local variables and pointers,
                    ascended counting of matrix elements */
{
    Vector *VRes;

    char *VResName;
    size_t Dim, Row, Clm, Len, ElCount;
    size_t *QLen;
    ElType **QEl, *PtrEl;
    Real Sum, Cmp;
    Real *VCmp, *VResCmp;

    if (LASResult() == LASOK) {
	if (Q->Dim == V->Dim) {
            Dim = Q->Dim;
	    VRes = (Vector *)malloc(sizeof(Vector));
	    VResName = (char *)malloc((strlen(Q_GetName(Q)) + strlen(V_GetName(V)) + 10)
		           * sizeof(char));
            if (VRes != NULL && VResName != NULL) {
	        sprintf(VResName, "(%s) * (%s)", Q_GetName(Q), V_GetName(V));
                V_Constr(VRes, VResName, Dim, Tempor, True);
		if (LASResult() == LASOK) {
                    /* initialisation of vector VRes */
		    if (Q->Symmetry || Q->ElOrder == Clmws)
                        V_SetAllCmp(VRes, 0.0);

                    /* analysis of multipliers of matrix Q and vector V
                       is not implemented yet */

                    /* multiplication of matrix elements by vector
                       components */
		    VCmp = V->Cmp;
		    VResCmp = VRes->Cmp;
                    QLen = Q->Len;
                    QEl = Q->El;
                    if (!Q->Symmetry) {
       		        if (Q->ElOrder == Rowws) {
		            for (Row = 1; Row <= Dim; Row++) {
                                Len = QLen[Row];
                                PtrEl = QEl[Row];
				Sum = 0.0;
			        for (ElCount = 0; ElCount < Len; ElCount++) {
				    Sum += (*PtrEl).Val * VCmp[(*PtrEl).Pos];
                                    PtrEl++;
			        }
				VResCmp[Row] = Sum;
  			    }
		        }
		        if (Q->ElOrder == Clmws) {
			    for (Clm = 1; Clm <= Dim; Clm++) {
                                Len = QLen[Clm];
                                PtrEl = QEl[Clm];
				Cmp = VCmp[Clm];
			        for (ElCount = 0; ElCount < Len; ElCount++) {
				    VResCmp[(*PtrEl).Pos] += (*PtrEl).Val * Cmp;
                                    PtrEl++;
			        }
			    }
		        }
                    } else {
                        /* multiplication by symmetric matrix is not
                           implemented yet */
                        V_SetAllCmp(VRes, 0.0);
                    }
		}
	    } else {
		LASError(LASMemAllocErr, "Mul_QV", Q_GetName(Q), V_GetName(V), NULL);
		if (VRes != NULL)
		    free(VRes);
		if (VResName != NULL)
		    free(VResName);
            }
	} else {
	    LASError(LASDimErr, "Mul_QV", Q_GetName(Q), V_GetName(V), NULL);
	    VRes = NULL;
	}
    } else {
        VRes = NULL;
    }

    if (Q != NULL) {
	if (Q->Instance == Tempor) {
	    Q_Destr(Q);
	    free(Q);
	}
    }
    if (V != NULL) {
	if (V->Instance == Tempor) {
	    V_Destr(V);
	    free(V);
	}
    }
    return(VRes);
}

Vector *Test4_QV(QMatrix *Q, Vector *V)
/* VRes = Q * V ... implementation using local variables and pointers,
                    descended counting of matrix elements */
{
    Vector *VRes;

    char *VResName;
    size_t Dim, Row, Clm, Len, ElCount;
    size_t *QLen;
    ElType **QEl, *PtrEl;
    Real Sum, Cmp;
    Real *VCmp, *VResCmp;

    if (LASResult() == LASOK) {
	if (Q->Dim == V->Dim) {
            Dim = Q->Dim;
	    VRes = (Vector *)malloc(sizeof(Vector));
	    VResName = (char *)malloc((strlen(Q_GetName(Q)) + strlen(V_GetName(V)) + 10)
		           * sizeof(char));
            if (VRes != NULL && VResName != NULL) {
	        sprintf(VResName, "(%s) * (%s)", Q_GetName(Q), V_GetName(V));
                V_Constr(VRes, VResName, Dim, Tempor, True);
		if (LASResult() == LASOK) {
                    /* initialisation of vector VRes */
		    if (Q->Symmetry || Q->ElOrder == Clmws)
                        V_SetAllCmp(VRes, 0.0);

                    /* analysis of multipliers of matrix Q and vector V
                       is not implemented yet */

                    /* multiplication of matrix elements by vector
                       components */
		    VCmp = V->Cmp;
		    VResCmp = VRes->Cmp;
                    QLen = Q->Len;
                    QEl = Q->El;
                    if (!Q->Symmetry) {
       		        if (Q->ElOrder == Rowws) {
		            for (Row = Dim; Row > 0; Row--) {
                                Len = QLen[Row];
                                PtrEl = QEl[Row] + Len - 1;
				Sum = 0.0;
			        for (ElCount = Len; ElCount > 0; ElCount--) {
				    Sum += (*PtrEl).Val * VCmp[(*PtrEl).Pos];
                                    PtrEl--;
			        }
				VResCmp[Row] = Sum;
  			    }
		        }
		        if (Q->ElOrder == Clmws) {
			    for (Clm = Dim; Clm > 0; Clm--) {
                                Len = QLen[Clm];
                                PtrEl = QEl[Clm] + Len - 1;
				Cmp = VCmp[Clm];
			        for (ElCount = Len; ElCount > 0; ElCount--) {
                                    VResCmp[(*PtrEl).Pos] += (*PtrEl).Val * Cmp;
                                    PtrEl--;;
			        }
			    }
		        }
                    } else {
                        /* multiplication by symmetric matrix is not
                           implemented yet */
                        V_SetAllCmp(VRes, 0.0);
                    }
		}
	    } else {
		LASError(LASMemAllocErr, "Mul_QV", Q_GetName(Q), V_GetName(V), NULL);
		if (VRes != NULL)
		    free(VRes);
		if (VResName != NULL)
		    free(VResName);
            }
	} else {
	    LASError(LASDimErr, "Mul_QV", Q_GetName(Q), V_GetName(V), NULL);
	    VRes = NULL;
	}
    } else {
        VRes = NULL;
    }

    if (Q != NULL) {
	if (Q->Instance == Tempor) {
	    Q_Destr(Q);
	    free(Q);
	}
    }
    if (V != NULL) {
	if (V->Instance == Tempor) {
	    V_Destr(V);
	    free(V);
	}
    }
    return(VRes);
}
