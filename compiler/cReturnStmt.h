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
        mExpr = expr;
    }

    virtual std::string toString()
    {
        if (mExpr != NULL)
            return "return " + mExpr->toString();
        else
            return "return";
    }

    virtual void GenerateCode()
    {
        if (mExpr != NULL) 
        {
            mExpr->GenerateCode();
            EmitInt(RETURNV_OP);
        } else {
            EmitInt(RETURN_OP);
        }
    }

  protected:
    cExpr *mExpr;
};

