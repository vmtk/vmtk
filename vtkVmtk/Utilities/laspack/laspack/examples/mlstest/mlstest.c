/****************************************************************************/
/*                                mlstest.c                                 */
/****************************************************************************/
/*                                                                          */
/* Multi-Level Solver TEST program for 1d and 2d poisson problems           */
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
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <float.h>

#include <laspack/errhandl.h>
#include <laspack/vector.h>
#include <laspack/matrix.h>
#include <laspack/qmatrix.h>
#include <laspack/operats.h>
#include <laspack/factor.h>
#include <laspack/precond.h>
#include <laspack/eigenval.h>
#include <laspack/rtc.h>
#include <laspack/itersolv.h>
#include <laspack/mlsolv.h>
#include <laspack/version.h>
#include <laspack/copyrght.h>

#include "mlstest.h"
#include "extrsolv.h"

typedef enum {
    Case1DDirich,
    Case2DDirich
} ProblemType;

static QMatrix *L;
static Vector *uw, *fr;
static Matrix *R, *P;

static void MGParmInput(ProblemType *Problem, int *NoLevels, size_t *MaxNoInt,
                IterIdType *MLSolverId, Boolean *NestedMG, int *NoMGIter,
                int *Gamma, int *RestrType, 
                IterProcType *SmoothProc, int *Nu1, int *Nu2,
		PrecondProcType *PrecondProc, double *Omea,
                IterProcType *SolvProc, int *NuC,
		PrecondProcType *PrecondProcC, double *OmegaC,
                int *MaxIter, double *Eps);
static void QVMConstr(int NoLevels, size_t *Dim);
static void QVMDestr(int NoLevels);
static void MGEqsGen1DDirich(int NoLevels, size_t *Dim, int RestrType);
static void MGEqsGen2DDirich(int NoLevels, size_t *Dim1, int RestrType);
static void MGSolver(int NoLevels, IterIdType MLSolverId,
		Boolean NestedMG, int NoMGIter, int Gamma,
                IterProcType SmoothProc, int Nu1, int Nu2,
		PrecondProcType PrecondProc, double Omega,
                IterProcType SolvProc, int NuC,
		PrecondProcType PrecondProcC, double OmegaC,
                int MaxIter, double Eps);
static void MGResOutput1DDirich(int NoLevels);
static void GenL1DDirich(int Level, size_t *Dim);
static void Genf1DDirich(int Level, size_t *Dim);
static void GenRSimple1DDirich(int Level, size_t *Dim);
static void GenRWeight1DDirich(int Level, size_t *Dim);
static void GenP1DDirich(int Level, size_t *Dim);
static void GenL2DDirich(int Level, size_t *Dim1);
static void Genf2DDirich(int Level, size_t *Dim1);
static void GenRSimple2DDirich(int Level, size_t *Dim1);
static void GenRWeight2DDirich(int Level, size_t *Dim1);
static void GenP2DDirich(int Level, size_t *Dim1);
static void IterStatus(int Iter, double rNorm, double bNorm, IterIdType IterId);

int main(void)
{
    int NoLevels; /* number of grid levels */
    int RestrType; /* type of restriction operator */
    int Gamma; /* multigrid iteration parameter */
    int Nu1; /* number of pre-smoothing iterations */
    int Nu2; /* number of post-smoothing iterations */
    int NuC; /* number of iterations on coarse grid */
    int MaxIter; /* maximum number of iterations */
    int NoMGIter; /* number of multigrid iterations within one MGPCG step */
    int Level;
    double Omega; /* relaxation parameter for smoothing iterations */
    double OmegaC; /* relaxation parameter for iterations on coarse grid */
    double Eps; /* epsilon - break off accurary */
    size_t MaxNoInt; /* maximal number of intervals */
    size_t *Dim; /* dimension, point number */
    size_t *Dim1; /* dimension, point number in one direction */
    Boolean NestedMG; /* nested multigrid iteration */
    IterIdType MLSolverId; /* identifier of the chosen multi-level solver */
    IterProcType SmoothProc; /* pointer to a procedure for smoothing iterations */
    IterProcType SolvProc; /* pointer to a procedure for solving on coarsest grid */
    PrecondProcType PrecondProc; /* pointer to preconditioner for smoothing iterations */
    PrecondProcType PrecondProcC; /* pointer to preconditioner on coarsest grid */
    ProblemType Problem; /* kind of problem */

    fprintf(stderr, "mlstest             Version %s\n", LASPACK_VERSION);
    fprintf(stderr, "                    (C) 1992-1996 Tomas Skalicky\n");
    fprintf(stderr,"\n\n");
    fprintf(stderr,"Multilevel solution of a Poisson problem\n");
    fprintf(stderr,"----------------------------------------\n");
    fprintf(stderr,"\n");
    /* multigrid paramater input */
    MGParmInput(&Problem, &NoLevels, &MaxNoInt, 
		&MLSolverId, &NestedMG, &NoMGIter, &Gamma, &RestrType,
		&SmoothProc, &Nu1, &Nu2, &PrecondProc, &Omega,
                &SolvProc, &NuC, &PrecondProcC, &OmegaC, &MaxIter, &Eps);
                
    /* allocation of dynamic variables */
    Dim = (size_t *)malloc(NoLevels * sizeof(size_t));
    Dim1 = (size_t *)malloc(NoLevels * sizeof(size_t));
    L = (QMatrix *)malloc(NoLevels * sizeof(QMatrix));
    uw = (Vector *)malloc(NoLevels * sizeof(Vector));
    fr = (Vector *)malloc(NoLevels * sizeof(Vector));
    R = (Matrix *)malloc(NoLevels * sizeof(Matrix));
    P = (Matrix *)malloc(NoLevels * sizeof(Matrix));
    if (Dim != NULL && Dim1 != NULL && L != NULL && uw != NULL && fr != NULL &&
        R != NULL && P != NULL) {
        /* setting of dimensions for all grid levels */
        Dim1[NoLevels - 1] = MaxNoInt - 1;
        for (Level = NoLevels - 2; Level >= 0; Level--)
            Dim1[Level] = (Dim1[Level + 1] + 1) / 2 - 1;
        switch (Problem) {
            case Case1DDirich:
                for (Level = NoLevels - 1; Level >= 0; Level--)
                    Dim[Level] = Dim1[Level];
                break;
            case Case2DDirich:
                for (Level = NoLevels - 1; Level >= 0; Level--)
                    Dim[Level] = Dim1[Level] * Dim1[Level];
                break;
        }
        /* allocation of matrices, vectors and operators */
        QVMConstr(NoLevels, Dim);
        /* generation of system of equations */
        switch (Problem) {
            case Case1DDirich:
                MGEqsGen1DDirich(NoLevels, Dim, RestrType);
                break;
            case Case2DDirich:
                MGEqsGen2DDirich(NoLevels, Dim1, RestrType);
                break;
        }
        /* solving of system of equations with multigrid method */
        MGSolver(NoLevels, MLSolverId, NestedMG, NoMGIter, Gamma,
            SmoothProc, Nu1, Nu2, PrecondProc, Omega,
	    SolvProc, NuC, PrecondProcC, OmegaC, MaxIter, Eps);
        /* solution output */
        if (LASResult() == LASOK && Problem == Case1DDirich)
            MGResOutput1DDirich(NoLevels);
        /* release of matrices, vectors and operators */
        QVMDestr(NoLevels);
    } else {
        /* error message */
        fprintf(stderr, "\n");
        fprintf(stderr, "Not enought memory running mgtest.\n");
    }
    
    /* release of dynamic variables */
    if (Dim != NULL)
        free(Dim);
    if (Dim1 != NULL)
        free(Dim1);
    if (L != NULL)
        free(L);
    if (uw != NULL)
        free(uw);
    if (fr != NULL)
        free(fr);
    if (R != NULL)
        free(R);
    if (P != NULL)
        free(P);
        
    /* LASPack error messages */
    if (LASResult() != LASOK) {
        fprintf(stderr, "\n");
        fprintf(stderr, "LASPack error: ");
        WriteLASErrDescr(stderr);     
    }
    fprintf(stderr, "\n");

    return(0);
}

