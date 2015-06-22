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

#include "cExpr.h"
#include "cStmt.h"
#include "cStmtsList.h"
#include "cParams.h"
#include "codegen.h"

class cAsmNode : public cExpr
{
  public:
    cAsmNode(int code, cParams *params) : cExpr()
    {
        mCode = code;
        mCode2 = 0;
        mParams = params;
        mTwoWord = false;
    }

    cAsmNode(int code, int code2, cParams *params) : cExpr()
    {
        mCode = code;
        mCode2 = code2;
        mParams = params;
        mTwoWord = true;
    }

    virtual cTypeDecl *GetType()
    {
        fatal_error("Illegal to call GetType on cAsmNode");
        return NULL;
    }

    virtual std::string toString()
    {
        std::string result("ASM ");
        result += std::to_string(mCode);
        if (mTwoWord) result += " " + std::to_string(mCode2);
        if (mParams != NULL) result += " " + mParams->toString();
        return result;
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
    cParams *mParams;       // items to push onto the stack prior to emitting
                            // the opcode
};

