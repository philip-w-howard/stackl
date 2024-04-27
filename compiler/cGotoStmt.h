#pragma once

#include <string>
#include "cStmt.h"
#include "cSymbolTable.h"

class cGotoStmt : public cStmt
{
    public:
        cGotoStmt(cSymbol *symbol) : cStmt(), mLabel(symbol->Name())
        {
            // if this identifier was made specifically to be a label, remove
            // it from the symbol table so that it's free to be an identifier
            if (symbol->GetDecl() == nullptr)
                symbolTableRoot->Remove(symbol);
        }

        string GetLabel() { return mLabel; }

        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
    private:
        string mLabel;
};