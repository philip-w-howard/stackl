#pragma once

#include <string>

#include "cDecl.h"
#include "cVarDecl.h"
#include "cExpr.h"
#include "cSymbol.h"
#include "cVarRef.h"

class cPlainVarRef : public cVarRef
{
  public:
    cPlainVarRef(cSymbol *id) : cVarRef() 
    {
        if (symbolTableRoot->Lookup(id->Name()) == nullptr)
        {
            ThrowSemanticError(id->Name() + " is not defined");
            return;
        }

        // Was cast to cDecl
        if (dynamic_cast<cDecl *>( id->GetDecl()) == nullptr)
        {
            ThrowSemanticError(id->Name() + " is not declared");
            return;
        }

        AddChild(id);
    }

    cSymbol*  GetName()          { return (cSymbol*)GetChild(0); }
    cVarDecl* GetDecl()          { return (cVarDecl*)(GetName()->GetDecl()); }
    virtual cTypeDecl *GetType(){ return GetDecl()->GetType(); }
    virtual bool IsStruct()     { return GetType()->IsStruct(); }
    virtual bool IsArray()      { return GetType()->IsArray(); }
    virtual bool IsPointer()    { return GetType()->IsPointer(); }
    virtual bool IsFunc()       { return GetType()->IsFunc(); }
    virtual bool IsArrayRef()   { return false; }


    virtual void GenerateAddress()
    {
        cVarDecl *var = dynamic_cast<cVarDecl*>(GetDecl());
        if (var == NULL)
            fatal_error("Attempted to generate code for a cVarRef without a decl");
        if (var->IsGlobal())
        {
            EmitInt(PUSH_OP);
            EmitGlobalRef(var->GetName()->Name());
            EmitInt(PUSH_OP);
            EmitInt(var->GetOffset());
            EmitInt(PLUS_OP);
        } else {
            EmitInt(PUSH_OP);
            EmitInt(var->GetOffset());
            EmitInt(PUSHFP_OP);
            EmitInt(PLUS_OP);
        }
    }

    virtual void GenerateCode()
    {
        cVarDecl *var = dynamic_cast<cVarDecl*>(GetDecl());
        if (var != NULL)
        {
            if (var->GetType()->IsArray())
            {
                GenerateAddress();
            } else {
                if (var->IsGlobal())
                {
                    GenerateAddress();
                    if (var->GetType()->Size() == 1)    // ElementSize?
                        EmitInt(PUSHCVARIND_OP);
                    else
                        EmitInt(PUSHVARIND_OP);
                } else {
                    if (var->GetType()->Size() == 1)
                        EmitInt(PUSHCVAR_OP);
                    else
                        EmitInt(PUSHVAR_OP);

                    EmitInt(var->GetOffset());
                }
            }
        } else {
            fatal_error("Attempt to generate code for a cVarRef with no decl");
        }
    }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};

