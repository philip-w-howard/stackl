#pragma once

#include <string>

#include "cVarRef.h"
#include "cSymbol.h"

class cStructRef : public cVarRef
{
  public:
    cStructRef(cExpr *base, cSymbol *field) : cVarRef() 
    {
        if (!base->IsStruct()) 
        {
            ThrowSemanticError("Left side of '.' is not a struct");
            return;
        }

        cTypeDecl *baseType = base->GetType();
        cStructType *s_decl = dynamic_cast<cStructType *>(baseType);
        if (!s_decl->Contains(field))
        {
            ThrowSemanticError(field->Name() + " is not a field");
            return;
        }

        mBase = base;
        mBaseType = baseType;
        mField = s_decl->GetField(field);
    }

    virtual cTypeDecl *GetType() 
    { 
        return dynamic_cast<cTypeDecl *>(mField->GetDecl()->GetType()); 
    }

    virtual void GenerateAddress()
    {
        cVarRef *var = dynamic_cast<cVarRef*>(mBase);
        if (var == NULL)
            fatal_error("cStructRef without underlying cVarRef");

        var->GenerateAddress();

        cVarDecl *field = dynamic_cast<cVarDecl*>(mField->GetDecl());
        if (field == NULL)
            fatal_error("cStructRef without underlying field.cVarDecl");
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

    virtual std::string toString()
    {
        return mBase->toString() + " . " + mField->toString();
    }
  protected:
    cExpr *mBase;
    cTypeDecl *mBaseType;
    cSymbol *mField;
};

