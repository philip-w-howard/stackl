#pragma once

#include <string>

#include "cVarDecl.h"
#include "cStructType.h"
#include "cTypeDecl.h"
#include "cVarRef.h"
#include "cSymbol.h"

class cStructDeref : public cVarRef
{
  public:
    cStructDeref(cExpr *base, cSymbol *field) : cVarRef() 
    {
        AddChild(base);
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

        field = s_decl->GetField(field);
        AddChild(field);
    }

    virtual cTypeDecl *GetType() 
    { 
        return dynamic_cast<cTypeDecl *>(GetField()->GetDecl()->GetType()); 
    }

    virtual void GenerateAddress()
    {
        GetBase()->GenerateCode();

        cVarDecl *field = dynamic_cast<cVarDecl*>(GetField()->GetDecl());
        if (field == NULL)
            fatal_error("cStructDeref without underlying field.cVarDecl");
        EmitInt(PUSH_OP);
        EmitInt(field->GetOffset());
        EmitInt(PLUS_OP);
    }

    virtual void GenerateCode()
    {
        GenerateAddress();

        if (GetField()->GetDecl()->GetType()->IsArray())
        {
            // do nothing: we want the addr
        }
        else
        {
            if (GetField()->GetDecl()->GetType()->Size() == 1)
                EmitInt(PUSHCVARIND_OP);
            else
                EmitInt(PUSHVARIND_OP);
        }
    }

    cExpr* GetBase()    { return (cExpr*)GetChild(0); }
    cSymbol* GetField() { return (cSymbol*)GetChild(1); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};

