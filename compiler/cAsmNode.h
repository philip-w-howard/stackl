#pragma once
//*******************************************************
// Purpose: Class for a emitting an assembly statement
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 4/11/2015
//
//*******************************************************

#include <string>

#include "cStmtNode.h"
#include "codegen.h"

class cAsmNode : public cStmtNode
{
  public:
    cAsmNode(int code) : cStmtNode()
    {
        mCode = code;
    }

    virtual std::string toString()
    {
        std::string result("ASM ");
        result += std::to_string(mCode);
        return result;
    }

    virtual int ComputeOffsets(int base)
    {
        return base;
    }

    virtual void GenerateCode()
    {
        EmitInt(mCode);
    }
  protected:
    int mCode;          // the opcode to be emitted
};

