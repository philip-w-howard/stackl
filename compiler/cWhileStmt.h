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
#include "codegen.h"

class cWhileStmt : public cStmt
{
  public:
    cWhileStmt(cExpr *expr, cStmt *stmt) : cStmt()
    {
        AddChild(expr);
        AddChild(stmt);
    }

    virtual void GenerateCode()
    {
        std::string start_loop = GenerateLabel();
        std::string end_loop = GenerateLabel();

        SetLabelValue(start_loop);
        GetCond()->GenerateCode();
        EmitInt(JUMPE_OP);
        SetLabelRef(end_loop);
        GetStmt()->GenerateCode();
        EmitInt(JUMP_OP);
        SetLabelRef(start_loop);
        SetLabelValue(end_loop);
    }

    cExpr* GetCond()    { return (cExpr*)GetChild(0); }
    cStmt* GetStmt()    { return (cStmt*)GetChild(1); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};

