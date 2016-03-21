#pragma once

#include <string>

#include "cStmt.h"
#include "cExpr.h"
#include "cSymbol.h"

class cExprStmt : public cStmt
{
  public:
    cExprStmt(cExpr *expr)  : cStmt()
    {
        AddChild(expr);
    }

    cExpr* GetExpr()    { return (cExpr*)GetChild(0); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};

