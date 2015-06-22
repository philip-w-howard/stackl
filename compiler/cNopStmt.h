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

    virtual std::string toString()
    {
        return "{}";
    }

    virtual void GenerateCode()
    {}

  protected:
};

