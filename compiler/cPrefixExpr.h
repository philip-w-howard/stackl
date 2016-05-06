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
                fatal_error("Unrecognized prefix operator");
                break;
        }

        return "unrecognized prefix op ";
    }

    cVarRef* GetExpr()  { return (cVarRef*)GetChild(0); }
    int GetOp()         { return mOp; }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

  protected:
    int   mOp;
};

