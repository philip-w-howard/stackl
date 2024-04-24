// This tests the ctype.h and stdio.h
#include <ctype.h> // everything else
#include <sysio.h> // prints

void print_property(char* prop, int hasprop)
{
    prints(prop);
    if (hasprop)
    {
        prints(":1\n");
    }
    else
    {
        prints(":0\n");
    }
}

void print_properties(int c) {
    print_property("isalnum", isalnum(c));
    print_property("isalpha", isalpha(c));
    print_property("isblank", isblank(c));
    print_property("iscntrl", iscntrl(c));
    print_property("isdigit", isdigit(c));
    print_property("isgraph", isgraph(c));
    print_property("islower", islower(c));
    print_property("isprint", isprint(c));
    print_property("ispunct", ispunct(c));
    print_property("isspace", isspace(c));
    print_property("isupper", isupper(c));
}

int main() {
    int ascii_char = 'a';
    while (ascii_char <= 'z')
    {
        char buff[2];
        buff[0] = (char)toupper(ascii_char);
        buff[1] = 0;
        prints(buff);
        prints("\n");
        ascii_char++;
    }
    ascii_char = 'A';
    while(ascii_char <= 'Z')
    {
        char buff[2];
        buff[0] = (char)tolower(ascii_char);
        buff[1] = 0;
        prints(buff);
        prints("\n");
        ascii_char++;
    }
    ascii_char = 0;
    while (ascii_char <= 127)
    {
        print_properties(ascii_char);
        prints("================\n");
        ascii_char++;
    }
    return 0;
}