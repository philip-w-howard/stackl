#pragma once

#include <string>

#include "cExpr.h"
#include "cSymbol.h"

class cIntExpr : public cExpr
{
  public:
    cIntExpr(int value) : cExpr() 
    {
        mValue = value;
    }

    virtual bool IsConst() { return true; }
    virtual int  ConstValue() {  return mValue; }

    virtual cTypeDecl *GetType()
    {
        cDecl *type;

        if (mValue >= -128 && mValue <= 127)
            type = symbolTableRoot->Lookup("char")->GetDecl();
        else
            type = symbolTableRoot->Lookup("int")->GetDecl();

        return dynamic_cast<cTypeDecl *>(type);
    }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

  protected:
    int mValue;
};

