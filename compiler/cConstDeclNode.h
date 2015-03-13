#pragma once
//*******************************************************
// Purpose: Class for a variable declaration
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>

#include "cAstNode.h"
#include "cDeclNode.h"
#include "cStructDeclNode.h"
#include "cArrayDeclNode.h"
#include "cSymbol.h"

#include "parse.h"

class cConstDeclNode : public cDeclNode
{
  public:
    cConstDeclNode(cSymbol *id, int value) : cDeclNode()
    {
        mSize = 0;
        mOffset = 0;
        cSymbol *localId = symbolTableRoot->LocalLookup(id->Name());
        if (localId != NULL)
        {
            semantic_error("Symbol " + id->Name() +
                    " already defined in current scope");
        }
        // We may have an id from an outter scope. This will either
        // create a new symbol or return the already existing symbol
        // in the current scope.
        id = symbolTableRoot->Insert(id);

        mType = symbolTableRoot->Lookup("int")->GetType();
        mId = id;
        mId->SetType(this);
        mValue = value;
    }

    // return the decl of the type
    virtual cDeclNode *GetBaseType()
    {
        return mType;
    }

    virtual bool IsInt()    { return true; }
    virtual bool IsConst()    { return true; }

    int GetValue()          { return mValue; }

    virtual int ComputeOffsets(int base)
    {
        return base;
    }

    virtual std::string toString()
    {
        std::string result("CONST: ");
        result += mId->toString();
        result += " " + std::to_string(mValue);

        return result;
    }

    virtual void GenerateCode()
    {
        EmitInt(PUSH_OP);
        EmitInt(mValue);
    }

  protected:
    cDeclNode *mType;   // the type for the decl
                        // NOTE: this class inherits members from cDeclNode
    int mValue;         // The value of the constant
};

