#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>

using std::vector;
using std::string;

int main(int argc, char **argv)
{
    char line[256];
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

    vector<string> inputList;

    while (fgets(line, sizeof(line), input) != NULL)
    {
        inputList.push_back(line);
    }

    fclose(input);

    for (string item : inputList)
    {
        std::cout << item;
    }

    return 0;
}
