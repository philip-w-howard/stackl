#pragma once
//*******************************************************
// Purpose: Class for unary minus expressions
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>

#include "cDeclNode.h"
#include "cExprNode.h"
#include "cSymbolTable.h"
#include "codegen.h"

class cUnaryOpNode : public cExprNode
{
  public:
    cUnaryOpNode(cExprNode *expr, char op)
    {
        mExpr = expr;
        mOp = op;
    }

    // determine type based on types of operands
    // Normal promotion rules apply
    virtual cDeclNode *GetType()
    {
        return mExpr->GetType();
    }

    virtual std::string toString()
    {
        std::string result("(EXPR: ");
        result += mOp;
        result += " ";
        result += mExpr->toString();
        result += ")";

        return result;
    }

    virtual int ComputeOffsets(int base)
    {
        mExpr->ComputeOffsets(base);
        return base;
    }

    virtual void GenerateCode()
    {
        mExpr->GenerateCode();
        if (mOp == '-')
            EmitInt(NEG_OP);
        else if (mOp == '~')
            EmitInt(COMP_OP);
    }
  protected:
    cExprNode *mExpr;           // expression
    char      mOp;              // operator
};

