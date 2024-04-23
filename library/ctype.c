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
    return 0x00 <= c && c <= 0x1f || c == 0x7f;
}
int isdigit(int c)
{
    return '0' <= c && c <= '9';
}
int isgraph(int c)
{
    return 32 < c && c != 0x7f;
}
int islower(int c)
{
    return 'a' <= c && c <= 'z';
}
int isprint(int c)
{
    return 32 <= c && c != 0x7f;
}
int ispunct(int c)
{
    return isprint(c) && !isspace(c) && !isalnum(c);
}
int isspace(int c)
{
    return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
}
int isupper(int c)
{
    return 'A' <= c && c <= 'Z';
}
int tolower(int c)
{
    if (isupper(c))
    {
        return c + 32;
    }
    return c;
}
int toupper(int c)
{
    if (islower(c))
    {
        return c - 32;
    }
    return c;
}
