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
#include "cFixupTable.h"
#include "../interp/syscodes.h"

// Output files
static std::ofstream Output;
static int  Next_Label = 1;

static cFixupTable FixupTable;
#define WORD_SIZE 4
static int Location = 0;

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

    EmitInt(CALL_OP);
    SetJumpSource("startup__");
    EmitInt(POP_OP);            // need to throw away the return value
    EmitInt(HALT_OP);

    return true;
}
//*****************************************
// output postfix, copy functions to tail of main file and close files
bool FinalizeOutput()
{
    FixupTable.FixupOutput(Output);

    Output << "X\n";
    Output.close();
    return true;
}
//*****************************************
// write an integer constant to the output
void EmitInt(int val)
{
    Output << "D " << val << "\n";
    Location += WORD_SIZE;
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
//*****************************************
void EmitFixup(int loc, int dest)
{
    Output << "F " << loc << " " << dest << "\n";
}
//*****************************************
void EmitString(std::string str)
{
    Output << "S " << str << "\n";
    Location += WORD_SIZE;
}
//*****************************************
void EmitComment(std::string str)
{
    Output << "C " << Location << " " << str << "\n";
}
//*****************************************
void SetJumpSource(std::string label)
{
    FixupTable.AddJumpSource(label, Location);
    EmitInt(-99);
}
//*****************************************
void SetJumpDest(std::string label)
{
    FixupTable.AddJumpDest(label, Location);
}
