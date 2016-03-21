#pragma once

#include <string>

#include "cTypeDecl.h"
#include "cSymbol.h"
#include "cCodeGen.h"

class cPointerType : public cTypeDecl
{
  public:
    cPointerType(cSymbol *name) : cTypeDecl()
    {
        name->SetDecl(this);
        symbolTableRoot->InsertRoot(name);
        AddChild(name);
        mSize = cCodeGen::STACKL_WORD_SIZE;
    }

    virtual cTypeDecl *ParentType() 
    {
        std::string name = GetName()->Name();

        // remove the last "*"
        name.pop_back();

        cSymbol *sym = symbolTableRoot->Lookup(name);
        if (sym == NULL) fatal_error("Pointer type with no base type.");

        return dynamic_cast<cTypeDecl*>(sym->GetDecl());
    }

    virtual bool IsPointer() { return true; }
    virtual int  ElementSize() { return ParentType()->Size(); }
    virtual cSymbol* GetName()  { return (cSymbol*)GetChild(0); }

    static cPointerType *PointerType(cTypeDecl *base)
    {
        std::string name = base->GetName()->Name() + "*";
        cSymbol *sym = symbolTableRoot->Lookup(name);
        if (sym == NULL) 
        {
            sym = new cSymbol(name);
            return new cPointerType(sym);
        } else {
            return dynamic_cast<cPointerType *>(sym->GetDecl());
        }

        return NULL;
    }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};

