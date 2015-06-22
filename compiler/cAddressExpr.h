#pragma once

#include <string>

#include "cExpr.h"
#include "cSymbol.h"

class cAddressExpr : public cExpr
{
  public:
    cAddressExpr(cExpr *expr) : cExpr() 
    {
        cVarRef *var = dynamic_cast<cVarRef*>(expr);
        if (var == NULL)
            ThrowSemanticError("Took the address of something that isn't a var");
        mExpr = expr;
    }

    virtual cTypeDecl *GetType() { return mExpr->GetType(); }

    virtual std::string toString()
    {
       return "( & " + mExpr->toString() + ")";
    }
    virtual void GenerateCode() 
    {
        cVarRef *var = dynamic_cast<cVarRef*>(mExpr);
        if (var == NULL)
            fatal_error("address of without underlying cVarRef");

        var->GenerateAddress();
    }

  protected:
    cExpr *mExpr;
};

