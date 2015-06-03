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

class cVarDeclNode : public cDeclNode
{
  public:
    cVarDeclNode(cSymbol *type, cSymbol *id, bool isGlobal = false) 
        : cDeclNode()
    {
        mSize = 0;
        mOffset = 0;
        mGlobal = isGlobal;
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

        mType = type->GetType();
        mId = id;
        mId->SetDecl(this);
    }

    void SetGlobal()
    {
        mGlobal = true;
    }

    virtual cDeclNode *GetType()
    {
        return mType;
    }

    virtual bool IsArray()  { return mType->IsArray(); }
    virtual bool IsStruct() { return mType->IsStruct(); }
    virtual bool IsInt()    { return mType->IsInt(); }
    virtual bool IsGlobal() { return mGlobal; }
    virtual cSymbol* GetVar() { return mId; }

    virtual int ComputeOffsets(int base)
    {
        mType->ComputeOffsets(base);
        if(mType->GetType() == symbolTableRoot->Lookup("char")->GetDecl())
            mOffset = base;
        else if(base % WORD_SIZE != 0)
            mOffset = base / WORD_SIZE * WORD_SIZE + WORD_SIZE;
        else
            mOffset = base;
        mSize = ComputeSize();

        return mOffset+mSize;
    }

    virtual int ComputeNegOffsets(int base)
    {
        mType->ComputeOffsets(base);
        mSize = ComputeSize();
        mOffset = base - mSize;

        return mOffset;
    }

    virtual void GenerateCode()
    {
        if (mGlobal) EmitGlobalDef(mId->Name(), ComputeSize());
    }

    virtual std::string toString()
    {
        std::string result;
        if (mGlobal)
            result = "GLOBAL VAR: ";
        else
            result = "VAR: ";
        result += mType->TypeId();
        result += " " + mId->toString();
        if (mSize != 0 || mOffset != 0)
        {
            result += " size: " + std::to_string(mSize);
            result += " offset: " + std::to_string(mOffset);
        }

        return result;
    }

  protected:
    cDeclNode *mType;   // the type for the decl
                        // NOTE: this class inherits members from cDeclNode
    bool mGlobal;       // true if this is a global variable
    int ComputeSize()
    {
        int size;

        if (mType->IsArray())
        {
            size = mType->GetArrayElementType()->Size();
            cArrayDeclNode *array = dynamic_cast<cArrayDeclNode *>(mType);
            size *= array->NumElements();
            size = ((size + WORD_SIZE - 1)/WORD_SIZE) * WORD_SIZE;
        }
        else
        {
            size = mType->Size();
        }
        return size;
    }
};

