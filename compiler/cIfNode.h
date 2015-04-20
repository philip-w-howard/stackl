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

#include "cStmtNode.h"
#include "cExprNode.h"
#include "codegen.h"

class cIfNode : public cStmtNode
{
  public:
    cIfNode(cExprNode *expr, cStmtNode *ifPart, cStmtNode *elsePart) 
        : cStmtNode()
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
        SetJumpSource(if_label);
        if (mStmt != NULL) mStmt->GenerateCode();

        if (mElse != NULL)
        {
            std::string else_label = GenerateLabel();
            EmitInt(JUMP_OP);
            SetJumpSource(else_label);
            SetJumpDest(if_label);
            mElse->GenerateCode();
            SetJumpDest(else_label);
        }
        else
        {
            SetJumpDest(if_label);
        }
    }

  protected:
    cExprNode *mExpr;       // conditional expression
    cStmtNode *mStmt;       // statements for if cond is true
    cStmtNode *mElse;       // optional statements for if cond is false
};

