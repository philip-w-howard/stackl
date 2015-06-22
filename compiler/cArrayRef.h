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

        mBase = base;
        mBaseType = baseType;
        mIndex = index;
    }

    virtual cTypeDecl *GetType() { return mBaseType; }

    virtual std::string toString()
    {
        return mBase->toString() + " [ " + mIndex->toString() + " ] ";
    }

    virtual void GenerateAddress()
    {
        cVarRef *var = dynamic_cast<cVarRef*>(mBase);
        if (var == NULL) 
        {
            fatal_error("Generating address for cArrayRef "
                        "without an underlying cVarRef");
            return;
        }

        if (mBase->GetType()->IsPointer())
            var->GenerateCode();
        else
            var->GenerateAddress();

        mIndex->GenerateCode();
        if (mBaseType->Size() == 1)
        {
            EmitInt(PLUS_OP);
        } else {
            EmitInt(PUSH_OP);
            EmitInt(mBaseType->Size());
            EmitInt(TIMES_OP);
            EmitInt(PLUS_OP);
        }

    }
    virtual void GenerateCode()
    {
        GenerateAddress();
        if (mBaseType->Size() == 1)
            EmitInt(PUSHCVARIND_OP);
        else
            EmitInt(PUSHVARIND_OP);
    }

  protected:
    cExpr       *mBase;
    cTypeDecl   *mBaseType;
    cExpr       *mIndex;
};
