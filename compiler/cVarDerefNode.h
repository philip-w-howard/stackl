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
            int deref_size;
            if(mList->back()->GetType()->GetBaseType()->IsPointer())
                deref_size = mList->back()->GetType()->GetBaseType()->GetPointsTo()->Size(); 
            else
                deref_size = mList->back()->GetType()->GetBaseType()->GetBaseType()->Size();

            if (mList->back()->IsGlobal())
            {
                EmitInt(PUSH_OP);
                EmitGlobalRef(mList->back()->Name());
                EmitInt(PUSH_OP);
                EmitInt(mOffset);
                EmitInt(PLUS_OP);
                EmitInt(PUSHVARIND_OP);
            }
            else
            {
                EmitInt(PUSHVAR_OP);
                EmitInt(mOffset);
            }
            if(deref_size == 1)
                EmitInt(PUSHCVARIND_OP);
            else
                EmitInt(PUSHVARIND_OP);
        }

        virtual void GenerateLVal()
        {
            // places value at top of stack into *x
            cVarPartNode *last = mList->back();

            //std::cout << "generating left-side deref...\n";
            if (last->IsArrayRef() || last->GetDecl()->IsArray())
            {
                int deref_size;
                if(last->GetType()->GetBaseType()->IsPointer())
                    deref_size = last->GetType()->GetBaseType()->GetPointsTo()->Size(); 
                else
                    deref_size = last->GetType()->GetBaseType()->GetBaseType()->Size();

                //std::cout << last->Name() << " is an array ref, refing " << last->GetType()->GetBaseType()->GetBaseType()->Name()  << " of size " << deref_size  << "...\n";

                EmitOffset();

                if(deref_size == 1) EmitInt(POPCVARIND_OP);
                else EmitInt(POPVARIND_OP);
            }
            else 
            {
                int deref_size;
                if(last->GetType()->GetBaseType()->IsPointer())
                    deref_size = last->GetType()->GetBaseType()->GetPointsTo()->Size(); 
                else
                    deref_size = last->GetType()->GetBaseType()->GetBaseType()->Size();

                if (mList->front()->IsGlobal())
                {
                    EmitInt(PUSH_OP);
                    EmitGlobalRef(mBaseName);
                    EmitInt(PUSH_OP);
                    EmitInt(mOffset);
                    EmitInt(PLUS_OP);
                    EmitInt(PUSHVARIND_OP);
                } else {
                    // push address
                    EmitInt(PUSHVAR_OP);
                    EmitInt(mOffset);
                }

                // pop SP - 2 (expr val) into SP - 1 (*ptr)
                if(deref_size == 1)
                    EmitInt(POPCVARIND_OP);
                else
                    EmitInt(POPVARIND_OP);

            }
        }
};

