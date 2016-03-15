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
        AddChild(expr);
    }

    virtual cTypeDecl *GetType() { return GetExpr()->GetType(); }

    virtual std::string toString()
    {
       return "( & " + GetExpr()->toString() + ")";
    }
    virtual void GenerateCode() 
    {
        cVarRef *var = dynamic_cast<cVarRef*>(GetExpr());
        if (var == NULL)
            fatal_error("address of without underlying cVarRef");

        var->GenerateAddress();
    }

    cExpr* GetExpr() { return (cExpr*)GetChild(0); }
};

