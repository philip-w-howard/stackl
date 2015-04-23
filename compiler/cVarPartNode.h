#pragma once
//*******************************************************
// Purpose: Class for a variable part. 
//          In struct refs, there is one varpart for each field referenced
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>

#include "cAstNode.h"
#include "cArrayValNode.h"
#include "cSymbol.h"
#include "parse.h"

class cVarPartNode : public cAstNode
{
  public:
    cVarPartNode(cSymbol *var, cArrayValNode *arrayPart) : cAstNode()
    {
        mId = var;
        mArrayPart = arrayPart;     // NULL if this isn't an array ref
        mDecl = mId->GetType();
    }

    // return the declaration for this item
    virtual cDeclNode *GetDecl()
    {
        return mDecl;
    }

    virtual bool IsGlobal()
    {
        return mDecl->IsGlobal();
    }

    // Gets called by cVarRefNode::AddNode for structs
    void SetDecl(cDeclNode *decl)
    {
        mDecl = decl;
    }

    // Gets called by cVarRefNode::AddNode for structs
    void UpdateSymbol(cSymbol *sym)
    {
        mId = sym;
    }

    // set/get the decl for the field
    void SetField(cDeclNode *field) { mFieldDecl = field; }
    cDeclNode *GetField() { return mFieldDecl; }

    bool IsArrayRef() { return mArrayPart != NULL; }
    cArrayValNode *GetArrayVal() { return mArrayPart; }

    std::string Name() { return mId->Name(); }

    virtual std::string toString()
    {
        std::string result("(VarRef: ");
        result += mId->toString();
        if (mArrayPart != NULL) result += "[" + mArrayPart->toString() + "]";

        return result;
    }

    bool IsArray()
    {
        // TODO why is this function necessary?
        return mId->GetType()->IsArray();
    }

  protected:
    cSymbol *mId;               // identifier for this item
    cArrayValNode *mArrayPart;  // array indeces (NULL if not an array)
    cDeclNode *mDecl;           // type of item. For structs this is the 
                                // struct decl
    cDeclNode *mFieldDecl;      // for structs, this is the vardecl for 
                                // this item
};

