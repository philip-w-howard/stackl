#pragma once

#include <string>
#include "cExprNode.h"
#include "cSymbolTable.h"
#include "codegen.h"

class cStringLitNode : public cExprNode
{
  public:
    cStringLitNode(char *str) : mStr(str)
    {}

    std::string toString() { return mStr; }

    virtual cDeclNode *GetType()
    {
        return symbolTableRoot->Lookup("cconst")->GetType();
    }

    void GenerateCode()
    {
        EmitInt(PUSH_OP);
        EmitString(mStr);
    }
  protected:
    std::string mStr;
};

