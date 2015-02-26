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

class cWhileNode : public cStmtNode
{
  public:
    cWhileNode(cExprNode *expr, cStmtNode *stmt) : cStmtNode()
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
        mExpr->ComputeOffsets(base);
        mStmt->ComputeOffsets(base);
        return base;
    }

    virtual void GenerateCode()
    {
        std::string start_loop = GenerateLabel();
        std::string end_loop = GenerateLabel();

        EmitString(start_loop + ":\n");
        EmitString("if (!(");
        mExpr->GenerateCode();
        EmitString(")) goto " + end_loop + ";\n");
        mStmt->GenerateCode();
        EmitString("goto " + start_loop + ";\n");
        EmitString(end_loop + ":\n");
    }

  protected:
    cExprNode *mExpr;       // conditional expression
    cStmtNode *mStmt;       // statement to execute while true
};

