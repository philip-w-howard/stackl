#pragma once
//*******************************************************
// Purpose: Class for binary expressions
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

class cBinaryExprNode : public cExprNode
{
  public:
    cBinaryExprNode(cExprNode *left, int op, cExprNode *right) : cExprNode()
    {
        mLeftExpr = left;
        mOperator = op;
        mRightExpr = right;
    }

    // determine type based on types of operands
    // Normal promotion rules apply
    virtual cDeclNode *GetType()
    {
        cDeclNode *leftType  = mLeftExpr->GetType();
        cDeclNode *rightType = mRightExpr->GetType();
        cDeclNode *intType   = symbolTableRoot->Lookup("int")->GetDecl();

        if (leftType == rightType) return leftType;
        return intType;
    }

    virtual std::string toString()
    {
        std::string result("(EXPR: ");
        result += mLeftExpr->toString() + " " 
               + StrOp() + " " 
               + mRightExpr->toString();
        result += ")";

        return result;
    }

    virtual int ComputeOffsets(int base)
    {
        mLeftExpr->ComputeOffsets(base);
        mRightExpr->ComputeOffsets(base);
        return base;
    }

    virtual void GenerateCode()
    {
        mLeftExpr->GenerateCode();
        mRightExpr->GenerateCode();
        EmitInt(IntOp());
    }
  protected:
    cExprNode *mLeftExpr;       // left expression
    cExprNode *mRightExpr;      // right expression
    int mOperator;              // operator: '+', '-', '*', '/', '%'
    std::string StrOp();
    int IntOp();
};

