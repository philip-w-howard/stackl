#pragma once

#include <string>

#include "cExpr.h"
#include "cSymbol.h"

class cSizeofExpr : public cExpr
{
  public:
    cSizeofExpr(cExpr *expr) : cExpr() 
    {
        mItemSize = expr->GetType()->Size();
    }

    cSizeofExpr(cSymbol *sym) : cExpr()
    {
        if (symbolTableRoot->Lookup(sym->Name()) == NULL)
        {
            ThrowSemanticError("Symbol " + sym->Name() + " not defined");
            return;
        }

        mItemSize = sym->GetDecl()->GetType()->Size();
    }

    virtual bool IsConst() { return true; }
    virtual int  ConstValue() { return mItemSize; }

    virtual cTypeDecl *GetType() 
    { 
        return symbolTableRoot->Lookup("int")->GetDecl()->GetType();
    }

    virtual std::string toString()
    {
       return "SIZEOF(" + std::to_string(mItemSize) + ")";
    }

    virtual void GenerateCode()
    {
        EmitInt(PUSH_OP);
        EmitInt(mItemSize);
    }

  protected:
    int   mItemSize;
};