static void MGParmInput(ProblemType *Problem, int *NoLevels, size_t *MaxNoInt,
         IterIdType *MLSolverId, Boolean *NestedMG, int *NoMGIter,
         int *Gamma, int *RestrType, 
         IterProcType *SmoothProc, int *Nu1, int *Nu2,
	 PrecondProcType *PrecondProc, double *Omega,
         IterProcType *SolvProc, int *NuC,
         PrecondProcType *PrecondProcC, double *OmegaC,
         int *MaxIter, double *Eps)
/* multigrid parameter input */
{
    char Key[2];
    int Level;
    int ProblemId; /* problem identifier */
    int MainMethId; /* main method identifier */
    int SmoothMethId; /* smoothing method identifier */
    int SolvMethId; /* solving method on coarse grid identifier */
    int PrecondId; /* preconditioning identifier */
    unsigned long AuxSize;
    size_t CoarseCoef;  /* coarsing coefficient */
    Boolean InputOK;

    fprintf(stderr, "\n");
    fprintf(stderr, "Problem type: 1D, Dirichlet boundary condition ... 1\n");
    fprintf(stderr, "              2D, Dirichlet boundary condition ... 2\n");
    fprintf(stderr, "Chosen type:  ");
    do {
        scanf("%d", &ProblemId);
        if (ProblemId < 1 || ProblemId > 2)
            fprintf(stderr, "???        :  ");
    } while (ProblemId < 1 || ProblemId > 2);
    switch (ProblemId) {
        case 1:
            *Problem = Case1DDirich;
            break;
        case 2:
            *Problem = Case2DDirich;
            break;
    }
    do {
        fprintf(stderr, "\n");
        fprintf(stderr, "Number of grid levels: ");
        scanf("%d", NoLevels);
        CoarseCoef = 1;
        for (Level = *NoLevels - 1; Level > 0; Level--)
            CoarseCoef = 2 * CoarseCoef;
        fprintf(stderr, "Number of intervals:   ");
        scanf("%lu", &AuxSize);
	*MaxNoInt = (size_t)AuxSize;
        InputOK = (*MaxNoInt % CoarseCoef == 0 && *MaxNoInt / CoarseCoef > 2);
        if (!InputOK) {
            fprintf(stderr, "Discretisation by this choice is impossible!\n");
        }
    } while (!InputOK);

    fprintf(stderr, "\n");
    fprintf(stderr, "Solution method: multigrid ..................... 1\n");
    fprintf(stderr, "                 multigrid preconditioned CG ... 2\n");
    fprintf(stderr, "                 BPX preconditioned CG ......... 3\n");
    fprintf(stderr, "Chosen method: ");
    do {
        scanf("%d", &MainMethId);
        if (MainMethId < 1 || MainMethId > 3)
            fprintf(stderr, "???          : ");
    } while (MainMethId < 1 || MainMethId > 3);
    switch (MainMethId) {
        case 1:
            *MLSolverId = MGIterId;
            fprintf(stderr, "\n");
            do {
                fprintf(stderr, "Nested multigrid iterations? (y/n) ");
                scanf("%1s", Key);
                Key[0] = toupper(Key[0]);
            } while (Key[0] != 'Y' && Key[0] != 'N');
            if (Key[0] == 'Y')
                *NestedMG = True;
            else
                *NestedMG = False;
            break;
        case 2:
            *MLSolverId = MGPCGIterId;
            fprintf(stderr, "\n");
            fprintf(stderr, "Number of multigrid iterations whithin one MGPCG step: ");
            scanf("%d", NoMGIter);
            break;
        case 3:
            *MLSolverId = BPXPCGIterId;
            break;
    }
    if (*MLSolverId == MGIterId || *MLSolverId == MGPCGIterId) {
        fprintf(stderr, "\n");
        fprintf(stderr, "Type of multigrid: V cycle ... 1\n");
        fprintf(stderr, "                   W cycle ... 2\n");
        fprintf(stderr, "Chosen type: ");
        scanf("%d", Gamma);
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "Type of restriction: simple ..... 1\n");
    fprintf(stderr, "                     weighted ... 2\n");
    fprintf(stderr, "Chosen type: ");
    do {
        scanf("%d", RestrType);
        if (*RestrType != 1 && *RestrType != 2)
            fprintf(stderr, "???        : ");
    } while (*RestrType != 1 && *RestrType != 2);
    
    fprintf(stderr, "\n");
    fprintf(stderr, "Iterative methods: Jacobi ................... 1\n");
    fprintf(stderr, "                   SOR forward .............. 2\n");
    fprintf(stderr, "                   SOR backward ............. 3\n");
    fprintf(stderr, "                   SOR symmetric ............ 4\n");
    fprintf(stderr, "                   Chebyshev ................ 5\n");
    fprintf(stderr, "                   CG ....................... 6\n");
    fprintf(stderr, "                   CGN ...................... 7\n");
    fprintf(stderr, "                   GMRES(10) ................ 8\n");
    fprintf(stderr, "                   BiCG ..................... 9\n");
    fprintf(stderr, "                   QMR ..................... 10\n");
    fprintf(stderr, "                   CGS ..................... 11\n");
    fprintf(stderr, "                   Bi-CGSTAB ............... 12\n");
    fprintf(stderr, "                   Test .................... 13\n");
    fprintf(stderr, "Smoothing method:                         ");
    do {
        scanf("%d", &SmoothMethId);
        if (SmoothMethId < 1 || SmoothMethId > 13)
            fprintf(stderr, "???             :                         ");
    } while (SmoothMethId < 1 || SmoothMethId > 13);
    switch (SmoothMethId) {
        case 1:
            *SmoothProc = JacobiIter;
            break;
        case 2:
            *SmoothProc = SORForwIter;
            break;
        case 3:
            *SmoothProc = SORBackwIter;
            break;
        case 4:
            *SmoothProc = SSORIter;
            break;
        case 5:
            *SmoothProc = ChebyshevIter;
            break;
        case 6:
            *SmoothProc = CGIter;
            break;
        case 7:
            *SmoothProc = CGNIter;
            break;
        case 8:
            *SmoothProc = GMRESIter;
            break;
        case 9:
            *SmoothProc = BiCGIter;
            break;
        case 10:
            *SmoothProc = QMRIter;
            break;
        case 11:
            *SmoothProc = CGSIter;
            break;
        case 12:
            *SmoothProc = BiCGSTABIter;
            break;
        case 13:
            *SmoothProc = TestIter;
            break;
    }
    if (*MLSolverId == MGIterId || *MLSolverId == MGPCGIterId) {
        fprintf(stderr, "Number of pre-smoothing iterations:       ");
        scanf("%d", Nu1);
        fprintf(stderr, "Number of post-smoothing iterations:      ");
        scanf("%d", Nu2);
    } else {
        fprintf(stderr, "Number of smoothing iterations:           ");
        scanf("%d", Nu1);
        Nu2 = 0;
    }
    fprintf(stderr, "Preconditioning: none .......... 0\n");
    fprintf(stderr, "                 Jacobi ........ 1\n");
    fprintf(stderr, "                 SSOR  ......... 2\n");
    fprintf(stderr, "                 ILU/ICH ....... 3\n");
    fprintf(stderr, "Preconditioning for smoothing iterations: ");
    do {
        scanf("%d", &PrecondId);
        if (PrecondId < 0 || PrecondId > 3)
            fprintf(stderr, "???                                     : ");
    } while (PrecondId < 0 || PrecondId > 3);
    switch (PrecondId) {
        case 0:
	    *PrecondProc = (PrecondProcType)NULL;
            break;
        case 1:
	    *PrecondProc = JacobiPrecond;
            break;
        case 2:
            *PrecondProc = SSORPrecond;
            break;
        case 3:
            *PrecondProc = ILUPrecond;
            break;
    }
    fprintf(stderr, "Relaxation parameter for smoothing:       ");
    scanf("%lf", Omega);
    
    fprintf(stderr, "Iterative methods: Jacobi ................... 1\n");
    fprintf(stderr, "                   SOR forward .............. 2\n");
    fprintf(stderr, "                   SOR backward ............. 3\n");
    fprintf(stderr, "                   SOR symmetric ............ 4\n");
    fprintf(stderr, "                   Chebyshev ................ 5\n");
    fprintf(stderr, "                   CG ....................... 6\n");
    fprintf(stderr, "                   CGN ...................... 7\n");
    fprintf(stderr, "                   GMRES(10) ................ 8\n");
    fprintf(stderr, "                   BiCG ..................... 9\n");
    fprintf(stderr, "                   QMR ..................... 10\n");
    fprintf(stderr, "                   CGS ..................... 11\n");
    fprintf(stderr, "                   Bi-CGSTAB ............... 12\n");
    fprintf(stderr, "                   Test .................... 13\n");
    fprintf(stderr, "Solution method on coarsest grid:      ");
    do {
        scanf("%d", &SolvMethId);
        if (SolvMethId < 1 || SolvMethId > 13)
            fprintf(stderr, "???                             :      ");
    } while (SolvMethId < 1 || SolvMethId > 13);
    switch (SolvMethId) {
        case 1:
            *SolvProc = JacobiIter;
            break;
        case 2:
            *SolvProc = SORForwIter;
            break;
        case 3:
            *SolvProc = SORBackwIter;
            break;
        case 4:
            *SolvProc = SSORIter;
            break;
        case 5:
            *SolvProc = ChebyshevIter;
            break;
        case 6:
            *SolvProc = CGIter;
            break;
        case 7:
            *SolvProc = CGNIter;
            break;
        case 8:
            *SolvProc = GMRESIter;
            break;
        case 9:
            *SolvProc = BiCGIter;
            break;
        case 10:
            *SolvProc = QMRIter;
            break;
        case 11:
            *SolvProc = CGSIter;
            break;
        case 12:
            *SolvProc = BiCGSTABIter;
            break;
        case 13:
            *SolvProc = TestIter;
            break;
    }
    fprintf(stderr, "Number of iterations on coarsest grid: ");
    scanf("%d", NuC);
    fprintf(stderr, "Preconditioning: none .......... 0\n");
    fprintf(stderr, "                 Jacobi ........ 1\n");
    fprintf(stderr, "                 SSOR  ......... 2\n");
    fprintf(stderr, "                 ILU/ICH ....... 3\n");
    fprintf(stderr, "Preconditioning on coarsest grid:      ");
    do {
        scanf("%d", &PrecondId);
        if (PrecondId < 0 || PrecondId > 3)
            fprintf(stderr, "???                             :      ");
    } while (PrecondId < 0 || PrecondId > 3);
    switch (PrecondId) {
        case 0:
	    *PrecondProcC = (PrecondProcType)NULL;
            break;
        case 1:
	    *PrecondProcC = JacobiPrecond;
            break;
        case 2:
            *PrecondProcC = SSORPrecond;
            break;
        case 3:
            *PrecondProcC = ILUPrecond;
            break;
    }
    fprintf(stderr, "Relaxation parameter on coarsest grid: ");
    scanf("%lf", OmegaC);

    fprintf(stderr, "\n");
    fprintf(stderr, "Maximum number of iterations:          ");
    scanf("%d", MaxIter);
    fprintf(stderr, "Break off accurary for the residual:   ");
    scanf("%lf", Eps);
}

