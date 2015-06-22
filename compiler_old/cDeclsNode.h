#pragma once
//*******************************************************
// Purpose: Base class for all declarations
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>
#include <list>
using std::list;

#include "cAstNode.h"
#include "cAstList.h"
#include "cDeclNode.h"

using std::list;

class cDeclsNode : public cAstNode
{
  public:
    // constructor places first decl in list
    cDeclsNode(cDeclNode *decl) : cAstNode()
    {
        mSize = 0;
        mList = new list<cDeclNode *>();
        mList->push_back(decl);
    }

    // add a declaration to the list
    void AddNode(cDeclNode *decl) 
    {
        mList->push_back(decl);
    }

    // add a list of decls to this list
    void AddList(cDeclsNode *decls)
    {
        for (list<cDeclNode *>::iterator it = decls->mList->begin(); 
            it != decls->mList->end(); it++)
        {
            AddNode( (*it) );
        }
    }

    // return total size of all decls.
    // ComputeOffsets must be called before this has meaning
    int Size() { return mSize; }

    virtual int ComputeOffsets(int base)
    {
        int offset = base;
        for (typename list<cDeclNode *>::iterator it = mList->begin(); 
            it != mList->end(); it++)
        {
            offset = (*it)->ComputeOffsets(offset);
        }
        mSize = offset - base;
        return offset;
    }

    virtual std::string toString()
    {
        std::string result("DECLS:\n{\n");
        result += ListToString<cDeclNode *>(mList, true);
        result += "}\n";

        return result;
    }

    virtual void GenerateCode()
    {
        // decls might contain functions, so we need to loop through them.
        for (list<cDeclNode *>::iterator it = mList->begin(); 
            it != mList->end(); it++)
        {
            (*it)->GenerateCode();
        }
    }

  protected:
    list<cDeclNode *> *mList;       // list of delcarations
    int mSize;                      // total size of decls
};

