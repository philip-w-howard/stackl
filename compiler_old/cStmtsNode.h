#pragma once
//*******************************************************
// Purpose: Class for a list of statements
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

class cStmtsNode : public cAstNode
{
  public:
    // create the list and insert the first statement
    cStmtsNode(cStmtNode *stmt) : cAstNode()
    {
        mList = new list<cStmtNode *>();
        if (stmt != NULL) mList->push_back(stmt);
    }

    // add a statement to the list
    void AddNode(cStmtNode *stmt) 
    {
        if (stmt != NULL) mList->push_back(stmt);
    }

    virtual int ComputeOffsets(int base)
    {
        for (list<cStmtNode *>::iterator it = mList->begin(); 
                it != mList->end(); it++)
        {
            base = (*it)->ComputeOffsets(base);
        }
        return base;
    }

    virtual std::string toString()
    {
        std::string result("STMTS:\n{\n");
        result += ListToString(mList, true);
        result += "}\n";

        return result;
    }

    virtual void GenerateCode()
    {
        for (list<cStmtNode *>::iterator it = mList->begin(); 
                it != mList->end(); it++)
        {
            (*it)->GenerateCode();
        }
    }
  protected:
    list<cStmtNode *> *mList;   // list of statements
};

