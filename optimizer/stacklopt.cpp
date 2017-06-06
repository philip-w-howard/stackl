#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string.h>

using std::vector;
using std::string;

typedef char line_t[128];

typedef struct
{
    line_t line;
    line_t line_copy;
    char *first;
    char *second;
} line_info_t;

const char *DELIMS = " \r\n\t";

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

    vector<line_info_t *> inputList;

    while (fgets(input_line, sizeof(input_line), input) != NULL)
    {
        line_info_t *line = (line_info_t*)malloc(sizeof(line_info_t));
        strcpy(line->line, input_line);
        strcpy(line->line_copy, input_line);
        line->first = strtok(line->line_copy, DELIMS);
        line->second = strtok(NULL, DELIMS);

        inputList.push_back(line);
    }

    fclose(input);

    for (line_info_t* item : inputList)
    {
        std::cout << item->line;
    }

    return 0;
}
