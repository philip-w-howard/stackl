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
    cBaseDeclNode(cSymbol *id, int size, bool isPointer) : cDeclNode()
    {
        mId = id;
        mSize = size;
        mIsPointer = isPointer;
    }

    virtual bool IsChar()  { return (mSize==1); }
    virtual bool IsInt()   { return true; }
    virtual bool IsType()  { return true; }
    virtual bool IsPointer() { return mIsPointer; }

    // NOTE charp is only pointer for now
    virtual bool IsString() { return IsPointer(); }

    virtual int ComputeOffsets(int base) { return base; }

    virtual std::string toString()
    {
        return mId->toString();
    }

  protected:
    // int mSize;       // declared in cDeclNode base class
    bool mIsPointer;
};

