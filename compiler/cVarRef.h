#pragma once

#include <string>

#include "cExpr.h"
#include "cSymbol.h"

class cVarRef : public cExpr
{
  public:
    cVarRef() : cExpr() {}

    virtual bool IsLval()       { return true; }
    //virtual bool IsStruct()     { return false; }
    //virtual bool IsArray()      { return false; }
    //virtual bool IsPointer()    { return false; }
    //virtual bool IsFunc()       { return false; }
    //virtual bool IsArrayRef()   { return false; }

    virtual cTypeDecl *GetType() = 0;
    virtual void GenerateAddress() = 0;

    virtual string NodeType()             { return "VarRef"; }
    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

};