static void QVMConstr(int NoLevels, size_t *Dim)
/* calling of constructors for matrices, vectors of unknowns,
   vectors of right hand side, restriction and prolongation operators */
{
    int Level;

    char Name[10];
    
    for (Level = NoLevels - 1; Level >= 0; Level--) {
        sprintf(Name, "L[%d]", Level % 1000);
#ifdef SYM_STOR
        Q_Constr(&L[Level], Name, Dim[Level], True, Rowws, Normal, True);
#else
        Q_Constr(&L[Level], Name, Dim[Level], False, Rowws, Normal, True);
#endif /* SYM_STOR */
        sprintf(Name, "uw[%d]", Level % 1000);
        V_Constr(&uw[Level], Name, Dim[Level], Normal, True);
        sprintf(Name, "fr[%d]", Level % 1000);
        V_Constr(&fr[Level], Name, Dim[Level], Normal, True);
        if (Level < NoLevels - 1) {
            sprintf(Name, "R[%d]", Level % 1000);
            M_Constr(&R[Level], Name, Dim[Level], Dim[Level + 1], Rowws, Normal, True);
        }
	if (Level > 0) {
            sprintf(Name, "P[%d]", Level % 1000);
            M_Constr(&P[Level], Name, Dim[Level], Dim[Level - 1], Clmws, Normal, True);
	}
    }
}

