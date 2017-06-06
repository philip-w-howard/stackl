#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include "lineitem.h"

using std::vector;
using std::string;

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

    vector<LineItem *> inputList;

    while (fgets(input_line, sizeof(input_line), input) != NULL)
    {
        LineItem *line = new LineItem(input_line);

        inputList.push_back(line);
    }

    fclose(input);

    for (LineItem* item : inputList)
    {
        std::cout << item->text();
    }

    return 0;
}
