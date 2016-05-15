#pragma once

#include <string>

#include "cTypeDecl.h"
#include "cSymbol.h"
#include "cSymbolTable.h"

class cArrayType : public cTypeDecl
{
  public:
    cArrayType(cTypeDecl *base, int size)
    {
        std::string name;
        cSymbol *sym;

        name = base->GetName()->Name() + "[" + std::to_string(size) + "]";

        sym = symbolTableRoot->Lookup(name);
        if (sym == nullptr) 
        {
            sym = new cSymbol(name);
        }

        // FIX THIS: semantic checks
        AddChild(sym);
        sym = symbolTableRoot->Insert(sym);
        sym->SetDecl(this);

        if (base->IsArray())
        {
            cArrayType *baseArray = (cArrayType*)base;
            AddChild(new cArrayType(baseArray->ParentType(), size));
            mSize = baseArray->mSize;
        }
        else
        {
            AddChild(base);
            mSize = size;
        }
    }

    virtual cTypeDecl *ParentType() 
    {
        return dynamic_cast<cTypeDecl*>(GetChild(1));
    }

    virtual cSymbol* GetName()  { return (cSymbol*)GetChild(0); }
    virtual bool IsArray()      { return true; }
    virtual int  Size()         { return ParentType()->Size() * mSize; }
    virtual int  ElementSize()  { return ParentType()->Size(); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};


