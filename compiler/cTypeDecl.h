#pragma once

#include <string>

#include "cDecl.h"
#include "cSymbol.h"

class cTypeDecl : public cDecl
{
  public:
    cTypeDecl() : cDecl() {}

    virtual cSymbol* GetName() = 0;

    virtual bool IsType()   { return true; }
    virtual bool IsVoid()   { return false; }

    virtual int ElementSize()       { return mSize; }
    virtual int Size()              { return mSize; }

    virtual cTypeDecl *GetType()    { return this; }
    virtual cTypeDecl *ParentType() { return this; }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

    static bool IsCompatibleWith(cTypeDecl *left, cTypeDecl *right);

  protected:
    int mSize;
};

