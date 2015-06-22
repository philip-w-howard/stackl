#pragma once

#include <string>

#include "cExpr.h"
#include "cSymbol.h"

class cUnaryExpr : public cExpr
{
  public:
    cUnaryExpr(int op, cExpr *expr) : cExpr() 
    {
        mExpr = expr;
        mOp = op;
    }

    virtual bool IsConst() { return mExpr->IsConst(); }
    virtual int  ConstValue()
    {
        switch (mOp)
        {
            case '-':
                return -mExpr->ConstValue();
            case '~':
                return ~mExpr->ConstValue();
            case '!':
                return !mExpr->ConstValue();
            default:
                fatal_error("Unrecognized unary operator");
                break;
        }

        return 0;
    }

    virtual cTypeDecl *GetType() { return mExpr->GetType(); }

    std::string OpToString()
    {
        switch (mOp)
        {
            case '-':
                return "-";
            case '~':
                return "~";
            case '!':
                return "!";
            default:
                fatal_error("Unrecognized unary operator");
                break;
        }

        return "unrecognized unary op ";
    }

    virtual std::string toString()
    {
       return "(" + OpToString() + mExpr->toString() + ")";
    }
    virtual void GenerateCode() 
    {
        mExpr->GenerateCode();

        switch (mOp)
        {
            case '-':
                EmitInt(NEG_OP);
                break;
            case '~':
                EmitInt(COMP_OP);
                break;
            case '!':
                EmitInt(NOT_OP);
                break;
            default:
                fatal_error("Unrecognized unary operator");
                break;
        }

    }

  protected:
    cExpr *mExpr;
    int   mOp;
};

