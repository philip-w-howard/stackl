#pragma once
//************************************************
// cAstXml
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Visitor to print the AST in XML form
//
#include <iostream>
#include <fstream>

#include "cVisitor.h"

class cSizeOffset : public cVisitor
{
    public:
        cSizeOffset();

        virtual void VisitAllNodes(cAstNode *node);

        virtual void Visit(cFuncDecl *node);
        virtual void Visit(cStructType *node);
        virtual void Visit(cVarDecl *node);
    protected:
        int m_offset;
        int RoundUp(int val);
};
