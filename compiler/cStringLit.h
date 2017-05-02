#pragma once

#include <string>
#include "cExpr.h"
#include "cSymbolTable.h"
#include "cPointerType.h"

class cStringLit : public cExpr
{
  public:
    cStringLit(char *str) :  cExpr(), mStr(str)
    {}

    virtual bool IsConst() { return true; }
    virtual int  ConstValue()
    {
        fatal_error("ConstValue not implemented for cStringLit");
        return 0;
    }

    virtual cTypeDecl *GetType()
    {
        return cPointerType::PointerType(
                dynamic_cast<cTypeDecl*>
                (symbolTableRoot->Lookup("char")->GetDecl()));
        //return dynamic_cast<cTypeDecl *>
            //(symbolTableRoot->Lookup("char*")->GetDecl());
    }

    virtual std::string GetString() { return mStr; }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

  protected:
    std::string mStr;
};

