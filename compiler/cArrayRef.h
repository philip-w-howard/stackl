#pragma once

#include <string>

#include "cVarRef.h"
#include "cSymbol.h"

class cArrayRef : public cVarRef
{
  public:
    cArrayRef(cExpr *base, cExpr *index) : cVarRef() 
    {
        cTypeDecl *baseType = base->GetType();

        if (!baseType->IsPointer() && !base->IsArray())
        {
            ThrowSemanticError("Attempted to index a non-array");
            return;
        }

        baseType = baseType->ParentType();

        AddChild(base);
        AddChild(index);
    }

    virtual std::string toString()
    {
        return GetBase()->toString() + " [ " + GetIndex()->toString() + " ] ";
    }

    virtual void GenerateAddress()
    {
        cVarRef *var = dynamic_cast<cVarRef*>(GetBase());
        if (var == NULL) 
        {
            fatal_error("Generating address for cArrayRef "
                        "without an underlying cVarRef");
            return;
        }

        if (GetType()->IsPointer())
            var->GenerateCode();
        else
            var->GenerateAddress();

        GetIndex()->GenerateCode();
        if (GetType()->Size() == 1)
        {
            EmitInt(PLUS_OP);
        } else {
            EmitInt(PUSH_OP);
            EmitInt(GetType()->Size());
            EmitInt(TIMES_OP);
            EmitInt(PLUS_OP);
        }

    }
    virtual void GenerateCode()
    {
        GenerateAddress();
        if (GetType()->Size() == 1)
            EmitInt(PUSHCVARIND_OP);
        else
            EmitInt(PUSHVARIND_OP);
    }


    cExpr* GetBase()                { return (cExpr*)GetChild(0); }
    virtual cTypeDecl *GetType()    { return GetBase()->GetType(); }
    cExpr* GetIndex()               { return (cExpr*)GetChild(1); }
};

