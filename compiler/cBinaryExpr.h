#pragma once

#include <string>

#include "cExpr.h"
#include "cSymbol.h"

#include "codegen.h"

class cBinaryExpr : public cExpr
{
  public:
    cBinaryExpr(cExpr *left, int op, cExpr *right) : cExpr() 
    {
        mLeft = left;
        mRight = right;
        mOp = op;
        if (GetType() == NULL)
        {
            ThrowSemanticError("Incompatible types in binary operation");
        }
    }

    virtual bool IsConst() { return mLeft->IsConst() && mRight->IsConst(); }
    virtual int  ConstValue();

    virtual cTypeDecl *GetType();

    virtual std::string toString()
    {
       return "(" + mLeft->toString() + OpToString() + mRight->toString() + ")";
    }

    virtual void GenerateCode()
    {
        mLeft->GenerateCode();
        mRight->GenerateCode();
        EmitInt(OpAsInt());
    }

  protected:
    cExpr *mLeft;
    cExpr *mRight;
    int   mOp;

    std::string OpToString();
    int OpAsInt();
};

