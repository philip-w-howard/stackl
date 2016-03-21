#pragma once
//*******************************************************
// Purpose: Class for an if statement
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>

#include "cStmt.h"
#include "cExpr.h"

class cIfStmt : public cStmt
{
  public:
    cIfStmt(cExpr *expr, cStmt *ifPart, cStmt *elsePart) 
        : cStmt()
    {
        AddChild(expr);
        AddChild(ifPart);
        AddChild(elsePart);
    }

    cExpr* GetCond()    { return (cExpr*)GetChild(0); }
    cStmt* GetIfStmt()  { return (cStmt*)GetChild(1); }
    cStmt* GetElseStmt(){ return (cStmt*)GetChild(2); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

};

