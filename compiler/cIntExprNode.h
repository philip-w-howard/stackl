#pragma once
//*******************************************************
// Purpose: Class for integer constants
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
#include "codegen.h"

class cIntExprNode : public cExprNode
{
  public:
    cIntExprNode(int value) : cExprNode()
    { mIntVal = value; }

    // return integer constant value
    virtual int GetValue()
    { return mIntVal; }

    // lookup and return type 'char' or 'int' based on value
    // The types are found in the symbol table
    virtual cDeclNode *GetType()
    {
        if (mIntVal >= -128 && mIntVal <= 127)
            return symbolTableRoot->Lookup("char")->GetType();
        else
            return symbolTableRoot->Lookup("int")->GetType();
    }

    virtual std::string toString()
    {
        std::string result("(EXPR: ");
        result += std::to_string(mIntVal);
        result += ")";

        return result;
    }

    virtual void GenerateCode()
    {
        EmitInt(PUSH_OP);
        EmitInt(mIntVal);
    }
  protected:
    int mIntVal;        // value of integer constant
};
