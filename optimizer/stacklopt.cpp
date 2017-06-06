#include <iostream>
#include <stdio.h>

#include "lineitem.h"
#include "sourcefile.h"
#include "popvarind.h"

int main(int argc, char **argv)
{
    char input_line[256];
    if (argc < 2)
    {
        fprintf(stderr, "Must specify an input file\n");
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (input == NULL)
    {
        fprintf(stderr, "Unable to open input file: %s\n", argv[1]);
        return 1;
    }

    SourceFile inputList;

    while (fgets(input_line, sizeof(input_line), input) != NULL)
    {
        inputList.Insert(input_line);
    }

    fclose(input);

    SourceFile outputList;
    process_popvar(inputList, outputList);

    while (!outputList.EndOfFile())
    {
        std::cout << outputList.next()->text();
    }

    return 0;
}
