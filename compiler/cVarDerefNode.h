#pragma once

#include <string>
#include <list>
using std::list;

#include "cAstNode.h"
#include "cExprNode.h"
#include "cAstList.h"
#include "cStructDeclNode.h"
#include "cArrayValNode.h"
#include "cVarDeclNode.h"
#include "cSymbolTable.h"
#include "cVarRefNode.h"
#include "parse.h"
#include "codegen.h"

class cVarDerefNode : public cExprNode
{
    public:
        cVarDerefNode(cVarRefNode *var) 
        {
            mVar = var;
            mOffset = 0;
        }

        cDeclNode *GetType()
        {
            return mVar->GetType();
        }

        virtual int ComputeOffsets(int base)
        {
            mVar->ComputeOffsets(base);
            mOffset = mVar->GetOffset();
            return base;
        }

        virtual std::string toString()
        {
            std::string result = "&";
            result += mVar->toString();

            return result;
        }

        virtual void GenerateCode()
        {
            int deref_size = -1;
            if(mVar->GetType()->GetBaseType()->IsPointer())
                deref_size = mVar->GetType()->GetBaseType()->GetPointsTo()->Size();
            else if(mVar->GetType()->IsArray())
                deref_size = mVar->GetType()->GetBaseType()->Size();

            if (mVar->IsGlobal())
            {
                EmitInt(PUSH_OP);
                EmitGlobalRef(mVar->Name());
                EmitInt(PUSH_OP);
                EmitInt(mOffset);
                EmitInt(PLUS_OP);
                EmitInt(PUSHVARIND_OP);
                if(deref_size == 1)
                    EmitInt(PUSHCVARIND_OP);
                else
                    EmitInt(PUSHVARIND_OP);
            } else {

                EmitInt(PUSHVAR_OP);
                EmitInt(mOffset);
                if(deref_size == 1)
                    EmitInt(PUSHCVARIND_OP);
                else
                    EmitInt(PUSHVARIND_OP);
            }
        }

    protected:
        cVarRefNode *mVar;
        int mOffset;
};

