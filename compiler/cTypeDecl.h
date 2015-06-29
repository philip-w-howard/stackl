#pragma once

#include <string>

#include "cDecl.h"
#include "cSymbol.h"

class cTypeDecl : public cDecl
{
  public:
    cTypeDecl(cSymbol *name, int size) : cDecl(name)
    {
        mSize = size;
    }

    virtual bool IsType()   { return true; }
    virtual bool IsStruct() { return false; }
    virtual bool IsPointer(){ return false; }
    virtual bool IsArray()  { return false; }

    virtual int ElementSize()       { return mSize; }
    virtual int Size()              { return mSize; }

    virtual cTypeDecl *GetType()    { return this; }
    virtual cTypeDecl *ParentType() { return this; }

    virtual std::string toString()
    {
        return "type: " + mName->toString();
    }

    virtual void GenerateCode()
    {}

  protected:
    int mSize;
};

