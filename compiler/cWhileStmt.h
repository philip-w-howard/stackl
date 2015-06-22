#pragma once
//*******************************************************
// Purpose: Class for a while statement
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>

#include "cStmt.h"
#include "cExpr.h"
#include "codegen.h"

class cWhileStmt : public cStmt
{
  public:
    cWhileStmt(cExpr *expr, cStmt *stmt) : cStmt()
    {
        mExpr = expr;
        mStmt = stmt;
    }

    virtual std::string toString()
    {
        std::string result("(WHILE: ");
        result += mExpr->toString();
        result += "\n" + mStmt->toString();
        result += "\n)";
        return result;
    }

    virtual int ComputeOffsets(int base)
    {
        base = mExpr->ComputeOffsets(base);
        if (mStmt != NULL) base = mStmt->ComputeOffsets(base);
        return base;
    }

    virtual void GenerateCode()
    {
        std::string start_loop = GenerateLabel();
        std::string end_loop = GenerateLabel();

        SetLabelValue(start_loop);
        mExpr->GenerateCode();
        EmitInt(JUMPE_OP);
        SetLabelRef(end_loop);
        mStmt->GenerateCode();
        EmitInt(JUMP_OP);
        SetLabelRef(start_loop);
        SetLabelValue(end_loop);
    }

  protected:
    cExpr *mExpr;       // conditional expression
    cStmt *mStmt;       // statement to execute while true
};

