#pragma once
//*******************************************************
// Purpose: Base class for all AST nodes
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <iostream>
#include <string>
#include <cassert>

extern int yylineno;

// declare semantic_error here so all AST classes can call it 
// without including parser.h
int semantic_error(std::string msg);

void fatal_error(std::string msg);

class cAstNode
{
  public:
    cAstNode() 
    {
        mSemanticError = false;
        mLineNumber = yylineno;
    }

    // return the source line number from when the node was created
    int LineNumber() { return mLineNumber; }

    // return a string representation of the class
    virtual std::string toString() = 0;

    void ThrowSemanticError(std::string msg)
    {
        semantic_error(msg);
        mSemanticError = true;
    }

    // return true if a semantic error was detected in this node
    virtual bool HasSemanticError() { return mSemanticError; }

    // Compute the size and offsets for any declarations
    // The default behavior does nothing. Subclasses with decls
    // need to override this function.
    virtual int ComputeOffsets(int base)
    { return base; }

    // Generate code for this node.
    // Subclasses that are responsible for actually generating code
    // need to override this function.
    virtual void GenerateCode() {}
  protected:
    bool mSemanticError;        // true indicates this node has a semantic error
    int mLineNumber;            // line number of source when node was created
};

