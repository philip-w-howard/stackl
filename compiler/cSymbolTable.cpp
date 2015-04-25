//*******************************************************
// Purpose: Manage the Symbol Tables
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <assert.h>
#include "cSymbol.h"
#include "cScope.h"
#include "cSymbolTable.h"
#include "cBaseDeclNode.h"

int cSymbol::totalSymbols = 0;              // total symbols created
cSymbolTable *symbolTableRoot;              // global symbol table

//*******************************************
// Create a new, empty symbol table
cSymbolTable::cSymbolTable() 
{
    mScope = new cScope(NULL);
}
//*******************************************
// create and return new symbol.
// If symbol already exists, return pointer to that symbol instead of creating
// a new one.
cSymbol *cSymbolTable::Insert(cSymbol *symbol)
{
    // only check if symbol is in local table.
    // If we are defining a symbol that exists in an outer scope,
    // we still want to define a symbol in this scope.
    cSymbol *localSymbol = LocalLookup(symbol->Name());

    // if symbol doesn't already exist, create a new one
    if (localSymbol == NULL)
    {
        cSymbol *globalSymbol = Lookup(symbol->Name());

        // if either the symbol doesn't exist at all or if
        // we have a different symbol from one with the same name
        if (globalSymbol == NULL || globalSymbol != symbol)
        {
            mScope->Insert(symbol->Name(), symbol);
        } else {
            // we need to create a new symbol so that we don't 
            // have the same actual symbol two places in the symbol table
            symbol = new cSymbol(symbol->Name());
            assert(symbol != NULL);

            mScope->Insert(symbol->Name(), symbol);
        }
    }

    return symbol;
}
//*******************************************
cSymbol *cSymbolTable::InsertRoot(cSymbol *symbol)
{
    cScope *scope = mScope;
    while(scope->Parent() != NULL)
        scope = scope->Parent();

    scope->Insert(symbol->Name(), symbol);

    return symbol;
}
//*******************************************
// Look for a symbol. Return NULL if not found
cSymbol *cSymbolTable::Lookup(std::string name)
{
    cScope *scope = mScope;
    cSymbol *symbol = NULL;
    while (symbol == NULL && scope != NULL)
    {
        symbol = scope->Lookup(name);
        scope = scope->Parent();
    }

    return symbol;
}
//*******************************************
// lookup a symbol in the local ST without looking at parent
cSymbol *cSymbolTable::LocalLookup(std::string name)
{
    return mScope->Lookup(name);
}
//*******************************************
// Increase the scoping level. Save pointer to old (outer) scoping level.
// Return pointer to new symtab
cScope *cSymbolTable::IncreaseScope()
{
    mScope = new cScope(mScope);
    return mScope;
}
//*******************************************
// Decrease scoping level.
// DON'T destroy the current table because the parse tree will have
// references to the symbols stored here.
cScope *cSymbolTable::DecreaseScope()
{
    // MEMORY LEAK:
    // we aren't deleteing the current table because parse tree has references
    // to our symbols
    
    mScope = mScope->Parent();
    return mScope;
}
//*******************************************
// create default symbol table including definitions for default types
cSymbolTable *cSymbolTable::CreateDefaultTable()
{
    cSymbolTable *defaultTable = new cSymbolTable();
    cSymbol *sym;

    sym = new cSymbol("char");
    defaultTable->Insert(sym);
    sym->SetType(new cBaseDeclNode(sym, 1, false, false, false));

    sym = new cSymbol("carray");
    defaultTable->Insert(sym);
    sym->SetType(new cBaseDeclNode(sym, 4, false, true, false));

    sym = new cSymbol("cconst");
    defaultTable->Insert(sym);
    sym->SetType(new cBaseDeclNode(sym, 4, false, false, true));

    sym = new cSymbol("int");
    defaultTable->Insert(sym);
    sym->SetType(new cBaseDeclNode(sym, 4, false, false, false));

    return defaultTable;
}

