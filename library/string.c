#pragma once

int strlen(char *str)
{
    int count;
    count = 0;
    while (*str != 0)
    {
        count++;
        str++;
    }

    return count;
}

char * strcpy(char *dest, char *src)
{
    char *dst;
    dst = dest;
    while (*src != 0)
    {
        *dst = *src;
        dst++; 
        src++;
    }
    *dst = 0;

    return dest;
}

char * strcat(char *dest, char *src)
{
    char *dst;

    dst = dest;
    while (*dst++) {}

    dst--;

    while (*src)
    {
        *dst++ = *src++;
    }

    *dst++ = 0;

    return dest;
}

char * strncpy(char *dest, char *src, int size)
{
    char *dst;
    dst = dest;
    int count;
    count = 0;
    while (*src != 0 && count < size)
    {
        *dst = *src;
        dst++;
        src++;
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
    while (*haystack != needle && *haystack)
    {
        haystack++;
    }

    if (*haystack != needle) return 0;

    return haystack;
}

int strcmp(char *str1, char *str2)
{
    int index;
    int result;
    while (*str1 != 0 || *str2 != 0)
    {
        result = *str1 - *str2;
        if (result != 0) return result;
        str1++;
        str2++;
    }
    return 0;
}

char *strrev(char *str)
{
    char *start;
    char *end;
    int  temp;

    if (str == (char *)0) return 0;

    end = str;
    start = str;

    // find the end of the string
    while (*end)
    {
        end++;
    }
    end--;

    while (start < end)
    {
        temp = *start;
        *start = *end;
        *end = temp;
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
        *ptr = (value % 10) + '0';
        ptr++;
        value /= 10;
    }

    if (minus) 
    {
        *ptr = '-';
        ptr++;
    }
    *ptr = 0;

    strrev(str);
    return str;
}

char *xtostr(int value, char *str)
{
    char *ptr;
    int minus;
    int digit;

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
        digit = value & 0x000F;
        if (digit < 10)
            *ptr = digit + '0';
        else
            *ptr = digit + 'A' - 10;
        ptr++;
        value >>= 4;
    }

    if (minus) 
    {
        *ptr = '-';
        ptr++;
    }
    *ptr = 0;

    strrev(str);
    return str;
}

void *memcpy(void *dest, void *source, int size)
{
    char *src;
    char *dst;
    dst = (char *)dest;
    src = (char *)source;

    while (size-- != 0)
    {
        *dst = *src;
        dst++; 
        src++;
    }

    return dest;
}

void *memset(void *buff, int val, int size)
{
    char *dst;
    dst = (char *)buff;

    while (size-- != 0)
    {
        *dst = val;
        dst++; 
    }

    return buff;
}
