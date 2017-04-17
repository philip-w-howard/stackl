#pragma once

#include <string>

#include "cTypeDecl.h"
#include "cDeclsList.h"
#include "cSymbol.h"
#include "cSymbolTable.h"

class cStructType : public cTypeDecl
{
  public:
    // modified by Joe
    cStructType(cSymbol *name)
        : cTypeDecl()
    {
        if (name == nullptr)
        {
            mSequence++;
            name = new cSymbol("$$unnamed_struct_" + std::to_string(mSequence));
        }

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

        AddChild(name);
        mScope = nullptr;
        mSize = 0;
        name->SetDecl(this);
        symbolTableRoot->Insert(name);
    }

    void AddDecls(cScope *scope, cDeclsList *decls)
    {
        AddChild(decls);
        mScope = scope;
    }

    bool Contains(cSymbol *sym)
    {
        return (mScope->Lookup(sym->Name()) != NULL);
    }

    cSymbol *GetField(cSymbol *sym)
    {
        return mScope->Lookup(sym->Name());
    }

    virtual bool IsStruct()         { return true; }
    virtual void SetSize(int size)  { mSize = size; }
    virtual int Size()              { return mSize; }
    virtual cSymbol* GetName()      { return (cSymbol*)GetChild(0); }

    cDeclsList* GetDecls()  { return (cDeclsList*)GetChild(1); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

  protected:
    cScope      *mScope;
    static int  mSequence;
};

