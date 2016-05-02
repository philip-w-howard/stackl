#pragma once

#include <string>

#include "cTypeDecl.h"
#include "cStmtsList.h"
#include "cReturnStmt.h"
#include "cIntExpr.h"
#include "cSymbol.h"
#include "cDeclsList.h"
#include "cCodeGen.h"

class cFuncDecl : public cTypeDecl
{
  public:
    cFuncDecl(cTypeDecl *type, cSymbol *name) : cTypeDecl()
    {
        mHasStatements = false;
        mHasParams = false;
        AddChild(name);
        AddChild(type);
        AddChild(nullptr); // params
        AddChild(nullptr); // stmts
        mHasStatements  = false;
        mDeclsSize      = 0;
        mSize = cCodeGen::STACKL_WORD_SIZE;

        // Do semantic checks
        cSymbol *func = symbolTableRoot->Lookup(name->Name());
        if (func != nullptr)
        {
            // we have another definition. Let's make sure that is compatible
            // with this def.
            cDecl *decl = func->GetDecl();
            if (!decl->IsFunc())
            {
                ThrowSemanticError(name->Name() +
                        " previously defined as other than a function");
                return;
            }
            else
            {
                cFuncDecl *funcDecl = decl->GetFunc();
                assert(funcDecl != nullptr);
                if (funcDecl->ReturnType() != type)
                {
                    ThrowSemanticError(name->Name() +
                        " previously defined with a different return type");
                    return;
                }
                else
                {
                    m_children = funcDecl->m_children;
                    mHasStatements = funcDecl->mHasStatements;
                    mHasParams = funcDecl->mHasParams;
                    name->SetDecl(this);
                }
            }

        }
        else
        {
            name->SetDecl(this);
            symbolTableRoot->Insert(name);
        }
    }

    void AddParams(cDeclsList *params)
    {
        cDeclsList *old_params = GetParams();
        // if params have been set and either the old or new are not NULL
        if (mHasParams && (params != nullptr || old_params != nullptr))
        {
            if ( (params != nullptr && old_params == nullptr) ||
                 (params == nullptr && old_params != nullptr) ||
                 (params->NumChildren() != old_params->NumChildren()) )
            {
                ThrowSemanticError(GetName()->Name() +
                        " redeclared with a different number of parameters");
                return;
            }

            for (int ii=0; ii<params->NumChildren(); ii++)
            {
                cDecl *decl1 = params->GetDecl(ii);
                cDecl *decl2 = old_params->GetDecl(ii);

                assert(decl1 != nullptr && decl2 != nullptr);
                if (decl1->GetType() != decl2->GetType())
                {
                    ThrowSemanticError(GetName()->Name() +
                        " redeclared with a different parameters");
                    return;
                }
            }
        }

        SetChild(2, params);
        mHasParams = true;
    }

    void AddStatements(cStmtsList *stmts)
    {
        if (mHasStatements)
        {
            ThrowSemanticError(GetName()->Name() + " already defined");
            return;
        }
        SetChild(3, stmts);
        mHasStatements = true;
    }

    virtual bool IsFunc()           { return true; }
    virtual bool IsType()           { return false; }
    virtual void SetSize(int size)  { mDeclsSize = size; }
    virtual int  GetSize()          { return mDeclsSize; }

    cSymbol* GetName()          { return (cSymbol*)GetChild(0); }
    cTypeDecl* ReturnType()     { return (cTypeDecl*)GetChild(1); }
    cDeclsList* GetParams()     { return (cDeclsList*)GetChild(2); }
    cStmtsList* GetStmts()      { return (cStmtsList*)GetChild(3); }
    bool IsDefinition()         { return mHasStatements; }
    int DeclsSize()             { return mDeclsSize; }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

  protected:
    bool mHasStatements;
    bool mHasParams;
    int mDeclsSize;
};

