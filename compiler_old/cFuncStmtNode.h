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
    cFuncStmtNode(cFuncCallNode *call) : cStmtNode()
    {
        mCall = call;
    }

    virtual std::string toString()
    {
        std::string result("");
        result += mCall->toString();

        return result;
    }

    virtual int ComputeOffsets(int base)
    {
        mCall->ComputeOffsets(base);
        return base;
    }

    virtual void GenerateCode()
    {
        mCall->GenerateCode();
        EmitInt(POP_OP);        // need to throw away the return value
    }
  protected:
    cFuncCallNode *mCall;
};

