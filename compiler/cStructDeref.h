#pragma once

#include <string>

#include "cVarRef.h"
#include "cSymbol.h"

class cStructDeref : public cVarRef
{
  public:
    cStructDeref(cExpr *base, cSymbol *field) : cVarRef() 
    {
        cTypeDecl *baseType = base->GetType();

        if (!baseType->IsPointer())
        {
            ThrowSemanticError("The left of -> must be a pointer");
            return;
        }

        cTypeDecl *baseBaseType = baseType->ParentType();
        if (!baseBaseType->IsStruct())
        {
            ThrowSemanticError("The left of -> must be a struct pointer");
            return;
        }

        cStructType *s_decl = dynamic_cast<cStructType *>(baseBaseType);
        if (!s_decl->Contains(field))
        {
            ThrowSemanticError(field->Name() + " is not a field");
            return;
        }

        mBase = base;
        mBaseType = baseBaseType;
        mField = s_decl->GetField(field);
    }

    virtual cTypeDecl *GetType() 
    { 
        return dynamic_cast<cTypeDecl *>(mField->GetDecl()->GetType()); 
    }

    virtual std::string toString()
    {
        return mBase->toString() + " -> " + mField->toString();
    }

    virtual void GenerateAddress()
    {
        mBase->GenerateCode();

        cVarDecl *field = dynamic_cast<cVarDecl*>(mField->GetDecl());
        if (field == NULL)
            fatal_error("cStructDeref without underlying field.cVarDecl");
        EmitInt(PUSH_OP);
        EmitInt(field->GetOffset());
        EmitInt(PLUS_OP);
    }

    virtual void GenerateCode()
    {
        GenerateAddress();

        if (mField->GetDecl()->GetType()->IsArray())
        {
            // do nothing: we want the addr
        }
        else
        {
            if (mField->GetDecl()->GetType()->Size() == 1)
                EmitInt(PUSHCVARIND_OP);
            else
                EmitInt(PUSHVARIND_OP);
        }
    }

  protected:
    cExpr       *mBase;
    cTypeDecl   *mBaseType;
    cSymbol     *mField;
};

