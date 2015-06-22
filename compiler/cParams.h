#pragma once
//*******************************************************
// Purpose: Base class for list of expressions
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
#include "cExpr.h"

using std::list;

class cParams : public cAstNode
{
  public:
    // constructor places first decl in list
    cParams(cExpr *expr) : cAstNode()
    {
        mList = new list<cExpr *>();
        mList->push_back(expr);
    }

    // add a declaration to the list
    virtual void AddNode(cExpr *expr) 
    {
        mList->push_back(expr);
    }

    virtual int Size()
    {
        int size = 0;
        for (list<cExpr *>::iterator it = mList->begin(); 
            it != mList->end(); it++)
        {
            if ( (*it)->GetType()->IsArray())
                size += WORD_SIZE;
            else
                size += (*it)->Size();
        }

        return size;
    }

    virtual std::string toString()
    {
        std::string result;
        result = ListToString<cExpr *>(mList, false);

        return result;
    }

    virtual int ComputeOffsets(int base)
    {
        for (list<cExpr *>::iterator it = mList->begin(); 
            it != mList->end(); it++)
        {
            base = (*it)->ComputeOffsets(base);
        }

        return base;
    }

    // generate code in reverse list order (right to left)
    virtual void GenerateCode()
    {
        list<cExpr *>::iterator it = mList->end(); 
        do
        {
            it--;
            (*it)->GenerateCode();
        } while (it != mList->begin());
    }

  protected:
    list<cExpr *> *mList;       // list of delcarations
};
