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
        AddChild(expr);
        AddChild(nullptr);
    }

    cSizeofExpr(cSymbol *sym) : cExpr()
    {
        if (symbolTableRoot->Lookup(sym->Name()) == NULL)
        {
            ThrowSemanticError("Symbol " + sym->Name() + " not defined");
            return;
        }

        AddChild(nullptr);
        AddChild(sym);
        mItemSize = sym->GetDecl()->GetType()->Size();
    }

    virtual bool IsConst() { return true; }
    virtual int  ConstValue() 
    { 
        if (GetChild(0) != nullptr)
        {
            cExpr *expr = (cExpr*)GetChild(0);
            mItemSize = expr->GetType()->Size();
        }
        else
        {
            cSymbol *sym = (cSymbol*)GetChild(1);
            mItemSize = sym->GetDecl()->GetType()->Size();
        }


        return mItemSize; 
    }

    virtual cTypeDecl *GetType() 
    { 
        return symbolTableRoot->Lookup("int")->GetDecl()->GetType();
    }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

  protected:
    int   mItemSize;
};

