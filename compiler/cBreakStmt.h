#pragma once

#include "cStmt.h"

class cBreakStmt : public cStmt
{
    public:
        cBreakStmt() : cStmt() {}

        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};