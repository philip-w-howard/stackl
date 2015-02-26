#pragma once
//*******************************************************
// Purpose: Utility functions for code generation
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>

bool InitOutput(const char *outfileName);
bool FinalizeOutput();
void StartFunctionOutput();
void EndFunctionOutput();
void EmitString(std::string str);
void EmitInt(int val);
void EmitFloat(double val);
void EmitPrintTemp();
void EmitPrintTemp_F();
void EmitPushTemp();
void EmitPushTemp_F();
void EmitPushFP();
void EmitPopFP();
void RewriteFunctions();
std::string GenerateLabel();
