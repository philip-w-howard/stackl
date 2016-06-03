#pragma once

#include <string>

#include "cExpr.h"
#include "cSymbol.h"

class cVarRef : public cExpr
{
  public:
    cVarRef() : cExpr() {}

    virtual bool IsLval()       { return !IsConst(); /* true; */ }

    virtual cTypeDecl *GetType() = 0;

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};

