#pragma once

#include <string>

#include "cExpr.h"
#include "cTypeDecl.h"

class cCastExpr : public cExpr
{
  public:
    cCastExpr(cTypeDecl* type, cExpr *expr) : cExpr() 
    {
        AddChild(type);
        AddChild(expr);
    }

    virtual bool IsConst() { return GetExpr()->IsConst(); }
    virtual int  ConstValue()
    {
        return GetExpr()->ConstValue();
    }

    virtual cTypeDecl *GetType() 
    { return static_cast<cTypeDecl*>(GetChild(0)); }

    cExpr* GetExpr()    { return (cExpr*)GetChild(1); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};

