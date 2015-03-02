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
    cFuncDeclNode(cSymbol *type, cSymbol *id)
    {
        cSymbol *existingSymbol = symbolTableRoot->LocalLookup(id->Name());
        if (existingSymbol != NULL && existingSymbol->GetType()->IsFunc())
        {
            // we already have a declaration, so make this one match
            cFuncDeclNode *fDecl = 
                dynamic_cast<cFuncDeclNode *>(existingSymbol->GetType());
            mId = existingSymbol;
            mReturnType = fDecl->mReturnType;
            mParams = fDecl->mParams;
            mDecls = fDecl->mDecls;
            mStmts = fDecl->mStmts;
            mParamsSet = fDecl->mParamsSet;
            mId->SetType(this);
        } else {
            symbolTableRoot->Insert(id);
            mReturnType = type;
            mId = id;
            mParams = NULL;
            mDecls = NULL;
            mStmts = NULL;
            mParamsSet = false;
            mId->SetType(this);
        }
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
    void SetBody(cDeclsNode *decls, cStmtsNode *stmts)
    {
        mDecls = decls;
        mStmts = stmts;
    }

    virtual int ComputeOffsets(int base)
    {
        // function decls start at offset 0 relative to function
        // functions take no space in outer scope, so leave base alone
        int offset = 0;
        assert(mParams == NULL);
        if (mParams != NULL) offset = mParams->ComputeOffsets(offset);
        if (mDecls != NULL) offset = mDecls->ComputeOffsets(offset);
        if (mStmts != NULL) offset = mStmts->ComputeOffsets(offset);
        if (mDecls != NULL) mDeclsSize = mDecls->Size();

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
        if (mDecls != NULL) result += "\n" + mDecls->toString();
        if (mStmts != NULL) result += "\n" + mStmts->toString();
        if (mDeclsSize != 0) result += "\nsize: " + std::to_string(mDeclsSize);
        result += "\n)";
        return result;
    }

    virtual void GenerateCode()
    {
        SetJumpDest(mId->Name());

        if (mDeclsSize != 0)
        {
            EmitInt(ADJSP_OP);
            EmitInt(mDeclsSize);
        }

        if (mStmts != NULL)
        {
            mStmts->GenerateCode();
            cReturnNode *ret = new cReturnNode(new cIntExprNode(0));
            ret->GenerateCode();
        }
    }
  protected:
    cSymbol *mReturnType;       // return type of function
    cParamsSpecNode *mParams;   // formal parameter list
    cDeclsNode *mDecls;         // local variables
    cStmtsNode *mStmts;         // executable statements
    bool mParamsSet;            // indicates parameters have been set
    int mDeclsSize;             // size of the declarations. Needed to
                                // allocate space on the stack for locals
};

