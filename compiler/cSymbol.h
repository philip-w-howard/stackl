#pragma once
//*******************************************************
// Purpose: a symbol e.g. variable, type, etc.
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>
#include "cAstNode.h"

class cDecl;

class cSymbol : public cAstNode
{
  public:
    // create a Symbol for the given name n
    cSymbol(std::string n) : cAstNode()
    {
        name = n; 
        sequence = ++totalSymbols;
        mDecl = NULL;
    }

    // return the name of the symbol
    std::string Name() {return name;}
    int GetSeq()        { return sequence; }

    // return the sequence number. Used in codegen for functions
    int Id() { return sequence; }

    void SetDecl(cDecl *decl)
    {
        mDecl = decl;
    }

    cDecl *GetDecl()
    {
        return mDecl;
    }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

  protected:
    std::string name;           // name of the symbol
    int sequence;               // unique ID for symbol
    cDecl *mDecl;               // The decl that defines this symbol
    static int totalSymbols;    // STATIC: keeps track of total number of
                                // symbols that have been created. Used
                                // to assign sequence: unique ID
};
