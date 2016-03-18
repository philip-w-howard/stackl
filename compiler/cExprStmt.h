#pragma once

#include <string>

#include "cStmt.h"
#include "cExpr.h"
#include "cSymbol.h"

#include "codegen.h"

class cExprStmt : public cStmt
{
  public:
    cExprStmt(cExpr *expr)  : cStmt()
    {
        AddChild(expr);
    }

    virtual int ComputeOffsets(int base)
    {
        return GetExpr()->ComputeOffsets(base);
    }

    virtual void GenerateCode()
    {
        GetExpr()->GenerateCode();

        // remove the result from the stack
        EmitInt(POP_OP);
    }

    cExpr* GetExpr()    { return (cExpr*)GetChild(0); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

};

