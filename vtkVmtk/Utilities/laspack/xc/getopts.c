/****************************************************************************/
/*                                getopts.c                                 */
/****************************************************************************/
/*                                                                          */
/* GET OPTionS from command line                                            */
/*                                                                          */
/* Copyright (C) 1992-1994 Tomas Skalicky. All rights reserved.             */
/*                                                                         */
/****************************************************************************/

#include <ctype.h>
#include <string.h>

#include "xc/xtypes.h"
#include "xc/getopts.h"

static size_t MaxStrLen = 16;
static OptErrType OptErr;

static OptDescrType *SearchOpt(char PtrArg, OptTabType *OptTab);
static char *SetOpt(OptDescrType *PtrOptDescr, char *PtrArg);

void GetOpts(int *argc, char **argv, OptTabType *OptTab)
/* get options from command line */
{
    int ArgOrig, ArgNeu;
    char *PtrArg;
    OptDescrType *PtrOptDescr;

    OptErr = OptOK;
    ArgNeu = 0;
    for (ArgOrig = 1; ArgOrig < *argc; ArgOrig++) {
        /* test if switch character found */
        if (argv[ArgOrig][0] == '-' || argv[ArgOrig][0] == '/') {
            /* analyse argument */
            PtrArg = argv[ArgOrig] + 1;
            while (PtrArg[0] != '\0') {
                /* search option description in options table */
                PtrOptDescr = SearchOpt(PtrArg[0], OptTab);
                PtrArg++;
                /* test if option description exists */
                if (PtrOptDescr != NULL) {
                    /* set option resp. assign option argument */
                    PtrArg = SetOpt(PtrOptDescr, PtrArg);
                } else {
                    /* set OptErr */
                    OptErr = OptNotDefErr;
                }
            }
        } else {
            /* copy argument in new argv */
            ArgNeu++;
            argv[ArgNeu] = argv[ArgOrig];
        }
    }
    *argc = ArgNeu + 1;
}

void SetMaxStrLen(size_t Len)
/* set maximal string lenght for string option type */
{
    MaxStrLen = Len;
}

OptErrType OptResult(void)
/* get result of options analysis */
{
    return(OptErr);
}

static OptDescrType *SearchOpt(char SearchChar, OptTabType *OptTab)
/* search option description for a given character */
{
    OptDescrType *PtrOptDescr; /* pointer to option description */

    int Opt; /* option */

    PtrOptDescr = NULL;
    for (Opt = 0; Opt < OptTab->No; Opt++) {
        /* test if option description found */
        if (OptTab->Descr[Opt].KeyChar == SearchChar)
            /* set pointer to this description */
            PtrOptDescr = &(OptTab->Descr[Opt]);
    }
    return(PtrOptDescr);
}

static char *SetOpt(OptDescrType *PtrOptDescr, char *PtrArg)
/* set option or assign option argument to variable in options table */
{
    int IntVal;
    size_t SizeVal;

    switch (PtrOptDescr->Type) {
        case BoolOptType:
            *((Boolean *)PtrOptDescr->Variable) = True;
            break;
        case CharOptType:
            *((char *)PtrOptDescr->Variable) = *PtrArg;
            if (*PtrArg != '\0')
                PtrArg++;
            break;
        case IntOptType:
            while (isspace(*PtrArg))
                PtrArg++;
            IntVal = 0;
            if (!(isdigit(*PtrArg)) || *PtrArg == '\0')
                OptErr = OptSyntaxErr;
            while (isdigit(*PtrArg)) {
                IntVal = 10 * IntVal + (*PtrArg - '0');
                PtrArg++;
            }
            *((int *)PtrOptDescr->Variable) = IntVal;
            break;
        case SizeOptType:
            while (isspace(*PtrArg))
                PtrArg++;
            SizeVal = 0;
            if (!(isdigit(*PtrArg)) || *PtrArg == '\0')
                OptErr = OptSyntaxErr;
            while (isdigit(*PtrArg)) {
                SizeVal = 10 * SizeVal + (*PtrArg - '0');
                PtrArg++;
            }
            *((int *)PtrOptDescr->Variable) = SizeVal;
            break;
        case StrOptType:
            strncpy((char *)PtrOptDescr->Variable, PtrArg, MaxStrLen);
            PtrArg += strlen(PtrArg);
            break;
        default:
            OptErr = OptDescrErr;
            break;
    }
    return(PtrArg);
}