static void QVMDestr(int NoLevels)
/* calling of destructors for matrices, vectors of unknowns,
   vectors of right hand side, restriction and prolongation operators */
{
    int Level;
    for (Level = NoLevels - 1; Level >= 0; Level--) {
        Q_Destr(&L[Level]);
        V_Destr(&uw[Level]);
        V_Destr(&fr[Level]);
        if (Level < NoLevels - 1)
            M_Destr(&R[Level]);
        if (Level > 0)
            M_Destr(&P[Level]);
    }
}

static void MGEqsGen1DDirich(int NoLevels, size_t *Dim, int RestrType)
/* generation of system of equations for all grid levels for 1D case,
   Dirichlet boundary condition */
{
    int Level;
    double ClockFactor, CPUTime;
    clock_t BegClock, EndClock;

    /* start of time counting */
    ClockFactor = 1.0 / (double)CLOCKS_PER_SEC;
    BegClock = clock();
    /* generation of matrices L */
    printf("\n");
    printf("Generation of matrices ... \n");
    for (Level = NoLevels - 1; Level >= 0; Level--)
        GenL1DDirich(Level, Dim);
    /* generation of right hand side f */
    printf("Generation of vectors ... \n");
    for (Level = NoLevels - 1; Level >= 0; Level--)
        Genf1DDirich(Level, Dim);
    /* generation of restriction operators */
    printf("Generation of restriction operators ... \n");
    for (Level = NoLevels - 2; Level >= 0; Level--) {
        if (RestrType == 1) {
            /* for simple restriction operators */
            GenRSimple1DDirich(Level, Dim);
        }
        if (RestrType ==  2) {
            /* for wighted restriction operators */
            GenRWeight1DDirich(Level, Dim);
        }
    }
    printf("Generation of prolongation operators ... \n");
    for (Level = NoLevels - 1; Level >= 1; Level--)
        GenP1DDirich(Level, Dim);
    /* end of time counting and geting out of CPU time */
    EndClock = clock();
    CPUTime = (double)(EndClock - BegClock) * ClockFactor;
    printf("\n");
    printf("  CPU time: %7.2f s\n", CPUTime);
}

static void MGEqsGen2DDirich(int NoLevels, size_t *Dim1, int RestrType)
/* generation of system of equations for all grid levels for 2D case,
   Dirichlet boundary condition */
{
    int Level;
    double ClockFactor, CPUTime;
    clock_t BegClock, EndClock;

    /* start of time counting */
    ClockFactor = 1.0 / (double)CLOCKS_PER_SEC;
    BegClock = clock();
    /* generation of matrices L */
    printf("\n");
    printf("Generation of matrices ... \n");
    for (Level = NoLevels - 1; Level >= 0; Level--)
        GenL2DDirich(Level, Dim1);
    /* generation of right hand side f */
    printf("Generation of vectors ... \n");
    for (Level = NoLevels - 1; Level >= 0; Level--)
        Genf2DDirich(Level, Dim1);
    /* generation of restriction operators */
    printf("Generation of restriction operators ... \n");
    for (Level = NoLevels - 2; Level >= 0; Level--) {
        if (RestrType == 1) {
            /* for simple restriction operators */
            GenRSimple2DDirich(Level, Dim1);
        }
        if (RestrType ==  2) {
            /* for wighted restriction operators */
            GenRWeight2DDirich(Level, Dim1);
        }
    }
    /* generation of prolongation operators P */
    printf("Generation of prolongation operators ... \n");
    for (Level = NoLevels - 1; Level >= 1; Level--)
        GenP2DDirich(Level, Dim1);
    /* end of time counting and geting out of CPU time */
    EndClock = clock();
    CPUTime = (double)(EndClock - BegClock) * ClockFactor;
    printf("\n");
    printf("  CPU time: %7.2f s\n", CPUTime);
}

