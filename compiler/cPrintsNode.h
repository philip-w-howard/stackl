#pragma once
//*******************************************************
// Purpose: Class for a prints statement
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>

#include "cStmtNode.h"
#include "cStringLitNode.h"
#include "codegen.h"

class cPrintsNode : public cStmtNode
{
  public:
    cPrintsNode(cStringLitNode *str) : cStmtNode() , mStr(str)
    {
    }

    virtual std::string toString()
    {
        std::string result("PRINTS: ");
        result += mStr->toString();
        return result;
    }

    virtual int ComputeOffsets(int base)
    {
        return base;
    }

    virtual void GenerateCode()
    {
        EmitInt(PRINTS_OP);
        mStr->GenerateCode();
    }
  protected:
    cStringLitNode *mStr;       // string to be printed
};

