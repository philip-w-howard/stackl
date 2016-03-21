#pragma once

#include <string>

#include "cStmt.h"
#include "cExpr.h"
#include "cSymbol.h"

class cReturnStmt : public cStmt
{
  public:
    cReturnStmt(cExpr *expr)  : cStmt()
    {
        AddChild(expr);
    }

    cExpr* GetExpr()    { return (cExpr*)GetChild(0); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};