static void MGSolver(int NoLevels, IterIdType MLSolverId,
	 Boolean NestedMG, int NoMGIter, int Gamma,
         IterProcType SmoothProc, int Nu1, int Nu2,
         PrecondProcType PrecondProc, double Omega,
         IterProcType SolvProc, int NuC,
         PrecondProcType PrecondProcC, double OmegaC,
         int MaxIter, double Eps)
/* solution of discret problem with multigrid solver */
{
    int NoIter; /* number of performed iterations */
    int Level;
    double AccBeg, AccEnd; /* reached accuracy (of residuum) */
    double ContrRatePerMGIter, ContrRatePerSec;
    double ClockFactor, CPUTime;
    clock_t BegClock, EndClock;

    /* setting of RTC parameters */
    SetRTCAccuracy(Eps);
    SetRTCAuxProc(IterStatus);
    
    /* estimate extremal eigenvalues */
    if(SmoothProc == ChebyshevIter || SolvProc == ChebyshevIter) {
        /* start of time counting */
        ClockFactor = 1.0 / (double)CLOCKS_PER_SEC;
        BegClock = clock();

        /* initialization random-number generator */
        srand(1);

        printf("\n");
        printf("Estimating extremal eigenvalues ...\n");
        if(SmoothProc == ChebyshevIter) {
            for (Level = NoLevels - 1; Level > 0; Level--) {
                GetMinEigenval(&L[Level], PrecondProc, Omega);
                GetMaxEigenval(&L[Level], PrecondProc, Omega);
            }
        }
        if(SolvProc == ChebyshevIter) {
            GetMinEigenval(&L[0], PrecondProcC, OmegaC);
            GetMaxEigenval(&L[0], PrecondProcC, OmegaC);
        }
        
        /* end of time counting and geting out the CPU time */
        EndClock = clock();
        CPUTime = (double)(EndClock - BegClock) * ClockFactor;
        printf("\n");
        printf("  CPU time: %7.2f s\n", CPUTime);
    }
    
    /* factorize matrices */
    if(PrecondProc == ILUPrecond || PrecondProcC == ILUPrecond) {
        /* start of time counting */
        ClockFactor = 1.0 / (double)CLOCKS_PER_SEC;
        BegClock = clock();

        printf("\n");
        printf("Incomplete factorization of matrices ...\n");
        if(PrecondProc == ILUPrecond
	    && SmoothProc != JacobiIter
	    && SmoothProc != SORForwIter
	    && SmoothProc != SORBackwIter
	    && SmoothProc != SSORIter) {
            for (Level = NoLevels - 1; Level > 0; Level--)
                ILUFactor(&L[Level]);
        }
        if(PrecondProcC == ILUPrecond
	    && SolvProc != JacobiIter
	    && SolvProc != SORForwIter
	    && SolvProc != SORBackwIter
	    && SolvProc != SSORIter) {
            ILUFactor(&L[0]);
        }
        
        /* end of time counting and geting out the CPU time */
        EndClock = clock();
        CPUTime = (double)(EndClock - BegClock) * ClockFactor;
        printf("\n");
        printf("  CPU time: %7.2f s\n", CPUTime);
    }
    
    /* solving of system of equations by nested multigrid method */
    if (MLSolverId == MGIterId && NestedMG) {
        /* initialisation of vector of unknowns */
        V_SetAllCmp(&uw[0], 0.0);
        
        /* approximation of solution by nested multigrid method */
        printf("\n");
        printf("Doing nested multigrid iterations ...\n");
        NestedMGIter(NoLevels, L, uw, fr, R, P, Gamma,
            SmoothProc, Nu1, Nu2, PrecondProc, Omega,
            SolvProc, NuC,  PrecondProcC ,OmegaC);

        AccBeg = GetLastAccuracy();
    } else {
        /* initialisation of vector of unknowns */
        V_SetAllCmp(&uw[NoLevels - 1], 0.0);

        AccBeg = 1.0;
    }

    /* start of time counting */
    ClockFactor = 1.0 / (double)CLOCKS_PER_SEC;
    BegClock = clock();

    /* solving of system of equations */
    switch (MLSolverId) {
        case MGIterId:
            printf("\n");
            printf("Doing multigrid iterations ... \n\n");
            MGIter(NoLevels, L, uw, fr, R, P, MaxIter, Gamma,
                SmoothProc, Nu1, Nu2, PrecondProc, Omega,
                SolvProc, NuC, PrecondProcC, OmegaC);
            break;
        case MGPCGIterId:
            printf("\n");
            printf("Doing multigrid preconditioned CG iterations ... \n\n");
            MGPCGIter(NoLevels, L, uw, fr, R, P, MaxIter, NoMGIter, Gamma,
                SmoothProc, Nu1, Nu2, PrecondProc, Omega,
                SolvProc, NuC, PrecondProcC, OmegaC);
            break;
        case BPXPCGIterId:
            printf("\n");
            printf("Doing BPX preconditioned CG iterations ... \n\n");
            BPXPCGIter(NoLevels, L, uw, fr, R, P, MaxIter,
                SmoothProc, Nu1, PrecondProc, Omega,
                SolvProc, NuC, PrecondProcC, OmegaC);
            break;
	default:
	    break;
    }

    /* end of time counting and geting out of CPU time */
    EndClock = clock();
    CPUTime = (double)(EndClock - BegClock) * ClockFactor;
    printf("\n");
    printf("  CPU time: %7.2f s\n", CPUTime);

    AccEnd = GetLastAccuracy();
    NoIter = GetLastNoIter();

    /* computing of middle contraction rates */
    if (NoIter > 0)
        ContrRatePerMGIter = pow(AccEnd / AccBeg, 1.0 / (double)NoIter);
    else
        ContrRatePerMGIter = 0.0;
    if (CPUTime > DBL_EPSILON)
        ContrRatePerSec = pow(AccEnd / AccBeg, 1.0 / CPUTime);
    else
        ContrRatePerSec = 0.0;
    printf("\n");
    printf("Middle contraction rate\n");
    printf("  referred to one iteration: %10.3e\n", ContrRatePerMGIter);
    printf("  referred to 1 s CPU time:  %10.3e\n", ContrRatePerSec);
}

