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
#include <unistd.h>     // for include path
#include <limits.h>     // for include path

#include "lex.h"
#include "cAstXml.h"
#include "cSizeOffset.h"

#include "../version.h"

extern int yydebug;

char Include_Path[PATH_MAX] = "";

// set the include path to be the same as the directory where the EXE is.
void set_include_path()
{
    char *ptr;

    if (readlink("/proc/self/exe", Include_Path, PATH_MAX) == -1)
    {
        strcpy(Include_Path, "");
    }

    ptr = strrchr(Include_Path, '/');
    if (ptr != NULL)
    {
        ptr++;
        *ptr = 0;
    }
}

extern cDeclsList *yyast_root;

char Input_File[200] = "";
int  Do_Debug = 0;
int  Do_Ast = 0;
int  Do_Assembler = 1;
void Process_Args(int argc, char **argv)
{
    for (int ii=1; ii<argc; ii++)
    {
        if (argv[ii][0] == '-')
        {
            char *arg = &argv[ii][1];
            if (strcmp(arg, "ast") == 0)
                Do_Ast = 1;
            else if (strcmp(arg, "c") == 0)
                Do_Assembler = 0;
            else if (strcmp(arg, "debug") == 0)
                Do_Debug = 1;
            else if (strcmp(arg, "help") == 0)
            {
                std::cout << "stacklc -help -version -yydebug -debug -ast <file>\n";
                exit(0);
            }
            else if (strcmp(arg, "version") == 0)
            {
                std::cout << "stacklc " << VERSION << " " <<__DATE__ << " " 
                    << __TIME__ << std::endl;
                exit(0);
            }
            else if (strcmp(arg, "yydebug") == 0)
                yydebug = 1;
            else
                std::cerr << "Unrecognized option '" << argv[ii] << "'" << std::endl;
        }
        else
        {
            // assume input file name
            strcpy(Input_File, argv[ii]);
        }
    }
}

//****************************************
int process_file(const char *filename, cDeclsList **program, int *total_errors)
{
    int result;

    // KLUDGE: should be handled within the scanner
    yylineno = 1;
    strcpy(yycurrentfile, filename);
    yyin = fopen(filename, "r");
    if (yyin == NULL)
    {
        std::cerr << "ERROR: Unable to open file " << filename << std::endl;
        exit(-1);
    }

    result = yyparse();
    while (yyast_root != NULL)
    {
        *total_errors += yynerrs;
        if (result == 0 && *total_errors == 0)
        {
            if (*program == NULL)
                *program = yyast_root;
            else
                (*program)->AddList(yyast_root);
        }
        result = yyparse();
    }

    fclose(yyin);

    if (result != 0) return result;
    if (*total_errors != 0) return 1;
    return 0;
}

//****************************************
int main(int argc, char **argv)
{
    char outfile_name[200];
    int result = 0;

    set_include_path();
    Process_Args(argc, argv);

//    std::streambuf *cout_buf = std::cout.rdbuf();

    if (strlen(Input_File) == 0)
    {
        std::cerr << "Must specify an input file." << std::endl;
        exit(-1);
    }

    symbolTableRoot = new cSymbolTable();
    symbolTableRoot->InitDefaultTable();
    cDeclsList *program = NULL;
    int total_errors = 0;

    if (result == 0) process_file(Input_File, &program, &total_errors);

    if (result == 0 && total_errors == 0)
    {
        if (program == NULL)
        {
            semantic_error("Unknown compiler error ");
            exit(-2);
        }

        cSizeOffset sizer;
        sizer.VisitAllNodes(program);

        if (Do_Ast)
        {
            strcpy(outfile_name, Input_File);
            char *ptr = strrchr(outfile_name, '.');
            if (ptr != NULL) *ptr = 0;
            strcat(outfile_name, ".ast");

            cAstXml astPrinter(outfile_name);
            astPrinter.VisitAllNodes(program);
        }

        // Create output filename
        strcpy(outfile_name, Input_File);
        char *ptr = strrchr(outfile_name, '.');
        if (ptr != NULL) *ptr = 0;
        strcat(outfile_name, ".sl");

        // coder must be in its own scope so it goes out of scope
        // and gets destructed
        {
            cCodeGen coder(outfile_name);
            coder.VisitAllNodes(program);
        }

        if (Do_Assembler)
        {
            string makebin("slasm ");
            makebin += outfile_name;

            int result = system(makebin.c_str());

            if (result < 0) fatal_error("Error creating binary output");
        }

    } else {
        std::cerr << total_errors << " Errors in compile" << std::endl;
    }

    return result;
}
