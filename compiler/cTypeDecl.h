#pragma once

#include <string>

#include "cDecl.h"
#include "cSymbol.h"

class cTypeDecl : public cDecl
{
  public:
    cTypeDecl(cSymbol *name, int size) : cDecl()
    {
        AddChild(name);
        mSize = size;
    }

    virtual cSymbol* GetName() { return (cSymbol*)GetChild(0); }

    virtual bool IsType()   { return true; }

    virtual int ElementSize()       { return mSize; }
    virtual int Size()              { return mSize; }

    virtual cTypeDecl *GetType()    { return this; }
    virtual cTypeDecl *ParentType() { return this; }

    virtual std::string toString()
    {
        return "type: " + GetName()->toString();
    }

    virtual void GenerateCode()
    {}

  protected:
    int mSize;
};

