/****************************************************************************/
/*                                getopts.h                                 */
/****************************************************************************/
/*                                                                          */
/* GET OPTionS from the command line                                        */
/*                                                                          */
/* Copyright (C) 1992-1994 Tomas Skalicky. All rights reserved.             */
/*                                                                          */
/****************************************************************************/

#ifndef GETOPTS_H
#define GETOPTS_H

#include <stddef.h>

typedef enum {
    BoolOptType,
    CharOptType,
    IntOptType,
    SizeOptType,
    StrOptType
} OptType; /* option type */

typedef struct {
    char KeyChar;
    OptType Type;
    void *Variable;
} OptDescrType; /* option description type */

typedef struct {
    int No;
    OptDescrType *Descr;
} OptTabType; /* options table type */

typedef enum {
    OptOK,
    OptNotDefErr,
    OptSyntaxErr,
    OptDescrErr
} OptErrType; /* error type for options analysis */

void GetOpts(int *argc, char **argv, OptTabType *OptTab);
void SetMaxStrLen(size_t Len);
OptErrType OptResult(void);

#endif /* GETOPTS_H */
