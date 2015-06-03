#pragma once
//*******************************************************
// Purpose: Class for a structure type delcaration
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>

#include "cAstNode.h"
#include "cDeclsNode.h"
#include "cDeclNode.h"
#include "cSymbol.h"
#include "cScope.h"

class cStructDeclNode : public cDeclNode
{
  public:
    cStructDeclNode(cScope *symTab, cDeclsNode *decls, cSymbol *id) 
        : cDeclNode()
    {
        mSymTab = symTab;
        mId = id;
        mDecls = decls;
        id->SetDecl(this);
        mSize = 0;

        symbolTableRoot->Insert(id);
    }

    virtual cDeclNode* GetType() { return this; }
    virtual bool IsStruct() { return true; }
    virtual bool IsType()   { return true; }

    // Lookup a field name to see if it is a member of this struct
    cSymbol *Lookup(std::string id)
    {
        return mSymTab->Lookup(id);
    }

    virtual int ComputeOffsets(int base)
    {
        // struct decls don't occupy memory, so don't update base
        mSize = mDecls->ComputeOffsets(0);
        return base;
    }

    virtual std::string toString()
    {
        std::string result("STRUCT: ");
        result += mDecls->toString();
        result += " " + mId->toString();
        if (mSize != 0) result += " size: " + std::to_string(mSize);

        return result;
    }

    virtual void GenerateCode()
    {
        mDecls->GenerateCode();
    }

  protected:
    cScope *mSymTab;        // symbol table for this struct
    cDeclsNode *mDecls;     // list of fields
};

