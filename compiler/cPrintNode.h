#pragma once
//*******************************************************
// Purpose: Class for a print statement
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
#include "cDeclNode.h"
#include "codegen.h"

class cPrintNode : public cStmtNode
{
  public:
    cPrintNode(cExprNode *expr) : cStmtNode()
    {
        mExpr = expr;
    }

    virtual std::string toString()
    {
        std::string result("PRINT: ");
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
        cDeclNode *type = mExpr->GetType();

        mExpr->GenerateCode();
        if (type->IsAbsolute() || type->IsPointer())
            EmitInt(PRINTSA_OP);
        else if (type->IsArray())
            EmitInt(PRINTSA_OP);
        else
            EmitInt(PRINT_OP);
    }
  protected:
    cExprNode *mExpr;       // expression to be printed
};

