#pragma once
#include <ctype.h>

int isalnum(int c)
{
    return isalpha(c) || isdigit(c);
}
int isalpha(int c)
{
    return islower(c) || isupper(c);
}
int isblank(int c)
{
    return c == 32 || c == 9;
}
int iscntrl(int c)
{
    return 0 <= c && c < 32
}
int isdigit(int c)
{
    return 47 < c && c < 58;
}
int isgraph(int c)
{
    return 32 < c;
}
int islower(int c)
{
    return 96 < c && c < 123;
}
int isprint(int c)
{
    return 31 < c;
}
int ispunct(int c)
{
    return !isspace(c) && !isalnum(c);
}
int isspace(int c)
{
    return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
}
int isupper(int c)
{
    return 64 < c && c < 91;
}
int tolower(int c)
{
    if (isupper(c))
    {
        return c - 32;
    }
    return c;
}
int toupper(int c)
{
    if (islower(c))
    {
        return c + 32;
    }
    return c;
}
