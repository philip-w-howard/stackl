#pragma once

#include <string>

#include "cSymbol.h"
#include "cExpr.h"
#include "cTypeDecl.h"
#include "cStructType.h"
#include "cVarRef.h"
#include "cSymbol.h"

class cStructRef : public cVarRef
{
  public:
    cStructRef(cExpr *base, cSymbol *field) : cVarRef() 
    {
        if (dynamic_cast<cVarRef*>(base) == nullptr)
        {
            ThrowSemanticError("cStructRef without underlying cVarRef");
            return;
        }
        cTypeDecl *baseType;

        if (!base->IsStruct()) 
        {
            ThrowSemanticError("Left side of '.' is not a struct");
            return;
        }

        if (base->IsArrayRef())
            baseType = base->GetType()->ParentType();
        else
            baseType = base->GetType();

        cStructType *s_decl = dynamic_cast<cStructType *>(baseType);
        if (s_decl==nullptr || !s_decl->Contains(field))
        {
            ThrowSemanticError(field->Name() + " is not a field");
            return;
        }

        field = s_decl->GetField(field);
        AddChild(base);
        AddChild(field);

        cVarDecl *test_field = dynamic_cast<cVarDecl*>(GetField()->GetDecl());
        if (test_field == NULL)
            fatal_error("cStructRef without underlying field.cVarDecl");
    }

    virtual cTypeDecl *GetType() 
    { 
        return dynamic_cast<cTypeDecl *>(GetField()->GetDecl()->GetType()); 
    }

    virtual void GenerateAddress()
    {
        cVarRef *var = dynamic_cast<cVarRef*>(GetBase());
        if (var == NULL)
            fatal_error("cStructRef without underlying cVarRef");

        var->GenerateAddress();

        cVarDecl *field = dynamic_cast<cVarDecl*>(GetField()->GetDecl());
        if (field == NULL)
            fatal_error("cStructRef without underlying field.cVarDecl");
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

    // FIX THIS: was cExpr
    cVarRef* GetBase()      { return (cVarRef*)GetChild(0); }
    cSymbol* GetField()     { return (cSymbol*)GetChild(1); }
    cVarDecl* GetFieldDecl()     { return (cVarDecl*)(GetField()->GetDecl()); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};

