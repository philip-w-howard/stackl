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
                // set name of this variable reference to the name
                // of the variable it references
                mBaseName = var->Name();
                mName = var->Name();

                // create the list of varparts that make up the varref
                mList = new list<cVarPartNode *>();

                // add the first varpart to the varref
                mList->push_back(var);

                // get the type of the first variable
                baseDecl = var->GetType();
                
                // if the type of the first varpart is an array
                // and we're referencing part of it, get the type of
                // the element
                if (baseDecl->IsArray() && var->IsArrayRef())
                    baseDecl = baseDecl->GetArrayElementType();
                
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
                    mDepthDecl = sField->GetDecl();
                    field->SetField(mDepthDecl);
                    if (mDepthDecl->IsArray() && field->IsArrayRef())
                        mDepthDecl = mDepthDecl->GetArrayElementType();
                    mList->push_back(field);
                    field->UpdateSymbol(sField);
                    field->SetDecl(base);

                    mName += "." + field->Name();
                }
            }
        }

        virtual cDeclNode *GetType()
        {
            return mDepthDecl->GetType();
        }

        int GetOffset() { return mOffset; }

        std::string Name() { return mName; }

        virtual bool IsArrayRef()
        {
            return mList->back()->IsArrayRef();
        }
        virtual bool IsGlobal()
        {
            return mList->front()->IsGlobal();
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
                mSize = mDepthDecl->GetType()->Size();
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

                if(last->GetType()->IsPointer())
                {
                    // ptr[index] pushes (ptr + (size * index))
                    int deref_size = last->GetType()->GetPointsTo()->Size(); 

                    if (mList->front()->IsGlobal())
                    {
                        EmitInt(PUSH_OP);
                        EmitGlobalRef(mBaseName);
                        EmitInt(PUSH_OP);
                        EmitInt(mOffset);
                        EmitInt(PLUS_OP);
                        EmitInt(PUSHVARIND_OP);
                    } else {
                        EmitInt(PUSHVAR_OP);
                        EmitInt(mOffset);
                    }

                    EmitInt(PUSH_OP);
                    EmitInt(deref_size);
                    index->GetIndex(0)->GenerateCode();
                    EmitInt(TIMES_OP);

                    EmitInt(PLUS_OP);
                }
                else if(last->GetType()->IsArray())
                {
                    // arr[index] pushes ((FP + offset) + (size * index))
                    int deref_size = last->GetType()->GetArrayElementType()->Size();

                    if (mList->front()->IsGlobal())
                    {
                        EmitInt(PUSH_OP);
                        EmitGlobalRef(mBaseName);
                        EmitInt(PUSH_OP);
                        EmitInt(mOffset);
                    } else {
                        EmitInt(PUSH_OP);
                        EmitInt(mOffset);
                        EmitInt(PUSHFP_OP);
                        EmitInt(PLUS_OP);
                    }
                    EmitInt(PUSH_OP);
                    EmitInt(deref_size);
                    index->GetIndex(0)->GenerateCode();
                    EmitInt(TIMES_OP);

                    EmitInt(PLUS_OP);
                }
            }
            else if(last->GetType()->IsArray())
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
                if(last->GetType()->IsPointer())
                {
                    //std::cout << last->Name() << "[] is a ptr of size " << last->GetType()->GetPointsTo()->Size()  << "...\n";
                    deref_size = last->GetType()->GetPointsTo()->Size();
                }
                else if(last->GetType()->IsArray())
                {
                    //std::cout << last->Name() << "[] is an array ref of size " << last->GetType()->Size()  << "...\n";
                    deref_size = last->GetType()->GetArrayElementType()->Size();
                }
                else
                    std::cout << last->Name() << "[] is an unknown array ref of size " << last->GetType()->Size()  << "...\n";

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

                if (mList->front()->IsGlobal())
                {
                    EmitInt(PUSH_OP);
                    EmitGlobalRef(mBaseName);
                    EmitInt(PUSH_OP);
                    EmitInt(mOffset);
                    EmitInt(PLUS_OP);
                }
                else
                {
                    //std::cout << mDepthDecl->Name() << " is an array on right side...\n";
                    EmitInt(PUSH_OP);
                    EmitInt(mOffset);
                    EmitInt(PUSHFP_OP);
                    EmitInt(PLUS_OP);
                }
            }
            else 
            {
                // non-arrays leave 'x' on top of stack
                int size;
                size = last->GetType()->Size();

                if (mList->front()->IsGlobal())
                {
                    EmitInt(PUSH_OP);
                    EmitGlobalRef(mBaseName);
                    EmitInt(PUSH_OP);
                    EmitInt(mOffset);
                    EmitInt(PLUS_OP);
                    if(size == 1)
                        EmitInt(PUSHCVARIND_OP);
                    else
                        EmitInt(PUSHVARIND_OP);
                }
                else
                {
                    if(size == 1)
                        EmitInt(PUSHCVAR_OP);
                    else
                        EmitInt(PUSHVAR_OP);
                    EmitInt(mOffset);
                }
            }
        }

        virtual void GenerateLVal()
        {
            // places value at top of stack into *x
            cVarPartNode *last = mList->back();

            if (last->IsArrayRef() || last->GetDecl()->IsArray())
            {
                int deref_size;
                if(last->GetType()->IsPointer())
                    deref_size = last->GetType()->GetPointsTo()->Size(); 
                else if(last->GetType()->IsArray())
                    deref_size = last->GetType()->GetArrayElementType()->Size();

                //std::cout << last->Name() << " is an array ref, refing " << last->GetType()->Name()  << " of size " << deref_size  << "...\n";
                EmitOffset();

                if(deref_size == 1)
                    EmitInt(POPCVARIND_OP);
                else
                    EmitInt(POPVARIND_OP);
            }
            else 
            {
                //std::cout << last->Name() << " is not an array ref or array...\n";
                int size;
                size = last->GetType()->Size();

                if (mList->front()->IsGlobal())
                {
                    EmitInt(PUSH_OP);
                    EmitGlobalRef(mBaseName);
                    EmitInt(PUSH_OP);
                    EmitInt(mOffset);
                    EmitInt(PLUS_OP);

                    if(size == 1)
                        EmitInt(POPCVARIND_OP);
                    else
                        EmitInt(POPVARIND_OP);
                }
                else
                {
                    if(size == 1)
                        EmitInt(POPCVAR_OP);
                    else
                        EmitInt(POPVAR_OP);

                    EmitInt(mOffset);
                }
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

