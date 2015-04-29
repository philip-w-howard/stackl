#pragma once
//*******************************************************
// Purpose: Class to represent a block
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>

#include "cStmtNode.h"
#include "cStmtsNode.h"
#include "cDeclsNode.h"
#include "codegen.h"

// blocks can be statements (e.g. if (cond) {block}
class cBlockNode : public cStmtNode
{
    public:
        cBlockNode(cDeclsNode *decls, cStmtsNode *stmts) : cStmtNode()
    {
        mDecls = decls;
        mStmts = stmts;
    }

        virtual int ComputeOffsets(int base)
        {
            int offset = base;
            if (mDecls != NULL) offset = mDecls->ComputeOffsets(offset);
            if (mStmts != NULL) offset = mStmts->ComputeOffsets(offset);

            mSize = offset - base;
            // blocks take no room in memory, so return the original base
            return base;
        }

        virtual std::string toString()
        {
            std::string result("BLOCK: ");
            if (mSize != 0) result += "size: " + std::to_string(mSize);
            result += "\n{\n";
            if (mDecls != NULL) result += mDecls->toString();
            if (mStmts != NULL) result += mStmts->toString();
            result += "}\n";

            return result;
        }

        virtual void GenerateCode()
        {
            // TODO optimize for zero-sized blocks 
            if(mSize != 0)
            {
                EmitInt(ADJSP_OP);
                EmitInt(mSize);
            }
            if (mDecls != NULL)
            {
                mDecls->GenerateCode();
            }
            if (mStmts != NULL) mStmts->GenerateCode();
            if(mSize != 0)
            {
                EmitInt(ADJSP_OP);
                EmitInt(mSize);
            }
        }
    protected:
        cDeclsNode *mDecls;     // declarations
        cStmtsNode *mStmts;     // statements
        int mSize;              // size of declarations
};

