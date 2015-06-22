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
        mExpr = expr;
    }

    virtual std::string toString()
    {
        return mExpr->toString();
    }

    virtual int ComputeOffsets(int base)
    {
        return mExpr->ComputeOffsets(base);
    }

    virtual void GenerateCode()
    {
        mExpr->GenerateCode();

        // remove the result from the stack
        EmitInt(POP_OP);
    }

  protected:
    cExpr *mExpr;
};

