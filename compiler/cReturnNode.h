#pragma once
//*******************************************************
// Purpose: Class for a return statement
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

class cReturnNode : public cStmtNode
{
  public:
    cReturnNode(cExprNode *expr) : cStmtNode()
    {
        mExpr = expr;
    }

    virtual std::string toString()
    {
        std::string result("RETURN: ");
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
        if (mExpr == NULL)
            EmitInt(RETURN_OP);
        else
        {
            mExpr->GenerateCode();
            EmitInt(RETURNV_OP);
        }
    }

  protected:
    cExprNode *mExpr;       // value to be returned
};

