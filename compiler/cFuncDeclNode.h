#pragma once
//*******************************************************
// Purpose: Class for a function definiiton or delcaration
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>

#include "cAstNode.h"
#include "cDeclNode.h"
#include "cExprNode.h"
#include "cParamsSpecNode.h"
#include "codegen.h"

class cFuncDeclNode : public cDeclNode
{
  public:
    // return type and name of function
    cFuncDeclNode(cSymbol *type, cSymbol *id) : cDeclNode()
    {
        mDeclsSize = 0;
        /*
        cSymbol *existingSymbol = symbolTableRoot->LocalLookup(id->Name());
        if (existingSymbol != NULL && existingSymbol->GetType()->IsFunc())
        {
            // we already have a declaration, so make this one match
            // FIX THIS: should do semantic checks instead of blindly
            // accepting the old def
            cFuncDeclNode *fDecl = 
                dynamic_cast<cFuncDeclNode *>(existingSymbol->GetType());
            mId = existingSymbol;
            mReturnType = fDecl->mReturnType;
            mParams = fDecl->mParams;
            mStmts = fDecl->mStmts;
            mParamsSet = fDecl->mParamsSet;
            mId->SetType(this);
        } 
        else 
        */
        {
            symbolTableRoot->Insert(id);
            mReturnType = type;
            mId = id;
            mParams = NULL;
            mHasStmts = false;
            mStmts = NULL;
            mParamsSet = false;
            mId->SetDecl(this);
        }
    }

    virtual cDeclNode *GetType()
    {
        return mReturnType->GetType();
    }

    // return the type of the return value
    virtual cDeclNode *GetBaseType()
    {
        return mReturnType->GetType();
    }

    virtual bool IsFunc()   { return true; }

    // size of the returned value
    virtual int ReturnSize(){ return mReturnType->GetType()->Size(); }

    // functions take no space
    virtual int Size()      { return 0; }

    // set the params. This can't be done in the constructor because
    // we create the node on the function header
    void SetParams(cParamsSpecNode *params)
    {
        if (!mParamsSet)
        {
            mParams = params;
            mParamsSet = true;
        }
    }

    // return the parameter list
    cParamsSpecNode *GetParams()
    { return mParams; }

    // set the local var delcs and the statments
    void SetBody(cStmtsNode *stmts)
    {
        mHasStmts = true;
        mStmts = stmts;
    }

    virtual int ComputeOffsets(int base)
    {
        // function decls start at offset 0 relative to function
        // functions take no space in outer scope, so leave base alone
        int offset = 0;
        if (mParams != NULL) mParams->ComputeOffsets(-STACK_FRAME_SIZE);
        if (mStmts != NULL) offset = mStmts->ComputeOffsets(offset);
        mDeclsSize = offset; 

        return base;
    }

    virtual std::string toString()
    {
        std::string result("(FUNC: ");
        result += mId->toString();
        if (mParams != NULL) 
            result += mParams->toString();
        else
            result += "()";
        if (mStmts != NULL) result += "\n" + mStmts->toString();
        if (mDeclsSize != 0) result += "\nsize: " + std::to_string(mDeclsSize);
        result += "\n)";
        return result;
    }

    virtual void GenerateCode()
    {
        // if no statements, this is a prototype and we don't gen code
        if (!mHasStmts) return;

        EmitComment(mId->Name() + "\n");
        SetLabelValue(mId->Name());
        int adj_size = (mDeclsSize / WORD_SIZE * WORD_SIZE) + WORD_SIZE;
        if (mDeclsSize != 0)
        {
            
            EmitInt(ADJSP_OP);
            EmitInt(adj_size);
        }

        if (mStmts != NULL) mStmts->GenerateCode();

        cReturnNode *ret = new cReturnNode(new cIntExprNode(0));
        ret->GenerateCode();
    }
  protected:
    cSymbol *mReturnType;       // return type of function
    cParamsSpecNode *mParams;   // formal parameter list
    bool mHasStmts;             // true = function definition false=decl
    cStmtsNode *mStmts;         // executable statements
    bool mParamsSet;            // indicates parameters have been set
    int mDeclsSize;             // size of the declarations. Needed to
                                // allocate space on the stack for locals
};

