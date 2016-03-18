#pragma once

#include <string>

#include "cStmt.h"
#include "cTypeDecl.h"
#include "cSymbol.h"

class cExpr : public cStmt
{
  public:
    cExpr() : cStmt() 
    {
    }

    virtual bool IsLval()       { return false; }
    virtual bool IsGlobal()     { return false; }

    virtual bool IsStruct()     { return false; }
    virtual bool IsArray()      { return false; }
    virtual bool IsPointer()    { return false; }
    virtual bool IsArrayRef()   { return false; }

    virtual bool IsConst()      { return false; }
    virtual int  ConstValue()   { return 0; }

    //virtual bool HasEffect()    = 0;

    virtual int Size()          { return GetType()->ElementSize(); }

    virtual cTypeDecl *GetType() = 0;
    //virtual int  GetOffset() = 0;
    virtual void GenerateCode() = 0;

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};

