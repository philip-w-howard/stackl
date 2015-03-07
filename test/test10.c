// strcpy
// strncpy
// strchr
//
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

int strcpy(char *dest, char *src)
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

int strncpy(char *dest, char *src, int size)
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

int strchr(char *haystack, int needle)
{
    while (haystack[0] != needle)
    {
        haystack++;
    }

    return haystack;
}

int main()
{
    char *ptr;
    char array[10];
    ptr = "12345";

    print( strlen(ptr) );
    strcpy(array, ptr);
    print( strlen(array) );

    array[6] = '\n';
    array[7] = 0;
    ptr = "this is a test";
    strncpy(array, ptr, 6);
    print(ptr);
    print(array);
    ptr = array;
    print(ptr);

    ptr = "abcdefghij\n";
    ptr = strchr(ptr, 'f');
    print(ptr);
    return 0;
}
