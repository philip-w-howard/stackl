#pragma once

#include <string>

#include "cExpr.h"
#include "cAssignExpr.h"
#include "cVarRef.h"
#include "cSymbol.h"

class cPostfixExpr : public cExpr
{
  public:
    cPostfixExpr(cExpr *expr, int op) : cExpr() 
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

        AddChild(expr);
        mOp = op;
    }

    virtual cTypeDecl *GetType() { return GetExpr()->GetType(); }

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

    cVarRef* GetExpr()  { return (cVarRef*)GetChild(0); }
    int GetOp()         { return mOp; }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

  protected:
    int   mOp;
};

