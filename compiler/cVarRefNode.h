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
#include "cConstDeclNode.h"
#include "cSymbolTable.h"
#include "cVarPartNode.h"

#include "parse.h"
#include "codegen.h"

class cVarRefNode : public cExprNode
{
    public:
        cVarRefNode(cVarPartNode *var) 
        {
            cDeclNode *baseDecl;

            if (symbolTableRoot->Lookup(var->Name()) == NULL)
            {
                semantic_error("Symbol " + var->Name() + " not defined");
                mSemanticError = true;
            }
            else
            {
                mBaseName = var->Name();
                mName = var->Name();
                mList = new list<cVarPartNode *>();
                mList->push_back(var);
                baseDecl = var->GetDecl();
                if (!baseDecl->IsArray())
                    baseDecl = baseDecl->GetBaseType();
                if (baseDecl->IsArray() && var->IsArrayRef())
                    baseDecl = baseDecl->GetBaseType();
                mDepthDecl = baseDecl;
            }

        }

        void AddNode(cVarPartNode *field) 
        {
            if (!mDepthDecl->IsStruct())
            {
                semantic_error(mName + " is not a struct");
                mSemanticError = true;
            }
            else
            {
                cStructDeclNode *base = dynamic_cast<cStructDeclNode *>(mDepthDecl);
                cSymbol *sField = base->Lookup(field->Name());
                if (sField == NULL)
                {
                    semantic_error(field->Name() + " is not a field of " + mName);
                    mSemanticError = true;
                }
                else
                {
                    mDepthDecl = sField->GetType();
                    field->SetField(mDepthDecl);
                    if (!mDepthDecl->IsArray())
                        mDepthDecl = mDepthDecl->GetBaseType();
                    if (mDepthDecl->IsArray() && field->IsArrayRef())
                        mDepthDecl = mDepthDecl->GetBaseType();
                    mList->push_back(field);
                    field->UpdateSymbol(sField);
                    field->SetDecl(base);

                    mName += "." + field->Name();
                }
            }
        }

        virtual cDeclNode *GetType()
        {
            return mDepthDecl;
        }

        int GetOffset() { return mOffset; }

        std::string Name() { return mName; }

        virtual bool IsArrayRef()
        {
            return mList->back()->IsArrayRef();
        }
        virtual int ComputeOffsets(int base)
        {
            list<cVarPartNode *>::iterator it = mList->begin();

            (*it)->ComputeOffsets(base);

            mOffset = (*it)->GetDecl()->GetOffset();

            for ( it++; it != mList->end(); it++)
            {
                (*it)->ComputeOffsets(base);
                mOffset += (*it)->GetField()->GetOffset();
            }

            if (mDepthDecl->IsArray())
                mSize = mDepthDecl->GetBaseType()->Size();
            else
                mSize = mDepthDecl->Size();

            return base;
        }

        virtual std::string toString()
        {
            std::string result;
            if (mList != NULL) result += ListToString(mList, false);
            // Note: change in class heirarchy forces the ')' 
            // to be appended here insead of inside cVarPartNode.
            for (unsigned int ii=0; ii<mList->size() - 1; ii++)
            {
                result += ")";
            }
            if (mSize != 0 || mOffset != 0)
            {
                result += " size: " + std::to_string(mSize);
                result += " offset: " + std::to_string(mOffset);
            }
            result += ")";

            return result;
        }

        void EmitOffset()
        {
            // should leave address of variable on top of stack
            cVarPartNode *last = mList->back();

            if (last->IsArrayRef())
            {
                cArrayValNode *index = last->GetArrayVal();

                if(last->GetDecl()->GetBaseType()->IsPointer())
                {
                    // ptr[index] pushes (ptr + (size * index))
                    int deref_size = last->GetDecl()->GetBaseType()->GetPointsTo()->Size(); 
                    
                    EmitInt(PUSHVAR_OP);
                    EmitInt(mOffset);

                    EmitInt(PUSH_OP);
                    EmitInt(deref_size);
                    index->GetIndex(0)->GenerateCode();
                    EmitInt(TIMES_OP);

                    EmitInt(PLUS_OP);
                }
                else
                {
                    // arr[index] pushes ((FP + offset) + (size * index))
                    int deref_size = last->GetDecl()->GetBaseType()->GetBaseType()->Size();
                    
                    EmitInt(PUSH_OP);
                    EmitInt(mOffset);
                    EmitInt(PUSHFP_OP);
                    EmitInt(PLUS_OP);
                    EmitInt(PUSH_OP);
                    //EmitInt(1);
                    EmitInt(deref_size);
                    index->GetIndex(0)->GenerateCode();
                    EmitInt(TIMES_OP);

                    EmitInt(PLUS_OP);
                }
            }
            else if(last->GetDecl()->GetBaseType()->IsArray())
            {
                // arr pushes (FP + offset)
                EmitInt(PUSH_OP);
                EmitInt(mOffset);
                EmitInt(PUSHFP_OP);
                EmitInt(PLUS_OP);
            }
        }

        virtual void GenerateCode()
        {
            //TODO lots of optimization to be done here in terms of the compiler code
            // variable 'x' should leave its value at the top of the stack

            cVarPartNode *last = mList->back();

            if (last->GetDecl()->IsConst())
            {
                // consts leave 'x' value on top of stack
                cConstDeclNode *val=dynamic_cast<cConstDeclNode *>(last->GetDecl());
                EmitInt(PUSH_OP);
                EmitInt(val->GetValue());
            }
            else if (last->IsArrayRef())
            {
                int deref_size;
                if(last->GetDecl()->GetBaseType()->IsPointer())
                    deref_size = last->GetDecl()->GetBaseType()->GetPointsTo()->Size();
                else
                    deref_size = last->GetDecl()->GetBaseType()->Size();
                 
                // array refs leave *x at top of stack 
                EmitOffset();
                
                if(deref_size == 1)
                    EmitInt(PUSHCVARIND_OP);
                else
                    EmitInt(PUSHVARIND_OP);
            } 
            else if (last->IsArray())
            {
                // arrays leave &x on top of stack
                // push (mOffset + FP)
                
                EmitInt(PUSH_OP);
                EmitInt(mOffset);
                EmitInt(PUSHFP_OP);
                EmitInt(PLUS_OP);
            }
            else if (mList->front()->IsGlobal())
            {
                EmitInt(PUSH_OP);
                EmitGlobalRef(mBaseName);
                EmitInt(PUSH_OP);
                EmitInt(mOffset);
                EmitInt(PLUS_OP);
                EmitInt(PUSHVARIND_OP);
            }
            else 
            {
                // non-arrays leave 'x' on top of stack
                EmitInt(PUSHVAR_OP);
                EmitInt(mOffset);
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
            else if (mList->front()->IsGlobal())
            {
                EmitInt(PUSH_OP);
                EmitGlobalRef(mBaseName);
                EmitInt(PUSH_OP);
                EmitInt(mOffset);
                EmitInt(PLUS_OP);
                EmitInt(POPVARIND_OP);
            }
            else 
            {
                EmitInt(POPVAR_OP);
                EmitInt(mOffset);
            }
        }

    protected:
        std::string mBaseName;
        std::string mName;
        list<cVarPartNode *> *mList;
        cDeclNode *mDepthDecl;
        int mOffset;
        int mSize;
};

