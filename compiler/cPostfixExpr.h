#pragma once

#include <string>

#include "cExpr.h"
#include "cAssignExpr.h"
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

    virtual void GenerateCode() 
    {
        cVarRef *var = dynamic_cast<cVarRef*>(GetExpr());
        if (var == NULL)
        {
            fatal_error("Generating code for cPrefixExpr without underlying cVarRef");
            return;
        }

        cBinaryExpr *performOp = new cBinaryExpr(GetExpr(), mOp, new cIntExpr(1));
        performOp->GenerateCode();
        EmitInt(DUP_OP);
        var->GenerateAddress();
        if (var->GetType()->Size() == 1)
            EmitInt(POPCVARIND_OP);
        else
            EmitInt(POPVARIND_OP);
    }

    cExpr* GetExpr()    { return (cExpr*)GetChild(0); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

  protected:
    int   mOp;
};

