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

    virtual void GenerateCode()
    {
        GetLeft()->GenerateCode();
        GetRight()->GenerateCode();
        EmitInt(OpAsInt());
    }

    cExpr* GetLeft()    { return (cExpr*)GetChild(0); }
    cExpr* GetRight()   { return (cExpr*)GetChild(1); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

    std::string OpToString();
    int OpAsInt();
  protected:
    int   mOp;

};

