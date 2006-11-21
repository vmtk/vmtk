/****************************************************************************/
/*                                extrsolv.h                                */
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

#ifndef EXTRSOLV_H
#define EXTRSOLV_H

#include <laspack/vector.h>
#include <laspack/qmatrix.h>
#include <laspack/itersolv.h>
#include <laspack/copyrght.h>

Vector *TestIter(QMatrix *A, Vector *x, Vector *b, int MaxIter,
	    PrecondProcType Dummy, double Omega);

#endif /* EXTRSOLV_H */
