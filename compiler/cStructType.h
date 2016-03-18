#pragma once

#include <string>

#include "cTypeDecl.h"
#include "cSymbol.h"
#include "cSymbolTable.h"

class cStructType : public cTypeDecl
{
  public:
    cStructType(cSymbol *name, cScope *scope, cDeclsList *decls) 
        : cTypeDecl(name, decls->Size())
    {
        // FIX THIS: semantic checks?
        AddChild(decls);
        mScope = scope;
        mSize = 0;
        name->SetDecl(this);
        symbolTableRoot->Insert(name);
    }

    virtual bool IsStruct() { return true; }

    bool Contains(cSymbol *sym)
    {
        return (mScope->Lookup(sym->Name()) != NULL);
    }

    cSymbol *GetField(cSymbol *sym)
    {
        return mScope->Lookup(sym->Name());
    }

    virtual int Size() { return mSize; }

    virtual int ComputeOffsets(int base)
    {
        // structs are offset from zero
        mSize = GetDecls()->ComputeOffsets(0);

        return base;
    }

    virtual void GenerateCode()
    {
    }

    cDeclsList* GetDecls()  { return (cDeclsList*)GetChild(1); }

    virtual string NodeType()             { return "StructType"; }
    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

  protected:
    cScope      *mScope;
};

