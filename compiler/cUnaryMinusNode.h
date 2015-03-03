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

class cUnaryMinusNode : public cExprNode
{
  public:
    cUnaryMinusNode(cExprNode *expr)
    {
        mExpr = expr;
    }

    // determine type based on types of operands
    // Normal promotion rules apply
    virtual cDeclNode *GetType()
    {
        return mExpr->GetType();
    }

    virtual std::string toString()
    {
        std::string result("(EXPR: - ");
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
        EmitInt(NEG_OP);
    }
  protected:
    cExprNode *mExpr;           // expression
};

