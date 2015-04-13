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
#include "cParamNode.h"
#include "codegen.h"

class cAsmNode : public cStmtNode
{
  public:
    cAsmNode(int code, cParamNode *params) : cStmtNode()
    {
        mCode = code;
        mParams = params;
    }

    virtual std::string toString()
    {
        std::string result("ASM ");
        result += std::to_string(mCode);
        if (mParams != NULL) result += " " + mParams->toString();
        return result;
    }

    virtual int ComputeOffsets(int base)
    {
        if (mParams != NULL) mParams->ComputeOffsets(base);
        return base;
    }

    virtual void GenerateCode()
    {
        if (mParams != NULL) mParams->GenerateCode();
        EmitInt(mCode);
    }
  protected:
    int mCode;              // the opcode to be emitted
    cParamNode *mParams;    // items to push onto the stack prior to emitting
                            // the opcode
};

