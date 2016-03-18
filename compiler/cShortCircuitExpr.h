#pragma once
//*******************************************************
// Purpose: Class for short circuiting binary expressions
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>

#include "cExpr.h"
#include "cBinaryExpr.h"
#include "codegen.h"

class cShortCircuitExpr : public cBinaryExpr
{
  public:
    cShortCircuitExpr(cExpr *left, int op, cExpr *right, bool shortOnTrue) 
        : cBinaryExpr(left, op, right)
    {
        mShortOnTrue = shortOnTrue;
    }

    virtual void GenerateCode()
    {
        if(mShortOnTrue)
        {
            // get a label to jump to
            std::string skipExpression = GenerateLabel();
            std::string checkExpression = GenerateLabel();

            // at a minimum, we want to output the left expression
            GetLeft()->GenerateCode();
            EmitInt(DUP_OP);
            EmitInt(JUMPE_OP);
            SetLabelRef(checkExpression);
            EmitInt(JUMP_OP);
            SetLabelRef(skipExpression);
            SetLabelValue(checkExpression);
            GetRight()->GenerateCode();
            EmitInt(OpAsInt());
            SetLabelValue(skipExpression);
        }
        else
        {
            // get a label to jump to
            std::string jmp = GenerateLabel();

            // at a minimum, we want to output the left expression
            GetLeft()->GenerateCode();

            // if the result of the left expression left a 0
            // on the stack, skip the right expression
            EmitInt(DUP_OP);
            EmitInt(JUMPE_OP);
            SetLabelRef(jmp);

            // generate the code for the right expression
            GetRight()->GenerateCode();

            // generate the code for the operator
            EmitInt(OpAsInt());

            // jump to the end if the left expression was false
            SetLabelValue(jmp);
        }
    }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

  protected:
    bool mShortOnTrue;      // short circuit if left op is true
                            // NOTE: also uses members from cBinaryExpr
};

