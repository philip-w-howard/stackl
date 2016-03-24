#include <stdio.h>
#include <string.h>

#include "formatstr.h"

char *format_string(char *str)
{
    char *sptr;

    sptr = strrchr(str, '"');
    if (sptr == NULL)
    {
        fprintf(stderr, "File format error: invalid string\n");
        return NULL;
    }

    // eliminate trailing "
    *sptr = 0;

    sptr = strchr(str, '"');
    if (sptr == NULL)
    {
        fprintf(stderr, "File format error: invalid string\n");
        return NULL;
    }

    // eliminate leading "
    sptr++;

    // Handle escape sequences
    str = sptr;
    sptr = strchr(str, '\\');
    while (sptr != NULL)
    {
        if (sptr[1] == 'n')
        {
            *sptr = '\n';
            sptr++;
            *sptr = 0;
            sptr++;
            // can't user strcat because dest overlaps with src
            //strcat(str, sptr);
            memmove(str+strlen(str), sptr, strlen(sptr)+1);
        } else {
            fprintf(stderr, "Unrecognized escape sequence in string\n");
            sptr += 2;
        }
        sptr = strchr(sptr, '\\');
    }

    return str;
}

