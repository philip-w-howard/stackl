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

#include <fstream>
#include <string>
#include "../interp/opcodes.h"

bool InitOutput(const char *filename);
bool FinalizeOutput();
void EmitString(std::string str);
void EmitInt(int val);
std::string GenerateLabel();
