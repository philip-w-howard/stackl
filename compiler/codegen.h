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
#include "../interp/machine_def.h"

static const int STACK_FRAME_SIZE = 8;

bool InitOutput(const char *filename);
bool FinalizeOutput();
void EmitInt(int val);
void EmitString(std::string str);
void EmitActualString(std::string str);
void EmitGlobalDef(std::string str, int size);
void EmitGlobalRef(std::string str);
void EmitActualGlobal(std::string str, int size);
void EmitComment(std::string str);
void EmitFixup(int loc, int dest);
void SetLabelRef(std::string label);
void SetLabelValue(std::string label);
std::string GenerateLabel();
