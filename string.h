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
    while (haystack[0] != needle && haystack[0] != 0)
    {
        haystack++;
    }

    if (haystack[0] != needle) return 0;

    return haystack;
}

int strcmp(char *str1, char *str2)
{
    int index;
    int result;
    index = 0;
    while (str1[index] != 0 || str2[index] != 0)
    {
        result = str1[index] - str2[index];
        if (result != 0) return result;
        index++;
    }
    return 0;
}

char *strrev(char *str)
{
    char *start;
    char *end;
    int  temp;

    if (str == 0) return 0;

    end = str;
    start = str;

    // find the end of the string
    while (end[0])
    {
        end++;
    }
    end--;

    while (start < end)
    {
        temp = start[0];
        start[0] = end[0];
        end[0] = temp;
        start++;
        end--;
    }

    return str;
}

char *itostr(int value, char *str)
{
    char *ptr;
    int minus;

    minus = 0;
    ptr = str;
    if (value == 0)
    {
        str[0] = '0';
        str[1] = 0;
        return str;
    }
    
    if (value < 0)
    {
        minus = 1;
        value = -value;
    }

    while (value > 0)
    {
        ptr[0] = (value % 10) + '0';
        ptr++;
        value /= 10;
    }

    if (minus) 
    {
        ptr[0] = '-';
        ptr++;
    }
    ptr[0] = 0;

    strrev(str);
    return str;
}