static void MGResOutput1DDirich(int NoLevels)
/* solution output for 1D case, Dirichlet boundary condition */
{
    char Key[2];
    double x;
    size_t Ind, Dim;

    fprintf(stderr, "\n");
    do {
        fprintf(stderr, "Output solution? (y/n) ");
        scanf("%1s", Key);
        Key[0] = toupper(Key[0]);
    } while (Key[0] != 'Y' && Key[0] != 'N');
    if (Key[0] == 'Y') {
        printf("\n");
        printf("     x         u(x)      (node)\n");
        printf("---------------------------------\n");
        printf("   0.00000    0.00000\n");
        Dim = V_GetDim(&uw[NoLevels - 1]);
        for (Ind = 1; Ind <= Dim; Ind++) {
            x = (double)Ind / (double)(Dim + 1);
            printf("  %8.5f   %8.5f    (%lu)\n", x, V_GetCmp(&uw[NoLevels - 1],Ind),
		   (unsigned long)Ind);
        }
        printf("   1.00000    0.00000\n");
        printf("---------------------------------\n");
    }
}

static void GenL1DDirich(int Level, size_t *Dim)
/* generation of matrix for 1D case, Dirichlet boundary
   condition */
{
    double h;
    size_t Row;

    if (LASResult() == LASOK) {
        /* computing of the grid parameter */
        h = 1.0 / (double)(Dim[Level] + 1);

#ifdef SYM_STOR

        /* setting of matrix elements */
        for (Row = 1; Row < Dim[Level]; Row++) {
            Q_SetLen(&L[Level], Row, 2);
            if (LASResult() == LASOK) {
                Q_SetEntry(&L[Level], Row, 0, Row, 2.0 / h);
                Q_SetEntry(&L[Level], Row, 1, Row + 1, -1.0 / h);
            }
        }
        Row = Dim[Level];
        Q_SetLen(&L[Level], Row, 1);
        if (LASResult() == LASOK) {
            Q_SetEntry(&L[Level], Row, 0, Row, 2.0 / h);
        }

#else

        Row = 1;
        Q_SetLen(&L[Level], Row, 2);
        if (LASResult() == LASOK) {
            Q_SetEntry(&L[Level], Row, 0, Row, 2.0 / h);
            Q_SetEntry(&L[Level], Row, 1, Row + 1, -1.0 / h);
        }
        for (Row = 2; Row < Dim[Level]; Row++) {
            Q_SetLen(&L[Level], Row, 3);
            if (LASResult() == LASOK) {
                Q_SetEntry(&L[Level], Row, 0, Row, 2.0 / h);
                Q_SetEntry(&L[Level], Row, 1, Row - 1, -1.0 / h);
                Q_SetEntry(&L[Level], Row, 2, Row + 1, -1.0 / h);
            }
        }
        Row = Dim[Level];
        Q_SetLen(&L[Level], Row, 2);
        if (LASResult() == LASOK) {
            Q_SetEntry(&L[Level], Row, 0, Row, 2.0 / h);
            Q_SetEntry(&L[Level], Row, 1, Row - 1, -1.0 / h);
        }

#endif /* SYM_STOR */

    }
}

static void Genf1DDirich(int Level, size_t *Dim)
/* generation of right hand side for 1D case, Dirichlet boundary condition */
{
    double h;
    size_t Ind;

    if (LASResult() == LASOK) {
        /* computing of the grid parameter */
        h = 1.0 / (double)(Dim[Level] + 1);

        for (Ind = 1; Ind <= Dim[Level]; Ind++) {
            V_SetCmp(&fr[Level], Ind, 1.0 * h);
        }
    }
}

static void GenRSimple1DDirich(int Level, size_t *Dim)
/* generation of simple restriction operator (as matrix) for 1D case,
   Dirichlet boundary condition */
{
    size_t Row;

    if (LASResult() == LASOK) {
        for (Row = 1; Row <= Dim[Level]; Row++) {
            M_SetLen(&R[Level], Row, 1);
            if (LASResult() == LASOK) {
                M_SetEntry(&R[Level], Row, 0, 2 * Row, 2.0);
            }
        }
    }
}

static void GenRWeight1DDirich(int Level, size_t *Dim)
/* generation of weighted restriction operator (as matrix) for 1D case,
   Dirichlet boundary condition */
{
    size_t Row;

    if (LASResult() == LASOK) {
        for (Row = 1; Row <= Dim[Level]; Row++) {
            M_SetLen(&R[Level], Row, 3);
            if (LASResult() == LASOK) {
                M_SetEntry(&R[Level], Row, 0, 2 * Row - 1, 0.5);
                M_SetEntry(&R[Level], Row, 1, 2 * Row, 1.0);
                M_SetEntry(&R[Level], Row, 2, 2 * Row + 1, 0.5);
            }
        }
    }
}

static void GenP1DDirich(int Level, size_t *Dim)
/* generation of prolongation operator (as matrix) for 1D case, Dirichlet
   boundary condition */
{
    size_t Clm;

    if (LASResult() == LASOK) {
        for (Clm = 1; Clm <= Dim[Level - 1]; Clm++) {
            M_SetLen(&P[Level], Clm, 3);
            if (LASResult() == LASOK) {
                M_SetEntry(&P[Level], Clm, 0, 2 * Clm - 1, 0.5);
                M_SetEntry(&P[Level], Clm, 1, 2 * Clm, 1.0);
                M_SetEntry(&P[Level], Clm, 2, 2 * Clm + 1, 0.5);
            }
        }
    }
}

