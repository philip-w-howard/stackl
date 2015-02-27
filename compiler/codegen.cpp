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
#include <string>
#include "codegen.h"

// Output files
static FILE *Output = NULL;
static int  Next_Label = 1;

//*****************************************
// Open output files and write the prefix
bool InitOutput(const char *outfileName)
{
    Output = fopen(outfileName, "w");
    if (Output == NULL)
    {
        std::cerr << "Unable to open " << outfileName << 
           " for output." << std::endl;
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
    fclose(Output);
    return true;
}
//*****************************************
// write a string to the output
void EmitString(std::string str)
{
    fprintf(Output, "%s", str.c_str());
}
//*****************************************
// write an integer constant to the output
void EmitInt(int val)
{
    fprintf(Output, "\n%d", val);
}
//*****************************************
// write a floating point constant to the output
void EmitFloat(double val)
{
    fprintf(Output, "%f", val);
}
//*****************************************
// write a print statement where the value to be printed is in Temp
void EmitPrintTemp()
{
    fprintf(Output, "printf(\"%%d\\n\", Temp);\n");
}
//*****************************************
// write a print statement where the value to be printed is in Temp
void EmitPrintTemp_F()
{
    fprintf(Output, "printf(\"%%6.4f\\n\", Temp_F);\n");
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
// Generate code to push the Temp register
void EmitPushTemp()
{
    EmitString("(*(int *)&Memory[Stack_Pointer]) = Temp;\n");
    EmitString("Stack_Pointer += 4;\n");
}
//*****************************************
// Generate code to push the Temp_F register
void EmitPushTemp_F()
{
    EmitString("(*(double *)&Memory[Stack_Pointer]) = Temp_F;\n");
    EmitString("Stack_Pointer += 8;\n");
}
//*****************************************
// Generate code to push the Frame_Pointer
void EmitPushFP()
{
    EmitString("(*(int *)&Memory[Stack_Pointer]) = Frame_Pointer;\n");
    EmitString("Stack_Pointer += 4;\n");
}
//*****************************************
// Generate code to pop the Frame_Pointer
void EmitPopFP()
{
    EmitString("Stack_Pointer -= 4;\n");
    EmitString("Frame_Pointer = (*(int *)&Memory[Stack_Pointer]);\n");
}
