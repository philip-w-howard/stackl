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

class cVarDerefNode : public cVarRefNode
{
    public:
        cVarDerefNode(cVarRefNode *var) : cVarRefNode(*var) 
    {
    }

        virtual std::string toString()
        {
            std::string result = "*";
            result += mList->back()->toString();

            return result;
        }

        virtual void GenerateCode()
        {
            int deref_size = -1;
            if(mList->back()->GetType()->GetBaseType()->IsPointer())
                deref_size = mList->back()->GetType()->GetBaseType()->GetPointsTo()->Size();
            else if(mList->back()->GetType()->IsArray())
                deref_size = mList->back()->GetType()->GetBaseType()->Size();

            if (mList->back()->IsGlobal())
            {
                EmitInt(PUSH_OP);
                EmitGlobalRef(mList->back()->Name());
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

        virtual void GenerateLVal()
        {
            // places value at top of stack into *x
            cVarPartNode *last = mList->back();

            if (last->IsArrayRef() || last->GetDecl()->IsArray())
            {
                EmitOffset();
                EmitInt(POPCVARIND_OP);
            }
            else 
            {
                //std::cout << last->Name() << " is not an array ref or array...\n";
                if (mList->front()->IsGlobal())
                {
                    EmitInt(PUSH_OP);
                    EmitGlobalRef(mBaseName);
                    EmitInt(PUSH_OP);
                    EmitInt(mOffset);
                    EmitInt(PLUS_OP);
                    EmitInt(PUSHVARIND_OP);
                    EmitInt(POPVARIND_OP);
                } else {
                    EmitInt(POPVARIND_OP);
                    EmitInt(mOffset);
                }
            }
        }
};

