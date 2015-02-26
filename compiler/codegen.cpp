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

// prefix for generated source file
static const std::string CODE_PREFIX =
    std::string("#include <stdio.h>\n") +
    "#define MEM_SIZE 100000\n" +
    "char Memory[MEM_SIZE];\n" +
    "int Frame_Pointer;\n" +
    "int Stack_Pointer;\n" +
    "int Temp;\n" +
    "double Temp_F;\n" +
    //"int Return_i;\n" +
    //"double Return_f;\n" +
    "int main() { \n" +
    "Frame_Pointer=0;\n" +
    "Stack_Pointer=0;\n";

// postfix for generated source file
static const std::string CODE_POSTFIX =
    "  return 0;\n}\n";

// used to keep track of whether we are outputting to function or main file
// zero means main file, positive indicates nested function depth
static int Is_Swapped_Output = 0;

// Output files
static FILE *Output = NULL;
static FILE *Functions = NULL;
static int  Next_Label = 1;

#define INT_VAL(offset)  (*(int *)(&Memory[(offset)]))
#define FLOAT_VAL(offset)  (*(double *)(&Memory[(offset)]))

//*****************************************
// Open output files and write the prefix
bool InitOutput(const char *outfileName)
{
    Functions = tmpfile();
    if (Functions == NULL)
    {
        std::cerr << "Unable to open tmpfile for functions" << std::endl;
        return false;
    }

    Output = fopen(outfileName, "w");
    if (Output == NULL)
    {
        std::cerr << "Unable to open " << outfileName << 
           " for output." << std::endl;
        return false;
    }

    EmitString(CODE_PREFIX);

    return true;
}
//*****************************************
// output postfix, copy functions to tail of main file and close files
bool FinalizeOutput()
{
    EmitString(CODE_POSTFIX);
    RewriteFunctions();
    fclose(Output);
    fclose(Functions);
    return true;
}
//*****************************************
// Start writing to a function. Functions can be nested, and this function
// accomodates that.
void StartFunctionOutput()
{
    if (!Is_Swapped_Output)
    {
        FILE *temp = Output;
        Output = Functions;
        Functions = temp;
    }
    Is_Swapped_Output++;
}
//*****************************************
// Stop writing a function. This function can handle nested functions
void EndFunctionOutput()
{
    Is_Swapped_Output--;
    if (!Is_Swapped_Output)
    {
        FILE *temp = Output;
        Output = Functions;
        Functions = temp;
    }
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
    fprintf(Output, "%d", val);
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
// Copy the function file to the end of the main output file
void RewriteFunctions()
{
    char buff[1000];

    rewind(Functions);
    while (fgets(buff, sizeof(buff), Functions))
    {
        fputs(buff, Output);
    }
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
