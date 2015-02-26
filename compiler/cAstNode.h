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

class cAstNode
{
  public:
    cAstNode() {mSemanticError = false;}

    // return a string representation of the class
    virtual std::string toString() = 0;

    // return true if a semantic error was detected in this node
    virtual bool SemanticError() { return mSemanticError; }

    // Compute the size and offsets for any declarations or varrefs
    // The default behavior does nothing. Subclasses with decls or refs
    // need to override this function.
    virtual int ComputeOffsets(int base)
    { return base; }

    // Generate code for this node.
    // Subclasses that are responsible for actually generating code
    // need to override this function.
    virtual void GenerateCode() {}
  protected:
    bool mSemanticError;        // true indicates this node has a semantic error
};

