#pragma once

#include <string>

#include "cExpr.h"
#include "cExpr.h"
#include "cVarRef.h"
#include "cSymbol.h"

class cAssignExpr : public cExpr
{
  public:
    cAssignExpr(cExpr* var, cExpr *expr)  : cExpr()
    {
        if (!var->IsLval()) 
        {
            ThrowSemanticError("lval must be a variable");
        } else {
            mVar  = dynamic_cast<cVarRef*>(var);
            assert(mVar != NULL);
        }
        mExpr = expr;
    }

    virtual cTypeDecl *GetType()    { return mVar->GetType(); }

    virtual std::string toString()
    {
        return "assign " + mVar->toString() + " = " + mExpr->toString();
    }

    virtual void GenerateCode()
    {
        mExpr->GenerateCode();

        // Need to dup the result in case the assign is treated as an expr
        EmitInt(DUP_OP);
        mVar->GenerateAddress();
        if (mVar->IsArrayRef() && mVar->GetType()->ElementSize() == 1)
            EmitInt(POPCVARIND_OP);
        else if (mVar->GetType()->Size() == 1)
            EmitInt(POPCVARIND_OP);
        else
            EmitInt(POPVARIND_OP);
    }

  protected:
    cVarRef *mVar;
    cExpr *mExpr;
};

