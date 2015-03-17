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

#include "cStmtNode.h"
#include "cExprNode.h"
#include "codegen.h"

class cForNode : public cStmtNode
{
  public:
    cForNode(cStmtNode *init, cExprNode *expr, cStmtNode *update, 
            cStmtNode *stmt) : cStmtNode()
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
        SetJumpDest(start_loop);
        mExpr->GenerateCode();
        EmitInt(JUMPE_OP);
        SetJumpSource(end_loop);
        mStmt->GenerateCode();
        mUpdate->GenerateCode();
        EmitInt(JUMP_OP);
        SetJumpSource(start_loop);
        SetJumpDest(end_loop);
    }

  protected:
    cStmtNode *mInit;       // initialize the loop variable
    cExprNode *mExpr;       // conditional expression
    cStmtNode *mUpdate;     // update prior to }
    cStmtNode *mStmt;       // statement to execute while true
};

