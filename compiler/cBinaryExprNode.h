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
        cDeclNode *intType   = symbolTableRoot->Lookup("int")->GetType();

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
        // TODO: don't hardcode the values for AND and OR ops
        if(mOperator == 281)
        {
            // get a label to jump to
            std::string jmp = GenerateLabel();

            // at a minimum, we want to output the left expression
            mLeftExpr->GenerateCode();

            // if the result of the left expression left a 0
            // on the stack, skip the right expression
            EmitInt(DUP_OP);
            EmitInt(JUMPE_OP);
            SetJumpSource(jmp);

            // generate the code for the right expression
            mRightExpr->GenerateCode();

            // generate the code for the operator
            EmitInt(IntOp());

            // jump to the end if the left expression was false
            SetJumpDest(jmp);
        }
        else if(mOperator == 282)
        {
            // get a label to jump to
            std::string skipExpression = GenerateLabel();
            std::string checkExpression = GenerateLabel();

            // at a minimum, we want to output the left expression
            mLeftExpr->GenerateCode();
            EmitInt(DUP_OP);
            EmitInt(JUMPE_OP);
            SetJumpSource(checkExpression);
            EmitInt(JUMP_OP);
            SetJumpSource(skipExpression);
            SetJumpDest(checkExpression);
            mRightExpr->GenerateCode();
            EmitInt(IntOp());
            SetJumpDest(skipExpression);
        }
        else
        {
            mLeftExpr->GenerateCode();
            mRightExpr->GenerateCode();
            EmitInt(IntOp());
        }
    }
  protected:
    cExprNode *mLeftExpr;       // left expression
    cExprNode *mRightExpr;      // right expression
    int mOperator;              // operator: '+', '-', '*', '/', '%'
    std::string StrOp();
    int IntOp();
};

