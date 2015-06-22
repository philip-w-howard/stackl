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

class cForStmt : public cStmt
{
  public:
    cForStmt(cExpr *init, cExpr *expr, cExpr *update, cStmt *stmt) : cStmt()
    {
        mInit = init;
        mExpr = expr;
        mUpdate = update;
        mStmt = stmt;
    }

    virtual std::string toString()
    {
        std::string result("(FOR: (");
        result += mInit->toString() + "; ";
        result += mExpr->toString() + "; ";
        result += mUpdate->toString() + ")\n";
        result += "\n" + mStmt->toString();
        result += "\n)";
        return result;
    }

    virtual int ComputeOffsets(int base)
    {
        if (mInit != NULL) mInit->ComputeOffsets(base);
        if (mExpr != NULL) mExpr->ComputeOffsets(base);
        if (mUpdate != NULL) mUpdate->ComputeOffsets(base);
        if (mStmt != NULL) mStmt->ComputeOffsets(base);
        return base;
    }

    virtual void GenerateCode()
    {
        std::string start_loop = GenerateLabel();
        std::string end_loop = GenerateLabel();

        mInit->GenerateCode();
        EmitInt(POP_OP);            // need to handle VOID
        SetLabelValue(start_loop);
        mExpr->GenerateCode();
        EmitInt(JUMPE_OP);
        SetLabelRef(end_loop);
        mStmt->GenerateCode();
        mUpdate->GenerateCode();
        EmitInt(POP_OP);            // need to handle VOID
        EmitInt(JUMP_OP);
        SetLabelRef(start_loop);
        SetLabelValue(end_loop);
    }

  protected:
    cExpr *mInit;       // initialize the loop variable
    cExpr *mExpr;       // conditional expression
    cExpr *mUpdate;     // update prior to }
    cStmt *mStmt;       // statement to execute while true
};

