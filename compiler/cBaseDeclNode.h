#pragma once
//*******************************************************
// Purpose: Decl class for primitive types
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>

#include "cDeclNode.h"
#include "cSymbol.h"

class cBaseDeclNode : public cDeclNode
{
  public:
    cBaseDeclNode(cSymbol *id, int size) : cDeclNode()
    {
        mId = id;
        mSize = size;
    }

    virtual bool IsInt()   { return true; }
    virtual bool IsType()  { return true; }

    virtual int ComputeOffsets(int base) { return base; }

    virtual std::string toString()
    {
        return mId->toString();
    }

  protected:
    // int mSize;       // declared in cDeclNode base class
};

