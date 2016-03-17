#pragma once

#include <string>

#include "cAstNode.h"
#include "cSymbol.h"

class cStmt : public cAstNode
{
  public:
    cStmt() : cAstNode()
    {
    }

    // useful for cDecl and cExpr, both of which are subclasses.
    // Other subclasses may not have a use for this.
    virtual int Size() { return 0; }
};

