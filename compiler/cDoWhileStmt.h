#pragma once
//*******************************************************
// Purpose: Class for a while statement
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

class cDoWhileStmt : public cStmt
{
  public:
    cDoWhileStmt(cExpr *expr, cStmt *stmt) : cStmt()
    {
        AddChild(expr);
        AddChild(stmt);
    }

    cExpr* GetCond()    { return (cExpr*)GetChild(0); }
    cStmt* GetStmt()    { return (cStmt*)GetChild(1); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};

