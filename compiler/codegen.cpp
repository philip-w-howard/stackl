//*******************************************************
// Purpose: Utility routines used for code generation
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <iostream>
#include <fstream>
#include <string>
#include "codegen.h"

// Output files
static std::ofstream Output;
static int  Next_Label = 1;

//*****************************************
// Open output files and write the prefix
bool InitOutput(const char *filename)
{
    Output.open(filename);
    if (!Output.is_open())
    {
        std::cerr << "Unable to open output file." << std::endl;
        return false;
    }

    EmitInt(PUSH_OP);
    EmitInt(4);
    EmitInt(CALL_OP);
    EmitInt(HALT_OP);

    return true;
}
//*****************************************
// output postfix, copy functions to tail of main file and close files
bool FinalizeOutput()
{
    Output.close();
    return true;
}
//*****************************************
// write a string to the output
void EmitString(std::string str)
{
    Output << str.c_str();
}
//*****************************************
// write an integer constant to the output
void EmitInt(int val)
{
    Output << "\n" << val;
}
//*****************************************
// Generate a unique label for GOTO statements
std::string GenerateLabel()
{
    Next_Label++;
    std::string label("LABEL_");
    label += std::to_string(Next_Label);
    return label;
}
