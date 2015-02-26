#pragma once
//*******************************************************
// Purpose: Class for array type declarations
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
#include "cArraySpecNode.h"
#include "cSymbol.h"

class cArrayDeclNode : public cDeclNode
{
  public:
    cArrayDeclNode(cSymbol *type, cSymbol *id, cArraySpecNode *array) 
        : cDeclNode()
    {
        mSize = 0;

        // We may have an id from an outter scope. This will either
        // create a new symbol or return the already existing symbol
        // in the current scope.
        id = symbolTableRoot->Insert(id);

        mType = type->GetType();
        mId = id;
        mId->SetType(this);
        mArraySpec = array;
    }

    // return the type of an element of the array
    virtual cDeclNode *GetBaseType()
    {
        return mType;
    }

    virtual bool IsArray()  { return true; }
    virtual bool IsType()   { return true; }

    virtual int ComputeOffsets(int base)
    {
        mSize = mType->Size() * mArraySpec->NumElements();

        return base;
    }

    virtual std::string toString()
    {
        std::string result("ARRAY: ");
        result += mType->toString();
        result += " " + mId->toString();
        result += " " + mArraySpec->toString();

        if (mSize != 0) result += " size: " + std::to_string(mSize);

        return result;
    }

  protected:
    cDeclNode      *mType;      // type of array elements
    cArraySpecNode *mArraySpec; // specification of indeces
};

