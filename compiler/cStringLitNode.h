#pragma once

#include <string>
#include "cAstNode.h"
#include "codegen.h"

class cStringLitNode : public cAstNode
{
  public:
    cStringLitNode(char *str) : mStr(str)
    {}

    std::string toString() { return mStr; }

    void GenerateCode()
    {
        EmitString(mStr);
    }
  protected:
    std::string mStr;
};

