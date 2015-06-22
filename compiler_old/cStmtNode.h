#pragma once
//*******************************************************
// Purpose: Base class for all statements
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>

#include "cAstNode.h"

class cStmtNode : public cAstNode
{
  public:
    cStmtNode() : cAstNode()
    {}
};

