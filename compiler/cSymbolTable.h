#pragma once
//*******************************************************
// Purpose: Implementation of nested symbol table
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include "cSymbol.h"
#include "cScope.h"

// Declare global symbol table
class cSymbolTable;
extern cSymbolTable *symbolTableRoot;

class cSymbolTable
{
  public:
    cSymbolTable();

    // Insert a symbol into the table. If symbol already exists, returns
    // the preexisting symbol. Otherwise, return new symbol
    cSymbol *Insert(cSymbol *symbol);

    cSymbol *InsertRoot(cSymbol *symbol);
    // Look for a symbol. Returns NULL if symbol is not found.
    cSymbol *Lookup(std::string name);

    // lookup a symbol in the local ST without looking at parent
    cSymbol *LocalLookup(std::string name);

    // Increase the scoping level. 
    // A new table is created with a pointer to the old (higher scope) table 
    // Returns pointer to new table.
    cScope *IncreaseScope();

    // Lower the scoping level.
    // Returns a pointer to the current (after decrease) table
    cScope *DecreaseScope();

    // create a default table with symbols for base types
    void InitDefaultTable();
  protected:
    // pointer to a hash table used to store info
    cScope *mScope;
};
