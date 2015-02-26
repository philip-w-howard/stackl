#pragma once
//*******************************************************
// Purpose: list of formal params to a function
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
#include "cVarDeclNode.h"

class cParamsSpecNode: public cAstNode
{
  public:
    // place the first param in the list
    cParamsSpecNode(cVarDeclNode *param) : cAstNode()
    {
        mList = new list<cVarDeclNode *>();
        mList->push_back(param);
    }

    // Add another param to the list
    void AddNode(cVarDeclNode *param) 
    {
        mList->push_back(param);
    }

    // return the number of params
    int NumParams()
    { return mList->size(); }

    // return a specific param
    cVarDeclNode *GetParam(int index)
    {
        return ListGetItem(mList, index);
    }

    virtual int ComputeOffsets(int base)
    {
        for (list<cVarDeclNode *>::iterator it = mList->begin(); 
            it != mList->end(); it++)
        {
            base = (*it)->ComputeOffsets(base);
        }
        return base;
    }

    virtual std::string toString()
    {
        std::string result("(PARAMS:");
        result += ListToString(mList, false);
        result += ")\n";

        return result;
    }

  protected:
    list<cVarDeclNode *> *mList;    // list of formal params
};

