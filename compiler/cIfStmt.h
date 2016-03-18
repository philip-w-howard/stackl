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
#include "codegen.h"

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

    virtual void GenerateCode()
    {
        std::string if_label = GenerateLabel();
        GetCond()->GenerateCode();
        EmitInt(JUMPE_OP);
        SetLabelRef(if_label);
        if (GetIfStmt() != NULL) GetIfStmt()->GenerateCode();

        if (GetElseStmt() != NULL)
        {
            std::string else_label = GenerateLabel();
            EmitInt(JUMP_OP);
            SetLabelRef(else_label);
            SetLabelValue(if_label);
            GetElseStmt()->GenerateCode();
            SetLabelValue(else_label);
        }
        else
        {
            SetLabelValue(if_label);
        }
    }

    cExpr* GetCond()    { return (cExpr*)GetChild(0); }
    cStmt* GetIfStmt()  { return (cStmt*)GetChild(1); }
    cStmt* GetElseStmt(){ return (cStmt*)GetChild(2); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

};

