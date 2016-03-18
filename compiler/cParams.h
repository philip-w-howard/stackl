#pragma once
//*******************************************************
// Purpose: Base class for list of expressions
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>
#include <list>
using std::list;

#include "cAstNode.h"
#include "cExpr.h"

using std::list;

class cParams : public cAstNode
{
  public:
    // constructor places first decl in list
    cParams(cExpr *expr) : cAstNode()
    {
        AddChild(expr);
    }

    // add a declaration to the list
    virtual void AddNode(cExpr *expr) 
    {
        AddChild(expr);
    }

    virtual int Size()
    {
        int size = 0;
        for (int ii=0; ii<NumChildren(); ii++)
        {
            if ( GetParam(ii)->GetType()->IsArray())
                size += WORD_SIZE;
            else
                size += GetParam(ii)->Size();
        }

        return size;
    }

    virtual int ComputeOffsets(int base)
    {
        for (cAstNode::iterator it = FirstChild();
            it != LastChild(); it++)
        {
            base = (*it)->ComputeOffsets(base);
        }

        return base;
    }

    // generate code in reverse list order (right to left)
    virtual void GenerateCode()
    {
        cAstNode::iterator it = LastChild(); 
        do
        {
            it--;
            (*it)->GenerateCode();
        } while (it != FirstChild());
    }

    cExpr* GetParam(int index) { return (cExpr*)GetChild(index); }

    virtual string NodeType()             { return "Params"; }
    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
