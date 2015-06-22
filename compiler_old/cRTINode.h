#pragma once
//*******************************************************
// Purpose: Class for a TRAP statement
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>

#include "cStmtNode.h"
#include "codegen.h"

class cRTINode : public cStmtNode
{
  public:
    cRTINode() : cStmtNode()
    {
    }

    virtual std::string toString()
    {
        std::string result("RTI");
        return result;
    }

    virtual int ComputeOffsets(int base)
    {
        return base;
    }

    virtual void GenerateCode()
    {
        EmitInt(RTI_OP);
    }
};

