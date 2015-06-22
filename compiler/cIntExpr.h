#pragma once

#include <string>

#include "cExpr.h"
#include "cSymbol.h"

#include "codegen.h"

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
        cDecl *type = symbolTableRoot->Lookup("int")->GetDecl();
        return dynamic_cast<cTypeDecl *>(type);
    }

    virtual std::string toString()
    {
        return " " + std::to_string(mValue) + " ";
    }

    virtual void GenerateCode()
    {
        EmitInt(PUSH_OP);
        EmitInt(mValue);
    }

  protected:
    int mValue;
};

