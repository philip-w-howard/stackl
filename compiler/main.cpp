//*******************************************************
// Purpose: main program for lang compiler
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "lex.h"
#include "codegen.h"

extern cAstNode *yyast_root;

char Input_File[200] = "";
int  Do_Debug = 0;
int  Do_Ast = 0;

void Process_Args(int argc, char **argv)
{
    for (int ii=1; ii<argc; ii++)
    {
        if (argv[ii][0] == '-')
        {
            char *arg = &argv[ii][1];
            if (strcmp(arg, "debug") == 0)
                Do_Debug = 1;
            else if (strcmp(arg, "ast") == 0)
                Do_Ast = 1;
            else
                std::cerr << "Unrecognized option " << argv[ii] << std::endl;
        }
        else
        {
            // assume input file name
            strcpy(Input_File, argv[ii]);
        }
    }
}

int main(int argc, char **argv)
{
    std::cout << "stacklc Beta " << __DATE__ << " " << __TIME__ << std::endl;

    char outfile_name[200];
    int result = 0;

    Process_Args(argc, argv);

//    std::streambuf *cout_buf = std::cout.rdbuf();

    if (strlen(Input_File) == 0)
    {
        std::cerr << "Must specify an input file." << std::endl;
        exit(-1);
    }

    yyin = fopen(Input_File, "r");
    if (yyin == NULL)
    {
        std::cerr << "ERROR: Unable to open file " << Input_File << std::endl;
        exit(-1);
    }


    // redirect cout to the output file
    //std::cerr.rdbuf(output.rdbuf());

    symbolTableRoot = cSymbolTable::CreateDefaultTable();
    result = yyparse();
    if (result == 0 && yynerrs == 0)
    {
        if (yyast_root == NULL)
        {
            semantic_error("Unknown compiler error ");
            exit(-2);
        }

        //std::cout << yyast_root->toString() << std::endl;
        yyast_root->ComputeOffsets(0);
        if (Do_Ast)
        {
            strcpy(outfile_name, Input_File);
            char *ptr = strrchr(outfile_name, '.');
            if (ptr != NULL) *ptr = 0;
            strcat(outfile_name, ".ast");
            std::ofstream output(outfile_name);
            if (!output.is_open())
            {
                std::cerr << "ERROR: Unable to open file " << 
                    outfile_name << std::endl;
                exit(-1);
            }

            output << yyast_root->toString() << std::endl;
            output.close();
        }

        strcpy(outfile_name, Input_File);
        char *ptr = strrchr(outfile_name, '.');
        if (ptr != NULL) *ptr = 0;
        strcat(outfile_name, ".sl");
        InitOutput(outfile_name);
        yyast_root->GenerateCode();
        FinalizeOutput();
    } else {
        std::cerr << yynerrs << " Errors in compile" << std::endl;
    }

    return result;
}
