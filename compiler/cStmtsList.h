#pragma once
//*******************************************************
// Purpose: Base class for list of statements (incl decls)
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

#include "cStmt.h"
#include "cAstList.h"
#include "cDecl.h"

using std::list;

class cStmtsList : public cStmt
{
  public:
    // constructor places first decl in list
    cStmtsList(cStmt *stmt) : cStmt()
    {
        mList = new list<cStmt *>();
        mList->push_back(stmt);
    }

    // add a declaration to the list
    virtual void AddNode(cStmt *stmt) 
    {
        mList->push_back(stmt);
    }

    // add a list of stmts to this list
    void AddList(cStmtsList *stmts)
    {
        for (list<cStmt *>::iterator it = stmts->mList->begin(); 
            it != stmts->mList->end(); it++)
        {
            AddNode( (*it) );
        }
    }

    virtual int Size()
    {
        int size = 0;
        for (list<cStmt *>::iterator it = mList->begin(); 
            it != mList->end(); it++)
        {
            size += (*it)->Size();
        }

        return size;
    }

    virtual std::string toString()
    {
        std::string result("STMTS:\n{\n");
        result += ListToString<cStmt *>(mList, true);
        result += "}\n";

        return result;
    }

    virtual int ComputeOffsets(int base)
    {
        for (list<cStmt *>::iterator it = mList->begin(); 
            it != mList->end(); it++)
        {
            base = (*it)->ComputeOffsets(base);
        }

        return base;
    }

    virtual void GenerateCode()
    {
        for (list<cStmt *>::iterator it = mList->begin(); 
            it != mList->end(); it++)
        {
            (*it)->GenerateCode();
        }
    }

  protected:
    list<cStmt *> *mList;       // list of delcarations
};

