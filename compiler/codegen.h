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

static const int STACK_FRAME_SIZE = 8;
static const int WORD_SIZE = 4;

bool InitOutput(const char *filename);
bool FinalizeOutput();
void EmitInt(int val);
void EmitString(std::string str);
void EmitActualString(std::string str);
void EmitComment(std::string str);
void EmitFixup(int loc, int dest);
void SetJumpSource(std::string label);
void SetJumpDest(std::string label);
std::string GenerateLabel();
