#pragma once
//*******************************************************
// Purpose: Hash table used for symbol tables.
//          This class encapsulates an unordered_map
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include "cSymbol.h"
#include <unordered_map>

using std::unordered_map;

class cScope
{
  public:
    cScope(cScope *p) : mSymTab()
    {
        mParent = p;
    }

    // Insert a symbol into the table. 
    void Insert(std::string name, cSymbol *symbol)
    {
        mSymTab[name] = symbol;
    }

    // Remove a symbol from the table.
    void Remove(cSymbol *symbol)
    {
        mSymTab.erase(symbol->Name());
    }

    // Look for a symbol. Returns NULL if symbol is not found.
    cSymbol *Lookup(std::string name)
    {
        unordered_map<std::string, cSymbol*>::const_iterator found;
        found = mSymTab.find(name);
        if (found != mSymTab.end()) return found->second;

        return NULL;
    }

    // return parent of this scope. Used when popping a nested symbol table
    cScope *Parent()
    {
        return mParent;
    }

  protected:
    // pointer to a hash table used to store info
    unordered_map<std::string, cSymbol*> mSymTab;

    cScope *mParent;      // Pointer to higher scope table
};
