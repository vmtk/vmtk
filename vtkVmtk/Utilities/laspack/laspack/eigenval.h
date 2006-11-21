/****************************************************************************/
/*                                eigenval.h                                */
/****************************************************************************/
/*                                                                          */
/* estimation of extremal EIGENVALues                                       */
/*                                                                          */
/* Copyright (C) 1992-1996 Tomas Skalicky. All rights reserved.             */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*        ANY USE OF THIS CODE CONSTITUTES ACCEPTANCE OF THE TERMS          */
/*              OF THE COPYRIGHT NOTICE (SEE FILE COPYRGHT.H)               */
/*                                                                          */
/****************************************************************************/

#ifndef EIGENVAL_H
#define EIGENVAL_H

#include "laspack/vector.h"
#include "laspack/qmatrix.h"
#include "laspack/precond.h"
#include "laspack/copyrght.h"

/* estimation of extremal eigenvalues */

void SetEigenvalAccuracy(double Eps);
double GetMinEigenval(QMatrix *Q, PrecondProcType PrecondProc, double OmegaPrecond);
double GetMaxEigenval(QMatrix *Q, PrecondProcType PrecondProc, double OmegaPrecond);

#endif /* EIGENVAL_H */