static void GenL2DDirich(int Level, size_t *Dim1)
/* generation of matrix for 2D case, Dirichlet boundary
   condition */
{
    size_t BlockRow, RowBasis, Row;

    if (LASResult() == LASOK) {
#ifdef SYM_STOR

        /* setting of matrix elements */
        for (BlockRow = 1; BlockRow < Dim1[Level]; BlockRow++) {
            RowBasis = Dim1[Level] * (BlockRow - 1);
            for (Row = RowBasis + 1; Row < RowBasis + Dim1[Level]; Row++) {
                Q_SetLen(&L[Level], Row, 3);
                if (LASResult() == LASOK) {
                    Q_SetEntry(&L[Level], Row, 0, Row, 4.0);
                    Q_SetEntry(&L[Level], Row, 1, Row + 1, -1.0);
                    Q_SetEntry(&L[Level], Row, 2, Row + Dim1[Level], -1.0);
                }
            }
            Row = RowBasis + Dim1[Level];
            Q_SetLen(&L[Level], Row, 2);
            if (LASResult() == LASOK) {
                Q_SetEntry(&L[Level], Row, 0, Row, 4.0);
                Q_SetEntry(&L[Level], Row, 1, Row + Dim1[Level], -1.0);
            }
        }
        BlockRow = Dim1[Level];
        RowBasis = Dim1[Level] * (BlockRow - 1);
        for (Row = RowBasis + 1; Row < RowBasis + Dim1[Level]; Row++) {
            Q_SetLen(&L[Level], Row, 2);
            if (LASResult() == LASOK) {
                Q_SetEntry(&L[Level], Row, 0, Row, 4.0);
                Q_SetEntry(&L[Level], Row, 1, Row + 1, -1.0);
            }
        }
        Row = RowBasis + Dim1[Level];
        Q_SetLen(&L[Level], Row, 1);
        if (LASResult() == LASOK) {
            Q_SetEntry(&L[Level], Row, 0, Row, 4.0);
        }

#else

        BlockRow = 1;
        RowBasis = Dim1[Level] * (BlockRow - 1);
        Row = RowBasis + 1;
        Q_SetLen(&L[Level], Row, 3);
        if (LASResult() == LASOK) {
            Q_SetEntry(&L[Level], Row, 0, Row, 4.0);
            Q_SetEntry(&L[Level], Row, 1, Row + 1, -1.0);
            Q_SetEntry(&L[Level], Row, 2, Row + Dim1[Level], -1.0);
        }
        for (Row = RowBasis + 2; Row < RowBasis + Dim1[Level]; Row++) {
            Q_SetLen(&L[Level], Row, 4);
            if (LASResult() == LASOK) {
                Q_SetEntry(&L[Level], Row, 0, Row,  4.0);
                Q_SetEntry(&L[Level], Row, 1, Row - 1, -1.0);
                Q_SetEntry(&L[Level], Row, 2, Row + 1, -1.0);
                Q_SetEntry(&L[Level], Row, 3, Row + Dim1[Level], -1.0);
            }
        }
        Row = RowBasis + Dim1[Level];
        Q_SetLen(&L[Level], Row, 3);
        if (LASResult() == LASOK) {
            Q_SetEntry(&L[Level], Row, 0, Row, 4.0);
            Q_SetEntry(&L[Level], Row, 1, Row - 1, -1.0);
            Q_SetEntry(&L[Level], Row, 2, Row + Dim1[Level], -1.0);
        }
        for (BlockRow = 2; BlockRow < Dim1[Level]; BlockRow++) {
            RowBasis = Dim1[Level] * (BlockRow - 1);
            Row = RowBasis + 1;
            Q_SetLen(&L[Level], Row, 4);
            if (LASResult() == LASOK) {
                Q_SetEntry(&L[Level], Row, 0, Row,  4.0);
                Q_SetEntry(&L[Level], Row, 1, Row - Dim1[Level], -1.0);
                Q_SetEntry(&L[Level], Row, 2, Row + 1, -1.0);
                Q_SetEntry(&L[Level], Row, 3, Row + Dim1[Level], -1.0);
            }
            for (Row = RowBasis + 2; Row < RowBasis + Dim1[Level]; Row++) {
                Q_SetLen(&L[Level], Row, 5);
                if (LASResult() == LASOK) {
                    Q_SetEntry(&L[Level], Row, 0, Row,  4.0);
                    Q_SetEntry(&L[Level], Row, 1, Row - 1, -1.0);
                    Q_SetEntry(&L[Level], Row, 2, Row - Dim1[Level], -1.0);
                    Q_SetEntry(&L[Level], Row, 3, Row + 1, -1.0);
                    Q_SetEntry(&L[Level], Row, 4, Row + Dim1[Level], -1.0);
                }
            }
            Row = RowBasis + Dim1[Level];
            Q_SetLen(&L[Level], Row, 4);
            if (LASResult() == LASOK) {
                Q_SetEntry(&L[Level], Row, 0, Row, 4.0);
                Q_SetEntry(&L[Level], Row, 1, Row - 1, -1.0);
                Q_SetEntry(&L[Level], Row, 2, Row - Dim1[Level], -1.0);
                Q_SetEntry(&L[Level], Row, 3, Row + Dim1[Level], -1.0);
            }
        }
        BlockRow = Dim1[Level];
        RowBasis = Dim1[Level] * (BlockRow - 1);
        Row = RowBasis + 1;
        Q_SetLen(&L[Level], Row, 3);
        if (LASResult() == LASOK) {
            Q_SetEntry(&L[Level], Row, 0, Row, 4.0);
            Q_SetEntry(&L[Level], Row, 1, Row - Dim1[Level], -1.0);
            Q_SetEntry(&L[Level], Row, 2, Row + 1, -1.0);
        }
        for (Row = RowBasis + 2; Row < RowBasis + Dim1[Level]; Row++) {
            Q_SetLen(&L[Level], Row, 4);
            if (LASResult() == LASOK) {
                Q_SetEntry(&L[Level], Row, 0, Row,  4.0);
                Q_SetEntry(&L[Level], Row, 1, Row - 1, -1.0);
                Q_SetEntry(&L[Level], Row, 2, Row - Dim1[Level], -1.0);
                Q_SetEntry(&L[Level], Row, 3, Row + 1, -1.0);
            }
        }
        Row = RowBasis + Dim1[Level];
        Q_SetLen(&L[Level], Row, 3);
        if (LASResult() == LASOK) {
            Q_SetEntry(&L[Level], Row, 0, Row,  4.0);
            Q_SetEntry(&L[Level], Row, 1, Row - 1, -1.0);
            Q_SetEntry(&L[Level], Row, 2, Row - Dim1[Level], -1.0);
        }

#endif /* SYM_STOR */

    }
}

static void Genf2DDirich(int Level, size_t *Dim1)
/* generation of right hand side for 2D case, Dirichlet boundary condition */
{
    double h, hh;
    size_t Block, IndBasis, Ind;

    if (LASResult() == LASOK) {
        /* computing of grid parameter */
        h = 1.0 / (double)(Dim1[Level] + 1);
        hh = pow(h, 2.0);

        for (Block = 1; Block <= Dim1[Level]; Block++) {
            IndBasis = Dim1[Level] * (Block - 1);
            for (Ind = IndBasis + 1; Ind <= IndBasis + Dim1[Level]; Ind++) {
                V_SetCmp(&fr[Level], Ind, 1.0 * hh);
            }
        }
    }
}

