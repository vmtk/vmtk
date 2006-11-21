/****************************************************************************/
/*                                xstring.c                                 */
/****************************************************************************/
/*                                                                          */
/* eXtension of standard STRING.h                                           */
/*                                                                          */
/* Copyright (C) 1992-1994 Tomas Skalicky. All rights reserved.             */
/*                                                                          */
/****************************************************************************/

#include <stddef.h>
#include <string.h>
#include <ctype.h>

#include "xc/xtypes.h"

char *str2lwr(char *String)
/* transforms string characters into lower cases */
{
    char *PtrChar;

    if (String != NULL) {
        PtrChar = String;
        while (*PtrChar != '\0') {
	    *PtrChar = (char)tolower(*PtrChar);
            PtrChar++;
        }
    }

    if (String != NULL)
        return(String);
    else
        return(NULL);
}

char *str2upr(char *String)
/* transforms string characters into upper cases */
{
    char *PtrChar;

    if (String != NULL) {
        PtrChar = String;
        while (*PtrChar != '\0') {
	    *PtrChar = (char)toupper(*PtrChar);
            PtrChar++;
        }
    }

    if (String != NULL)
        return(String);
    else
        return(NULL);
}


char *strcatchar(char *String, char Ch)
/* concatenates a character on a string */
{
    size_t Len;

    if (String != NULL) {
        Len = strlen(String);
        String[Len] = Ch;
        String[Len + 1] = '\0';
    }

    if (String != NULL)
        return(String);
    else
        return(NULL);
}
