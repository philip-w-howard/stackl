#pragma once

#include <string>

#include "cVarRef.h"
#include "cSymbol.h"

class cPointerDeref : public cVarRef
{
  public:
    cPointerDeref(cExpr *base) : cVarRef() 
    {
        AddChild(base);

        cTypeDecl *baseType = base->GetType();

        if (!baseType->IsPointer())
        {
            ThrowSemanticError("Attempted to dereference a non-pointer");
            return;
        }
    }

    cExpr* GetBase()             { return (cExpr*)GetChild(0); }
    virtual cTypeDecl* GetType() { return GetBase()->GetType()->ParentType(); }

    virtual void GenerateAddress()
    {
        GetBase()->GenerateCode();
    }

    virtual void GenerateCode()
    {
        GetBase()->GenerateCode();
        if (GetType()->ElementSize() == 1)
        {
            EmitInt(PUSHCVARIND_OP);
        } else {
            EmitInt(PUSHVARIND_OP);
        }
    }

    virtual std::string toString()
    {
        return "( * " + GetBase()->toString() + " )";
    }
};

