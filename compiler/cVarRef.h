#pragma once

#include <string>

#include "cExpr.h"
#include "cSymbol.h"

class cVarRef : public cExpr
{
  public:
    cVarRef() : cExpr()
    {
        mSymbol = NULL;
        mDecl   = NULL;
    }

    cVarRef(cSymbol *id) : cExpr() 
    {
        mSymbol = id;
        if (symbolTableRoot->Lookup(id->Name()) == NULL)
        {
            ThrowSemanticError(id->Name() + " is not defined");
            return;
        }

        mDecl = dynamic_cast<cDecl *>( mSymbol->GetDecl());
        if (mDecl == NULL)
        {
            ThrowSemanticError(id->Name() + " is not declared");
            return;
        }
    }

    virtual bool IsLval()   { return true; }
    virtual bool IsStruct() { return GetType()->IsStruct(); }
    virtual bool IsArray()  { return GetType()->IsArray(); }
    virtual bool IsPointer(){ return GetType()->IsPointer(); }
    virtual bool IsFunc()   { return GetType()->IsFunc(); }

    virtual cTypeDecl *GetType() 
    { 
        return mDecl->GetType(); 
    }

    virtual void GenerateAddress()
    {
        cVarDecl *var = dynamic_cast<cVarDecl*>(mDecl);
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
        cVarDecl *var = dynamic_cast<cVarDecl*>(mDecl);
        if (var != NULL)
        {
            if (var->GetType()->IsArray())
            {
                GenerateAddress();
            } else {
                if (var->IsGlobal())
                {
                    GenerateAddress();
                    if (var->GetType()->Size() == 1)
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
            fatal_error("Attempted to generate code for a cVarRef without a decl");
        }
    }

    virtual std::string toString()
    {
        return "var " + mSymbol->toString();
    }
  protected:
    cSymbol *mSymbol;
    cDecl   *mDecl;
};

