#pragma once
//*******************************************************
// Purpose: A list of actual params passed to a function
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>
#include <list>

#include "cAstNode.h"
#include "cExprNode.h"
#include "cAstList.h"

using std::list;

class cParamNode: public cAstNode
{
  public:
    // place the first param in the list
    cParamNode(cExprNode *param) : cAstNode()
    {
        mList = new list<cExprNode *>();
        if (param != NULL) mList->push_back(param);
        mSize = 0;
    }

    // place the next param in the list
    void AddNode(cExprNode *param) 
    {
        mList->push_back(param);
    }

    // return a specific param
    cExprNode *GetParam(int index)
    {
        return ListGetItem(mList, index);
    }

    int NumParams()
    { return mList->size(); }

    // ComputeOffsets must be called before this is meaningful
    int Size() { return mSize; }

    virtual std::string toString()
    {
        std::string result("(PARAM:");
        result += ListToString(mList, false);
        result += ")\n";

        return result;
    }

    virtual int ComputeOffsets(int base)
    {
        mSize = 0;

        for (list<cExprNode *>::iterator it = mList->begin(); 
                it != mList->end(); it++)
        {
            // actual params take no space, so don't update base
            // but we do need to keep track of how much stack space we use
            mSize += 4;
            (*it)->ComputeOffsets(base);
        }
        return base;
    }

    virtual void GenerateCode()
    {
        // need to push args in reverse order
        for (list<cExprNode *>::reverse_iterator it = mList->rbegin(); 
                it != mList->rend(); it++)
        {
            (*it)->GenerateCode();
        }
    }

  protected:
    list<cExprNode *> *mList;       // list of parameters
    int mSize;
};

