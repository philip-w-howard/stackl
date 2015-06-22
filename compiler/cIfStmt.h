#pragma once
//*******************************************************
// Purpose: Class for an if statement
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

class cIfStmt : public cStmt
{
  public:
    cIfStmt(cExpr *expr, cStmt *ifPart, cStmt *elsePart) 
        : cStmt()
    {
        mExpr = expr;
        mStmt = ifPart;
        mElse = elsePart;
    }

    virtual std::string toString()
    {
        std::string result("(IF: ");
        result += mExpr->toString();
        if (mStmt != NULL) result += "\n" + mStmt->toString();
        if (mElse != NULL) result += "\nELSE:\n" + mElse->toString();
        result += "\n)";
        return result;
    }

    virtual int ComputeOffsets(int base)
    {
        mExpr->ComputeOffsets(base);
        if (mStmt != NULL) mStmt->ComputeOffsets(base);
        if (mElse != NULL) mElse->ComputeOffsets(base);
        return base;
    }

    virtual void GenerateCode()
    {
        std::string if_label = GenerateLabel();
        mExpr->GenerateCode();
        EmitInt(JUMPE_OP);
        SetLabelRef(if_label);
        if (mStmt != NULL) mStmt->GenerateCode();

        if (mElse != NULL)
        {
            std::string else_label = GenerateLabel();
            EmitInt(JUMP_OP);
            SetLabelRef(else_label);
            SetLabelValue(if_label);
            mElse->GenerateCode();
            SetLabelValue(else_label);
        }
        else
        {
            SetLabelValue(if_label);
        }
    }

  protected:
    cExpr *mExpr;       // conditional expression
    cStmt *mStmt;       // statements for if cond is true
    cStmt *mElse;       // optional statements for if cond is false
};

