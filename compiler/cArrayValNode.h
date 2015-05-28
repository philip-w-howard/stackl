#pragma once
//*******************************************************
// Purpose: Class to specify the dimentions is an array reference
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
#include "cArraySpecNode.h"

class cArrayValNode: public cAstNode
{
  public:
    // expr is the value of the first index
    cArrayValNode(cExprNode *expr) : cAstNode()
    {
        mList = new list<cExprNode *>();
        mList->push_back(expr);
    }

    // add another index
    void AddNode(cExprNode *expr) 
    {
        mList->push_back(expr);
    }

    // return the number of indexes
    int GetCount()
    {
        return mList->size();
    }

    cExprNode *GetIndex(int index)
    {
        return mList->front();
    }

    int ComputeOffsets(int base)
    {
        ListComputeOffsets(mList, base);

        return base;
    }

    virtual std::string toString()
    {
        std::string result("(ARRAYVAL:");
        result += ListToString(mList, false);
        result += ")";

        return result;
    }

  protected:
    list<cExprNode *> *mList;   // list of index values
};

