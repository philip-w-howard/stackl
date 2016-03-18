#pragma once

#include <string>

#include "cStmt.h"
#include "cSymbol.h"

class cNopStmt : public cStmt
{
  public:
    cNopStmt()  : cStmt()
    {
    }

    virtual void GenerateCode()
    {}

    virtual string NodeType()             { return "NOP"; }
    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};

