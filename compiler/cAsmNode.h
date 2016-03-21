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

class cAsmNode : public cExpr
{
  public:
    cAsmNode(int code, cParams *params) : cExpr()
    {
        mCode = code;
        mCode2 = 0;
        AddChild(params);
        mTwoWord = false;
    }

    cAsmNode(int code, int code2, cParams *params) : cExpr()
    {
        mCode = code;
        mCode2 = code2;
        AddChild(params);
        mTwoWord = true;
    }

    virtual cTypeDecl *GetType()
    {
        fatal_error("Illegal to call GetType on cAsmNode");
        return NULL;
    }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

    int GetOp1()            { return mCode; }
    int GetOp2()            { return mCode2; }
    bool UsesTwoArgs()      { return mTwoWord; }
    cParams* GetParams()    { return (cParams*)GetChild(0); }

  protected:
    int mCode;              // the opcode to be emitted
    int mCode2;             // 2nd opcode for 2 word instructions
    bool mTwoWord;          // true of 2 word instruction
};

