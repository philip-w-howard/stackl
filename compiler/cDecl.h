#pragma once

#include <string>

#include "cStmt.h"
#include "cSymbol.h"

class cTypeDecl;
class cVarDecl;
class cFuncDecl;
class cDecl : public cStmt
{
  public:
    cDecl() {}

    virtual bool IsType()   { return false; }
    virtual bool IsVar()    { return false; }
    virtual bool IsFunc()   { return false; }
    virtual bool IsStruct() { return false; }
    virtual bool IsPointer(){ return false; }
    virtual bool IsArray()  { return false; }
    virtual cSymbol *GetName()      = 0;
    virtual cTypeDecl *GetType();
    virtual cVarDecl  *GetVar();
    virtual cFuncDecl *GetFunc();

    virtual std::string toString()
    {
        return "Decl " + GetName()->toString();
    }
};

