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
        AddChild(left);
        AddChild(right);
        mOp = op;
        if (GetType() == NULL)
        {
            ThrowSemanticError("Incompatible types in binary operation");
        }
    }

    virtual bool IsConst() { return GetLeft()->IsConst() && GetRight()->IsConst(); }
    virtual int  ConstValue();

    virtual cTypeDecl *GetType();

    virtual std::string toString()
    {
       return "(" + GetLeft()->toString() + OpToString() + GetRight()->toString() + ")";
    }

    virtual void GenerateCode()
    {
        GetLeft()->GenerateCode();
        GetRight()->GenerateCode();
        EmitInt(OpAsInt());
    }

    cExpr* GetLeft()    { return (cExpr*)GetChild(0); }
    cExpr* GetRight()   { return (cExpr*)GetChild(1); }
  protected:
    int   mOp;

    std::string OpToString();
    int OpAsInt();
};

