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

    cSizeofExpr(cTypeDecl *type) : cExpr()
    {
        AddChild(nullptr);
        AddChild(type);
        mItemSize = type->Size();
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
            cTypeDecl *type = (cTypeDecl*)GetChild(1);
            mItemSize = type->Size();
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