static void GenRSimple2DDirich(int Level, size_t *Dim1)
/* generation of simple restriction operator (as matrix) for 2D case,
   Dirichlet boundary condition */
{
    size_t BlockRow, RowBasis, RowInBlock, Row;

    if (LASResult() == LASOK) {
        for (BlockRow = 1; BlockRow <= Dim1[Level]; BlockRow++) {
            RowBasis = Dim1[Level] * (BlockRow - 1);
            for (RowInBlock = 1; RowInBlock <= Dim1[Level]; RowInBlock++) {
                Row = RowBasis + RowInBlock;
                M_SetLen(&R[Level], Row, 1);
                if (LASResult() == LASOK) {
                    M_SetEntry(&R[Level], Row, 0, 2 * RowInBlock 
                        + (2 * BlockRow - 1) * Dim1[Level + 1], 4.0);
                }
            }
        }
    }
}

static void GenRWeight2DDirich(int Level, size_t *Dim1)
/* generation of weighted restriction operator (as matrix) for 2D case,
   Dirichlet boundary condition */
{
    size_t BlockRow, RowBasis, RowInBlock, Row;

    if (LASResult() == LASOK) {
        for (BlockRow = 1; BlockRow <= Dim1[Level]; BlockRow++) {
            RowBasis = Dim1[Level] * (BlockRow - 1);
            for (RowInBlock = 1; RowInBlock <= Dim1[Level]; RowInBlock++) {
                Row = RowBasis + RowInBlock;
                M_SetLen(&R[Level], Row, 9);
                if (LASResult() == LASOK) {
                    M_SetEntry(&R[Level], Row, 0, 2 * RowInBlock
                        + (2 * BlockRow - 2) * Dim1[Level + 1] - 1, 0.25);
                    M_SetEntry(&R[Level], Row, 1, 2 * RowInBlock 
                        + (2 * BlockRow - 2) * Dim1[Level + 1], 0.5);
                    M_SetEntry(&R[Level], Row, 2, 2 * RowInBlock
                        + (2 * BlockRow - 2) * Dim1[Level + 1] + 1, 0.25);
                    M_SetEntry(&R[Level], Row, 3, 2 * RowInBlock
                        + (2 * BlockRow - 1) * Dim1[Level + 1] - 1, 0.5);
                    M_SetEntry(&R[Level], Row, 4, 2 * RowInBlock
                        + (2 * BlockRow - 1) * Dim1[Level + 1], 1.0);
                    M_SetEntry(&R[Level], Row, 5, 2 * RowInBlock
                        + (2 * BlockRow - 1) * Dim1[Level + 1] + 1, 0.5);
                    M_SetEntry(&R[Level], Row, 6, 2 * RowInBlock
                        + (2 * BlockRow) * Dim1[Level + 1] - 1, 0.25);
                    M_SetEntry(&R[Level], Row, 7, 2 * RowInBlock
                        + (2 * BlockRow) * Dim1[Level + 1], 0.5);
                    M_SetEntry(&R[Level], Row, 8, 2 * RowInBlock
                        + (2 * BlockRow) * Dim1[Level + 1] + 1, 0.25);
                }
            }
        }
    }
}

static void GenP2DDirich(int Level, size_t *Dim1)
/* generation of prolongation operator (as matrix) for 2D case, Dirichlet
   boundary condition */
{
    size_t BlockClm, ClmBasis, ClmInBlock, Clm;

    if (LASResult() == LASOK) {
        for (BlockClm = 1; BlockClm <= Dim1[Level - 1]; BlockClm++) {
            ClmBasis = Dim1[Level - 1] * (BlockClm - 1);
            for (ClmInBlock = 1; ClmInBlock <= Dim1[Level - 1]; ClmInBlock++) {
                Clm = ClmBasis + ClmInBlock;
                M_SetLen(&P[Level], Clm, 9);
                if (LASResult() == LASOK) {
                    M_SetEntry(&P[Level], Clm, 0, 2 * ClmInBlock
                        + (2 * BlockClm - 2) * Dim1[Level] - 1, 0.25);
                    M_SetEntry(&P[Level], Clm, 1, 2 * ClmInBlock
                        + (2 * BlockClm - 2) * Dim1[Level], 0.5);
                    M_SetEntry(&P[Level], Clm, 2, 2 * ClmInBlock
                        + (2 * BlockClm - 2) * Dim1[Level] + 1, 0.25);
                    M_SetEntry(&P[Level], Clm, 3, 2 * ClmInBlock
                        + (2 * BlockClm - 1) * Dim1[Level] - 1, 0.5);
                    M_SetEntry(&P[Level], Clm, 4, 2 * ClmInBlock
                        + (2 * BlockClm - 1) * Dim1[Level], 1.0);
                    M_SetEntry(&P[Level], Clm, 5, 2 * ClmInBlock
                        + (2 * BlockClm - 1) * Dim1[Level] + 1, 0.5);
                    M_SetEntry(&P[Level], Clm, 6, 2 * ClmInBlock
                        + (2 * BlockClm) * Dim1[Level] - 1, 0.25);
                    M_SetEntry(&P[Level], Clm, 7, 2 * ClmInBlock
                        + (2 * BlockClm) * Dim1[Level], 0.5);
                    M_SetEntry(&P[Level], Clm, 8, 2 * ClmInBlock
                        + (2 * BlockClm) * Dim1[Level] + 1, 0.25);
                }
            }
        }
    }
}

static void IterStatus(int Iter, double rNorm, double bNorm, IterIdType IterId)
/* put out accuracy after each multigrid iteration */
{
    if (IterId == MGIterId || IterId == MGPCGIterId || IterId == BPXPCGIterId) {
        printf("%3d. iteration ... accuracy = ", Iter);
        if (!IsZero(bNorm))
            printf("%11.4e\n", rNorm / bNorm);
	else
            printf("    ---\n");
    }
}
