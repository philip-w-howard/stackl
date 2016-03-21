#pragma once

#include <string>

#include "cTypeDecl.h"
#include "cSymbol.h"
#include "cSymbolTable.h"

class cStructType : public cTypeDecl
{
  public:
    cStructType(cSymbol *name, cScope *scope, cDeclsList *decls) 
        : cTypeDecl()
    {
        // FIX THIS: semantic checks?
        AddChild(name);
        AddChild(decls);
        mScope = scope;
        mSize = 0;
        name->SetDecl(this);
        symbolTableRoot->Insert(name);
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
};

