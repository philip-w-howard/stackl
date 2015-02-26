#pragma once
//*******************************************************
// Purpose: Class for floating point constants
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>

#include "cSymbol.h"
#include "cSymbolTable.h"
#include "cExprNode.h"

class cFloatExprNode : public cExprNode
{
  public:
    cFloatExprNode(double value) : cExprNode()
    {
        mFloatVal = value;
    }

    // lookup and return type of 'float' in symbol table
    virtual cDeclNode *GetType()
    {
        cSymbol *type = symbolTableRoot->Lookup("float");
        return type->GetType();
    }

    virtual std::string toString()
    {
        std::string result("(EXPR: ");
        result += std::to_string(mFloatVal);
        result += ")";

        return result;
    }

    virtual void GenerateCode()
    {
        EmitFloat(mFloatVal);
    }
  protected:
    double mFloatVal;       // value of float constant
};

