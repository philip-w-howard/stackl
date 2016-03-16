#pragma once

#include <string>

#include "cVarRef.h"
#include "cSymbol.h"

class cPointerDeref : public cVarRef
{
  public:
    cPointerDeref(cExpr *base) : cVarRef() 
    {
        cTypeDecl *baseType = base->GetType();

        if (!baseType->IsPointer())
        {
            ThrowSemanticError("Attempted to dereference a non-pointer");
            return;
        }

        cTypeDecl *baseBaseType = baseType->ParentType();

        mBase = base;
        mBaseType = baseBaseType;
    }

    virtual cTypeDecl *GetType() { return mBaseType; }

    virtual void GenerateAddress()
    {
        mBase->GenerateCode();
    }

    virtual void GenerateCode()
    {
        mBase->GenerateCode();
        if (mBaseType->ElementSize() == 1)
        {
            EmitInt(PUSHCVARIND_OP);
        } else {
            EmitInt(PUSHVARIND_OP);
        }
    }

    virtual std::string toString()
    {
        return "( * " + mBase->toString() + " )";
    }
  protected:
    cExpr       *mBase;
    cTypeDecl   *mBaseType;
};

