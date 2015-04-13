#pragma once
//*******************************************************
// Purpose: Class for an assignment statement
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>

#include "cStmtNode.h"
#include "cExprNode.h"
#include "parse.h"
#include "codegen.h"

class cAssignAsmNode : public cStmtNode
{
  public:
    cAssignAsmNode(cVarRefNode *lval, cAsmNode *stmt) : cStmtNode()
    {
        mLval = lval;
        mAsm = stmt;
        if (!lval->GetType()->CompatibleWith(
                    symbolTableRoot->Lookup("int")->GetType())
           )
        {
            semantic_error("Cannot assign int to " + 
                    lval->GetType()->GetBaseType()->Name());
        }
    }

    virtual int ComputeOffsets(int base)
    {
        mLval->ComputeOffsets(base);
        mAsm->ComputeOffsets(base);
        return base;
    }

    virtual std::string toString()
    {
        std::string result("(ASSIGN: ");
        result += mLval->toString();
        result += " = ";
        result += mAsm->toString();
        result += ")";
        return result;
    }

    virtual void GenerateCode()
    {
        mAsm->GenerateCode();
        mLval->GenerateLVal();
    }

  protected:
    cVarRefNode *mLval;     // the lhs of the assignment
    cAsmNode    *mAsm;      // the opcode to be emitted
};

