#pragma once
//*******************************************************
// Purpose: Class for a function call as a statement
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>

#include "cStmtNode.h"

class cFuncStmtNode : public cStmtNode
{
  public:
    cFuncStmtNode(cVarRefNode *lval, cFuncCallNode *call) : cStmtNode()
    {
        mLval = lval;
        mCall = call;
    }

    virtual std::string toString()
    {
        std::string result("");
        if (mLval != NULL) 
        {
            result += "(ASSIGN: ";
            result += mLval->toString() + " = ";
        }
        result += mCall->toString();
        if (mLval != NULL) result += ")";

        return result;
    }

    virtual int ComputeOffsets(int base)
    {
        if (mLval != NULL) mLval->ComputeOffsets(base);
        mCall->ComputeOffsets(base);
        return base;
    }

    virtual void GenerateCode()
    {
        mCall->GenerateCode();
        if (mLval != NULL)
        {
            mLval->GenerateCode();
            if (mCall->GetType()->IsFloat())
                EmitString(" = Temp_F;\n");
            else
                EmitString(" = Temp;\n");
        }

    }
  protected:
    cVarRefNode   *mLval;
    cFuncCallNode *mCall;
};

