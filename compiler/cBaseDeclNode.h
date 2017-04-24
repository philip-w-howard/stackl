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

#include "cTypeDecl.h"
#include "cSymbol.h"

class cBaseDeclNode : public cTypeDecl
{
  public:
    cBaseDeclNode(cSymbol *name, int size, bool isVoid = false) : cTypeDecl()
    {
        AddChild(name);
        mSize = size;
        mIsVoid = isVoid;
    }

    virtual bool IsType() { return true; }
    virtual bool IsInt()  { return !mIsVoid; }
    virtual bool IsVoid() { return mIsVoid; }
    virtual cSymbol* GetName()            { return (cSymbol*)GetChild(0); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
  protected:
    bool mIsVoid;
};

