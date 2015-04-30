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
        mCode2 = 0;
        mParams = params;
        mTwoWord = false;
    }

    cAsmNode(int code, int code2, cParamNode *params) : cStmtNode()
    {
        mCode = code;
        mCode2 = code2;
        mParams = params;
        mTwoWord = true;
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
        if (mTwoWord) EmitInt(mCode2);
    }
  protected:
    int mCode;              // the opcode to be emitted
    int mCode2;             // 2nd opcode for 2 word instructions
    bool mTwoWord;          // true of 2 word instruction
    cParamNode *mParams;    // items to push onto the stack prior to emitting
                            // the opcode
};

