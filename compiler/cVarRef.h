#pragma once

#include <string>

#include "cExpr.h"
#include "cSymbol.h"

class cVarRef : public cExpr
{
  public:
    cVarRef() : cExpr() {}

    virtual bool IsLval()       { return !IsConst(); /* true; */ }

    virtual bool IsStruct()     { return GetType()->IsStruct(); }
    virtual bool IsArray()      { return GetType()->IsArray(); }
    virtual bool IsPointer()    { return GetType()->IsPointer(); }


    virtual cTypeDecl *GetType() = 0;
    virtual cExpr     *GetBase() { return this; }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};

