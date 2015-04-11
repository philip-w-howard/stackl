#pragma once
//*******************************************************
// Purpose: Class for a SIZE_OF statement
//
// Author: Ryan Fox
// Email:  ryan.fox@oit.edu
//
// Date: 4/10/2015
//
//*******************************************************

#include <string>

#include "cSymbol.h"
#include "cStmtNode.h"
#include "parse.h"
#include "codegen.h"

class cSizeOfNode : public cExprNode
{
    public:
        cSizeOfNode(cSymbol *id)
        {
            mType = id->GetType()->GetBaseType();
            mSize = mType->Size();
        }

        cSizeOfNode(bool isChar)
        {
            if(isChar)
            {
                mType = symbolTableRoot->Lookup("char")->GetType()->GetBaseType();
                mSize = mType->Size();
            }
            else
            {
                // ints are considered types, chars are just ints CHAR
                // for whatever reason, fix in future; as it stands, this
                // will never be invoked
                mType = symbolTableRoot->Lookup("int")->GetType();
                mSize = mType->Size();
            }
        }

        virtual std::string toString()
        {
            std::string result("SIZEOF");
            return result;
        }

        virtual int ComputeOffsets(int base)
        {
            return base;
        }

        virtual void GenerateCode()
        {
            EmitInt(PUSH_OP);
            EmitInt(mSize);
        }

        virtual cDeclNode* GetType()
        {
            return symbolTableRoot->Lookup("int")->GetType(); 
        }

    private:
        cDeclNode* mType;
        int mSize;
};

