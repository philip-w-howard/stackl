#pragma once

#include <string>

#include "cStmt.h"
#include "cExpr.h"
#include "cSymbol.h"

#include "codegen.h"

class cReturnStmt : public cStmt
{
  public:
    cReturnStmt(cExpr *expr)  : cStmt()
    {
        AddChild(expr);
    }

    virtual void GenerateCode()
    {
        if (GetExpr() != NULL) 
        {
            GetExpr()->GenerateCode();
            EmitInt(RETURNV_OP);
        } else {
            EmitInt(RETURN_OP);
        }
    }

    cExpr* GetExpr()    { return (cExpr*)GetChild(0); }

    virtual string NodeType()             { return "ReturnStmt"; }
    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

};

