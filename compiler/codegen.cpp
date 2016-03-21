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
#include <string.h>
#include "codegen.h"
#include "cFixupTable.h"
#include "../interp/syscodes.h"

#error "codegen functionality moved to cCodeGen"

// Output files
static std::ofstream Output;
static int  Next_Label = 1;

static cFixupTable FixupTable;
static int Location = 0;

// moved to cCodeGen.cpp
//const std::string cFixupTable::GlobalLabel = "$global$";

static char Filename[255];
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

    strcpy(Filename, filename);

    // Leave room for ISR address (if any)
    SetLabelRef("interrupt");
    SetLabelRef("systrap");

    EmitInt(JUMP_OP);
    SetLabelRef("startup__");
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

    char makebin[500];
    strcpy(makebin, "makebin ");
    strcat(makebin, Filename);

    int result = system(makebin);

    if (result < 0) return false;

    return true;
}
//*****************************************
// write an integer constant to the output
void EmitInt(int val)
{
    Output << "D " << Location << " " << val << "\n";
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
    std::string label = GenerateLabel();
    SetLabelRef(label);
    FixupTable.FixupAddString(str, label);
    // Output << "S " << str << "\n";
    // Location += WORD_SIZE;
}
//*****************************************
void EmitActualString(std::string str)
{
    int size;
    Output << "S " << Location << " " << str << "\n";
    size = (str.size() + WORD_SIZE)/WORD_SIZE;
    size *= WORD_SIZE;
    Location += size;
}
//*****************************************
void EmitGlobalDef(std::string str, int size)
{
    FixupTable.FixupAddGlobal(str, size);
}
//*****************************************
void EmitGlobalRef(std::string str)
{
    SetLabelRef(cFixupTable::GlobalLabel);
}
//*****************************************
void EmitActualGlobal(std::string str, int size)
{
    Output << "G " << Location << " " << str << " " << size << "\n";
    size = (size + WORD_SIZE - 1)/WORD_SIZE;
    size *= WORD_SIZE;
    Location += size;
}
//*****************************************
void EmitComment(std::string str)
{
    Output << "C " << Location << " " << str << "\n";
}
//*****************************************
void SetLabelRef(std::string label)
{
    FixupTable.AddLabelRef(label, Location);
    EmitInt(-99);
}
//*****************************************
void SetLabelValue(std::string label)
{
    FixupTable.AddLabelValue(label, Location);
}
