#pragma once
//*******************************************************
// Purpose: Class for a print char statement
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>

#include "cStmtNode.h"
#include "cExprNode.h"
#include "codegen.h"

class cPrintcNode : public cStmtNode
{
  public:
    cPrintcNode(cExprNode *expr) : cStmtNode()
    {
        mExpr = expr;
    }

    virtual std::string toString()
    {
        std::string result("PRINTC: ");
        result += mExpr->toString();
        return result;
    }

    virtual int ComputeOffsets(int base)
    {
        mExpr->ComputeOffsets(base);
        return base;
    }

    virtual void GenerateCode()
    {
        EmitInt(ILLEGAL_OP);
        /*
        mExpr->GenerateCode();
        EmitInt(PRINTC_OP);
        */
    }
  protected:
    cExprNode *mExpr;       // expression to be printed
};

