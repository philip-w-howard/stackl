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
    cBaseDeclNode(cSymbol *id, int size, bool isPointer, 
            bool isArray, bool isAbs) 
        : cDeclNode()
    {
        mId = id;
        mSize = size;
        mIsPointer = isPointer;
        mIsArray = isArray;
        mIsAbsolute = isAbs;
    }

    virtual bool IsChar()       { return (mSize==1); }
    virtual bool IsInt()        { return true; }
    virtual bool IsType()       { return true; }
    virtual bool IsPointer()    { return mIsPointer; }
    virtual bool IsArray()      { return mIsArray; }
    //virtual bool IsAbsolute()   { return mIsAbsolute; }

    // NOTE strings are the only pointers or arrays for now
    virtual bool IsString() { return (IsPointer() || (IsChar() && IsArray())); }

    virtual int ComputeOffsets(int base) { return base; }

    virtual std::string toString()
    {
        return mId->toString();
    }

  protected:
    // int mSize;       // declared in cDeclNode base class
    bool mIsPointer;
    bool mIsArray;
    bool mIsAbsolute;
};

