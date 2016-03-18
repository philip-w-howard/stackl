#pragma once
//*******************************************************
// Purpose: Base class for list of declarations
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

#include "cStmtsList.h"
#include "cDecl.h"
#include "cTypeDecl.h"

using std::list;

class cDeclsList : public cStmtsList
{
  public:
    // constructor places first decl in list
    cDeclsList(cDecl *decl) : cStmtsList(decl)
    {
    }

    // add a declaration to the list
    virtual void AddNode(cDecl *decl) 
    {
        AddChild(decl);
    }

    cDecl* GetDecl(int index)   { return (cDecl*)GetChild(index); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};

