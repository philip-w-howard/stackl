#pragma once
//*******************************************************
// Purpose: Class for representing a #pragma statement
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 5/3/2015
//
//*******************************************************

#include <string>

#include "cDecl.h"

class cPragma : public cDecl
{
  public:
    cPragma(std::string name, std::string arg) : cDecl()
    {
        mName = name;
        mArg = arg;
    }

    cSymbol* GetName() { return nullptr; }
    string GetOp() { return mName; }
    string GetArg()  { return mArg; }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

  protected:
    string mName;            // pragma name
    string mArg;             // pragma argument
};

