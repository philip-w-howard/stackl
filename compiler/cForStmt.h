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

class cForStmt : public cStmt
{
  public:
    cForStmt(cExpr *init, cExpr *expr, cExpr *update, cStmt *stmt) : cStmt()
    {
        AddChild(init);
        AddChild(expr);
        AddChild(update);
        AddChild(stmt);
    }

    cExpr* GetInit()    { return (cExpr*)GetChild(0); }
    cExpr* GetExpr()    { return (cExpr*)GetChild(1); }
    cExpr* GetUpdate()  { return (cExpr*)GetChild(2); }
    cStmt* GetStmt()    { return (cStmt*)GetChild(3); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};

