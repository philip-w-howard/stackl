#include <iostream>
#include <stdio.h>

#include "lineitem.h"
#include "sourcefile.h"
#include "optimizations.h"

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

    int count = 1;
    int loops = 0;
    while (count > 0)
    {
        count = 0;
        
        count += process_const(inputList, outputList);
        inputList = outputList;
        outputList.Clear();

        count += process_times_1(inputList, outputList);
        inputList = outputList;
        outputList.Clear();

        count += process_plus_0(inputList, outputList);
        inputList = outputList;
        outputList.Clear();

        count += process_popvar(inputList, outputList);
        inputList = outputList;
        outputList.Clear();

        printf("Finished pass %d\n", ++loops);
    }

    FILE *output;
    char *filename;
    if (argc > 2)
        filename = argv[2];
    else
        filename = (char *)"optimized.sl";

    output = fopen(filename, "w");

    if (output == NULL)
    {
        fprintf(stderr, "Unable to open output file: %s\n", filename);
        return 1;
    }


    while (!inputList.EndOfFile())
    {
        fprintf(output, "%s", inputList.next()->text().c_str());
    }

    return 0;
}
