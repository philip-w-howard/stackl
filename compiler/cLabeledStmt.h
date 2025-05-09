#pragma once

#include <string>
#include "cStmt.h"
#include "cSymbolTable.h"

class cLabeledStmt : public cStmt
{
    public:
        cLabeledStmt(cSymbol *symbol, cStmt *stmt) : cStmt(), mLabel(symbol->Name())
        {
            // if this identifier was made specifically to be a label, remove
            // it from the symbol table so that it's free to be an identifier
            if (symbol->GetDecl() == nullptr)
                symbolTableRoot->Remove(symbol);
            AddChild(stmt);
        }

        string GetLabel() { return mLabel; }
        cStmt* GetStmt() { return (cStmt*)GetChild(0); }

        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
    private:
        string mLabel;
};