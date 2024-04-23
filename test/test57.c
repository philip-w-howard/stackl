// This tests the ctype.h and stdio.h
#include <ctype.h> // everything else
#include <stdio.h> // puts

void print_properties(int c) {
    if (isalpha(c))
    {
        char *property = "is alpha";
        puts(property);
    }
    else
    {
        char *property = "is not alpha";
        puts(property);
    }
    if (isalnum(c))
    {
        char *property = "is alnum";
        puts(property);
    }
    else
    {
        char *property = "is not alnum";
        puts(property);
    }
    if (islower(c))
    {
        char *property = "is lower";
        puts(property);
    }
    else
    {
        char *property = "is lower";
        puts(property);
    }
    if (isupper(c))
    {
        char *property = "is upper";
        puts(property);
    }
    else
    {
        char *property = "is not upper";
        puts(property);
    }
    if (isblank(c))
    {
        char *property = "is blank";
        puts(property);
    }
    else
    {
        char *property = "is not blank";
        puts(property);
    }
    if (iscntrl(c))
    {
        char *property = "is cntrl";
        puts(property);
    }
    else
    {
        char *property = "is not cntrl";
        puts(property);
    }
    if (isdigit(c))
    {
        char *property = "is digit";
        puts(property);
    }
    else
    {
        char *property = "is not digit";
        puts(property);
    }
    if (isgraph(c))
    {
        char *property = "is graph";
        puts(property);
    }
    else
    {
        char *property = "is not graph";
        puts(property);
    }
    if (isprint(c))
    {
        char *property = "is print";
        puts(property);
    }
    else
    {
        char *property = "is not print";
        puts(property);
    }
    if (ispunct(c))
    {
        char *property = "is punctuation";
        puts(property);
    }
    else
    {
        char *property = "is not punctuation";
        puts(property);
    }
    if (isspace(c))
    {
        char *property = "is space";
        puts(property);
    }
    else
    {
        char *property = "is not space";
        puts(property);
    }
}

int main() {
    int ascii_char = 'a';
    while (ascii_char <= 'z')
    {
        char buff[2];
        buff[0] = (char)toupper(ascii_char);
        buff[1] = 0;
        puts(buff);
        ascii_char++;
    }
    ascii_char = 'A';
    while (ascii_char <= 'Z')
    {
        char buff[2];
        buff[0] = (char)tolower(ascii_char);
        buff[1] = 0;
        puts(buff);
        ascii_char++;
    }
    ascii_char = 0;
    while (ascii_char <= 127)
    {
        print_properties(ascii_char);
        puts("================");
        ascii_char++;
    }
    return 0;
}