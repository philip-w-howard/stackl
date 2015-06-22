//*******************************************************
// Purpose: Base class for declarations.
//          This code could not go in the .h file because of circular
//          dependency with cSymbol.h
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include "cDeclNode.h"
#include "cSymbol.h"

// Return the name of the declared item
std::string cDeclNode::Name() 
{ 
    return mId->Name(); 
}

// Return a string version of the ID. Used in some toStrings
std::string cDeclNode::TypeId()
{
    return mId->toString();
}
