int strlen(char *str)
{
    int count;
    count = 0;
    while (str[0] != 0)
    {
        count++;
        str++;
    }

    return count;
}

char * strcpy(char *dest, char *src)
{
    int count;
    count = 0;
    while (src[count] != 0)
    {
        dest[count] = src[count];
        count++;
    }
    dest[count] = 0;

    return dest;
}

char * strncpy(char *dest, char *src, int size)
{
    int count;
    count = 0;
    while (src[count] != 0 && count < size)
    {
        dest[count] = src[count];
        count++;
    }
    for (count = count; count < size; count++)
    {
        dest[count] = 0;
    }

    return dest;
}

char * strchr(char *haystack, int needle)
{
    while (haystack[0] != needle)
    {
        haystack++;
    }

    return haystack;
}

