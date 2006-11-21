/****************************************************************************/
/*                                testproc.h                                */
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

#ifndef TESTPROC_H
#define TESTPROC_H

#include <laspack/vector.h>
#include <laspack/qmatrix.h>
#include <laspack/copyrght.h>

Vector *Test1_QV(QMatrix *Q, Vector *V);
Vector *Test2_QV(QMatrix *Q, Vector *V);
Vector *Test3_QV(QMatrix *Q, Vector *V);
Vector *Test4_QV(QMatrix *Q, Vector *V);

#endif /* TESTPROC_H */
