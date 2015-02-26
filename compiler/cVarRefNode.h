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
        } else {
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
        } else {
            cStructDeclNode *base = dynamic_cast<cStructDeclNode *>(mDepthDecl);
            cSymbol *sField = base->Lookup(field->Name());
            if (sField == NULL)
            {
                semantic_error(field->Name() + " is not a field of " + mName);
                mSemanticError = true;
            } else {
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

    std::string Name() { return mName; }

    virtual int ComputeOffsets(int base)
    {
        list<cVarPartNode *>::iterator it = mList->begin();
        mOffset = (*it)->GetDecl()->GetOffset();

        for ( it++; it != mList->end(); it++)
        {
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

    virtual void GenerateCode()
    {
        if (mDepthDecl->IsFloat())
            EmitString("(*(double *)&");
        else if (mDepthDecl->IsInt() && mSize == 4) 
            EmitString("(*(int *)&");
        else
            EmitString("(");
        EmitString("Memory[Frame_Pointer + ");
        EmitInt(mOffset);
        EmitString("])");
    }

  protected:
    std::string mName;
    list<cVarPartNode *> *mList;
    cDeclNode *mDepthDecl;
    int mOffset;
    int mSize;
};

