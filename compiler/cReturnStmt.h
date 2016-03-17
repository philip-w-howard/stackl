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

    virtual std::string toString()
    {
        if (GetExpr() != NULL)
            return "return " + GetExpr()->toString();
        else
            return "return";
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
};

