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

    virtual int ComputeOffsets(int base)
    {
        if (GetInit() != NULL) GetInit()->ComputeOffsets(base);
        if (GetExpr() != NULL) GetExpr()->ComputeOffsets(base);
        if (GetUpdate() != NULL) GetUpdate()->ComputeOffsets(base);
        if (GetStmt() != NULL) GetStmt()->ComputeOffsets(base);
        return base;
    }

    virtual void GenerateCode()
    {
        std::string start_loop = GenerateLabel();
        std::string end_loop = GenerateLabel();

        GetInit()->GenerateCode();
        EmitInt(POP_OP);            // need to handle VOID
        SetLabelValue(start_loop);
        GetExpr()->GenerateCode();
        EmitInt(JUMPE_OP);
        SetLabelRef(end_loop);
        GetStmt()->GenerateCode();
        GetUpdate()->GenerateCode();
        EmitInt(POP_OP);            // need to handle VOID
        EmitInt(JUMP_OP);
        SetLabelRef(start_loop);
        SetLabelValue(end_loop);
    }

    cExpr* GetInit()    { return (cExpr*)GetChild(0); }
    cExpr* GetExpr()    { return (cExpr*)GetChild(1); }
    cExpr* GetUpdate()  { return (cExpr*)GetChild(2); }
    cStmt* GetStmt()    { return (cStmt*)GetChild(3); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

};

