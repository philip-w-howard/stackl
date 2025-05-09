#pragma once

#include "cStmt.h"

class cContinueStmt : public cStmt
{
    public:
        cContinueStmt() : cStmt() {}

        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};