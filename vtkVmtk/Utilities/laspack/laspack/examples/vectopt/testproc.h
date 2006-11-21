/****************************************************************************/
/*                                testproc.h                                */
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

#ifndef TESTPROC_H
#define TESTPROC_H

#include <laspack/vector.h>
#include <laspack/qmatrix.h>
#include <laspack/copyrght.h>

Vector *Test1_VV(Vector *V1, Vector *V2);
Vector *Test2_VV(Vector *V1, Vector *V2);
Vector *Test3_VV(Vector *V1, Vector *V2);
Vector *Test4_VV(Vector *V1, Vector *V2);
Vector *Test5_VV(Vector *V1, Vector *V2);

#endif /* TESTPROC_H */
