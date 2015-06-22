#pragma once

#include <string>

#include "cExpr.h"
#include "cAssignExpr.h"
#include "cSymbol.h"

class cPrefixExpr : public cExpr
{
  public:
    cPrefixExpr(int op, cExpr *expr) : cExpr() 
    {
        if (expr->IsConst()) 
        {
            ThrowSemanticError("attempted to alter a const expression");
            return;
        }

        if (!expr->IsLval()) 
        {
            ThrowSemanticError("attempted to alter a non-Lval");
            return;
        }
        mExpr = expr;
        mOp = op;
    }

    virtual cTypeDecl *GetType() { return mExpr->GetType(); }

    std::string OpToString()
    {
        switch (mOp)
        {
            case '+':
                return "++";
            case '-':
                return "--";
            default:
                fatal_error("Unrecognized postfix operator");
                break;
        }

        return "unrecognized postfix op ";
    }

    virtual std::string toString()
    {
       return "(" + OpToString() + mExpr->toString() + ")";
    }

    virtual int ComputeOffsets(int base)
    {
        return mExpr->ComputeOffsets(base);
    }

    virtual void GenerateCode() 
    {
        cVarRef *var = dynamic_cast<cVarRef*>(mExpr);
        if (var == NULL)
        {
            fatal_error("Generating code for cPrefixExpr without underlying cVarRef");
            return;
        }

        mExpr->GenerateCode();
        cBinaryExpr *performOp = new cBinaryExpr(mExpr, mOp, new cIntExpr(1));
        performOp->GenerateCode();
        var->GenerateAddress();
        if (var->GetType()->Size() == 1)
            EmitInt(POPCVARIND_OP);
        else
            EmitInt(POPVARIND_OP);
    }

  protected:
    cExpr *mExpr;
    int   mOp;
};

