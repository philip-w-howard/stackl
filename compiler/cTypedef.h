#pragma once

#include <string>

#include "cTypeDecl.h"
#include "cSymbol.h"
#include "cSymbolTable.h"
#include "cStructType.h"

class cTypedef : public cTypeDecl
{
  public:
    cTypedef(cSymbol *typeName, cSymbol *name)
        : cTypeDecl()
    {
        if (symbolTableRoot->LocalLookup(name->Name()) != nullptr)
        {
            ThrowSemanticError(name->Name() +
                    " already defined in local scope");
            return;
        }

        // if the symbol exists in an outer scope, we need to create a new one
        // instead of re-using the outer scoped symbol
        if (symbolTableRoot->Lookup(name->Name()) != nullptr)
        {
            name = new cSymbol(name->Name());
        }

        AddChild(typeName);
        AddChild(name);
        name->SetDecl(typeName->GetDecl());
        symbolTableRoot->Insert(name);
    }

    cTypedef(cTypeDecl *typeDecl, cSymbol *name)
        : cTypeDecl()
    {
        if (symbolTableRoot->LocalLookup(name->Name()) != nullptr)
        {
            ThrowSemanticError(name->Name() +
                    " already defined in local scope");
            return;
        }

        // if the symbol exists in an outer scope, we need to create a new one
        // instead of re-using the outer scoped symbol
        if (symbolTableRoot->Lookup(name->Name()) != nullptr)
        {
            name = new cSymbol(name->Name());
        }

        AddChild(typeDecl);
        AddChild(name);
        name->SetDecl(typeDecl);
        symbolTableRoot->Insert(name);
    }

    virtual bool IsType()           { return true; }
    virtual void SetSize(int size)  { mSize = size; }
    virtual int Size()              { return mSize; }
    virtual cSymbol* GetName()      { return (cSymbol*)GetChild(1); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};

